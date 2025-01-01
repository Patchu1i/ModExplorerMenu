#include "Console.h"
#include "Menu.h"
#include "Teleport.h"

namespace Modex
{
	void TeleportWindow::ShowActions(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_config;

		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

		const float button_height = ImGui::GetFontSize() * 1.5f;
		const float button_width = ImGui::GetContentRegionAvail().x;

		ImGui::SeparatorText(_TFM("Behavior", ":"));

		ImGui::PushStyleColor(ImGuiCol_Header, a_style.button);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, a_style.buttonActive);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, a_style.buttonHovered);

		if (ImGui::m_Selectable(_TICON(ICON_RPG_SPAWNED_NPC, "TELE_CLICK_TO_TELEPORT"), b_ClickToTeleport, a_style, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_ClickToFavorite = false;
		}

		if (ImGui::m_Selectable(_TICON(ICON_RPG_HEART, "GENERAL_CLICK_TO_FAVORITE"), b_ClickToFavorite, a_style, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_ClickToTeleport = false;
		}

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(2);

		ImGui::SeparatorText(_TFM("Favorite", ":"));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

		auto constexpr flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration |
		                       ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;

		auto tempList = Data::GetCellMap();
		if (ImGui::BeginChild("##TeleportFavoriteScroll", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 10), false, flags)) {
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			for (auto& cell : tempList) {
				if (cell.favorite) {
					if (ImGui::m_Button(cell.GetEditorID().data(), a_style, ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
						Console::Teleport(cell.GetEditorID().data());
						Console::StartProcessThread();

						Menu::GetSingleton()->Close();
						break;
					}
				}
			}

			ImGui::PopStyleVar(1);
			ImGui::EndChild();
		}

		ImGui::PopStyleVar(1);
	}
}