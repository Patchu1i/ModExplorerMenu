#pragma once

#include "Data.h"
#include "Settings.h"
#include "Windows/Columns.h"
#include "Windows/Frame.h"

namespace Modex
{
	class QuestWindow : private ISortable
	{
	private:
		static void ShowFormTable(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowSearch(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowActions(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowQuestListContextMenu(Quest& a_quest);
		static void ApplyFilters();

		static inline std::vector<Quest*> questList;
		static inline QuestColumns columnList;

		// Behavior State Variables.
		static inline bool b_ClickToSelect = true;
		static inline bool b_ClickToFavorite = false;

		static inline bool _itemHovered = false;
		static inline bool _itemSelected = false;
		static inline Quest* selectedQuest = nullptr;
		static inline Quest* hoveredQuest = nullptr;

		// Filtering

	public:
		static void Draw(Settings::Style& a_style, Settings::Config& a_config);
		static void Refresh();
		static void Init();

	private:
		// Search Input Field
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
	};
}