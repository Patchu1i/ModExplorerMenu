#include "Console.h"
#include "Menu.h"

std::mutex mtx;

// Create, compile, and run a console command (script).
// @param cmd: Command to be executed.
// @note This function is called from the main thread.
inline void ConsoleCommand::SendConsoleCommand(std::string cmd)
{
	// Intercept <prid_last> command for override.
	if (cmd == "<prid_last>") {
		if (commandHistory.empty()) {
			throw std::runtime_error("No command history found");
		}

		// Grab FormID from last command.
		auto strlastCMD = commandHistory.top();
		std::istringstream iss(strlastCMD);
		std::getline(iss, strlastCMD, ' ');
		std::getline(iss, strlastCMD, ' ');
		logger::info("lastCMD: {}", strlastCMD);
		auto lastID = RE::FormID(std::stoul(strlastCMD, nullptr, 16));

		// Iterate over nearby object references for match.
		SKSE::GetTaskInterface()->AddTask([lastID]() {
			auto process = RE::ProcessLists::GetSingleton();
			auto references = std::vector<RE::FormID>();

			for (auto& handle : process->highActorHandles) {
				if (!handle.get() || !handle.get().get()) {
					continue;
				}

				auto actor = handle.get().get();
				auto base = actor->GetBaseObject()->GetFormID();
				auto ref = actor->GetFormID();

				if (base == lastID) {
					references.push_back(ref);
				}
			}

			// Tell process thread that processing is open.
			isLocked.store(false);

			if (references.empty()) {
				return;
			} else {
				std::sort(references.begin(), references.end());
				auto last = references.back();

				// std::sort to grab newest reference.
				AddToFront("prid " + std::format("{:08x}", last), 0ms);
				StartProcessThread(true);
			}
		});

		return;  // Early out.
	}

	const auto scriptFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>();
	const auto script = scriptFactory ? scriptFactory->Create() : nullptr;

	if (script) {
		const auto consoleRef = RE::Console::GetSelectedRef();
		script->SetCommand(cmd);
		script->CompileAndRun(consoleRef.get());
		logger::info("Script compiled {}", cmd);
		delete script;
	}

	commandHistory.push(cmd);
}

// Called from main thread Present function.
// This executes functions in the task queue (FIFO).
void ConsoleCommand::ProcessMainThreadTasks()
{
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
void ConsoleCommand::StartProcessThread(bool unlock = false)
{
	if (isProcessing.load()) {
		return;
	}

	isProcessing.store(true);
	processThread = std::thread([unlock]() {
		while (true) {
			std::pair<std::string, std::chrono::milliseconds> cmd;
			{
				// Lock mutex for commandQueue access.
				std::lock_guard<std::mutex> lock(mtx);

				// If commandQueue is empty, break out of loop and open processor.
				if (ConsoleCommand::commandQueue.empty()) {
					isProcessing.store(false);
					break;
				}

				// Pop FILO command from commandQueue.
				cmd = ConsoleCommand::commandQueue.front();

				// Here we interrupt the process if we hit a <lock>.
				// Expected behavior is to wait for <prid_last> to unlock.
				// From the SKSETaskInterface call.
				if (cmd.first == "<lock>") {
					isProcessing.store(false);

					if (unlock) {
						ConsoleCommand::commandQueue.pop_front();
					}
					continue;
				}

				ConsoleCommand::commandQueue.pop_front();
			}

			// Optionally delay that may no longer be required.
			std::this_thread::sleep_for(std::chrono::milliseconds(cmd.second));

			// Send command as function to taskQueue on main thread.
			taskQueue.push([cmd]() {
				ConsoleCommand::SendConsoleCommand(cmd.first);
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
void ConsoleCommand::AddToQueue(std::string a_cmd, std::chrono::milliseconds a_delay = 0ms)
{
	commandQueue.push_back(std::make_pair(a_cmd, a_delay));
}

// Add a console command to the front of the command queue.
// @note see `ConsoleCommand::AddToQueue()` for more information.
// @param a_cmd: Command to be added to the queue.
// @param a_delay: Delay in seconds before executing the command.
void ConsoleCommand::AddToFront(std::string a_cmd, std::chrono::milliseconds a_delay)
{
	commandQueue.push_front(std::make_pair(a_cmd, a_delay));
}

// Add an item to the player's inventory
// @param a_itemFormID: Base Form ID of the item.
// @param a_count: Count of the item.
void ConsoleCommand::AddItem(std::string a_itemFormID, int a_count)
{
	std::string cmd = "player.additem " + a_itemFormID + " " + std::to_string(a_count);
	AddToQueue(cmd, 0ms);
	StartProcessThread();
}

// Place a item at the player's location.
// @param a_itemFormID: Base Form ID of the item.
// @param a_count: Count of the item.
void ConsoleCommand::PlaceAtMe(std::string a_itemFormID, int a_count)
{
	std::string cmd = "player.placeatme " + a_itemFormID + " " + std::to_string(a_count);
	AddToQueue(cmd);
	StartProcessThread();
}

// Place an NPC at the player's location by its base FormID.
// @param a_npcBaseID: Base Form ID of the NPC.
// @param a_count: Count of the NPC.
void ConsoleCommand::PlaceAtMeNPC(RE::FormID a_npcBaseID)
{
	auto s = std::format("{:08x}", a_npcBaseID);
	AddToQueue("player.placeatme " + s + " 1");
	StartProcessThread();
}

// Fetch the last reference ID and set it as the console target reference.
void ConsoleCommand::PridLast()
{
	AddToQueue("<prid_last>");
	AddToQueue("<lock>");
	StartProcessThread();
}

// Move player to target reference id.
// @param a_targetRefID: Reference FormID of the target.
void ConsoleCommand::MoveTo(std::string a_targetRefID)
{
	AddToQueue("player.moveto " + a_targetRefID);
	StartProcessThread();
}

// Move target reference id to player.
// @param a_targetRefID: Reference FormID of the target.
// @param prid: Use prid command to set target reference.
void ConsoleCommand::MoveToPlayer(std::string a_targetRefID, bool prid)
{
	if (prid) {
		auto const delay = 100ms;
		AddToQueue("prid " + a_targetRefID);
		AddToQueue("moveto player", delay);
	} else {
		AddToQueue("moveto player " + a_targetRefID);
	}

	StartProcessThread();
}

// Kill target reference id.
// @param a_targetRefID: Reference FormID of the target.
// @param prid: Use prid command to set target reference.
void ConsoleCommand::Kill(std::string a_targetRefID, bool prid)
{
	if (prid) {
		auto const delay = 100ms;
		AddToQueue("prid " + a_targetRefID);
		AddToQueue("kill", delay);
	} else {
		AddToQueue("kill");
	}

	StartProcessThread();
}

// Resurrect target reference id.
// @param a_targetRefID: Reference FormID of the target.
// @param prid: Use prid command to set target reference.
void ConsoleCommand::Resurrect(std::string a_targetRefID, bool prid)
{
	if (prid) {
		auto const delay = 100ms;
		AddToQueue("prid " + a_targetRefID);
		AddToQueue("resurrect 1", delay);
	} else {
		AddToQueue("resurrect 1");
	}

	StartProcessThread();
}

// Unequip all items from target reference id.
// @param a_targetRefID: Reference FormID of the target.
// @param prid: Use prid command to set target reference.
void ConsoleCommand::UnEquipAll(std::string a_targetRefID, bool prid)
{
	if (prid) {
		auto const delay = 100ms;
		AddToQueue("prid " + a_targetRefID);
		AddToQueue("unequipall", delay);
	} else {
		AddToQueue("unequipall");
	}

	StartProcessThread();
}

// Toggle AI for target reference id.
// @param a_targetRefID: Reference FormID of the target.
// @param prid: Use prid command to set target reference.
void ConsoleCommand::ToggleFreeze(std::string a_targetRefID, bool prid)
{
	if (prid) {
		auto const delay = 100ms;
		AddToQueue("prid " + a_targetRefID);
		AddToQueue("TAI", delay);
	} else {
		AddToQueue("TAI");
	}

	StartProcessThread();
}

// Callback definition for console command script.
bool ConsoleCommand::Run(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData,
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
void ConsoleCommand::Register()
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