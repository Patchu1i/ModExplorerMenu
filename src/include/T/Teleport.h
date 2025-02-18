#pragma once

#include "include/C/Columns.h"
#include "include/F/Frame.h"
#include "include/I/ISearch.h"
#include "include/I/ISortable.h"

// clang-format off

namespace Modex
{
	class TeleportWindow : private ISortable, private ISearch
	{
	public:
		static inline TeleportWindow* GetSingleton()
		{
			static TeleportWindow singleton;
			return &singleton;
		}

		TeleportWindow() = default;
		~TeleportWindow() = default;

		void 			Draw(float a_offset);
		void 			Init();
		void 			Refresh();

	private:
		void 			ApplyFilters();
		void 			ShowActions();
		void 			ShowSearch();
		void 			ShowFormTable();
		void 			ShowTeleportContextMenu(CellData& a_cell);

		// Local State Variables.
		bool 						b_ClickToTeleport;
		// bool 						b_ClickToFavorite;

		std::vector<CellData*> 		cellList;
		TeleportColumns 			columnList;
		CellData* 					selectedCell;

		// Mouse Draggin
		enum MOUSE_STATE
		{
			DRAG_IGNORE,
			DRAG_SELECT,
			DRAG_NONE
		};

		std::unordered_set<CellData*> itemSelectionList;
		MOUSE_STATE						isMouseSelecting;
		ImVec2							mouseDragStart;
		ImVec2							mouseDragEnd;

		// Input Fuzzy Search
		BaseColumn::ID 				searchKey;
		char 						inputBuffer[256];
		bool 						dirty;

		// Sorting & Filtering
		const std::map<BaseColumn::ID, const char*> InputSearchMap = {
			{ BaseColumn::ID::Plugin, "Plugin" },
			{ BaseColumn::ID::Space, "Worldspace" },
			{ BaseColumn::ID::Zone, "Zone" },
			{ BaseColumn::ID::CellName, "Cell" },
			{ BaseColumn::ID::EditorID, "Editor ID" }
		};

		// ISearch Interface
		char 						modSearchBuffer[256];
		std::string 				selectedMod;

		TableView<CellData>			tableView;
	};
}