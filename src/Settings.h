#pragma once

class Settings
{
public:
	void LoadSettings(const wchar_t* a_path);

	void GetIni(const wchar_t* a_path, const std::function<void(CSimpleIniA&)> a_func);

	void LoadStyleSettings(CSimpleIniA& a_ini);

	static inline Settings* GetSingleton()
	{
		static Settings singleton;
		return &singleton;
	}

	// https://github.com/powerof3/PhotoMode
	// License: MIT
	struct Style
	{
		// unused, helpers
		float bgAlpha{ 0.68f };
		float disabledAlpha{ 0.30f };

		float buttonScale{ 0.5f };
		float checkboxScale{ 0.5f };

		ImVec4 iconDisabled{ 1.0f, 1.0f, 1.0f, disabledAlpha };

		ImVec4 background{ 0.0f, 0.0f, 0.0f, bgAlpha };

		ImVec4 border{ 0.569f, 0.545f, 0.506f, bgAlpha };
		float borderSize{ 3.5f };

		ImVec4 text{ 1.0f, 1.0f, 1.0f, 1.0f };
		ImVec4 textDisabled{ 1.0f, 1.0f, 1.0f, disabledAlpha };

		ImVec4 comboBoxText{ 1.0f, 1.0f, 1.0f, 0.8f };
		ImVec4 comboBoxTextBox{ 0.0f, 0.0f, 0.0f, 1.0f };
		ImVec4 button{ 0.0f, 0.0f, 0.0f, bgAlpha };  // arrow button

		ImVec4 frameBG{ 0.2f, 0.2f, 0.2f, bgAlpha };

		ImVec4 separator{ 0.569f, 0.545f, 0.506f, bgAlpha };
		float separatorThickness{ 3.5f };
	};

	Style def;
	Style user;

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
		} else {
			return std::format("{:.3f}", a_style);
		}
	}

	[[nodiscard]] static inline float GetFloat(std::string& a_str) { return std::stof(a_str); };
	[[nodiscard]] static inline int GetInt(std::string& a_str) { return std::stoi(a_str); };
	[[nodiscard]] static inline bool GetBool(std::string& a_str) { return a_str == "true"; };
	[[nodiscard]] static inline Style* GetStyle() { return &GetSingleton()->user; };

private:
};