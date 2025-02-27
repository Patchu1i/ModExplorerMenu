#pragma once

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
	class ObjectWindow : private ISortable, private ISearch
	{
	public:
		static inline ObjectWindow* GetSingleton()
		{
			static ObjectWindow singleton;
			return std::addressof(singleton);
		}

		ObjectWindow() : tableView(TableView<ObjectData>()) {}
		~ObjectWindow() = default;

		void 						Draw(float a_offset);
		void 						ShowActions();
		void 						Init(bool is_default);
		void						Unload();
		void						Load();
		void 						Refresh() {tableView.Refresh(); }
		TableView<ObjectData>&		GetTableView() { return tableView; }

	private:
		// Local State Variables.
		bool 						b_ClickToPlace;
		int 						clickToPlaceCount;

		// Menu State Variables.
		enum class Viewport
		{
			TableView = 0,
			BlacklistView,
			Count
			// Settings View
		};

		Viewport 					activeViewport;

		// TableView implementation
		TableView<ObjectData>		tableView;

	};
}