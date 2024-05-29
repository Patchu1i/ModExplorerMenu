#include "Settings.h"
#include "Menu.h"
#include "SimpleIni.h"
#include "lib/ini_comments.h"
#include <format>

using namespace IniHelper;

void Settings::GetIni(const wchar_t* a_path, const std::function<void(CSimpleIniA&)> a_func)
{
	CSimpleIniA ini;

	ini.SetUnicode();

	if (const auto rc = ini.LoadFile(a_path)) {
		if (rc < SI_OK) {
			(void)ini.SaveFile(a_path);  // Could not locate, let's procreate.
		}
	}

	a_func(ini);

	(void)ini.SaveFile(a_path);
}

bool ResetIni = false;  // TODO: Implement developer mode to overwrite presets.

template <class T>
T GET_VALUE(const char* section, const char* key, T a_default, CSimpleIniA& a_ini)
{
	if (a_ini.GetValue(section, key) == nullptr) {
		a_ini.SetValue(section, key, Settings::ToString(a_default, false).c_str());
	}

	std::string value = a_ini.GetValue(section, key);

	// A plus de-serialization.
	if constexpr (std::is_same_v<T, ImVec4>) {
		auto color = Settings::GetColor<T>(value);
		return (color.second ? color.first : a_default);
	} else if constexpr (std::is_same_v<T, ImVec2>) {
		auto vec = Settings::GetVec2(value);
		return (vec.second ? vec.first : a_default);
	} else if constexpr (std::is_same_v<T, std::string>) {
		return Settings::GetString(value);
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

// Execute ini value assignment where necessary.
void Settings::LoadSettings(const wchar_t* a_path)
{
	GetIni(a_path, [](CSimpleIniA& a_ini) {
		Settings::GetSingleton()->LoadPreset(a_ini);
	});
}

// Call on new ini file to generate comments and sections before inserting.
void Settings::FormatThemeIni(CSimpleIniA& a_ini)
{
	a_ini.SetValue(rSections[Window], NULL, NULL, GetComment(iComment::ThemeConfigHeader));
	a_ini.SetValue(rSections[Frame], NULL, NULL, GetComment(iComment::ThemeFrameHeader));
	a_ini.SetValue(rSections[Child], NULL, NULL, GetComment(iComment::ThemeChildHeader));
	a_ini.SetValue(rSections[Text], NULL, NULL, GetComment(iComment::ThemeTextHeader));
	a_ini.SetValue(rSections[Table], NULL, NULL, GetComment(iComment::ThemeTableHeader));
	a_ini.SetValue(rSections[Widgets], NULL, NULL, GetComment(iComment::ThemeWidgetsHeader));
}

// Call on new ini file to generate comments and sections before inserting.
void Settings::FormatPresetIni(CSimpleIniA& a_ini)
{
	a_ini.SetValue(rSections[Main], NULL, NULL, GetComment(iComment::PresetMainHeader));
	a_ini.SetValue(rSections[AddItem], NULL, NULL, GetComment(iComment::PresetAddItemHeader));
	a_ini.SetValue(rSections[FormLookup], NULL, NULL, GetComment(iComment::PresetFormLookupHeader));
	a_ini.SetValue(rSections[NPC], NULL, NULL, GetComment(iComment::PresetNPCHeader));
	a_ini.SetValue(rSections[Teleport], NULL, NULL, GetComment(iComment::PresetTeleportHeader));
}

// Manually create a default preset file in-case end-user is a baddy.
void Settings::CreateDefaultPreset()
{
	GetIni(L"Data/Interface/ModExplorerMenu/presets/Default.ini", [](CSimpleIniA& a_ini) {
		FormatPresetIni(a_ini);
		a_ini.SetValue(rSections[Main], "Preset", "Default");
	});
}

// This is executed within the scope of ModExplorerMenu.ini
// Loads the preset specified in master ini. Then loads theme from there.
void Settings::LoadPreset(CSimpleIniA& a_ini, bool use_default_preset)
{
	std::pair<std::string, bool> callback;

	// Preset
	if (use_default_preset) {
		user.config.preset = "Default";
		a_ini.SetValue("Config", "Preset", user.config.preset.c_str());
		callback = Settings::SetPresetFromIni(user.config.preset);
	} else {
		user.config.preset = GET_VALUE<std::string>("Config", "Preset", user.config.preset, a_ini);
		callback = Settings::SetPresetFromIni(user.config.preset);
	}

	if (!callback.second) {
		if (callback.first == "Default") {
			CreateDefaultPreset();
			LoadPreset(a_ini, true);
			return;
		} else {
			LoadPreset(a_ini, true);
			return;
		}
	} else {
		std::wstring ini_preset_path_str = ini_preset_path + std::wstring(callback.first.begin(), callback.first.end()) + L".ini";
		GetIni(ini_preset_path_str.c_str(), [](CSimpleIniA& a_ini) {
			Settings::GetSingleton()->LoadPresetFromIni(a_ini);
		});
	}
}

// DEPRECATED::
// Previously used to load themes from ImGuiStyle themes. Now we load from .ini files.
// void Settings::LoadStyleTheme(ImGuiStyle a_theme)
// {
// 	user.style.text = a_theme.Colors[ImGuiCol_Text];
// 	user.style.textDisabled = a_theme.Colors[ImGuiCol_TextDisabled];
// 	user.style.windowBg = a_theme.Colors[ImGuiCol_WindowBg];
// 	user.style.childBg = a_theme.Colors[ImGuiCol_ChildBg];
// 	user.style.popupBg = a_theme.Colors[ImGuiCol_PopupBg];
// 	user.style.border = a_theme.Colors[ImGuiCol_Border];
// 	user.style.borderShadow = a_theme.Colors[ImGuiCol_BorderShadow];
// 	user.style.frameBg = a_theme.Colors[ImGuiCol_FrameBg];
// 	user.style.frameBgHovered = a_theme.Colors[ImGuiCol_FrameBgHovered];
// 	user.style.frameBgActive = a_theme.Colors[ImGuiCol_FrameBgActive];
// 	user.style.titleBg = a_theme.Colors[ImGuiCol_TitleBg];
// 	user.style.titleBgActive = a_theme.Colors[ImGuiCol_TitleBgActive];
// 	user.style.titleBgCollapsed = a_theme.Colors[ImGuiCol_TitleBgCollapsed];
// 	user.style.menuBarBg = a_theme.Colors[ImGuiCol_MenuBarBg];
// 	user.style.scrollbarBg = a_theme.Colors[ImGuiCol_ScrollbarBg];
// 	user.style.scrollbarGrab = a_theme.Colors[ImGuiCol_ScrollbarGrab];
// 	user.style.scrollbarGrabHovered = a_theme.Colors[ImGuiCol_ScrollbarGrabHovered];
// 	user.style.scrollbarGrabActive = a_theme.Colors[ImGuiCol_ScrollbarGrabActive];
// 	user.style.checkMark = a_theme.Colors[ImGuiCol_CheckMark];
// 	user.style.sliderGrab = a_theme.Colors[ImGuiCol_SliderGrab];
// 	user.style.sliderGrabActive = a_theme.Colors[ImGuiCol_SliderGrabActive];
// 	user.style.button = a_theme.Colors[ImGuiCol_Button];
// 	user.style.buttonHovered = a_theme.Colors[ImGuiCol_ButtonHovered];
// 	user.style.buttonActive = a_theme.Colors[ImGuiCol_ButtonActive];
// 	user.style.header = a_theme.Colors[ImGuiCol_Header];
// 	user.style.headerHovered = a_theme.Colors[ImGuiCol_HeaderHovered];
// 	user.style.headerActive = a_theme.Colors[ImGuiCol_HeaderActive];
// 	user.style.separator = a_theme.Colors[ImGuiCol_Separator];
// 	user.style.separatorHovered = a_theme.Colors[ImGuiCol_SeparatorHovered];
// 	user.style.separatorActive = a_theme.Colors[ImGuiCol_SeparatorActive];
// 	user.style.resizeGrip = a_theme.Colors[ImGuiCol_ResizeGrip];
// 	user.style.resizeGripHovered = a_theme.Colors[ImGuiCol_ResizeGripHovered];
// 	user.style.resizeGripActive = a_theme.Colors[ImGuiCol_ResizeGripActive];
// 	user.style.tableHeaderBg = a_theme.Colors[ImGuiCol_TableHeaderBg];
// 	user.style.tableBorderStrong = a_theme.Colors[ImGuiCol_TableBorderStrong];
// 	user.style.tableBorderLight = a_theme.Colors[ImGuiCol_TableBorderLight];
// 	user.style.tableRowBg = a_theme.Colors[ImGuiCol_TableRowBg];
// 	user.style.textSelectedBg = a_theme.Colors[ImGuiCol_TextSelectedBg];
// 	user.style.navHighlight = a_theme.Colors[ImGuiCol_NavHighlight];
// 	user.style.navWindowingDimBg = a_theme.Colors[ImGuiCol_NavWindowingDimBg];
// 	user.style.modalWindowDimBg = a_theme.Colors[ImGuiCol_ModalWindowDimBg];

// 	user.style.windowPadding = a_theme.WindowPadding;
// 	user.style.framePadding = a_theme.FramePadding;
// 	user.style.cellPadding = a_theme.CellPadding;
// 	user.style.itemSpacing = a_theme.ItemSpacing;
// 	user.style.itemInnerSpacing = a_theme.ItemInnerSpacing;

// 	user.style.alpha = a_theme.Alpha;
// 	user.style.disabledAlpha = a_theme.Alpha;
// 	user.style.windowRounding = a_theme.WindowRounding;
// 	user.style.windowBorderSize = a_theme.WindowBorderSize;
// 	user.style.childBorderSize = a_theme.ChildBorderSize;
// 	user.style.childRounding = a_theme.ChildRounding;
// 	user.style.frameBorderSize = a_theme.FrameBorderSize;
// 	user.style.frameRounding = a_theme.FrameRounding;
// 	user.style.tabBorderSize = a_theme.TabBorderSize;
// 	user.style.tabRounding = a_theme.TabRounding;
// 	user.style.indentSpacing = a_theme.IndentSpacing;
// 	user.style.columnsMinSpacing = a_theme.ColumnsMinSpacing;
// 	user.style.scrollbarRounding = a_theme.ScrollbarRounding;
// 	user.style.scrollbarSize = a_theme.ScrollbarSize;
// 	user.style.grabMinSize = a_theme.GrabMinSize;
// 	user.style.grabRounding = a_theme.GrabRounding;
// 	user.style.popupBorderSize = a_theme.PopupBorderSize;
// 	user.style.popupRounding = a_theme.PopupRounding;

// 	Menu::GetSingleton()->SyncUserStyleToImGui(user.style);
// }

// Export theme and style values to a standalone ini file.
// Decentralized from the main ini configuration to allow for easy sharing of themes.
void Settings::ExportThemeToIni(const wchar_t* a_path, Style& a_user)
{
	GetIni(a_path, [&a_user](CSimpleIniA& a_ini) {
		FormatThemeIni(a_ini);

		a_ini.SetValue(rSections[Text], "TextColor", Settings::ToString(a_user.text, false).c_str());
		a_ini.SetValue(rSections[Text], "TextDisabledColor", Settings::ToString(a_user.textDisabled, false).c_str());
		a_ini.SetValue(rSections[Window], "WindowBGColor", Settings::ToString(a_user.windowBg, false).c_str());
		a_ini.SetValue(rSections[Child], "ChildBGColor", Settings::ToString(a_user.childBg, false).c_str());
		a_ini.SetValue(rSections[Child], "PopupBGColor", Settings::ToString(a_user.popupBg, false).c_str());
		a_ini.SetValue(rSections[Window], "BorderColor", Settings::ToString(a_user.border, false).c_str());
		a_ini.SetValue(rSections[Window], "BorderShadowColor", Settings::ToString(a_user.borderShadow, false).c_str());
		a_ini.SetValue(rSections[Frame], "FrameBGColor", Settings::ToString(a_user.frameBg, false).c_str());
		a_ini.SetValue(rSections[Frame], "FrameBGHoveredColor", Settings::ToString(a_user.frameBgHovered, false).c_str());
		a_ini.SetValue(rSections[Frame], "FrameBGActiveColor", Settings::ToString(a_user.frameBgActive, false).c_str());
		a_ini.SetValue("ToRemove", "MenuBarColor", Settings::ToString(a_user.menuBarBg, false).c_str());
		a_ini.SetValue(rSections[Widgets], "ScrollbarBGColor", Settings::ToString(a_user.scrollbarBg, false).c_str());
		a_ini.SetValue(rSections[Widgets], "ScrollbarGrabColor", Settings::ToString(a_user.scrollbarGrab, false).c_str());
		a_ini.SetValue(rSections[Widgets], "ScrollbarGrabHoveredColor", Settings::ToString(a_user.scrollbarGrabHovered, false).c_str());
		a_ini.SetValue(rSections[Widgets], "ScrollbarGrabActiveColor", Settings::ToString(a_user.scrollbarGrabActive, false).c_str());
		a_ini.SetValue(rSections[Widgets], "CheckMarkColor", Settings::ToString(a_user.checkMark, false).c_str());
		a_ini.SetValue(rSections[Widgets], "SliderGrabColor", Settings::ToString(a_user.sliderGrab, false).c_str());
		a_ini.SetValue(rSections[Widgets], "SliderGrabActiveColor", Settings::ToString(a_user.sliderGrabActive, false).c_str());
		a_ini.SetValue(rSections[Widgets], "ButtonColor", Settings::ToString(a_user.button, false).c_str());
		a_ini.SetValue(rSections[Widgets], "ButtonHoveredColor", Settings::ToString(a_user.buttonHovered, false).c_str());
		a_ini.SetValue(rSections[Widgets], "ButtonActiveColor", Settings::ToString(a_user.buttonActive, false).c_str());
		a_ini.SetValue(rSections[Table], "HeaderColor", Settings::ToString(a_user.header, false).c_str());
		a_ini.SetValue(rSections[Table], "HeaderHoveredColor", Settings::ToString(a_user.headerHovered, false).c_str());
		a_ini.SetValue(rSections[Table], "HeaderActiveColor", Settings::ToString(a_user.headerActive, false).c_str());
		a_ini.SetValue(rSections[Widgets], "SeparatorColor", Settings::ToString(a_user.separator, false).c_str());
		a_ini.SetValue(rSections[Widgets], "SeparatorHoveredColor", Settings::ToString(a_user.separatorHovered, false).c_str());
		a_ini.SetValue(rSections[Widgets], "SeparatorActiveColor", Settings::ToString(a_user.separatorActive, false).c_str());
		a_ini.SetValue(rSections[Widgets], "ResizeGripColor", Settings::ToString(a_user.resizeGrip, false).c_str());
		a_ini.SetValue(rSections[Widgets], "ResizeGripHoveredColor", Settings::ToString(a_user.resizeGripHovered, false).c_str());
		a_ini.SetValue(rSections[Widgets], "ResizeGripActiveColor", Settings::ToString(a_user.resizeGripActive, false).c_str());
		a_ini.SetValue(rSections[Table], "TableHeaderBGColor", Settings::ToString(a_user.tableHeaderBg, false).c_str());
		a_ini.SetValue(rSections[Table], "TableBorderStrongColor", Settings::ToString(a_user.tableBorderStrong, false).c_str());
		a_ini.SetValue(rSections[Table], "TableBorderLightColor", Settings::ToString(a_user.tableBorderLight, false).c_str());
		a_ini.SetValue(rSections[Table], "TableRowBGColor", Settings::ToString(a_user.tableRowBg, false).c_str());
		a_ini.SetValue(rSections[Text], "TextSelectedBGColor", Settings::ToString(a_user.textSelectedBg, false).c_str());
		a_ini.SetValue("ToRemove", "NavHighlightColor", Settings::ToString(a_user.navHighlight, false).c_str());
		a_ini.SetValue("ToRemove", "NavWindowingDimBGColor", Settings::ToString(a_user.navWindowingDimBg, false).c_str());
		a_ini.SetValue("ToRemove", "ModalWindowDimBGColor", Settings::ToString(a_user.modalWindowDimBg, false).c_str());

		a_ini.SetValue(rSections[Window], "WindowPadding", Settings::ToString(a_user.windowPadding, false).c_str());
		a_ini.SetValue(rSections[Frame], "FramePadding", Settings::ToString(a_user.framePadding, false).c_str());
		a_ini.SetValue(rSections[Table], "CellPadding", Settings::ToString(a_user.cellPadding, false).c_str());
		a_ini.SetValue(rSections[Window], "ItemSpacing", Settings::ToString(a_user.itemSpacing, false).c_str());
		a_ini.SetValue(rSections[Window], "ItemInnerSpacing", Settings::ToString(a_user.itemInnerSpacing, false).c_str());

		a_ini.SetValue(rSections[Window], "Alpha", Settings::ToString(a_user.alpha, false).c_str());
		a_ini.SetValue("ToRemove", "DisabledAlpha", Settings::ToString(a_user.disabledAlpha, false).c_str());
		a_ini.SetValue(rSections[Window], "WindowRounding", Settings::ToString(a_user.windowRounding, false).c_str());
		a_ini.SetValue(rSections[Window], "WindowBorderSize", Settings::ToString(a_user.windowBorderSize, false).c_str());
		a_ini.SetValue(rSections[Child], "ChildBorderSize", Settings::ToString(a_user.childBorderSize, false).c_str());
		a_ini.SetValue(rSections[Child], "ChildRounding", Settings::ToString(a_user.childRounding, false).c_str());
		a_ini.SetValue(rSections[Frame], "FrameBorderSize", Settings::ToString(a_user.frameBorderSize, false).c_str());
		a_ini.SetValue(rSections[Frame], "FrameRounding", Settings::ToString(a_user.frameRounding, false).c_str());
		a_ini.SetValue("ToRemove", "TabBorderSize", Settings::ToString(a_user.tabBorderSize, false).c_str());
		a_ini.SetValue("ToRemove", "TabRounding", Settings::ToString(a_user.tabRounding, false).c_str());
		a_ini.SetValue(rSections[Window], "IndentSpacing", Settings::ToString(a_user.indentSpacing, false).c_str());
		a_ini.SetValue(rSections[Table], "ColumnMinSpacing", Settings::ToString(a_user.columnsMinSpacing, false).c_str());
		a_ini.SetValue(rSections[Widgets], "ScrollbarRounding", Settings::ToString(a_user.scrollbarRounding, false).c_str());
		a_ini.SetValue(rSections[Widgets], "ScrollbarSize", Settings::ToString(a_user.scrollbarSize, false).c_str());
		a_ini.SetValue(rSections[Widgets], "GrabMinSize", Settings::ToString(a_user.grabMinSize, false).c_str());
		a_ini.SetValue(rSections[Widgets], "GrabRounding", Settings::ToString(a_user.grabRounding, false).c_str());
		a_ini.SetValue(rSections[Child], "PopupBorderSize", Settings::ToString(a_user.popupBorderSize, false).c_str());
		a_ini.SetValue(rSections[Child], "PopupRounding", Settings::ToString(a_user.popupRounding, false).c_str());
	});

	logger::info("Settings Saved");
};

// Call inside GetIni() to load the preset from the ini file into the user values.
// Executed within the scope of the presets/ directory.
void Settings::LoadPresetFromIni(CSimpleIniA& a_ini)
{
	std::pair<std::string, bool> callback;

	user.config.theme = GET_VALUE<std::string>("Config", "Theme", user.config.theme, a_ini);
	callback = Settings::SetThemeFromIni(user.config.theme);

	if (!callback.second) {
		if (callback.first == "Default") {
			stl::report_and_error("Failed to load default theme!\n\nEnsure the theme is present in the themes directory.\n\n(ModExplorerMenu/Interface/ModExplorerMenu/presets/Default.ini)");
		} else {
			a_ini.SetValue("Config", "Theme", "Default");
			LoadPresetFromIni(a_ini);
		}
	}
}

// Call inside GetIni() to load the theme from the ini file into the user values.
// Executed within the scope of the themes/ directory.
void Settings::LoadThemeFromIni(CSimpleIniA& a_ini)
{
	user.style.text = GET_VALUE<ImVec4>(rSections[Text], "TextColor", user.style.text, a_ini);
	user.style.textDisabled = GET_VALUE<ImVec4>(rSections[Text], "TextDisabledColor", user.style.textDisabled, a_ini);
	user.style.windowBg = GET_VALUE<ImVec4>(rSections[Window], "WindowBGColor", user.style.windowBg, a_ini);
	user.style.childBg = GET_VALUE<ImVec4>(rSections[Child], "ChildBGColor", user.style.childBg, a_ini);
	user.style.popupBg = GET_VALUE<ImVec4>(rSections[Child], "PopupBGColor", user.style.popupBg, a_ini);
	user.style.border = GET_VALUE<ImVec4>(rSections[Window], "BorderColor", user.style.border, a_ini);
	user.style.borderShadow = GET_VALUE<ImVec4>(rSections[Window], "BorderShadowColor", user.style.borderShadow, a_ini);
	user.style.frameBg = GET_VALUE<ImVec4>(rSections[Frame], "FrameBGColor", user.style.frameBg, a_ini);
	user.style.frameBgHovered = GET_VALUE<ImVec4>(rSections[Frame], "FrameBGHoveredColor", user.style.frameBgHovered, a_ini);
	user.style.frameBgActive = GET_VALUE<ImVec4>(rSections[Frame], "FrameBGActiveColor", user.style.frameBgActive, a_ini);
	user.style.menuBarBg = GET_VALUE<ImVec4>("ToRemove", "MenuBarColor", user.style.menuBarBg, a_ini);
	user.style.scrollbarBg = GET_VALUE<ImVec4>(rSections[Widgets], "ScrollbarBGColor", user.style.scrollbarBg, a_ini);
	user.style.scrollbarGrab = GET_VALUE<ImVec4>(rSections[Widgets], "ScrollbarGrabColor", user.style.scrollbarGrab, a_ini);
	user.style.scrollbarGrabHovered = GET_VALUE<ImVec4>(rSections[Widgets], "ScrollbarGrabHoveredColor", user.style.scrollbarGrabHovered, a_ini);
	user.style.scrollbarGrabActive = GET_VALUE<ImVec4>(rSections[Widgets], "ScrollbarGrabActiveColor", user.style.scrollbarGrabActive, a_ini);
	user.style.checkMark = GET_VALUE<ImVec4>(rSections[Widgets], "CheckMarkColor", user.style.checkMark, a_ini);
	user.style.sliderGrab = GET_VALUE<ImVec4>(rSections[Widgets], "SliderGrabColor", user.style.sliderGrab, a_ini);
	user.style.sliderGrabActive = GET_VALUE<ImVec4>(rSections[Widgets], "SliderGrabActiveColor", user.style.sliderGrabActive, a_ini);
	user.style.button = GET_VALUE<ImVec4>(rSections[Widgets], "ButtonColor", user.style.button, a_ini);
	user.style.buttonHovered = GET_VALUE<ImVec4>(rSections[Widgets], "ButtonHoveredColor", user.style.buttonHovered, a_ini);
	user.style.buttonActive = GET_VALUE<ImVec4>(rSections[Widgets], "ButtonActiveColor", user.style.buttonActive, a_ini);
	user.style.header = GET_VALUE<ImVec4>(rSections[Table], "HeaderColor", user.style.header, a_ini);
	user.style.headerHovered = GET_VALUE<ImVec4>(rSections[Table], "HeaderHoveredColor", user.style.headerHovered, a_ini);
	user.style.headerActive = GET_VALUE<ImVec4>(rSections[Table], "HeaderActiveColor", user.style.headerActive, a_ini);
	user.style.separator = GET_VALUE<ImVec4>(rSections[Widgets], "SeparatorColor", user.style.separator, a_ini);
	user.style.separatorHovered = GET_VALUE<ImVec4>(rSections[Widgets], "SeparatorHoveredColor", user.style.separatorHovered, a_ini);
	user.style.separatorActive = GET_VALUE<ImVec4>(rSections[Widgets], "SeparatorActiveColor", user.style.separatorActive, a_ini);
	user.style.resizeGrip = GET_VALUE<ImVec4>(rSections[Widgets], "ResizeGripColor", user.style.resizeGrip, a_ini);
	user.style.resizeGripHovered = GET_VALUE<ImVec4>(rSections[Widgets], "ResizeGripHoveredColor", user.style.resizeGripHovered, a_ini);
	user.style.resizeGripActive = GET_VALUE<ImVec4>(rSections[Widgets], "ResizeGripActiveColor", user.style.resizeGripActive, a_ini);
	user.style.tableHeaderBg = GET_VALUE<ImVec4>(rSections[Table], "TableHeaderBGColor", user.style.tableHeaderBg, a_ini);
	user.style.tableBorderStrong = GET_VALUE<ImVec4>(rSections[Table], "TableBorderStrongColor", user.style.tableBorderStrong, a_ini);
	user.style.tableBorderLight = GET_VALUE<ImVec4>(rSections[Table], "TableBorderLightColor", user.style.tableBorderLight, a_ini);
	user.style.tableRowBg = GET_VALUE<ImVec4>(rSections[Table], "TableRowBGColor", user.style.tableRowBg, a_ini);
	user.style.textSelectedBg = GET_VALUE<ImVec4>(rSections[Text], "TextSelectedBGColor", user.style.textSelectedBg, a_ini);
	user.style.navHighlight = GET_VALUE<ImVec4>("ToRemove", "NavHighlightColor", user.style.navHighlight, a_ini);
	user.style.navWindowingDimBg = GET_VALUE<ImVec4>("ToRemove", "NavWindowingDimBGColor", user.style.navWindowingDimBg, a_ini);
	user.style.modalWindowDimBg = GET_VALUE<ImVec4>("ToRemove", "ModalWindowDimBGColor", user.style.modalWindowDimBg, a_ini);

	user.style.windowPadding = GET_VALUE<ImVec2>(rSections[Window], "WindowPadding", user.style.windowPadding, a_ini);
	user.style.framePadding = GET_VALUE<ImVec2>(rSections[Frame], "FramePadding", user.style.framePadding, a_ini);
	user.style.cellPadding = GET_VALUE<ImVec2>(rSections[Table], "CellPadding", user.style.cellPadding, a_ini);
	user.style.itemSpacing = GET_VALUE<ImVec2>(rSections[Window], "ItemSpacing", user.style.itemSpacing, a_ini);
	user.style.itemInnerSpacing = GET_VALUE<ImVec2>(rSections[Window], "ItemInnerSpacing", user.style.itemInnerSpacing, a_ini);

	user.style.alpha = GET_VALUE<float>(rSections[Window], "Alpha", user.style.alpha, a_ini);
	user.style.disabledAlpha = GET_VALUE<float>("ToRemove", "DisabledAlpha", user.style.disabledAlpha, a_ini);
	user.style.windowRounding = GET_VALUE<float>(rSections[Window], "WindowRounding", user.style.windowRounding, a_ini);
	user.style.windowBorderSize = GET_VALUE<float>(rSections[Window], "WindowBorderSize", user.style.windowBorderSize, a_ini);
	user.style.childBorderSize = GET_VALUE<float>(rSections[Child], "ChildBorderSize", user.style.childBorderSize, a_ini);
	user.style.childRounding = GET_VALUE<float>(rSections[Child], "ChildRounding", user.style.childRounding, a_ini);
	user.style.frameBorderSize = GET_VALUE<float>(rSections[Frame], "FrameBorderSize", user.style.frameBorderSize, a_ini);
	user.style.frameRounding = GET_VALUE<float>(rSections[Frame], "FrameRounding", user.style.frameRounding, a_ini);
	user.style.tabBorderSize = GET_VALUE<float>("ToRemove", "TabBorderSize", user.style.tabBorderSize, a_ini);
	user.style.tabRounding = GET_VALUE<float>("ToRmove", "TabRounding", user.style.tabRounding, a_ini);
	user.style.indentSpacing = GET_VALUE<float>(rSections[Window], "IndentSpacing", user.style.indentSpacing, a_ini);
	user.style.columnsMinSpacing = GET_VALUE<float>(rSections[Table], "ColumnMinSpacing", user.style.columnsMinSpacing, a_ini);
	user.style.scrollbarRounding = GET_VALUE<float>(rSections[Widgets], "ScrollbarRounding", user.style.scrollbarRounding, a_ini);
	user.style.scrollbarSize = GET_VALUE<float>(rSections[Widgets], "ScrollbarSize", user.style.scrollbarSize, a_ini);
	user.style.grabMinSize = GET_VALUE<float>(rSections[Widgets], "GrabMinSize", user.style.grabMinSize, a_ini);
	user.style.grabRounding = GET_VALUE<float>(rSections[Widgets], "GrabRounding", user.style.grabRounding, a_ini);
	user.style.popupBorderSize = GET_VALUE<float>(rSections[Child], "PopupBorderSize", user.style.popupBorderSize, a_ini);
	user.style.popupRounding = GET_VALUE<float>(rSections[Child], "PopupRounding", user.style.popupRounding, a_ini);
}
