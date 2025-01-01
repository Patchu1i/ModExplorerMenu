#pragma once

#include "Language.h"
#include "Utils/IconRpgAwesome.h"
#include "d3d11.h"

// Sourced from dTry's Wheeler and Llama's Tiny Hud. Adapted to NG.
// Huge gratitude for their public works.

namespace Modex
{
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

		struct Font
		{
			std::string name = "Default";
			ImFont* normal = nullptr;
			ImFont* large = nullptr;

			bool operator==(const Font& other) const
			{
				return name.compare(other.name) == 0;
			}
		};

		enum FontSize
		{
			Nano,
			Tiny,
			Medium,
			Large
		};

		static inline std::map<std::string, GraphicManager::Image> image_library;
		static inline std::map<std::string, GraphicManager::Image> imgui_library;
		static inline std::map<std::string, Font> font_library;
		static inline std::map<std::string, Font> icon_library;

		static void DrawImage(Image& a_texture, ImVec2 a_center);
		static void LoadImagesFromFilepath(std::string a_path, std::map<std::string, Image>& out_struct);
		static void LoadFontsFromDirectory(std::string a_path, std::map<std::string, Font>& out_struct, Language::Locale a_language);
		static void SetupLanguageFont(Language::Locale a_language);

		static void Init();

		[[nodiscard]] static bool GetD3D11Texture(const char* filename, ID3D11ShaderResourceView** out_srv, int& out_width,
			int& out_height);

		[[nodiscard]] static Font GetFont(std::string a_font)
		{
			auto found = font_library.find(a_font);
			if (found != font_library.end()) {
				return found->second;
			}

			logger::warn("Font Reference not found: {}", a_font);
			return Font();
		}

		[[nodiscard]] static Image GetImage(std::string a_name)
		{
			auto found = image_library.find(a_name);
			if (found != image_library.end()) {
				return found->second;
			}

			return Image();
		}

		[[nodiscard]] static std::string GetImageName(Image a_image)
		{
			// Note to self: return image_library[key] adds key value if not valid.
			for (const auto& [key, value] : image_library) {
				if (value.texture == a_image.texture) {
					return key;
				}
			}

			return "None";
		}
	};
}