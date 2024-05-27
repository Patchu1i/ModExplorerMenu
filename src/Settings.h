#pragma once

class Settings
{
public:
	void LoadSettings(const wchar_t* a_path);

	void GetIni(const wchar_t* a_path, const std::function<void(CSimpleIniA&)> a_func);

	void LoadStyleSettings(CSimpleIniA& a_ini);
	void LoadStyleTheme(ImGuiStyle a_theme);

	static inline Settings* GetSingleton()
	{
		static Settings singleton;
		return &singleton;
	}

	constexpr inline static const wchar_t* ini_theme_path = L"Data/Interface/ModExplorerMenu/";
	constexpr inline static const wchar_t* ini_settings_fullpath = L"Data/Interface/ModExplorerMenu/ModExplorerMenu.ini";

	// https://github.com/powerof3/PhotoMode
	// License: MIT
	struct Style
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
		ImVec4 titleBg = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		ImVec4 titleBgActive = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
		ImVec4 titleBgCollapsed = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		ImVec4 menuBarBg = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
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
		ImVec4 plotLines = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		ImVec4 plotLinesHovered = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		ImVec4 plotHistogram = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		ImVec4 plotHistogramHovered = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		ImVec4 textSelectedBg = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		ImVec4 dragDropTarget = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		ImVec4 navHighlight = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		ImVec4 navWindowingDimBg = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		ImVec4 modalWindowDimBg = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		ImVec2 windowPadding = ImVec2(8.00f, 8.00f);
		ImVec2 framePadding = ImVec2(4.00f, 3.00f);
		ImVec2 cellPadding = ImVec2(6.00f, 6.00f);
		ImVec2 itemSpacing = ImVec2(6.00f, 6.00f);
		ImVec2 itemInnerSpacing = ImVec2(6.00f, 6.00f);
		ImVec2 touchExtraPadding = ImVec2(0.00f, 0.00f);
		ImVec2 displayWindowPadding = ImVec2(19.00f, 19.00f);
		ImVec2 displaySafeAreaPadding = ImVec2(3.00f, 3.00f);

		float alpha = 1.0f;
		float disabledAlpha = 0.5f;
		float windowRounding = 9;
		float windowBorderSize = 1;
		float childBorderSize = 1;
		float childRounding = 0;
		float frameBorderSize = 1;
		float frameRounding = 3;
		float tabBorderSize = 1;
		float tabRounding = 4;
		float indentSpacing = 21;
		float columnsMinSpacing = 50;
		float scrollbarRounding = 9;
		float scrollbarSize = 14;
		float grabMinSize = 10;
		float grabRounding = 3;
		float mouseCursorScale = 1;
		float antiAliasedLines = 1;
		float antiAliasedLinesUseTex = 1;
		float antiAliasedFill = 1;
		float curveTessellationTol = 1;
		float circleSegmentMaxError = 1;
		float popupBorderSize = 1;
		float popupRounding = 3;
		float logSliderDeadzone = 4;
	};

	Style def;
	Style user;

	void SaveStyleThemeToIni(const wchar_t* a_path, Style& user);

	// https://github.com/powerof3/PhotoMode
	// License: MIT
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
		} else {
			return std::format("{:.3f}", a_style);
		}
	}

	// MIT License
	// https://github.com/powerof3/PhotoMode
	template <class T>
	static std::pair<T, bool> GetColor(std::string& a_str)
	{
		if constexpr (std::is_same_v<ImVec4, T>) {
			logger::info("Color: {}", a_str.size());
			static std::regex rgb_pattern("\\{([0-9]+),([0-9]+),([0-9]+),([0-9]+)\\}");
			static std::regex hex_pattern("#([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})");

			std::smatch rgb_matches;
			std::smatch hex_matches;

			if (std::regex_match(a_str, rgb_matches, rgb_pattern)) {
				auto red = std::stoi(rgb_matches[1]);
				auto green = std::stoi(rgb_matches[2]);
				auto blue = std::stoi(rgb_matches[3]);
				auto alpha = std::stoi(rgb_matches[4]);

				logger::info("Color rgb: {}, {}, {}, {}", red, green, blue, alpha);

				return { ImVec4(red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f), true };  //{ { red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f }, false };
			} else if (std::regex_match(a_str, hex_matches, hex_pattern)) {
				auto red = std::stoi(hex_matches[1], 0, 16);
				auto green = std::stoi(hex_matches[2], 0, 16);
				auto blue = std::stoi(hex_matches[3], 0, 16);
				auto alpha = std::stoi(hex_matches[4], 0, 16);

				logger::info("Color hex: {}, {}, {}, {}", red, green, blue, alpha);
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

	[[nodiscard]] static inline float GetFloat(std::string& a_str) { return std::stof(a_str); };
	[[nodiscard]] static inline int GetInt(std::string& a_str) { return std::stoi(a_str); };
	[[nodiscard]] static inline bool GetBool(std::string& a_str) { return a_str == "true"; };
	[[nodiscard]] inline Style& GetStyle() { return user; };

private:
};