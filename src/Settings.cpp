#include "Settings.h"
#include "Menu.h"
#include "SimpleIni.h"
#include <format>

void Settings::GetIni(const wchar_t* a_path, const std::function<void(CSimpleIniA&)> a_func)
{
	CSimpleIniA ini;

	ini.SetUnicode();

	if (const auto rc = ini.LoadFile(a_path)) {
		if (rc < SI_OK) {
			stl::report_and_fail("Failed to load ModExplorerMenu.ini file!"sv);
			return;
		}
	}

	a_func(ini);

	(void)ini.SaveFile(a_path);
}

void Settings::LoadSettings(const wchar_t* a_path)
{
	logger::info("Settings Loaded");

	GetIni(a_path, [](CSimpleIniA& a_ini) {
		//auto menu = Menu::GetSingleton();
		//menu->LoadSettings(a_ini);
		//Settings::LoadStyleSettings(a_ini);
		Settings::GetSingleton()->LoadStyleSettings(a_ini);
	});
}

bool ResetIni = true;

template <class T>
T GET_VALUE(const char* section, const char* key, T a_default, CSimpleIniA& a_ini)
{
	std::string _default = Settings::ToString(a_default, false);
	std::string value = a_ini.GetValue(section, key, _default.c_str());

	// Generate defaults in ini if they don't exist.
	if (value == _default || ResetIni) {
		a_ini.SetValue(section, key, _default.c_str());
	}

	if constexpr (std::is_same_v<T, ImVec4>) {
		auto color = Settings::GetColor<T>(value);
		return (color.second ? color.first : a_default);
	} else if constexpr (std::is_same_v<T, bool>) {
		return Settings::GetBool(value);
	} else if constexpr (std::is_same_v<T, int>) {
		return Settings::GetInt(value);
	} else if constexpr (std::is_same_v<T, float>) {
		return Settings::GetFloat(value);
	} else {
		stl::report_and_fail("Unhandled type passed to GET_VALUE in Menu.cpp!");
		return a_default;
	}
}

void Settings::LoadStyleSettings(CSimpleIniA& a_ini)
{
	user.background = GET_VALUE<ImVec4>("Style", "BackgroundColor", def.background, a_ini);
	user.border = GET_VALUE<ImVec4>("Style", "BorderColor", def.border, a_ini);
	user.borderSize = GET_VALUE<float>("Style", "BorderSize", def.borderSize, a_ini);

	user.text = GET_VALUE<ImVec4>("Style", "TextColor", def.text, a_ini);
	user.textDisabled = GET_VALUE<ImVec4>("Style", "TextDisabledColor", def.textDisabled, a_ini);

	user.comboBoxText = GET_VALUE<ImVec4>("Style", "ComboBoxTextColor", def.comboBoxText, a_ini);
	user.comboBoxTextBox = GET_VALUE<ImVec4>("Style", "ComboBoxTextBoxColor", def.comboBoxTextBox, a_ini);

	user.button = GET_VALUE<ImVec4>("Style", "ButtonColor", def.button, a_ini);

	user.frameBG = GET_VALUE<ImVec4>("Style", "FrameBGColor", def.frameBG, a_ini);

	user.separator = GET_VALUE<ImVec4>("Style", "SeparatorColor", def.separator, a_ini);
	user.separatorThickness = GET_VALUE<float>("Style", "SeparatorThickness", def.separatorThickness, a_ini);

	Menu::GetSingleton()->SetupStyle(user);
}
