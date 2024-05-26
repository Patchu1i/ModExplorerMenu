#pragma once

#include "d3d11.h"

// Sourced from dTry's Wheeler and Llama's Tiny Hud. Adapted to NG.
// Huge gratitude for their public works.

class GraphicManager
{
private:
public:
	struct Image
	{
		ID3D11ShaderResourceView* texture = nullptr;
		int32_t width = 0;
		int32_t height = 0;
	};

	static std::map<std::string, GraphicManager::Image> image_library;
	static std::map<std::string, ImFont*> font_library;

	static bool GetD3D11Texture(const char* filename, ID3D11ShaderResourceView** out_srv, int& out_width,
		int& out_height);

	static void LoadImagesFromFilepath(std::string a_path, std::map<std::string, Image>& out_struct);

	static void LoadFontsFromDirectory(std::string a_path, std::map<std::string, ImFont*>& out_struct);

	static void Init();

	static inline std::atomic_bool initialized = false;

	[[nodiscard]] static ImFont* GetFont(const char* a_size)
	{
		return font_library[std::string(a_size)];
	}

	[[nodiscard]] static Image GetImage(const char* a_name)
	{
		return image_library[std::string(a_name)];
	}
};