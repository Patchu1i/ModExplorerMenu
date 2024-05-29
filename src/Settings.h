#pragma once

// Current process of loading settings::
// Start by loading the user Config and getting the Theme string.
// Look for the theme string in the themes directory for a matching ini.
// (If it doesn't exist, load the default theme from def values in Style struct)
// Load the theme ini and apply the values to the user Style struct.

class Settings
{
public:
	// void LoadStyleTheme(ImGuiStyle a_theme); - DEPRECATED

	void GetIni(const wchar_t* a_path, const std::function<void(CSimpleIniA&)> a_func);
	void LoadSettings(const wchar_t* a_path);
	void LoadPreset(CSimpleIniA& a_ini, bool use_default_preset = false);
	void LoadThemeFromIni(CSimpleIniA& a_ini);
	void LoadPresetFromIni(CSimpleIniA& a_ini);

	void LoadStyleTheme(ImGuiStyle a_theme);

	void CreateDefaultPreset();
	static void FormatThemeIni(CSimpleIniA& a_ini);
	static void FormatPresetIni(CSimpleIniA& a_ini);

	static inline Settings* GetSingleton()
	{
		static Settings singleton;
		return &singleton;
	}

	constexpr inline static const wchar_t* ini_theme_path = L"Data/Interface/ModExplorerMenu/themes/";
	constexpr inline static const wchar_t* ini_preset_path = L"Data/Interface/ModExplorerMenu/presets/";
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
		Autotest
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
		{ Autotest, "Autotest" }
	};

	struct Config
	{
		std::string theme = "Default";
		std::string preset = "Default";
		bool works = false;
	};

	ImGuiStyle test;

	struct Style
	{
		ImVec4 text;
		ImVec4 textDisabled;
		ImVec4 windowBg;
		ImVec4 childBg;
		ImVec4 popupBg;
		ImVec4 border;
		ImVec4 borderShadow;
		ImVec4 frameBg;
		ImVec4 frameBgHovered;
		ImVec4 frameBgActive;
		ImVec4 titleBg;
		ImVec4 titleBgActive;
		ImVec4 titleBgCollapsed;
		ImVec4 menuBarBg;
		ImVec4 scrollbarBg;
		ImVec4 scrollbarGrab;
		ImVec4 scrollbarGrabHovered;
		ImVec4 scrollbarGrabActive;
		ImVec4 checkMark;
		ImVec4 sliderGrab;
		ImVec4 sliderGrabActive;
		ImVec4 button;
		ImVec4 buttonHovered;
		ImVec4 buttonActive;
		ImVec4 header;
		ImVec4 headerHovered;
		ImVec4 headerActive;
		ImVec4 separator;
		ImVec4 separatorHovered;
		ImVec4 separatorActive;
		ImVec4 resizeGrip;
		ImVec4 resizeGripHovered;
		ImVec4 resizeGripActive;
		ImVec4 tableHeaderBg;
		ImVec4 tableBorderStrong;
		ImVec4 tableBorderLight;
		ImVec4 tableRowBg;
		ImVec4 textSelectedBg;
		ImVec4 navHighlight;
		ImVec4 navWindowingDimBg;
		ImVec4 modalWindowDimBg;

		ImVec2 windowPadding;
		ImVec2 framePadding;
		ImVec2 cellPadding;
		ImVec2 itemSpacing;
		ImVec2 itemInnerSpacing;

		float alpha;
		float disabledAlpha;
		float windowRounding;
		float windowBorderSize;
		float childBorderSize;
		float childRounding;
		float frameBorderSize;
		float frameRounding;
		float tabBorderSize;
		float tabRounding;
		float indentSpacing;
		float columnsMinSpacing;
		float scrollbarRounding;
		float scrollbarSize;
		float grabMinSize;
		float grabRounding;
		float popupBorderSize;
		float popupRounding;
	};

	struct Setting
	{
		Config config;
		Style style;
	};

	Setting def;  // TODO: Is this necessary with ini configurations?
	Setting user;

	void ExportThemeToIni(const wchar_t* a_path, Style& user);

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
		return { T(), false };
	}

	static std::pair<ImVec2, bool> GetVec2(std::string& a_str)
	{
		static std::regex pattern("\\{([0-9]+),([0-9]+)\\}");
		std::smatch matches;

		if (std::regex_match(a_str, matches, pattern)) {
			float x = std::stof(matches[1]);
			float y = std::stof(matches[2]);

			return { ImVec2(x, y), true };
		}

		logger::critical("Failed to parse ImVec2 from .ini file! Ensure you're using the correct format!");
		return { ImVec2(), false };
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

	static std::vector<std::string> GetListOfPresets()
	{
		std::vector<std::string> presets;
		std::string path_to_presets = "Data/Interface/ModExplorerMenu/presets";

		// TODO: Implement scope wide error handling.
		if (std::filesystem::exists(path_to_presets) == false) {
			stl::report_and_fail(
				"ModExplorerMenu.dll] FATAL ERROR:\n\n"
				"Could not find preset directory (\"DATA/Interface/ModExplorerMenu/presets/\")\n\n"
				"Make sure the ModExplorerMenu mod contains this directory respective to the data folder."sv);
		}

		for (const auto& entry : std::filesystem::directory_iterator(path_to_presets)) {
			if (entry.path().filename().extension() != ".ini") {
				continue;  // Pass invalid file types
			}
			auto index = entry.path().filename().stem().string();
			logger::info("Found preset: {}", index);
			presets.push_back(index);
		}

		return presets;
	}

	static std::pair<std::string, bool> SetPresetFromIni(std::string& a_str)
	{
		std::vector<std::string> presets = GetListOfPresets();
		std::string path_to_presets = "Data/Interface/ModExplorerMenu/presets";

		for (const auto& entry : presets) {
			if (entry == a_str) {
				const wchar_t* full_path = (wchar_t*)(path_to_presets + "/" + a_str + ".ini").c_str();
				Settings::GetSingleton()->GetIni(full_path, [](CSimpleIniA& a_ini) {
					Settings::GetSingleton()->LoadPresetFromIni(a_ini);
				});
				return { entry, true };
			}
		}

		return { a_str, false };
	}

	// Searches theme directory, and executes SetThemeFromIni(a_path) on the first matching theme
	// passed in. Returns the name of the theme if it was found, and a bool if it was successful.
	static std::pair<std::string, bool> SetThemeFromIni(std::string& a_str)
	{
		std::vector<std::string> themes = GetListOfThemes();
		std::string path_to_themes = "Data/Interface/ModExplorerMenu/themes";

		for (const auto& entry : themes) {
			if (entry == a_str) {
				const wchar_t* full_path = (wchar_t*)(path_to_themes + "/" + a_str + ".ini").c_str();
				Settings::GetSingleton()->GetIni(full_path, [](CSimpleIniA& a_ini) {
					Settings::GetSingleton()->LoadThemeFromIni(a_ini);
				});
				return { entry, true };
			}
		}

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