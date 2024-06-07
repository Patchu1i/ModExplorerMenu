#pragma once

#include "Data.h"
#include "Graphic.h"
#include "Libraries/DescriptionFrameworkAPI.h"
#include "Settings.h"
#include <unordered_set>

// #include "Console.h"

class AddItemWindow
{
public:
	static void Draw(Settings::Style& a_style, Settings::Config& a_config);
	static void ShowModSelection(Settings::Style& a_style, Settings::Config& a_config);
	static void ShowAdvancedOptions(Settings::Style& a_style, Settings::Config& a_config);
	static void ShowFormTable(Settings::Style& a_style, Settings::Config& a_config);
	static void ShowActions(Settings::Style& a_style, Settings::Config& a_config);
	static void ShowSearch(Settings::Style& a_style, Settings::Config& a_config);
	static void ShowItemCard(MEMData::CachedItem* item);
	static void ShowPlotGraph();
	static void ShowHistogramGraph();
	static void ShowItemListContextMenu(MEMData::CachedItem& a_item);
	static void ShowBookPreview();
	static void ApplyFilters();
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
		ColumnID_BaseDamage,
		ColumnID_Speed,
		ColumnID_CritDamage,
		ColumnID_Skill,
		ColumnID_Weight,
		ColumnID_DPS,
		ColumnID_None
	};

	// Show book window.
	static inline MEMData::CachedItem* openBook = nullptr;

	// Actions
	static inline bool b_ClickToAdd = false;
	static inline int clickToAddCount = 1;
	static inline bool b_ClickToPlace = false;
	static inline int clickToPlaceCount = 1;
	static inline bool b_StickySelect = true;
	static inline bool b_ClickToFavorite = false;

	// Search Input Field.
	static inline const ImGuiTableSortSpecs* s_current_sort_specs;
	static inline ColumnID searchKey = ColumnID::ColumnID_Name;
	static inline char inputBuffer[256] = "";
	static inline RE::TESFile* selectedMod = nullptr;
	static inline bool dirty = true;

	// Table & Filtering.
	static inline bool b_Alchemy = false;
	static inline bool b_Ingredient = false;
	static inline bool b_Ammo = false;
	static inline bool b_Key = false;
	static inline bool b_Misc = false;
	static inline bool b_Armor = false;
	static inline bool b_Book = false;
	static inline bool b_Weapon = false;

	static constexpr int column_count = 12;
	static inline std::unordered_set<RE::FormType> itemFilters;
	static inline std::vector<MEMData::CachedItem*> itemList;
	static inline std::vector<std::tuple<bool*, RE::FormType, std::string>> filterMap = {
		{ &b_Alchemy, RE::FormType::AlchemyItem, "ALCH" }, { &b_Ingredient, RE::FormType::Ingredient, "INGR" },
		{ &b_Ammo, RE::FormType::Ammo, "AMMO" }, { &b_Key, RE::FormType::KeyMaster, "KEYS" },
		{ &b_Misc, RE::FormType::Misc, "MISC" }, { &b_Armor, RE::FormType::Armor, "ARMO" },
		{ &b_Book, RE::FormType::Book, "BOOK" }, { &b_Weapon, RE::FormType::Weapon, "WEAP" }
	};

	static bool SortColumn(const MEMData::CachedItem* v1, const MEMData::CachedItem* v2);
	static void SortColumnsWithSpecs(ImGuiTableSortSpecs* sort_specs);

	// Description Framework API.
	static inline DescriptionFrameworkAPI::IDescriptionFrameworkInterface001* g_DescriptionFrameworkInterface = nullptr;

private:
	// TODO: Add RowBG as theme option
	static inline constexpr auto AddItemTableFlags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_Sortable |
	                                                 ImGuiTableFlags_Borders | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Hideable |
	                                                 ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_NoBordersInBody |
	                                                 ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY |
	                                                 ImGuiTableFlags_SizingStretchProp;
	static inline constexpr auto InputSearchFlags = ImGuiInputTextFlags_EscapeClearsAll;

	static inline constexpr auto ActionBarFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;

	static inline const std::map<ColumnID, const char*> InputSearchMap = {
		{ ColumnID_None, "None" },
		{ ColumnID_Name, "Name" },
		{ ColumnID_EditorID, "Editor ID" },
		{ ColumnID_FormID, "Form ID" }
	};
};
