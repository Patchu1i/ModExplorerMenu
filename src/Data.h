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

		static NPC* CreateCachedNPC(RE::TESNPC* a_npc);

		// [[nodiscard]] static inline std::unordered_set<RE::TESFile*> GetModList()
		// {
		// 	return _modList;
		// }

		[[nodiscard]] static inline std::vector<RE::TESFile*> GetModList()
		{
			std::vector<RE::TESFile*> modList(_modList.begin(), _modList.end());

			std::sort(modList.begin(), modList.end(), [](const RE::TESFile* a, const RE::TESFile* b) {
				return a->GetFilename() < b->GetFilename();
			});

			return modList;
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
		static inline std::vector<StaticObject> _staticCache;
		static inline std::unordered_set<RE::TESFile*> _modList;

		void CacheCells(RE::TESFile* a_file, std::vector<Cell>& a_map);

		template <class T>
		static void CacheItems(RE::TESDataHandler* a_data);

		template <class T>
		void CacheNPCs(RE::TESDataHandler* a_data);

		template <class T>
		void CacheStaticObjects(RE::TESDataHandler* a_data);

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

		[[nodiscard]] static inline std::vector<NPC>& GetNPCList()
		{
			return _npcCache;
		}

		[[nodiscard]] static inline std::vector<StaticObject>& GetObjectList()
		{
			return _staticCache;
		}

		void Run();
	};
}