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

		ImGui::SeparatorText("Behavior:");

		ImGui::PushFont(a_style.font.medium);
		ImGui::PushStyleColor(ImGuiCol_Header, a_style.button);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, a_style.buttonActive);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, a_style.buttonHovered);

		// Add To Inventory Toggle.
		if (ImGui::Selectable(ICON_RPG_HAND " Add to Inventory", &b_AddToInventory, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_PlaceOnGround = false;
			b_AddToFavorites = false;
		};
		ImGui::SetDelayedTooltip("Quickly add item(s) to your inventory by left clicking.");

		// Place On Ground Toggle.
		if (ImGui::Selectable(ICON_RPG_GRASS " Place on Ground", &b_PlaceOnGround, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_AddToInventory = false;
			b_AddToFavorites = false;
		}
		ImGui::SetDelayedTooltip("Place item(s) on the ground by left clicking.");

		// Add To Favorites Toggle.
		if (ImGui::Selectable(ICON_RPG_HEART " Add to Favorites", &b_AddToFavorites, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_AddToInventory = false;
			b_PlaceOnGround = false;
		}
		ImGui::SetDelayedTooltip("Quickly favorite items by left clicking.");

		ImGui::PopStyleColor(3);
		ImGui::SeparatorText("Shortcuts:");
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(a_style.button.x, a_style.button.y + 0.3f, a_style.button.y, a_style.button.w));

		// Add All From Table Shortcut.
		if (ImGui::Button("Add All From Table", ImVec2(button_width, button_height))) {
			for (auto& item : itemList) {
				Console::AddItem(item->GetFormID());
			}

			Console::StartProcessThread();
		}
		ImGui::SetDelayedTooltip(
			"Add all items from the table to your inventory.\n\n"
			"Warning: This may add a lot of items!");

		// Place All from Table Shortcut.
		if (ImGui::Button("Place All From Table", ImVec2(button_width, button_height))) {
			if (itemList.size() > 30) {
				ImGui::OpenPopup("Warning: Large Query Detected");
			} else {
				for (auto& item : itemList) {
					Console::PlaceAtMe(item->GetFormID());
				}
			}

			Console::StartProcessThread();
		}
		ImGui::SetDelayedTooltip(
			"Place all items from the table on the ground.\n\n"
			"Warning: This may place a lot of items!\n"
			"Use at your own risk.");

		ImGui::PopStyleColor(1);

		ImGui::ShowWarningPopup("Warning: Large Query Detected", [&]() {
			for (auto& item : itemList) {
				Console::PlaceAtMe(item->GetFormID());
			}
		});

		// Show Favorites
		if (ImGui::Button("Goto Favorite", ImVec2(button_width, button_height))) {
			selectedMod = "Favorite";
			ApplyFilters();
		}

		ImGui::PopFont();
		ImGui::PopStyleVar(2);

		ImGui::SeparatorText("Preview:");
		ShowItemPreview<Item>(itemPreview);

		ImGui::EndChild();
	}
}