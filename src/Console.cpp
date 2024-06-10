#include "Console.h"
#include "Menu.h"

// Define ConsoleCommand t_Execute callback function.
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

RE::NiPointer<RE::TESObjectREFR> GetConsoleRefr()
{
	REL::ID id;
	int testid;

	if (REL::Module::IsAE()) {
		constexpr REL::Version RUNTIME_SSE_1_6_1170(1, 6, 1170, 0);
		if (REL::Module::get().version().compare(RUNTIME_SSE_1_6_1170) != std::strong_ordering::less) {
			id = 21890;
			testid = 21890;
		} else {
			id = 405935;
			testid = 405935;
		}
	} else {
		id = 519394;
		testid = 519394;
	}

	logger::info("id before: {}", testid);
	id = 21890;
	logger::info("id after: {}", 21890);

	// Wtf, does this work?
	for (int i = 0; i < 600000; i++) {
		id = i;
		const REL::Relocation<RE::ObjectRefHandle*> selectedRef{ id };

		if (selectedRef->get() != nullptr) {
			logger::info("Found correct ID {}", i);
			return selectedRef->get();
		}
	}

	const REL::Relocation<RE::ObjectRefHandle*> selectedRef{ id };
	return selectedRef->get();
}

inline void ConsoleCommand::SendConsoleCommand(std::string_view cmd)
{
	const auto scriptFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>();
	const auto script = scriptFactory ? scriptFactory->Create() : nullptr;
	if (script) {
		const auto consoleRef = RE::Console::GetSelectedRef();
		script->SetCommand(cmd);
		script->CompileAndRun(consoleRef.get());
		delete script;
	}
}

// Continously process the `ConsoleCommand::taskQueue` in the main thread.
// This is called every frame using `DXGI::Present` hook.
void ConsoleCommand::ProcessMainThreadTasks()
{
	if (!taskQueue.empty()) {
		auto task = taskQueue.front();
		task();
		taskQueue.pop();
	}
}

std::mutex mtx;
// Start processing the `ConsoleCommand::commandQueue` in a separate thread.
// Only one instance of this thread will run at a single time.
void ConsoleCommand::StartProcessThread()
{
	if (isProcessing.load()) {
		return;
	}

	isProcessing.store(true);
	logger::info("isProcessing TRUE");
	processThread = std::thread([]() {
		while (true) {
			std::pair<std::string, int> cmd;
			{
				std::lock_guard<std::mutex> lock(mtx);  // Lock the mutex
				if (ConsoleCommand::commandQueue.empty()) {
					logger::info("isProcessing FALSE end of queue");
					isProcessing.store(false);
					break;
				}

				cmd = ConsoleCommand::commandQueue.front();
				ConsoleCommand::commandQueue.pop();
			}

			logger::info("Sent Command From Side Thread");

			std::this_thread::sleep_for(std::chrono::seconds(cmd.second));

			taskQueue.push([cmd]() {
				ConsoleCommand::SendConsoleCommand(cmd.first);
			});
		}
	});

	processThread.detach();
}

// Add an `std::string cmd` to the `ConsoleCommand::commandQueue` with an optional delay.
// @param a_cmd: Command to be added to the queue.
// @param a_delay: Delay in seconds before executing the command.
void ConsoleCommand::AddToQueue(std::string a_cmd, int a_delay = 0)
{
	commandQueue.push(std::make_pair(a_cmd, a_delay));
}

// Add (a_count) of item(s) with form id (a_itemFormID) to player.
// @param a_itemFormID: Base Form ID of the item.
// @param a_count: Count of the item.
void ConsoleCommand::AddItem(std::string a_itemFormID, int a_count)
{
	std::string cmd = "player.additem " + a_itemFormID + " " + std::to_string(a_count);
	AddToQueue(cmd);

	StartProcessThread();
}

// Place (a_count) of item(s) with form id (a_itemFormID) at player.
// @param a_itemFormID: Base Form ID of the item.
// @param a_count: Count of the item.
void ConsoleCommand::PlaceAtMe(std::string a_itemFormID, int a_count)
{
	std::string cmd = "player.placeatme " + a_itemFormID + " " + std::to_string(a_count);
	AddToQueue(cmd);

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
void ConsoleCommand::MoveToPlayer(std::string a_targetRefID)
{
	const int delay = 1;
	AddToQueue("prid " + a_targetRefID);
	AddToQueue("moveto player", delay);

	StartProcessThread();
}

// Kill target reference id.
// @param a_targetRefID: Reference FormID of the target.
void ConsoleCommand::Kill(std::string a_targetRefID)
{
	const int delay = 1;
	AddToQueue("prid " + a_targetRefID);
	AddToQueue("kill", delay);

	StartProcessThread();
}

// Resurrect target reference id.
// @param a_targetRefID: Reference FormID of the target.
void ConsoleCommand::Resurrect(std::string a_targetRefID)
{
	const int delay = 1;
	AddToQueue("prid " + a_targetRefID);
	AddToQueue("resurrect 1", delay);

	StartProcessThread();
}

void ConsoleCommand::UnEquipAll(std::string a_targetRefID)
{
	const int delay = 1;
	AddToQueue("prid " + a_targetRefID);
	AddToQueue("unequipall", delay);

	StartProcessThread();
}

void ConsoleCommand::ToggleFreeze(std::string a_targetRefID)
{
	const int delay = 1;
	AddToQueue("prid " + a_targetRefID);
	AddToQueue("TAI", delay);
	AddToQueue("TCAI");

	StartProcessThread();
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