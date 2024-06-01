#pragma once

#include "PCH.h"
#include <string>
#include <unordered_set>

using MEMMap = std::multimap<std::string, std::multimap<std::string, std::multimap<std::string, std::pair<std::string, uint32_t>>>>;

class MEMData
{
public:
	struct CachedObject  // Removed ItemType (formType), should source from RE::FormType::
	{
		const char* name;
		std::string formid;
		RE::TESForm* form;
		std::string editorid;
		RE::FormType formType;
		std::string typeName;
		std::int32_t goldValue;
		RE::TESFile* mod;
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

	static inline std::vector<CachedObject> _cache;
	static inline std::map<std::string, CachedCell> _cellCache;  // EditorID, CachedCell
	static inline std::unordered_set<RE::TESFile*> _modList;
	//inline static std::map<std::pair<std::uint32_t, const std::string>, std::string_view> _cellMap;

	//inline static std::map<const std::string, std::pair<std::uint32_t, std::string_view>> _testMap;
	//inline static std::map<const std::string, CachedCell> _newMap;

	//inline static std::map<std::string, std::pair<std::string, std::pair<std::string, std::pair<std::string, MEMCell>>>> _cacheCells;
	//inline static std::map<const std::string, std::map<std::string, std::map<std::string, std::pair<std::string, uint32_t>>>> _cacheCells;

	//inline static std::multimap<std::string, std::multimap<std::string, std::multimap<std::string, std::pair<std::string, uint32_t>>>> pluginMap;

	void CacheCells(RE::TESFile* a_file, std::map<std::string, CachedCell>& a_map);

	template <class T>
	static void CacheItems(RE::TESDataHandler* a_data);

public:
	static inline MEMData* GetSingleton()
	{
		static MEMData singleton;
		return &singleton;
	}

	static inline std::map<std::string, CachedCell>& GetCellMap()
	{
		return _cellCache;
	}

	void Run();
};