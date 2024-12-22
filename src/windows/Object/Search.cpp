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

		auto& cached_item_list = Data::GetObjectList();

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
			case BaseColumn::ID::Type:
				compare = item.GetTypeName();
				break;
			case BaseColumn::ID::FormID:
				compare = item.GetFormID();
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
					objectList.push_back(&item);
				}
				continue;
			}

			if (selectedMod == "Favorite" && !item.IsFavorite()) {
				continue;
			}

			if (selectedMod != "All Mods" && selectedMod != "Favorite" && item.GetPluginName() != selectedMod) {
				continue;
			}

			if (objectFilters.size() > 0) {
				if (objectFilters.find(item.GetFormType()) == objectFilters.end()) {
					continue;
				}
			}

			if (compare.find(input) != std::string::npos) {
				objectList.push_back(&item);
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

			auto searchByValue = InputSearchMap.at(searchKey);
			auto combo_flags = ImGuiComboFlags_None;
			ImGui::SetNextItemWidth(filterWidth);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5.0f);
			if (ImGui::BeginCombo("##ObjectWindow::InputFilter", _T(searchByValue), combo_flags)) {
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
			ImGui::Text(_TFM("GENERAL_FILTER_ITEM_TYPE", ":"));
			ImGui::NewLine();

			auto column = 0;
			bool _change = false;
			auto start_x = ImGui::GetCursorPosX();
			auto width = ImGui::GetContentRegionAvail().x / 8.0f;
			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
			for (auto& filter : filterMap) {
				auto isEnabled = std::get<0>(filter);
				const auto name = std::get<2>(filter);
				column++;

				if (column == 5) {
					ImGui::SetCursorPosX(start_x + 5.0f);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + a_style.itemSpacing.y);
				} else {
					ImGui::SameLine(0.0f, a_style.itemSpacing.x + 2.0f);
				}

				if (column == 1) {
					ImGui::SetCursorPosX(start_x + 5.0f);
				}

				if (*isEnabled == true) {
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(a_style.header.x, a_style.header.y, a_style.header.z, a_style.header.w));
				} else {
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(a_style.header.x, a_style.header.y, a_style.header.z, a_style.header.w - 0.2f));
				}

				if (ImGui::Selectable(_T(name), true, ImGuiSelectableFlags_SelectOnClick, ImVec2(width, ImGui::GetFontSize() * 1.5f))) {
					_change = true;
					*isEnabled = !*isEnabled;
				}

				ImGui::PopStyleColor();
			}
			ImGui::PopStyleVar();

			if (_change) {
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

				auto mapped_mods = Data::GetModFormTypeMap();
				for (auto& mod : Data::GetModList(Data::STATIC_MOD_LIST, a_config.modListSort)) {
					const char* modName = mod->GetFilename().data();
					bool is_selected = false;

					auto found = false;
					for (auto& mapped_mod : mapped_mods) {
						if (mod == mapped_mod.first) {
							auto count = 0;
							for (auto& filter : filterMap) {
								auto isEnabled = *std::get<0>(filter);
								auto formType = std::get<1>(filter);

								if (!isEnabled) {
									continue;
								}

								count++;

								if (found) {
									continue;
								}

								switch (formType) {
								case RE::FormType::Tree:
									if (mapped_mod.second.tree) {
										found = true;
									}
									break;
								case RE::FormType::Static:
									if (mapped_mod.second.staticObject) {
										found = true;
									}
									break;
								case RE::FormType::Container:
									if (mapped_mod.second.container) {
										found = true;
									}
									break;
								case RE::FormType::Light:
									if (mapped_mod.second.light) {
										found = true;
									}
									break;
								case RE::FormType::Door:
									if (mapped_mod.second.door) {
										found = true;
									}
									break;
								case RE::FormType::Activator:
									if (mapped_mod.second.activator) {
										found = true;
									}
									break;
								default:
									break;
								}
							}

							if (count == 0) {
								found = true;
							}
						}
					}

					if (!found) {
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