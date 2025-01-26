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
	class AddItemWindow : private ISortable, private ISearch
	{
	public:
		static inline AddItemWindow* GetSingleton()
		{
			static AddItemWindow singleton;
			return &singleton;
		}

		AddItemWindow() = default;
		~AddItemWindow() = default;

		void 					Draw(float a_offset);
		void 					ShowBookPreview();
		void 					Refresh();
		void 					Init();

	private:
		void 					ApplyFilters();
		void 					ShowFormTable();
		void 					ShowActions();
		void 					ShowSearch();
		void 					ShowItemListContextMenu(ItemData& a_item);

		// Primary Filter List
		std::vector<RE::FormType> filterList = {
			{ RE::FormType::Armor },
			{ RE::FormType::AlchemyItem },
			{ RE::FormType::Ammo },
			{ RE::FormType::Book },
			{ RE::FormType::Ingredient },
			{ RE::FormType::KeyMaster },
			{ RE::FormType::Misc },
			{ RE::FormType::Weapon }
		};

		// Book Popup Window State.
		ItemData* 				openBook;
		ItemData* 				itemPreview;

		// Local State Variables.
		bool 					b_AddToInventory;
		bool 					b_PlaceOnGround;
		bool 					b_AddToFavorites;
		int 					clickToAddCount;

		RE::FormType 			primaryFilter;
		std::string 			secondaryFilter;
		AddItemColumns 			columnList;
		std::vector<ItemData*> 	itemList;


		// Mouse Draggin
		enum MOUSE_STATE
		{
			DRAG_IGNORE,
			DRAG_SELECT,
			DRAG_NONE
		};

		std::unordered_set<ItemData*> 	itemSelectionList;
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
		DescriptionFrameworkAPI::IDescriptionFrameworkInterface001* g_DescriptionFrameworkInterface;


		// Input Fuzzy Search
		BaseColumn::ID 			searchKey;
		char 					inputBuffer[256];
		bool 					dirty;

		const std::map<BaseColumn::ID, const char*> InputSearchMap = {
			{ BaseColumn::ID::Name, "Name" },
			{ BaseColumn::ID::EditorID, "Editor ID" },
			{ BaseColumn::ID::FormID, "Form ID" }
		};

		// ISearch Interface
		char 					modSearchBuffer[256];
		std::string 			selectedMod;
	};
}