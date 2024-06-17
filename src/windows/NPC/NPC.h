#pragma once

#include "Data.h"
#include "Libraries/DescriptionFrameworkAPI.h"
#include "Settings.h"
#include "Windows/Columns.h"

namespace ModExplorerMenu
{
	class NPCWindow : private ISortable
	{
	private:
		enum State
		{
			showAll,
			showLocal,
			showSpawned
		};

		static inline std::vector<NPC*> npcList;
		static inline NPCColumns columnList;
		static inline State currentState;
		static inline NPC* selectedNPC = nullptr;

	public:
		static void Draw(Settings::Style& a_style, Settings::Config& a_config);
		static void Init();

		static inline void SetState(State a_state) { currentState = a_state; }
		static State GetState() { return currentState; }

	private:
		static void ShowFormTable(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowSearch(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowAdvancedOptions(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowActions(Settings::Style& a_style, Settings::Config& a_config);
		static void PopulateListWithLocals();
		static void PopulateListWithSpawned();
		static void ApplyFilters();

		static inline auto NPCTableFlags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_Sortable |
		                                   ImGuiTableFlags_Borders | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Hideable |
		                                   ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_NoBordersInBody |
		                                   ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX |
		                                   ImGuiTableFlags_SizingStretchProp;

		// NPC Spawning & Actions
		static inline std::shared_ptr<std::vector<RE::FormID>> localRefIDs = nullptr;
		static inline bool applyActionsToAll = false;
		static inline bool b_placeFrozen = false;
		static inline bool b_clickToPlace = true;
		static inline bool b_placeNaked = false;

		// Search Input Field.
		static inline BaseColumn::ID searchKey = BaseColumn::ID::Name;
		static inline char inputBuffer[256] = "";
		static inline std::string selectedMod = "All Mods";
		static inline bool dirty = true;

		// Sorting & Filtering
		static inline const std::map<BaseColumn::ID, const char*> InputSearchMap = {
			{ BaseColumn::ID::Name, "Name" },
			{ BaseColumn::ID::EditorID, "Editor ID" },
			{ BaseColumn::ID::FormID, "Form ID" }
		};

		static inline DescriptionFrameworkAPI::IDescriptionFrameworkInterface001* g_DescriptionFrameworkInterface = nullptr;
	};
}