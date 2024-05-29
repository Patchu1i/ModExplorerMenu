#pragma once

#include "lib/Graphic.h"
#include <PCH.h>

class Frame
{
public:
	static void Draw();
	static void Install();

	static inline std::atomic_bool _init = false;
	//static inline ID3D11ShaderResourceView* header_texture;
	static inline GraphicManager::Image header_texture;

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