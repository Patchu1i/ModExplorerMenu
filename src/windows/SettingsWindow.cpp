#include "SettingsWindow.h"
#include "Menu.h"
#include "Settings.h"
#include <codecvt>

// TODO: Move this to ImGui namespace?
void AddColorPicker(const char* a_text, ImVec4& a_colRef)
{
	constexpr ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar;
	auto id = "##ColorPicker" + std::string(a_text);
	auto width = 50.0f;
	ImGui::Text(a_text);
	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - width - 10.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(-10.0f, 0.0f));
	if (ImGui::ColorEdit4(id.c_str(), (float*)&a_colRef, flags)) {
		SettingsWindow::changes.store(true);
		SettingsWindow::file_changes.store(true);
	}
	ImGui::PopStyleVar(1);
}

// TODO: Move this to ImGui namespace?
void AddSliderPicker(const char* a_text, float& a_valRef, float a_min, float a_max)
{
	auto id = "##SliderPicker" + std::string(a_text);
	auto width = 100.0f;
	ImGui::Text(a_text);
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - width - 10.0f);
	ImGui::SetNextItemWidth(width);
	if (ImGui::SliderFloat(id.c_str(), &a_valRef, a_min, a_max)) {
		SettingsWindow::changes.store(true);
		SettingsWindow::file_changes.store(true);
	}
}

// TODO: Move this to ImGui namespace?
void AddCheckbox(const char* a_text, bool& a_boolRef)
{
	auto id = std::string(a_text) + "##Checkbox";
	ImGui::Text(a_text);
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcItemWidth() - 5.0f);
	if (ImGui::Checkbox(id.c_str(), &a_boolRef)) {
		SettingsWindow::changes.store(true);
		SettingsWindow::file_changes.store(true);
	}
}

void AddImageDropdown(const char* a_text, GraphicManager::Image* a_imageRef)
{
	auto id = "##ImageDropdown" + std::string(a_text);
	auto width = 200.0f;
	auto imageName = GraphicManager::GetImageName(*a_imageRef);
	ImGui::Text(a_text);
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - width - 10.0f);
	if (ImGui::BeginCombo(id.c_str(), imageName.c_str(), 0)) {
		auto images = Settings::GetListOfImages();
		ImGui::PushID("##ImageSelectionPopup");
		for (const auto& image : images) {
			// if (image.first == "") {
			// 	continue;  // FIXME: Why is image_library generating an empty image at index 0?
			// }

			if (ImGui::Selectable(image.first.c_str())) {
				*a_imageRef = image.second;
				SettingsWindow::file_changes.store(true);
			}
		}
		ImGui::PopID();
		ImGui::EndCombo();
	}
}

void AddFontDropdown(const char* a_text, ImFont** a_fontRef)
{
	auto id = "##FontDropdown" + std::string(a_text);
	auto width = 200.0f;
	auto fontName = GraphicManager::GetFontName(*a_fontRef);
	ImGui::Text(a_text);
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - width - 10.0f);
	if (ImGui::BeginCombo(id.c_str(), fontName.c_str(), 0)) {
		auto fonts = Settings::GetListOfFonts();
		ImGui::PushID("##FontSelectionPopup");
		for (const auto& font : fonts) {
			if (font.first == "") {
				continue;  // FIXME: Why is font_library generating an empty font at index 0?
			}

			if (ImGui::Selectable(font.first.c_str())) {
				*a_fontRef = font.second;
				SettingsWindow::file_changes.store(true);
			}
		}
		ImGui::PopID();
		ImGui::EndCombo();
	}
}

// TODO: Move this to ImGui namespace?
void SettingsWindow::DrawPopped()
{
	constexpr auto window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;
	if (ImGui::Begin("##SettingsPopupWindow", nullptr, window_flags)) {
		DrawThemeSelector();
	}

	ImGui::End();
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

void SettingsWindow::DrawThemeSelector()
{
	Settings::Style& style = Settings::GetSingleton()->GetStyle();
	Settings::Config& config = Settings::GetSingleton()->GetConfig();

	ImGui::SeparatorText("Select a theme from a list of presets:");

	ImGui::HelpMarker("Choose a theme from the dropdown box below.\nThemes are a collection of colors and styles that can be applied to the menu.\n\nYou can start with a theme, and modify it further below.\n\n(WARNING): This will overwrite any modified values below.");
	constexpr auto combo_flags = ImGuiComboFlags_HeightLarge;

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

	ImGui::SeparatorText("Actions:");

	ImGui::HelpMarker("Resets the current theme/style configuration to defaults\nThis will not write changes to currently selected theme.\nThis will also not impact settings outside of this category.");
	if (ImGui::Button("Reset to Default")) {
		style = Settings::Style();  // FIXME: Revisit this
	}

	ImGui::HelpMarker("Export your current theme configuration to a .ini file\n(Warning): This will overwrite any existing file with the same name!");
	if (ImGui::Button("Export to File")) {
		ImGui::OpenPopup("save_to_file");
	}

	ImGui::HelpMarker("Save changes made to the current theme configuration\nThis will overwrite the current theme ini with the new settings.");
	if (SettingsWindow::file_changes.load()) {
		if (ImGui::Button("Save Changes")) {
			SaveThemeToFile(config.theme, style);
			SettingsWindow::file_changes.store(false);
		}
	}

	if (ImGui::Button("Reload Theme")) {
		Settings::GetSingleton()->SetThemeFromIni(config.theme);
		changes.store(true);
	}

	if (ImGui::Button("Refresh Theme")) {
		Menu::GetSingleton()->RefreshStyle();
	}

	if (ImGui::Button("Sync Theme")) {
		Menu::GetSingleton()->SyncUserStyleToImGui(style);
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

	ImGui::HelpMarker("Pop this window out so that you can navigate the explorer\nand change settings on the fly. Useful for when\nyou want to see changes made.");
	if (ImGui::Button("Pop Window")) {
		Menu::is_settings_popped = !Menu::is_settings_popped;
	}

	constexpr auto color_flags = ImGuiTreeNodeFlags_Framed;
	if (ImGui::CollapsingHeader("Fonts & Images")) {
		ImGui::SeparatorText("Fonts");
		ImGui::Indent();
		AddFontDropdown("Text Font:", &style.textFont);
		AddFontDropdown("Sidebar Font:", &style.sidebarFont);
		AddFontDropdown("Button Font:", &style.buttonFont);
		AddFontDropdown("Column Header Font:", &style.headerFont);
		AddImageDropdown("Splash Image", &style.splashImage);
		AddImageDropdown("Favorite Icon (Enabled)", &style.favoriteIconEnabled);
		AddImageDropdown("Favorite Icon (Disabled)", &style.favoriteIconDisabled);
		ImGui::Unindent();
	}
	if (ImGui::CollapsingHeader("Colors & Styles", color_flags)) {
		ImGui::SeparatorText("Window");
		ImGui::Indent();
		AddColorPicker("Window Color", style.windowBg);
		// AddSliderPicker("Alpha", style.alpha, 0.1f, 1.0f);
		AddColorPicker("Border Color", style.border);
		AddColorPicker("Border Shadow Color", style.borderShadow);
		// AddSliderPicker("Window Padding X", style.windowPadding.x, 0.1f, 20.0f);
		// AddSliderPicker("Window Padding Y", style.windowPadding.y, 0.1f, 20.0f);
		// AddSliderPicker("Window Rounding", style.windowRounding, 0.1f, 20.0f);
		// AddSliderPicker("Window Border Size", style.windowBorderSize, 0.1f, 20.0f);
		// AddSliderPicker("Item Spacing X", style.itemSpacing.x, 0.1f, 20.0f);
		// AddSliderPicker("Item Spacing Y", style.itemSpacing.y, 0.1f, 20.0f);
		// AddSliderPicker("Item Inner Spacing X", style.itemInnerSpacing.x, 0.1f, 20.0f);
		// AddSliderPicker("Item Inner Spacing Y", style.itemInnerSpacing.y, 0.1f, 20.0f);
		// AddSliderPicker("Indent Spacing", style.indentSpacing, 0.1f, 20.0f);
		ImGui::Unindent();

		ImGui::NewLine();

		ImGui::SeparatorText("Frame");
		ImGui::Indent();
		AddColorPicker("Frame BG Color", style.frameBg);
		AddColorPicker("Frame BG Hover Color", style.frameBgHovered);
		AddColorPicker("Frame BG Active Color", style.frameBgActive);
		// AddSliderPicker("Frame Rounding", style.frameRounding, 0.1f, 20.0f);
		// AddSliderPicker("Frame Border Size", style.frameBorderSize, 0.1f, 20.0f);
		// AddSliderPicker("Frame Padding X", style.framePadding.x, 0.1f, 20.0f);
		// AddSliderPicker("Frame Padding Y", style.framePadding.y, 0.1f, 20.0f);
		ImGui::Unindent();

		ImGui::NewLine();

		ImGui::SeparatorText("Child Window & Popup Window");
		ImGui::Indent();
		AddColorPicker("Child Window Color", style.childBg);
		// AddSliderPicker("Child Border Size", style.childBorderSize, 0.1f, 20.0f);
		// AddSliderPicker("Child Rounding", style.childRounding, 0.1f, 20.0f);
		AddColorPicker("Popup BG Color", style.popupBg);
		// AddSliderPicker("Popup Border Size", style.popupBorderSize, 0.1f, 20.0f);
		// AddSliderPicker("Popup Rounding", style.popupRounding, 0.1f, 20.0f);
		ImGui::Unindent();

		ImGui::NewLine();

		ImGui::SeparatorText("Text");
		ImGui::Indent();
		ImGui::Text("Font:");
		AddColorPicker("Text Color", style.text);
		AddColorPicker("Text Disabled Color", style.textDisabled);
		//AddColorPicker("Title BG Color", style.titleBg);
		//AddColorPicker("Title BG Collapsed Color", style.titleBgCollapsed);
		//AddColorPicker("Title BG Active Color", style.titleBgActive);
		AddColorPicker("Text Selected BG Color", style.textSelectedBg);
		ImGui::Unindent();

		ImGui::NewLine();

		ImGui::SeparatorText("Table & Columns");
		ImGui::Indent();
		AddColorPicker("Header Color", style.header);
		AddColorPicker("Header Hovered Color", style.headerHovered);
		AddColorPicker("Header Active Color", style.headerActive);
		// AddSliderPicker("Cell Padding X", style.cellPadding.x, 0.1f, 20.0f);
		// AddSliderPicker("Cell Padding Y", style.cellPadding.y, 0.1f, 20.0f);
		// AddSliderPicker("Columns Min Spacing", style.columnsMinSpacing, 0.1f, 20.0f);
		AddColorPicker("Table Header BG Color", style.tableHeaderBg);
		AddColorPicker("Table Border Strong Color", style.tableBorderStrong);
		AddColorPicker("Table Border Light Color", style.tableBorderLight);
		AddColorPicker("Table Row BG Color", style.tableRowBg);
		ImGui::Unindent();

		ImGui::NewLine();

		ImGui::SeparatorText("Buttons & Widgets");
		ImGui::Indent();
		AddColorPicker("Checkmark Color", style.checkMark);
		AddColorPicker("Button Color", style.button);
		AddColorPicker("Button Hovered Color", style.buttonHovered);
		AddColorPicker("Button Active Color", style.buttonActive);
		AddColorPicker("Scrollbar BG Color", style.scrollbarBg);
		AddColorPicker("Scrollbar Grab Color", style.scrollbarGrab);
		AddColorPicker("Scrollbar Grab Hovered Color", style.scrollbarGrabHovered);
		AddColorPicker("Scrollbar Grab Active Color", style.scrollbarGrabActive);
		// AddSliderPicker("Scrollbar Rounding", style.scrollbarRounding, 0.1f, 20.0f);
		// AddSliderPicker("Scrollbar Size", style.scrollbarSize, 0.1f, 20.0f);
		// AddSliderPicker("Grabber Min Size", style.grabMinSize, 0.1f, 20.0f);
		// AddSliderPicker("Grabber Rounding", style.grabRounding, 0.1f, 20.0f);
		AddColorPicker("Slider Grab Color", style.sliderGrab);
		AddColorPicker("Slider Grab Active Color", style.sliderGrabActive);
		AddColorPicker("Separator Color", style.separator);
		AddColorPicker("Separator Hovered Color", style.separatorHovered);
		AddColorPicker("Separator Active Color", style.separatorActive);
		ImGui::Unindent();

		ImGui::SeparatorText("To Remove:");
		ImGui::Indent();
		// AddSliderPicker("Disabled Alpha", style.disabledAlpha, 0.1f, 1.0f);     // unused?
		AddColorPicker("MenuBar BG Color", style.menuBarBg);                   // unused?
		AddColorPicker("Resize Grip Color", style.resizeGrip);                 // unused
		AddColorPicker("Resize Grip Hovered Color", style.resizeGripHovered);  // unused
		AddColorPicker("Resize Grip Active Color", style.resizeGripActive);    // unused
		// AddColorPicker("Nav Highlight Color", style.navHighlight);              // unused
		// AddColorPicker("Nav Windowing Dim BG Color", style.navWindowingDimBg);  // unused
		// AddColorPicker("Modal Window Dim BG Color", style.modalWindowDimBg);    // unused
		// AddSliderPicker("Tab Rounding", style.tabRounding, 0.1f, 20.0f);        // unused
		// AddSliderPicker("Tab Border Size", style.tabBorderSize, 0.1f, 20.0f);   // unused
		ImGui::Unindent();
	}
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

	if (changes.load()) {
		Menu::GetSingleton()->RefreshStyle();
		changes.store(false);
	}
}

void SettingsWindow::Init()
{
	// Open = true;
}
