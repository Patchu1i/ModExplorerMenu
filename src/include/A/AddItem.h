#pragma once

#include "extern/DescriptionFrameworkAPI.h"
#include "include/C/Columns.h"
#include "include/F/Frame.h"
#include "include/G/Graphic.h"
#include "include/I/ISearch.h"
#include "include/I/ISortable.h"
#include "include/I/InputManager.h"

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
		void					ShowKitBar();
		void					ShowKitTable();
		void					ShowKitPluginView();
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
		bool					b_AddToKit;
		int 					clickToAddCount;

		RE::FormType 			primaryFilter;
		std::string 			secondaryFilter;
		AddItemColumns 			columnList;
		std::vector<ItemData*> 	itemList;

		std::unordered_set<ItemData*> 	itemSelectionList;
		ItemData*						lastItemSelected;

		// Menu State Variables.
		enum class Viewport
		{
			TableView = 0,
			BlacklistView,
			KitView,
			Count
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

		// ISearch Plugin Interface
		char 					modSearchBuffer[256];
		std::string 			selectedMod;

		// Kit Implementation (WIP)
		char					kitSearchBuffer[256];
		char					newKitName[256];
		std::string				selectedKit;
		std::unordered_set<std::shared_ptr<KitItem>> kitSelectionList;

		enum class DRAG_TARGET
		{
			RemoveFromKit,
			ReplaceSearch,
			AddToKit
		};

		bool						showKitsForPlugin;
		Kit*						kitHoveredInTableView; // gross
		std::shared_ptr<KitItem>	kitPreview;
		std::unordered_set<Kit*> 	kitsFound;
		KitColumns 					kitColumnList;
		bool						wasMouseInBounds; // InputManager?
		bool						wasMouseInBoundsKit;
		void						ShowDragTarget(DRAG_TARGET a_target);
		void						RemoveItemFromKit(const std::shared_ptr<KitItem>& a_item);
		void						AddItemToKit(const std::shared_ptr<KitItem>& a_item);
		void						ShowKitItemContext(const std::shared_ptr<KitItem>& a_item);
		std::shared_ptr<KitItem>	CreateKitItem(ItemData* a_item);
		void						AddKitToInventory(const Kit& a_kit);
		void						PlaceKitAtMe(const Kit& a_kit);
	};
}