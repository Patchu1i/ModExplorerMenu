#include "include/C/Console.h"
#include "include/M/Menu.h"

namespace Modex
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

	std::string GetCellIDFromCMD(std::string cmd)
	{
		std::istringstream iss(cmd);
		std::string token;
		std::getline(iss, token, ' ');
		std::getline(iss, token, ' ');
		return token;
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

	void Console::SendQuickCommand(std::string cmd)
	{
		const auto scriptFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>();
		const auto script = scriptFactory ? scriptFactory->Create() : nullptr;

		if (script) {
			script->SetCommand(cmd);
			script->CompileAndRun(nullptr);
			delete script;
		} else {
			stl::report_and_fail("Failed to create script using scriptFactory.");
		}

		commandHistory.push(cmd);
	}

	int InventoryBoundObjects(const RE::TESForm* a_form, RE::TESBoundObject*& out_object, RE::ExtraDataList* out_extra)
	{
		auto* thePlayer = RE::PlayerCharacter::GetSingleton();
		RE::TESBoundObject* foundObject = nullptr;
		std::vector<RE::ExtraDataList*> extraDataCopy;
		RE::FormType a_type = a_form->GetFormType();

		std::map<RE::TESBoundObject*, std::pair<int, std::unique_ptr<RE::InventoryEntryData>>> candidates =
			thePlayer->GetInventory([a_type](const RE::TESBoundObject& a_object) { return a_object.Is(a_type); });

		auto count = 0;
		for (const auto& [item, inventoryData] : candidates) {
			const auto& [num_items, entry] = inventoryData;
			if (entry->object->formID == a_form->formID) {
				foundObject = item;
				count = num_items;
				auto simpleList = entry->extraLists;

				if (simpleList) {
					for (auto* extraData : *simpleList) {
						extraDataCopy.push_back(extraData);
					}
				}
				break;
			}
		}

		if (!foundObject) {
			return 0;
		}

		if (extraDataCopy.size() > 0) {
			out_extra = extraDataCopy.back();
		}
		out_object = foundObject;
		return count;
	}

	// Create, compile, and run a console command (script).
	// @param cmd: Command to be executed.
	// @note This function is called from the main thread.
	inline void Console::SendConsoleCommand(std::string cmd)
	{
		// Intercept <coc> command for override.
		if (cmd.find("coc") != std::string::npos) {
			std::string targetCell = GetCellIDFromCMD(cmd);
			SKSE::GetTaskInterface()->AddTask([targetCell]() {
				auto* player = RE::PlayerCharacter::GetSingleton();

				if (player) {
					player->CenterOnCell(targetCell.c_str());
				}
			});

			commandHistory.push(cmd);
			return;
		}

		if (cmd == "<read_last>") {
			if (commandHistory.empty()) {
				stl::report_and_error("No command history found for <read_last>.");
			}

			auto lastCommand = commandHistory.top();
			auto lastID = RE::FormID(GetFormFromCMD(lastCommand));

			SKSE::GetTaskInterface()->AddTask([lastID]() {
				// auto* player = RE::PlayerCharacter::GetSingleton();
				RE::TESForm* form = RE::TESForm::LookupByID(lastID);

				if (form == nullptr) {
					return;
				}

				RE::TESObjectBOOK* book = form->As<RE::TESObjectBOOK>();

				if (book == nullptr) {
					return;
				}

				RE::NiPoint3 defaultPos{};
				RE::BSString buf;
				book->GetDescription(buf, nullptr);

				// The commented code below simply performs the Read action on a book.
				// causing any relative skill or spell to be learned, and the book flagged
				// as read. It does nothing to open the book itself.

				// if (form) {
				// 	if (auto book = form->As<RE::TESObjectBOOK>()) {
				// 		book->Read(player);
				//	 }
				// }

				RE::TESBoundObject* equipObject = nullptr;
				RE::ExtraDataList* extraData = nullptr;
				InventoryBoundObjects(form, equipObject, extraData);

				RE::TESObjectREFR* bookRef = equipObject->As<RE::TESObjectREFR>();

				if (bookRef != nullptr) {
					RE::BookMenu::OpenBookMenu(buf, extraData, bookRef, book, defaultPos, defaultPos, 1.0f, true);
				}
			});

			return;
		}

		// intercept <equip_last> command for override.
		if (cmd == "<equip_last>") {
			if (commandHistory.empty()) {
				stl::report_and_error("No command history found for <equip_last>.");
			}

			// Grab FormID from last command.
			auto lastCommand = commandHistory.top();
			auto lastID = RE::FormID(GetFormFromCMD(lastCommand));

			// Equip the last FormID.
			SKSE::GetTaskInterface()->AddTask([lastID]() {
				logger::info("Running Task");
				auto* player = RE::PlayerCharacter::GetSingleton();
				RE::TESForm* form = RE::TESForm::LookupByID(lastID);

				RE::BGSEquipSlot* equipSlot = nullptr;
				RE::TESBoundObject* equipObject = nullptr;
				RE::ExtraDataList* extraData = nullptr;

				InventoryBoundObjects(form, equipObject, extraData);

				if (RE::TESObjectWEAP* weapon = form->As<RE::TESObjectWEAP>()) {
					equipSlot = weapon->GetEquipSlot();
				} else if (RE::TESObjectARMO* armor = form->As<RE::TESObjectARMO>()) {
					logger::info("Setting Armor Up");
					equipSlot = armor->GetEquipSlot();
				}

				if (player) {
					logger::info("Equipping Object");
					RE::ActorEquipManager::GetSingleton()->EquipObject(player, equipObject, extraData, 1, equipSlot);
				}
			});

			return;  // Early out.
		}

		// Intercept <prid_last> command for override.
		auto references = std::make_shared<std::vector<RE::FormID>>();
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
			//auto* playerREF = RE::PlayerCharacter::GetSingleton()->AsReference();

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

	// Clear's all items the player is equipping
	void Console::ClearEquipped()
	{
		if (IsPlayerLoaded()) [[likely]] {
			AddToQueue("player.unequipall");
		}

		Console::StartProcessThread();
	}

	void Console::ClearInventory()
	{
		if (IsPlayerLoaded()) [[likely]] {
			AddToQueue("player.removeallitems");
		}

		Console::StartProcessThread();
	}

	// Add an item to the player's inventory
	// @param a_itemFormID: Base Form ID of the item.
	// @param a_count: Count of the item.
	// @param a_equip: Equip the item after adding.
	void Console::AddItemEx(RE::FormID a_itemFormID, int a_count, bool a_equip)
	{
		if (IsPlayerLoaded()) [[likely]] {
			AddToQueue("player.additem " + std::format("{:08x}", a_itemFormID) + " " + std::to_string(a_count));

			if (a_equip) {
				AddToQueue("<equip_last>");
			}
		}
	}

	// Add a book, and open it.
	void Console::ReadBook(std::string a_formid)
	{
		if (IsPlayerLoaded()) [[likely]] {
			AddToQueue("player.additem " + a_formid + " 1");

			AddToQueue("<read_last>");
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
			AddToQueue("moveto player " + a_targetRefID);
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
		AddToQueue("coc " + a_editorID);
	}
}