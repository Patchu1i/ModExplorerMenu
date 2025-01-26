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
				compareString = npc.GetRace();
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

			if (selectedMod != "All Mods" && npc.GetPluginName() != selectedMod) {
				continue;
			}

			// Ensure NPC's from Blacklisted Plugins aren't shown.
			if (selectedMod == "All Mods") {
				if (PersistentData::GetSingleton()->m_blacklist.contains(npc.GetPlugin())) {
					continue;
				}
			}

			bool skip = false;
			if (primaryFilter != RE::FormType::None && secondaryFilter != "Show All") {
				for (auto& filter : filterList) {
					bool isSelected = (filter == primaryFilter);

					if (isSelected) {
						if (primaryFilter == RE::FormType::Class) {
							auto npcClass = npc.GetClass();

							if (npcClass != secondaryFilter) {
								skip = true;
								break;
							}
						}

						if (primaryFilter == RE::FormType::Race) {
							auto npcRace = npc.GetRace();

							if (npcRace != secondaryFilter) {
								skip = true;
								break;
							}
						}

						if (primaryFilter == RE::FormType::Faction) {
							auto npcFaction = npc.GetFactions();

							skip = true;
							for (auto& faction : npcFaction) {
								if (ValidateTESName(faction.faction) == secondaryFilter) {
									skip = false;
									break;
								}
							}
						}
					}
				}
			}

			if (skip) {
				continue;
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

	std::set<std::string> NPCWindow::GetSecondaryFilterList()
	{
		switch (primaryFilter) {
		case RE::FormType::Class:
			return Data::GetSingleton()->GetNPCClassList();
		case RE::FormType::Race:
			return Data::GetSingleton()->GetNPCRaceList();
		case RE::FormType::Faction:
			return Data::GetSingleton()->GetNPCFactionList();
		case RE::FormType::None:
		default:
			return {};
		}
	}

	// Draw search bar for filtering items.
	void NPCWindow::ShowSearch()
	{
		auto filterWidth = ImGui::GetContentRegionAvail().x / 8.0f;
		auto inputTextWidth = ImGui::GetContentRegionAvail().x / 1.5f - filterWidth;
		auto totalWidth = inputTextWidth + filterWidth + 2.0f;

		// Search bar for compare string.
		ImGui::Text(_TFM("GENERAL_SEARCH_RESULTS", ":"));
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

		// Class, Race, Faction filtering
		ImGui::Text(_TFM("GENERAL_FILTER_NPC_TYPE", ":"));
		ImGui::SetNextItemWidth(totalWidth);
		ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, 0.0f), ImVec2(0.0f, ImGui::GetWindowSize().y / 4));

		std::string primaryFilterName = RE::FormTypeToString(primaryFilter).data();
		if (ImGui::BeginCombo("###NPCWindow::PrimaryFilter", primaryFilterName.c_str())) {
			if (ImGui::Selectable(_T("None"), primaryFilter == RE::FormType::None)) {
				primaryFilter = RE::FormType::None;
				ImGui::SetItemDefaultFocus();
				ApplyFilters();
			}

			for (auto& filter : filterList) {
				bool isSelected = (filter == primaryFilter);

				// Class, Race, Faction
				std::string formName = RE::FormTypeToString(filter).data();
				if (ImGui::Selectable(_T(formName), isSelected)) {
					primaryFilter = filter;

					// TODO: Is it necessary to regenerate this?
					// Why not just cache it at startup..

					switch (primaryFilter) {
					case RE::FormType::Class:
						Data::GetSingleton()->GenerateNPCClassList();
						secondaryFilter = "Show All";
						break;
					case RE::FormType::Race:
						Data::GetSingleton()->GenerateNPCRaceList();
						secondaryFilter = "Show All";
						break;
					case RE::FormType::Faction:
						Data::GetSingleton()->GenerateNPCFactionList();
						secondaryFilter = "Show All";
						break;
					}

					ApplyFilters();
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		// If we selected a primary filter, show secondary filter list:
		if (primaryFilter != RE::FormType::None) {
			for (auto& filter : filterList) {
				if (primaryFilter == filter) {
					auto _list = GetSecondaryFilterList();
					std::vector<std::string> list(_list.begin(), _list.end());
					list.insert(list.begin(), "Show All");

					if (InputTextComboBox("##NPCWindow::SecondaryFilter", secondaryFilterBuffer, secondaryFilter, IM_ARRAYSIZE(secondaryFilterBuffer), list, totalWidth)) {
						secondaryFilter = "Show All";

						if (secondaryFilter.find(secondaryFilterBuffer) != std::string::npos) {
							ImFormatString(secondaryFilterBuffer, IM_ARRAYSIZE(secondaryFilterBuffer), "");
						} else {
							for (auto& item : list) {
								if (item.empty()) {
									continue;
								}

								if (item.find(secondaryFilterBuffer) != std::string::npos) {
									secondaryFilter = item;
									ImFormatString(secondaryFilterBuffer, IM_ARRAYSIZE(secondaryFilterBuffer), "");
									break;
								}
							}
						}

						ApplyFilters();
					}
				}
			}
		}

		ImGui::NewLine();

		// Mod List sort and filter.
		const auto& config = Settings::GetSingleton()->GetConfig();
		auto modListVector = Data::GetSingleton()->GetFilteredListOfPluginNames(Data::PLUGIN_TYPE::NPC, (Data::SORT_TYPE)config.modListSort, RE::FormType::None);
		modListVector.insert(modListVector.begin(), "All Mods");
		ImGui::Text(_TFM("GENERAL_FILTER_MODLIST", ":"));
		if (InputTextComboBox("##NPCWindow::ModField", modSearchBuffer, selectedMod, IM_ARRAYSIZE(modSearchBuffer), modListVector, totalWidth)) {
			auto modList = Data::GetSingleton()->GetModulePluginList(Data::PLUGIN_TYPE::NPC);
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