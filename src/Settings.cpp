#include "include/S/Settings.h"
#include "SimpleIni.h"
#include "include/I/INI.h"
#include "include/I/InputManager.h"
#include "include/M/Menu.h"
#include "include/U/Util.h"
#include <format>

using namespace IniHelper;

namespace Modex
{

	void Settings::GetIni(const std::filesystem::path& a_path, const std::function<void(CSimpleIniA&)> a_func)
	{
		CSimpleIniA ini;

		if (a_path.empty()) {
			logger::critical("[Settings] Invalid Path provided to GetIni");
		}

		// This is okay since we're interfacing with an external API
		const std::wstring wide_path = a_path.wstring();

		ini.SetUnicode();

		if (const auto rc = ini.LoadFile(wide_path.c_str())) {
			if (rc < SI_OK) {
				(void)ini.SaveFile(wide_path.c_str());  // Could not locate, let's procreate.
			}
		}

		a_func(ini);

		(void)ini.SaveFile(wide_path.c_str());
	}

	template <class T>
	T GET_VALUE(const char* section, const char* key, T a_default, CSimpleIniA& a_ini)
	{
		std::string value;

		if (a_ini.GetValue(section, key) == nullptr) {
			a_ini.SetValue(section, key, Settings::ToString(a_default, false).c_str());
			return a_default;
		} else {
			value = a_ini.GetValue(section, key);
		}

		if (value.empty()) {
			logger::warn("[Settings] Failed to parse value from .ini file! Ensure you're using the correct format!");
			return a_default;
		}

		// A+ plus de-serialization.
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
		} else if constexpr (std::is_same_v<T, uint32_t>) {
			return Settings::GetUInt(value);
		} else if constexpr (std::is_same_v<T, float>) {
			return Settings::GetFloat(value);
		} else if constexpr (std::is_same_v<T, GraphicManager::Image>) {
			return GraphicManager::GetImage(value);
		} else if constexpr (std::is_same_v<T, Language::GlyphRanges>) {
			return Language::GetGlyphRange(value);
		} else {
			logger::error("[Settings] Unhandled type passed to GET_VALUE in Menu.cpp!");
			return a_default;
		}
	}

	void Settings::FormatMasterIni(CSimpleIniA& a_ini)
	{
		a_ini.SetValue(rSections[Main], NULL, NULL, GetHeader(iHeader::MasterPresetHeader));
		a_ini.SetValue(rSections[Modules], NULL, NULL, GetHeader(iHeader::PresetMainModules));
	}

	// Call on new ini file to generate comments and sections before inserting.
	void Settings::FormatThemeIni(CSimpleIniA& a_ini)
	{
		a_ini.SetValue(rSections[Window], NULL, NULL, GetHeader(iHeader::ThemeConfigHeader));
		a_ini.SetValue(rSections[Text], NULL, NULL, GetHeader(iHeader::ThemeTextHeader));
		a_ini.SetValue(rSections[Table], NULL, NULL, GetHeader(iHeader::ThemeTableHeader));
		a_ini.SetValue(rSections[Widgets], NULL, NULL, GetHeader(iHeader::ThemeWidgetsHeader));
		a_ini.SetValue(rSections[Images], NULL, NULL, GetHeader(iHeader::ThemeImageHeader));
	}

	// Master ini has no defaults, they're established here. Theme defaults are derived from def class.
	void Settings::CreateDefaultMaster()
	{
		GetIni(L"Data/Interface/Modex/Modex.ini", [](CSimpleIniA& a_ini) {
			FormatMasterIni(a_ini);

			auto& _default = Settings::GetSingleton()->def.config;

			// General
			a_ini.SetValue(rSections[Main], "Theme", _default.theme.c_str(), GetComment(iComment::ConfigTheme));
			a_ini.SetValue(rSections[Main], "ShowMenuKey", std::to_string(_default.showMenuKey).c_str(), GetComment(iComment::ConfigShowMenuKey));
			a_ini.SetValue(rSections[Main], "ShowMenuModifier", std::to_string(_default.showMenuModifier).c_str(), GetComment(iComment::ConfigShowMenuModifier));
			a_ini.SetValue(rSections[Main], "ModListSort", std::to_string(_default.modListSort).c_str(), GetComment(iComment::ConfigModListSort));
			a_ini.SetValue(rSections[Main], "UI Scale", std::to_string(_default.uiScale).c_str(), GetComment(iComment::ConfigUIScale));
			a_ini.SetValue(rSections[Main], "Fullscreen", ToString(_default.fullscreen).c_str(), GetComment(iComment::ConfigFullscreen));
			a_ini.SetValue(rSections[Main], "PauseGame", ToString(_default.pauseGame).c_str(), GetComment(iComment::ConfigPauseGame));
			a_ini.SetValue(rSections[Main], "DisableInMenu", ToString(_default.disableInMenu).c_str(), GetComment(iComment::ConfigDisableInMenu));

			// Font & Localization
			a_ini.SetValue(rSections[Main], "Language", ToString(_default.language, false).c_str(), GetComment(iComment::ConfigLanguage));
			a_ini.SetValue(rSections[Main], "GlyphRange", Language::GetGlyphName(_default.glyphRange).c_str(), GetComment(iComment::ConfigGlyphRange));
			a_ini.SetValue(rSections[Main], "GlobalFont", _default.globalFont.c_str(), GetComment(iComment::ConfigGlobalFont));
			a_ini.SetValue(rSections[Main], "GlobalFontSize", std::to_string(_default.globalFontSize).c_str(), GetComment(iComment::ConfigGlobalFontSize));

			// Modules
			a_ini.SetValue(rSections[Modules], "DefaultShow", std::to_string(_default.defaultShow).c_str(), GetComment(iComment::ConfigDefaultShow));
			a_ini.SetValue(rSections[Modules], "ShowHomeMenu", ToString(_default.showHomeMenu).c_str(), GetComment(iComment::ConfigShowHomeMenu));
			a_ini.SetValue(rSections[Modules], "ShowAddItemMenu", ToString(_default.showAddItemMenu).c_str(), GetComment(iComment::ConfigShowAddItemMenu));
			a_ini.SetValue(rSections[Modules], "ShowObjectMenu", ToString(_default.showObjectMenu).c_str(), GetComment(iComment::ConfigShowObjectMenu));
			a_ini.SetValue(rSections[Modules], "ShowNPCMenu", ToString(_default.showNPCMenu).c_str(), GetComment(iComment::ConfigShowNPCMenu));
			a_ini.SetValue(rSections[Modules], "ShowTeleportMenu", ToString(_default.showTeleportMenu).c_str(), GetComment(iComment::ConfigShowTeleportMenu));
		});
	}

	// Execute ini value assignment where necessary.
	void Settings::LoadSettings(const std::filesystem::path& a_path)
	{
		// If master ini doesn't exist this will default it to "Default"
		if (!std::filesystem::exists(a_path)) {
			logger::warn("[Settings] Master ini not found! Creating default master ini...");

			CreateDefaultMaster();
		}

		// LogExpert won't show proper unicode support!
		const std::u8string path_string = a_path.u8string();

		if (path_string.c_str()) {
			logger::info("[Settings] Loading settings from: {}", std::string(path_string.begin(), path_string.end()));
		} else {
			logger::error("[Settings] Critical Error: Failed to convert path to wide string! Please report this immediately");
		}

		// Master
		GetIni(a_path, [](CSimpleIniA& a_ini) {
			Settings::GetSingleton()->LoadMasterIni(a_ini);
		});

		logger::info("[Settings] Using Theme specified in Modex.ini: {}", user.config.theme);

		// Language
		Translate::GetSingleton()->LoadLanguage(user.config.language);

		// Theme
		InstantiateDefaultTheme(def.style);
		Settings::GetSingleton()->SetThemeFromIni(user.config.theme);

		if (path_string.c_str()) {
			logger::info("[Settings] Loaded settings from: {}", std::string(path_string.begin(), path_string.end()));
		} else {
			logger::error("[Settings] Critical Error: Failed to convert path to wide string! Please report this immediately");
		}

		Menu::GetSingleton()->SyncUserStyleToImGui(user.style);

		logger::info("[Settings] Settings loaded successfully.");
	}

	// Load font separately to allow GraphicManager to read language config first.
	// Also need the settings to be loaded first to load proper fonts.
	void Settings::LoadUserFontSetting()
	{
		GetIni(L"Data/Interface/Modex/Modex.ini", [](CSimpleIniA& a_ini) {
			Settings::GetSingleton()->user.config.globalFont = GET_VALUE<std::string>(rSections[Main], "GlobalFont", Settings::GetSingleton()->def.config.globalFont, a_ini);
		});

		logger::info("[Settings] Loaded user font settings: {}", user.config.globalFont);
	}

	void Settings::SaveSettings()
	{
		GetIni(L"Data/Interface/Modex/Modex.ini", [](CSimpleIniA& a_ini) {
			FormatMasterIni(a_ini);

			// General
			a_ini.SetValue(rSections[Main], "Theme", Settings::GetSingleton()->user.config.theme.c_str());
			a_ini.SetValue(rSections[Main], "ShowMenuKey", std::to_string(Settings::GetSingleton()->user.config.showMenuKey).c_str());
			a_ini.SetValue(rSections[Main], "ShowMenuModifier", std::to_string(Settings::GetSingleton()->user.config.showMenuModifier).c_str());
			a_ini.SetValue(rSections[Main], "ModListSort", std::to_string(Settings::GetSingleton()->user.config.modListSort).c_str());
			a_ini.SetValue(rSections[Main], "UI Scale", std::to_string(Settings::GetSingleton()->user.config.uiScale).c_str());
			a_ini.SetValue(rSections[Main], "Fullscreen", ToString(Settings::GetSingleton()->user.config.fullscreen).c_str());
			a_ini.SetValue(rSections[Main], "PauseGame", ToString(Settings::GetSingleton()->user.config.pauseGame).c_str());
			a_ini.SetValue(rSections[Main], "DisableInMenu", ToString(Settings::GetSingleton()->user.config.disableInMenu).c_str());

			// Font & Localization
			a_ini.SetValue(rSections[Main], "Language", ToString(Settings::GetSingleton()->user.config.language).c_str());
			a_ini.SetValue(rSections[Main], "GlyphRange", Language::GetGlyphName(Settings::GetSingleton()->user.config.glyphRange).c_str());
			a_ini.SetValue(rSections[Main], "GlobalFont", Settings::GetSingleton()->user.config.globalFont.c_str());
			a_ini.SetValue(rSections[Main], "GlobalFontSize", std::to_string(Settings::GetSingleton()->user.config.globalFontSize).c_str());

			// Modules
			a_ini.SetValue(rSections[Modules], "DefaultShow", std::to_string(Settings::GetSingleton()->user.config.defaultShow).c_str());
			a_ini.SetValue(rSections[Modules], "ShowHomeMenu", ToString(Settings::GetSingleton()->user.config.showHomeMenu).c_str());
			a_ini.SetValue(rSections[Modules], "ShowAddItemMenu", ToString(Settings::GetSingleton()->user.config.showAddItemMenu).c_str());
			a_ini.SetValue(rSections[Modules], "ShowObjectMenu", ToString(Settings::GetSingleton()->user.config.showObjectMenu).c_str());
			a_ini.SetValue(rSections[Modules], "ShowNPCMenu", ToString(Settings::GetSingleton()->user.config.showNPCMenu).c_str());
			a_ini.SetValue(rSections[Modules], "ShowTeleportMenu", ToString(Settings::GetSingleton()->user.config.showTeleportMenu).c_str());
		});

		// This function body is called when a change is made, so send updates out:
		InputManager::GetSingleton()->UpdateSettings();
	}

	// This is executed within the scope of Modex.ini
	// Loads the preset specified in master ini. Then loads theme from there.
	// Executed within the scope of the Modex.ini file.
	void Settings::LoadMasterIni(CSimpleIniA& a_ini)
	{
		auto& _default = def.config;

		user.config.theme = GET_VALUE<std::string>(rSections[Main], "Theme", _default.theme, a_ini);
		user.config.showMenuKey = GET_VALUE<uint32_t>(rSections[Main], "ShowMenuKey", _default.showMenuKey, a_ini);
		user.config.showMenuModifier = GET_VALUE<uint32_t>(rSections[Main], "ShowMenuModifier", _default.showMenuModifier, a_ini);
		user.config.modListSort = GET_VALUE<int>(rSections[Main], "ModListSort", _default.modListSort, a_ini);
		user.config.uiScale = GET_VALUE<int>(rSections[Main], "UI Scale", _default.uiScale, a_ini);
		user.config.fullscreen = GET_VALUE<bool>(rSections[Main], "Fullscreen", _default.fullscreen, a_ini);
		user.config.pauseGame = GET_VALUE<bool>(rSections[Main], "PauseGame", _default.pauseGame, a_ini);
		user.config.disableInMenu = GET_VALUE<bool>(rSections[Main], "DisableInMenu", _default.disableInMenu, a_ini);

		user.config.language = GET_VALUE<std::string>(rSections[Main], "Language", _default.language, a_ini);
		user.config.glyphRange = GET_VALUE<Language::GlyphRanges>(rSections[Main], "GlyphRange", _default.glyphRange, a_ini);
		user.config.globalFont = GET_VALUE<std::string>(rSections[Main], "GlobalFont", _default.globalFont, a_ini);
		user.config.globalFontSize = GET_VALUE<float>(rSections[Main], "GlobalFontSize", _default.globalFontSize, a_ini);

		user.config.defaultShow = GET_VALUE<int>(rSections[Modules], "DefaultShow", _default.defaultShow, a_ini);
		user.config.showHomeMenu = GET_VALUE<bool>(rSections[Modules], "ShowHomeMenu", _default.showHomeMenu, a_ini);
		user.config.showAddItemMenu = GET_VALUE<bool>(rSections[Modules], "ShowAddItemMenu", _default.showAddItemMenu, a_ini);
		user.config.showObjectMenu = GET_VALUE<bool>(rSections[Modules], "ShowObjectMenu", _default.showObjectMenu, a_ini);
		user.config.showNPCMenu = GET_VALUE<bool>(rSections[Modules], "ShowNPCMenu", _default.showNPCMenu, a_ini);
		user.config.showTeleportMenu = GET_VALUE<bool>(rSections[Modules], "ShowTeleportMenu", _default.showTeleportMenu, a_ini);
	}

	void Settings::InstantiateDefaultTheme(Settings::Style& a_out)
	{
		ImGuiStyle style = ImGui::GetStyle();
		a_out.text = style.Colors[ImGuiCol_Text];
		a_out.textDisabled = style.Colors[ImGuiCol_TextDisabled];
		a_out.windowBg = style.Colors[ImGuiCol_WindowBg];
		a_out.border = style.Colors[ImGuiCol_Border];
		a_out.widgetBg = style.Colors[ImGuiCol_FrameBg];
		a_out.widgetHovered = style.Colors[ImGuiCol_FrameBgHovered];
		a_out.widgetActive = style.Colors[ImGuiCol_FrameBgActive];
		a_out.scrollbarBg = style.Colors[ImGuiCol_ScrollbarBg];
		a_out.scrollbarGrab = style.Colors[ImGuiCol_ScrollbarGrab];
		a_out.scrollbarGrabHovered = style.Colors[ImGuiCol_ScrollbarGrabHovered];
		a_out.scrollbarGrabActive = style.Colors[ImGuiCol_ScrollbarGrabActive];
		a_out.checkMark = style.Colors[ImGuiCol_CheckMark];
		a_out.sliderGrab = style.Colors[ImGuiCol_SliderGrab];
		a_out.sliderGrabActive = style.Colors[ImGuiCol_SliderGrabActive];
		a_out.button = style.Colors[ImGuiCol_Button];
		a_out.buttonHovered = style.Colors[ImGuiCol_ButtonHovered];
		a_out.buttonActive = style.Colors[ImGuiCol_ButtonActive];
		a_out.secondaryButton = def.style.secondaryButton;
		a_out.secondaryButtonActive = def.style.secondaryButtonActive;
		a_out.secondaryButtonHovered = def.style.secondaryButtonHovered;
		a_out.header = style.Colors[ImGuiCol_Header];
		a_out.headerHovered = style.Colors[ImGuiCol_HeaderHovered];
		a_out.headerActive = style.Colors[ImGuiCol_HeaderActive];
		a_out.separator = style.Colors[ImGuiCol_Separator];
		a_out.separatorHovered = style.Colors[ImGuiCol_SeparatorHovered];
		a_out.separatorActive = style.Colors[ImGuiCol_SeparatorActive];
		a_out.resizeGrip = style.Colors[ImGuiCol_ResizeGrip];
		a_out.resizeGripHovered = style.Colors[ImGuiCol_ResizeGripHovered];
		a_out.resizeGripActive = style.Colors[ImGuiCol_ResizeGripActive];
		a_out.tableHeaderBg = style.Colors[ImGuiCol_TableHeaderBg];
		a_out.tableBorderStrong = style.Colors[ImGuiCol_TableBorderStrong];
		a_out.tableBorderLight = style.Colors[ImGuiCol_TableBorderLight];
		a_out.textSelectedBg = style.Colors[ImGuiCol_TextSelectedBg];

		a_out.windowPadding = style.WindowPadding;
		a_out.widgetPadding = style.FramePadding;
		a_out.cellPadding = style.CellPadding;
		a_out.itemSpacing = style.ItemSpacing;
		a_out.itemInnerSpacing = style.ItemInnerSpacing;

		a_out.sidebarSpacing = def.style.sidebarSpacing;
		a_out.windowRounding = style.WindowRounding;
		a_out.windowBorderSize = style.WindowBorderSize;
		a_out.widgetBorderSize = style.FrameBorderSize;
		a_out.widgetRounding = style.FrameRounding;
		a_out.indentSpacing = style.IndentSpacing;
		a_out.columnsMinSpacing = style.ColumnsMinSpacing;
		a_out.scrollbarRounding = style.ScrollbarRounding;
		a_out.scrollbarSize = style.ScrollbarSize;
		a_out.grabMinSize = style.GrabMinSize;
		a_out.grabRounding = style.GrabRounding;

		a_out.noIconText = false;
		a_out.showTableRowBG = true;

		a_out.splashImage;

		// changes below
		a_out.tableRowBg = style.Colors[ImGuiCol_TableRowBg];
		a_out.tableAltRowBg = style.Colors[ImGuiCol_TableRowBgAlt];
	}

	// Call inside GetIni() to load the theme from the ini file into the user values.
	// Executed within the scope of the themes/ directory.
	void Settings::LoadThemeFromIni(CSimpleIniA& a_ini)
	{
		user.style.text = GET_VALUE<ImVec4>(rSections[Text], "TextColor", def.style.text, a_ini);
		user.style.textDisabled = GET_VALUE<ImVec4>(rSections[Text], "TextDisabledColor", def.style.textDisabled, a_ini);
		user.style.windowBg = GET_VALUE<ImVec4>(rSections[Window], "WindowBGColor", def.style.windowBg, a_ini);
		user.style.border = GET_VALUE<ImVec4>(rSections[Window], "BorderColor", def.style.border, a_ini);
		user.style.widgetBg = GET_VALUE<ImVec4>(rSections[Widgets], "WidgeteBGColor", def.style.widgetBg, a_ini);
		user.style.widgetHovered = GET_VALUE<ImVec4>(rSections[Widgets], "WidgetHoveredColor", def.style.widgetHovered, a_ini);
		user.style.widgetActive = GET_VALUE<ImVec4>(rSections[Widgets], "WidgetActiveColor", def.style.widgetActive, a_ini);
		user.style.scrollbarBg = GET_VALUE<ImVec4>(rSections[Widgets], "ScrollbarBGColor", def.style.scrollbarBg, a_ini);
		user.style.scrollbarGrab = GET_VALUE<ImVec4>(rSections[Widgets], "ScrollbarGrabColor", def.style.scrollbarGrab, a_ini);
		user.style.scrollbarGrabHovered = GET_VALUE<ImVec4>(rSections[Widgets], "ScrollbarGrabHoveredColor", def.style.scrollbarGrabHovered, a_ini);
		user.style.scrollbarGrabActive = GET_VALUE<ImVec4>(rSections[Widgets], "ScrollbarGrabActiveColor", def.style.scrollbarGrabActive, a_ini);
		user.style.checkMark = GET_VALUE<ImVec4>(rSections[Widgets], "CheckMarkColor", def.style.checkMark, a_ini);
		user.style.sliderGrab = GET_VALUE<ImVec4>(rSections[Widgets], "SliderGrabColor", def.style.sliderGrab, a_ini);
		user.style.sliderGrabActive = GET_VALUE<ImVec4>(rSections[Widgets], "SliderGrabActiveColor", def.style.sliderGrabActive, a_ini);
		user.style.button = GET_VALUE<ImVec4>(rSections[Widgets], "ButtonColor", def.style.button, a_ini);
		user.style.buttonHovered = GET_VALUE<ImVec4>(rSections[Widgets], "ButtonHoveredColor", def.style.buttonHovered, a_ini);
		user.style.buttonActive = GET_VALUE<ImVec4>(rSections[Widgets], "ButtonActiveColor", def.style.buttonActive, a_ini);
		user.style.secondaryButton = GET_VALUE<ImVec4>(rSections[Widgets], "SecondaryButtonColor", def.style.secondaryButton, a_ini);
		user.style.secondaryButtonHovered = GET_VALUE<ImVec4>(rSections[Widgets], "SecondaryButtonHoveredColor", def.style.secondaryButtonHovered, a_ini);
		user.style.secondaryButtonActive = GET_VALUE<ImVec4>(rSections[Widgets], "SecondaryButtonActiveColor", def.style.secondaryButtonActive, a_ini);
		user.style.header = GET_VALUE<ImVec4>(rSections[Table], "HeaderColor", def.style.header, a_ini);
		user.style.headerHovered = GET_VALUE<ImVec4>(rSections[Table], "HeaderHoveredColor", def.style.headerHovered, a_ini);
		user.style.headerActive = GET_VALUE<ImVec4>(rSections[Table], "HeaderActiveColor", def.style.headerActive, a_ini);
		user.style.separator = GET_VALUE<ImVec4>(rSections[Widgets], "SeparatorColor", def.style.separator, a_ini);
		user.style.separatorHovered = GET_VALUE<ImVec4>(rSections[Widgets], "SeparatorHoveredColor", def.style.separatorHovered, a_ini);
		user.style.separatorActive = GET_VALUE<ImVec4>(rSections[Widgets], "SeparatorActiveColor", def.style.separatorActive, a_ini);
		user.style.resizeGrip = GET_VALUE<ImVec4>(rSections[Widgets], "ResizeGripColor", def.style.resizeGrip, a_ini);
		user.style.resizeGripHovered = GET_VALUE<ImVec4>(rSections[Widgets], "ResizeGripHoveredColor", def.style.resizeGripHovered, a_ini);
		user.style.resizeGripActive = GET_VALUE<ImVec4>(rSections[Widgets], "ResizeGripActiveColor", def.style.resizeGripActive, a_ini);
		user.style.tableHeaderBg = GET_VALUE<ImVec4>(rSections[Table], "TableHeaderBGColor", def.style.tableHeaderBg, a_ini);
		user.style.tableBorderStrong = GET_VALUE<ImVec4>(rSections[Table], "TableBorderStrongColor", def.style.tableBorderStrong, a_ini);
		user.style.tableBorderLight = GET_VALUE<ImVec4>(rSections[Table], "TableBorderLightColor", def.style.tableBorderLight, a_ini);
		user.style.textSelectedBg = GET_VALUE<ImVec4>(rSections[Text], "TextSelectedBGColor", def.style.textSelectedBg, a_ini);

		user.style.sidebarSpacing = GET_VALUE<float>(rSections[Window], "SidebarSpacing", def.style.sidebarSpacing, a_ini);
		user.style.windowPadding = GET_VALUE<ImVec2>(rSections[Window], "WindowPadding", def.style.windowPadding, a_ini);
		user.style.widgetPadding = GET_VALUE<ImVec2>(rSections[Widgets], "WidgetPadding", def.style.widgetPadding, a_ini);
		user.style.cellPadding = GET_VALUE<ImVec2>(rSections[Table], "CellPadding", def.style.cellPadding, a_ini);
		user.style.itemSpacing = GET_VALUE<ImVec2>(rSections[Window], "ItemSpacing", def.style.itemSpacing, a_ini);
		user.style.itemInnerSpacing = GET_VALUE<ImVec2>(rSections[Window], "ItemInnerSpacing", def.style.itemInnerSpacing, a_ini);

		user.style.windowRounding = GET_VALUE<float>(rSections[Window], "WindowRounding", def.style.windowRounding, a_ini);
		user.style.windowBorderSize = GET_VALUE<float>(rSections[Window], "WindowBorderSize", def.style.windowBorderSize, a_ini);
		user.style.widgetBorderSize = GET_VALUE<float>(rSections[Widgets], "WidgetBorderSize", def.style.widgetBorderSize, a_ini);
		user.style.widgetRounding = GET_VALUE<float>(rSections[Widgets], "WidgetRounding", def.style.widgetRounding, a_ini);
		user.style.indentSpacing = GET_VALUE<float>(rSections[Window], "IndentSpacing", def.style.indentSpacing, a_ini);
		user.style.columnsMinSpacing = GET_VALUE<float>(rSections[Table], "ColumnMinSpacing", def.style.columnsMinSpacing, a_ini);
		user.style.scrollbarRounding = GET_VALUE<float>(rSections[Widgets], "ScrollbarRounding", def.style.scrollbarRounding, a_ini);
		user.style.scrollbarSize = GET_VALUE<float>(rSections[Widgets], "ScrollbarSize", def.style.scrollbarSize, a_ini);
		user.style.grabMinSize = GET_VALUE<float>(rSections[Widgets], "GrabMinSize", def.style.grabMinSize, a_ini);
		user.style.grabRounding = GET_VALUE<float>(rSections[Widgets], "GrabRounding", def.style.grabRounding, a_ini);

		user.style.noIconText = GET_VALUE<bool>(rSections[Text], "NoIconText", def.style.noIconText, a_ini);
		user.style.showTableRowBG = GET_VALUE<bool>(rSections[Table], "ShowTableRowBG", def.style.showTableRowBG, a_ini);

		user.style.splashImage = GET_VALUE<GraphicManager::Image>(rSections[Images], "SplashImage", def.style.splashImage, a_ini);

		// changes below
		user.style.tableRowBg = GET_VALUE<ImVec4>(rSections[Table], "TableRowBGColor", def.style.tableRowBg, a_ini);
		user.style.tableAltRowBg = GET_VALUE<ImVec4>(rSections[Table], "TableAltRowBGColor", def.style.tableAltRowBg, a_ini);
	}

	// Export theme and style values to a standalone ini file.
	// Decentralized from the main ini configuration to allow for easy sharing of themes.
	void Settings::ExportThemeToIni(const std::filesystem::path& a_path, Style a_user)
	{
		GetIni(a_path, [a_user](CSimpleIniA& a_ini) {
			FormatThemeIni(a_ini);

			a_ini.SetValue(rSections[Text], "TextColor", Settings::ToString(a_user.text, false).c_str());
			a_ini.SetValue(rSections[Text], "TextDisabledColor", Settings::ToString(a_user.textDisabled, false).c_str());
			a_ini.SetValue(rSections[Window], "WindowBGColor", Settings::ToString(a_user.windowBg, false).c_str());
			a_ini.SetValue(rSections[Window], "BorderColor", Settings::ToString(a_user.border, false).c_str());
			a_ini.SetValue(rSections[Widgets], "WidgetColor", Settings::ToString(a_user.widgetBg, false).c_str());
			a_ini.SetValue(rSections[Widgets], "WidgetHoveredColor", Settings::ToString(a_user.widgetHovered, false).c_str());
			a_ini.SetValue(rSections[Widgets], "WidgetActiveColor", Settings::ToString(a_user.widgetActive, false).c_str());
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
			a_ini.SetValue(rSections[Widgets], "SecondaryButtonColor", Settings::ToString(a_user.secondaryButton, false).c_str());
			a_ini.SetValue(rSections[Widgets], "SecondaryButtonHoveredColor", Settings::ToString(a_user.secondaryButtonHovered, false).c_str());
			a_ini.SetValue(rSections[Widgets], "SecondaryButtonActiveColor", Settings::ToString(a_user.secondaryButtonActive, false).c_str());
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
			a_ini.SetValue(rSections[Text], "TextSelectedBGColor", Settings::ToString(a_user.textSelectedBg, false).c_str());

			a_ini.SetValue(rSections[Window], "SidebarSpacing", Settings::ToString(a_user.sidebarSpacing, false).c_str());
			a_ini.SetValue(rSections[Window], "WindowPadding", Settings::ToString(a_user.windowPadding, false).c_str());
			a_ini.SetValue(rSections[Widgets], "WidgetPadding", Settings::ToString(a_user.widgetPadding, false).c_str());
			a_ini.SetValue(rSections[Table], "CellPadding", Settings::ToString(a_user.cellPadding, false).c_str());
			a_ini.SetValue(rSections[Window], "ItemSpacing", Settings::ToString(a_user.itemSpacing, false).c_str());
			a_ini.SetValue(rSections[Window], "ItemInnerSpacing", Settings::ToString(a_user.itemInnerSpacing, false).c_str());

			a_ini.SetValue(rSections[Window], "WindowRounding", Settings::ToString(a_user.windowRounding, false).c_str());
			a_ini.SetValue(rSections[Window], "WindowBorderSize", Settings::ToString(a_user.windowBorderSize, false).c_str());
			a_ini.SetValue(rSections[Widgets], "WidgetBorderSize", Settings::ToString(a_user.widgetBorderSize, false).c_str());
			a_ini.SetValue(rSections[Widgets], "WidgetRounding", Settings::ToString(a_user.widgetRounding, false).c_str());
			a_ini.SetValue(rSections[Window], "IndentSpacing", Settings::ToString(a_user.indentSpacing, false).c_str());
			a_ini.SetValue(rSections[Table], "ColumnMinSpacing", Settings::ToString(a_user.columnsMinSpacing, false).c_str());
			a_ini.SetValue(rSections[Widgets], "ScrollbarRounding", Settings::ToString(a_user.scrollbarRounding, false).c_str());
			a_ini.SetValue(rSections[Widgets], "ScrollbarSize", Settings::ToString(a_user.scrollbarSize, false).c_str());
			a_ini.SetValue(rSections[Widgets], "GrabMinSize", Settings::ToString(a_user.grabMinSize, false).c_str());
			a_ini.SetValue(rSections[Widgets], "GrabRounding", Settings::ToString(a_user.grabRounding, false).c_str());
			a_ini.SetValue(rSections[Table], "ShowTableRowBG", Settings::ToString(a_user.showTableRowBG, false).c_str());

			a_ini.SetValue(rSections[Text], "NoIconText", Settings::ToString(a_user.noIconText, false).c_str());

			a_ini.SetValue(rSections[Images], "SplashImage", Settings::ToString(a_user.splashImage, false).c_str());

			// changes below
			a_ini.SetValue(rSections[Table], "TableRowBGColor", Settings::ToString(a_user.tableRowBg, false).c_str());
			a_ini.SetValue(rSections[Table], "TableAltRowBGColor", Settings::ToString(a_user.tableAltRowBg, false).c_str());
		});

		logger::info("[Settings] Exported theme to: {}", Utils::utf8_encode(a_path.wstring()));
	};
}