#pragma once

#include <PCH.h>

class ConsoleCommand
{
public:
	static bool Run(const RE::SCRIPT_PARAMETER* a_paramInfo, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData,
		RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* a_containingObj, RE::Script* a_scriptObj,
		RE::ScriptLocals* a_locals, double& a_result, std::uint32_t& a_opcodeOffsetPtr);
	static void Register();
	static void SendConsoleCommand(std::string_view cmd);

	// Commands
	static void AddItem(std::string a_itemFormID, int a_count = 1);
	static void PlaceAtMe(std::string a_itemFormID, int a_count = 1);
	static void MoveToPlayer(std::string a_targetRefID);
	static void MoveTo(std::string a_targetRefID);

	// Multi-threaded command processing.
	static inline std::queue<std::pair<std::string, int>> commandQueue;  // Main thread -> Process thread
	static inline std::queue<std::function<void()>> taskQueue;           // Process thread -> Main thread
	static inline std::atomic<bool> isProcessing{ false };
	static inline std::thread processThread;

	static void AddToQueue(std::string a_cmd, int a_delay);
	static void ProcessMainThreadTasks();
	static void StartProcessThread();

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