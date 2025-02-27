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
	class NPCWindow : private ISortable, private ISearch
	{
	public:
		static inline NPCWindow* GetSingleton()
		{
			static NPCWindow singleton;
			return std::addressof(singleton);
		}

		NPCWindow() : tableView(TableView<NPCData>()) {}
		~NPCWindow() = default;

		void 					Draw(float a_offset);
		void 					ShowActions();
		void 					Init(bool is_default);
		void					Unload();
		void					Load();
		void 					Refresh() {tableView.Refresh(); }
		TableView<NPCData>&		GetTableView() { return tableView; }

	private:

		// Behavior State
		bool 					b_ClickToPlace;
		int 					clickToPlaceCount;

		// Menu State Variables.
		enum class Viewport
		{
			TableView = 0,
			BlacklistView,
			Count
		};

		Viewport 				activeViewport;

		// TableView implementation
		TableView<NPCData>		tableView;

	};
}