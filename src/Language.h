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
			Default,
			Chinese,
			Japanese,
			Russian,
			Korean
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

					logger::info("Added language to master list: {}", path);
				}
			}
		}

		static const ImWchar* GetLanguageGlyphRange(GlyphRanges a_range)
		{
			switch (a_range) {
			case GlyphRanges::Chinese:
				return ImGui::GetIO().Fonts->GetGlyphRangesChineseFull();
			case GlyphRanges::Japanese:
				return ImGui::GetIO().Fonts->GetGlyphRangesJapanese();
			case GlyphRanges::Russian:
				return ImGui::GetIO().Fonts->GetGlyphRangesCyrillic();
			case GlyphRanges::Korean:
				return ImGui::GetIO().Fonts->GetGlyphRangesKorean();
			default:
				return ImGui::GetIO().Fonts->GetGlyphRangesDefault();
			}
		}

		[[nodiscard]] static GlyphRanges GetGlyphRange(std::string a_language)
		{
			if (a_language == "Chinese") {
				return GlyphRanges::Chinese;
			} else if (a_language == "Japanese") {
				return GlyphRanges::Japanese;
			} else if (a_language == "Russian") {
				return GlyphRanges::Russian;
			} else if (a_language == "Korean") {
				return GlyphRanges::Korean;
			} else {
				return GlyphRanges::Default;
			}
		}

		[[nodiscard]] static std::string GetGlyphName(GlyphRanges a_range)
		{
			switch (a_range) {
			case GlyphRanges::Chinese:
				return "Chinese";
			case GlyphRanges::Japanese:
				return "Japanese";
			case GlyphRanges::Russian:
				return "Russian";
			case GlyphRanges::Korean:
				return "Korean";
			default:
				return "Default";
			}
		}

		// Not going to bother making this fancy.
		[[nodiscard]] static std::set<std::string> GetListOfGlyphNames()
		{
			return { "Default", "Chinese", "Japanese", "Russian", "Korean" };
		}
	};
}