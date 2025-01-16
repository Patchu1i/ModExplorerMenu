#pragma once

#include "extern/DescriptionFrameworkAPI.h"
#include "include/C/Columns.h"
#include "include/F/Frame.h"
#include "include/G/Graphic.h"
#include "include/I/ISearch.h"
#include "include/I/ISortable.h"

namespace Modex
{
	class NPCWindow : private ISortable, private ISearch
	{
	private:
		void ShowFormTable();
		void ShowSearch();
		void ShowActions();
		void ShowNPCListContextMenu(NPCData& a_npc);
		void ApplyFilters();

		std::vector<NPCData*> npcList;
		NPCColumns columnList;

		// Behavior State Variables.
		bool b_ClickToSelect = true;
		bool b_ClickToFavorite = false;
		bool b_ClickToPlace = false;
		int clickToPlaceCount = 1;

		bool _itemHovered = false;
		bool _itemSelected = false;
		NPCData* selectedNPC = nullptr;
		NPCData* hoveredNPC = nullptr;

		// Filtering
		bool b_ShowSearchbar = false;
		bool b_Class = false;
		bool b_Race = false;
		bool b_Faction = false;

		// static inline std::vector<std::pair<bool*, std::string>> filterMap = {
		// 	{ &b_Class, "Class" },
		// 	{ &b_Race, "Race" },
		// 	{ &b_Faction, "Faction" }
		// };

		// static inline std::vector<std::pair<std::function<std::set<std::string>()>, std::string>> secondaryFilterMap = {
		// 	{ RE::FormType::Class, "Class" },
		// 	{ &Data::GetSingleton()->GetNPCRaceList, "Race" },
		// 	{ &Data::GetSingleton()->GetNPCFactionList, "Faction" }
		// };

		std::string selectedFilter = "None";
		std::string secondaryFilter = "Show All";

	public:
		static inline NPCWindow* GetSingleton()
		{
			static NPCWindow singleton;
			return &singleton;
		}

		NPCWindow() = default;
		~NPCWindow() = default;

		void Draw();
		void Refresh();
		void Init();

	private:
		// Search Input Field.
		BaseColumn::ID searchKey = BaseColumn::ID::Name;
		char inputBuffer[256] = "";
		// static inline char modListBuffer[256] = "";
		// static inline std::string selectedMod = "All Mods";
		char secondaryFilterBuffer[256] = "";
		bool dirty = true;

		// Sorting & Filtering
		const std::map<BaseColumn::ID, const char*> InputSearchMap = {
			{ BaseColumn::ID::Name, "Name" },
			{ BaseColumn::ID::EditorID, "Editor ID" },
			{ BaseColumn::ID::FormID, "Form ID" },
			{ BaseColumn::ID::Race, "Race" }
		};

		char modSearchBuffer[256];
		std::string selectedMod;

		DescriptionFrameworkAPI::IDescriptionFrameworkInterface001* g_DescriptionFrameworkInterface = nullptr;
	};
}