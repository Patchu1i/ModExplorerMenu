#pragma once

#include "extern/DescriptionFrameworkAPI.h"
#include "include/C/Columns.h"
#include "include/F/Frame.h"
#include "include/G/Graphic.h"
#include "include/I/ISearch.h"
#include "include/I/ISortable.h"

// clang-format off

namespace Modex
{
	class NPCWindow : private ISortable, private ISearch
	{
	public:
		static inline NPCWindow* GetSingleton()
		{
			static NPCWindow singleton;
			return &singleton;
		}

		NPCWindow() = default;
		~NPCWindow() = default;

		void 					Draw();
		void 					Refresh();
		void 					Init();

	private:
		void 					ApplyFilters();
		void 					ShowFormTable();
		void 					ShowSearch();
		void 					ShowActions();
		void 					ShowNPCListContextMenu(NPCData& a_npc);
		std::set<std::string>	GetSecondaryFilterList();

		// Primary Filter List
		std::vector<RE::FormType> filterList = {
			{ RE::FormType::Class },
			{ RE::FormType::Race },
			{ RE::FormType::Faction }
		};
		
		// Filtering State Variables.
		bool 					b_ClickToSelect;
		bool 					b_ClickToFavorite;
		bool 					b_ClickToPlace;
		int 					clickToPlaceCount;

		bool 					_itemHovered;
		bool 					_itemSelected;
		NPCData* 				selectedNPC;
		NPCData* 				hoveredNPC;

		RE::FormType 			primaryFilter;
		std::vector<NPCData*> 	npcList;
		NPCColumns 				columnList;
		
		// Description Framework API.
		DescriptionFrameworkAPI::IDescriptionFrameworkInterface001* g_DescriptionFrameworkInterface = nullptr;

		// Input Fuzzy Search
		BaseColumn::ID 			searchKey;
		char 					inputBuffer[256];
		bool 					dirty;

		const std::map<BaseColumn::ID, const char*> InputSearchMap = {
			{ BaseColumn::ID::Name, "Name" },
			{ BaseColumn::ID::EditorID, "Editor ID" },
			{ BaseColumn::ID::FormID, "Form ID" },
			{ BaseColumn::ID::Race, "Race" }
		};

		// ISearch Interface
		char 					modSearchBuffer[256];
		std::string 			selectedMod;

		// SecondaryFilter ISearch Interface
		char					secondaryFilterBuffer[256];
		std::string				secondaryFilter;

	};
}