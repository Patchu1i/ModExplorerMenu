#	include "Console.h"
#	include "Menu.h"

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
	//auto* playerREF = RE::PlayerCharacter::GetSingleton()->AsReference();

	const auto scriptFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>();
	const auto script = scriptFactory ? scriptFactory->Create() : nullptr;
	if (script) {
		logger::info("Sending console command: {}", cmd);
		//const auto consoleRef = GetConsoleRefr().get();
		const auto consoleRef = RE::Console::GetSelectedRef();
		script->SetCommand(cmd);
		script->CompileAndRun(consoleRef.get());
		logger::info("deleting script");
		delete script;
	}
}

void ConsoleCommand::AddItem(std::string formid)
{
	std::string cmd = "player.additem " + formid + " 1";
	SendConsoleCommand(cmd);
}

// Register ConsoleCommand SCRIPT_FUNCTION.
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