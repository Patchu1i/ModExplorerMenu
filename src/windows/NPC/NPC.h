#pragma once

#include "Data.h"
#include "Libraries/DescriptionFrameworkAPI.h"
#include "Settings.h"
#include "Windows/Columns.h"

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

		enum State
		{
			showAll,
			showLocal,
			showSpawned
		};

		// Column Container
		static inline NPCColumns columnList;

		// State
		static inline State currentState = showAll;
		static inline void SetState(State newState) { currentState = newState; }
		static inline const State GetState() { return currentState; }

		// NPC Spawning & Actions
		static inline std::shared_ptr<std::vector<RE::FormID>> localRefIDs = nullptr;
		static inline bool applyActionsToAll = false;
		static inline bool b_placeFrozen = false;
		static inline bool b_clickToPlace = true;
		static inline bool b_placeNaked = false;

		// Search Input Field.
		static inline const ImGuiTableSortSpecs* s_current_sort_specs;
		static inline BaseColumn::ID searchKey = BaseColumn::ID::Name;
		static inline char inputBuffer[256] = "";
		static inline std::string selectedMod = "All Mods";
		static inline bool dirty = true;

		static inline DescriptionFrameworkAPI::IDescriptionFrameworkInterface001* g_DescriptionFrameworkInterface = nullptr;

		// Sorting & Filtering
		static inline const std::map<BaseColumn::ID, const char*> InputSearchMap = {
			{ BaseColumn::ID::Name, "Name" },
			{ BaseColumn::ID::EditorID, "Editor ID" },
			{ BaseColumn::ID::FormID, "Form ID" }
		};

		static bool SortColumns(const NPC* v1, const NPC* v2);
		static void SortColumnsWithSpecs(ImGuiTableSortSpecs* sort_specs);
	};
}