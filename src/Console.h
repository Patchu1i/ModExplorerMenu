#pragma once

#include <PCH.h>

namespace ModExplorerMenu
{
	class Console
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
		static void PlaceAtMeFormID(RE::FormID a_npcBaseID, int a_count = 1);
		static void PridLast();
		static void MoveToREFR(std::string a_targetRefID);
		static void MoveREFRToPlayer(std::string a_targetRefID);
		static void KillREFR(std::string a_targetRefIDbool);
		static void Kill();
		static void ResurrectREFR(std::string a_targetRefID, int a_param = 1);
		static void Resurrect(int a_param = 1);
		static void UnEquipREFR(std::string a_targetRefID);
		static void UnEquip();
		static void FreezeREFR(std::string a_targetRefID);
		static void Freeze();
		static void Teleport(std::string a_editorID);

		// Multi-threaded command processing.
		static inline std::vector<RE::FormID> npcPlaceHistoryVector;
		static inline std::unordered_set<RE::FormID> npcPlaceHistorySet;

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
		static void StartProcessThread(bool unlock = false);
		[[nodiscard]] static std::unordered_set<RE::FormID>* GetSpawnedActorsSet() { return &npcPlaceHistorySet; }
		[[nodiscard]] static std::vector<RE::FormID>* GetSpawnedActorsVector() { return &npcPlaceHistoryVector; }

	private:
		Console() = delete;
		Console(const Console&) = delete;
		Console(Console&&) = delete;
		~Console() = delete;
		Console& operator=(const Console&) = delete;
		Console& operator=(Console&&) = delete;

		// Define LONG & SHORT console commands
		static constexpr char LONG_NAME[] = "OpenMenu";
		static constexpr char SHORT_NAME[] = "mm";
	};
}