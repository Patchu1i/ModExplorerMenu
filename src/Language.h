#pragma once
#include "PCH.h"
#include "Utils/IconRpgAwesome.h"

namespace ModExplorerMenu
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

		void LoadLanguage(std::string a_path)
		{
			if (!lang.empty()) {
				lang.clear();
			}

			std::ifstream file(std::wstring(json_lang_path) + std::wstring(a_path.begin(), a_path.end()) + L".json");
			if (!file.is_open()) {
				stl::report_and_fail("[JSON] Unable to open language file for reading.");
			}

			nlohmann::json json;
			file >> json;
			file.close();

			for (auto& [key, value] : json.items()) {
				lang[key] = value;
			}

			logger::info("Loaded language: {}", a_path);
		}

		const char* GetTranslation(const std::string& key) const
		{
			auto it = lang.find(key);
			if (it != lang.end()) {
				return it->second.c_str();
			} else {
				logger::info("[Translation Error] Key not found: {}", key);
				return key.c_str();  // Return the key itself as a fallback
			}
		}

	private:
		constexpr inline static const wchar_t* json_lang_path = L"Data/Interface/ModExplorerMenu/User/Language/";

		Translate() = default;
		~Translate() = default;
		Translate(const Translate&) = delete;
		Translate& operator=(const Translate&) = delete;
	};

#define _T(key) Translate::GetSingleton()->GetTranslation(key)
#define _TFM(key, suffix) (std::string(_T(key)) + suffix).c_str()
#define _TICON(icon, key) (std::string(icon) + _T(key)).c_str()
#define _TICONM(icon, key, suffix) (std::string(icon) + _T(key) + suffix).c_str()

	class Language
	{
	public:
		enum class Locale
		{
			BR_Portuguese,
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
				"BR-Portuguese",
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
			case Locale::BR_Portuguese:
				return "BR-Portuguese";
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
			if (a_language == "BR-Portuguese") {
				return Locale::BR_Portuguese;
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