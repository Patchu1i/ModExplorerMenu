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
			return std::addressof(singleton);
		}

		// AddItemWindow() = default;
		AddItemWindow() :
			tableView(TableView<ItemData>()),
			kitTableView(TableView<ItemData>())
		{}

		~AddItemWindow() = default;

		void 					Draw(float a_offset);
		void					Refresh() { tableView.Refresh(); }
		void 					Init(bool is_default);
		void					Unload();
		void					Load();
		TableView<ItemData>&	GetTableView() { return tableView; }

	private:
		void 					ShowActions();
		void					ShowKitBar();

		// Behavior State
		bool 					b_AddToInventory;
		bool 					b_PlaceOnGround;
		bool					b_AddToKit;
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

		void						AddKitToInventory(const Kit& a_kit);
		void						PlaceKitAtMe(const Kit& a_kit);
		void						CreateKitFromEquipped();


		TableView<ItemData>			tableView;
		TableView<ItemData>			kitTableView;
	};
}