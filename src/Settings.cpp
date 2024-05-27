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
			(void)ini.SaveFile(a_path);
			//stl::report_and_fail("Failed to load ModExplorerMenu.ini file!"sv);
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
	} else if constexpr (std::is_same_v<T, ImVec2>) {
		auto vec = Settings::GetVec2(value);
		return (vec.second ? vec.first : a_default);
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

// Used to load a specific theme into uservalues.
void Settings::LoadStyleTheme(ImGuiStyle a_theme)
{
	user.text = a_theme.Colors[ImGuiCol_Text];
	user.textDisabled = a_theme.Colors[ImGuiCol_TextDisabled];
	user.windowBg = a_theme.Colors[ImGuiCol_WindowBg];
	user.childBg = a_theme.Colors[ImGuiCol_ChildBg];
	user.popupBg = a_theme.Colors[ImGuiCol_PopupBg];
	user.border = a_theme.Colors[ImGuiCol_Border];
	user.borderShadow = a_theme.Colors[ImGuiCol_BorderShadow];
	user.frameBg = a_theme.Colors[ImGuiCol_FrameBg];
	user.frameBgHovered = a_theme.Colors[ImGuiCol_FrameBgHovered];
	user.frameBgActive = a_theme.Colors[ImGuiCol_FrameBgActive];
	user.titleBg = a_theme.Colors[ImGuiCol_TitleBg];
	user.titleBgActive = a_theme.Colors[ImGuiCol_TitleBgActive];
	user.titleBgCollapsed = a_theme.Colors[ImGuiCol_TitleBgCollapsed];
	user.menuBarBg = a_theme.Colors[ImGuiCol_MenuBarBg];
	user.scrollbarBg = a_theme.Colors[ImGuiCol_ScrollbarBg];
	user.scrollbarGrab = a_theme.Colors[ImGuiCol_ScrollbarGrab];
	user.scrollbarGrabHovered = a_theme.Colors[ImGuiCol_ScrollbarGrabHovered];
	user.scrollbarGrabActive = a_theme.Colors[ImGuiCol_ScrollbarGrabActive];
	user.checkMark = a_theme.Colors[ImGuiCol_CheckMark];
	user.sliderGrab = a_theme.Colors[ImGuiCol_SliderGrab];
	user.sliderGrabActive = a_theme.Colors[ImGuiCol_SliderGrabActive];
	user.button = a_theme.Colors[ImGuiCol_Button];
	user.buttonHovered = a_theme.Colors[ImGuiCol_ButtonHovered];
	user.buttonActive = a_theme.Colors[ImGuiCol_ButtonActive];
	user.header = a_theme.Colors[ImGuiCol_Header];
	user.headerHovered = a_theme.Colors[ImGuiCol_HeaderHovered];
	user.headerActive = a_theme.Colors[ImGuiCol_HeaderActive];
	user.separator = a_theme.Colors[ImGuiCol_Separator];
	user.separatorHovered = a_theme.Colors[ImGuiCol_SeparatorHovered];
	user.separatorActive = a_theme.Colors[ImGuiCol_SeparatorActive];
	user.resizeGrip = a_theme.Colors[ImGuiCol_ResizeGrip];
	user.resizeGripHovered = a_theme.Colors[ImGuiCol_ResizeGripHovered];
	user.resizeGripActive = a_theme.Colors[ImGuiCol_ResizeGripActive];
	user.tableHeaderBg = a_theme.Colors[ImGuiCol_TableHeaderBg];
	user.tableBorderStrong = a_theme.Colors[ImGuiCol_TableBorderStrong];
	user.tableBorderLight = a_theme.Colors[ImGuiCol_TableBorderLight];
	user.tableRowBg = a_theme.Colors[ImGuiCol_TableRowBg];
	user.plotLines = a_theme.Colors[ImGuiCol_PlotLines];
	user.plotLinesHovered = a_theme.Colors[ImGuiCol_PlotLinesHovered];
	user.plotHistogram = a_theme.Colors[ImGuiCol_PlotHistogram];
	user.plotHistogramHovered = a_theme.Colors[ImGuiCol_PlotHistogramHovered];
	user.textSelectedBg = a_theme.Colors[ImGuiCol_TextSelectedBg];
	user.dragDropTarget = a_theme.Colors[ImGuiCol_DragDropTarget];
	user.navHighlight = a_theme.Colors[ImGuiCol_NavHighlight];
	user.navWindowingDimBg = a_theme.Colors[ImGuiCol_NavWindowingDimBg];
	user.modalWindowDimBg = a_theme.Colors[ImGuiCol_ModalWindowDimBg];

	user.windowPadding = a_theme.WindowPadding;
	user.framePadding = a_theme.FramePadding;
	user.cellPadding = a_theme.CellPadding;
	user.itemSpacing = a_theme.ItemSpacing;
	user.itemInnerSpacing = a_theme.ItemInnerSpacing;
	user.touchExtraPadding = a_theme.TouchExtraPadding;

	user.alpha = a_theme.Alpha;
	user.disabledAlpha = a_theme.Alpha;
	user.windowRounding = a_theme.WindowRounding;
	user.windowBorderSize = a_theme.WindowBorderSize;
	user.childBorderSize = a_theme.ChildBorderSize;
	user.childRounding = a_theme.ChildRounding;
	user.frameBorderSize = a_theme.FrameBorderSize;
	user.frameRounding = a_theme.FrameRounding;
	user.tabBorderSize = a_theme.TabBorderSize;
	user.tabRounding = a_theme.TabRounding;
	user.indentSpacing = a_theme.IndentSpacing;
	user.scrollbarRounding = a_theme.ScrollbarRounding;
	user.scrollbarSize = a_theme.ScrollbarSize;
	user.grabMinSize = a_theme.GrabMinSize;
	user.grabRounding = a_theme.GrabRounding;
	user.popupBorderSize = a_theme.PopupBorderSize;
	user.popupRounding = a_theme.PopupRounding;
	user.logSliderDeadzone = a_theme.LogSliderDeadzone;
}

void Settings::SaveStyleThemeToIni(const wchar_t* a_path, Style& a_user)
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

// Executed on initialization to load user settings from ini.
// Not to be used during modification or saving.
void Settings::LoadStyleSettings(CSimpleIniA& a_ini)
{
	user.text = GET_VALUE<ImVec4>("Style", "TextColor", user.text, a_ini);
	user.textDisabled = GET_VALUE<ImVec4>("Style", "TextDisabledColor", user.textDisabled, a_ini);
	user.windowBg = GET_VALUE<ImVec4>("Style", "WindowBGColor", user.windowBg, a_ini);
	user.childBg = GET_VALUE<ImVec4>("Style", "ChildBGColor", user.childBg, a_ini);
	user.popupBg = GET_VALUE<ImVec4>("Style", "PopupBGColor", user.popupBg, a_ini);
	user.border = GET_VALUE<ImVec4>("Style", "BorderColor", user.border, a_ini);
	user.borderShadow = GET_VALUE<ImVec4>("Style", "BorderShadowColor", user.borderShadow, a_ini);
	user.frameBg = GET_VALUE<ImVec4>("Style", "FrameBGColor", user.frameBg, a_ini);
	user.frameBgHovered = GET_VALUE<ImVec4>("Style", "FrameBGHoveredColor", user.frameBgHovered, a_ini);
	user.frameBgActive = GET_VALUE<ImVec4>("Style", "FrameBGActiveColor", user.frameBgActive, a_ini);
	user.titleBg = GET_VALUE<ImVec4>("Style", "TitleBGColor", user.titleBg, a_ini);
	user.titleBgActive = GET_VALUE<ImVec4>("Style", "TitleBGActiveColor", user.titleBgActive, a_ini);
	user.titleBgCollapsed = GET_VALUE<ImVec4>("Style", "TitleBGCollapsedColor", user.titleBgCollapsed, a_ini);
	user.menuBarBg = GET_VALUE<ImVec4>("Style", "MenuBarColor", user.menuBarBg, a_ini);
	user.scrollbarBg = GET_VALUE<ImVec4>("Style", "ScrollbarBGColor", user.scrollbarBg, a_ini);
	user.scrollbarGrab = GET_VALUE<ImVec4>("Style", "ScrollbarGrabColor", user.scrollbarGrab, a_ini);
	user.scrollbarGrabHovered = GET_VALUE<ImVec4>("Style", "ScrollbarGrabHoveredColor", user.scrollbarGrabHovered, a_ini);
	user.scrollbarGrabActive = GET_VALUE<ImVec4>("Style", "ScrollbarGrabActiveColor", user.scrollbarGrabActive, a_ini);
	user.checkMark = GET_VALUE<ImVec4>("Style", "CheckMarkColor", user.checkMark, a_ini);
	user.sliderGrab = GET_VALUE<ImVec4>("Style", "SliderGrabColor", user.sliderGrab, a_ini);
	user.sliderGrabActive = GET_VALUE<ImVec4>("Style", "SliderGrabActiveColor", user.sliderGrabActive, a_ini);
	user.button = GET_VALUE<ImVec4>("Style", "ButtonColor", user.button, a_ini);
	user.buttonHovered = GET_VALUE<ImVec4>("Style", "ButtonHoveredColor", user.buttonHovered, a_ini);
	user.buttonActive = GET_VALUE<ImVec4>("Style", "ButtonActiveColor", user.buttonActive, a_ini);
	user.header = GET_VALUE<ImVec4>("Style", "HeaderColor", user.header, a_ini);
	user.headerHovered = GET_VALUE<ImVec4>("Style", "HeaderHoveredColor", user.headerHovered, a_ini);
	user.headerActive = GET_VALUE<ImVec4>("Style", "HeaderActiveColor", user.headerActive, a_ini);
	user.separator = GET_VALUE<ImVec4>("Style", "SeparatorColor", user.separator, a_ini);
	user.separatorHovered = GET_VALUE<ImVec4>("Style", "SeparatorHoveredColor", user.separatorHovered, a_ini);
	user.separatorActive = GET_VALUE<ImVec4>("Style", "SeparatorActiveColor", user.separatorActive, a_ini);
	user.resizeGrip = GET_VALUE<ImVec4>("Style", "ResizeGripColor", user.resizeGrip, a_ini);
	user.resizeGripHovered = GET_VALUE<ImVec4>("Style", "ResizeGripHoveredColor", user.resizeGripHovered, a_ini);
	user.resizeGripActive = GET_VALUE<ImVec4>("Style", "ResizeGripActiveColor", user.resizeGripActive, a_ini);
	user.tableHeaderBg = GET_VALUE<ImVec4>("Style", "TableHeaderBGColor", user.tableHeaderBg, a_ini);
	user.tableBorderStrong = GET_VALUE<ImVec4>("Style", "TableBorderStrongColor", user.tableBorderStrong, a_ini);
	user.tableBorderLight = GET_VALUE<ImVec4>("Style", "TableBorderLightColor", user.tableBorderLight, a_ini);
	user.tableRowBg = GET_VALUE<ImVec4>("Style", "TableRowBGColor", user.tableRowBg, a_ini);
	user.plotLines = GET_VALUE<ImVec4>("Style", "PlotLinesColor", user.plotLines, a_ini);
	user.plotLinesHovered = GET_VALUE<ImVec4>("Style", "PlotLinesHoveredColor", user.plotLinesHovered, a_ini);
	user.plotHistogram = GET_VALUE<ImVec4>("Style", "PlotHistogramColor", user.plotHistogram, a_ini);
	user.plotHistogramHovered = GET_VALUE<ImVec4>("Style", "PlotHistogramHoveredColor", user.plotHistogramHovered, a_ini);
	user.textSelectedBg = GET_VALUE<ImVec4>("Style", "TextSelectedBGColor", user.textSelectedBg, a_ini);
	user.dragDropTarget = GET_VALUE<ImVec4>("Style", "DragDropTargetColor", user.dragDropTarget, a_ini);
	user.navHighlight = GET_VALUE<ImVec4>("Style", "NavHighlightColor", user.navHighlight, a_ini);
	user.navWindowingDimBg = GET_VALUE<ImVec4>("Style", "NavWindowingDimBGColor", user.navWindowingDimBg, a_ini);
	user.modalWindowDimBg = GET_VALUE<ImVec4>("Style", "ModalWindowDimBGColor", user.modalWindowDimBg, a_ini);

	user.windowPadding = GET_VALUE<ImVec2>("Style", "WindowPadding", user.windowPadding, a_ini);
	user.framePadding = GET_VALUE<ImVec2>("Style", "FramePadding", user.framePadding, a_ini);
	user.cellPadding = GET_VALUE<ImVec2>("Style", "CellPadding", user.cellPadding, a_ini);
	user.itemSpacing = GET_VALUE<ImVec2>("Style", "ItemSpacing", user.itemSpacing, a_ini);
	user.itemInnerSpacing = GET_VALUE<ImVec2>("Style", "ItemInnerSpacing", user.itemInnerSpacing, a_ini);
	user.touchExtraPadding = GET_VALUE<ImVec2>("Style", "TouchExtraPadding", user.touchExtraPadding, a_ini);

	user.alpha = GET_VALUE<float>("Style", "Alpha", user.alpha, a_ini);
	user.disabledAlpha = GET_VALUE<float>("Style", "DisabledAlpha", user.disabledAlpha, a_ini);
	user.windowRounding = GET_VALUE<float>("Style", "WindowRounding", user.windowRounding, a_ini);
	user.windowBorderSize = GET_VALUE<float>("Style", "WindowBorderSize", user.windowBorderSize, a_ini);
	user.childBorderSize = GET_VALUE<float>("Style", "ChildBorderSize", user.childBorderSize, a_ini);
	user.childRounding = GET_VALUE<float>("Style", "ChildRounding", user.childRounding, a_ini);
	user.frameBorderSize = GET_VALUE<float>("Style", "FrameBorderSize", user.frameBorderSize, a_ini);
	user.frameRounding = GET_VALUE<float>("Style", "FrameRounding", user.frameRounding, a_ini);
	user.tabBorderSize = GET_VALUE<float>("Style", "TabBorderSize", user.tabBorderSize, a_ini);
	user.tabRounding = GET_VALUE<float>("Style", "TabRounding", user.tabRounding, a_ini);
	user.indentSpacing = GET_VALUE<float>("Style", "IndentSpacing", user.indentSpacing, a_ini);
	user.scrollbarRounding = GET_VALUE<float>("Style", "ScrollbarRounding", user.scrollbarRounding, a_ini);
	user.scrollbarSize = GET_VALUE<float>("Style", "ScrollbarSize", user.scrollbarSize, a_ini);
	user.grabMinSize = GET_VALUE<float>("Style", "GrabMinSize", user.grabMinSize, a_ini);
	user.grabRounding = GET_VALUE<float>("Style", "GrabRounding", user.grabRounding, a_ini);
	user.popupBorderSize = GET_VALUE<float>("Style", "PopupBorderSize", user.popupBorderSize, a_ini);
	user.popupRounding = GET_VALUE<float>("Style", "PopupRounding", user.popupRounding, a_ini);
	user.logSliderDeadzone = GET_VALUE<float>("Style", "LogSliderDeadzone", user.logSliderDeadzone, a_ini);

	Menu::GetSingleton()->SetupStyle(user);
}
