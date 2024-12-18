#include "Console.h"
#include "Teleport.h"

namespace ModExplorerMenu
{
	void TeleportWindow::ShowActions(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_config;

		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

		const float button_height = ImGui::GetFontSize() * 1.5f;
		const float button_width = ImGui::GetContentRegionAvail().x;

		ImGui::SeparatorText("Behavior:");

		ImGui::PushFont(a_style.font.medium);
		ImGui::PushStyleColor(ImGuiCol_Header, a_style.button);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, a_style.buttonActive);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, a_style.buttonHovered);

		if (ImGui::Selectable(ICON_RPG_SPAWNED_NPC " Click to Teleport", &b_ClickToTeleport, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_ClickToFavorite = false;
		}

		if (ImGui::Selectable(ICON_RPG_HEART " Click to Favorite", &b_ClickToFavorite, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_ClickToTeleport = false;
		}

		ImGui::PopStyleColor(3);
		ImGui::PopFont();
		ImGui::PopStyleVar(2);

		ImGui::SeparatorText("Favorite Locations:");
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

		const ImVec2 button_size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.5f);
		auto tempList = Data::GetCellMap();
		for (auto& cell : tempList) {
			if (cell.favorite) {
				if (ImGui::Button(cell.GetEditorID().data(), button_size)) {
					Console::Teleport(cell.GetEditorID().data());
					Console::StartProcessThread();
					break;
				}
			}
		}

		ImGui::PopStyleVar(1);
	}
}