#include "Console.h"
#include "Utils/Util.h"
#include "Window.h"

namespace ModExplorerMenu
{
	void AddItemWindow::ShowActions(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_config;

		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

		ImGui::BeginChild("##AddItemWindow::Actions", ImVec2(ImGui::GetContentRegionAvail()), ImGuiChildFlags_AlwaysUseWindowPadding);
		ImGui::SeparatorText("Selection:");

		const float button_height = ImGui::GetFontSize() * 1.5f;
		const float button_width = ImGui::GetContentRegionAvail().x;

		if (ImGui::BeginTable("##AddItemWindow::ActionBarSelection", 1, ActionBarFlags, ImVec2(ImGui::GetContentRegionAvail().x, 150.0f))) {
			ImGui::PushFont(a_style.font.medium);
			ImGui::TableSetupColumn("Item(s)", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::PopFont();

			for (auto& item : itemList) {
				if (item->selected) {
					if (ImGui::Selectable(item->GetName().data(), false, ImGuiSelectableFlags_SpanAllColumns)) {
						item->selected = false;
					};
					ImGui::SetDelayedTooltip("Click to remove.");
				}
			}
			ImGui::EndTable();
		}

		ImGui::PushFont(a_style.font.medium);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.5f, a_style.button.w));
		if (ImGui::Button("Clear Selection", ImVec2(button_width, button_height))) {
			for (auto& item : itemList) {
				item->selected = false;
			}
		}
		ImGui::PopStyleColor(1);
		ImGui::PopFont();  // Button font

		ImGui::SeparatorText("Selection:");

		ImGui::PushFont(a_style.font.medium);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 1.0f, 0.5f, a_style.button.w));

		ImGui::InlineCheckbox("Sticky Select", &b_StickySelect);
		ImGui::SetDelayedTooltip(
			"Enabling this will keep your selected items select when you interact with\n"
			"actions shown in this sidebar. If this is disabled, items will be cleared once\n"
			"they are acted upon");
		ImGui::NewLine();

		if (ImGui::Button("Add to Inventory", ImVec2(button_width, button_height))) {
			for (auto& item : itemList) {
				if (item->selected) {
					Console::AddItem(item->GetFormID());
					item->selected = b_StickySelect;
					logger::info("Added item: {}", item->GetFormID());
				}
			}
		}
		ImGui::SetDelayedTooltip(
			"Directly add the selected item(s) above to your inventory.\n"
			"You can optionally enable \"Click To Add\" or \"Click To Place\" to quickly\n"
			"add or place items simply by clicking them in the left pane.");

		if (ImGui::Button("Place At Me", ImVec2(button_width, button_height))) {
			for (auto& item : itemList) {
				if (item->selected) {
					Console::PlaceAtMe(item->GetFormID());
					item->selected = b_StickySelect;
				}
			}
		}
		ImGui::SetDelayedTooltip(
			"Directly place the selected item(s) above in the world.\n"
			"You can optionally enable \"Click To Add\" or \"Click To Place\" to quickly\n"
			"add or place items simply by clicking them in the left pane.");

		if (ImGui::Button("Add to Favorites", ImVec2(button_width, button_height))) {
			for (auto& item : itemList) {
				if (item->selected) {
					item->favorite = !item->favorite;
				}
			}
		}
		ImGui::SetDelayedTooltip(
			"Add the selected items above to your favorites.\n"
			"Clicking this repeatedly will toggle the favorite status of the selected items.");
		ImGui::PopStyleColor(1);

		if (ImGui::Button("Select All Favorites", ImVec2(button_width, button_height))) {
			for (auto& item : itemList) {
				if (item->favorite) {
					item->selected = true;
				} else {
					item->selected = false;
				}
			}
		}
		ImGui::SetDelayedTooltip(
			"Select all favorited items from the currently indexed search results.");

		if (ImGui::Button("Select All", ImVec2(button_width, button_height))) {
			for (auto& item : itemList) {
				item->selected = true;
			}
		}

		ImGui::SeparatorText("Shortcuts:");

		ImGui::SetDelayedTooltip(
			"Select all items from the currently indexed search results.");
		ImGui::InlineCheckbox("Click to Add", &b_ClickToAdd);

		ImGui::SetDelayedTooltip(
			"Enabling this will allow you to quickly add items to your inventory by left clicking.\n\n"
			"This will disable the ability to select items for other actions.");

		ImGui::NewLine();

		if (b_ClickToAdd) {
			ImGui::InputInt("##AddItemWindow::ClickToAddInput", &clickToAddCount);
			ImGui::SetDelayedTooltip(
				"Amount of items to add when clicking on an item.");
		}

		ImGui::InlineCheckbox("Click to Place", &b_ClickToPlace);

		ImGui::SetDelayedTooltip(
			"Enabling this will allow you to quickly place items in the world by left clicking.\n\n"
			"This will disable the ability to select items for other actions.");

		ImGui::NewLine();

		if (b_ClickToPlace) {
			ImGui::InputInt("##AddItemWindow::ClickToPlaceInput", &clickToPlaceCount);
			ImGui::SetDelayedTooltip(
				"Amount of items to place when clicking on an item.");
		}

		ImGui::InlineCheckbox("Click to Favorite", &b_ClickToFavorite);

		ImGui::SetDelayedTooltip(
			"Enabling this will allow you to quickly favorite items by left clicking.\n\n"
			"This will disable the ability to select items for other actions.");

		ImGui::PopFont();
		ImGui::PopStyleVar(2);

		ImGui::EndChild();
	}
}