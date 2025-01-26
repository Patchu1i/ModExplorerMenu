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

			if (item.IsNonPlayable())  // TODO: Doesn't do what expected.
				continue;

			if (selectedMod != "All Mods" && item.GetPluginName() != selectedMod) {
				continue;
			}

			// Ensure Items from Blacklisted Plugins aren't shown.
			if (selectedMod == "All Mods") {
				if (PersistentData::GetSingleton()->m_blacklist.contains(item.GetPlugin())) {
					continue;
				}
			}

			// Primary Filter (Record Type)
			if (primaryFilter != RE::FormType::None) {
				if (item.GetFormType() != primaryFilter) {
					continue;
				}
			}

			// Secondary Filter
			// Used for Armor Slots exclusively for now.
			if (secondaryFilter != "All") {
				if (primaryFilter == RE::FormType::Armor && item.GetFormType() == RE::FormType::Armor) {
					auto armorSlots = Utils::GetArmorSlots(item.GetForm()->As<RE::TESObjectARMO>());
					if (std::find(armorSlots.begin(), armorSlots.end(), secondaryFilter) == armorSlots.end()) {
						continue;
					}
				}

				if (primaryFilter == RE::FormType::Weapon && item.GetFormType() == RE::FormType::Weapon) {
					auto weaponType = item.GetForm()->As<RE::TESObjectWEAP>()->GetWeaponType();

					const char* weaponTypes[] = {
						"Hand to Hand",
						"One Handed Sword",
						"One Handed Dagger",
						"One Handed Axe",
						"One Handed Mace",
						"Two Handed Greatsword",
						"Two Handed Battleaxe",
						"Bow",
						"Staff",
						"Crossbow"
					};

					if (weaponTypes[static_cast<int>(weaponType)] != secondaryFilter) {
						continue;
					}
				}
			}

			// Input Fuzzy Search
			if (compareString.find(inputString) != std::string::npos) {
				itemList.push_back(&item);
				continue;
			}

			dirty = true;
		}
	}

	void AddItemWindow::Refresh()
	{
		itemSelectionList.clear();
		ApplyFilters();
	}

	// Draw search bar for filtering items.
	void AddItemWindow::ShowSearch()
	{
		auto filterWidth = ImGui::GetContentRegionAvail().x / 8.0f;
		auto inputTextWidth = ImGui::GetContentRegionAvail().x / 1.5f - filterWidth;
		auto totalWidth = inputTextWidth + filterWidth + 2.0f;

		// Search bar for compare string.
		ImGui::Text(_TFM("GENERAL_SEARCH_RESULTS", ":"));
		ImGui::SetNextItemWidth(inputTextWidth);
		if (ImGui::InputTextWithHint("##AddItemWindow::InputField", _T("GENERAL_CLICK_TO_TYPE"), inputBuffer,
				IM_ARRAYSIZE(inputBuffer),
				Frame::INPUT_FLAGS)) {
			Refresh();
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
					Refresh();
				}

				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::NewLine();

		// Secondary Record Type filters
		ImGui::Text(_TFM("GENERAL_FILTER_ITEM_TYPE", ":"));
		ImGui::SetNextItemWidth(totalWidth);

		std::string filterName = RE::FormTypeToString(primaryFilter).data();
		if (ImGui::BeginCombo("##AddItemWindow::FilterByType", _T(filterName), ImGuiComboFlags_HeightLarge)) {
			if (ImGui::Selectable(_T("None"), primaryFilter == RE::FormType::None)) {
				primaryFilter = RE::FormType::None;
				secondaryFilter = "All";
				Refresh();
				ImGui::SetItemDefaultFocus();
			}

			for (auto& filter : filterList) {
				bool isSelected = (filter == primaryFilter);

				std::string formName = RE::FormTypeToString(filter).data();
				if (ImGui::Selectable(_T(formName), isSelected)) {
					primaryFilter = filter;
					secondaryFilter = "All";
					Refresh();
				}

				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		// TODO: Localization Fix
		if (primaryFilter == RE::FormType::Armor) {
			ImGui::SetNextItemWidth(totalWidth);
			if (ImGui::BeginCombo("##AddItemWindow::SecondaryFilter", secondaryFilter.c_str(), ImGuiComboFlags_HeightLarge)) {
				if (ImGui::Selectable(_T("All"), secondaryFilter.empty())) {
					secondaryFilter = "All";
					Refresh();
					ImGui::SetItemDefaultFocus();
				}

				const auto armorSlots = Utils::GetArmorSlotList();

				for (auto& slot : armorSlots) {
					bool isSelected = (slot == secondaryFilter);

					if (ImGui::Selectable(_T(slot), isSelected)) {
						secondaryFilter = slot;
						Refresh();
					}

					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}
		}

		if (primaryFilter == RE::FormType::Weapon) {
			ImGui::SetNextItemWidth(totalWidth);
			if (ImGui::BeginCombo("##AddItemWindow::SecondaryFilter", secondaryFilter.c_str(), ImGuiComboFlags_HeightLarge)) {
				if (ImGui::Selectable(_T("All"), secondaryFilter.empty())) {
					secondaryFilter = "All";
					Refresh();
					ImGui::SetItemDefaultFocus();
				}

				// TODO: Refactor and include this with ItemPreview reference.
				const char* weaponTypes[] = {
					"Hand to Hand",
					"One Handed Sword",
					"One Handed Dagger",
					"One Handed Axe",
					"One Handed Mace",
					"Two Handed Greatsword",
					"Two Handed Battleaxe",
					"Bow",
					"Staff",
					"Crossbow"
				};

				for (auto& type : weaponTypes) {
					bool isSelected = (type == secondaryFilter);

					if (ImGui::Selectable(_T(type), isSelected)) {
						secondaryFilter = type;
						Refresh();
					}

					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}
		}

		ImGui::NewLine();

		// Mod List sort and filter.
		const auto& config = Settings::GetSingleton()->GetConfig();
		auto modListVector = Data::GetSingleton()->GetFilteredListOfPluginNames(Data::PLUGIN_TYPE::ITEM, (Data::SORT_TYPE)config.modListSort, primaryFilter);
		modListVector.insert(modListVector.begin(), "All Mods");
		ImGui::Text(_TFM("GENERAL_FILTER_MODLIST", ":"));
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

			Refresh();
		}
	}
}