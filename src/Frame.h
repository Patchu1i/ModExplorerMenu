#pragma once

#	include <PCH.h>

class Frame
{
public:
	static void Draw();
	static void Install();

	static inline std::atomic_bool _init = false;
	static inline ID3D11ShaderResourceView* header_texture;

	enum ActiveWindow
	{
		Home,
		AddItem,
		Lookup,
		Teleport,
		Settings
	};

	static ActiveWindow _activeWindow;
};