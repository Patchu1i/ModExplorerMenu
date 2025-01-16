#include "include/P/Persistent.h"
#include "include/T/Teleport.h"

namespace Modex
{
	// Populate list with NPCs according to filters & search results.
	// Will set state to `showAll` and delete any existing CachedNPC objects.
	// TODO: Find alternatives to C style comparison and copying.
	void TeleportWindow::ApplyFilters()
	{
		cellList.clear();
		selectedCell = nullptr;

		auto& cached_item_list = Data::GetSingleton()->GetTeleportList();

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

			if (selectedMod == "Favorite" && !item.IsFavorite()) {
				continue;
			}

			if (selectedMod != "All Mods" && selectedMod != "Favorite" && item.GetPluginName() != selectedMod) {
				continue;
			}

			if (selectedMod == "All Mods") {
				if (PersistentData::GetSingleton()->m_blacklist.contains(item.mod)) {
					continue;
				}
			}

			if (compare.find(input) != std::string::npos) {
				cellList.push_back(&item);
				continue;
			}
		}

		dirty = true;
	}

	void TeleportWindow::Refresh()
	{
		ApplyFilters();
	}

	// Draw search bar for filtering items.
	void TeleportWindow::ShowSearch()
	{
		if (ImGui::CollapsingHeader(_TFM("GENERAL_REFINE_SEARCH", ":"), ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Indent();

			auto filterWidth = ImGui::GetContentRegionAvail().x / 8.0f;
			auto inputTextWidth = ImGui::GetContentRegionAvail().x / 1.5f - filterWidth;
			// auto totalWidth = inputTextWidth + filterWidth + 2.0f;

			// Search bar for compare string.
			if (ImGui::TreeNodeEx(_TFM("GENERAL_SEARCH_RESULTS", ":"), ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::SetNextItemWidth(inputTextWidth);
				if (ImGui::InputTextWithHint("##TeleportWindow::InputField", _T("GENERAL_CLICK_TO_TYPE"), inputBuffer,
						IM_ARRAYSIZE(inputBuffer),
						ImGuiInputTextFlags_EscapeClearsAll)) {
					ApplyFilters();
				}

				ImGui::SameLine();

				auto searchByValue = InputSearchMap.at(searchKey);
				auto combo_flags = ImGuiComboFlags_None;
				ImGui::SetNextItemWidth(filterWidth);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5.0f);
				if (ImGui::BeginCombo("##TeleportWindow::InputFilter", _T(searchByValue), combo_flags)) {
					for (auto& item : InputSearchMap) {
						auto searchBy = item.first;
						auto _searchByValue = item.second;
						bool is_selected = (searchKey == searchBy);

						if (ImGui::Selectable(_T(_searchByValue), is_selected)) {
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
				ImGui::TreePop();
			}

			// Mod List sort and filter.
			if (ImGui::TreeNodeEx(_TFM("GENERAL_FILTER_MODLIST", ":"), ImGuiTreeNodeFlags_DefaultOpen)) {
				// ImGui::SetNextItemWidth(totalWidth);
				// ImGui::InputTextWithHint("##TeleportWindow::ModField", _T("GENERAL_CLICK_TO_TYPE"), modListBuffer,
				// 	IM_ARRAYSIZE(modListBuffer),
				// 	Frame::INPUT_FLAGS);

				// std::string selectedModName = selectedMod == "Favorite" ? _TICON(ICON_RPG_HEART, selectedMod) :
				//                               selectedMod == "All Mods" ? _TICON(ICON_RPG_WRENCH, selectedMod) :
				//                                                           selectedMod;

				// (void)a_config;
				// auto min = ImVec2(totalWidth, 0.0f);
				// auto max = ImVec2(totalWidth, ImGui::GetWindowSize().y / 4);
				// ImGui::SetNextItemWidth(totalWidth);
				// ImGui::SetNextWindowSizeConstraints(min, max);
				// if (ImGui::BeginCombo("##TeleportWindow::FilterByMod", selectedModName.c_str())) {
				// 	if (ImGui::Selectable(_TICON(ICON_RPG_WRENCH, "All Mods"), selectedMod == "All Mods")) {
				// 		selectedMod = "All Mods";
				// 		ApplyFilters();
				// 		ImGui::SetItemDefaultFocus();
				// 	}

				// 	if (ImGui::Selectable(_TICON(ICON_RPG_HEART, "Favorite"), selectedMod == "Favorite")) {
				// 		selectedMod = "Favorite";
				// 		ApplyFilters();
				// 		ImGui::SetItemDefaultFocus();
				// 	}

				// 	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

				// 	auto modList = Data::GetModList(Data::ITEM_MOD_LIST, a_config.modListSort);
				// 	for (auto& mod : *modList) {
				// 		bool bSelected = false;

				// 		if (mod->GetFilename().data() == nullptr) {
				// 			continue;
				// 		}

				// 		const char* modName = mod->GetFilename().data();

				// 		if (PersistentData::GetSingleton()->m_blacklist.contains(mod)) {
				// 			continue;
				// 		}

				// 		if (std::strlen(modListBuffer) > 0) {
				// 			std::string compare = modName;
				// 			std::string input = modListBuffer;

				// 			std::transform(input.begin(), input.end(), input.begin(),
				// 				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

				// 			std::transform(compare.begin(), compare.end(), compare.begin(),
				// 				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

				// 			if (compare.find(input) != std::string::npos) {
				// 				// Do nothing?
				// 			} else {
				// 				continue;
				// 			}
				// 		}

				// 		if (ImGui::Selectable(modName, selectedMod == modName)) {
				// 			selectedMod = modName;
				// 			ApplyFilters();
				// 		}

				// 		if (bSelected)  // ??
				// 			ImGui::SetItemDefaultFocus();
				// 	}
				// 	ImGui::EndCombo();
				// }
				ImGui::NewLine();
				ImGui::TreePop();
			}

			ImGui::Unindent();
		}
	}
}