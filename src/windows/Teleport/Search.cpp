#include "Teleport.h"
#include "Utils/Util.h"

namespace ModExplorerMenu
{
	// Populate list with NPCs according to filters & search results.
	// Will set state to `showAll` and delete any existing CachedNPC objects.
	// TODO: Find alternatives to C style comparison and copying.
	void TeleportWindow::ApplyFilters()
	{
		cellList.clear();
		selectedCell = nullptr;

		auto& cached_item_list = Data::GetCellMap();

		std::string compare;
		std::string input = inputBuffer;
		std::transform(input.begin(), input.end(), input.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

		// TODO: Implement additional columns
		for (auto& item : cached_item_list) {
			switch (searchKey) {
			case BaseColumn::ID::Plugin:
				compare = item.GetPluginName();
				break;
			case BaseColumn::ID::Space:
				compare = item.GetSpace();
				break;
			case BaseColumn::ID::Zone:
				compare = item.GetZone();
				break;
			case BaseColumn::ID::CellName:
				compare = item.GetCellName();
				break;
			case BaseColumn::ID::EditorID:
				compare = item.GetEditorID();
				break;
			default:
				compare = item.GetEditorID();
				break;
			}

			std::transform(compare.begin(), compare.end(), compare.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

			// If the input is wrapped in quotes, we do an exact match across all parameters.
			if (!input.empty() && input.front() == '"' && input.back() == '"') {
				std::string match = input.substr(1, input.size() - 2);

				if (compare == match) {
					cellList.push_back(&item);
				}
				continue;
			}

			if (selectedMod == ICON_RPG_HEART " Favorite" && !item.IsFavorite()) {
				continue;
			}

			if (selectedMod != ICON_RPG_WRENCH " All Mods" && selectedMod != ICON_RPG_HEART " Favorite" && item.GetPluginName() != selectedMod) {
				continue;
			}

			if (compare.find(input) != std::string::npos) {
				cellList.push_back(&item);
				continue;
			}
		}

		dirty = true;
	}

	// Draw search bar for filtering items.
	void TeleportWindow::ShowSearch(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_style;

		if (ImGui::CollapsingHeader("Refine your search:", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::NewLine();
			ImGui::Indent();

			ImGui::Text("Search results by:");

			auto filterWidth = ImGui::GetContentRegionAvail().x / 10.0f;
			auto inputTextWidth = ImGui::GetContentRegionAvail().x / 1.5f - filterWidth;
			auto totalWidth = inputTextWidth + filterWidth;

			ImGui::SetNextItemWidth(inputTextWidth);
			if (ImGui::InputTextWithHint("##TeleportWindow::InputField", "(Click to begin typing..)", inputBuffer,
					IM_ARRAYSIZE(inputBuffer),
					ImGuiInputTextFlags_EscapeClearsAll)) {
				ApplyFilters();
			}

			ImGui::SameLine();

			auto searchByValue = InputSearchMap.at(searchKey);
			auto combo_flags = ImGuiComboFlags_WidthFitPreview;
			ImGui::SetNextItemWidth(filterWidth);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5.0f);
			if (ImGui::BeginCombo("##TeleportWindow::InputFilter", searchByValue, combo_flags)) {
				for (auto& item : InputSearchMap) {
					auto searchBy = item.first;
					auto _searchByValue = item.second;
					bool is_selected = (searchKey == searchBy);

					if (ImGui::Selectable(_searchByValue, is_selected)) {
						searchKey = searchBy;
						ApplyFilters();
					}

					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			ImGui::NewLine();

			ImGui::Text("Filter modlist by:");
			ImGui::SetNextItemWidth(totalWidth);
			ImGui::InputTextWithHint("##TeleportWindow::ModField", "(Click to begin typing..)", modListBuffer,
				IM_ARRAYSIZE(modListBuffer),
				Frame::INPUT_FLAGS);

			auto min = ImVec2(0.0f, 0.0f);
			auto max = ImVec2(0.0f, ImGui::GetWindowSize().y / 4);
			ImGui::SetNextItemWidth(totalWidth);
			ImGui::SetNextWindowSizeConstraints(min, max);
			if (ImGui::BeginCombo("##TeleportWindow::FilterByMod", selectedMod.c_str())) {
				if (ImGui::Selectable(ICON_RPG_WRENCH " All Mods", selectedMod == ICON_RPG_WRENCH " All Mods")) {
					selectedMod = ICON_RPG_WRENCH " All Mods";
					ApplyFilters();
					ImGui::SetItemDefaultFocus();
				}

				if (ImGui::Selectable(ICON_RPG_HEART " Favorite", selectedMod == ICON_RPG_HEART " Favorite")) {
					selectedMod = ICON_RPG_HEART " Favorite";
					ApplyFilters();
					ImGui::SetItemDefaultFocus();
				}

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

				for (auto& mod : Data::GetModList(Data::CELL_MOD_LIST, a_config.modListSort)) {
					const char* modName = mod->GetFilename().data();
					bool is_selected = false;

					if (std::strlen(modListBuffer) > 0) {
						std::string compare = modName;
						std::string input = modListBuffer;

						std::transform(input.begin(), input.end(), input.begin(),
							[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

						std::transform(compare.begin(), compare.end(), compare.begin(),
							[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

						if (compare.find(input) != std::string::npos) {
							// Do nothing?
						} else {
							continue;
						}
					}

					if (ImGui::Selectable(modName, selectedMod == modName)) {
						selectedMod = modName;
						ApplyFilters();
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::Unindent();
			ImGui::NewLine();
		}
	}
}