#include "include/A/AddItem.h"
#include "include/C/Console.h"
#include "include/I/ISearch.h"
#include "include/P/Persistent.h"

namespace Modex
{
	void AddItemWindow::ApplyFilters()
	{
		itemList.clear();

		auto& cached_item_list = Data::GetSingleton()->GetAddItemList();

		std::string compareString;
		std::string inputString = inputBuffer;
		std::transform(inputString.begin(), inputString.end(), inputString.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

		//TODO: Implement additional columns
		for (auto& item : cached_item_list) {
			switch (searchKey) {
			case BaseColumn::ID::Name:
				compareString = item.GetNameView();
				break;
			case BaseColumn::ID::FormID:
				compareString = item.GetFormID();
				break;
			case BaseColumn::ID::EditorID:
				compareString = item.GetEditorID();
				break;
			default:
				compareString = item.GetNameView();
				break;
			}

			std::transform(compareString.begin(), compareString.end(), compareString.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

			// If the input is wrapped in quotes, we do an exact match across all parameters.
			if (!inputString.empty() && inputString.front() == '"' && inputString.back() == '"') {
				std::string match = inputString.substr(1, inputString.size() - 2);

				if (compareString == match) {
					itemList.push_back(&item);
				}
				continue;
			}

			// const char* compareString = nullptr;

			// for (auto& item : cached_item_list) {
			// 	switch (searchKey) {
			// 	case BaseColumn::ID::Name:
			// 		compareString = item.GetName();
			// 		break;
			// 	case BaseColumn::ID::FormID:
			// 		compareString = item.GetFormID().data();
			// 		break;
			// 	case BaseColumn::ID::EditorID:
			// 		compareString = item.GetEditorID().data();
			// 		break;
			// 	default:
			// 		compareString = item.GetName().data();
			// 		break;
			// 	}

			// if (strcasecmp(compareString, inputBuffer) == 0) {
			// 	continue;
			// }

			if (selectedMod != "All Mods" && item.GetPluginName() != selectedMod) {
				continue;
			}

			if (item.GetNameView() == "")
				continue;

			if (item.IsNonPlayable())  // non-useable
				continue;

			// if (selectedFilter.second != "None") {
			// 	if (item.GetFormType() != selectedFilter.first) {
			// 		continue;
			// 	}
			// }
			if (primaryFilter != RE::FormType::None) {
				if (item.GetFormType() != primaryFilter) {
					continue;
				}
			}

			if (selectedMod == "All Mods") {
				if (PersistentData::GetSingleton()->m_blacklist.contains(item.GetPlugin())) {
					continue;
				}
			}

			// if (strstr(compareString, inputBuffer) != nullptr) {
			// 	itemList.push_back(&item);
			// }
			if (compareString.find(inputString) != std::string::npos) {
				itemList.push_back(&item);
				continue;
			}

			dirty = true;
		}
	}

	void AddItemWindow::Refresh()
	{
		ApplyFilters();
	}

	// Draw search bar for filtering items.
	void AddItemWindow::ShowSearch()
	{
		if (ImGui::CollapsingHeader(_TFM("GENERAL_REFINE_SEARCH", ":"), ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Indent();

			auto filterWidth = ImGui::GetContentRegionAvail().x / 8.0f;
			auto inputTextWidth = ImGui::GetContentRegionAvail().x / 1.5f - filterWidth;
			auto totalWidth = inputTextWidth + filterWidth + 2.0f;

			// Search bar for compare string.
			if (ImGui::TreeNodeEx(_TFM("GENERAL_SEARCH_RESULTS", ":"), ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::SetNextItemWidth(inputTextWidth);
				if (ImGui::InputTextWithHint("##AddItemWindow::InputField", _T("GENERAL_CLICK_TO_TYPE"), inputBuffer,
						IM_ARRAYSIZE(inputBuffer),
						Frame::INPUT_FLAGS)) {
					ApplyFilters();
				}

				ImGui::SameLine();

				// TODO: Candidate for template function.
				auto currentFilter = InputSearchMap.at(searchKey);
				auto combo_flags = ImGuiComboFlags_None;
				ImGui::SetNextItemWidth(filterWidth);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5.0f);
				if (ImGui::BeginCombo("##AddItemWindow::InputFilter", _T(currentFilter), combo_flags)) {
					for (auto& compare : InputSearchMap) {
						BaseColumn::ID searchID = compare.first;
						const char* searchValue = compare.second;
						bool is_selected = (searchKey == searchID);

						if (ImGui::Selectable(_T(searchValue), is_selected)) {
							searchKey = searchID;
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

			// Secondary Record Type filters
			if (ImGui::TreeNodeEx(_TFM("GENERAL_FILTER_ITEM_TYPE", ":"), ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::SetNextItemWidth(totalWidth);

				std::string filterName = RE::FormTypeToString(primaryFilter).data();
				if (ImGui::BeginCombo("##AddItemWindow::FilterByType", _T(filterName), ImGuiComboFlags_HeightLarge)) {
					if (ImGui::Selectable(_T("None"), primaryFilter == RE::FormType::None)) {
						primaryFilter = RE::FormType::None;
						ApplyFilters();
						ImGui::SetItemDefaultFocus();
					}

					for (auto& filter : filterList) {
						bool isSelected = (filter == primaryFilter);

						std::string formName = RE::FormTypeToString(filter).data();
						if (ImGui::Selectable(_T(formName), isSelected)) {
							primaryFilter = filter;
							ApplyFilters();
						}

						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndCombo();
				}

				ImGui::NewLine();
				ImGui::TreePop();
			}

			// Mod List sort and filter.
			auto modListVector = Data::GetSingleton()->GetFilteredListOfPluginNames(Data::PLUGIN_TYPE::ITEM, primaryFilter);
			modListVector.insert(modListVector.begin(), "All Mods");
			if (ImGui::TreeNodeEx(_TFM("GENERAL_FILTER_MODLIST", ":"), ImGuiTreeNodeFlags_DefaultOpen)) {
				if (InputTextComboBox("##AddItemWindow::ModField", modSearchBuffer, selectedMod, IM_ARRAYSIZE(modSearchBuffer), modListVector, totalWidth)) {
					auto modList = Data::GetSingleton()->GetModulePluginList(Data::PLUGIN_TYPE::ITEM);
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

				ImGui::NewLine();
				ImGui::TreePop();
			}

			ImGui::Unindent();
		}
	}
}