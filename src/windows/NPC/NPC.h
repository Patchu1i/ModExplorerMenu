#pragma once

#include "Data.h"
#include "Libraries/DescriptionFrameworkAPI.h"
#include "Settings.h"

namespace ModExplorerMenu
{
	class NPCWindow
	{
	public:
		static void Draw(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowFormTable(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowSearch(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowModSelection(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowAdvancedOptions(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowActions(Settings::Style& a_style, Settings::Config& a_config);
		static void PopulateListWithLocals();
		static void PopulateListWithSpawned();
		static std::shared_ptr<std::vector<RE::FormID>> GetRefID(RE::FormID a_targetBaseID);
		static void ApplyFilters();
		static void Init();

		static inline std::vector<NPC*> npcList;

		// Action / Preview Sidebar
		static inline NPC* selectedNPC = nullptr;

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

		enum State
		{
			showAll,
			showLocal,
			showSpawned
		};

		// State
		static inline State currentState = showAll;
		static void SetState(State newState)
		{
			currentState = newState;
		}

		static const State GetState()
		{
			return currentState;
		}

		static inline int column_count = ColumnID::None;

		static inline std::shared_ptr<std::vector<RE::FormID>> localRefIDs = nullptr;
		static inline bool applyActionsToAll = false;
		static inline bool b_placeFrozen = false;
		static inline bool b_clickToPlace = true;
		static inline bool b_placeNaked = false;

		// Search Input Field.
		static inline const ImGuiTableSortSpecs* s_current_sort_specs;
		static inline ColumnID searchKey = ColumnID::Name;
		static inline char inputBuffer[256] = "";
		static inline std::string selectedMod = "All Mods";
		static inline bool dirty = true;

		static inline DescriptionFrameworkAPI::IDescriptionFrameworkInterface001* g_DescriptionFrameworkInterface = nullptr;

		// Sorting & Filtering
		static inline const std::map<ColumnID, const char*> InputSearchMap = {
			{ Name, "Name" },
			{ EditorID, "Editor ID" },
			{ FormID, "Form ID" }
		};

		static bool SortColumns(const NPC* v1, const NPC* v2);
		static void SortColumnsWithSpecs(ImGuiTableSortSpecs* sort_specs);
	};
}