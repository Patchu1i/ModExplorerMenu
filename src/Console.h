#pragma once

#include <PCH.h>

class ConsoleCommand
{
public:
	static bool Run(const RE::SCRIPT_PARAMETER* a_paramInfo, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData,
		RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* a_containingObj, RE::Script* a_scriptObj,
		RE::ScriptLocals* a_locals, double& a_result, std::uint32_t& a_opcodeOffsetPtr);
	static void Register();
	static void SendConsoleCommand(std::string cmd);

	// Commands
	static void AddItem(std::string a_itemFormID, int a_count = 1);

	static void PlaceAtMe(std::string a_itemFormID, int a_count = 1);
	static void PlaceAtMeNPC(RE::FormID a_npcBaseID);
	static void PridLast();
	static void MoveToPlayer(std::string a_targetRefID, bool prid = true);
	static void MoveTo(std::string a_targetRefID);
	static void Kill(std::string a_targetRefIDbool, bool prid = true);
	static void Resurrect(std::string a_targetRefID, bool prid = true);
	static void UnEquipAll(std::string a_targetRefID, bool prid = true);
	static void ToggleFreeze(std::string a_targetRefID, bool prid = true);

	// Multi-threaded command processing.
	static inline std::string isWaitingFor;
	static inline std::stack<std::string> commandHistory;
	static inline std::deque<std::pair<std::string, std::chrono::milliseconds>> commandQueue;  // Main thread -> Process thread
	static inline std::queue<std::function<void()>> taskQueue;                                 // Process thread -> Main thread
	static inline std::atomic<bool> isProcessing{ false };
	static inline std::atomic<bool> isLocked{ false };
	static inline std::thread processThread;

	static void AddToQueue(std::string a_cmd, std::chrono::milliseconds a_delay);
	static void AddToFront(std::string a_cmd, std::chrono::milliseconds a_delay);
	static void WaitForLast();
	static void ProcessMainThreadTasks();
	static void StartProcessThread(bool unlock);

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