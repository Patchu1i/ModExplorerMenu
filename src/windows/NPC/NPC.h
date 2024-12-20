#pragma once

#include "Data.h"
#include "Libraries/DescriptionFrameworkAPI.h"
#include "Settings.h"
#include "Windows/Columns.h"
#include "Windows/Frame.h"

namespace ModExplorerMenu
{
	class NPCWindow : private ISortable
	{
	private:
		static void ShowFormTable(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowSearch(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowActions(Settings::Style& a_style, Settings::Config& a_config);
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

	public:
		static void Draw(Settings::Style& a_style, Settings::Config& a_config);
		static void Init();

	private:
		// NPC Spawning & Actions
		// static inline std::shared_ptr<std::vector<RE::FormID>> localRefIDs = nullptr;

		// Search Input Field.
		static inline BaseColumn::ID searchKey = BaseColumn::ID::Name;
		static inline char inputBuffer[256] = "";
		static inline char modListBuffer[256] = "";
		static inline std::string selectedMod = _TICON(ICON_RPG_WRENCH, "All Mods");
		static inline bool dirty = true;

		// Sorting & Filtering
		static inline const std::map<BaseColumn::ID, const char*> InputSearchMap = {
			{ BaseColumn::ID::Name, "Name" },
			{ BaseColumn::ID::EditorID, "Editor ID" },
			{ BaseColumn::ID::FormID, "Form ID" }
		};

		static inline DescriptionFrameworkAPI::IDescriptionFrameworkInterface001* g_DescriptionFrameworkInterface = nullptr;
	};
}