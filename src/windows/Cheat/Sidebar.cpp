#include "Cheat.h"

namespace ModExplorerMenu
{
	void CheatWindow::ShowSidebar(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_style;
		(void)a_config;

		auto buttonSize = ImVec2(ImGui::GetContentRegionAvail().x, 20.0f);
		auto col = ImGui::GetStyleColorVec4(ImGuiCol_Button);

		ImGui::SeparatorText("Cheats:");
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
		for (auto& tab : tabs) {
			auto color = tab.IsEnabled() ? ImVec4(col.x, col.y, col.z, 1.0f) : ImVec4(col.x, col.y, col.z, 0.5f);
			ImGui::PushStyleColor(ImGuiCol_Button, color);
			if (ImGui::Button(tab.GetName().c_str(), buttonSize)) {
				EnableTab(tab);
			}
			ImGui::PopStyleColor();
		}
		ImGui::PopStyleVar();
	}
}