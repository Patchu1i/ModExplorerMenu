#include "AddItem.h"
#include "Console.h"
#include "Utils/Util.h"

namespace Modex
{
	void AddItemWindow::ApplyFilters()
	{
		itemList.clear();

		auto& cached_item_list = Data::GetItemList();

		std::string compareString;
		std::string inputString = inputBuffer;
		std::transform(inputString.begin(), inputString.end(), inputString.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

		// TODO: Implement additional columns
		for (auto& item : cached_item_list) {
			switch (searchKey) {
			case BaseColumn::ID::Name:
				compareString = item.GetName();
				break;
			case BaseColumn::ID::FormID:
				compareString = item.GetFormID();
				break;
			case BaseColumn::ID::EditorID:
				compareString = item.GetEditorID();
				break;
			default:
				compareString = item.GetName();
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

			if (selectedFilter.second != "None") {
				if (item.GetFormType() != selectedFilter.first) {
					continue;
				}
			}

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
	void AddItemWindow::ShowSearch(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_style;

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
				if (ImGui::BeginCombo("##AddItemWindow::FilterByType", _T(selectedFilter.second), ImGuiComboFlags_HeightLarge)) {
					if (ImGui::Selectable(_T("None"), selectedFilter.second == "None")) {
						selectedFilter = { RE::FormType::None, "None" };
						ApplyFilters();
						ImGui::SetItemDefaultFocus();
					}

					for (auto& filter : filterMap) {
						auto formName = std::get<1>(filter).data();

						bool isSelected = (formName == selectedFilter.second);

						if (ImGui::Selectable(_T(formName), isSelected)) {
							selectedFilter = filter;
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
			if (ImGui::TreeNodeEx(_TFM("GENERAL_FILTER_MODLIST", ":"), ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::SetNextItemWidth(totalWidth);
				ImGui::InputTextWithHint("##AddItemWindow::ModField", _T("GENERAL_CLICK_TO_TYPE"), modListBuffer,
					IM_ARRAYSIZE(modListBuffer),
					Frame::INPUT_FLAGS);

				auto min = ImVec2(0.0f, 0.0f);
				auto max = ImVec2(0.0f, ImGui::GetWindowSize().y / 4);
				ImGui::SetNextItemWidth(totalWidth);
				ImGui::SetNextWindowSizeConstraints(min, max);

				std::string selectedModName = selectedMod == "Favorite" ? _TICON(ICON_RPG_HEART, selectedMod) :
				                              selectedMod == "All Mods" ? _TICON(ICON_RPG_WRENCH, selectedMod) :
				                                                          selectedMod;

				auto numOfFilter = 0;
				if (ImGui::BeginCombo("##AddItemWindow::FilterByMod", selectedModName.c_str())) {
					if (ImGui::Selectable(_TICON(ICON_RPG_WRENCH, "All Mods"), selectedMod == "All Mods")) {
						selectedMod = "All Mods";
						ApplyFilters();
						ImGui::SetItemDefaultFocus();
					}

					if (ImGui::Selectable(_TICON(ICON_RPG_HEART, "Favorite"), selectedMod == "Favorite")) {
						selectedMod = "Favorite";
						ApplyFilters();
						ImGui::SetItemDefaultFocus();
					}

					ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

					auto modFormTypeMap = Data::GetModFormTypeMap();
					for (auto& mod : Data::GetModList(Data::ITEM_MOD_LIST, a_config.modListSort)) {
						const char* modName = mod->GetFilename().data();
						bool bSelected = false;

						auto match = false;
						for (auto& modMap : modFormTypeMap) {
							if (mod == modMap.first) {
								numOfFilter = 0;
								for (auto& filter : filterMap) {
									auto formName = std::get<1>(filter);
									auto formType = std::get<0>(filter);
									auto isEnabled = (filter == selectedFilter);

									if (!isEnabled) {
										continue;
									}

									numOfFilter++;

									if (match) {
										continue;
									}

									switch (formType) {
									case RE::FormType::Armor:
										if (modMap.second.armor) {
											match = true;
										}
										break;
									case RE::FormType::Book:
										if (modMap.second.book) {
											match = true;
										}
										break;
									case RE::FormType::Weapon:
										if (modMap.second.weapon) {
											match = true;
										}
										break;
									case RE::FormType::Misc:
										if (modMap.second.misc) {
											match = true;
										}
										break;
									case RE::FormType::KeyMaster:
										if (modMap.second.key) {
											match = true;
										}
										break;
									case RE::FormType::Ammo:
										if (modMap.second.ammo) {
											match = true;
										}
										break;
									case RE::FormType::AlchemyItem:
										if (modMap.second.alchemy) {
											match = true;
										}
										break;
									case RE::FormType::Ingredient:
										if (modMap.second.ingredient) {
											match = true;
										}
										break;
									case RE::FormType::Scroll:
										if (modMap.second.scroll) {
											match = true;
										}
										break;
									default:
										break;
									}
								}

								if (numOfFilter == 0) {
									match = true;
								}
							}
						}

						if (!match) {
							continue;
						}

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

						if (bSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				ImGui::NewLine();
				ImGui::TreePop();
			}

			ImGui::Unindent();
		}
	}
}