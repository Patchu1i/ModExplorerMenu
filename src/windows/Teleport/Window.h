#pragma once

#include "Data.h"
#include "Settings.h"

namespace ModExplorerMenu
{
	class TeleportWindow
	{
	public:
		static void Draw(Settings::Style& a_style);
		static void ShowTable(Settings::Style& a_style);
		static void ShowOptions(Settings::Style& a_style);
		static void ShowInputSearch(Settings::Style& a_style);
		static void ApplyFilters();

		static inline std::array<bool, 6> column_toggle = { true, true, true, true, true, true };

	private:
		enum ColumnID
		{
			ColumnID_Favorite,
			ColumnID_Plugin,
			ColumnID_Space,
			ColumnID_Zone,
			ColumnID_FullName,
			ColumnID_EditorID,
			ColumnID_None,
		};

		//static bool SortColumn(const std::map<std::string, Data::CachedCell*>* a_v1, const std::map<std::string, Data::CachedCell*>* a_v2);
		//static bool SortColumn(const std::pair<const std::string, Data::CachedCell*>& a_v1, const std::pair<const std::string, Data::CachedCell*>& a_v2);
		static bool SortColumn(const Data::CachedCell* v1, const Data::CachedCell* v2);
		static void SortColumnsWithSpecs(ImGuiTableSortSpecs* sort_specs);

		static inline const ImGuiTableSortSpecs* s_current_sort_specs;
		static inline ColumnID searchKey = ColumnID_None;
		static inline char inputBuffer[256] = "";
		static inline RE::TESFile* selectedMod = nullptr;
		static inline bool dirty = true;

		static const int column_count = 6;
		//static inline std::map<std::string, Data::CachedCell*> cellMap;
		static inline std::vector<Data::CachedCell*> cellMap;
	};
}