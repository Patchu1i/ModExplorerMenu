#include "SettingsWindow.h"
#include "Settings.h"

void SettingsWindow::Draw()
{
	ImGui::Text("Welcome to the Settings Window!");

	Settings::Style* user = Settings::GetStyle();

	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, (user->separatorThickness));

	constexpr auto color_flags = ImGuiTreeNodeFlags_Framed;
	if (ImGui::CollapsingHeader("Color Settings", color_flags)) {
		ImGui::Text("Background Color");
		//ImGui::ColorEdit4("##Background Color", &(user->frameBG), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		//ImGui::ColorPicker4("##Background Color", (float*)&(user->frameBG), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	}

	if (ImGui::CollapsingHeader("Style Settings", color_flags)) {
		ImGui::Text("Separator Thickness");
		ImGui::SliderFloat("##Separator Thickness", &(user->separatorThickness), 0.1f, 20.0f);
	}
}

void SettingsWindow::Init()
{
	// Open = true;
}
