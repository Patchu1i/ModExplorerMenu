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

			if (objectFilters.size() > 0) {
				if (objectFilters.find(obj.GetFormType()) == objectFilters.end()) {
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

	// Draw search bar for filtering items.
	void ObjectWindow::ShowSearch(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_style;

		if (ImGui::CollapsingHeader(_TFM("GENERAL_REFINE_SEARCH", ":"), ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::NewLine();
			ImGui::Indent();

			ImGui::Text(_TFM("GENERAL_SEARCH_RESULTS", ":"));

			auto filterWidth = ImGui::GetContentRegionAvail().x / 10.0f;
			auto inputTextWidth = ImGui::GetContentRegionAvail().x / 1.5f - filterWidth;
			auto totalWidth = inputTextWidth + filterWidth;

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

			// Filter checkboxes up top.
			ImGui::NewLine();
			ImGui::Text(_TFM("GENERAL_FILTER_ITEM_TYPE", ":"));
			ImGui::NewLine();

			auto numOfFilter = 0;
			bool isDirty = false;
			auto sx = ImGui::GetCursorPosX();
			auto width = ImGui::GetContentRegionAvail().x / 8.0f;
			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
			for (auto& filter : filterMap) {
				auto isEnabled = std::get<0>(filter);
				const auto name = std::get<2>(filter);
				numOfFilter++;

				if (numOfFilter == 5) {
					ImGui::SetCursorPosX(sx + 5.0f);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + a_style.itemSpacing.y);
				} else {
					ImGui::SameLine(0.0f, a_style.itemSpacing.x + 2.0f);
				}

				if (numOfFilter == 1) {
					ImGui::SetCursorPosX(sx + 5.0f);
				}

				if (*isEnabled == true) {
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(a_style.button.x, a_style.button.y, a_style.button.z, a_style.button.w));
				} else {
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(a_style.button.x, a_style.button.y, a_style.button.z, a_style.button.w - 0.2f));
				}

				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(a_style.buttonHovered.x, a_style.buttonHovered.y, a_style.buttonHovered.z, a_style.buttonHovered.w));

				if (ImGui::Selectable(_T(name), true, ImGuiSelectableFlags_SelectOnClick, ImVec2(width, ImGui::GetFontSize() * 1.5f))) {
					isDirty = true;
					*isEnabled = !*isEnabled;
				}

				ImGui::PopStyleColor(2);
			}
			ImGui::PopStyleVar();

			if (isDirty) {
				objectFilters.clear();

				for (auto& item : filterMap) {
					auto isEnabled = *std::get<0>(item);
					const auto formType = std::get<1>(item);

					if (isEnabled) {
						objectFilters.insert(formType);
					}
				}

				ApplyFilters();
				dirty = true;
			}

			ImGui::NewLine();
			ImGui::Text(_TFM("GENERAL_FILTER_MODLIST", ":"));
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
				for (auto& mod : Data::GetModList(Data::STATIC_MOD_LIST, a_config.modListSort)) {
					const char* modName = mod->GetFilename().data();
					bool bSelected = false;

					auto match = false;
					for (auto& modMap : modFormTypeMap) {
						if (mod == modMap.first) {
							numOfFilter = 0;
							for (auto& filter : filterMap) {
								auto isEnabled = *std::get<0>(filter);
								auto formType = std::get<1>(filter);

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

			ImGui::Unindent();
			ImGui::NewLine();
		}
	}
}