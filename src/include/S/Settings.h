#pragma once

#include "include/G/Graphic.h"

// Current process of loading settings::
// Start by loading the user Config and getting the Theme string.
// Look for the theme string in the themes directory for a matching ini.
// (If it doesn't exist, load the default theme from def values in Style struct)
// Load the theme ini and apply the values to the user Style struct.

namespace Modex
{

	class Settings
	{
	public:
		// void LoadStyleTheme(ImGuiStyle a_theme); - DEPRECATED

		void GetIni(const wchar_t* a_path, const std::function<void(CSimpleIniA&)> a_func);
		void LoadSettings(const wchar_t* a_path);
		void LoadUserFontSetting();
		void SaveSettings();
		void LoadMasterIni(CSimpleIniA& a_ini);
		void LoadThemeFromIni(CSimpleIniA& a_ini);

		void LoadStyleTheme(ImGuiStyle a_theme);

		void CreateDefaultMaster();
		static void FormatMasterIni(CSimpleIniA& a_ini);
		static void FormatThemeIni(CSimpleIniA& a_ini);

		static inline Settings* GetSingleton()
		{
			static Settings singleton;
			return &singleton;
		}

		constexpr inline static const wchar_t* ini_theme_path = L"Data/Interface/Modex/themes/";
		constexpr inline static const wchar_t* ini_mem_path = L"Data/Interface/Modex/Modex.ini";

		enum SECTION
		{
			Window,
			Frame,
			Text,
			Table,
			Widgets,
			Images,
			Main,
			Modules,
			AddItem,
			Object,
			FormLookup,
			NPC,
			Teleport,
		};

		static inline std::map<SECTION, const char*> rSections = {
			{ Window, "Window" },
			{ Text, "Text" },
			{ Table, "Table" },
			{ Widgets, "Widgets" },
			{ Images, "Images & Icons" },
			{ Main, "Main" },
			{ Modules, "Modules" },
			{ AddItem, "AddItem" },
			{ Object, "Object" },
			{ FormLookup, "FormLookup" },
			{ NPC, "NPC" },
			{ Teleport, "Teleport" }
		};

		struct Config
		{
			// General
			std::string theme = "Default";
			int showMenuKey = 211;
			int showMenuModifier = 0;
			int modListSort = 0;  // 0 = Alphabetical, 1 = Load Order (ASC) 2 = Load Order (DESC)
			int uiScale = 100;    // 80, 90, 100, 110, 120 (Should be a slider..)
			bool fullscreen = false;
			bool pauseGame = false;

			// Font Stuff
			std::string language = "English";
			Language::GlyphRanges glyphRange = Language::GlyphRanges::English;
			std::string globalFont = "Default";
			int globalFontSize = 16;

			// Modules
			int defaultShow = 1;  // 0 = Home, 1 = AddItem, 2 = Object, 3 = NPC, 4 = Teleport, 5 = Settings
			bool showHomeMenu = false;
			bool showAddItemMenu = true;
			bool showObjectMenu = true;
			bool showNPCMenu = true;
			bool showTeleportMenu = true;

			std::string dataPath = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Skyrim Special Edition\\Data";

			// Hiden from User
			ImVec2 screenScaleRatio;
		};

		ImGuiStyle test;

		struct Style  // should extend ImGuiStyle (?)
		{
			ImVec4 text = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			ImVec4 textDisabled = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
			ImVec4 windowBg = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
			ImVec4 border = ImVec4(0.43f, 0.43f, 0.50f, 1.00f);
			ImVec4 scrollbarBg = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
			ImVec4 scrollbarGrab = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
			ImVec4 scrollbarGrabHovered = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
			ImVec4 scrollbarGrabActive = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
			ImVec4 checkMark = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			ImVec4 sliderGrab = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
			ImVec4 sliderGrabActive = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			ImVec4 widgetBg = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
			ImVec4 widgetHovered = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			ImVec4 widgetActive = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			ImVec4 button = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			ImVec4 buttonHovered = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
			ImVec4 buttonActive = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
			ImVec4 secondaryButton = ImVec4(0.07f, 0.71f, 0.36f, 0.75f);
			ImVec4 secondaryButtonHovered = ImVec4(0.39f, 0.75f, 0.48f, 1.0f);
			ImVec4 secondaryButtonActive = ImVec4(0.39f, 0.75f, 0.48f, 1.0f);
			ImVec4 header = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
			ImVec4 headerHovered = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			ImVec4 headerActive = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			ImVec4 separator = ImVec4(0.43f, 0.43f, 0.50f, 1.00f);
			ImVec4 separatorHovered = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
			ImVec4 separatorActive = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
			ImVec4 resizeGrip = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
			ImVec4 resizeGripHovered = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
			ImVec4 resizeGripActive = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
			ImVec4 tableHeaderBg = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
			ImVec4 tableBorderStrong = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
			ImVec4 tableBorderLight = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
			ImVec4 tableRowBg = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			ImVec4 textSelectedBg = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);

			ImVec2 windowPadding = ImVec2(8.00f, 8.00f);
			ImVec2 widgetPadding = ImVec2(4.00f, 3.00f);
			ImVec2 cellPadding = ImVec2(6.00f, 6.00f);
			ImVec2 itemSpacing = ImVec2(6.00f, 6.00f);
			ImVec2 itemInnerSpacing = ImVec2(6.00f, 6.00f);

			float sidebarSpacing = 0;
			float windowRounding = 9;
			float windowBorderSize = 1;
			float widgetBorderSize = 1;
			float widgetRounding = 3;
			float indentSpacing = 21;
			float columnsMinSpacing = 50;
			float scrollbarRounding = 9;
			float scrollbarSize = 14;
			float grabMinSize = 10;
			float grabRounding = 3;

			bool showTableRowBG = true;
			bool noIconText = false;

			GraphicManager::Image splashImage;
		};

		struct Setting
		{
			Config config;
			Style style;
		};

		Setting def;
		Setting user;

		void ExportThemeToIni(const wchar_t* a_path, Style user);
		void InstantiateDefaultTheme(Style& a_out);

		// https://github.com/powerof3/PhotoMode | License: MIT
		template <class T>
		static std::string ToString(const T& a_style, bool a_hex = false)
		{
			if constexpr (std::is_same_v<ImVec4, T>) {
				if (a_hex) {
					return std::format("#{:02X}{:02X}{:02X}{:02X}", std::uint8_t(255.0f * a_style.x), std::uint8_t(255.0f * a_style.y), std::uint8_t(255.0f * a_style.z), std::uint8_t(255.0f * a_style.w));
				}
				return std::format("{}{},{},{},{}{}", "{", std::uint8_t(255.0f * a_style.x), std::uint8_t(255.0f * a_style.y), std::uint8_t(255.0f * a_style.z), std::uint8_t(255.0f * a_style.w), "}");
			} else if constexpr (std::is_same_v<ImVec2, T>) {
				return std::format("{}{},{}{}", "{", a_style.x, a_style.y, "}");
			} else if constexpr (std::is_same_v<std::string, T>) {
				return a_style;
			} else if constexpr (std::is_same_v<FontManager::FontData, T>) {
				return a_style.name;
			} else if constexpr (std::is_same_v<GraphicManager::Image, T>) {
				return GraphicManager::GetImageName(a_style);
			} else if constexpr (std::is_same_v<Language::GlyphRanges, T>) {
				return Language::GetGlyphName(a_style);
			} else if constexpr (std::is_same_v<float, T>) {
				return std::format("{:.3f}", a_style);
			} else if constexpr (std::is_same_v<bool, T>) {
				return a_style ? "true" : "false";
			} else if constexpr (std::is_same_v<int, T>) {
				return std::to_string(a_style);
			} else {
				stl::report_and_fail("Unsupported type for ToString");  // FIXME: static_assert?
			}
		}

		// https://github.com/powerof3/PhotoMode | License: MIT
		template <class T>
		static std::pair<T, bool> GetColor(std::string& a_str)
		{
			if constexpr (std::is_same_v<ImVec4, T>) {
				static std::regex rgb_pattern("\\{([0-9]+),([0-9]+),([0-9]+),([0-9]+)\\}");
				static std::regex hex_pattern("#([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})");

				std::smatch rgb_matches;
				std::smatch hex_matches;

				if (std::regex_match(a_str, rgb_matches, rgb_pattern)) {
					auto red = std::stoi(rgb_matches[1]);
					auto green = std::stoi(rgb_matches[2]);
					auto blue = std::stoi(rgb_matches[3]);
					auto alpha = std::stoi(rgb_matches[4]);

					return { ImVec4(red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f), true };  //{ { red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f }, false };
				} else if (std::regex_match(a_str, hex_matches, hex_pattern)) {
					auto red = std::stoi(hex_matches[1], 0, 16);
					auto green = std::stoi(hex_matches[2], 0, 16);
					auto blue = std::stoi(hex_matches[3], 0, 16);
					auto alpha = std::stoi(hex_matches[4], 0, 16);

					return { ImVec4(red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f), true };
				}
			}

			logger::critical("Failed to parse color from .ini file! Ensure you're using the correct format!");
			logger::critical("ImVec4 input: {}", a_str);
			return { T(), false };
		}

		static std::pair<ImVec2, bool> GetVec2(std::string& a_str)
		{
			static std::regex pattern("\\{([0-9]*\\.?[0-9]+),([0-9]*\\.?[0-9]+)\\}");
			std::smatch matches;

			if (std::regex_match(a_str, matches, pattern)) {
				float x = std::stof(matches[1]);
				float y = std::stof(matches[2]);

				return { ImVec2(x, y), true };
			}

			logger::critical("Failed to parse ImVec2 from .ini file! Ensure you're using the correct format!");
			logger::critical("ImVec2 input: {}", a_str);
			return { ImVec2(), false };
		}

		static std::map<std::string, GraphicManager::Image> GetListOfImages()
		{
			return GraphicManager::image_library;
		}

		// TODO: Remove hard-coded path
		static std::vector<std::string> GetListOfThemes()
		{
			std::vector<std::string> themes;
			std::string path_to_themes = "Data/Interface/Modex/themes";

			// TODO: Implement scope wide error handling.
			if (std::filesystem::exists(path_to_themes) == false) {
				stl::report_and_fail(
					"Modex.dll] FATAL ERROR:\n\n"
					"Could not find themes directory (\"DATA/Interface/Modex/themes/\")\n\n"
					"Make sure the Modex mod contains this directory respective to the data folder."sv);
			}

			for (const auto& entry : std::filesystem::directory_iterator(path_to_themes)) {
				if (entry.path().filename().extension() != ".ini") {
					continue;  // Pass invalid file types
				}

				auto index = entry.path().filename().stem().string();
				themes.push_back(index);
			}

			return themes;
		}

		static std::wstring GetThemePath(std::string& a_str)
		{
			std::wstring path(ini_theme_path);
			return path + std::wstring(a_str.begin(), a_str.end()) + L".ini";
		}

		// Searches theme directory, and executes SetThemeFromIni(a_path) on the first matching theme
		// passed in. Returns the name of the theme if it was found, and a bool if it was successful.
		static std::pair<std::string, bool> SetThemeFromIni(std::string a_str)
		{
			std::vector<std::string> themes = GetListOfThemes();
			std::wstring full_path = GetThemePath(a_str);

			// Case-insensitive comparison for Steamdeck support, and common sense.
			std::transform(a_str.begin(), a_str.end(), a_str.begin(), ::tolower);

			for (auto entry : themes) {
				std::transform(entry.begin(), entry.end(), entry.begin(), ::tolower);

				if (entry == a_str) {
					Settings::GetSingleton()->GetIni(full_path.c_str(), [](CSimpleIniA& a_ini) {
						Settings::GetSingleton()->LoadThemeFromIni(a_ini);
					});
					return { a_str, true };
				}
			}

			// If we did not find it, create a new ini with defaults and retry.
			// Mainly a fail-safe if the user edits the master config and breaks the theme.
			Settings::GetSingleton()->ExportThemeToIni(full_path.c_str(), Settings::GetSingleton()->def.style);
			SetThemeFromIni(a_str);

			return { a_str, false };
		}

		// Horrendous de-serialization.
		[[nodiscard]] static inline float GetFloat(std::string& a_str) { return std::stof(a_str); };
		[[nodiscard]] static inline int GetInt(std::string& a_str) { return std::stoi(a_str); };
		[[nodiscard]] static inline bool GetBool(std::string& a_str) { return a_str == "true"; };
		[[nodiscard]] static inline std::string GetString(std::string& a_str) { return a_str; };  // lol
		[[nodiscard]] inline Style& GetStyle() { return user.style; };
		[[nodiscard]] inline Config& GetConfig() { return user.config; };

	private:
	};
}