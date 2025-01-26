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

			if (selectedMod != "All Mods" && item.GetPluginName() != selectedMod) {
				continue;
			}

			// Don't include Blacklist with Teleport since users will likely
			// blacklist mods that are landscape or quest mods for AddItem or NPC.
			//
			// if (selectedMod == "All Mods") {
			// 	if (PersistentData::GetSingleton()->m_blacklist.contains(item.GetPluginName())) {
			// 		continue;
			// 	}
			// }

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
		auto filterWidth = ImGui::GetContentRegionAvail().x / 8.0f;
		auto inputTextWidth = ImGui::GetContentRegionAvail().x / 1.5f - filterWidth;
		// auto totalWidth = inputTextWidth + filterWidth + 2.0f;

		// Search bar for compare string.
		ImGui::Text(_TFM("GENERAL_SEARCH_RESULTS", ":"));
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

		// Mod List sort and filter.
		const auto& config = Settings::GetSingleton()->GetConfig();
		auto modListVector = Data::GetSingleton()->GetFilteredListOfPluginNames(Data::PLUGIN_TYPE::CELL, (Data::SORT_TYPE)config.modListSort, RE::FormType::None);
		modListVector.insert(modListVector.begin(), "All Mods");
		ImGui::Text(_TFM("GENERAL_FILTER_MODLIST", ":"));
		if (InputTextComboBox("##TeleportWindow::ModField", modSearchBuffer, selectedMod, IM_ARRAYSIZE(modSearchBuffer), modListVector, inputTextWidth)) {
			auto modList = Data::GetSingleton()->GetModulePluginList(Data::PLUGIN_TYPE::CELL);
			selectedMod = "All Mods";

			if (selectedMod.find(modSearchBuffer) != std::string::npos) {
				ImFormatString(modSearchBuffer, IM_ARRAYSIZE(modSearchBuffer), "");
			} else {
				for (auto& mod : modList) {
					if (PersistentData::GetSingleton()->m_blacklist.contains(mod)) {
						continue;
					}

					std::string modName = ValidateTESFileName(mod);

					if (modName == "All Mods") {
						ImFormatString(modSearchBuffer, IM_ARRAYSIZE(modSearchBuffer), "");
						break;
					}

					if (modName.find(modSearchBuffer) != std::string::npos) {
						selectedMod = modName;
						ImFormatString(modSearchBuffer, IM_ARRAYSIZE(modSearchBuffer), "");
						break;
					}
				}
			}

			ApplyFilters();
		}
	}
}
