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
			(void)ini.SaveFile(a_path);  // Could not locate, let's procreate.
		}
	}

	a_func(ini);

	(void)ini.SaveFile(a_path);
}

bool ResetIni = false;  // TO-DO Implement overwrite mode.

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
		Settings::GetSingleton()->LoadConfiguration(a_ini);
	});
}

// Load the users configuration ini file. (master)
// Implemented fallback for improper theme name. Maybe overkill(?)
void Settings::LoadConfiguration(CSimpleIniA& a_ini, bool use_default)
{
	std::pair<std::string, bool> callback;

	if (use_default) {
		logger::info("(use_default=true) Loading default theme.");
		user.config.theme = "Default";
		a_ini.SetValue("Config", "Theme", user.config.theme.c_str());
		callback = Settings::SetThemeFromIni(user.config.theme);
	} else {
		user.config.theme = GET_VALUE<std::string>("Config", "Theme", user.config.theme, a_ini);
		logger::info("(use_default=false) Loading theme from ini: {}", user.config.theme);
		callback = Settings::SetThemeFromIni(user.config.theme);
	}

	if (!callback.second) {
		if (callback.first == "Default") {
			stl::report_and_fail("Failed to load default theme!\n\nEnsure the theme is present in the themes directory.");
		} else {
			logger::info("Failed to load theme: {}. Defaulting to Default theme.", callback.first);
			LoadConfiguration(a_ini, true);
		}
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
// 	user.style.plotLines = a_theme.Colors[ImGuiCol_PlotLines];
// 	user.style.plotLinesHovered = a_theme.Colors[ImGuiCol_PlotLinesHovered];
// 	user.style.plotHistogram = a_theme.Colors[ImGuiCol_PlotHistogram];
// 	user.style.plotHistogramHovered = a_theme.Colors[ImGuiCol_PlotHistogramHovered];
// 	user.style.textSelectedBg = a_theme.Colors[ImGuiCol_TextSelectedBg];
// 	user.style.dragDropTarget = a_theme.Colors[ImGuiCol_DragDropTarget];
// 	user.style.navHighlight = a_theme.Colors[ImGuiCol_NavHighlight];
// 	user.style.navWindowingDimBg = a_theme.Colors[ImGuiCol_NavWindowingDimBg];
// 	user.style.modalWindowDimBg = a_theme.Colors[ImGuiCol_ModalWindowDimBg];

// 	user.style.windowPadding = a_theme.WindowPadding;
// 	user.style.framePadding = a_theme.FramePadding;
// 	user.style.cellPadding = a_theme.CellPadding;
// 	user.style.itemSpacing = a_theme.ItemSpacing;
// 	user.style.itemInnerSpacing = a_theme.ItemInnerSpacing;
// 	user.style.touchExtraPadding = a_theme.TouchExtraPadding;

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
// 	user.style.scrollbarRounding = a_theme.ScrollbarRounding;
// 	user.style.scrollbarSize = a_theme.ScrollbarSize;
// 	user.style.grabMinSize = a_theme.GrabMinSize;
// 	user.style.grabRounding = a_theme.GrabRounding;
// 	user.style.popupBorderSize = a_theme.PopupBorderSize;
// 	user.style.popupRounding = a_theme.PopupRounding;
// 	user.style.logSliderDeadzone = a_theme.LogSliderDeadzone;
// }

// Export theme and style values to a standalone ini file.
// Decentralized from the main ini configuration to allow for easy sharing of themes.
void Settings::ExportThemeToIni(const wchar_t* a_path, Style& a_user)
{
	GetIni(a_path, [&a_user](CSimpleIniA& a_ini) {
		a_ini.SetValue("Style", "TextColor", Settings::ToString(a_user.text, false).c_str());
		a_ini.SetValue("Style", "TextDisabledColor", Settings::ToString(a_user.textDisabled, false).c_str());
		a_ini.SetValue("Style", "WindowBGColor", Settings::ToString(a_user.windowBg, false).c_str());
		a_ini.SetValue("Style", "ChildBGColor", Settings::ToString(a_user.childBg, false).c_str());
		a_ini.SetValue("Style", "PopupBGColor", Settings::ToString(a_user.popupBg, false).c_str());
		a_ini.SetValue("Style", "BorderColor", Settings::ToString(a_user.border, false).c_str());
		a_ini.SetValue("Style", "BorderShadowColor", Settings::ToString(a_user.borderShadow, false).c_str());
		a_ini.SetValue("Style", "FrameBGColor", Settings::ToString(a_user.frameBg, false).c_str());
		a_ini.SetValue("Style", "FrameBGHoveredColor", Settings::ToString(a_user.frameBgHovered, false).c_str());
		a_ini.SetValue("Style", "FrameBGActiveColor", Settings::ToString(a_user.frameBgActive, false).c_str());
		a_ini.SetValue("Style", "TitleBGColor", Settings::ToString(a_user.titleBg, false).c_str());
		a_ini.SetValue("Style", "TitleBGActiveColor", Settings::ToString(a_user.titleBgActive, false).c_str());
		a_ini.SetValue("Style", "TitleBGCollapsedColor", Settings::ToString(a_user.titleBgCollapsed, false).c_str());
		a_ini.SetValue("Style", "MenuBarColor", Settings::ToString(a_user.menuBarBg, false).c_str());
		a_ini.SetValue("Style", "ScrollbarBGColor", Settings::ToString(a_user.scrollbarBg, false).c_str());
		a_ini.SetValue("Style", "ScrollbarGrabColor", Settings::ToString(a_user.scrollbarGrab, false).c_str());
		a_ini.SetValue("Style", "ScrollbarGrabHoveredColor", Settings::ToString(a_user.scrollbarGrabHovered, false).c_str());
		a_ini.SetValue("Style", "ScrollbarGrabActiveColor", Settings::ToString(a_user.scrollbarGrabActive, false).c_str());
		a_ini.SetValue("Style", "CheckMarkColor", Settings::ToString(a_user.checkMark, false).c_str());
		a_ini.SetValue("Style", "SliderGrabColor", Settings::ToString(a_user.sliderGrab, false).c_str());
		a_ini.SetValue("Style", "SliderGrabActiveColor", Settings::ToString(a_user.sliderGrabActive, false).c_str());
		a_ini.SetValue("Style", "ButtonColor", Settings::ToString(a_user.button, false).c_str());
		a_ini.SetValue("Style", "ButtonHoveredColor", Settings::ToString(a_user.buttonHovered, false).c_str());
		a_ini.SetValue("Style", "ButtonActiveColor", Settings::ToString(a_user.buttonActive, false).c_str());
		a_ini.SetValue("Style", "HeaderColor", Settings::ToString(a_user.header, false).c_str());
		a_ini.SetValue("Style", "HeaderHoveredColor", Settings::ToString(a_user.headerHovered, false).c_str());
		a_ini.SetValue("Style", "HeaderActiveColor", Settings::ToString(a_user.headerActive, false).c_str());
		a_ini.SetValue("Style", "SeparatorColor", Settings::ToString(a_user.separator, false).c_str());
		a_ini.SetValue("Style", "SeparatorHoveredColor", Settings::ToString(a_user.separatorHovered, false).c_str());
		a_ini.SetValue("Style", "SeparatorActiveColor", Settings::ToString(a_user.separatorActive, false).c_str());
		a_ini.SetValue("Style", "ResizeGripColor", Settings::ToString(a_user.resizeGrip, false).c_str());
		a_ini.SetValue("Style", "ResizeGripHoveredColor", Settings::ToString(a_user.resizeGripHovered, false).c_str());
		a_ini.SetValue("Style", "ResizeGripActiveColor", Settings::ToString(a_user.resizeGripActive, false).c_str());
		a_ini.SetValue("Style", "TableHeaderBGColor", Settings::ToString(a_user.tableHeaderBg, false).c_str());
		a_ini.SetValue("Style", "TableBorderStrongColor", Settings::ToString(a_user.tableBorderStrong, false).c_str());
		a_ini.SetValue("Style", "TableBorderLightColor", Settings::ToString(a_user.tableBorderLight, false).c_str());
		a_ini.SetValue("Style", "TableRowBGColor", Settings::ToString(a_user.tableRowBg, false).c_str());
		a_ini.SetValue("Style", "PlotLinesColor", Settings::ToString(a_user.plotLines, false).c_str());
		a_ini.SetValue("Style", "PlotLinesHoveredColor", Settings::ToString(a_user.plotLinesHovered, false).c_str());
		a_ini.SetValue("Style", "PlotHistogramColor", Settings::ToString(a_user.plotHistogram, false).c_str());
		a_ini.SetValue("Style", "PlotHistogramHoveredColor", Settings::ToString(a_user.plotHistogramHovered, false).c_str());
		a_ini.SetValue("Style", "TextSelectedBGColor", Settings::ToString(a_user.textSelectedBg, false).c_str());
		a_ini.SetValue("Style", "DragDropTargetColor", Settings::ToString(a_user.dragDropTarget, false).c_str());
		a_ini.SetValue("Style", "NavHighlightColor", Settings::ToString(a_user.navHighlight, false).c_str());
		a_ini.SetValue("Style", "NavWindowingDimBGColor", Settings::ToString(a_user.navWindowingDimBg, false).c_str());
		a_ini.SetValue("Style", "ModalWindowDimBGColor", Settings::ToString(a_user.modalWindowDimBg, false).c_str());

		a_ini.SetValue("Style", "WindowPadding", Settings::ToString(a_user.windowPadding, false).c_str());
		a_ini.SetValue("Style", "FramePadding", Settings::ToString(a_user.framePadding, false).c_str());
		a_ini.SetValue("Style", "CellPadding", Settings::ToString(a_user.cellPadding, false).c_str());
		a_ini.SetValue("Style", "ItemSpacing", Settings::ToString(a_user.itemSpacing, false).c_str());
		a_ini.SetValue("Style", "ItemInnerSpacing", Settings::ToString(a_user.itemInnerSpacing, false).c_str());
		a_ini.SetValue("Style", "TouchExtraPadding", Settings::ToString(a_user.touchExtraPadding, false).c_str());

		a_ini.SetValue("Style", "Alpha", std::to_string(a_user.alpha).c_str());
		a_ini.SetValue("Style", "DisabledAlpha", std::to_string(a_user.disabledAlpha).c_str());
		a_ini.SetValue("Style", "WindowRounding", std::to_string(a_user.windowRounding).c_str());
		a_ini.SetValue("Style", "WindowBorderSize", std::to_string(a_user.windowBorderSize).c_str());
		a_ini.SetValue("Style", "ChildBorderSize", std::to_string(a_user.childBorderSize).c_str());
		a_ini.SetValue("Style", "ChildRounding", std::to_string(a_user.childRounding).c_str());
		a_ini.SetValue("Style", "FrameBorderSize", std::to_string(a_user.frameBorderSize).c_str());
		a_ini.SetValue("Style", "FrameRounding", std::to_string(a_user.frameRounding).c_str());
		a_ini.SetValue("Style", "TabBorderSize", std::to_string(a_user.tabBorderSize).c_str());
		a_ini.SetValue("Style", "TabRounding", std::to_string(a_user.tabRounding).c_str());
		a_ini.SetValue("Style", "IndentSpacing", std::to_string(a_user.indentSpacing).c_str());
		a_ini.SetValue("Style", "ScrollbarRounding", std::to_string(a_user.scrollbarRounding).c_str());
		a_ini.SetValue("Style", "ScrollbarSize", std::to_string(a_user.scrollbarSize).c_str());
		a_ini.SetValue("Style", "GrabMinSize", std::to_string(a_user.grabMinSize).c_str());
		a_ini.SetValue("Style", "GrabRounding", std::to_string(a_user.grabRounding).c_str());
		a_ini.SetValue("Style", "PopupBorderSize", std::to_string(a_user.popupBorderSize).c_str());
		a_ini.SetValue("Style", "PopupRounding", std::to_string(a_user.popupRounding).c_str());
		a_ini.SetValue("Style", "LogSliderDeadzone", std::to_string(a_user.logSliderDeadzone).c_str());
	});

	logger::info("Settings Saved");
};

// Call inside GetIni() to load the theme from the ini file into the user values.
void Settings::LoadThemeFromIni(CSimpleIniA& a_ini)
{
	user.style.text = GET_VALUE<ImVec4>("Style", "TextColor", user.style.text, a_ini);
	user.style.textDisabled = GET_VALUE<ImVec4>("Style", "TextDisabledColor", user.style.textDisabled, a_ini);
	user.style.windowBg = GET_VALUE<ImVec4>("Style", "WindowBGColor", user.style.windowBg, a_ini);
	user.style.childBg = GET_VALUE<ImVec4>("Style", "ChildBGColor", user.style.childBg, a_ini);
	user.style.popupBg = GET_VALUE<ImVec4>("Style", "PopupBGColor", user.style.popupBg, a_ini);
	user.style.border = GET_VALUE<ImVec4>("Style", "BorderColor", user.style.border, a_ini);
	user.style.borderShadow = GET_VALUE<ImVec4>("Style", "BorderShadowColor", user.style.borderShadow, a_ini);
	user.style.frameBg = GET_VALUE<ImVec4>("Style", "FrameBGColor", user.style.frameBg, a_ini);
	user.style.frameBgHovered = GET_VALUE<ImVec4>("Style", "FrameBGHoveredColor", user.style.frameBgHovered, a_ini);
	user.style.frameBgActive = GET_VALUE<ImVec4>("Style", "FrameBGActiveColor", user.style.frameBgActive, a_ini);
	user.style.titleBg = GET_VALUE<ImVec4>("Style", "TitleBGColor", user.style.titleBg, a_ini);
	user.style.titleBgActive = GET_VALUE<ImVec4>("Style", "TitleBGActiveColor", user.style.titleBgActive, a_ini);
	user.style.titleBgCollapsed = GET_VALUE<ImVec4>("Style", "TitleBGCollapsedColor", user.style.titleBgCollapsed, a_ini);
	user.style.menuBarBg = GET_VALUE<ImVec4>("Style", "MenuBarColor", user.style.menuBarBg, a_ini);
	user.style.scrollbarBg = GET_VALUE<ImVec4>("Style", "ScrollbarBGColor", user.style.scrollbarBg, a_ini);
	user.style.scrollbarGrab = GET_VALUE<ImVec4>("Style", "ScrollbarGrabColor", user.style.scrollbarGrab, a_ini);
	user.style.scrollbarGrabHovered = GET_VALUE<ImVec4>("Style", "ScrollbarGrabHoveredColor", user.style.scrollbarGrabHovered, a_ini);
	user.style.scrollbarGrabActive = GET_VALUE<ImVec4>("Style", "ScrollbarGrabActiveColor", user.style.scrollbarGrabActive, a_ini);
	user.style.checkMark = GET_VALUE<ImVec4>("Style", "CheckMarkColor", user.style.checkMark, a_ini);
	user.style.sliderGrab = GET_VALUE<ImVec4>("Style", "SliderGrabColor", user.style.sliderGrab, a_ini);
	user.style.sliderGrabActive = GET_VALUE<ImVec4>("Style", "SliderGrabActiveColor", user.style.sliderGrabActive, a_ini);
	user.style.button = GET_VALUE<ImVec4>("Style", "ButtonColor", user.style.button, a_ini);
	user.style.buttonHovered = GET_VALUE<ImVec4>("Style", "ButtonHoveredColor", user.style.buttonHovered, a_ini);
	user.style.buttonActive = GET_VALUE<ImVec4>("Style", "ButtonActiveColor", user.style.buttonActive, a_ini);
	user.style.header = GET_VALUE<ImVec4>("Style", "HeaderColor", user.style.header, a_ini);
	user.style.headerHovered = GET_VALUE<ImVec4>("Style", "HeaderHoveredColor", user.style.headerHovered, a_ini);
	user.style.headerActive = GET_VALUE<ImVec4>("Style", "HeaderActiveColor", user.style.headerActive, a_ini);
	user.style.separator = GET_VALUE<ImVec4>("Style", "SeparatorColor", user.style.separator, a_ini);
	user.style.separatorHovered = GET_VALUE<ImVec4>("Style", "SeparatorHoveredColor", user.style.separatorHovered, a_ini);
	user.style.separatorActive = GET_VALUE<ImVec4>("Style", "SeparatorActiveColor", user.style.separatorActive, a_ini);
	user.style.resizeGrip = GET_VALUE<ImVec4>("Style", "ResizeGripColor", user.style.resizeGrip, a_ini);
	user.style.resizeGripHovered = GET_VALUE<ImVec4>("Style", "ResizeGripHoveredColor", user.style.resizeGripHovered, a_ini);
	user.style.resizeGripActive = GET_VALUE<ImVec4>("Style", "ResizeGripActiveColor", user.style.resizeGripActive, a_ini);
	user.style.tableHeaderBg = GET_VALUE<ImVec4>("Style", "TableHeaderBGColor", user.style.tableHeaderBg, a_ini);
	user.style.tableBorderStrong = GET_VALUE<ImVec4>("Style", "TableBorderStrongColor", user.style.tableBorderStrong, a_ini);
	user.style.tableBorderLight = GET_VALUE<ImVec4>("Style", "TableBorderLightColor", user.style.tableBorderLight, a_ini);
	user.style.tableRowBg = GET_VALUE<ImVec4>("Style", "TableRowBGColor", user.style.tableRowBg, a_ini);
	user.style.plotLines = GET_VALUE<ImVec4>("Style", "PlotLinesColor", user.style.plotLines, a_ini);
	user.style.plotLinesHovered = GET_VALUE<ImVec4>("Style", "PlotLinesHoveredColor", user.style.plotLinesHovered, a_ini);
	user.style.plotHistogram = GET_VALUE<ImVec4>("Style", "PlotHistogramColor", user.style.plotHistogram, a_ini);
	user.style.plotHistogramHovered = GET_VALUE<ImVec4>("Style", "PlotHistogramHoveredColor", user.style.plotHistogramHovered, a_ini);
	user.style.textSelectedBg = GET_VALUE<ImVec4>("Style", "TextSelectedBGColor", user.style.textSelectedBg, a_ini);
	user.style.dragDropTarget = GET_VALUE<ImVec4>("Style", "DragDropTargetColor", user.style.dragDropTarget, a_ini);
	user.style.navHighlight = GET_VALUE<ImVec4>("Style", "NavHighlightColor", user.style.navHighlight, a_ini);
	user.style.navWindowingDimBg = GET_VALUE<ImVec4>("Style", "NavWindowingDimBGColor", user.style.navWindowingDimBg, a_ini);
	user.style.modalWindowDimBg = GET_VALUE<ImVec4>("Style", "ModalWindowDimBGColor", user.style.modalWindowDimBg, a_ini);

	user.style.windowPadding = GET_VALUE<ImVec2>("Style", "WindowPadding", user.style.windowPadding, a_ini);
	user.style.framePadding = GET_VALUE<ImVec2>("Style", "FramePadding", user.style.framePadding, a_ini);
	user.style.cellPadding = GET_VALUE<ImVec2>("Style", "CellPadding", user.style.cellPadding, a_ini);
	user.style.itemSpacing = GET_VALUE<ImVec2>("Style", "ItemSpacing", user.style.itemSpacing, a_ini);
	user.style.itemInnerSpacing = GET_VALUE<ImVec2>("Style", "ItemInnerSpacing", user.style.itemInnerSpacing, a_ini);
	user.style.touchExtraPadding = GET_VALUE<ImVec2>("Style", "TouchExtraPadding", user.style.touchExtraPadding, a_ini);

	user.style.alpha = GET_VALUE<float>("Style", "Alpha", user.style.alpha, a_ini);
	user.style.disabledAlpha = GET_VALUE<float>("Style", "DisabledAlpha", user.style.disabledAlpha, a_ini);
	user.style.windowRounding = GET_VALUE<float>("Style", "WindowRounding", user.style.windowRounding, a_ini);
	user.style.windowBorderSize = GET_VALUE<float>("Style", "WindowBorderSize", user.style.windowBorderSize, a_ini);
	user.style.childBorderSize = GET_VALUE<float>("Style", "ChildBorderSize", user.style.childBorderSize, a_ini);
	user.style.childRounding = GET_VALUE<float>("Style", "ChildRounding", user.style.childRounding, a_ini);
	user.style.frameBorderSize = GET_VALUE<float>("Style", "FrameBorderSize", user.style.frameBorderSize, a_ini);
	user.style.frameRounding = GET_VALUE<float>("Style", "FrameRounding", user.style.frameRounding, a_ini);
	user.style.tabBorderSize = GET_VALUE<float>("Style", "TabBorderSize", user.style.tabBorderSize, a_ini);
	user.style.tabRounding = GET_VALUE<float>("Style", "TabRounding", user.style.tabRounding, a_ini);
	user.style.indentSpacing = GET_VALUE<float>("Style", "IndentSpacing", user.style.indentSpacing, a_ini);
	user.style.scrollbarRounding = GET_VALUE<float>("Style", "ScrollbarRounding", user.style.scrollbarRounding, a_ini);
	user.style.scrollbarSize = GET_VALUE<float>("Style", "ScrollbarSize", user.style.scrollbarSize, a_ini);
	user.style.grabMinSize = GET_VALUE<float>("Style", "GrabMinSize", user.style.grabMinSize, a_ini);
	user.style.grabRounding = GET_VALUE<float>("Style", "GrabRounding", user.style.grabRounding, a_ini);
	user.style.popupBorderSize = GET_VALUE<float>("Style", "PopupBorderSize", user.style.popupBorderSize, a_ini);
	user.style.popupRounding = GET_VALUE<float>("Style", "PopupRounding", user.style.popupRounding, a_ini);
	user.style.logSliderDeadzone = GET_VALUE<float>("Style", "LogSliderDeadzone", user.style.logSliderDeadzone, a_ini);
}
