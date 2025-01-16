#pragma once

#include "include/S/Settings.h"
#include <PCH.h>

// clang-format off

namespace Modex
{
	class Frame
	{
	public:
		static Frame* GetSingleton()
		{
			static Frame singleton;
			return &singleton;
		}

		Frame() = default;
		~Frame() = default;

		void 			Draw(bool is_settings_popped = false);
		void 			Install();
		void 			RefreshStyle();

		static const ImGuiTableFlags 	TABLE_FLAGS =
			ImGuiTableFlags_Reorderable 			| ImGuiTableFlags_Sortable |
			ImGuiTableFlags_Borders 				| ImGuiTableFlags_BordersOuterH |
			ImGuiTableFlags_Hideable 				| ImGuiTableFlags_BordersInnerH |
			ImGuiTableFlags_NoBordersInBody 		| ImGuiTableFlags_Resizable |
			ImGuiTableFlags_ScrollY 				| ImGuiTableFlags_SizingStretchProp;

		static const ImGuiTableFlags TELEPORT_FLAGS =
			ImGuiTableFlags_Reorderable 			| ImGuiTableFlags_RowBg |
			ImGuiTableFlags_Sortable 				| ImGuiTableFlags_Borders |
			ImGuiTableFlags_BordersOuterH 			| ImGuiTableFlags_Hideable |
			ImGuiTableFlags_BordersInnerH 			| ImGuiTableFlags_NoBordersInBody |
			ImGuiTableFlags_Resizable 				| ImGuiTableFlags_ScrollY;

		static const ImGuiInputTextFlags INPUT_FLAGS =
			ImGuiInputTextFlags_AutoSelectAll;

		static const ImGuiTableFlags ACTIONBAR_FLAGS =
			ImGuiTableFlags_Borders 				| ImGuiTableFlags_RowBg |
			ImGuiTableFlags_ScrollY;

		static const ImGuiWindowFlags BACKGROUND_FLAGS =
			ImGuiWindowFlags_NoTitleBar 			| ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove 				| ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoInputs 				| ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoBringToFrontOnFocus 	| ImGuiWindowFlags_NoNav;

	private:
		enum ActiveWindow
		{
			Home,
			AddItem,
			Object,
			NPC,
			Teleport,
			Settings
		};

		bool 			b_Home 		= true;
		bool 			b_AddItem 	= false;
		bool 			b_Object 	= false;
		bool 			b_NPC 		= false;
		bool 			b_Settings 	= false;
		bool 			b_Teleport 	= false;

		ActiveWindow 	activeWindow = ActiveWindow::Home;

		void ResetSelectable()
		{
			b_Home 		= (activeWindow == Home);
			b_AddItem 	= (activeWindow == AddItem);
			b_Object 	= (activeWindow == Object);
			b_NPC 		= (activeWindow == NPC);
			b_Settings 	= (activeWindow == Settings);
			b_Teleport 	= (activeWindow == Teleport);
		}
	};
}