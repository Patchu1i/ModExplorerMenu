#include "SettingsWindow.h"
#include "Menu.h"
#include "Settings.h"
#include "lib/Anim.h"
#include <codecvt>

//
// # Begin Helpers
//

void AddColorPicker(const char* a_text, ImVec4& a_colRef)
{
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
	constexpr ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar;
	auto id = "##ColorPicker" + std::string(a_text);
	auto width = ImGui::GetFontSize();
	ImGui::Text(a_text);
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

void AddSliderPicker(const char* a_text, float& a_valRef, float a_min, float a_max)
{
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
	auto id = "##SliderPicker" + std::string(a_text);
	auto width = 200.0f;
	ImGui::Text(a_text);
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
	ImGui::Text(a_text);
	ImGui::SameLine(ImGui::GetContentRegionMax().x - width - 10.0f);
	ImGui::SetNextItemWidth(width);
	if (ImGui::SliderFloat2(id.c_str(), new_size, a_min, a_max, "%.1f")) {
		a_valRef_a = new_size[0];
		a_valRef_b = new_size[1];
		SettingsWindow::changes.store(true);
		SettingsWindow::file_changes.store(true);
	}
}

void AddCheckbox(const char* a_text, bool& a_boolRef)
{
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
	auto id = std::string(a_text) + "##Checkbox";
	auto width = ImGui::GetFontSize();
	ImGui::Text(a_text);
	ImGui::SameLine(ImGui::GetContentRegionMax().x - width - 10.0f);
	if (ImGui::Checkbox(id.c_str(), &a_boolRef)) {
		SettingsWindow::changes.store(true);
		SettingsWindow::file_changes.store(true);
	}
}

void AddImageDropdown(const char* a_text, GraphicManager::Image* a_imageRef)
{
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
	auto id = "##ImageDropdown" + std::string(a_text);
	auto imageName = GraphicManager::GetImageName(*a_imageRef);
	auto width = 200.0f;
	constexpr auto flags = ImGuiComboFlags_None;
	ImGui::Text(a_text);
	ImGui::SameLine(ImGui::GetContentRegionMax().x - width - 10.0f);
	ImGui::PushItemWidth(width);
	if (ImGui::BeginCombo(id.c_str(), imageName.c_str(), flags)) {
		auto images = Settings::GetListOfImages();
		ImGui::PushID("##ImageSelectionPopup");
		for (const auto& image : images) {
			if (ImGui::Selectable(image.first.c_str())) {
				*a_imageRef = image.second;
				SettingsWindow::file_changes.store(true);
			}
		}
		ImGui::PopID();
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
}

void AddFontDropdown(const char* a_text, ImFont** a_fontRef)
{
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
	auto id = "##FontDropdown" + std::string(a_text);
	auto fontName = GraphicManager::GetFontName(*a_fontRef);
	auto width = 200.0f;
	constexpr auto flags = ImGuiComboFlags_None;
	ImGui::Text(a_text);
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
}

//
// # End Helpers
//

void SettingsWindow::DrawPopped()
{
	constexpr auto window_flags = ImGuiWindowFlags_NoCollapse;
	if (ImGui::Begin("Settings Popup Window - Click to Drag me!", nullptr, window_flags)) {
		DrawThemeSelector();

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
		if (ImGui::Button("- Close Popup Window -", ImVec2(ImGui::GetContentRegionAvail().x, 25.0f))) {
			Menu::is_settings_popped = false;
		}
		ImGui::PopStyleColor(1);
	}

	ImGui::End();
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

	if (ImGui::CollapsingHeader("Theme Preset", color_flags | ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::SeparatorText("Select a theme from a list of presets:");

		constexpr auto combo_flags = ImGuiComboFlags_HeightLarge;
		ImGui::HelpMarker("Choose a theme from the dropdown box below.\nThemes are a collection of colors and styles that can be applied to the menu.\n\nYou can start with a theme, and modify it further below.\n\n(WARNING): This will overwrite any modified values below.");
		if (ImGui::BeginCombo("##Settings::PresetDropdown", config.theme.c_str(), combo_flags)) {
			std::vector<std::string> themes = Settings::GetListOfThemes();

			for (const auto& theme : themes) {
				if (ImGui::Selectable(theme.c_str())) {
					config.theme = theme;
					Settings::GetSingleton()->SetThemeFromIni(config.theme);
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
		if (ImGui::Button("Create New Theme")) {
			ImGui::OpenPopup("new_theme");
		}
		ImGui::PopStyleColor(1);

		if (ImGui::BeginPopup("new_theme")) {
			constexpr auto input_flags = ImGuiInputTextFlags_EnterReturnsTrue;
			char newTheme[128] = { 0 };
			ImGui::HelpMarker("Create a new theme by entering a name\nThis will create a new .ini file with the name provided.\n\nYou can not use the name 'Default'");
			if (ImGui::InputTextWithHint("##InputThemeName", "Enter Theme Name..", newTheme, IM_ARRAYSIZE(newTheme), input_flags)) {
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
			ImGui::SeparatorText("Note:");

			ImGui::Text("You are currently using the Default theme, which cannot be modified.");
			ImGui::NewLine();
			ImGui::Text("Please select a different theme from the dropdown above.");
			ImGui::NewLine();
			ImGui::Text("Or create a new theme by pressing 'Create New Theme'");
			return;
		}

		ImGui::SeparatorText("Save, Reset, or Export changes:");

		ImGui::HelpMarker("Resets the current theme/style configuration to defaults\nThis will not write changes to currently selected theme.\nThis will also not impact settings outside of this category.");
		if (ImGui::Button("Reset to Default", ImVec2(ImGui::GetContentRegionAvail().x - 20.0f, ImGui::GetFontSize() * 1.5f))) {
			std::string _default = "Default";
			Settings::SetThemeFromIni(_default);
			changes.store(true);
		}

		ImGui::HelpMarker("Export your current theme configuration to a .ini file\n(Warning): This will overwrite any existing file with the same name!");
		if (ImGui::Button("Export to File", ImVec2(ImGui::GetContentRegionAvail().x - 20.0f, ImGui::GetFontSize() * 1.5f))) {
			ImGui::OpenPopup("save_to_file");
		}

		if (SettingsWindow::file_changes.load()) {
			float alpha = ImGuiAnim::pulse(3.0f, 0.7f, 1.0f);
			ImGui::HelpMarker("Save changes made to the current theme configuration\nThis will overwrite the current theme ini with the new settings.");
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.7f, 0.2f, alpha));
			if (ImGui::Button("Save Changes", ImVec2(ImGui::GetContentRegionAvail().x - 20.0f, ImGui::GetFontSize() * 1.5f))) {
				SaveThemeToFile(config.theme, style);
				SettingsWindow::file_changes.store(false);
			}
			ImGui::PopStyleColor(1);
		}

		if (ImGui::BeginPopup("save_to_file")) {
			ImGui::Text("Save to .ini File");
			constexpr auto input_flags = ImGuiInputTextFlags_EnterReturnsTrue;
			if (ImGui::InputTextWithHint("##InputFilename", "Enter Filename..", savePath, IM_ARRAYSIZE(savePath), input_flags)) {
				SaveThemeToFile(savePath, style);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("Save")) {
				SaveThemeToFile(savePath, style);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::HelpMarker("Pop this window out so that you can navigate the explorer\nand change settings on the fly. Useful for when you want to see changes made.");
		if (ImGui::Button("Pop Window", ImVec2(ImGui::GetContentRegionAvail().x - 20.0f, ImGui::GetFontSize() * 1.5f))) {
			Menu::is_settings_popped = !Menu::is_settings_popped;
		}
	}

	if (config.theme == "Default") {
		return;
	};

	if (ImGui::CollapsingHeader("Window Style", color_flags)) {
		ImGui::Indent();
		AddColorPicker("Window Color:", style.windowBg);
		AddSliderPicker("Window Alpha:", style.alpha, 0.1f, 1.0f);
		AddDualSlider("Window Padding:", style.windowPadding.x, style.windowPadding.y, 0.1f, 25.0f);
		AddColorPicker("Border Color:", style.border);
		AddColorPicker("Border Shadow Color:", style.borderShadow);
		AddSliderPicker("Window Rounding:", style.windowRounding, 0.1f, 15.0f);
		AddSliderPicker("Window Border Size:", style.windowBorderSize, 0.1f, 15.0f);
		AddDualSlider("Item Spacing:", style.itemSpacing.x, style.itemSpacing.y, 0.1f, 20.0f);
		AddDualSlider("Item Inner Spacing:", style.itemInnerSpacing.x, style.itemInnerSpacing.y, 0.1f, 20.0f);
		AddSliderPicker("Indent Spacing:", style.indentSpacing, 0.1f, 35.0f);
		AddImageDropdown("Splash Image:", &style.splashImage);
		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader("Frame Style", color_flags)) {
		ImGui::Indent();
		AddColorPicker("Frame BG Color:", style.frameBg);
		AddColorPicker("Frame BG Hover Color:", style.frameBgHovered);
		AddColorPicker("Frame BG Active Color:", style.frameBgActive);
		AddSliderPicker("Frame Rounding:", style.frameRounding, 0.1f, 15.0f);
		AddSliderPicker("Frame Border Size:", style.frameBorderSize, 0.1f, 15.0f);
		AddDualSlider("Frame Padding:", style.framePadding.x, style.framePadding.y, 0.1f, 25.0f);
		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader("Child & Popup Style", color_flags)) {
		ImGui::Indent();
		AddColorPicker("Child Window Color:", style.childBg);
		AddSliderPicker("Child Border Size:", style.childBorderSize, 0.1f, 15.0f);
		AddSliderPicker("Child Rounding:", style.childRounding, 0.1f, 15.0f);
		AddColorPicker("Popup BG Color:", style.popupBg);
		AddSliderPicker("Popup Border Size:", style.popupBorderSize, 0.1f, 15.0f);
		AddSliderPicker("Popup Rounding:", style.popupRounding, 0.1f, 15.0f);
		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader("Text & Fonts", color_flags)) {
		ImGui::Indent();
		AddColorPicker("Text Color:", style.text);
		AddColorPicker("Text Disabled Color:", style.textDisabled);
		AddColorPicker("Text Selected BG Color:", style.textSelectedBg);
		AddFontDropdown("Regular Text Font", &style.textFont);
		AddFontDropdown("Column Header Font:", &style.headerFont);
		AddFontDropdown("Button Font:", &style.buttonFont);
		AddFontDropdown("Sidebar Button Font:", &style.sidebarFont);
		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader("Table & Column Style", color_flags)) {
		ImGui::Indent();
		AddColorPicker("Header Color", style.header);
		AddColorPicker("Header Hovered Color", style.headerHovered);
		AddColorPicker("Header Active Color", style.headerActive);
		AddDualSlider("Cell Padding:", style.cellPadding.x, style.cellPadding.y, 0.1f, 20.0f);
		AddSliderPicker("Columns Min Spacing", style.columnsMinSpacing, 0.1f, 20.0f);
		AddColorPicker("Table Header BG Color", style.tableHeaderBg);
		AddColorPicker("Table Border Strong Color", style.tableBorderStrong);
		AddColorPicker("Table Border Light Color", style.tableBorderLight);
		AddColorPicker("Table Row BG Color", style.tableRowBg);
		AddImageDropdown("Favorite Icon (Enabled)", &style.favoriteIconEnabled);
		AddImageDropdown("Favorite Icon (Disabled)", &style.favoriteIconDisabled);
		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader("Buttons & Widget Style", color_flags)) {
		ImGui::Indent();
		AddColorPicker("Checkmark Color", style.checkMark);
		AddColorPicker("Button Color", style.button);
		AddColorPicker("Button Hovered Color", style.buttonHovered);
		AddColorPicker("Button Active Color", style.buttonActive);
		AddColorPicker("Scrollbar BG Color", style.scrollbarBg);
		AddColorPicker("Scrollbar Grab Color", style.scrollbarGrab);
		AddColorPicker("Scrollbar Grab Hovered Color", style.scrollbarGrabHovered);
		AddColorPicker("Scrollbar Grab Active Color", style.scrollbarGrabActive);
		AddSliderPicker("Scrollbar Grab Rounding", style.scrollbarRounding, 0.1f, 35.0f);
		AddSliderPicker("Scrollbar Size", style.scrollbarSize, 0.1f, 50.0f);
		AddSliderPicker("Grabber Min Size", style.grabMinSize, 0.1f, 20.0f);
		AddSliderPicker("Grabber Rounding", style.grabRounding, 0.1f, 20.0f);
		AddColorPicker("Slider Grab Color", style.sliderGrab);
		AddColorPicker("Slider Grab Active Color", style.sliderGrabActive);
		AddColorPicker("Separator Color", style.separator);
		AddColorPicker("Separator Hovered Color", style.separatorHovered);
		AddColorPicker("Separator Active Color", style.separatorActive);
		ImGui::Unindent();
	}

	ImGui::NewLine();
}

void SettingsWindow::Draw()
{
	ImGui::Text("Welcome to the Settings Window!");

	constexpr auto child_flags = ImGuiChildFlags_Border;
	if (ImGui::BeginChild("##SettingsTable", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 30.0f), child_flags)) {
		if (ImGui::CollapsingHeader("Theme Configuration", ImGuiTreeNodeFlags_Framed)) {
			ImGui::Indent();
			DrawThemeSelector();
			ImGui::Unindent();
		}

		if (ImGui::CollapsingHeader("AddItem Configuration", ImGuiTreeNodeFlags_Framed)) {
			ImGui::Indent();
			ImGui::Text("AddItem Configuration");
			ImGui::Indent();
		}
	}
	ImGui::EndChild();
}

void SettingsWindow::Init()
{
	// Open = true;
}
