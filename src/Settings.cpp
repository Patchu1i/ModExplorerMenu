#include "Settings.h"
#include "Menu.h"
#include "SimpleIni.h"
#include "Utils/INI.h"
#include <codecvt>
#include <format>

using namespace IniHelper;

namespace ModExplorerMenu
{

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
			stl::report_and_fail("Failed to parse value from .ini file! Ensure you're using the correct format!");
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
		} else if constexpr (std::is_same_v<T, float>) {
			return Settings::GetFloat(value);
		} else if constexpr (std::is_same_v<T, GraphicManager::Font>) {
			return GraphicManager::GetFont(value);
		} else if constexpr (std::is_same_v<T, GraphicManager::Image>) {
			return GraphicManager::GetImage(value);
		} else if constexpr (std::is_same_v<T, Language::Locale>) {
			return Language::GetLanguage(value);
		} else {
			stl::report_and_fail("Unhandled type passed to GET_VALUE in Menu.cpp!");
			return a_default;
		}
	}

	void Settings::FormatMasterIni(CSimpleIniA& a_ini)
	{
		a_ini.SetValue(rSections[Main], NULL, NULL, GetComment(iComment::PresetMainHeader));
		a_ini.SetValue(rSections[AddItem], NULL, NULL, GetComment(iComment::PresetAddItemHeader));
		a_ini.SetValue(rSections[Object], NULL, NULL, GetComment(iComment::PresetObjectHeader));
		a_ini.SetValue(rSections[FormLookup], NULL, NULL, GetComment(iComment::PresetFormLookupHeader));
		a_ini.SetValue(rSections[NPC], NULL, NULL, GetComment(iComment::PresetNPCHeader));
		a_ini.SetValue(rSections[Teleport], NULL, NULL, GetComment(iComment::PresetTeleportHeader));
	}

	// Call on new ini file to generate comments and sections before inserting.
	void Settings::FormatThemeIni(CSimpleIniA& a_ini)
	{
		a_ini.SetValue(rSections[Window], NULL, NULL, GetComment(iComment::ThemeConfigHeader));
		a_ini.SetValue(rSections[Frame], NULL, NULL, GetComment(iComment::ThemeFrameHeader));
		a_ini.SetValue(rSections[Text], NULL, NULL, GetComment(iComment::ThemeTextHeader));
		a_ini.SetValue(rSections[Table], NULL, NULL, GetComment(iComment::ThemeTableHeader));
		a_ini.SetValue(rSections[Widgets], NULL, NULL, GetComment(iComment::ThemeWidgetsHeader));
	}

	// Master ini has no defaults, they're established here. Theme defaults are derived from def class.
	void Settings::CreateDefaultMaster()
	{
		GetIni(L"Data/Interface/ModExplorerMenu/ModExplorerMenu.ini", [](CSimpleIniA& a_ini) {
			FormatMasterIni(a_ini);
			a_ini.SetValue(rSections[Main], "Theme", "Default");
			a_ini.SetValue(rSections[Main], "ShowMenuKey", "211");
			a_ini.SetValue(rSections[Main], "ShowMenuModifier", "0");
			a_ini.SetValue(rSections[Main], "Language", "English");
			a_ini.SetValue(rSections[Main], "ModListSort", "0");
			a_ini.SetValue(rSections[Main], "UI Scale", "100");

			a_ini.SetValue(rSections[Main], "DefaultShow", "0");
			a_ini.SetValue(rSections[Main], "ShowHomeMenu", "false");
			a_ini.SetValue(rSections[Main], "ShowAddItemMenu", "true");
			a_ini.SetValue(rSections[Main], "ShowObjectMenu", "true");
			a_ini.SetValue(rSections[Main], "ShowNPCMenu", "true");
			a_ini.SetValue(rSections[Main], "ShowTeleportMenu", "true");

			a_ini.SetValue(rSections[Main], "DataPath", "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Skyrim Special Edition\\Data");
		});
	}

	// Execute ini value assignment where necessary.
	void Settings::LoadSettings(const wchar_t* a_path)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

		// If master ini doesn't exist this will default it to "Default"
		if (!std::filesystem::exists(a_path)) {
			CreateDefaultMaster();
		}

		// Master
		GetIni(a_path, [](CSimpleIniA& a_ini) {
			Settings::GetSingleton()->LoadMasterIni(a_ini);
		});

		// Language
		auto lang = Language::GetLanguageName(user.config.language);
		Translate::GetSingleton()->LoadLanguage(lang);

		// Theme
		InstantiateDefaultTheme(def.style);
		Settings::GetSingleton()->SetThemeFromIni(user.config.theme);

		Menu::GetSingleton()->SyncUserStyleToImGui(user.style);
	}

	// Load font separately to allow GraphicManager to read language config first.
	// Also need the settings to be loaded first to load proper fonts.
	void Settings::LoadFont()
	{
		std::wstring fullPath = GetThemePath(user.config.theme);
		GetIni(fullPath.c_str(), [](CSimpleIniA& a_ini) {
			Settings::GetSingleton()->user.style.font = GET_VALUE<GraphicManager::Font>(rSections[Fonts], "Font", Settings::GetSingleton()->def.style.font, a_ini);
			Settings::GetSingleton()->user.style.buttonFont = GET_VALUE<GraphicManager::Font>(rSections[Fonts], "ButtonFont", Settings::GetSingleton()->def.style.buttonFont, a_ini);
		});
	}

	void Settings::SaveSettings()
	{
		GetIni(L"Data/Interface/ModExplorerMenu/ModExplorerMenu.ini", [](CSimpleIniA& a_ini) {
			FormatMasterIni(a_ini);

			a_ini.SetValue(rSections[Main], "Theme", Settings::GetSingleton()->user.config.theme.c_str());
			a_ini.SetValue(rSections[Main], "ShowMenuKey", std::to_string(Settings::GetSingleton()->user.config.showMenuKey).c_str());
			a_ini.SetValue(rSections[Main], "ShowMenuModifier", std::to_string(Settings::GetSingleton()->user.config.showMenuModifier).c_str());
			a_ini.SetValue(rSections[Main], "Language", ToString(Settings::GetSingleton()->user.config.language, false).c_str());
			a_ini.SetValue(rSections[Main], "ModListSort", std::to_string(Settings::GetSingleton()->user.config.modListSort).c_str());
			a_ini.SetValue(rSections[Main], "UI Scale", std::to_string(Settings::GetSingleton()->user.config.uiScale).c_str());

			a_ini.SetValue(rSections[Main], "DefaultShow", std::to_string(Settings::GetSingleton()->user.config.defaultShow).c_str());
			a_ini.SetValue(rSections[Main], "ShowHomeMenu", ToString(Settings::GetSingleton()->user.config.showHomeMenu, false).c_str());
			a_ini.SetValue(rSections[Main], "ShowAddItemMenu", ToString(Settings::GetSingleton()->user.config.showAddItemMenu, true).c_str());
			a_ini.SetValue(rSections[Main], "ShowObjectMenu", ToString(Settings::GetSingleton()->user.config.showObjectMenu, true).c_str());
			a_ini.SetValue(rSections[Main], "ShowNPCMenu", ToString(Settings::GetSingleton()->user.config.showNPCMenu, true).c_str());
			a_ini.SetValue(rSections[Main], "ShowTeleportMenu", ToString(Settings::GetSingleton()->user.config.showTeleportMenu, true).c_str());

			a_ini.SetValue(rSections[Main], "DataPath", Settings::GetSingleton()->user.config.dataPath.c_str());
		});
	}

	// This is executed within the scope of ModExplorerMenu.ini
	// Loads the preset specified in master ini. Then loads theme from there.
	// Executed within the scope of the ModExplorerMenu.ini file.
	void Settings::LoadMasterIni(CSimpleIniA& a_ini)
	{
		user.config.theme = GET_VALUE<std::string>(rSections[Main], "Theme", "Default", a_ini);
		user.config.showMenuKey = GET_VALUE<int>(rSections[Main], "ShowMenuKey", 211, a_ini);
		user.config.showMenuModifier = GET_VALUE<int>(rSections[Main], "ShowMenuModifier", 0, a_ini);
		user.config.language = GET_VALUE<Language::Locale>(rSections[Main], "Language", Language::Locale::English, a_ini);
		user.config.modListSort = GET_VALUE<int>(rSections[Main], "ModListSort", 0, a_ini);
		user.config.uiScale = GET_VALUE<int>(rSections[Main], "UI Scale", 100, a_ini);

		user.config.defaultShow = GET_VALUE<int>(rSections[Main], "DefaultShow", 0, a_ini);
		user.config.showHomeMenu = GET_VALUE<bool>(rSections[Main], "ShowHomeMenu", false, a_ini);
		user.config.showAddItemMenu = GET_VALUE<bool>(rSections[Main], "ShowAddItemMenu", true, a_ini);
		user.config.showObjectMenu = GET_VALUE<bool>(rSections[Main], "ShowObjectMenu", true, a_ini);
		user.config.showNPCMenu = GET_VALUE<bool>(rSections[Main], "ShowNPCMenu", true, a_ini);
		user.config.showTeleportMenu = GET_VALUE<bool>(rSections[Main], "ShowTeleportMenu", true, a_ini);

		user.config.dataPath = GET_VALUE<std::string>(rSections[Main], "DataPath", "", a_ini);
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
		a_out.tableRowBg = style.Colors[ImGuiCol_TableRowBg];
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

		a_out.font = GraphicManager::font_library["Default"];
		a_out.buttonFont = GraphicManager::font_library["Default"];
		a_out.globalFontSize;

		a_out.splashImage;
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
		user.style.tableRowBg = GET_VALUE<ImVec4>(rSections[Table], "TableRowBGColor", def.style.tableRowBg, a_ini);
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

		user.style.globalFontSize = GET_VALUE<float>(rSections[Fonts], "GlobalFontSize", def.style.globalFontSize, a_ini);

		user.style.splashImage = GET_VALUE<GraphicManager::Image>(rSections[Images], "SplashImage", def.style.splashImage, a_ini);
	}

	// Export theme and style values to a standalone ini file.
	// Decentralized from the main ini configuration to allow for easy sharing of themes.
	void Settings::ExportThemeToIni(const wchar_t* a_path, Style a_user)
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
			a_ini.SetValue(rSections[Table], "TableRowBGColor", Settings::ToString(a_user.tableRowBg, false).c_str());
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

			a_ini.SetValue(rSections[Text], "NoIconText", Settings::ToString(a_user.noIconText, false).c_str());
			a_ini.SetValue(rSections[Table], "ShowTableRowBG", Settings::ToString(a_user.showTableRowBG, false).c_str());

			a_ini.SetValue(rSections[Fonts], "Font", Settings::ToString(a_user.font, false).c_str());
			a_ini.SetValue(rSections[Fonts], "ButtonFont", Settings::ToString(a_user.buttonFont, false).c_str());
			a_ini.SetValue(rSections[Fonts], "GlobalFontSize", Settings::ToString(a_user.globalFontSize, false).c_str());

			a_ini.SetValue(rSections[Images], "SplashImage", Settings::ToString(a_user.splashImage, false).c_str());
		});
	};
}