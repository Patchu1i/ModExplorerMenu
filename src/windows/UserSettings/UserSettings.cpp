#include "UserSettings.h"
#include "Data.h"
#include "Language.h"
#include "Menu.h"
#include "Settings.h"
#include "Utils/Anim.h"
#include "Utils/Keycode.h"
#include "Utils/Util.h"
#include "Windows/AddItem/AddItem.h"
#include <codecvt>

namespace ModExplorerMenu
{
	//
	// # Begin Helpers
	//

	void AddColorPicker(const char* a_text, ImVec4& a_colRef)
	{
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		constexpr ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar;
		auto id = "##ColorPicker" + std::string(_T(a_text));
		auto width = ImGui::GetFontSize();
		ImGui::Text(_T(a_text));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - width - 10.0f);
		ImGui::PushItemWidth(50.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		if (ImGui::ColorEdit4(id.c_str(), (float*)&a_colRef, flags)) {
			SettingsWindow::changes.store(true);
			SettingsWindow::file_changes.store(true);
		}
		ImGui::PopStyleVar(1);
		ImGui::PopItemWidth();
	}

	void AddSliderPicker(const char* a_text, float& a_valRef, float a_min, float a_max, const char* help = nullptr)
	{
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		auto id = "##SliderPicker" + std::string(a_text);
		auto width = 200.0f;

		if (help != nullptr) {
			ImGui::HelpMarker(help);
		}

		ImGui::Text(_T(a_text));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - width - 10.0f);
		ImGui::SetNextItemWidth(width);
		if (ImGui::SliderFloat(id.c_str(), &a_valRef, a_min, a_max)) {
			SettingsWindow::changes.store(true);
			SettingsWindow::file_changes.store(true);
		}
	}

	void AddDualSlider(const char* a_text, float& a_valRef_a, float& a_valRef_b, float a_min, float a_max)
	{
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		auto id = "##DualSlider" + std::string(a_text);
		auto width = 200.0f;
		float new_size[] = { a_valRef_a, a_valRef_b };
		ImGui::Text(_T(a_text));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - width - 10.0f);
		ImGui::SetNextItemWidth(width);
		if (ImGui::SliderFloat2(id.c_str(), new_size, a_min, a_max, "%.1f")) {
			a_valRef_a = new_size[0];
			a_valRef_b = new_size[1];
			SettingsWindow::changes.store(true);
			SettingsWindow::file_changes.store(true);
		}
	}

	static int newModifier = 0;
	void AddModifier(const char* a_text, int& a_modifier, int defaultKey)
	{
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		auto id = "##Modifier" + std::string(a_text);
		auto width = 200.0f;
		ImGui::Text(_T(a_text));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - width - 10.0f);
		ImGui::SetNextItemWidth(width);
		if (ImGui::Button(std::to_string(a_modifier).c_str())) {
			ImGui::OpenPopup("##ModifierPopup");
			newModifier = 0;
		}

		ImGui::SameLine();

		if (ImGui::Button("Reset##ResetModifier")) {
			a_modifier = defaultKey;
			SettingsWindow::changes.store(true);
			SettingsWindow::file_changes.store(true);
		}

		constexpr auto flags = ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopup("##ModifierPopup", flags)) {
			if (newModifier > 0) {
				if (ImGui::IsModifierKey(newModifier) && GetAsyncKeyState(newModifier)) {
					a_modifier = ImGui::GetSkyrimKeyCode(newModifier);
					SettingsWindow::changes.store(true);
					SettingsWindow::file_changes.store(true);
					ImGui::CloseCurrentPopup();
				} else {
					newModifier = 0;
				}
			}

			if (newModifier == 0) {
				for (auto& Key : ImGui::KeyCodes) {
					if (GetAsyncKeyState(Key)) {
						if (ImGui::IsModifierKey(Key)) {
							newModifier = Key;
						}

						if (Key == VK_ESCAPE) {
							ImGui::CloseCurrentPopup();
						}
					}
				}
			}

			ImGui::Text(_T("CONFIG_MODIFIER_SET"));
			ImGui::NewLine();
			ImGui::Text(_T("CONFIG_KEY_CANCEL"));

			ImGui::EndPopup();
		}
	}

	static int newKeybind = 0;
	void AddKeybind(const char* a_text, int& a_keybind, int defaultKey)
	{
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		auto id = "##Keybind" + std::string(a_text);
		auto width = 200.0f;
		ImGui::Text(_T(a_text));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - width - 10.0f);
		ImGui::SetNextItemWidth(width);
		if (ImGui::Button(ImGui::SkyrimKeymap.at(a_keybind))) {
			ImGui::OpenPopup("##KeybindPopup");
			newKeybind = 0;
		}

		ImGui::SameLine();

		if (ImGui::Button("Reset##ResetKeybind")) {
			a_keybind = defaultKey;
			SettingsWindow::changes.store(true);
			SettingsWindow::file_changes.store(true);
		}

		constexpr auto flags = ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopup("##KeybindPopup", flags)) {
			if (newKeybind > 0) {
				if (ImGui::IsKeyboardWhitelist(newKeybind) && GetAsyncKeyState(newKeybind)) {
					a_keybind = ImGui::GetSkyrimKeyCode(newKeybind);
					SettingsWindow::changes.store(true);
					SettingsWindow::file_changes.store(true);
					ImGui::CloseCurrentPopup();
				} else {
					newKeybind = 0;
				}
			}

			if (newKeybind == 0) {
				for (auto& Key : ImGui::KeyCodes) {
					if (GetAsyncKeyState(Key)) {
						if (ImGui::IsKeyboardWhitelist(Key)) {
							newKeybind = Key;
						}

						if (Key == VK_ESCAPE) {
							ImGui::CloseCurrentPopup();
						}
					}
				}
			}

			ImGui::Text(_T("CONFIG_HOTKEY_SET"));
			ImGui::NewLine();
			ImGui::Text(_T("CONFIG_KEY_CANCEL"));

			ImGui::EndPopup();
		}

		// for (auto& key : ImGui::KeyCodes) {
		// 	if (GetAsyncKeyState(key)) {
		// 		a_keybind = (uint32_t)key;
		// 		SettingsWindow::changes.store(true);
		// 		SettingsWindow::file_changes.store(true);
		// 		ImGui::CloseCurrentPopup();
		// 		logger::info("Keybind set to: {}", a_keybind);
		// 		break;
		// 	}
		// }
	}

	void AddCheckbox(const char* a_text, bool& a_boolRef)
	{
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		auto id = "##Checkbox" + std::string(a_text);
		auto width = ImGui::GetFontSize();
		ImGui::Text(_T(a_text));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - width - 10.0f);
		if (ImGui::Checkbox(id.c_str(), &a_boolRef)) {
			SettingsWindow::changes.store(true);
			SettingsWindow::file_changes.store(true);
		}
	}

	void AddSelectionDropdown(const char* a_text, int& a_selection, const std::vector<std::string>& a_items)
	{
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		auto id = "##SelectionDropdown" + std::string(a_text);
		auto width = 200.0f;
		ImGui::Text(_T(a_text));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - width - 10.0f);
		ImGui::PushItemWidth(width);
		if (ImGui::BeginCombo(id.c_str(), _T(a_items[a_selection]))) {
			for (int i = 0; i < a_items.size(); ++i) {
				if (ImGui::Selectable(_T(a_items[i]))) {
					a_selection = i;
					SettingsWindow::changes.store(true);
					SettingsWindow::file_changes.store(true);
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
	}

	void AddImageDropdown(const char* a_text, GraphicManager::Image* a_imageRef)
	{
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		auto id = "##ImageDropdown" + std::string(a_text);
		auto imageName = GraphicManager::GetImageName(*a_imageRef);
		auto width = 200.0f;
		constexpr auto flags = ImGuiComboFlags_None;
		ImGui::Text(_T(a_text));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - width - 10.0f);
		ImGui::PushItemWidth(width);
		if (ImGui::BeginCombo(id.c_str(), imageName.c_str(), flags)) {
			auto images = Settings::GetListOfImages();
			ImGui::PushID("##ImageSelectionPopup");
			for (const auto& image : images) {
				if (ImGui::Selectable(image.first.c_str())) {
					*a_imageRef = image.second;
					SettingsWindow::changes.store(true);
					SettingsWindow::file_changes.store(true);
				}
			}
			ImGui::PopID();
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
	}

	void AddFontDropdown(const char* a_text, GraphicManager::Font* a_fontRef)
	{
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		auto id = "##FontDropdown" + std::string(a_text);
		auto fontName = GraphicManager::GetFontName(*a_fontRef);
		auto width = 200.0f;
		constexpr auto flags = ImGuiComboFlags_None;
		ImGui::Text(_T(a_text));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - width - 10.0f);
		ImGui::PushItemWidth(width);
		if (ImGui::BeginCombo(id.c_str(), fontName.c_str(), flags)) {
			auto fonts = Settings::GetListOfFonts();
			ImGui::PushID("##FontSelectionPopup");
			for (const auto& font : fonts) {
				if (ImGui::Selectable(font.first.c_str())) {
					*a_fontRef = font.second;
					SettingsWindow::file_changes.store(true);
				}
			}
			ImGui::PopID();
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
	}

	void SaveThemeToFile(std::string a_path, Settings::Style& a_style)
	{
		std::wstring path(Settings::ini_theme_path);

		// Add the .ini extension if it doesn't exist
		if (a_path.rfind(".ini") != a_path.length() - 4) {
			a_path += ".ini";
		}

		std::wstring full_path = path + std::wstring(a_path.begin(), a_path.end());
		const wchar_t* new_path = full_path.c_str();

		Settings::GetSingleton()->ExportThemeToIni(new_path, a_style);
		Settings::GetSingleton()->SaveSettings();
	}

	//
	// # End Helpers
	//

	void SettingsWindow::DrawPopped()
	{
		constexpr auto window_flags = ImGuiWindowFlags_NoCollapse;
		if (ImGui::Begin(_T("THEME_POPUP_TITLE"), nullptr, window_flags)) {
			DrawThemeSelector();

			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
			if (ImGui::Button(_T("THEME_POPUP_CLOSE"), ImVec2(ImGui::GetContentRegionAvail().x, 25.0f))) {
				Menu::is_settings_popped = false;
			}
			ImGui::PopStyleColor(1);
		}

		ImGui::End();
	}

	void SettingsWindow::DrawGeneralSettings()
	{
		//Settings::Style& style = Settings::GetSingleton()->GetStyle();
		Settings::Config& config = Settings::GetSingleton()->GetConfig();

		if (changes.load()) {
			Menu::GetSingleton()->RefreshStyle();
			changes.store(false);
		}

		AddKeybind("SETTING_MENU_KEYBIND", config.showMenuKey, 211);
		AddModifier("SETTING_MENU_MODIFIER", config.showMenuModifier, 0);

		// std::vector<std::string> languages = { "English", "Russian", "Chinese", "Japanese", "Korean", "French", "German", "Spanish", "Italian" };
		// if (AddSelectionStringDropdown("Language", config.language, languages)) {
		// 	GraphicManager::SetLanguage(config.language);
		// 	changes.store(true);
		// }

		auto width = 200.0f;
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		ImGui::Text(_TFM("Language", ":"));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - width - 10.0f);
		ImGui::PushItemWidth(width);

		auto languages = Language::GetSupportedLanguageList();

		if (ImGui::BeginCombo("##LanguageSelection", Language::GetLanguageName(config.language).c_str())) {
			for (int i = 0; i < languages.size(); ++i) {
				if (ImGui::Selectable(languages[i].c_str())) {
					config.language = Language::Locale(i);
					SettingsWindow::changes.store(true);
					SettingsWindow::file_changes.store(true);
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		std::vector<std::string> sorts = { "SETTING_SORT_ALPHA", "SETTING_SORT_RECENT" };
		AddSelectionDropdown("SETTING_SORT", config.modListSort, sorts);
	}

	void SettingsWindow::DrawThemeSelector()
	{
		Settings::Style& style = Settings::GetSingleton()->GetStyle();
		Settings::Config& config = Settings::GetSingleton()->GetConfig();
		constexpr auto color_flags = ImGuiTreeNodeFlags_Framed;

		if (changes.load()) {
			Menu::GetSingleton()->RefreshStyle();
			changes.store(false);
		}

		if (ImGui::CollapsingHeader(_T("THEME_PRESET"), color_flags | ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SeparatorText(_TFM("THEME_PRESET_SELECT", ":"));

			constexpr auto combo_flags = ImGuiComboFlags_HeightLarge;
			ImGui::HelpMarker(_T("THEME_HELP_CHOICE"));
			if (ImGui::BeginCombo("##Settings::PresetDropdown", config.theme.c_str(), combo_flags)) {
				std::vector<std::string> themes = Settings::GetListOfThemes();

				for (const auto& theme : themes) {
					if (ImGui::Selectable(theme.c_str())) {
						config.theme = theme;
						Settings::GetSingleton()->SetThemeFromIni(config.theme);
						Settings::GetSingleton()->SaveSettings();
						changes.store(true);
					}
				}

				ImGui::EndCombo();
			}

			ImGui::SameLine();

			float bAlpha = 1.0f;
			if (config.theme == "Default") {
				bAlpha = ImGuiAnim::pulse(2.5f, 0.5f, 1.0f);
			}
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(style.button.x, style.button.y, style.button.z, bAlpha));
			if (ImGui::Button(_T("THEME_PRESET_CREATE"))) {
				ImGui::OpenPopup("new_theme");
			}
			ImGui::PopStyleColor(1);

			if (ImGui::BeginPopup("new_theme")) {
				constexpr auto input_flags = ImGuiInputTextFlags_EnterReturnsTrue;
				char newTheme[128] = { 0 };
				ImGui::HelpMarker(_T("THEME_HELP_CREATE"));
				if (ImGui::InputTextWithHint("##InputThemeName", _T("THEME_NAME_INPUT"), newTheme, IM_ARRAYSIZE(newTheme), input_flags)) {
					if (strcmp(newTheme, "Default") == 0) {
						ImGui::EndPopup();
						return;
					}
					SaveThemeToFile(newTheme, style);
					config.theme = newTheme;
					changes.store(true);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			if (config.theme == "Default") {
				// ImGui::SeparatorText("Note:");
				// ImGui::Text("You are currently using the Default theme, which cannot be modified.");
				// ImGui::NewLine();
				// ImGui::Text("Please select a different theme from the dropdown above.");
				// ImGui::NewLine();
				// ImGui::Text("Or create a new theme by pressing 'Create New Theme'");

				ImGui::SeparatorText(_TFM("Note", ":"));

				ImGui::Text(_T("THEME_NOTE_1"));
				ImGui::NewLine();
				ImGui::Text(_T("THEME_NOTE_2"));
				ImGui::NewLine();
				ImGui::Text(_T("THEME_NOTE_3"));
				return;
			}

			ImGui::SeparatorText(_TFM("THEME_CHANGES", +":"));

			ImGui::HelpMarker(_T("THEME_HELP_RESET"));
			if (ImGui::Button(_T("THEME_RESET"), ImVec2(ImGui::GetContentRegionAvail().x - 20.0f, ImGui::GetFontSize() * 1.5f))) {
				std::string _default = "Default";
				Settings::SetThemeFromIni(_default);
				changes.store(true);
			}

			ImGui::HelpMarker(_T("THEME_HELP_EXPORT"));
			if (ImGui::Button(_T("THEME_EXPORT"), ImVec2(ImGui::GetContentRegionAvail().x - 20.0f, ImGui::GetFontSize() * 1.5f))) {
				ImGui::OpenPopup("save_to_file");
			}

			if (ImGui::BeginPopup("save_to_file")) {
				ImGui::Text(_T("THEME_SAVE"));
				constexpr auto input_flags = ImGuiInputTextFlags_EnterReturnsTrue;
				if (ImGui::InputTextWithHint("##InputFilename", _T("THEME_NAME_INPUT"),
						savePath,
						IM_ARRAYSIZE(savePath),
						input_flags)) {
					SaveThemeToFile(savePath, style);
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::Button("Save")) {
					SaveThemeToFile(savePath, style);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			ImGui::HelpMarker("THEME_HELP_POP_WINDOW");
			if (ImGui::Button(_T("THEME_POPUP"), ImVec2(ImGui::GetContentRegionAvail().x - 20.0f, ImGui::GetFontSize() * 1.5f))) {
				Menu::is_settings_popped = !Menu::is_settings_popped;
			}
		}

		if (config.theme == "Default") {
			return;
		};

		if (ImGui::CollapsingHeader(_T("THEME_WINDOW_STYLE"), color_flags)) {
			ImGui::Indent();
			AddColorPicker("THEME_WINDOW_COLOR", style.windowBg);
			AddSliderPicker("THEME_WINDOW_ALPHA", style.alpha, 0.1f, 1.0f);
			AddDualSlider("THEME_WINDOW_PADDING", style.windowPadding.x, style.windowPadding.y, 0.1f, 25.0f);
			AddColorPicker("THEME_BORDER_COLOR", style.border);
			AddColorPicker("THEME_BORDER_SHADOW_COLOR", style.borderShadow);
			AddSliderPicker("THEME_WINDOW_ROUNDING", style.windowRounding, 0.1f, 15.0f);
			AddSliderPicker("THEME_WINDOW_BORDER_SIZE", style.windowBorderSize, 0.1f, 15.0f);
			AddDualSlider("THEME_ITEM_SPACING", style.itemSpacing.x, style.itemSpacing.y, 0.1f, 20.0f);
			AddDualSlider("THEME_ITEM_INNER_SPACING", style.itemInnerSpacing.x, style.itemInnerSpacing.y, 0.1f, 20.0f);
			AddSliderPicker("THEME_INDENT_SPACING", style.indentSpacing, 0.1f, 35.0f);
			AddImageDropdown("THEME_SPLASH_IMAGE", &style.splashImage);
			ImGui::Unindent();
		}

		if (ImGui::CollapsingHeader(_T("THEME_FRAME_STYLE"), color_flags)) {
			ImGui::Indent();
			AddColorPicker("THEME_FRAME_BG_COLOR", style.frameBg);
			AddColorPicker("THEME_FRAME_BG_HOVER_COLOR", style.frameBgHovered);
			AddColorPicker("THEME_FRAME_BG_ACTIVE_COLOR", style.frameBgActive);
			AddSliderPicker("THEME_FRAME_ROUNDING", style.frameRounding, 0.1f, 15.0f);
			AddSliderPicker("THEME_FRAME_BORDER_SIZE", style.frameBorderSize, 0.1f, 15.0f);
			AddDualSlider("THEME_FRAME_PADDING", style.framePadding.x, style.framePadding.y, 0.1f, 25.0f);
			ImGui::Unindent();
		}

		if (ImGui::CollapsingHeader(_T("THEME_CHILD_POPUP_STYLE"), color_flags)) {
			ImGui::Indent();
			AddColorPicker("THEME_CHILD_WINDOW_COLOR", style.childBg);
			AddSliderPicker("THEME_CHILD_BORDER_SIZE", style.childBorderSize, 0.1f, 15.0f);
			AddSliderPicker("THEME_CHILD_ROUNDING", style.childRounding, 0.1f, 15.0f);
			AddColorPicker("THEME_POPUP_BG_COLOR", style.popupBg);
			AddSliderPicker("THEME_POPUP_BORDER_SIZE", style.popupBorderSize, 0.1f, 15.0f);
			AddSliderPicker("THEME_POPUP_ROUNDING", style.popupRounding, 0.1f, 15.0f);
			ImGui::Unindent();
		}

		if (ImGui::CollapsingHeader(_T("THEME_TEXT_FONTS"), color_flags)) {
			ImGui::Indent();
			AddColorPicker("THEME_TEXT_COLOR", style.text);
			AddColorPicker("THEME_TEXT_DISABLED_COLOR", style.textDisabled);
			AddColorPicker("THEME_TEXT_SELECTED_BG_COLOR", style.textSelectedBg);
			AddFontDropdown("THEME_TEXT_FONT", &style.font);
			AddSliderPicker("THEME_TEXT_FONT_SIZE", style.globalFontSize, 0.5f, 3.0f);
			ImGui::Unindent();
		}

		if (ImGui::CollapsingHeader(_T("THEME_TABLE_COLUMN_STYLE"), color_flags)) {
			ImGui::Indent();
			AddColorPicker("THEME_HEADER_COLOR", style.header);
			AddColorPicker("THEME_HEADER_HOVER_COLOR", style.headerHovered);
			AddColorPicker("THEME_HEADER_ACTIVE_COLOR", style.headerActive);
			AddDualSlider("THEME_CELL_PADDING", style.cellPadding.x, style.cellPadding.y, 0.1f, 20.0f);
			AddSliderPicker("THEME_COLUMNS_MIN_SPACING", style.columnsMinSpacing, 0.1f, 20.0f);
			AddColorPicker("THEME_TABLE_HEADER_BG_COLOR", style.tableHeaderBg);
			AddColorPicker("THEME_TABLE_BORDER_STRONG_COLOR", style.tableBorderStrong);
			AddColorPicker("THEME_TABLE_BORDER_LIGHT_COLOR", style.tableBorderLight);
			AddColorPicker("THEME_TABLE_ROW_BG_COLOR", style.tableRowBg);
			AddCheckbox("THEME_TABLE_ROW_ALTERNATING_BG", style.showTableRowBG);
			ImGui::Unindent();
		}

		if (ImGui::CollapsingHeader(_T("THEME_BUTTONS_WIDGET_STYLE"), color_flags)) {
			ImGui::Indent();
			AddColorPicker("THEME_CHECKMARK_COLOR", style.checkMark);
			AddColorPicker("THEME_BUTTON_COLOR", style.button);
			AddColorPicker("THEME_BUTTON_HOVER_COLOR", style.buttonHovered);
			AddColorPicker("THEME_BUTTON_ACTIVE_COLOR", style.buttonActive);
			AddColorPicker("THEME_SCROLLBAR_BG_COLOR", style.scrollbarBg);
			AddColorPicker("THEME_SCROLLBAR_GRAB_COLOR", style.scrollbarGrab);
			AddColorPicker("THEME_SCROLLBAR_GRAB_HOVER_COLOR", style.scrollbarGrabHovered);
			AddColorPicker("THEME_SCROLLBAR_GRAB_ACTIVE_COLOR", style.scrollbarGrabActive);
			AddSliderPicker("THEME_SCROLLBAR_ROUNDING", style.scrollbarRounding, 0.1f, 35.0f);
			AddSliderPicker("THEME_SCROLLBAR_SIZE", style.scrollbarSize, 0.1f, 50.0f);
			AddSliderPicker("THEME_GRABBER_MIN_SIZE", style.grabMinSize, 0.1f, 20.0f);
			AddSliderPicker("THEME_GRABBER_ROUNDING", style.grabRounding, 0.1f, 20.0f);
			AddColorPicker("THEME_SLIDER_GRAB_COLOR", style.sliderGrab);
			AddColorPicker("THEME_SLIDER_GRAB_ACTIVE_COLOR", style.sliderGrabActive);
			AddColorPicker("THEME_SEPARATOR_COLOR", style.separator);
			AddColorPicker("THEME_SEPARATOR_HOVER_COLOR", style.separatorHovered);
			AddColorPicker("THEME_SEPARATOR_ACTIVE_COLOR", style.separatorActive);
			AddImageDropdown("THEME_FAVORITE_ICON_ENABLED", &style.favoriteIconEnabled);
			AddImageDropdown("THEME_FAVORITE_ICON_DISABLED", &style.favoriteIconDisabled);
			ImGui::Unindent();
		}

		ImGui::NewLine();
	}

	void SettingsWindow::Draw()
	{
		ImGui::Text(_T("THEME_WELCOME"));

		constexpr auto child_flags = ImGuiChildFlags_Border;
		if (ImGui::BeginChild("##SettingsTable", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 30.0f), child_flags)) {
			if (ImGui::CollapsingHeader(_T("THEME_GENERAL"), ImGuiTreeNodeFlags_Framed)) {
				ImGui::Indent();
				DrawGeneralSettings();
				ImGui::Unindent();
			}

			if (ImGui::CollapsingHeader(_T("THEME_THEME"), ImGuiTreeNodeFlags_Framed)) {
				ImGui::Indent();
				DrawThemeSelector();
				ImGui::Unindent();
			}
		}
		ImGui::EndChild();

		auto& config = Settings::GetSingleton()->GetConfig();
		auto& style = Settings::GetSingleton()->GetStyle();

		if (SettingsWindow::file_changes.load()) {
			float alpha = ImGuiAnim::pulse(3.0f, 0.7f, 1.0f);
			ImGui::HelpMarker(_T("THEME_HELP_SAVE"));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.7f, 0.2f, alpha));
			if (ImGui::Button(_T("THEME_SAVE_CHANGES"), ImVec2(ImGui::GetContentRegionAvail().x - 20.0f, ImGui::GetFontSize() * 1.5f))) {
				SaveThemeToFile(config.theme, style);
				SettingsWindow::file_changes.store(false);
			}
			ImGui::PopStyleColor(1);
		}
	}

	void SettingsWindow::Init()
	{
		// Open = true;
	}
}
