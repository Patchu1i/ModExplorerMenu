#pragma once
#include "PCH.h"
#include "Utils/IconRpgAwesome.h"

namespace Modex
{
	class Translate
	{
	public:
		std::unordered_map<std::string, std::string> lang;

		static inline Translate* GetSingleton()
		{
			static Translate singleton;
			return &singleton;
		}

		void RefreshLanguage(std::string a_language)
		{
			lang.clear();
			LoadLanguage(a_language);
		}

		// TODO: Marked as duplicate, can be refactored in with PersistentData
		void LoadLanguage(std::string a_path)
		{
			if (!lang.empty()) {
				lang.clear();
			}

			std::ifstream file(std::wstring(json_lang_path) + std::wstring(a_path.begin(), a_path.end()) + L".json");
			if (!file.is_open()) {
				return;
			}

			// If the file is empty, don't bother parsing it.
			file.seekg(0, std::ios::end);
			if (file.tellg() == 0) {
				file.close();
				return;
			}

			// Reset pointer to beginning.
			file.seekg(0, std::ios::beg);

			try {
				nlohmann::json json;
				file >> json;
				file.close();

				for (auto& [key, value] : json.items()) {
					lang[key] = value;
				}

				logger::info("[Translation] Loaded language: {}", a_path);
			} catch (const nlohmann::json::parse_error& e) {
				file.close();
				stl::report_and_fail(std::string("[JSON] Error parsing language file:\n\nValidate your JSON formatting, and try again.\n") + e.what());
			} catch (const nlohmann::json::exception& e) {
				file.close();
				stl::report_and_fail(std::string("[JSON] Error Exception reading language file: ") + e.what());
			}
		}

		// Accessor func for translations. Intentionally return the translation key
		// if not found to offer a fallback for incorrect or missing translations.
		const char* GetTranslation(const std::string& key) const
		{
			auto it = lang.find(key);
			if (it != lang.end()) {
				return it->second.c_str();
			} else {
				return key.c_str();
			}
		}

	private:
		constexpr inline static const wchar_t* json_lang_path = L"Data/Interface/Modex/User/Language/";

		Translate() = default;
		~Translate() = default;
		Translate(const Translate&) = delete;
		Translate& operator=(const Translate&) = delete;
	};

#define _T(key) Translate::GetSingleton()->GetTranslation(key)
#define _TFM(key, suffix) (std::string(_T(key)) + suffix).c_str()
#define _TICON(icon, key) (Settings::GetSingleton()->GetStyle().noIconText ? _T(key) : (std::string(icon) + _T(key)).c_str())
#define _TICONM(icon, key, suffix) (Settings::GetSingleton()->GetStyle().noIconText ? _TFM(key, suffix) : (std::string(icon) + _T(key) + suffix).c_str())

	class Language
	{
	private:
		constexpr inline static const wchar_t* json_lang_path = L"Data/Interface/Modex/User/Language/";
		constexpr inline static const wchar_t* json_font_path = L"Data/Interface/Modex/User/Fonts/";

		Language() = default;
		~Language() = default;
		Language(const Language&) = delete;
		Language& operator=(const Language&) = delete;

	public:
		static inline Language* GetSingleton()
		{
			static Language singleton;
			return &singleton;
		}

		enum GlyphRanges
		{
			English,
			Greek,
			Korean,
			Japanese,
			ChineseFull,
			ChineseSimplified,
			Cyrillic,
			Thai,
			Vietnamese
		};

		static inline std::set<std::string> languages;

		static inline std::set<std::string> GetLanguages()
		{
			if (languages.empty()) {
				stl::report_and_fail("No languages found in the language directory. Unable to Load Languages.");
			}

			return languages;
		}

		static const void BuildLanguageList()
		{
			for (const auto& entry : std::filesystem::directory_iterator(std::wstring(json_lang_path))) {
				if (entry.path().extension() == L".json") {
					std::string path = entry.path().filename().string();
					languages.insert(path.substr(0, path.find_last_of('.')));

					logger::info("[Language] Added language to master list: {}", path);
				}
			}
		}

		static const ImWchar* GetUserGlyphRange(GlyphRanges a_range)
		{
			auto& io = ImGui::GetIO();

			switch (a_range) {
			case GlyphRanges::English:
				return io.Fonts->GetGlyphRangesDefault();  // Basic Latin, Extended Latin
			case GlyphRanges::Greek:
				return io.Fonts->GetGlyphRangesGreek();  // Default + Greek and Coptic
			case GlyphRanges::Korean:
				return io.Fonts->GetGlyphRangesKorean();  // Default + Korean characters
			case GlyphRanges::Japanese:
				return io.Fonts->GetGlyphRangesJapanese();  // Default + Hiragana, Katakana, Half-Width, Selection of 2999 Ideographs
			case GlyphRanges::ChineseFull:
				return io.Fonts->GetGlyphRangesChineseFull();  // Default + Half-Width + Japanese Hiragana/Katakana + full set of about 21000 CJK Unified Ideographs
			case GlyphRanges::ChineseSimplified:
				return io.Fonts->GetGlyphRangesChineseSimplifiedCommon();  // Default + Half-Width + Japanese Hiragana/Katakana + set of 2500 CJK Unified Ideographs for common simplified Chinese
			case GlyphRanges::Cyrillic:
				return io.Fonts->GetGlyphRangesCyrillic();  // Default + about 400 Cyrillic characters
			case GlyphRanges::Thai:
				return io.Fonts->GetGlyphRangesThai();  // Default + Thai characters
			case GlyphRanges::Vietnamese:
				return io.Fonts->GetGlyphRangesVietnamese();  // Default + Vietnamese characters
			default:
				return io.Fonts->GetGlyphRangesDefault();  // Basic Latin, Extended Latin
			};
		}

		[[nodiscard]] static GlyphRanges GetGlyphRange(std::string a_language)
		{
			if (a_language.compare("English") == 0) {
				return GlyphRanges::English;
			} else if (a_language.compare("Greek") == 0) {
				return GlyphRanges::Greek;
			} else if (a_language.compare("Korean") == 0) {
				return GlyphRanges::Korean;
			} else if (a_language.compare("Japanese") == 0) {
				return GlyphRanges::Japanese;
			} else if (a_language.compare("ChineseFull") == 0) {
				return GlyphRanges::ChineseFull;
			} else if (a_language.compare("ChineseSimplified") == 0) {
				return GlyphRanges::ChineseSimplified;
			} else if (a_language.compare("Cyrillic") == 0) {
				return GlyphRanges::Cyrillic;
			} else if (a_language.compare("Thai") == 0) {
				return GlyphRanges::Thai;
			} else if (a_language.compare("Vietnamese") == 0) {
				return GlyphRanges::Vietnamese;
			} else {
				return GlyphRanges::English;
			}
		}

		[[nodiscard]] static std::string GetGlyphName(GlyphRanges a_range)
		{
			switch (a_range) {
			case GlyphRanges::English:
				return "English";
			case GlyphRanges::Greek:
				return "Greek";
			case GlyphRanges::Korean:
				return "Korean";
			case GlyphRanges::Japanese:
				return "Japanese";
			case GlyphRanges::ChineseFull:
				return "ChineseFull";
			case GlyphRanges::ChineseSimplified:
				return "ChineseSimplified";
			case GlyphRanges::Cyrillic:
				return "Cyrillic";
			case GlyphRanges::Thai:
				return "Thai";
			case GlyphRanges::Vietnamese:
				return "Vietnamese";
			default:
				return "English";
			};
		}

		// Not going to bother making this fancy.
		[[nodiscard]] static std::set<std::string> GetListOfGlyphNames()
		{
			return { "English", "Greek", "Korean", "Japanese", "ChineseFull", "ChineseSimplified", "Cyrillic", "Thai", "Vietnamese" };
		}
	};

	class FontManager
	{
	public:
		enum class FontSize
		{
			Small,
			Medium,
			Large,
		};

		struct FontData
		{
			std::string name;
			std::string fullPath;
		};

		static inline FontManager* GetSingleton()
		{
			static FontManager singleton;
			return &singleton;
		}

		void LoadCustomFont(FontData& a_font, float a_size);
		void MergeIconFont(ImGuiIO& io, float size);
		void SetStartupFont();
		void BuildFontLibrary();

		// Returns std::map<std::string, FontData>
		[[nodiscard]] static inline FontData& GetFontData(const std::string& a_name)
		{
			return font_library[a_name];
		}

		// Returns a sorted list of registered fonts.
		[[nodiscard]] static inline std::vector<std::string> GetFontLibrary()
		{
			std::vector<std::string> fonts;
			for (const auto& [key, value] : font_library) {
				fonts.push_back(key);
			}

			std::sort(fonts.begin(), fonts.end());
			return fonts;
		}

		// members
		static inline std::map<std::string, FontData> font_library;

	private:
		inline static const wchar_t* font_path = L"Data/Interface/Modex/Fonts/";
		inline static const wchar_t* imgui_font_path = L"Data/Interface/ImGuiIcons/Fonts";

		inline static const char* chinese_font = "c:\\Windows\\Fonts\\simsun.ttc";
		inline static const char* japanese_font = "c:\\Windows\\Fonts\\msgothic.ttc";
		inline static const char* korean_font = "c:\\Windows\\Fonts\\malgun.ttf";
		inline static const char* russian_font = "c:\\Windows\\Fonts\\arial.ttf";

		void AddDefaultFont();

		FontManager() = default;
		~FontManager() = default;
		FontManager(const FontManager&) = delete;
		FontManager& operator=(const FontManager&) = delete;
	};
}