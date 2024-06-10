#pragma once

#include "Data.h"
#include "Libraries/DescriptionFrameworkAPI.h"
#include "Settings.h"

class NPCWindow
{
public:
	static void Draw(Settings::Style& a_style, Settings::Config& a_config);
	static void ShowFormTable(Settings::Style& a_style, Settings::Config& a_config);
	static void ShowSearch(Settings::Style& a_style, Settings::Config& a_config);
	static void ShowModSelection(Settings::Style& a_style, Settings::Config& a_config);
	static void ShowAdvancedOptions(Settings::Style& a_style, Settings::Config& a_config);
	static void ShowActions(Settings::Style& a_style, Settings::Config& a_config);
	static void ShowItemCard(MEMData::CachedNPC* a_npc);
	static void PopulateListWithLocals();
	static void ApplyFilters();
	static void Init();

	static inline std::vector<MEMData::CachedNPC*> npcList;

	// Action / Preview Sidebar
	static inline MEMData::CachedNPC* selectedNPC = nullptr;

private:
	static inline constexpr auto NPCTableFlags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_Sortable |
	                                             ImGuiTableFlags_Borders | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Hideable |
	                                             ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_NoBordersInBody |
	                                             ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX |
	                                             ImGuiTableFlags_SizingStretchProp;

	enum ColumnID
	{
		Favorite,
		FormID,
		Plugin,
		Name,
		EditorID,
		Health,
		Magicka,
		Stamina,
		CarryWeight,
		None,
	};

	static inline int column_count = ColumnID::None;

	// Local actions
	static inline bool showLocalsOnly = false;
	static inline std::shared_ptr<std::vector<RE::FormID>> localRefIDs = nullptr;

	// Search Input Field.
	static inline const ImGuiTableSortSpecs* s_current_sort_specs;
	static inline ColumnID searchKey = ColumnID::None;
	static inline char inputBuffer[256] = "";
	static inline std::string selectedMod = "All Mods";
	static inline bool dirty = true;

	static inline DescriptionFrameworkAPI::IDescriptionFrameworkInterface001* g_DescriptionFrameworkInterface = nullptr;

	// Sorting & Filtering
	static inline const std::map<ColumnID, const char*> InputSearchMap = {
		{ None, "None" },
		{ Name, "Name" },
		{ EditorID, "Editor ID" },
		{ FormID, "Form ID" }
	};

	static bool SortColumns(const MEMData::CachedNPC* v1, const MEMData::CachedNPC* v2);
	static void SortColumnsWithSpecs(ImGuiTableSortSpecs* sort_specs);
};