#define STB_IMAGE_IMPLEMENTATION
#include "Libraries/stb_image.h"

#include "Graphic.h"
#include "Menu.h"

namespace ModExplorerMenu
{
	bool GraphicManager::GetD3D11Texture(const char* filename, ID3D11ShaderResourceView** out_srv, int& out_width,
		int& out_height)
	{
		auto menu = Menu::GetSingleton();
		auto device = menu->GetDevice();

		auto* render_manager = RE::BSGraphics::Renderer::GetSingleton();
		if (!render_manager) {
			logger::error("Cannot find render manager. Initialization failed."sv);
			return false;
		}

		// Load from disk into a buffer
		int image_width = 0;
		int image_height = 0;
		unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
		if (image_data == NULL) {
			logger::error("Failed to load image: {}", filename);
			return false;
		}

		// Create texture
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.Width = image_width;
		desc.Height = image_height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		ID3D11Texture2D* p_texture = nullptr;
		D3D11_SUBRESOURCE_DATA sub_resource;
		sub_resource.pSysMem = image_data;
		sub_resource.SysMemPitch = desc.Width * 4;
		sub_resource.SysMemSlicePitch = 0;

		device->CreateTexture2D(&desc, &sub_resource, &p_texture);

		// Create texture view
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
		ZeroMemory(&srv_desc, sizeof srv_desc);
		srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MipLevels = desc.MipLevels;
		srv_desc.Texture2D.MostDetailedMip = 0;

		device->CreateShaderResourceView(p_texture, &srv_desc, out_srv);
		p_texture->Release();

		out_width = image_width;
		out_height = image_height;
		stbi_image_free(image_data);

		return true;
	}

	void GraphicManager::LoadImagesFromFilepath(std::string a_path, std::map<std::string, Image>& out_struct)
	{
		if (std::filesystem::exists(a_path) == false) {
			auto warning = std::string("FATAL ERROR: Font and/or Graphic asset directory not found. This is because ModExplorerMenu cannot locate the path '") + a_path + "'. Check your installation.";
			stl::report_and_fail(warning);
			return;
		}

		for (const auto& entry : std::filesystem::directory_iterator(a_path)) {
			if (entry.path().filename().extension() != ".png") {
				continue;
			}

			auto index = entry.path().filename().stem().string();  // Get the filename without extension

			bool success = GraphicManager::GetD3D11Texture(entry.path().string().c_str(), &out_struct[index.c_str()].texture,
				out_struct[index.c_str()].width, out_struct[index.c_str()].height);

			if (!success) {
				logger::error("Failed to load image: {}", entry.path().string());
			}
		}
	}

	// Subtracting -1 from size results in odd font sizes. Good idea?
	void MergeIconFont(ImGuiIO& io, float size)
	{
		ImFontConfig config;
		config.MergeMode = true;
		config.GlyphMinAdvanceX = 10.0f;
		config.GlyphExtraSpacing.x = 5.0f;
		config.GlyphOffset.y = 1.0f;
		config.GlyphOffset.x = 1.0f;
		static const ImWchar icon_ranges[] = { ICON_RPG_MIN, ICON_RPG_MAX, 0 };
		io.Fonts->AddFontFromFileTTF("Data/Interface/ModExplorerMenu/fonts/icons/rpgawesome-webfont.ttf", size - 1.0f, &config, icon_ranges);
	}

	// TODO: Remove the nano, small, medium, etc indices and replace with a single font size parameter.
	// This will allow for easier font merging and more universal font usage. Only issue is that the font dpi and scale
	// might be a problem if changing UI size?
	void GraphicManager::LoadFontsFromDirectory(std::string a_path, std::map<std::string, Font>& out_struct, Language::Locale a_language)
	{
		if (std::filesystem::exists(a_path) == false) {
			auto warning = std::string("FATAL ERROR: Font and/or Graphic asset directory not found. This is because ModExplorerMenu cannot locate the path '") + a_path + "'. Check your installation.";
			stl::report_and_fail(warning);
			return;
		}

		auto glyph_ranges = Language::GetLanguageGlyphRange(a_language);

		for (const auto& entry : std::filesystem::directory_iterator(a_path)) {
			if (entry.path().filename().extension() != ".ttf" && entry.path().filename().extension() != ".otf") {
				continue;  // Pass invalid file types
			}

			auto index = entry.path().filename().stem().string();

			ImGuiIO& io = ImGui::GetIO();
			out_struct[index].nano = io.Fonts->AddFontFromFileTTF(entry.path().string().c_str(), 16.0f, NULL, glyph_ranges);
			MergeIconFont(io, 16.0f);
			out_struct[index].tiny = io.Fonts->AddFontFromFileTTF(entry.path().string().c_str(), 18.0f, NULL, glyph_ranges);
			MergeIconFont(io, 18.0f);
			out_struct[index].medium = io.Fonts->AddFontFromFileTTF(entry.path().string().c_str(), 20.0f, NULL, glyph_ranges);
			MergeIconFont(io, 20.0f);
			out_struct[index].large = io.Fonts->AddFontFromFileTTF(entry.path().string().c_str(), 24.0f, NULL, glyph_ranges);
			MergeIconFont(io, 24.0f);
		}
	}

	void GraphicManager::SetupLanguageFont(Language::Locale a_language)
	{
		auto glyph_range = Language::GetLanguageGlyphRange(a_language);
		ImGuiIO& io = ImGui::GetIO();

		switch (a_language) {
		case Language::Locale::Chinese:
			font_library["Default"].nano = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simsun.ttc", 16.0f, NULL, glyph_range);
			MergeIconFont(io, 16.0f);
			font_library["Default"].tiny = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simsun.ttc", 18.0f, NULL, glyph_range);
			MergeIconFont(io, 18.0f);
			font_library["Default"].medium = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simsun.ttc", 20.0f, NULL, glyph_range);
			MergeIconFont(io, 20.0f);
			font_library["Default"].large = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simsun.ttc", 24.0f, NULL, glyph_range);
			MergeIconFont(io, 24.0f);
			break;
		case Language::Locale::Japanese:
			font_library["Default"].nano = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 16.0f, NULL, glyph_range);
			MergeIconFont(io, 16.0f);
			font_library["Default"].tiny = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 18.0f, NULL, glyph_range);
			MergeIconFont(io, 18.0f);
			font_library["Default"].medium = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 20.0f, NULL, glyph_range);
			MergeIconFont(io, 20.0f);
			font_library["Default"].large = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 24.0f, NULL, glyph_range);
			MergeIconFont(io, 24.0f);
			break;
		case Language::Locale::Korean:
			font_library["Default"].nano = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\malgun.ttf", 16.0f, NULL, glyph_range);
			MergeIconFont(io, 16.0f);
			font_library["Default"].tiny = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\malgun.ttf", 18.0f, NULL, glyph_range);
			MergeIconFont(io, 18.0f);
			font_library["Default"].medium = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\malgun.ttf", 20.0f, NULL, glyph_range);
			MergeIconFont(io, 20.0f);
			font_library["Default"].large = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\malgun.ttf", 24.0f, NULL, glyph_range);
			MergeIconFont(io, 24.0f);
			break;
		case Language::Locale::Russian:
			font_library["Default"].nano = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\arial.ttf", 16.0f, NULL, glyph_range);
			MergeIconFont(io, 16.0f);
			font_library["Default"].tiny = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\arial.ttf", 18.0f, NULL, glyph_range);
			MergeIconFont(io, 18.0f);
			font_library["Default"].medium = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\arial.ttf", 20.0f, NULL, glyph_range);
			MergeIconFont(io, 20.0f);
			font_library["Default"].large = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\arial.ttf", 24.0f, NULL, glyph_range);
			MergeIconFont(io, 24.0f);
			break;
		default:
			font_library["Default"].nano = io.Fonts->AddFontDefault();
			MergeIconFont(io, 16.0f);
			font_library["Default"].tiny = io.Fonts->AddFontDefault();
			MergeIconFont(io, 18.0f);
			font_library["Default"].medium = io.Fonts->AddFontDefault();
			MergeIconFont(io, 20.0f);
			font_library["Default"].large = io.Fonts->AddFontDefault();
			MergeIconFont(io, 24.0f);
		}
	}

	void GraphicManager::DrawImage(Image& a_image, ImVec2 a_center)
	{
		auto scale = ImGui::GetIO().DisplaySize.y / 1080.0f;

		auto width = a_image.width * scale;
		auto height = a_image.height * scale;
		auto texture = a_image.texture;

		const ImVec2 pos[4] = {
			ImVec2(a_center.x - width / 2, a_center.y - height / 2),
			ImVec2(a_center.x + width / 2, a_center.y - height / 2),
			ImVec2(a_center.x + width / 2, a_center.y + height / 2),
			ImVec2(a_center.x - width / 2, a_center.y + height / 2)
		};

		const ImVec2 uv[4] = {
			ImVec2(0.0f, 0.0f),
			ImVec2(1.0f, 0.0f),
			ImVec2(1.0f, 1.0f),
			ImVec2(0.0f, 1.0f)
		};

		ImGui::GetBackgroundDrawList()->AddImageQuad(texture, pos[0], pos[1], pos[2], pos[3], uv[0], uv[1], uv[2], uv[3]);
	}

	void GraphicManager::Init()
	{
		image_library["None"] = Image();
		GraphicManager::LoadImagesFromFilepath(std::string("Data/Interface/ModExplorerMenu/images"), GraphicManager::image_library);

		// Set the default font to what the user has selected in the settings.
		// For non-english users, this will replace the ImGui default font.

		auto config = Settings::GetSingleton()->GetConfig();
		SetupLanguageFont(config.language);

		// I wrapped this into a switch statement so that all directories don't have to be present.
		// In addition, it allows users of varying languages to only need one directory for custom fonts.
		// For example, a Chinese user would install their custom fonts into the Chinese directory.
		// These directories are automatically setup by the FOMOD.

		switch (config.language) {
		case Language::Locale::Chinese:
			GraphicManager::LoadFontsFromDirectory(std::string("Data/Interface/ModExplorerMenu/fonts/chinese"), GraphicManager::font_library, Language::Locale::Chinese);
			break;
		case Language::Locale::Japanese:
			GraphicManager::LoadFontsFromDirectory(std::string("Data/Interface/ModExplorerMenu/fonts/japanese"), GraphicManager::font_library, Language::Locale::Japanese);
			break;
		case Language::Locale::Korean:
			GraphicManager::LoadFontsFromDirectory(std::string("Data/Interface/ModExplorerMenu/fonts/korean"), GraphicManager::font_library, Language::Locale::Korean);
			break;
		case Language::Locale::Russian:
			GraphicManager::LoadFontsFromDirectory(std::string("Data/Interface/ModExplorerMenu/fonts/russian"), GraphicManager::font_library, Language::Locale::Russian);
			break;
		default:
			GraphicManager::LoadFontsFromDirectory(std::string("Data/Interface/ModExplorerMenu/fonts/english"), GraphicManager::font_library, Language::Locale::English);
			break;
		}

		// Detect ImGui Icons mod and load it if it exists.
		if (std::filesystem::exists("Data/Interface/ImGuiIcons")) {
			GraphicManager::LoadImagesFromFilepath(std::string("Data/Interface/ImGuiIcons/Icons"), GraphicManager::imgui_library);
			GraphicManager::LoadFontsFromDirectory(std::string("Data/Interface/ImGuiIcons/Fonts"), GraphicManager::font_library, Language::Locale::English);
			logger::info("Successfully found and loaded ImGui Icons.");
		}

		// Queue other assets to load.
		Menu::initialized.store(true);
	}
}