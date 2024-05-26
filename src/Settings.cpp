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
		auto menu = Menu::GetSingleton();

		menu->LoadSettings(a_ini);
	});
}

// Deserialization starts here.
// Worst deserialization code I've ever seen.

int Settings::GetInt(std::string& a_str)
{
	return std::stoi(a_str);
}

bool Settings::GetBool(std::string& a_str)
{
	if (a_str == "true") {
		return true;
	}

	if (a_str == "false") {
		return false;
	}

	return false;
}

// Remove # before calling this.
// Supports #FF00FF, #FF00FF00, 255,0,255,0 only.
ImVec4 Settings::GetColor(std::string& a_str)
{
	logger::info("Color: {}", a_str.size());

	if (a_str.size() == 8) {
		auto r = std::stoi(a_str.substr(0, 2), 0, 16);
		auto g = std::stoi(a_str.substr(2, 2), 0, 16);
		auto b = std::stoi(a_str.substr(4, 2), 0, 16);
		auto a = std::stoi(a_str.substr(6, 2), 0, 16);

		logger::info("Color: {} {} {} {}", r, g, b, a);
		return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	}

	if (a_str.size() == 6) {
		auto r = std::stoi(a_str.substr(0, 2), 0, 16);
		auto g = std::stoi(a_str.substr(2, 2), 0, 16);
		auto b = std::stoi(a_str.substr(4, 2), 0, 16);

		logger::info("Color: {} {} {}", r, g, b);
		return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
	}

	// This works but ini settings using {x,x,x,x} has variable size
	// while ImVec4 is a fixed size of 9 (maybe)
	// So this needs to be more dynamic.
	if (a_str.size() == 9) {
		std::regex rgx("(.+),(.+),(.+),(.+)");
		std::smatch match;

		logger::info("Found default format");
		logger::info("String: {}", a_str);

		if (std::regex_search(a_str, match, rgx)) {
			logger::info("regex search started");
			auto r = std::stoi(match[1].str());
			auto g = std::stoi(match[2].str());
			auto b = std::stoi(match[3].str());
			auto a = std::stoi(match[4].str());

			logger::info("Color: {} {} {} {}", r, g, b, a);
			return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
		}
	}

	return ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
}
