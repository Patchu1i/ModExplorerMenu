#pragma once

#include "Settings.h"
#include <PCH.h>

namespace ModExplorerMenu
{
	class Frame
	{
	public:
		static void Draw(bool is_settings_popped = false);
		static void Install();
		static void RefreshStyle();

		static inline std::atomic_bool _init = false;

		static inline ImFont* text_font;
		static inline ImFont* header_font;
		static inline ImFont* sidebar_font;
		static inline ImFont* button_font;

		enum ActiveWindow
		{
			Home,
			AddItem,
			Object,
			NPC,
			Teleport,
			Settings
		};

		static inline bool b_Home = true;
		static inline bool b_AddItem = false;
		static inline bool b_Object = false;
		static inline bool b_NPC = false;
		static inline bool b_Settings = false;
		static inline bool b_Teleport = false;

		static ActiveWindow _activeWindow;
		static void ResetSelectable()
		{
			b_Home = (_activeWindow == Home);
			b_AddItem = (_activeWindow == AddItem);
			b_Object = (_activeWindow == Object);
			b_NPC = (_activeWindow == NPC);
			b_Settings = (_activeWindow == Settings);
			b_Teleport = (_activeWindow == Teleport);
		}
	};
}