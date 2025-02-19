#include "include/L/Language.h"
#include "include/S/Settings.h"

namespace Modex
{

	// Convert wide string to UTF-8 string.
	std::string WideToUTF8(const std::wstring& a_wide)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.to_bytes(a_wide);
	}

	void FontManager::BuildFontLibrary()
	{
		// Note to self: do not remove this fucking warning.
		if (std::filesystem::exists(font_path) == false) {
			stl::report_and_fail("FATAL ERROR: Font and/or Graphic asset directory not found. This is because Modex cannot locate the path 'Data/Interface/Modex/Fonts/'. Check your installation.");
			return;
		}

		// Search for fonts relative to Modex directory.
		for (const auto& entry : std::filesystem::directory_iterator(font_path)) {
			if (entry.path().filename().extension() != ".ttf" && entry.path().filename().extension() != ".otf") {
				continue;  // Pass invalid file types
			}

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

				logger::info("Font Index Names: {}", data.name);
			}
		}
	}

	// Subtracting -1 from size results in odd font sizes. Good idea?
	void FontManager::MergeIconFont(ImGuiIO& io, float size)
	{
		ImFontConfig imFontConfig;
		imFontConfig.MergeMode = true;
		imFontConfig.GlyphMinAdvanceX = 10.0f;
		imFontConfig.GlyphExtraSpacing.x = 5.0f;
		imFontConfig.GlyphOffset.y = (size / 2.0f) / 1.5f;
		// imFontConfig.GlyphOffset.y = size / 6.0f;
		// imFontConfig.GlyphOffset.x = 1.0f;

		static const ImWchar icon_ranges[] = { ICON_MIN_LC, ICON_MAX_LC, 0 };
		io.Fonts->AddFontFromFileTTF("Data/Interface/Modex/icons/lucide.ttf", size + 3.0f, &imFontConfig, icon_ranges);
	}

	void FontManager::AddDefaultFont()
	{
		// auto& style = Settings::GetSingleton()->GetStyle();
		auto& config = Settings::GetSingleton()->GetConfig();
		auto& io = ImGui::GetIO();

		ImFontConfig imFontConfig;
		imFontConfig.SizePixels = (float)config.globalFontSize;

		auto glyphRange = Language::GetUserGlyphRange(config.glyphRange);

		switch (config.glyphRange) {
		case Language::GlyphRanges::ChineseFull:
		case Language::GlyphRanges::ChineseSimplified:
			io.Fonts->AddFontFromFileTTF(chinese_font, imFontConfig.SizePixels, NULL, glyphRange);
			break;
		case Language::GlyphRanges::Japanese:
			io.Fonts->AddFontFromFileTTF(japanese_font, imFontConfig.SizePixels, NULL, glyphRange);
			break;
		case Language::GlyphRanges::Korean:
			io.Fonts->AddFontFromFileTTF(korean_font, imFontConfig.SizePixels, NULL, glyphRange);
			break;
		case Language::GlyphRanges::Cyrillic:
			io.Fonts->AddFontFromFileTTF(russian_font, imFontConfig.SizePixels, NULL, glyphRange);
			break;
		default:  // English / Latin; Greek; Cyrillic; Thai; Vietnamese (?)
			io.Fonts->AddFontDefault(&imFontConfig);
			break;
		}

		MergeIconFont(io, imFontConfig.SizePixels);
	}

	// Do not call this function directly. See Menu::RebuildFontAtlas().
	// Builds a single font from file with appropriate glyph range.
	void FontManager::LoadCustomFont(FontData& a_font, float a_size)
	{
		auto& io = ImGui::GetIO();
		auto& config = Settings::GetSingleton()->GetConfig();
		// auto& style = Settings::GetSingleton()->GetStyle();

		const auto& glyphRange = Language::GetUserGlyphRange(config.glyphRange);

		io.Fonts->AddFontFromFileTTF(a_font.fullPath.c_str(), a_size, NULL, glyphRange);

		MergeIconFont(io, a_size);
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
			LoadCustomFont(fontData, (float)config.globalFontSize);

		} else {
			logger::info("[Font Manager] No Custom font specified. Loading default font.");
			AddDefaultFont();
		}

		// io.FontDefault = io.Fonts->Fonts[0];
	}

}