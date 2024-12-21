#pragma once

#include "Objects.h"
#include "PCH.h"
#include <string>
#include <unordered_set>

using MEMMap = std::multimap<std::string, std::multimap<std::string, std::multimap<std::string, std::pair<std::string, uint32_t>>>>;

namespace ModExplorerMenu
{

	class Data
	{
	public:
		void CacheNPCRefIds();
		void MergeNPCRefIds(std::shared_ptr<std::unordered_map<RE::FormID, RE::FormID>> npc_ref_map);

		struct CachedItem  // Removed ItemType (formType), should source from RE::FormType::
		{
			const char* name;
			std::string formid;
			RE::TESForm* form;
			std::string editorid;
			RE::FormType formType;
			std::string typeName;
			std::int32_t goldValue;
			RE::TESFile* mod;
			float weight;
			bool nonPlayable;
			bool favorite;
			int quantity;
			bool selected;
		};

		struct CachedCell
		{
			std::string plugin;
			std::string space;
			std::string zone;
			std::string fullName;
			std::string editorid;
			uint32_t cellid;
			bool favorite = false;
			const RE::TESFile* mod;

			CachedCell(std::string plugin, std::string space, std::string zone, std::string fullName, std::string editorid, const RE::TESFile* mod = nullptr) :
				plugin(plugin), space(space), zone(zone), fullName(fullName), editorid(editorid), mod(mod) {}
		};

		struct CachedNPC
		{
			RE::TESForm* form;
			RE::FormID refID;
			std::string plugin;
			std::string name;
			std::string formid;
			std::string editorid;

			float health;
			float magicka;
			float stamina;
			float carryweight;

			RE::TESNPC::Skills skills;

			bool favorite;
		};

		enum
		{
			ITEM_MOD_LIST = 0,
			NPC_MOD_LIST,
			STATIC_MOD_LIST,
			CELL_MOD_LIST
		};

		static CachedNPC* CreateCachedNPC(RE::TESNPC* a_npc);

		// [[nodiscard]] static inline std::unordered_set<RE::TESFile*> GetModList()
		// {
		// 	return _modList;
		// }

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
		};

		static inline std::unordered_map<RE::TESFile*, ModFileItemFlags> _itemListModFormTypeMap;
		static inline std::unordered_map<RE::TESFile*, std::time_t> _modListLastModified;

		void CacheCells(RE::TESFile* a_file, std::vector<Cell>& a_map);

		template <class T>
		static void CacheItems(RE::TESDataHandler* a_data);

		template <class T>
		void CacheNPCs(RE::TESDataHandler* a_data);

		template <class T>
		void CacheStaticObjects(RE::TESDataHandler* a_data);

		// Windows Specific API for File Creation Time
		[[nodiscard]] static std::time_t GetFileCreationTime(const std::filesystem::path& path)
		{
			HANDLE hFile = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) {
				std::cerr << "Unable to open file: " << path << std::endl;
				return std::time_t{};
			}

			FILETIME creationTime;
			if (!GetFileTime(hFile, &creationTime, NULL, NULL)) {
				std::cerr << "Unable to get file creation time: " << path << std::endl;
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