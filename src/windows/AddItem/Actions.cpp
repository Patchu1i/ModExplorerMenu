#include "AddItem.h"
#include "Console.h"
#include "Utils/Util.h"
#include "Windows/ItemPreview.h"

namespace ModExplorerMenu
{
	void AddItemWindow::ShowActions(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_config;

		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

		ImGui::BeginChild("##AddItemWindow::Actions", ImVec2(ImGui::GetContentRegionAvail()), ImGuiChildFlags_AlwaysUseWindowPadding);

		const float button_height = ImGui::GetFontSize() * 1.5f;
		const float button_width = ImGui::GetContentRegionAvail().x;

		// ImGui::SeparatorText((std::string(_T("Behavior")) + ":").c_str());
		ImGui::SeparatorText(_TFM("Behavior", ":"));

		ImGui::PushFont(a_style.font.medium);
		ImGui::PushStyleColor(ImGuiCol_Header, a_style.button);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, a_style.buttonActive);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, a_style.buttonHovered);

		// Add To Inventory Toggle.
		if (ImGui::Selectable(_TICON(ICON_RPG_HAND, "AIM_ADD"), &b_AddToInventory, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_PlaceOnGround = false;
			b_AddToFavorites = false;
		};
		ImGui::SetDelayedTooltip(_T("AIM_ADD_HELP"));

		// Place On Ground Toggle.
		if (ImGui::Selectable(_TICON(ICON_RPG_GRASS, "AIM_PLACE"), &b_PlaceOnGround, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_AddToInventory = false;
			b_AddToFavorites = false;
		}
		ImGui::SetDelayedTooltip(_T("AIM_PLACE_HELP"));

		// Add To Favorites Toggle.
		if (ImGui::Selectable(_TICON(ICON_RPG_HEART, "AIM_FAVORITE"), &b_AddToFavorites, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_AddToInventory = false;
			b_PlaceOnGround = false;
		}
		ImGui::SetDelayedTooltip(_T("AIM_FAVORITE_HELP"));

		ImGui::PopStyleColor(3);
		ImGui::SeparatorText(_TFM("Shortcuts", ":"));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(a_style.button.x, a_style.button.y + 0.3f, a_style.button.y, a_style.button.w));

		// Add All From Table Shortcut.
		if (ImGui::Button(_T("AIM_ADD_ALL"), ImVec2(button_width, button_height))) {
			for (auto& item : itemList) {
				Console::AddItem(item->GetFormID());
			}

			Console::StartProcessThread();
		}
		ImGui::SetDelayedTooltip(_T("AIM_ADD_ALL_HELP"));

		// Place All from Table Shortcut.
		if (ImGui::Button(_T("GENERAL_PLACE_ALL"), ImVec2(button_width, button_height))) {
			if (itemList.size() > 30) {
				ImGui::OpenPopup(_T("AIM_LARGE_QUERY"));
			} else {
				for (auto& item : itemList) {
					Console::PlaceAtMe(item->GetFormID());
				}
			}

			Console::StartProcessThread();
		}
		ImGui::SetDelayedTooltip(_T("AIM_PLACE_ALL_HELP"));

		ImGui::PopStyleColor(1);

		ImGui::ShowWarningPopup(_T("AIM_LARGE_QUERY"), [&]() {
			for (auto& item : itemList) {
				Console::PlaceAtMe(item->GetFormID());
			}
		});

		// Show Favorites
		if (ImGui::Button(_T("GENERAL_GOTO_FAVORITE"), ImVec2(button_width, button_height))) {
			selectedMod = "Favorite";
			ApplyFilters();
		}

		ImGui::PopFont();
		ImGui::PopStyleVar(2);

		ImGui::SeparatorText(_TFM("Preview", ":"));
		ShowItemPreview<Item>(itemPreview);

		ImGui::EndChild();
	}
}