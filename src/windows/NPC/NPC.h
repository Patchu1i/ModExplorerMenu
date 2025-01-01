#pragma once

#include "Data.h"
#include "Libraries/DescriptionFrameworkAPI.h"
#include "Settings.h"
#include "Windows/Columns.h"
#include "Windows/Frame.h"

namespace Modex
{
	class NPCWindow : private ISortable
	{
	private:
		static void ShowFormTable(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowSearch(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowActions(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowNPCListContextMenu(NPC& a_npc);
		static void ApplyFilters();

		static inline std::vector<NPC*> npcList;
		static inline NPCColumns columnList;

		// Behavior State Variables.
		static inline bool b_ClickToSelect = true;
		static inline bool b_ClickToFavorite = false;
		static inline bool b_PlaceFrozen = false;
		static inline bool b_PlaceNaked = false;

		static inline bool _itemHovered = false;
		static inline bool _itemSelected = false;
		static inline NPC* selectedNPC = nullptr;
		static inline NPC* hoveredNPC = nullptr;

		// Filtering
		static inline bool b_ShowSearchbar = false;
		static inline bool b_Class = false;
		static inline bool b_Race = false;
		static inline bool b_Faction = false;

		static inline std::vector<std::tuple<bool*, std::function<void()>, std::string>> filterMap = {
			{ &b_Class, Data::GenerateNPCClassList, "Class" },
			{ &b_Race, Data::GenerateNPCRaceList, "Race" },
			{ &b_Faction, Data::GenerateNPCFactionList, "Faction" }
		};

		static inline std::vector<std::pair<std::function<std::set<std::string>()>, std::string>> secondaryFilterMap = {
			{ Data::GetNPCClassList, "Class" },
			{ Data::GetNPCRaceList, "Race" },
			{ Data::GetNPCFactionList, "Faction" }
		};

		static inline std::string selectedFilter = "None";
		static inline std::string secondaryFilter = "Show All";

	public:
		static void Draw(Settings::Style& a_style, Settings::Config& a_config);
		static void Refresh();
		static void Init();

	private:
		// Search Input Field.
		static inline BaseColumn::ID searchKey = BaseColumn::ID::Name;
		static inline char inputBuffer[256] = "";
		static inline char modListBuffer[256] = "";
		static inline char secondaryFilterBuffer[256] = "";
		static inline std::string selectedMod = "All Mods";
		static inline bool dirty = true;

		// Sorting & Filtering
		static inline const std::map<BaseColumn::ID, const char*> InputSearchMap = {
			{ BaseColumn::ID::Name, "Name" },
			{ BaseColumn::ID::EditorID, "Editor ID" },
			{ BaseColumn::ID::FormID, "Form ID" },
			{ BaseColumn::ID::Race, "Race" }
		};

		static inline DescriptionFrameworkAPI::IDescriptionFrameworkInterface001* g_DescriptionFrameworkInterface = nullptr;
	};
}