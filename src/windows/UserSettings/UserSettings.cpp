#include "include/U/UserSettings.h"
#include "include/A/AddItem.h"
#include "include/D/Data.h"
#include "include/G/Graphic.h"
#include "include/K/Keycode.h"
#include "include/M/Menu.h"
#include "include/N/NPC.h"
#include "include/O/Object.h"
#include "include/P/Persistent.h"
#include "include/T/Teleport.h"
#include "include/U/Util.h"

// TODO: God this is a mess. It started out strong, but as I've added more and more settings without
// much consideration into scale, it's bad. I need to re-do this entireley ASAP.

namespace Modex
{
	//
	// # Begin Helpers
	//

	static constexpr float p_fixedWidth = 150.0f;
	static constexpr float p_padding = 0.0f;  // placeholder

	void AddColorPicker(const char* a_text, ImVec4& a_colRef)
	{
		constexpr ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar;
		auto id = "##ColorPicker" + std::string(_T(a_text));
		auto popup = id + "-Popup";
		ImGui::Spacing();
		ImGui::Text(_T(a_text));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - p_fixedWidth - p_padding - ImGui::GetStyle().IndentSpacing);

		if (ImGui::ColorButton(id.c_str(), a_colRef, flags, ImVec2(p_fixedWidth, 0))) {
			ImGui::OpenPopup(popup.c_str());
		}

		if (ImGui::BeginPopup(popup.c_str())) {
			if (ImGui::ColorPicker4(id.c_str(), (float*)&a_colRef, flags)) {
				SettingsWindow::changes.store(true);
				SettingsWindow::file_changes.store(true);
			}
			ImGui::EndPopup();
		}

		ImGui::Spacing();

		// ImGui::PushItemWidth(50.0f);
		// ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		// if (ImGui::ColorEdit4(id.c_str(), (float*)&a_colRef, flags)) {
		// 	SettingsWindow::changes.store(true);
		// 	SettingsWindow::file_changes.store(true);
		// }
	}

	void AddSliderPicker(const char* a_text, float& a_valRef, float a_min, float a_max)
	{
		auto id = "##SliderPicker" + std::string(a_text);
		ImGui::Spacing();
		ImGui::Text(_T(a_text));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - p_fixedWidth - p_padding - ImGui::GetStyle().IndentSpacing);
		ImGui::SetNextItemWidth(p_fixedWidth);
		if (ImGui::SliderFloat(id.c_str(), &a_valRef, a_min, a_max)) {
			SettingsWindow::changes.store(true);
			SettingsWindow::file_changes.store(true);
		}
		ImGui::Spacing();
	}

	void AddDualSlider(const char* a_text, float& a_valRef_a, float& a_valRef_b, float a_min, float a_max)
	{
		auto id = "##DualSlider" + std::string(a_text);
		float new_size[] = { a_valRef_a, a_valRef_b };
		ImGui::Spacing();
		ImGui::Text(_T(a_text));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - p_fixedWidth - p_padding - ImGui::GetStyle().IndentSpacing);
		ImGui::SetNextItemWidth(p_fixedWidth);
		if (ImGui::SliderFloat2(id.c_str(), new_size, a_min, a_max, "%.1f")) {
			a_valRef_a = new_size[0];
			a_valRef_b = new_size[1];
			SettingsWindow::changes.store(true);
			SettingsWindow::file_changes.store(true);
		}
		ImGui::Spacing();
	}

	// ASyncKeyState is so fast that I store the new keybind in the below variable
	// and check if the key is still pressed in the next frame.
	static int newModifier = 0;

	// I have separated the AddModifier and AddKeybind functions into two separate functions
	// because the AddModifier function requires a different implementation than the AddKeybind function.
	// More specifically, the behavior for checking valid modifier keys and confirmation.
	void AddModifier(const char* a_text, int& a_modifier, int defaultKey, ImVec4& a_hover)
	{
		Settings::Config& config = Settings::GetSingleton()->GetConfig();
		Settings::Style& style = Settings::GetSingleton()->GetStyle();

		auto id = "##Modifier" + std::string(a_text);
		ImGui::Spacing();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + style.itemSpacing.y / 2 + ImGui::GetFontSize() / 2);
		ImGui::Text(_T(a_text));

		if (GraphicManager::imgui_library.empty()) {
			const float width = ImGui::GetFontSize() * 8.0f;
			const float height = ((config.uiScale / 100) * 20.0f) + 10.0f;

			ImGui::SameLine(ImGui::GetContentRegionAvail().x - width + 20.0f);   // Right Align
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (height / 2) + 5.0f);  // Center Align

			ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
			if (ImGui::Button(ImGui::SkyrimKeymap.at(a_modifier), ImVec2(width, height + 5.0f))) {
				ImGui::OpenPopup("##ModifierPopup");
				newModifier = 0;
			}
			ImGui::PopStyleVar();
		} else {
			const ImVec2& uv0 = ImVec2(0, 0);
			const ImVec2& uv1 = ImVec2(1, 1);
			const ImVec4& bg_col = ImVec4(0, 0, 0, 0);
			const float alpha = 1.0f;

			GraphicManager::Image img = GraphicManager::imgui_library[ImGui::ImGuiKeymap.at(a_modifier)];

			float scale = config.uiScale / 100.0f;
			const float imageWidth = ((float)img.width * 0.5f) * scale;
			const float imageHeight = ((float)img.height * 0.5f) * scale;
			const ImVec2& size = ImVec2(imageWidth, imageHeight);

			ImGui::SameLine(ImGui::GetContentRegionMax().x - (p_fixedWidth - p_padding) + imageWidth / scale);  // Center Align
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (imageHeight / 2) + 5.0f);                            // Center Align

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * alpha);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.f, 0.f, 0.f, 0.f));

			ImTextureID texture = (ImTextureID)(intptr_t)img.texture;
			if (ImGui::ImageButton("##ImGuiHotkeyModifier", texture, size, uv0, uv1, bg_col, a_hover)) {
				ImGui::OpenPopup("##ModifierPopup");
				newModifier = 0;
			}

			if (ImGui::IsItemHovered()) {
				a_hover = ImVec4(1.f, 1.f, 1.f, 1.f);
			} else {
				a_hover = ImVec4(0.9f, 0.9f, 0.9f, 0.9f);
			}

			ImGui::PopStyleVar();
			ImGui::PopStyleColor(5);
		}

		ImGui::Spacing();

		constexpr auto flags = ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopup("##ModifierPopup", flags)) {
			auto _prevModifier = a_modifier;
			int _newModifier = 0;

			ImGui::Text(_T("CONFIG_MODIFIER_SET"));
			ImGui::NewLine();
			ImGui::Text(_T("CONFIG_KEY_RESET"));
			ImGui::NewLine();
			ImGui::Text(_T("CONFIG_KEY_CANCEL"));

			if (ImGui::IsKeyPressed(ImGuiMod_Alt)) {
				_newModifier = ImGui::VirtualKeyToSkyrim(VK_LMENU);
			} else if (ImGui::IsKeyPressed(ImGuiMod_Ctrl)) {
				_newModifier = ImGui::VirtualKeyToSkyrim(VK_LCONTROL);
			} else if (ImGui::IsKeyPressed(ImGuiMod_Shift)) {
				_newModifier = ImGui::VirtualKeyToSkyrim(VK_LSHIFT);
			}

			if (_newModifier != 0) {
				if (_newModifier != _prevModifier) {
					a_modifier = _newModifier;
					Settings::GetSingleton()->SaveSettings();
				}

				ImGui::CloseCurrentPopup();
			} else {
				if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::IsKeyPressed(ImGuiKey_T)) {
					a_modifier = defaultKey;
					Settings::GetSingleton()->SaveSettings();
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndPopup();
		}
	}

	static int newKeybind = 0;
	void AddKeybind(const char* a_text, int& a_keybind, int defaultKey, ImVec4& a_hover)
	{
		Settings::Config& config = Settings::GetSingleton()->GetConfig();
		Settings::Style& style = Settings::GetSingleton()->GetStyle();

		auto id = "##Keybind" + std::string(a_text);
		ImGui::Spacing();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + style.itemSpacing.y / 2 + ImGui::GetFontSize() / 2);
		ImGui::Text(_T(a_text));

		if (GraphicManager::imgui_library.empty()) {
			const float width = ImGui::GetFontSize() * 8.0f;
			const float height = ((config.uiScale / 100) * 20.0f) + 10.0f;

			ImGui::SameLine(ImGui::GetContentRegionAvail().x - width + 20.0f);   // Right Align
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (height / 2) + 5.0f);  // Center Align

			ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
			if (ImGui::Button(ImGui::SkyrimKeymap.at(a_keybind), ImVec2(width, height + 5.0f))) {
				ImGui::OpenPopup("##KeybindPopup");
				newKeybind = 0;
			}
			ImGui::PopStyleVar();
		} else {
			const ImVec2& uv0 = ImVec2(0, 0);
			const ImVec2& uv1 = ImVec2(1, 1);
			const ImVec4& bg_col = ImVec4(0, 0, 0, 0);
			const float alpha = 1.0f;

			GraphicManager::Image img = GraphicManager::imgui_library[ImGui::ImGuiKeymap.at(a_keybind)];

			float scale = config.uiScale / 100.0f;
			const float imageWidth = ((float)img.width * 0.5f) * scale;
			const float imageHeight = ((float)img.height * 0.5f) * scale;
			const ImVec2& size = ImVec2(imageWidth, imageHeight);

			ImGui::SameLine(ImGui::GetContentRegionMax().x - (p_fixedWidth - p_padding) + imageWidth / scale);  // Right Align
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (imageHeight / 2) + 5.0f);                            // Center Align

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * alpha);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.f, 0.f, 0.f, 0.f));

			ImTextureID texture = (ImTextureID)(intptr_t)img.texture;
			if (ImGui::ImageButton("##ImGuiHotkey", texture, size, uv0, uv1, bg_col, a_hover)) {
				ImGui::OpenPopup("##KeybindPopup");
				newModifier = 0;
			}

			if (ImGui::IsItemHovered()) {
				a_hover = ImVec4(1.f, 1.f, 1.f, 1.f);
			} else {
				a_hover = ImVec4(0.9f, 0.9f, 0.9f, 0.9f);
			}

			ImGui::PopStyleVar();
			ImGui::PopStyleColor(5);
		}

		ImGui::Spacing();

		constexpr auto flags = ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopup("##KeybindPopup", flags)) {
			if (ImGui::IsWindowAppearing()) {
				ImGui::GetIO().ClearInputKeys();
			}
			auto _prevKeybind = a_keybind;
			int _newKeybind = 0;

			ImGui::Text(_T("CONFIG_HOTKEY_SET"));
			ImGui::NewLine();
			ImGui::Text(_T("CONFIG_KEY_RESET"));
			ImGui::NewLine();
			ImGui::Text(_T("CONFIG_KEY_CANCEL"));

			for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key++) {
				ImGuiKey imGuiKey = static_cast<ImGuiKey>(key);

				if (ImGui::IsKeyPressed(imGuiKey)) {
					auto skyrimKey = ImGui::ImGuiKeyToSkyrimKey(imGuiKey);

					if (skyrimKey != 0 && ImGui::IsKeyboardWhitelist(imGuiKey)) {
						_newKeybind = skyrimKey;
					}

					if (_newKeybind == a_keybind) {
						ImGui::CloseCurrentPopup();
					}
				}
			}

			if (_newKeybind != 0) {
				if (_newKeybind != _prevKeybind) {
					a_keybind = _newKeybind;
					Settings::GetSingleton()->SaveSettings();
				}

				ImGui::CloseCurrentPopup();
			} else {
				if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::IsKeyPressed(ImGuiKey_T)) {
					a_keybind = defaultKey;
					Settings::GetSingleton()->SaveSettings();
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndPopup();
		}
	}

	bool AddToggleButton(const char* a_text, bool& a_boolRef)
	{
		// const float width = ImGui::GetFrameHeight() * 3.55f;
		bool result;

		ImGui::Spacing();
		ImGui::Text(_T(a_text));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - p_fixedWidth - p_padding - ImGui::GetStyle().IndentSpacing);
		result = ImGui::ToggleButton(a_text, &a_boolRef, p_fixedWidth);
		ImGui::Spacing();

		return result;
	}

	void AddSelectionDropdown(const char* a_text, int& a_selection, const std::vector<std::string>& a_items)
	{
		auto id = "##SelectionDropdown" + std::string(a_text);
		ImGui::Spacing();
		ImGui::Text(_T(a_text));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - p_fixedWidth - p_padding - ImGui::GetStyle().IndentSpacing);
		ImGui::PushItemWidth(p_fixedWidth);
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
		ImGui::Spacing();
		ImGui::PopItemWidth();
	}

	void AddImageDropdown(const char* a_text, GraphicManager::Image* a_imageRef)
	{
		auto id = "##ImageDropdown" + std::string(a_text);
		auto imageName = GraphicManager::GetImageName(*a_imageRef);
		constexpr auto flags = ImGuiComboFlags_None;

		ImGui::Spacing();
		ImGui::Text(_T(a_text));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - p_fixedWidth - p_padding - ImGui::GetStyle().IndentSpacing);
		ImGui::PushItemWidth(p_fixedWidth);
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
		ImGui::Spacing();
		ImGui::PopItemWidth();
	}

	void AddFontDropdown(const char* a_text, std::string* a_font)
	{
		ImGui::Spacing();
		auto id = "##FontDropdown" + std::string(a_text);
		ImGui::Text(_T(a_text));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - p_fixedWidth - p_padding - ImGui::GetStyle().IndentSpacing);
		ImGui::PushItemWidth(p_fixedWidth);
		if (ImGui::BeginCombo(id.c_str(), a_font->c_str())) {
			auto fontLibrary = FontManager::GetFontLibrary();
			ImGui::PushID("##FontSelectionPopup");
			for (const auto& font : fontLibrary) {
				if (ImGui::Selectable(font.c_str())) {
					*a_font = font;

					SettingsWindow::changes.store(true);
					SettingsWindow::file_changes.store(true);

					Menu::GetSingleton()->RefreshFont();
				}
			}
			ImGui::PopID();
			ImGui::EndCombo();
		}
		ImGui::Spacing();
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

	void AddSubCategoryHeader(const char* a_text)
	{
		ImGui::Unindent();
		ImGui::SubCategoryHeader(_T(a_text));
		ImGui::Indent();
		ImGui::Spacing();
	}

	//
	// # End Helpers
	//

	void SettingsWindow::DrawPopped()
	{
		constexpr auto window_flags = ImGuiWindowFlags_NoCollapse;
		if (ImGui::Begin(_T("THEME_POPUP_TITLE"), nullptr, window_flags)) {
			DrawThemeSelector();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
			if (ImGui::Button(_T("THEME_POPUP_CLOSE"), ImVec2(ImGui::GetContentRegionAvail().x, 25.0f))) {
				Menu::GetSingleton()->showSettingWindow = false;
			}
			ImGui::PopStyleColor(1);
		}

		ImGui::End();
	}

	static inline ImVec4 keyHoverTintColor = ImVec4(0.9f, 0.9f, 0.9f, 0.9f);
	static inline ImVec4 modifierHoverTint = ImVec4(0.9f, 0.9f, 0.9f, 0.9f);
	void SettingsWindow::DrawGeneralSettings()
	{
		// Settings::Style& style = Settings::GetSingleton()->GetStyle();
		Settings::Config& config = Settings::GetSingleton()->GetConfig();

		if (changes.load()) {
			Menu::GetSingleton()->RefreshStyle();
			changes.store(false);
		}

		AddSubCategoryHeader(_T("SETTING_GENERAL"));

		AddKeybind("SETTING_MENU_KEYBIND", config.showMenuKey, 211, keyHoverTintColor);

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		AddModifier("SETTING_MENU_MODIFIER", config.showMenuModifier, 0, modifierHoverTint);

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		std::vector<std::string> sorts = { "SETTING_SORT_ALPHA", "SETTING_SORT_LOAD_ASC", "SETTING_SORT_LOAD_DESC" };
		AddSelectionDropdown("SETTING_SORT", config.modListSort, sorts);

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		// UI Scale Setting
		ImGui::Spacing();
		ImGui::Text(_T("SETTING_UI_SCALE"));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - p_fixedWidth - p_padding - ImGui::GetStyle().IndentSpacing);
		ImGui::PushItemWidth(p_fixedWidth);
		ImGui::SliderInt("##UIScaleSelection", &_uiScale, 60, 120, "%d%%");

		if (ImGui::IsItemDeactivatedAfterEdit()) {
			config.uiScale = _uiScale;

			SettingsWindow::changes.store(true);
			SettingsWindow::file_changes.store(true);
		}
		ImGui::Spacing();
		ImGui::PopItemWidth();
		// End UI Scale Setting

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		if (AddToggleButton("SETTINGS_FULLSCREEN", config.fullscreen)) {
			Frame::GetSingleton()->RefreshStyle();
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		AddToggleButton("SETTINGS_PAUSE_GAME", config.pauseGame);

		AddSubCategoryHeader(_T("SETTING_FONT_AND_LANGUAGE"));

		// Language Dropdown
		ImGui::Spacing();
		ImGui::Text(_T("Language"));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - p_fixedWidth - p_padding - ImGui::GetStyle().IndentSpacing);
		ImGui::PushItemWidth(p_fixedWidth);

		auto languages = Language::GetLanguages();

		if (ImGui::BeginCombo("##LanguageSelection", config.language.c_str())) {
			for (auto& language : languages) {
				if (ImGui::Selectable(language.c_str())) {
					config.language = language;

					SettingsWindow::changes.store(true);
					SettingsWindow::file_changes.store(true);

					Translate::GetSingleton()->RefreshLanguage(config.language);
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Spacing();
		ImGui::PopItemWidth();

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		// Glpyh Dropdown
		ImGui::Spacing();
		ImGui::Text(_T("SETTING_GLYPH_RANGE"));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - p_fixedWidth - p_padding - ImGui::GetStyle().IndentSpacing);
		ImGui::PushItemWidth(p_fixedWidth);

		auto glyphs = Language::GetListOfGlyphNames();
		auto currentGlyph = Language::GetGlyphName(config.glyphRange);

		if (ImGui::BeginCombo("##GlpyhSelection", currentGlyph.data())) {
			for (auto& glyph : glyphs) {
				if (ImGui::Selectable(glyph.data())) {
					config.glyphRange = Language::GetGlyphRange(glyph);

					SettingsWindow::changes.store(true);
					SettingsWindow::file_changes.store(true);

					Menu::GetSingleton()->RefreshFont();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Spacing();
		ImGui::PopItemWidth();

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		AddFontDropdown("SETTING_FONT", &config.globalFont);

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		// Font Size Setting
		ImGui::Spacing();
		ImGui::Text(_T("SETTING_FONT_SIZE"));
		ImGui::SameLine(ImGui::GetContentRegionMax().x - p_fixedWidth - p_padding - ImGui::GetStyle().IndentSpacing);
		ImGui::PushItemWidth(p_fixedWidth);

		ImGui::SliderInt("##FontSizeSelection", &_fontSize, 8, 28, "%d");

		if (ImGui::IsItemDeactivatedAfterEdit()) {
			config.globalFontSize = _fontSize;

			SettingsWindow::changes.store(true);
			SettingsWindow::file_changes.store(true);

			Menu::GetSingleton()->RefreshFont();
		}
		ImGui::Spacing();
		ImGui::PopItemWidth();

		// End of Text & Font Settings
		// Begin of Module settings

		AddSubCategoryHeader(_T("SETTING_MODULE"));

		AddSelectionDropdown("SETTING_DEFAULT_SHOW", config.defaultShow, { "Home", "Add Item", "Object", "NPC", "Teleport", "Settings" });

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		AddToggleButton("SETTING_SHOW_HOME", config.showHomeMenu);

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		if (AddToggleButton("SETTING_SHOW_ADDITEM", config.showAddItemMenu)) {
			if (Data::GetSingleton()->GetAddItemList().empty()) {
				Data::GetSingleton()->GenerateItemList();
				AddItemWindow::GetSingleton()->Refresh();
			}
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		if (AddToggleButton("SETTING_SHOW_OBJECT", config.showObjectMenu)) {
			if (Data::GetSingleton()->GetObjectList().empty()) {
				Data::GetSingleton()->GenerateObjectList();
				ObjectWindow::GetSingleton()->Refresh();
			}
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		if (AddToggleButton("SETTING_SHOW_NPC", config.showNPCMenu)) {
			if (Data::GetSingleton()->GetNPCList().empty()) {
				Data::GetSingleton()->GenerateNPCList();
				NPCWindow::GetSingleton()->Refresh();
			}
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		if (AddToggleButton("SETTING_SHOW_TELEPORT", config.showTeleportMenu)) {
			if (Data::GetSingleton()->GetTeleportList().empty()) {
				Data::GetSingleton()->GenerateCellList();
				TeleportWindow::GetSingleton()->Refresh();
			}
		}
	}

	void SettingsWindow::DrawThemeSelector()
	{
		Settings::Style& style = Settings::GetSingleton()->GetStyle();
		Settings::Config& config = Settings::GetSingleton()->GetConfig();

		if (changes.load()) {
			Menu::GetSingleton()->RefreshStyle();
			changes.store(false);
		}

		AddSubCategoryHeader(_T("THEME_PRESET_SELECT"));

		constexpr auto combo_flags = ImGuiComboFlags_HeightLarge;
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - p_padding - ImGui::GetStyle().IndentSpacing);
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

		if (ImGui::Button(_T("THEME_PRESET_CREATE"), ImVec2(ImGui::GetContentRegionAvail().x - p_padding - ImGui::GetStyle().IndentSpacing, ImGui::GetFontSize() * 1.5f))) {
			ImGui::OpenPopup("new_theme");
		}

		if (ImGui::BeginPopup("new_theme")) {
			constexpr auto input_flags = ImGuiInputTextFlags_EnterReturnsTrue;
			char newTheme[128] = { 0 };
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
			AddSubCategoryHeader(_T("Note"));

			float cx = ImGui::GetCenterTextPosX(_T("THEME_NOTE_1"));
			ImGui::SetCursorPosX(cx);
			ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.2f, 1.0f), _T("THEME_NOTE_1"));
			ImGui::NewLine();

			cx = ImGui::GetCenterTextPosX(_T("THEME_NOTE_2"));
			ImGui::SetCursorPosX(cx);
			ImGui::Text(_T("THEME_NOTE_2"));
			ImGui::NewLine();

			cx = ImGui::GetCenterTextPosX(_T("THEME_NOTE_3"));
			ImGui::SetCursorPosX(cx);
			ImGui::Text(_T("THEME_NOTE_3"));
			ImGui::NewLine();
			return;
		}

		// ImGui::SeparatorText(_TFM("THEME_CHANGES", +":"));
		ImGui::Spacing();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		ImGui::Spacing();

		if (ImGui::Button(_T("THEME_RESET"), ImVec2(ImGui::GetContentRegionAvail().x - p_padding - ImGui::GetStyle().IndentSpacing, ImGui::GetFontSize() * 1.5f))) {
			std::string _default = "Default";
			Settings::SetThemeFromIni(_default);
			changes.store(true);
		}

		if (ImGui::Button(_T("THEME_EXPORT"), ImVec2(ImGui::GetContentRegionAvail().x - p_padding - ImGui::GetStyle().IndentSpacing, ImGui::GetFontSize() * 1.5f))) {
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

		if (ImGui::Button(_T("THEME_POPUP"), ImVec2(ImGui::GetContentRegionAvail().x - p_padding - ImGui::GetStyle().IndentSpacing, ImGui::GetFontSize() * 1.5f))) {
			Menu::GetSingleton()->showSettingWindow = !Menu::GetSingleton()->showSettingWindow;
		}

		if (config.theme == "Default") {
			return;
		};

		AddSubCategoryHeader(_T("THEME_WINDOW_STYLE"));

		AddColorPicker("THEME_WINDOW_COLOR", style.windowBg);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddDualSlider("THEME_WINDOW_PADDING", style.windowPadding.x, style.windowPadding.y, 0.1f, 25.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_BORDER_COLOR", style.border);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddSliderPicker("THEME_WINDOW_ROUNDING", style.windowRounding, 0.1f, 15.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddSliderPicker("THEME_WINDOW_BORDER_SIZE", style.windowBorderSize, 0.1f, 15.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddSliderPicker("THEME_SIDEBAR_SPACING", style.sidebarSpacing, 0.1f, 100.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_HEADER_COLOR", style.header);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_HEADER_HOVER_COLOR", style.headerHovered);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_HEADER_ACTIVE_COLOR", style.headerActive);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddDualSlider("THEME_ITEM_SPACING", style.itemSpacing.x, style.itemSpacing.y, 0.1f, 20.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddDualSlider("THEME_ITEM_INNER_SPACING", style.itemInnerSpacing.x, style.itemInnerSpacing.y, 0.1f, 20.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddSliderPicker("THEME_INDENT_SPACING", style.indentSpacing, 0.1f, 35.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddImageDropdown("THEME_SPLASH_IMAGE", &style.splashImage);

		AddSubCategoryHeader(_T("THEME_TEXT_FONTS"));

		AddColorPicker("THEME_TEXT_COLOR", style.text);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_TEXT_DISABLED_COLOR", style.textDisabled);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_TEXT_SELECTED_BG_COLOR", style.textSelectedBg);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddToggleButton("THEME_DISABLE_ICON_TEXT", style.noIconText);

		AddSubCategoryHeader(_T("THEME_TABLE_COLUMN_STYLE"));

		AddDualSlider("THEME_CELL_PADDING", style.cellPadding.x, style.cellPadding.y, 0.1f, 20.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddSliderPicker("THEME_COLUMNS_MIN_SPACING", style.columnsMinSpacing, 0.1f, 20.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_TABLE_HEADER_BG_COLOR", style.tableHeaderBg);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_TABLE_BORDER_STRONG_COLOR", style.tableBorderStrong);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_TABLE_BORDER_LIGHT_COLOR", style.tableBorderLight);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_TABLE_ROW_BG_COLOR", style.tableRowBg);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddToggleButton("THEME_TABLE_ROW_ALTERNATING_BG", style.showTableRowBG);

		AddSubCategoryHeader(_T("THEME_BUTTONS_WIDGET_STYLE"));

		AddColorPicker("THEME_WIDGET_BG_COLOR", style.widgetBg);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_WIDGET_HOVER_COLOR", style.widgetHovered);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_WIDGET_ACTIVE_COLOR", style.widgetActive);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddSliderPicker("THEME_WIDGET_ROUNDING", style.widgetRounding, 0.0f, 15.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddSliderPicker("THEME_WIDGET_BORDER_SIZE", style.widgetBorderSize, 0.0f, 15.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddDualSlider("THEME_WIDGET_PADDING", style.widgetPadding.x, style.widgetPadding.y, 0.0f, 25.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_CHECKMARK_COLOR", style.checkMark);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_BUTTON_COLOR", style.button);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_BUTTON_HOVER_COLOR", style.buttonHovered);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_BUTTON_ACTIVE_COLOR", style.buttonActive);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_SECONDARY_BUTTON_COLOR", style.secondaryButton);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_SECONDARY_BUTTON_HOVER_COLOR", style.secondaryButtonHovered);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_SECONDARY_BUTTON_ACTIVE_COLOR", style.secondaryButtonActive);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_SCROLLBAR_BG_COLOR", style.scrollbarBg);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_SCROLLBAR_GRAB_COLOR", style.scrollbarGrab);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_SCROLLBAR_GRAB_HOVER_COLOR", style.scrollbarGrabHovered);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_SCROLLBAR_GRAB_ACTIVE_COLOR", style.scrollbarGrabActive);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddSliderPicker("THEME_SCROLLBAR_ROUNDING", style.scrollbarRounding, 0.1f, 35.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddSliderPicker("THEME_SCROLLBAR_SIZE", style.scrollbarSize, 0.1f, 50.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddSliderPicker("THEME_GRABBER_MIN_SIZE", style.grabMinSize, 0.1f, 20.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddSliderPicker("THEME_GRABBER_ROUNDING", style.grabRounding, 0.1f, 20.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_SLIDER_GRAB_COLOR", style.sliderGrab);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_SLIDER_GRAB_ACTIVE_COLOR", style.sliderGrabActive);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		AddColorPicker("THEME_SEPARATOR_COLOR", style.separator);

		ImGui::NewLine();
	}

	void SettingsWindow::Draw()
	{
		constexpr auto child_flags = ImGuiChildFlags_Border;
		constexpr auto header_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed;
		if (ImGui::BeginChild("##SettingsTable", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 30.0f), child_flags)) {
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 15.0f));
			if (ImGui::CollapsingHeader(_T("THEME_GENERAL"), header_flags)) {
				ImGui::PopStyleVar();

				ImGui::Indent();
				DrawGeneralSettings();
				ImGui::Unindent();
			} else {
				ImGui::PopStyleVar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 15.0f));
			if (ImGui::CollapsingHeader(_T("THEME_THEME"), header_flags)) {
				ImGui::PopStyleVar();

				ImGui::Indent();
				DrawThemeSelector();
				ImGui::Unindent();
			} else {
				ImGui::PopStyleVar();
			}
		}

		ImGui::EndChild();

		auto& config = Settings::GetSingleton()->GetConfig();
		auto& style = Settings::GetSingleton()->GetStyle();

		if (SettingsWindow::file_changes.load()) {
			const float alpha = 1.0f;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.7f, 0.2f, alpha));
			if (ImGui::Button(_T("THEME_SAVE_CHANGES"), ImVec2(ImGui::GetContentRegionAvail().x - 20.0f, 0))) {
				SaveThemeToFile(config.theme, style);
				SettingsWindow::file_changes.store(false);
			}
			ImGui::PopStyleColor(1);
		}
	}

	// Called from XSEPlugin -> Frame::Install() -> Modules::Init()
	// This is important, because settings *must* be loaded before.
	void SettingsWindow::Init()
	{
		_uiScale = Settings::GetSingleton()->GetConfig().uiScale;
		_fontSize = Settings::GetSingleton()->GetConfig().globalFontSize;
	}
}
