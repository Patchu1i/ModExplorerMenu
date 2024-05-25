#pragma once

#	include <PCH.h>

class ConsoleCommand
{
public:
	static bool Run(const RE::SCRIPT_PARAMETER* a_paramInfo, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData,
		RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* a_containingObj, RE::Script* a_scriptObj,
		RE::ScriptLocals* a_locals, double& a_result, std::uint32_t& a_opcodeOffsetPtr);
	static void Register();
	static void AddItem(std::string formid);
	static void PlaceAtMe(std::string formid);
	static void SendConsoleCommand(std::string_view cmd);

private:
	ConsoleCommand() = delete;
	ConsoleCommand(const ConsoleCommand&) = delete;
	ConsoleCommand(ConsoleCommand&&) = delete;
	~ConsoleCommand() = delete;
	ConsoleCommand& operator=(const ConsoleCommand&) = delete;
	ConsoleCommand& operator=(ConsoleCommand&&) = delete;

	// Define LONG & SHORT console commands
	static constexpr char LONG_NAME[] = "OpenMenu";
	static constexpr char SHORT_NAME[] = "mm";
};