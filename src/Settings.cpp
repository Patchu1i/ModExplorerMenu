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
		if (a_ini.GetValue(section, key) == nullptr) {
			a_ini.SetValue(section, key, Settings::ToString(a_default, false).c_str());
		}

		std::string value = a_ini.GetValue(section, key);

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
		} else {
			stl::report_and_fail("Unhandled type passed to GET_VALUE in Menu.cpp!");
			return a_default;
		}
	}

	void Settings::FormatMasterIni(CSimpleIniA& a_ini)
	{
		a_ini.SetValue(rSections[Main], NULL, NULL, GetComment(iComment::PresetMainHeader));
		a_ini.SetValue(rSections[AddItem], NULL, NULL, GetComment(iComment::PresetAddItemHeader));
		a_ini.SetValue(rSections[FormLookup], NULL, NULL, GetComment(iComment::PresetFormLookupHeader));
		a_ini.SetValue(rSections[NPC], NULL, NULL, GetComment(iComment::PresetNPCHeader));
		a_ini.SetValue(rSections[Teleport], NULL, NULL, GetComment(iComment::PresetTeleportHeader));
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

	void Settings::CreateDefaultMaster()
	{
		GetIni(L"Data/Interface/ModExplorerMenu/ModExplorerMenu.ini", [](CSimpleIniA& a_ini) {
			FormatMasterIni(a_ini);
			a_ini.SetValue(rSections[Main], "Theme", "Default");
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

		// Theme
		InstantiateDefaultTheme(def.style);
		Settings::GetSingleton()->SetThemeFromIni(user.config.theme);

		Menu::GetSingleton()->SyncUserStyleToImGui(user.style);
	}

	void Settings::SaveSettings()
	{
		GetIni(L"Data/Interface/ModExplorerMenu/ModExplorerMenu.ini", [](CSimpleIniA& a_ini) {
			FormatMasterIni(a_ini);

			a_ini.SetValue(rSections[Main], "Theme", Settings::GetSingleton()->user.config.theme.c_str());
			a_ini.SetValue(rSections[Main], "ShowMenu", std::to_string(Settings::GetSingleton()->user.config.showMenu).c_str());

			a_ini.SetValue(rSections[AddItem], "ShowFavoriteColumn", std::to_string(Settings::GetSingleton()->user.config.aimShowFavoriteColumn).c_str());
			a_ini.SetValue(rSections[AddItem], "ShowTypeColumn", std::to_string(Settings::GetSingleton()->user.config.aimShowTypeColumn).c_str());
			a_ini.SetValue(rSections[AddItem], "ShowFormIDColumn", std::to_string(Settings::GetSingleton()->user.config.aimShowFormIDColumn).c_str());
			a_ini.SetValue(rSections[AddItem], "ShowNameColumn", std::to_string(Settings::GetSingleton()->user.config.aimShowNameColumn).c_str());
			a_ini.SetValue(rSections[AddItem], "ShowEditorIDColumn", std::to_string(Settings::GetSingleton()->user.config.aimShowEditorIDColumn).c_str());
			a_ini.SetValue(rSections[AddItem], "ShowGoldValueColumn", std::to_string(Settings::GetSingleton()->user.config.aimShowGoldValueColumn).c_str());
			a_ini.SetValue(rSections[AddItem], "ShowBaseDamageColumn", std::to_string(Settings::GetSingleton()->user.config.aimShowBaseDamageColumn).c_str());
			a_ini.SetValue(rSections[AddItem], "ShowArmorRatingColumn", std::to_string(Settings::GetSingleton()->user.config.aimShowArmorRatingColumn).c_str());
			a_ini.SetValue(rSections[AddItem], "ShowSpeedColumn", std::to_string(Settings::GetSingleton()->user.config.aimShowSpeedColumn).c_str());
			a_ini.SetValue(rSections[AddItem], "ShowCritDamageColumn", std::to_string(Settings::GetSingleton()->user.config.aimShowCritDamageColumn).c_str());
			a_ini.SetValue(rSections[AddItem], "ShowSkillColumn", std::to_string(Settings::GetSingleton()->user.config.aimShowSkillColumn).c_str());
			a_ini.SetValue(rSections[AddItem], "ShowWeightColumn", std::to_string(Settings::GetSingleton()->user.config.aimShowWeightColumn).c_str());
			a_ini.SetValue(rSections[AddItem], "ShowDPSColumn", std::to_string(Settings::GetSingleton()->user.config.aimShowDPSColumn).c_str());

			a_ini.SetValue(rSections[Teleport], "ShowFavoriteColumn", std::to_string(Settings::GetSingleton()->user.config.teleShowFavoriteColumn).c_str());
			a_ini.SetValue(rSections[Teleport], "ShowPluginColumn", std::to_string(Settings::GetSingleton()->user.config.teleShowPluginColumn).c_str());
			a_ini.SetValue(rSections[Teleport], "ShowSpaceColumn", std::to_string(Settings::GetSingleton()->user.config.teleShowSpaceColumn).c_str());
			a_ini.SetValue(rSections[Teleport], "ShowZoneColumn", std::to_string(Settings::GetSingleton()->user.config.teleShowZoneColumn).c_str());
			a_ini.SetValue(rSections[Teleport], "ShowFullNameColumn", std::to_string(Settings::GetSingleton()->user.config.teleShowFullNameColumn).c_str());
			a_ini.SetValue(rSections[Teleport], "ShowEditorIDColumn", std::to_string(Settings::GetSingleton()->user.config.teleShowEditorIDColumn).c_str());

			a_ini.SetValue(rSections[NPC], "ShowFavoriteColumn", std::to_string(Settings::GetSingleton()->user.config.npcShowFavoriteColumn).c_str());
			a_ini.SetValue(rSections[NPC], "ShowPluginColumn", std::to_string(Settings::GetSingleton()->user.config.npcShowPluginColumn).c_str());
			a_ini.SetValue(rSections[NPC], "ShowNameColumn", std::to_string(Settings::GetSingleton()->user.config.npcShowNameColumn).c_str());
			a_ini.SetValue(rSections[NPC], "ShowEditorIDColumn", std::to_string(Settings::GetSingleton()->user.config.npcShowEditorIDColumn).c_str());
			a_ini.SetValue(rSections[NPC], "ShowHealthColumn", std::to_string(Settings::GetSingleton()->user.config.npcShowHealthColumn).c_str());
			a_ini.SetValue(rSections[NPC], "ShowHealthRegenColumn", std::to_string(Settings::GetSingleton()->user.config.npcShowHealthRegenColumn).c_str());
			a_ini.SetValue(rSections[NPC], "ShowMagickaColumn", std::to_string(Settings::GetSingleton()->user.config.npcShowMagickaColumn).c_str());
			a_ini.SetValue(rSections[NPC], "ShowMagickaRegenColumn", std::to_string(Settings::GetSingleton()->user.config.npcShowMagickaRegenColumn).c_str());
			a_ini.SetValue(rSections[NPC], "ShowStaminaColumn", std::to_string(Settings::GetSingleton()->user.config.npcShowStaminaColumn).c_str());
			a_ini.SetValue(rSections[NPC], "ShowStaminaRegenColumn", std::to_string(Settings::GetSingleton()->user.config.npcShowStaminaRegenColumn).c_str());
			a_ini.SetValue(rSections[NPC], "ShowCarryWeightColumn", std::to_string(Settings::GetSingleton()->user.config.npcShowCarryWeightColumn).c_str());
			a_ini.SetValue(rSections[NPC], "ShowMassColumn", std::to_string(Settings::GetSingleton()->user.config.npcShowMassColumn).c_str());
			a_ini.SetValue(rSections[NPC], "ShowDamageResistColumn", std::to_string(Settings::GetSingleton()->user.config.npcShowDamageResistColumn).c_str());
			a_ini.SetValue(rSections[NPC], "ShowAttackDamageColumn", std::to_string(Settings::GetSingleton()->user.config.npcShowAttackDamageColumn).c_str());
			a_ini.SetValue(rSections[NPC], "ShowUnarmedDamageColumn", std::to_string(Settings::GetSingleton()->user.config.npcShowUnarmedDamageColumn).c_str());
		});
	}

	// This is executed within the scope of ModExplorerMenu.ini
	// Loads the preset specified in master ini. Then loads theme from there.
	// Executed within the scope of the ModExplorerMenu.ini file.
	void Settings::LoadMasterIni(CSimpleIniA& a_ini)
	{
		user.config.theme = GET_VALUE<std::string>(rSections[Main], "Theme", "Default", a_ini);
		user.config.showMenu = GET_VALUE<int>(rSections[Main], "ShowMenu", 0, a_ini);

		user.config.aimShowFavoriteColumn = GET_VALUE<bool>(rSections[AddItem], "ShowFavoriteColumn", true, a_ini);
		user.config.aimShowPluginColumn = GET_VALUE<bool>(rSections[AddItem], "ShowPluginColumn", true, a_ini);
		user.config.aimShowTypeColumn = GET_VALUE<bool>(rSections[AddItem], "ShowTypeColumn", true, a_ini);
		user.config.aimShowFormIDColumn = GET_VALUE<bool>(rSections[AddItem], "ShowFormIDColumn", false, a_ini);
		user.config.aimShowNameColumn = GET_VALUE<bool>(rSections[AddItem], "ShowNameColumn", true, a_ini);
		user.config.aimShowEditorIDColumn = GET_VALUE<bool>(rSections[AddItem], "ShowEditorIDColumn", true, a_ini);
		user.config.aimShowGoldValueColumn = GET_VALUE<bool>(rSections[AddItem], "ShowGoldValueColumn", false, a_ini);
		user.config.aimShowBaseDamageColumn = GET_VALUE<bool>(rSections[AddItem], "ShowBaseDamageColumn", false, a_ini);
		user.config.aimShowArmorRatingColumn = GET_VALUE<bool>(rSections[AddItem], "ShowArmorRatingColumn", false, a_ini);
		user.config.aimShowSpeedColumn = GET_VALUE<bool>(rSections[AddItem], "ShowSpeedColumn", false, a_ini);
		user.config.aimShowCritDamageColumn = GET_VALUE<bool>(rSections[AddItem], "ShowCritDamageColumn", false, a_ini);
		user.config.aimShowSkillColumn = GET_VALUE<bool>(rSections[AddItem], "ShowSkillColumn", false, a_ini);
		user.config.aimShowWeightColumn = GET_VALUE<bool>(rSections[AddItem], "ShowWeightColumn", false, a_ini);
		user.config.aimShowDPSColumn = GET_VALUE<bool>(rSections[AddItem], "ShowDPSColumn", false, a_ini);

		user.config.soShowFavoriteColumn = GET_VALUE<bool>(rSections[Object], "ShowFavoriteColumn", true, a_ini);
		user.config.soShowPluginColumn = GET_VALUE<bool>(rSections[Object], "ShowPluginColumn", true, a_ini);
		user.config.soShowTypeColumn = GET_VALUE<bool>(rSections[Object], "ShowTypeColumn", true, a_ini);
		user.config.soShowFormIDColumn = GET_VALUE<bool>(rSections[Object], "ShowFormIDColumn", false, a_ini);
		user.config.soShowEditorIDColumn = GET_VALUE<bool>(rSections[Object], "ShowEditorIDColumn", true, a_ini);

		user.config.teleShowFavoriteColumn = GET_VALUE<bool>(rSections[Teleport], "ShowFavoriteColumn", true, a_ini);
		user.config.teleShowPluginColumn = GET_VALUE<bool>(rSections[Teleport], "ShowPluginColumn", true, a_ini);
		user.config.teleShowSpaceColumn = GET_VALUE<bool>(rSections[Teleport], "ShowSpaceColumn", true, a_ini);
		user.config.teleShowZoneColumn = GET_VALUE<bool>(rSections[Teleport], "ShowZoneColumn", true, a_ini);
		user.config.teleShowFullNameColumn = GET_VALUE<bool>(rSections[Teleport], "ShowFullNameColumn", true, a_ini);
		user.config.teleShowEditorIDColumn = GET_VALUE<bool>(rSections[Teleport], "ShowEditorIDColumn", true, a_ini);

		user.config.npcShowFavoriteColumn = GET_VALUE<bool>(rSections[NPC], "ShowFavoriteColumn", true, a_ini);
		user.config.npcShowPluginColumn = GET_VALUE<bool>(rSections[NPC], "ShowPluginColumn", false, a_ini);
		user.config.npcShowNameColumn = GET_VALUE<bool>(rSections[NPC], "ShowNameColumn", true, a_ini);
		user.config.npcShowFormIDColumn = GET_VALUE<bool>(rSections[NPC], "ShowFormIDColumn", true, a_ini);
		user.config.npcShowEditorIDColumn = GET_VALUE<bool>(rSections[NPC], "ShowEditorIDColumn", true, a_ini);
		user.config.npcShowHealthColumn = GET_VALUE<bool>(rSections[NPC], "ShowHealthColumn", false, a_ini);
		user.config.npcShowHealthRegenColumn = GET_VALUE<bool>(rSections[NPC], "ShowHealthRegenColumn", false, a_ini);
		user.config.npcShowMagickaColumn = GET_VALUE<bool>(rSections[NPC], "ShowMagickaColumn", false, a_ini);
		user.config.npcShowMagickaRegenColumn = GET_VALUE<bool>(rSections[NPC], "ShowMagickaRegenColumn", false, a_ini);
		user.config.npcShowStaminaColumn = GET_VALUE<bool>(rSections[NPC], "ShowStaminaColumn", false, a_ini);
		user.config.npcShowStaminaRegenColumn = GET_VALUE<bool>(rSections[NPC], "ShowStaminaRegenColumn", false, a_ini);
		user.config.npcShowCarryWeightColumn = GET_VALUE<bool>(rSections[NPC], "ShowCarryWeightColumn", false, a_ini);
		user.config.npcShowMassColumn = GET_VALUE<bool>(rSections[NPC], "ShowMassColumn", false, a_ini);
		user.config.npcShowDamageResistColumn = GET_VALUE<bool>(rSections[NPC], "ShowDamageResistColumn", false, a_ini);
		user.config.npcShowAttackDamageColumn = GET_VALUE<bool>(rSections[NPC], "ShowAttackDamageColumn", false, a_ini);
		user.config.npcShowUnarmedDamageColumn = GET_VALUE<bool>(rSections[NPC], "ShowUnarmedDamageColumn", false, a_ini);
	}

	void Settings::InstantiateDefaultTheme(Settings::Style& a_out)
	{
		ImGuiStyle style = ImGui::GetStyle();
		a_out.text = style.Colors[ImGuiCol_Text];
		a_out.textDisabled = style.Colors[ImGuiCol_TextDisabled];
		a_out.windowBg = style.Colors[ImGuiCol_WindowBg];
		a_out.childBg = style.Colors[ImGuiCol_ChildBg];
		a_out.popupBg = style.Colors[ImGuiCol_PopupBg];
		a_out.border = style.Colors[ImGuiCol_Border];
		a_out.borderShadow = style.Colors[ImGuiCol_BorderShadow];
		a_out.frameBg = style.Colors[ImGuiCol_FrameBg];
		a_out.frameBgHovered = style.Colors[ImGuiCol_FrameBgHovered];
		a_out.frameBgActive = style.Colors[ImGuiCol_FrameBgActive];
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
		a_out.framePadding = style.FramePadding;
		a_out.cellPadding = style.CellPadding;
		a_out.itemSpacing = style.ItemSpacing;
		a_out.itemInnerSpacing = style.ItemInnerSpacing;

		a_out.alpha = style.Alpha;
		a_out.windowRounding = style.WindowRounding;
		a_out.windowBorderSize = style.WindowBorderSize;
		a_out.childBorderSize = style.ChildBorderSize;
		a_out.childRounding = style.ChildRounding;
		a_out.frameBorderSize = style.FrameBorderSize;
		a_out.frameRounding = style.FrameRounding;
		a_out.indentSpacing = style.IndentSpacing;
		a_out.columnsMinSpacing = style.ColumnsMinSpacing;
		a_out.scrollbarRounding = style.ScrollbarRounding;
		a_out.scrollbarSize = style.ScrollbarSize;
		a_out.grabMinSize = style.GrabMinSize;
		a_out.grabRounding = style.GrabRounding;
		a_out.popupBorderSize = style.PopupBorderSize;
		a_out.popupRounding = style.PopupRounding;

		a_out.showTableRowBG = true;

		a_out.font;
		a_out.globalFontSize;

		a_out.splashImage;
		a_out.favoriteIconEnabled;
		a_out.favoriteIconDisabled;

		// TODO: Font stuff
		// TODO: Image stuff
	}

	// Call inside GetIni() to load the theme from the ini file into the user values.
	// Executed within the scope of the themes/ directory.
	void Settings::LoadThemeFromIni(CSimpleIniA& a_ini)
	{
		user.style.text = GET_VALUE<ImVec4>(rSections[Text], "TextColor", def.style.text, a_ini);
		user.style.textDisabled = GET_VALUE<ImVec4>(rSections[Text], "TextDisabledColor", def.style.textDisabled, a_ini);
		user.style.windowBg = GET_VALUE<ImVec4>(rSections[Window], "WindowBGColor", def.style.windowBg, a_ini);
		user.style.childBg = GET_VALUE<ImVec4>(rSections[Child], "ChildBGColor", def.style.childBg, a_ini);
		user.style.popupBg = GET_VALUE<ImVec4>(rSections[Child], "PopupBGColor", def.style.popupBg, a_ini);
		user.style.border = GET_VALUE<ImVec4>(rSections[Window], "BorderColor", def.style.border, a_ini);
		user.style.borderShadow = GET_VALUE<ImVec4>(rSections[Window], "BorderShadowColor", def.style.borderShadow, a_ini);
		user.style.frameBg = GET_VALUE<ImVec4>(rSections[Frame], "FrameBGColor", def.style.frameBg, a_ini);
		user.style.frameBgHovered = GET_VALUE<ImVec4>(rSections[Frame], "FrameBGHoveredColor", def.style.frameBgHovered, a_ini);
		user.style.frameBgActive = GET_VALUE<ImVec4>(rSections[Frame], "FrameBGActiveColor", def.style.frameBgActive, a_ini);
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

		user.style.windowPadding = GET_VALUE<ImVec2>(rSections[Window], "WindowPadding", def.style.windowPadding, a_ini);
		user.style.framePadding = GET_VALUE<ImVec2>(rSections[Frame], "FramePadding", def.style.framePadding, a_ini);
		user.style.cellPadding = GET_VALUE<ImVec2>(rSections[Table], "CellPadding", def.style.cellPadding, a_ini);
		user.style.itemSpacing = GET_VALUE<ImVec2>(rSections[Window], "ItemSpacing", def.style.itemSpacing, a_ini);
		user.style.itemInnerSpacing = GET_VALUE<ImVec2>(rSections[Window], "ItemInnerSpacing", def.style.itemInnerSpacing, a_ini);

		user.style.alpha = GET_VALUE<float>(rSections[Window], "Alpha", def.style.alpha, a_ini);
		user.style.windowRounding = GET_VALUE<float>(rSections[Window], "WindowRounding", def.style.windowRounding, a_ini);
		user.style.windowBorderSize = GET_VALUE<float>(rSections[Window], "WindowBorderSize", def.style.windowBorderSize, a_ini);
		user.style.childBorderSize = GET_VALUE<float>(rSections[Child], "ChildBorderSize", def.style.childBorderSize, a_ini);
		user.style.childRounding = GET_VALUE<float>(rSections[Child], "ChildRounding", def.style.childRounding, a_ini);
		user.style.frameBorderSize = GET_VALUE<float>(rSections[Frame], "FrameBorderSize", def.style.frameBorderSize, a_ini);
		user.style.frameRounding = GET_VALUE<float>(rSections[Frame], "FrameRounding", def.style.frameRounding, a_ini);
		user.style.indentSpacing = GET_VALUE<float>(rSections[Window], "IndentSpacing", def.style.indentSpacing, a_ini);
		user.style.columnsMinSpacing = GET_VALUE<float>(rSections[Table], "ColumnMinSpacing", def.style.columnsMinSpacing, a_ini);
		user.style.scrollbarRounding = GET_VALUE<float>(rSections[Widgets], "ScrollbarRounding", def.style.scrollbarRounding, a_ini);
		user.style.scrollbarSize = GET_VALUE<float>(rSections[Widgets], "ScrollbarSize", def.style.scrollbarSize, a_ini);
		user.style.grabMinSize = GET_VALUE<float>(rSections[Widgets], "GrabMinSize", def.style.grabMinSize, a_ini);
		user.style.grabRounding = GET_VALUE<float>(rSections[Widgets], "GrabRounding", def.style.grabRounding, a_ini);
		user.style.popupBorderSize = GET_VALUE<float>(rSections[Child], "PopupBorderSize", def.style.popupBorderSize, a_ini);
		user.style.popupRounding = GET_VALUE<float>(rSections[Child], "PopupRounding", def.style.popupRounding, a_ini);

		user.style.showTableRowBG = GET_VALUE<bool>(rSections[Table], "ShowTableRowBG", def.style.showTableRowBG, a_ini);

		user.style.font = GET_VALUE<GraphicManager::Font>(rSections[Fonts], "TextFont", def.style.font, a_ini);
		user.style.globalFontSize = GET_VALUE<float>(rSections[Fonts], "GlobalFontSize", def.style.globalFontSize, a_ini);

		user.style.splashImage = GET_VALUE<GraphicManager::Image>(rSections[Images], "SplashImage", def.style.splashImage, a_ini);
		user.style.favoriteIconEnabled = GET_VALUE<GraphicManager::Image>(rSections[Images], "FavoriteIconEnabled", def.style.favoriteIconEnabled, a_ini);
		user.style.favoriteIconDisabled = GET_VALUE<GraphicManager::Image>(rSections[Images], "FavoriteIconDisabled", def.style.favoriteIconDisabled, a_ini);
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
			a_ini.SetValue(rSections[Child], "ChildBGColor", Settings::ToString(a_user.childBg, false).c_str());
			a_ini.SetValue(rSections[Child], "PopupBGColor", Settings::ToString(a_user.popupBg, false).c_str());
			a_ini.SetValue(rSections[Window], "BorderColor", Settings::ToString(a_user.border, false).c_str());
			a_ini.SetValue(rSections[Window], "BorderShadowColor", Settings::ToString(a_user.borderShadow, false).c_str());
			a_ini.SetValue(rSections[Frame], "FrameBGColor", Settings::ToString(a_user.frameBg, false).c_str());
			a_ini.SetValue(rSections[Frame], "FrameBGHoveredColor", Settings::ToString(a_user.frameBgHovered, false).c_str());
			a_ini.SetValue(rSections[Frame], "FrameBGActiveColor", Settings::ToString(a_user.frameBgActive, false).c_str());
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

			a_ini.SetValue(rSections[Window], "WindowPadding", Settings::ToString(a_user.windowPadding, false).c_str());
			a_ini.SetValue(rSections[Frame], "FramePadding", Settings::ToString(a_user.framePadding, false).c_str());
			a_ini.SetValue(rSections[Table], "CellPadding", Settings::ToString(a_user.cellPadding, false).c_str());
			a_ini.SetValue(rSections[Window], "ItemSpacing", Settings::ToString(a_user.itemSpacing, false).c_str());
			a_ini.SetValue(rSections[Window], "ItemInnerSpacing", Settings::ToString(a_user.itemInnerSpacing, false).c_str());

			a_ini.SetValue(rSections[Window], "Alpha", Settings::ToString(a_user.alpha, false).c_str());
			a_ini.SetValue(rSections[Window], "WindowRounding", Settings::ToString(a_user.windowRounding, false).c_str());
			a_ini.SetValue(rSections[Window], "WindowBorderSize", Settings::ToString(a_user.windowBorderSize, false).c_str());
			a_ini.SetValue(rSections[Child], "ChildBorderSize", Settings::ToString(a_user.childBorderSize, false).c_str());
			a_ini.SetValue(rSections[Child], "ChildRounding", Settings::ToString(a_user.childRounding, false).c_str());
			a_ini.SetValue(rSections[Frame], "FrameBorderSize", Settings::ToString(a_user.frameBorderSize, false).c_str());
			a_ini.SetValue(rSections[Frame], "FrameRounding", Settings::ToString(a_user.frameRounding, false).c_str());
			a_ini.SetValue(rSections[Window], "IndentSpacing", Settings::ToString(a_user.indentSpacing, false).c_str());
			a_ini.SetValue(rSections[Table], "ColumnMinSpacing", Settings::ToString(a_user.columnsMinSpacing, false).c_str());
			a_ini.SetValue(rSections[Widgets], "ScrollbarRounding", Settings::ToString(a_user.scrollbarRounding, false).c_str());
			a_ini.SetValue(rSections[Widgets], "ScrollbarSize", Settings::ToString(a_user.scrollbarSize, false).c_str());
			a_ini.SetValue(rSections[Widgets], "GrabMinSize", Settings::ToString(a_user.grabMinSize, false).c_str());
			a_ini.SetValue(rSections[Widgets], "GrabRounding", Settings::ToString(a_user.grabRounding, false).c_str());
			a_ini.SetValue(rSections[Child], "PopupBorderSize", Settings::ToString(a_user.popupBorderSize, false).c_str());
			a_ini.SetValue(rSections[Child], "PopupRounding", Settings::ToString(a_user.popupRounding, false).c_str());

			a_ini.SetValue(rSections[Table], "ShowTableRowBG", Settings::ToString(a_user.showTableRowBG, false).c_str());

			a_ini.SetValue(rSections[Fonts], "Font", Settings::ToString(a_user.font, false).c_str());
			a_ini.SetValue(rSections[Fonts], "GlobalFontSize", Settings::ToString(a_user.globalFontSize, false).c_str());

			a_ini.SetValue(rSections[Images], "SplashImage", Settings::ToString(a_user.splashImage, false).c_str());
			a_ini.SetValue(rSections[Images], "FavoriteIconEnabled", Settings::ToString(a_user.favoriteIconEnabled, false).c_str());
			a_ini.SetValue(rSections[Images], "FavoriteIconDisabled", Settings::ToString(a_user.favoriteIconDisabled, false).c_str());
		});
	};
}