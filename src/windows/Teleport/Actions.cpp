#include "include/C/Console.h"
#include "include/M/Menu.h"
#include "include/T/Teleport.h"

namespace Modex
{
	void TeleportWindow::ShowActions()
	{
		auto a_style = Settings::GetSingleton()->GetStyle();

		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

		const float button_height = ImGui::GetFontSize() * 1.5f;
		const float button_width = ImGui::GetContentRegionAvail().x;

		ImGui::SubCategoryHeader(_T("Behavior"), ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		if (ImGui::GradientSelectableEX(_TICON(ICON_RPG_SPAWNED_NPC, "TELE_CLICK_TO_TELEPORT"), b_ClickToTeleport, ImVec2(button_width, button_height))) {
			b_ClickToFavorite = false;
		}

		if (ImGui::GradientSelectableEX(_TICON(ICON_RPG_HEART, "GENERAL_CLICK_TO_FAVORITE"), b_ClickToFavorite, ImVec2(button_width, button_height))) {
			b_ClickToTeleport = false;
		}

		ImGui::PopStyleVar(2);

		ImGui::Spacing();
		ImGui::SubCategoryHeader(_T("Favorite"), ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

		auto constexpr flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration |
		                       ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;

		auto tempList = Data::GetSingleton()->GetTeleportList();
		if (ImGui::BeginChild("##TeleportFavoriteScroll", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 10), false, flags)) {
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			for (auto& cell : tempList) {
				if (cell.favorite) {
					if (ImGui::GradientButton(cell.GetEditorID().data(), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
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