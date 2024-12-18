#pragma once

#include "Data.h"
#include "Settings.h"
#include "Windows/Columns.h"
#include "Windows/Frame.h"

namespace ModExplorerMenu
{
	class ObjectWindow : private ISortable
	{
	private:
		static inline std::vector<StaticObject*> objectList;
		static inline ObjectColumns columnList;
		static inline StaticObject* selectedObject;

	public:
		static void Draw(Settings::Style& a_style, Settings::Config& a_config);
		static void Init();

	private:
		static void ApplyFilters();
		static void ShowActions(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowSearch(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowFormTable(Settings::Style& a_style, Settings::Config& a_config);

		static inline constexpr auto TeleportTableFlags =
			ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Sortable |
			ImGuiTableFlags_Borders | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Hideable |
			ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_NoBordersInBody |
			ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;

		// Actions
		static inline bool b_clickToPlace = false;

		// Searching
		static inline BaseColumn::ID searchKey = BaseColumn::ID::EditorID;
		static inline char inputBuffer[256] = "";
		static inline char modListBuffer[256] = "";
		static inline std::string selectedMod = ICON_RPG_WRENCH " All Mods";
		static inline bool dirty = true;

		// Sorting & Filtering
		static inline const std::map<BaseColumn::ID, const char*> InputSearchMap = {
			{ BaseColumn::ID::Plugin, "Plugin" },
			{ BaseColumn::ID::Type, "Type" },
			{ BaseColumn::ID::FormID, "Form ID" },
			{ BaseColumn::ID::EditorID, "Editor ID" }
		};

		//static const int column_count = 6;
		//static inline std::map<std::string, Data::CachedCell*> cellMap;
	};
}