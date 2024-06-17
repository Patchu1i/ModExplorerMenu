#include "Console.h"
#include "Menu.h"

namespace ModExplorerMenu
{
	std::mutex m_Process;
	std::mutex m_Task;

	inline bool IsPlayerLoaded()
	{
		return RE::PlayerCharacter::GetSingleton()->Is3DLoaded();
	}

	// Get the FormID from a command string by parsing the second token.
	// @param cmd: Command string (e.g. prid ff0082fb).
	// TODO: Add exception if FormID not found in token.
	unsigned long GetFormFromCMD(std::string cmd)
	{
		std::istringstream iss(cmd);
		std::string token;
		std::getline(iss, token, ' ');
		std::getline(iss, token, ' ');
		return std::stoul(token, nullptr, 16);
	}

	// Add NPC FormID to history vector and set.
	// @param a_ref: Reference FormID of the NPC.
	void AddNPCToHistory(RE::FormID a_ref)
	{
		Console::npcPlaceHistoryVector.push_back(a_ref);
		Console::npcPlaceHistorySet.insert(a_ref);
	}

	// Check if NPC FormID is in history set.
	// @param a_ref: Reference FormID of the NPC.
	bool IsNPCInHistory(RE::FormID a_ref)
	{
		return Console::npcPlaceHistorySet.find(a_ref) != Console::npcPlaceHistorySet.end();
	}

	// Get the last NPC FormID from the history vector.
	// @return FormID of the last NPC.
	RE::FormID GetLastNPCReference()
	{
		return Console::npcPlaceHistoryVector.back();
	}

	// Create, compile, and run a console command (script).
	// @param cmd: Command to be executed.
	// @note This function is called from the main thread.
	inline void Console::SendConsoleCommand(std::string cmd)
	{
		auto references = std::make_shared<std::vector<RE::FormID>>();
		// Intercept <prid_last> command for override.
		if (cmd == "<prid_last>") {
			if (commandHistory.empty()) {
				stl::report_and_error("No command history found for <prid_last>.");
			}

			// Grab FormID from last command.
			auto lastCommand = commandHistory.top();
			auto lastID = RE::FormID(GetFormFromCMD(lastCommand));

			// Iterate over nearby object references for match.
			SKSE::GetTaskInterface()->AddTask([references, lastID]() {
				auto process = RE::ProcessLists::GetSingleton();

				for (auto& handle : process->highActorHandles) {
					if (!handle.get() || !handle.get().get()) {
						continue;
					}

					auto actor = handle.get().get();
					auto base = actor->GetBaseObject()->GetFormID();
					auto ref = actor->GetFormID();

					if (base == lastID) {
						references->push_back(ref);
					}
				}

				// Tell process thread that processing is open.
				isLocked.store(false);

				if (references->empty()) {
					//stl::report_and_error("No matching references found using <prid_last>");
				} else {
					for (auto ref : *references) {
						if (!IsNPCInHistory(ref)) {
							AddNPCToHistory(ref);
						}
					}

					std::string cmd = "prid " + std::format("{:08x}", GetLastNPCReference());
					AddToFront(cmd, 0ms);
					StartProcessThread(true);
				}
			});

			return;  // Early out.
		}

		const auto scriptFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>();
		const auto script = scriptFactory ? scriptFactory->Create() : nullptr;

		if (script) {
			auto queue = RE::UIMessageQueue::GetSingleton();
			const auto consoleRef = RE::Console::GetSelectedRef();

			if (queue) {
				queue->AddMessage(RE::BSFixedString("Console"), RE::UI_MESSAGE_TYPE::kShow, nullptr);
				script->SetCommand(cmd);
				script->CompileAndRun(consoleRef.get());
				queue->AddMessage(RE::BSFixedString("Console"), RE::UI_MESSAGE_TYPE::kHide, nullptr);
			} else {
				stl::report_and_fail("Failed to get UIMessageQueue singleton");
			}
			delete script;
		} else {
			stl::report_and_fail("Failed to create script using scriptFactory.");
		}

		commandHistory.push(cmd);
	}

	// Called from main thread Present function.
	// This executes functions in the task queue (FIFO).
	void Console::ProcessMainThreadTasks()
	{
		std::lock_guard<std::mutex> lock(m_Task);
		if (!taskQueue.empty()) {
			auto task = taskQueue.front();
			if (task) {
				task();
			}
			taskQueue.pop();
		}
	}

	// Using a separate thread to artificially delay commands such as "prid".
	// This is to ensure that the command is executed after the target reference is loaded.
	// We can send <lock> as a command to halt the process from continuing to process further
	// instructions in the commandQueue. Only once the <prid_last> command is executed, does
	// the process thread "unlock" and continue processing the commandQueue.
	// (Note) Only one instance of this thread will run at a single time.
	// @param unlock: Unlock the process thread.
	void Console::StartProcessThread(bool unlock)
	{
		if (isProcessing.load()) {
			return;
		}

		isProcessing.store(true);
		processThread = std::thread([unlock]() {
			while (true) {
				auto cmd = std::shared_ptr<std::pair<std::string, std::chrono::milliseconds>>();
				{
					// Lock mutex for commandQueue access.
					std::lock_guard<std::mutex> lock(m_Process);

					// If commandQueue is empty, break out of loop and open processor.
					if (Console::commandQueue.empty()) {
						isProcessing.store(false);
						break;
					}

					cmd = std::make_shared<std::pair<std::string, std::chrono::milliseconds>>(Console::commandQueue.front());

					// Here we interrupt the process if we hit a <lock>.
					// Expected behavior is to wait for <prid_last> to unlock.
					// From the SKSETaskInterface call.
					if (cmd->first == "<lock>") {
						isProcessing.store(false);

						if (unlock) {
							Console::commandQueue.pop_front();
						}

						continue;
					}

					Console::commandQueue.pop_front();
				}

				// Send command as function to taskQueue on main thread.
				std::lock_guard<std::mutex> lock(m_Task);
				taskQueue.push([cmd]() {
					Console::SendConsoleCommand(cmd->first);
				});
			}
		});

		processThread.detach();
	}

	// Add a console command to the command queue to be processed.
	// @note Passing <lock> as a command will halt the process thread.
	// @note Ensure <prid_last> is executed before <lock>.
	// @param a_cmd: Command to be added to the queue.
	// @param a_delay: Delay in seconds before executing the command.
	void Console::AddToQueue(std::string a_cmd, std::chrono::milliseconds a_delay = 0ms)
	{
		std::lock_guard<std::mutex> lock(m_Process);
		commandQueue.push_back(std::make_pair(a_cmd, a_delay));
	}

	// Add a console command to the front of the command queue.
	// @note see `Console::AddToQueue()` for more information.
	// @param a_cmd: Command to be added to the queue.
	// @param a_delay: Delay in seconds before executing the command.
	void Console::AddToFront(std::string a_cmd, std::chrono::milliseconds a_delay)
	{
		std::lock_guard<std::mutex> lock(m_Process);
		commandQueue.push_front(std::make_pair(a_cmd, a_delay));
	}

	// Add an item to the player's inventory
	// @param a_itemFormID: Base Form ID of the item.
	// @param a_count: Count of the item.
	void Console::AddItem(std::string a_itemFormID, int a_count)
	{
		if (IsPlayerLoaded()) [[likely]] {
			AddToQueue("player.additem " + a_itemFormID + " " + std::to_string(a_count));
		}
	}

	// Place a item at the player's location.
	// @param std::string a_itemFormID: Base Form ID of the item.
	// @param a_count: Count of objects
	void Console::PlaceAtMe(std::string a_itemFormID, int a_count)
	{
		if (IsPlayerLoaded()) [[likely]] {
			AddToQueue("player.placeatme " + a_itemFormID + " " + std::to_string(a_count));
		}
	}

	// Place an NPC at the player's location by its base FormID.
	// @param a_npcBaseID: Base Form ID of the NPC.
	// @param a_count: Count of objects
	void Console::PlaceAtMeFormID(RE::FormID a_npcBaseID, int a_count)
	{
		if (IsPlayerLoaded()) [[likely]] {
			AddToQueue("player.placeatme " + std::format("{:08x}", a_npcBaseID) + " " + std::to_string(a_count));
		}
	}

	// Fetch the last spawned REFR ID and set it as the console target reference.
	void Console::PridLast()
	{
		if (IsPlayerLoaded()) [[likely]] {
			AddToQueue("<prid_last>");
			AddToQueue("<lock>");
		}
	}

	// Move player to target reference id.
	// @param a_targetRefID: Reference FormID of the target.
	void Console::MoveToREFR(std::string a_targetRefID)
	{
		AddToQueue("player.moveto " + a_targetRefID);
	}

	// Move target reference id to player.
	// @param a_targetRefID: Reference FormID of the target.
	void Console::MoveREFRToPlayer(std::string a_targetRefID)
	{
		if (IsPlayerLoaded()) [[likely]] {
			AddToQueue("prid " + a_targetRefID);
			AddToQueue("moveto player");
		}
	}

	// Kill target reference id.
	// @param a_targetRefID: Reference FormID of the target.
	void Console::KillREFR(std::string a_targetRefID)
	{
		if (IsPlayerLoaded()) [[likely]] {
			AddToQueue("prid " + a_targetRefID);
			AddToQueue("kill");
		}
	}

	// Kill selected reference or player.
	// @note Remember to call `Console::PridLast()`
	void Console::Kill()
	{
		if (IsPlayerLoaded()) [[likely]] {
			AddToQueue("kill");
		}
	}

	// Resurrect target reference id.
	// @param a_targetRefID: Reference FormID of the target.
	// @param a_param: Resurrect parameter <1, 2>
	// @note 1 = Resurect with items restored.
	// @note 0 = Remove corpse and create a fresh copy.
	void Console::ResurrectREFR(std::string a_targetRefID, int a_param)
	{
		if (IsPlayerLoaded()) [[likely]] {
			AddToQueue("prid " + a_targetRefID);
			AddToQueue("resurrect " + std::to_string(a_param));
		}
	}

	// Ressurect selected reference (or restore inventory).
	// @note Remember to call `Console::PridLast()`
	void Console::Resurrect(int a_param)
	{
		if (IsPlayerLoaded()) [[likely]] {
			AddToQueue("resurrect " + std::to_string(a_param));
		}
	}

	// Unequip all items from target reference id.
	// @param a_targetRefID: Reference FormID of the target.
	void Console::UnEquipREFR(std::string a_targetRefID)
	{
		if (IsPlayerLoaded()) [[likely]] {
			AddToQueue("prid " + a_targetRefID);
			AddToQueue("unequipall");
		}
	}

	// Unequip all equipped items for selected reference or player.
	// @note Remember to call `Console::PridLast()`
	void Console::UnEquip()
	{
		if (IsPlayerLoaded()) [[likely]] {
			AddToQueue("unequipall");
		}
	}

	// Toggle AI for target reference id.
	// @param a_targetRefID: Reference FormID of the target.
	void Console::FreezeREFR(std::string a_targetRefID)
	{
		if (IsPlayerLoaded()) [[likely]] {
			AddToQueue("prid " + a_targetRefID);
			AddToQueue("TAI");
		}
	}

	// Toggle AI for selected reference or player.
	// @note Remember to call `Console::PridLast()`
	void Console::Freeze()
	{
		if (IsPlayerLoaded()) [[likely]] {
			AddToQueue("TAI");
		}
	}

	// Teleport player to cell by EditorID
	// @param a_editorID: EditorID of the target cell.
	void Console::Teleport(std::string a_editorID)
	{
		if (IsPlayerLoaded()) [[likely]] {
			AddToQueue("coc " + a_editorID);
		}
	}

	// Callback definition for console command script.
	bool Console::Run(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData,
		RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* a_containingObj, RE::Script* a_scriptObj,
		RE::ScriptLocals* a_locals, double& a_result, std::uint32_t& a_opcodeOffsetPtr)
	{
		(void)a_opcodeOffsetPtr;
		(void)a_result;
		(void)a_locals;
		(void)a_scriptObj;
		(void)a_containingObj;
		(void)a_thisObj;
		(void)a_scriptData;

		// UIMessageQueue -> AddMessage -> kHide
		auto queue = RE::UIMessageQueue::GetSingleton();
		auto menu = Menu::GetSingleton();

		if (queue) {
			queue->AddMessage(RE::BSFixedString("Console"), RE::UI_MESSAGE_TYPE::kHide, nullptr);
			menu->SetEnabled(true);
		}

		return true;
	}

	// Register custom Console Command.
	void Console::Register()
	{
		auto info = RE::SCRIPT_FUNCTION::LocateConsoleCommand("Timing");  // unused
		if (info) {
			static RE::SCRIPT_PARAMETER params;

			info->functionName = LONG_NAME;
			info->shortName = SHORT_NAME;
			info->referenceFunction = false;
			info->executeFunction = Run;
			info->conditionFunction = nullptr;
		}
	}
}