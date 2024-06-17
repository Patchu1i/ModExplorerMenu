#pragma once

#include "Graphic.h"

// Current process of loading settings::
// Start by loading the user Config and getting the Theme string.
// Look for the theme string in the themes directory for a matching ini.
// (If it doesn't exist, load the default theme from def values in Style struct)
// Load the theme ini and apply the values to the user Style struct.

namespace ModExplorerMenu
{

	class Settings
	{
	public:
		// void LoadStyleTheme(ImGuiStyle a_theme); - DEPRECATED

		void GetIni(const wchar_t* a_path, const std::function<void(CSimpleIniA&)> a_func);
		void LoadSettings(const wchar_t* a_path);
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

		constexpr inline static const wchar_t* ini_theme_path = L"Data/Interface/ModExplorerMenu/themes/";
		constexpr inline static const wchar_t* ini_mem_path = L"Data/Interface/ModExplorerMenu/ModExplorerMenu.ini";

		enum SECTION
		{
			Window,
			Frame,
			Child,
			Text,
			Table,
			Widgets,
			Main,
			AddItem,
			FormLookup,
			NPC,
			Teleport,
			Autotest,
			Fonts,
			Images
		};

		static inline std::map<SECTION, const char*> rSections = {
			{ Window, "Window" },
			{ Frame, "Frame" },
			{ Child, "Child" },
			{ Text, "Text" },
			{ Table, "Table" },
			{ Widgets, "Widgets" },
			{ Main, "Main" },
			{ AddItem, "AddItem" },
			{ FormLookup, "FormLookup" },
			{ NPC, "NPC" },
			{ Teleport, "Teleport" },
			{ Autotest, "Autotest" },
			{ Fonts, "Fonts" },
			{ Images, "Images & Icons" }
		};

		struct Config
		{
			// Main
			std::string theme = "Default";

			// Additem
			bool aimShowFavoriteColumn;
			bool aimShowPluginColumn;
			bool aimShowTypeColumn;
			bool aimShowFormIDColumn;
			bool aimShowNameColumn;
			bool aimShowEditorIDColumn;
			bool aimShowGoldValueColumn;
			bool aimShowBaseDamageColumn;
			bool aimShowArmorRatingColumn;
			bool aimShowSpeedColumn;
			bool aimShowCritDamageColumn;
			bool aimShowSkillColumn;
			bool aimShowWeightColumn;
			bool aimShowDPSColumn;

			// Teleport
			bool teleShowFavoriteColumn;
			bool teleShowPluginColumn;
			bool teleShowSpaceColumn;
			bool teleShowZoneColumn;
			bool teleShowFullNameColumn;
			bool teleShowEditorIDColumn;

			// NPC
			bool npcShowFavoriteColumn;
			bool npcShowPluginColumn;
			bool npcShowNameColumn;
			bool npcShowFormIDColumn;
			bool npcShowEditorIDColumn;
			bool npcShowHealthColumn;
			bool npcShowHealthRegenColumn;
			bool npcShowMagickaColumn;
			bool npcShowMagickaRegenColumn;
			bool npcShowStaminaColumn;
			bool npcShowStaminaRegenColumn;
			bool npcShowCarryWeightColumn;
			bool npcShowMassColumn;
			bool npcShowDamageResistColumn;
			bool npcShowAttackDamageColumn;
			bool npcShowUnarmedDamageColumn;
			bool npcShowSkillsColumn;
		};

		ImGuiStyle test;

		struct Style  // should extend ImGuiStyle (?)
		{
			ImVec4 text = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			ImVec4 textDisabled = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
			ImVec4 windowBg = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
			ImVec4 childBg = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			ImVec4 popupBg = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
			ImVec4 border = ImVec4(0.43f, 0.43f, 0.50f, 1.00f);
			ImVec4 borderShadow = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			ImVec4 frameBg = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
			ImVec4 frameBgHovered = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			ImVec4 frameBgActive = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			ImVec4 scrollbarBg = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
			ImVec4 scrollbarGrab = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
			ImVec4 scrollbarGrabHovered = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
			ImVec4 scrollbarGrabActive = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
			ImVec4 checkMark = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			ImVec4 sliderGrab = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
			ImVec4 sliderGrabActive = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			ImVec4 button = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			ImVec4 buttonHovered = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
			ImVec4 buttonActive = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
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
			ImVec2 framePadding = ImVec2(4.00f, 3.00f);
			ImVec2 cellPadding = ImVec2(6.00f, 6.00f);
			ImVec2 itemSpacing = ImVec2(6.00f, 6.00f);
			ImVec2 itemInnerSpacing = ImVec2(6.00f, 6.00f);

			float alpha = 1.0f;
			float windowRounding = 9;
			float windowBorderSize = 1;
			float childBorderSize = 1;
			float childRounding = 0;
			float frameBorderSize = 1;
			float frameRounding = 3;
			float indentSpacing = 21;
			float columnsMinSpacing = 50;
			float scrollbarRounding = 9;
			float scrollbarSize = 14;
			float grabMinSize = 10;
			float grabRounding = 3;
			float popupBorderSize = 1;
			float popupRounding = 3;
			float globalFontSize = 0.5;

			bool showTableRowBG = true;

			GraphicManager::Font font;

			GraphicManager::Image splashImage;
			GraphicManager::Image favoriteIconEnabled;
			GraphicManager::Image favoriteIconDisabled;
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
		static std::string ToString(const T& a_style, bool a_hex)
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
			} else if constexpr (std::is_same_v<GraphicManager::Font, T>) {
				return GraphicManager::GetFontName(a_style);
			} else if constexpr (std::is_same_v<GraphicManager::Image, T>) {
				return GraphicManager::GetImageName(a_style);
			} else if constexpr (std::is_same_v<float, T>) {
				return std::format("{:.3f}", a_style);
			} else if constexpr (std::is_same_v<bool, T>) {
				return a_style ? "true" : "false";
			} else {
				stl::report_and_fail("Unsupported type for ToString");  // FIXME: static_assert?
				return;
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

		static std::map<std::string, GraphicManager::Font> GetListOfFonts()
		{
			return GraphicManager::font_library;
		}

		static std::map<std::string, GraphicManager::Image> GetListOfImages()
		{
			return GraphicManager::image_library;
		}

		// TODO: Remove hard-coded path
		static std::vector<std::string> GetListOfThemes()
		{
			std::vector<std::string> themes;
			std::string path_to_themes = "Data/Interface/ModExplorerMenu/themes";

			// TODO: Implement scope wide error handling.
			if (std::filesystem::exists(path_to_themes) == false) {
				stl::report_and_fail(
					"ModExplorerMenu.dll] FATAL ERROR:\n\n"
					"Could not find themes directory (\"DATA/Interface/ModExplorerMenu/themes/\")\n\n"
					"Make sure the ModExplorerMenu mod contains this directory respective to the data folder."sv);
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

		// Searches theme directory, and executes SetThemeFromIni(a_path) on the first matching theme
		// passed in. Returns the name of the theme if it was found, and a bool if it was successful.
		static std::pair<std::string, bool> SetThemeFromIni(std::string& a_str)
		{
			std::vector<std::string> themes = GetListOfThemes();
			std::wstring path(ini_theme_path);
			std::wstring full_path = path + std::wstring(a_str.begin(), a_str.end()) + L".ini";

			for (const auto& entry : themes) {
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
		[[nodiscard]] static inline bool GetBool(std::string& a_str) { return a_str == "1"; };
		[[nodiscard]] static inline std::string GetString(std::string& a_str) { return a_str; };  // lol
		[[nodiscard]] inline Style& GetStyle() { return user.style; };
		[[nodiscard]] inline Config& GetConfig() { return user.config; };

	private:
	};
}