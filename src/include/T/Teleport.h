#pragma once

#include "include/C/Columns.h"
#include "include/F/Frame.h"
#include "include/I/ISearch.h"
#include "include/I/ISortable.h"

// clang-format off

namespace Modex
{

	class BaseColumnList
    {
    public:
        std::vector<BaseColumn> columns;

        inline const int GetTotalColumns() const { return static_cast<int>(columns.size()); }
    };

    class TeleportColumns : public BaseColumnList
    {
    public:
        static inline auto flag = ImGuiTableColumnFlags_WidthStretch;

        TeleportColumns()
        {
            // columns.push_back({ ICON_RPG_HEART, ImGuiTableColumnFlags_WidthFixed, 15.0f, false, BaseColumn::ID::Favorite });
            columns.push_back({ _T("Plugin"), flag, 0.0f, false, BaseColumn::ID::Plugin });
            columns.push_back({ _T("Space"), flag, 0.0f, false, BaseColumn::ID::Space });
            columns.push_back({ _T("Zone"), flag, 0.0f, false, BaseColumn::ID::Zone });
            columns.push_back({ _T("Cell"), flag, 0.0f, false, BaseColumn::ID::CellName });
            columns.push_back({ _T("Editor ID"), flag, 0.0f, false, BaseColumn::ID::EditorID });
        }
    };
	
	class TeleportWindow : private ISortable, private ISearch
	{
	public:
		static inline TeleportWindow* GetSingleton()
		{
			static TeleportWindow singleton;
			return std::addressof(singleton);
		}

		TeleportWindow() = default;
		~TeleportWindow() = default;

		void 			Draw(float a_offset);
		void 			Init(bool is_default);
		void			Unload();
		void			Load();
		void 			Refresh();

	private:
		void 			ApplyFilters();
		void 			ShowActions();
		void 			ShowSearch();
		void 			ShowFormTable();
		void 			ShowTeleportContextMenu(CellData& a_cell);

		// Local State Variables.
		bool 						b_ClickToTeleport;

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
	};
}