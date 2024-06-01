#pragma once

#include "Data.h"
#include "Settings.h"

class TeleportWindow
{
public:
	static void Draw(Settings::Style a_style);
	static void ShowTable(Settings::Style a_style);
	static void ShowOptions(Settings::Style a_style);
	static void ShowInputSearch(Settings::Style a_style);
	static void ApplyFilters();

	static inline std::array<bool, 6> column_toggle = { true, true, true, true, true, true };

private:
	enum ColumnID
	{
		ColumnID_Favorite,
		ColumnID_ESM,
		ColumnID_Space,
		ColumnID_Zone,
		ColumnID_FullName,
		ColumnID_EditorID,
		ColumnID_None,
	};

	static inline ColumnID searchKey = ColumnID_None;
	static inline char inputBuffer[256] = "";
	static inline RE::TESFile* selectedMod = nullptr;

	static const int column_count = 6;
	static inline std::map<std::string, MEMData::CachedCell*> cellMap;
};