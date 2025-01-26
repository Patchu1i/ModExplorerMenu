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

		void 					Draw(float a_offset);
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
		bool 					b_ClickToPlace;
		bool 					b_ClickToFavorite;
		int 					clickToPlaceCount;

		bool 					_itemHovered;
		bool 					_itemSelected;
		NPCData* 				selectedNPC;
		NPCData* 				hoveredNPC;

		RE::FormType 			primaryFilter;
		std::vector<NPCData*> 	npcList;
		NPCColumns 				columnList;

		// Mouse Draggin
		enum MOUSE_STATE
		{
			DRAG_IGNORE,
			DRAG_SELECT,
			DRAG_NONE
		};

		std::unordered_set<NPCData*> 	itemSelectionList;
		MOUSE_STATE						isMouseSelecting;
		ImVec2							mouseDragStart;
		ImVec2							mouseDragEnd;

		// Menu State Variables.
		enum class Viewport
		{
			TableView,
			BlacklistView,
			SettingsView // TODO.
		};

		Viewport 				activeViewport;
		
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