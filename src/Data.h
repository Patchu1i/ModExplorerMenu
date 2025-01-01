#pragma once

#include "Objects.h"
#include "PCH.h"
#include <string>
#include <unordered_set>

using MEMMap = std::multimap<std::string, std::multimap<std::string, std::multimap<std::string, std::pair<std::string, uint32_t>>>>;

namespace Modex
{

	class Data
	{
	public:
		static void CacheNPCRefIds();
		static void MergeNPCRefIds(std::shared_ptr<std::unordered_map<RE::FormID, RE::FormID>> npc_ref_map);

		enum
		{
			ITEM_MOD_LIST = 0,
			NPC_MOD_LIST,
			STATIC_MOD_LIST,
			CELL_MOD_LIST
		};

		[[nodiscard]] static inline std::vector<RE::TESFile*> SortModList(std::vector<RE::TESFile*> modList, int sort = 0)
		{
			switch (sort) {
			case 0:  // Alphabetical
				std::sort(modList.begin(), modList.end(), [](const RE::TESFile* a, const RE::TESFile* b) {
					return a->GetFilename() < b->GetFilename();
				});
				break;
			case 1:  // Last Installed
				std::sort(modList.begin(), modList.end(), [](RE::TESFile* a, RE::TESFile* b) {
					return _modListLastModified[a] > _modListLastModified[b];
				});
				break;
			}

			return modList;
		}

		[[nodiscard]] static inline std::vector<RE::TESFile*> GetModList(int listType, int sort = 0)
		{
			switch (listType) {
			case ITEM_MOD_LIST:
				return SortModList(std::vector<RE::TESFile*>(_itemModList.begin(), _itemModList.end()), sort);
			case NPC_MOD_LIST:
				return SortModList(std::vector<RE::TESFile*>(_npcModList.begin(), _npcModList.end()), sort);
			case STATIC_MOD_LIST:
				return SortModList(std::vector<RE::TESFile*>(_staticModList.begin(), _staticModList.end()), sort);
			case CELL_MOD_LIST:
				return SortModList(std::vector<RE::TESFile*>(_cellModList.begin(), _cellModList.end()), sort);
			default:
				return SortModList(std::vector<RE::TESFile*>(_modList.begin(), _modList.end()), sort);
			}
		}

	private:
		inline static const char* _skyrimFiles[9] = {
			"ccBGSSSE025-AdvDSGS.esm",
			"Dragonborn.esm",
			"HearthFires.esm",
			"Skyrim.esm",
			"ccBGSSSE037-Curios.esm",
			"Update.esm",
			"Dawnguard.esm",
			"ccBGSSSE001-Fish.esm",
			"ccQDRSSE001-SurvivalMode.esl",
		};

		static inline std::vector<Item> _cache;
		static inline std::vector<Cell> _cellCache;
		static inline std::vector<NPC> _npcCache;
		static inline std::vector<RE::TESObjectREFR*> _npcRefIds;
		static inline std::vector<StaticObject> _staticCache;
		static inline std::unordered_set<RE::TESFile*> _modList;

		static inline std::unordered_set<RE::TESFile*> _itemModList;
		static inline std::unordered_set<RE::TESFile*> _npcModList;
		static inline std::unordered_set<RE::TESFile*> _staticModList;
		static inline std::unordered_set<RE::TESFile*> _cellModList;

		static inline std::set<std::string> _npcClassList;
		static inline std::set<std::string> _npcRaceList;
		static inline std::set<std::string> _npcFactionList;

		struct ModFileItemFlags
		{
			bool alchemy = false;
			bool ingredient = false;
			bool ammo = false;
			bool key = false;
			bool misc = false;
			bool armor = false;
			bool book = false;
			bool weapon = false;
			bool scroll = false;
			bool npc = false;
			bool staticObject = false;
			bool tree = false;
			bool activator = false;
			bool container = false;
			bool door = false;
			bool light = false;
			bool furniture = false;
		};

		static inline std::unordered_map<RE::TESFile*, ModFileItemFlags> _itemListModFormTypeMap;
		static inline std::unordered_map<RE::TESFile*, std::time_t> _modListLastModified;

		static void CacheCells(RE::TESFile* a_file, std::vector<Cell>& a_map);

		template <class T>
		static void CacheItems(RE::TESDataHandler* a_data);

		template <class T>
		static void CacheNPCs(RE::TESDataHandler* a_data);

		template <class T>
		static void CacheStaticObjects(RE::TESDataHandler* a_data);

		// Windows Specific API for File Creation Time
		[[nodiscard]] static std::time_t GetFileCreationTime(const std::filesystem::path& path)
		{
			HANDLE hFile = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) {
				logger::warn("[Modex] Unable to open file: {}", path.string());
				return std::time_t{};
			}

			FILETIME creationTime;
			if (!GetFileTime(hFile, &creationTime, NULL, NULL)) {
				logger::warn("[Modex] Unable to get file time: {}", path.string());
				CloseHandle(hFile);
				return std::time_t{};
			}

			CloseHandle(hFile);

			ULARGE_INTEGER ull;
			ull.LowPart = creationTime.dwLowDateTime;
			ull.HighPart = creationTime.dwHighDateTime;

			return std::chrono::system_clock::to_time_t(std::chrono::system_clock::time_point(std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::nanoseconds(ull.QuadPart * 100))));
		}

	public:
		static void GenerateItemList();
		static void GenerateNPCList();
		static void GenerateObjectList();
		static void GenerateCellList();

		static void GenerateNPCClassList();
		static void GenerateNPCRaceList();
		static void GenerateNPCFactionList();

		static inline Data* GetSingleton()
		{
			static Data singleton;
			return &singleton;
		}

		[[nodiscard]] static inline std::vector<Cell>& GetCellMap()
		{
			return _cellCache;
		}

		[[nodiscard]] static inline std::vector<Item>& GetItemList()
		{
			return _cache;
		}

		[[nodiscard]] static std::unordered_map<RE::TESFile*, ModFileItemFlags> GetModFormTypeMap()
		{
			return _itemListModFormTypeMap;
		}

		[[nodiscard]] static inline std::vector<NPC>& GetNPCList()
		{
			return _npcCache;
		}

		[[nodiscard]] static inline std::set<std::string>& GetNPCClassList()
		{
			return _npcClassList;
		}

		[[nodiscard]] static inline std::set<std::string>& GetNPCRaceList()
		{
			return _npcRaceList;
		}

		[[nodiscard]] static inline std::set<std::string>& GetNPCFactionList()
		{
			return _npcFactionList;
		}

		[[nodiscard]] static inline std::vector<RE::TESObjectREFR*>& GetNPCRefIds()
		{
			return _npcRefIds;
		}

		[[nodiscard]] static inline std::vector<StaticObject>& GetObjectList()
		{
			return _staticCache;
		}

		void Run();
	};
}