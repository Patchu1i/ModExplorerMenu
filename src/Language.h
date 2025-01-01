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
	public:
		enum class Locale
		{
			PortugueseBR,
			Chinese,
			English,
			French,
			German,
			Japanese,
			Korean,
			Russian,
			Spanish
		};

		static const std::vector<std::string> GetSupportedLanguageList()
		{
			return {
				"Portuguese-BR",
				"Chinese",
				"English",
				"French",
				"German",
				"Japanese",
				"Korean",
				"Russian",
				"Spanish"
			};
		}

		static const ImWchar* GetLanguageGlyphRange(Locale a_language)
		{
			switch (a_language) {
			case Locale::Chinese:
				return ImGui::GetIO().Fonts->GetGlyphRangesChineseFull();
			case Locale::Japanese:
				return ImGui::GetIO().Fonts->GetGlyphRangesJapanese();
			case Locale::Russian:
				return ImGui::GetIO().Fonts->GetGlyphRangesCyrillic();
			case Locale::Korean:
				return ImGui::GetIO().Fonts->GetGlyphRangesKorean();
			default:
				return ImGui::GetIO().Fonts->GetGlyphRangesDefault();
			}
		}

		[[nodiscard]] static std::string GetLanguageName(Locale a_language)
		{
			switch (a_language) {
			case Locale::PortugueseBR:
				return "Portuguese-BR";
			case Locale::Chinese:
				return "Chinese";
			case Locale::English:
				return "English";
			case Locale::French:
				return "French";
			case Locale::German:
				return "German";
			case Locale::Japanese:
				return "Japanese";
			case Locale::Korean:
				return "Korean";
			case Locale::Russian:
				return "Russian";
			case Locale::Spanish:
				return "Spanish";
			default:
				return "English";
			}
		}

		[[nodiscard]] static Locale GetLanguage(std::string a_language)
		{
			if (a_language == "Portuguese-BR") {
				return Locale::PortugueseBR;
			} else if (a_language == "Chinese") {
				return Locale::Chinese;
			} else if (a_language == "English") {
				return Locale::English;
			} else if (a_language == "French") {
				return Locale::French;
			} else if (a_language == "German") {
				return Locale::German;
			} else if (a_language == "Japanese") {
				return Locale::Japanese;
			} else if (a_language == "Korean") {
				return Locale::Korean;
			} else if (a_language == "Russian") {
				return Locale::Russian;
			} else if (a_language == "Spanish") {
				return Locale::Spanish;
			} else {
				return Locale::English;
			}
		}
	};
}