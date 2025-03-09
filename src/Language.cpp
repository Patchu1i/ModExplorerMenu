#include "include/L/Language.h"
#include "include/S/Settings.h"

namespace Modex
{

	// Convert wide string to UTF-8 string.
	// TODO: Replace this with the functions declared in Util.h
	std::string WideToUTF8(const std::wstring& a_wide)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		return converter.to_bytes(a_wide);
	}

	void FontManager::BuildFontLibrary()
	{
		// Note to self: do not remove this fucking warning.
		if (std::filesystem::exists(font_path) == false) {
			stl::report_and_fail("FATAL ERROR: Font and/or Graphic asset directory not found. This is because Modex cannot locate the path 'Data/Interface/Modex/Fonts/'. Check your installation.");
			return;
		}

		// Search for fonts relative to Modex mod directory.
		for (const auto& entry : std::filesystem::directory_iterator(font_path)) {
			if (entry.path().filename().extension() != ".ttf" && entry.path().filename().extension() != ".otf") {
				continue;  // Pass invalid file types
			}

			// Since we're reading from the filesystem, we interpret the path as a wide string
			// then convert it to a utf-8 string for storage. Since we do not want to store paths or
			// filenames using wide chars. This ensures utf-8 is used for all strings.

			FontData data;
			data.name = WideToUTF8(entry.path().filename().stem().wstring());
			data.fullPath = WideToUTF8(entry.path().wstring());

			font_library[data.name] = data;
		}

		// Search for fonts relative to ImGui Icon Library directory.
		if (std::filesystem::exists(imgui_font_path)) {
			for (const auto& entry : std::filesystem::directory_iterator(imgui_font_path)) {
				if (entry.path().filename().extension() != ".ttf" && entry.path().filename().extension() != ".otf") {
					continue;  // Pass invalid file types
				}

				FontData data;
				data.name = WideToUTF8(entry.path().filename().stem().wstring());
				data.fullPath = WideToUTF8(entry.path().wstring());

				font_library[data.name] = data;

				logger::info("[FontManager] Loaded and Registered Font: {}", data.name);
			}
		}
	}

	// Subtracting -1 from size results in odd font sizes. Good idea?
	void FontManager::MergeIconFont(ImGuiIO& io, float a_size)
	{
		ImFontConfig config;
		config.MergeMode = true;
		config.GlyphMinAdvanceX = 10.0f;
		config.GlyphExtraSpacing.x = 5.0f;
		config.SizePixels = a_size;
		config.GlyphOffset.y = (a_size / 2.0f) / 1.5f;

		static const ImWchar icon_ranges[] = { ICON_MIN_LC, ICON_MAX_LC, 0 };
		io.Fonts->AddFontFromFileTTF("Data/Interface/Modex/icons/lucide.ttf", a_size + 3.0f, &config, icon_ranges);
	}

	// TODO: Replace default font from ImGui proggy with a better font.
	void FontManager::AddDefaultFont()
	{
		auto& config = Settings::GetSingleton()->GetConfig();
		auto& io = ImGui::GetIO();

		float size = config.globalFontSize;

		auto glyphRange = Language::GetUserGlyphRange(config.glyphRange);

		switch (config.glyphRange) {
		case Language::GlyphRanges::ChineseFull:
		case Language::GlyphRanges::ChineseSimplified:
			io.Fonts->AddFontFromFileTTF(chinese_font, size, NULL, glyphRange);
			break;
		case Language::GlyphRanges::Japanese:
			io.Fonts->AddFontFromFileTTF(japanese_font, size, NULL, glyphRange);
			break;
		case Language::GlyphRanges::Korean:
			io.Fonts->AddFontFromFileTTF(korean_font, size, NULL, glyphRange);
			break;
		case Language::GlyphRanges::Cyrillic:
			io.Fonts->AddFontFromFileTTF(russian_font, size, NULL, glyphRange);
			break;
		default:  // English / Latin; Greek; Cyrillic; Thai; Vietnamese (?)
			io.Fonts->AddFontFromFileTTF(russian_font, size, NULL, glyphRange);
			//io.Fonts->AddFontDefault(&imFontConfig);
			break;
		}

		MergeIconFont(io, size);
	}

	// Do not call this function directly. See Menu::RebuildFontAtlas().
	// Builds a single font from file with appropriate glyph range.
	void FontManager::LoadCustomFont(FontData& a_font)
	{
		auto& io = ImGui::GetIO();
		auto& config = Settings::GetSingleton()->GetConfig();

		float size = config.globalFontSize;
		const auto& glyphRange = Language::GetUserGlyphRange(config.glyphRange);

		io.Fonts->AddFontFromFileTTF(a_font.fullPath.c_str(), size, NULL, glyphRange);

		MergeIconFont(io, size);
	}

	// Runs after Settings are loaded.
	void FontManager::SetStartupFont()
	{
		logger::info("[Font Manager] Setting startup font.");
		auto& config = Settings::GetSingleton()->GetConfig();
		auto fontData = GetFontData(config.globalFont);

		logger::info("[Font Manager] Global Font: {}", config.globalFont);
		logger::info("[Font Manager] Loading font: {}", fontData.fullPath);

		if (std::filesystem::exists(fontData.fullPath) == true) {
			logger::info("[Font Manager] Loading custom font: {}", fontData.name);
			LoadCustomFont(fontData);

		} else {
			logger::info("[Font Manager] No Custom font specified. Loading default font.");
			AddDefaultFont();
		}
	}

}