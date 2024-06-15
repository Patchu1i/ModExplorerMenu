#pragma once

#include "PCH.h"
#include <string>
#include <unordered_set>

using MEMMap = std::multimap<std::string, std::multimap<std::string, std::multimap<std::string, std::pair<std::string, uint32_t>>>>;

class MEMData
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

	static CachedNPC* CreateCachedNPC(RE::TESNPC* a_npc);

	[[nodiscard]] static inline std::unordered_set<RE::TESFile*> GetModList()
	{
		return _modList;
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

	static inline std::vector<CachedItem> _cache;
	static inline std::vector<CachedCell> _cellCache;
	static inline std::vector<CachedNPC> _npcCache;
	static inline std::unordered_set<RE::TESFile*> _modList;

	void CacheCells(RE::TESFile* a_file, std::vector<CachedCell>& a_map);

	template <class T>
	static void CacheItems(RE::TESDataHandler* a_data);

	template <class T>
	void CacheNPCs(RE::TESDataHandler* a_data);

public:
	static inline MEMData* GetSingleton()
	{
		static MEMData singleton;
		return &singleton;
	}

	[[nodiscard]] static inline std::vector<CachedCell>& GetCellMap()
	{
		return _cellCache;
	}

	[[nodiscard]] static inline std::vector<CachedItem>& GetItemList()
	{
		return _cache;
	}

	[[nodiscard]] static inline std::vector<CachedNPC>& GetNPCList()
	{
		return _npcCache;
	}

	void Run();
};