#include "AddItem.h"
#include "Console.h"
#include "Utils/Util.h"

namespace ModExplorerMenu
{
	void AddItemWindow::ApplyFilters()
	{
		itemList.clear();

		auto& cached_item_list = Data::GetItemList();

		std::string compare;
		std::string input = inputBuffer;
		std::transform(input.begin(), input.end(), input.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

		// TODO: Implement additional columns
		for (auto& item : cached_item_list) {
			switch (searchKey) {
			case BaseColumn::ID::Name:
				compare = item.GetName();
				break;
			case BaseColumn::ID::FormID:
				compare = item.GetFormID();
				break;
			case BaseColumn::ID::EditorID:
				compare = item.GetEditorID();
				break;
			default:
				compare = item.GetName();
				break;
			}

			std::transform(compare.begin(), compare.end(), compare.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

			// If the input is wrapped in quotes, we do an exact match across all parameters.
			if (!input.empty() && input.front() == '"' && input.back() == '"') {
				std::string match = input.substr(1, input.size() - 2);

				if (compare == match) {
					itemList.push_back(&item);
				}
				continue;
			}

			if (selectedMod == _TICON(ICON_RPG_HEART, "Favorite") && !item.IsFavorite()) {
				continue;
			}

			if (selectedMod != _TICON(ICON_RPG_WRENCH, "All Mods") && selectedMod != _TICON(ICON_RPG_HEART, "Favorite") && item.GetPluginName() != selectedMod) {
				continue;
			}

			if (item.GetName() == "")
				continue;

			if (item.IsNonPlayable())  // non-useable
				continue;

			if (itemFilters.size() > 0) {
				if (itemFilters.find(item.GetFormType()) == itemFilters.end()) {
					continue;
				}
			}

			if (compare.find(input) != std::string::npos) {
				itemList.push_back(&item);
				continue;
			}
		}

		dirty = true;
	}

	// Draw search bar for filtering items.
	void AddItemWindow::ShowSearch(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_style;

		if (ImGui::CollapsingHeader((std::string(_T("GENERAL_REFINE_SEARCH")) + ":").c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::NewLine();
			ImGui::Indent();

			ImGui::Text((std::string(_T("GENERAL_SEARCH_RESULTS")) + ":").c_str());

			auto filterWidth = ImGui::GetContentRegionAvail().x / 10.0f;
			auto inputTextWidth = ImGui::GetContentRegionAvail().x / 1.5f - filterWidth;
			auto totalWidth = inputTextWidth + filterWidth;

			ImGui::SetNextItemWidth(inputTextWidth);
			if (ImGui::InputTextWithHint("##AddItemWindow::InputField", _T("GENERAL_CLICK_TO_TYPE"), inputBuffer,
					IM_ARRAYSIZE(inputBuffer),
					Frame::INPUT_FLAGS)) {
				ApplyFilters();
			}

			ImGui::SameLine();

			auto searchByValue = InputSearchMap.at(searchKey);
			auto combo_flags = ImGuiComboFlags_None;
			ImGui::SetNextItemWidth(filterWidth);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5.0f);
			if (ImGui::BeginCombo("##AddItemWindow::InputFilter", _T(searchByValue), combo_flags)) {
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

			// Filter checkboxes up top.
			ImGui::NewLine();
			ImGui::Text((std::string(_T("AIM_FILTER_ITEM_TYPE")) + ":").c_str());
			ImGui::NewLine();
			ImGui::Unindent();
			bool _change = false;
			for (auto& item : AddItemWindow::filterMap) {
				auto first = std::get<0>(item);
				const auto third = std::get<2>(item);

				ImGui::SameLine();
				if (ImGui::Checkbox(_T(third), first)) {
					_change = true;
				}
			}

			if (_change) {
				itemFilters.clear();

				for (auto& item : filterMap) {
					auto first = *std::get<0>(item);
					const auto second = std::get<1>(item);

					if (first) {
						itemFilters.insert(second);
					}
				}

				ApplyFilters();
				dirty = true;
			}

			ImGui::Indent();
			ImGui::NewLine();

			ImGui::Text((std::string(_T("GENERAL_FILTER_MODLIST")) + ":").c_str());
			ImGui::SetNextItemWidth(totalWidth);
			ImGui::InputTextWithHint("##AddItemWindow::ModField", _T("GENERAL_CLICK_TO_TYPE"), modListBuffer,
				IM_ARRAYSIZE(modListBuffer),
				Frame::INPUT_FLAGS);

			auto min = ImVec2(0.0f, 0.0f);
			auto max = ImVec2(0.0f, ImGui::GetWindowSize().y / 4);
			ImGui::SetNextItemWidth(totalWidth);
			ImGui::SetNextWindowSizeConstraints(min, max);
			if (ImGui::BeginCombo("##AddItemWindow::FilterByMod", _T(selectedMod))) {
				if (ImGui::Selectable(_TICON(ICON_RPG_WRENCH, "All Mods"), selectedMod == _TICON(ICON_RPG_WRENCH, "All Mods"))) {
					selectedMod = _TICON(ICON_RPG_WRENCH, "All Mods");
					ApplyFilters();
					ImGui::SetItemDefaultFocus();
				}

				if (ImGui::Selectable(_TICON(ICON_RPG_HEART, "Favorite"), selectedMod == _TICON(ICON_RPG_HEART, "Favorite"))) {
					selectedMod = _TICON(ICON_RPG_HEART, "Favorite");
					ApplyFilters();
					ImGui::SetItemDefaultFocus();
				}

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

				for (auto& mod : Data::GetModList(Data::ITEM_MOD_LIST, a_config.modListSort)) {
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