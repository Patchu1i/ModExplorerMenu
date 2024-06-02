#pragma once

#include "Data.h"
#include "lib/Graphic.h"
#include <unordered_set>

// #include "Console.h"

class AddItemWindow
{
public:
	static void Draw();
	static void Init();

	enum FilterType
	{
		Alchemy,
		Ingredient,
		Ammo,
		Key,
		Misc,
		Armor,
		Book,
		Weapon
	};

	enum ColumnID
	{
		ColumnID_Favorite,
		ColumnID_Type,
		ColumnID_FormID,
		ColumnID_Name,
		ColumnID_EditorID,
		ColumnID_GoldValue,
		ColumnID_None,
	};

	static inline std::array<bool, 6> column_toggle = { true, true, false, true, true, false };

	static inline const ImGuiTableSortSpecs* s_current_sort_specs;
	static inline ColumnID searchKey = ColumnID::ColumnID_Name;  // _searchBy
	static inline char inputBuffer[256] = "";                    // searchBuffer
	static inline RE::TESFile* selectedMod = nullptr;            // _currentMod
	static inline bool dirty = true;

	static constexpr int column_count = 6;
	static inline std::vector<MEMData::CachedItem*> itemList;  // _activeList

	static inline bool b_Alchemy = false;
	static inline bool b_Ingredient = false;
	static inline bool b_Ammo = false;
	static inline bool b_Key = false;
	static inline bool b_Misc = false;
	static inline bool b_Armor = false;
	static inline bool b_Book = false;
	static inline bool b_Weapon = false;

	static inline std::unordered_set<RE::FormType> itemFilters;  // _filters
	static inline std::vector<std::tuple<bool*, RE::FormType, std::string>> filterMap = {
		{ &b_Alchemy, RE::FormType::AlchemyItem, "ALCH" }, { &b_Ingredient, RE::FormType::Ingredient, "INGR" },
		{ &b_Ammo, RE::FormType::Ammo, "AMMO" }, { &b_Key, RE::FormType::KeyMaster, "KEYS" },
		{ &b_Misc, RE::FormType::Misc, "MISC" }, { &b_Armor, RE::FormType::Armor, "ARMO" },
		{ &b_Book, RE::FormType::Book, "BOOK" }, { &b_Weapon, RE::FormType::Weapon, "WEAP" }
	};

	static bool SortColumn(const MEMData::CachedItem* v1, const MEMData::CachedItem* v2);
	static void SortColumnsWithSpecs(ImGuiTableSortSpecs* sort_specs);

	// ImGui related calls.
	static void Draw_InputSearch();
	static void Draw_FormTable();
	static void Draw_Actions();
	static void Draw_AdvancedOptions();
	static void Context_CopyOnly(const char* form, const char* name, const char* editor);
	static void ApplyFilters();
};
