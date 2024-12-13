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

			if (selectedMod == "Favorite" && !item.IsFavorite()) {
				continue;
			}

			if (selectedMod != "All Mods" && selectedMod != "Favorite" && item.GetPluginName() != selectedMod) {
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
		(void)a_config;

		if (ImGui::CollapsingHeader("Refine your search:", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::NewLine();
			ImGui::Indent();

			if (ImGui::InputTextWithHint("##AddItemWindow::InputField", "Enter text to filter results by...", inputBuffer,
					IM_ARRAYSIZE(inputBuffer),
					InputSearchFlags)) {
				ApplyFilters();
			}

			ImGui::SameLine();

			auto searchByValue = InputSearchMap.at(searchKey);
			auto combo_flags = ImGuiComboFlags_WidthFitPreview;
			if (ImGui::BeginCombo("##AddItemWindow::InputFilter", searchByValue, combo_flags)) {
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
			ImGui::NewLine();

			bool _change = false;

			// Filter checkboxes up top.
			for (auto& item : AddItemWindow::filterMap) {
				auto first = std::get<0>(item);
				const auto third = std::get<2>(item);

				ImGui::SameLine();
				if (ImGui::Checkbox(third.c_str(), first)) {
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

			ImGui::NewLine();

			ImGui::InputTextWithHint("##AddItemWindow::ModField", "Enter text to filter mod list by...", modListBuffer,
				IM_ARRAYSIZE(modListBuffer),
				InputSearchFlags);

			if (ImGui::BeginCombo("##AddItemWindow::FilterByMod", selectedMod.c_str())) {
				if (ImGui::Selectable("All Mods", selectedMod == "All Mods")) {
					selectedMod = "All Mods";
					ApplyFilters();
					ImGui::SetItemDefaultFocus();
				}

				if (ImGui::Selectable("Favorite", selectedMod == "Favorite")) {
					selectedMod = "Favorite";
					ApplyFilters();
					ImGui::SetItemDefaultFocus();
				}

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

				for (auto& mod : Data::GetModList()) {
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
					if (ImGui::Selectable(modName, is_selected)) {
						selectedMod = modName;
						ApplyFilters();
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::SameLine();

			const auto addAllItems = []() {
				for (auto& item : Data::GetItemList()) {
					if (item.GetPluginName() == selectedMod) {
						Console::AddItem(item.GetFormID());
					}
				}

				Console::StartProcessThread();
			};

			ImGui::Unindent();
			ImGui::NewLine();
		}
	}
}