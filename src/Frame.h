#pragma once

#include "lib/Graphic.h"
#include <PCH.h>

class Frame
{
public:
	static void Draw(bool is_settings_popped = false);
	static void Install();
	static void RefreshStyle();

	static inline std::atomic_bool _init = false;
	//static inline ID3D11ShaderResourceView* header_texture;
	static inline GraphicManager::Image header_texture;

	static inline ImFont* text_font;
	static inline ImFont* header_font;
	static inline ImFont* sidebar_font;
	static inline ImFont* button_font;

	enum ActiveWindow
	{
		Home,
		AddItem,
		Lookup,
		NPC,
		Teleport,
		Settings
	};

	static ActiveWindow _activeWindow;
};