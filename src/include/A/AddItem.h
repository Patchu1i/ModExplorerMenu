#pragma once

#include "extern/DescriptionFrameworkAPI.h"
#include "include/C/Columns.h"
#include "include/F/Frame.h"
#include "include/G/Graphic.h"
#include "include/I/ISearch.h"
#include "include/I/ISortable.h"
#include "include/I/InputManager.h"
#include "include/T/Table.h"

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

		// AddItemWindow() = default;
		AddItemWindow() :
			tableView(TableView<ItemData>()),
			kitTableView(TableView<ItemData>())
		{}

		~AddItemWindow() = default;

		void 					Draw(float a_offset);
		void					Refresh() { tableView.Refresh(); }
		void 					ShowBookPreview();
		void 					Init();
		TableView<ItemData>&	GetTableView() { return tableView; }

	private:
		void 					ShowActions();
		void					ShowKitBar();
		void					ProcessData();

		// Book Popup Window State.
		ItemData* 				openBook;

		// Local State Variables.
		bool 					b_AddToInventory;
		bool 					b_PlaceOnGround;
		bool					b_AddToKit;

		// TODO: This isn't really used inside the new TableView
		int 					clickToAddCount;

		// Menu State Variables.
		enum class Viewport
		{
			TableView = 0,
			BlacklistView,
			KitView,
			Count
		};

		Viewport 				activeViewport;

		// Kit Implementation (WIP)
		char					kitSearchBuffer[256];
		char					newKitName[256];
		std::string				selectedKit;

		void						RemoveItemFromKit(const std::shared_ptr<KitItem>& a_item);
		void						AddItemToKit(const std::shared_ptr<KitItem>& a_item);
		std::shared_ptr<KitItem>	CreateKitItem(const ItemData* a_item);
		void						AddKitToInventory(const Kit& a_kit);
		void						PlaceKitAtMe(const Kit& a_kit);


		TableView<ItemData>			tableView;
		TableView<ItemData>			kitTableView;
	};
}