#pragma once

#include "d3d11.h"
#include "include/I/IconRpgAwesome.h"
#include "include/L/Language.h"

// clang-format off

namespace Modex
{
	// D3D11 Implementation Sourced from dTry's Wheeler and Llama's Tiny Hud. Adapted to NG.

	class GraphicManager
	{
	private:
	public:
		using D3D11View = ID3D11ShaderResourceView*;

		struct Image
		{
			D3D11View 					texture = nullptr;
			int32_t 					width = 0;
			int32_t 					height = 0;
		};

		static inline std::map<std::string, GraphicManager::Image> image_library;
		static inline std::map<std::string, GraphicManager::Image> imgui_library;

		static void 				Init();
		static void 				DrawImage(Image& a_texture, ImVec2 a_center);
		static void 				LoadImagesFromFilepath(std::string a_path, std::map<std::string, Image>& out_struct);

		[[nodiscard]] static bool 	GetD3D11Texture(const char* filename, D3D11View* out_srv, int& out_width, int& out_height);

		[[nodiscard]] static Image 	GetImage(std::string a_name)
		{
			auto found = image_library.find(a_name);
			if (found != image_library.end()) {
				return found->second;
			}

			return Image();
		}

		// Note to self: return image_library[key] adds key value if not valid.
		[[nodiscard]] static std::string GetImageName(Image a_image)
		{
			for (const auto& [key, value] : image_library) {
				if (value.texture == a_image.texture) {
					return key;
				}
			}

			return "None";
		}
	};
}