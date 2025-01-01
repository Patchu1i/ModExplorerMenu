#include "Object.h"
#include "Utils/Util.h"

namespace ModExplorerMenu
{
	// Populate list with NPCs according to filters & search results.
	// Will set state to `showAll` and delete any existing CachedNPC objects.
	// TODO: Find alternatives to C style comparison and copying.
	void ObjectWindow::ApplyFilters()
	{
		objectList.clear();
		selectedObject = nullptr;

		auto& cachedObjectList = Data::GetObjectList();

		std::string compareString;
		std::string inputString = inputBuffer;
		std::transform(inputString.begin(), inputString.end(), inputString.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

		// TODO: Implement additional columns
		for (auto& obj : cachedObjectList) {
			switch (searchKey) {
			case BaseColumn::ID::Plugin:
				compareString = obj.GetPluginName();
				break;
			case BaseColumn::ID::Type:
				compareString = obj.GetTypeName();
				break;
			case BaseColumn::ID::FormID:
				compareString = obj.GetFormID();
				break;
			case BaseColumn::ID::EditorID:
				compareString = obj.GetEditorID();
				break;
			default:
				compareString = obj.GetEditorID();
				break;
			}

			std::transform(compareString.begin(), compareString.end(), compareString.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

			// If the input is wrapped in quotes, we do an exact match across all parameters.
			if (!inputString.empty() && inputString.front() == '"' && inputString.back() == '"') {
				std::string match = inputString.substr(1, inputString.size() - 2);

				if (compareString == match) {
					objectList.push_back(&obj);
				}
				continue;
			}

			if (selectedMod == "Favorite" && !obj.IsFavorite()) {
				continue;
			}

			if (selectedMod != "All Mods" && selectedMod != "Favorite" && obj.GetPluginName() != selectedMod) {
				continue;
			}

			if (selectedFilter.second != "None") {
				if (obj.GetFormType() != selectedFilter.first) {
					continue;
				}
			}

			if (compareString.find(inputString) != std::string::npos) {
				objectList.push_back(&obj);
				continue;
			}
		}

		dirty = true;
	}

	void ObjectWindow::Refresh()
	{
		ApplyFilters();
	}

	// Draw search bar for filtering items.
	void ObjectWindow::ShowSearch(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_style;

		if (ImGui::CollapsingHeader(_TFM("GENERAL_REFINE_SEARCH", ":"), ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Indent();

			auto filterWidth = ImGui::GetContentRegionAvail().x / 8.0f;
			auto inputTextWidth = ImGui::GetContentRegionAvail().x / 1.5f - filterWidth;
			auto totalWidth = inputTextWidth + filterWidth + 2.0f;

			// General search bar for filtering items.
			if (ImGui::TreeNodeEx(_TFM("GENERAL_SEARCH_RESULTS", ":"), ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::SetNextItemWidth(inputTextWidth);
				if (ImGui::InputTextWithHint("##ObjectWindow::InputField", _T("GENERAL_CLICK_TO_TYPE"), inputBuffer,
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

			// Secondary Filter for object record types.
			if (ImGui::TreeNodeEx(_TFM("GENERAL_FILTER_ITEM_TYPE", ":"), ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::SetNextItemWidth(totalWidth);
				if (ImGui::BeginCombo("##ObjectWindow::FilterByType", _T(selectedFilter.second))) {
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

			if (ImGui::TreeNodeEx(_TFM("GENERAL_FILTER_MODLIST", ":"), ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::SetNextItemWidth(totalWidth);
				ImGui::InputTextWithHint("##ObjectWindow::ModField", _T("GENERAL_CLICK_TO_TYPE"), modListBuffer,
					IM_ARRAYSIZE(modListBuffer),
					Frame::INPUT_FLAGS);

				auto min = ImVec2(0.0f, 0.0f);
				auto max = ImVec2(0.0f, ImGui::GetWindowSize().y / 4);
				ImGui::SetNextItemWidth(totalWidth);
				ImGui::SetNextWindowSizeConstraints(min, max);

				std::string selectedModName = selectedMod == "Favorite" ? _TICON(ICON_RPG_HEART, selectedMod) :
				                              selectedMod == "All Mods" ? _TICON(ICON_RPG_WRENCH, selectedMod) :
				                                                          selectedMod;

				if (ImGui::BeginCombo("##ObjectWindow::FilterByMod", selectedModName.c_str())) {
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
					auto numOfFilter = 0;
					for (auto& mod : Data::GetModList(Data::STATIC_MOD_LIST, a_config.modListSort)) {
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
									case RE::FormType::Tree:
										if (modMap.second.tree) {
											match = true;
										}
										break;
									case RE::FormType::Static:
										if (modMap.second.staticObject) {
											match = true;
										}
										break;
									case RE::FormType::Container:
										if (modMap.second.container) {
											match = true;
										}
										break;
									case RE::FormType::Light:
										if (modMap.second.light) {
											match = true;
										}
										break;
									case RE::FormType::Door:
										if (modMap.second.door) {
											match = true;
										}
										break;
									case RE::FormType::Activator:
										if (modMap.second.activator) {
											match = true;
										}
										break;
									case RE::FormType::Furniture:
										if (modMap.second.furniture) {
											match = true;
										}
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
							std::string compareString = modName;
							std::string inputString = modListBuffer;

							std::transform(inputString.begin(), inputString.end(), inputString.begin(),
								[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

							std::transform(compareString.begin(), compareString.end(), compareString.begin(),
								[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

							if (compareString.find(inputString) != std::string::npos) {
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