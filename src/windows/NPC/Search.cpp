#include "include/C/Console.h"
#include "include/N/NPC.h"
#include "include/P/Persistent.h"

namespace Modex
{
	// Populate list with NPCs according to filters & search results.
	// Will set state to `showAll` and delete any existing CachedNPC objects.
	// TODO: Find alternatives to C style comparison and copying.
	void NPCWindow::ApplyFilters()
	{
		npcList.clear();
		selectedNPC = nullptr;

		auto& cachedNPCList = Data::GetSingleton()->GetNPCList();

		std::string compareString;
		std::string inputString = inputBuffer;
		std::transform(inputString.begin(), inputString.end(), inputString.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

		// TODO: Implement additional columns
		for (auto& npc : cachedNPCList) {
			switch (searchKey) {
			case BaseColumn::ID::Name:
				compareString = npc.GetNameView();
				break;
			case BaseColumn::ID::FormID:
				compareString = npc.GetFormID();
				break;
			case BaseColumn::ID::EditorID:
				compareString = npc.GetEditorID();
				break;
			case BaseColumn::ID::Race:
				compareString = npc.GetForm()->As<RE::TESNPC>()->race->GetFullName();
				break;
			default:
				compareString = npc.GetNameView();
				break;
			}

			std::transform(compareString.begin(), compareString.end(), compareString.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

			// If the input is wrapped in quotes, we do an exact match across all parameters.
			if (!inputString.empty() && inputString.front() == '"' && inputString.back() == '"') {
				std::string match = inputString.substr(1, inputString.size() - 2);

				if (compareString == match) {
					npcList.push_back(&npc);
				}
				continue;
			}

			if (selectedMod == "Favorite" && !npc.IsFavorite()) {
				continue;
			}

			if (selectedMod != "All Mods" && selectedMod != "Favorite" && npc.GetPluginName() != selectedMod) {
				continue;
			}

			if (npc.GetNameView() == "")
				continue;

			// for (auto& filter : filterMap) {
			// 	bool isEnabled = *std::get<0>(filter);
			// 	auto filterName = std::get<2>(filter);

			// 	if (isEnabled && selectedFilter == filterName) {
			// 		if (filterName == "Class") {
			// 			auto npcClass = npc.GetClass();
			// 			if (npcClass == secondaryFilter) {
			// 				npcList.push_back(&npc);
			// 			}
			// 		}

			// 		if (filterName == "Race") {
			// 			auto npcRace = npc.GetRace();
			// 			if (npcRace == secondaryFilter) {
			// 				npcList.push_back(&npc);
			// 			}
			// 		}

			// 		if (filterName == "Faction") {
			// 			auto npcFaction = npc.AsTESNPC->factions;
			// 			for (auto& faction : npcFaction) {
			// 				std::string factionName = faction.faction->GetFullName();
			// 				if (factionName == secondaryFilter) {
			// 					npcList.push_back(&npc);
			// 				}
			// 			}
			// 		}
			// 	}
			// }

			// if (selectedFilter != "None" && secondaryFilter != "Show All") {
			// 	continue;
			// }

			if (selectedMod == "All Mods") {
				if (PersistentData::GetSingleton()->m_blacklist.contains(npc.GetPlugin())) {
					continue;
				}
			}

			if (compareString.find(inputString) != std::string::npos) {
				npcList.push_back(&npc);
				continue;
			}
		}

		dirty = true;
	}

	void NPCWindow::Refresh()
	{
		ApplyFilters();
	}

	// Draw search bar for filtering items.
	void NPCWindow::ShowSearch()
	{
		if (ImGui::CollapsingHeader(_TFM("GENERAL_REFINE_SEARCH", ":"), ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Indent();

			auto filterWidth = ImGui::GetContentRegionAvail().x / 8.0f;
			auto inputTextWidth = ImGui::GetContentRegionAvail().x / 1.5f - filterWidth;
			auto totalWidth = inputTextWidth + filterWidth + 2.0f;

			// Search bar for compare string.
			if (ImGui::TreeNodeEx(_TFM("GENERAL_SEARCH_RESULTS", ":"), ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::SetNextItemWidth(inputTextWidth);

				if (ImGui::InputTextWithHint("##NPCWindow::InputField", _T("GENERAL_CLICK_TO_TYPE"), inputBuffer,
						IM_ARRAYSIZE(inputBuffer),
						ImGuiInputTextFlags_EscapeClearsAll)) {
					ApplyFilters();
				}

				ImGui::SameLine();

				// TODO: Candidate for template function.
				auto currentFilter = InputSearchMap.at(searchKey);
				auto combo_flags = ImGuiComboFlags_None;
				ImGui::SetNextItemWidth(filterWidth);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5.0f);
				if (ImGui::BeginCombo("##NPCWindow::InputFilter", _T(currentFilter), combo_flags)) {
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

			// Secondary drop down filter list.
			// if (ImGui::TreeNodeEx(_TFM("GENERAL_FILTER_SEARCH", ":"), ImGuiTreeNodeFlags_DefaultOpen)) {
			// 	ImGui::SetNextItemWidth(totalWidth);
			// 	ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, 0.0f), ImVec2(0.0f, ImGui::GetWindowSize().y / 4));

			// 	if (ImGui::BeginCombo("##NPCWindow::PrimaryFilter", selectedFilter.c_str())) {
			// 		if (ImGui::Selectable(_T("None"), selectedFilter == "None")) {
			// 			selectedFilter = "None";
			// 			ApplyFilters();
			// 			ImGui::SetItemDefaultFocus();
			// 		}

			// 		for (auto& filter : filterMap) {
			// 			auto& isEnabled = *std::get<0>(filter);
			// 			auto func = std::get<1>(filter);
			// 			auto name = std::get<2>(filter);

			// 			if (ImGui::Selectable(name.c_str(), selectedFilter == name)) {
			// 				selectedFilter = name;
			// 				isEnabled = !isEnabled;

			// 				func();

			// 				// Reset the list for clean start.
			// 				secondaryFilter = "Show All";
			// 				ApplyFilters();
			// 			}
			// 		}
			// 		ImGui::EndCombo();
			// 	}

			// 	if (selectedFilter != "None") {
			// 		ImGui::SetNextItemWidth(totalWidth);
			// 		ImGui::SetNextWindowSizeConstraints(ImVec2(totalWidth, 0.0f), ImVec2(totalWidth, ImGui::GetWindowSize().y / 2));

			// 		if (ImGui::BeginCombo("##NPCWindow::SecondaryFilter", secondaryFilter.c_str())) {
			// 			if (ImGui::Selectable(_T("GENERAL_SHOW_SEARCHBAR"), &b_ShowSearchbar)) {
			// 				secondaryFilterBuffer[0] = '\0';
			// 				ImGui::SetItemDefaultFocus();
			// 			}

			// 			if (ImGui::Selectable(_T("Show All"), secondaryFilter == "Show All")) {
			// 				secondaryFilter = "Show All";
			// 				ApplyFilters();
			// 				ImGui::SetItemDefaultFocus();
			// 			}

			// 			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			// 			for (auto& filter : secondaryFilterMap) {
			// 				if (filter.second == selectedFilter) {
			// 					auto list = filter.first();

			// 					for (auto& item : list) {
			// 						if (item.empty()) {
			// 							continue;
			// 						}

			// 						if (b_ShowSearchbar) {
			// 							if (std::strlen(secondaryFilterBuffer) > 0) {
			// 								std::string compareString = item;
			// 								std::string inputString = secondaryFilterBuffer;

			// 								std::transform(inputString.begin(), inputString.end(), inputString.begin(),
			// 									[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

			// 								std::transform(compareString.begin(), compareString.end(), compareString.begin(),
			// 									[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

			// 								if (compareString.find(inputString) != std::string::npos) {
			// 									// Do nothing?
			// 								} else {
			// 									continue;
			// 								}
			// 							}
			// 						}

			// 						if (ImGui::Selectable(item.c_str(), secondaryFilter == item)) {
			// 							secondaryFilter = item;

			// 							ApplyFilters();
			// 							ImGui::SetItemDefaultFocus();
			// 						}
			// 					}
			// 				}
			// 			}
			// 			ImGui::EndCombo();
			// 		}

			// 		if (b_ShowSearchbar) {
			// 			ImGui::NewLine();
			// 			ImGui::Text(_TFM("GENERAL_FILTER_REFINE", ":"));
			// 			ImGui::SetNextItemWidth(inputTextWidth + filterWidth);
			// 			if (ImGui::InputTextWithHint("##NPCWindow::SecondaryFilterSearch", _T("GENERAL_CLICK_TO_TYPE"), secondaryFilterBuffer,
			// 					IM_ARRAYSIZE(secondaryFilterBuffer),
			// 					ImGuiInputTextFlags_EscapeClearsAll)) {
			// 				// Text Callback if we need it.
			// 			}
			// 		} else {
			// 			secondaryFilterBuffer[0] = '\0';
			// 		}
			// 	}

			// 	ImGui::NewLine();
			// 	ImGui::TreePop();
			// }

			// Mod List sort and filter.

			auto modListVector = Data::GetSingleton()->GetFilteredListOfPluginNames(Data::PLUGIN_TYPE::NPC, RE::FormType::None);
			modListVector.insert(modListVector.begin(), "All Mods");
			if (ImGui::TreeNodeEx(_TFM("GENERAL_FILTER_MODLIST", ":"), ImGuiTreeNodeFlags_DefaultOpen)) {
				if (InputTextComboBox("##NPCWindow::ModField", modSearchBuffer, selectedMod, IM_ARRAYSIZE(modSearchBuffer), modListVector, totalWidth)) {
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