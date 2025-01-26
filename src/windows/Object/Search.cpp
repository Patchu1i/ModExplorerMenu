#include "include/O/Object.h"
#include "include/P/Persistent.h"

namespace Modex
{
	// Populate list with NPCs according to filters & search results.
	// Will set state to `showAll` and delete any existing CachedNPC objects.
	// TODO: Find alternatives to C style comparison and copying.
	void ObjectWindow::ApplyFilters()
	{
		objectList.clear();
		selectedObject = nullptr;

		auto& cachedObjectList = Data::GetSingleton()->GetObjectList();

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

			if (selectedMod != "All Mods" && obj.GetPluginName() != selectedMod) {
				continue;
			}

			// Ensure Object's from Blacklisted Plugins aren't shown.
			if (selectedMod == "All Mods") {
				if (PersistentData::GetSingleton()->m_blacklist.contains(obj.GetPlugin())) {
					continue;
				}
			}

			if (primaryFilter != RE::FormType::None) {
				if (obj.GetFormType() != primaryFilter) {
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
		itemSelectionList.clear();
		ApplyFilters();
	}

	// Draw search bar for filtering items.
	void ObjectWindow::ShowSearch()
	{
		auto filterWidth = ImGui::GetContentRegionAvail().x / 8.0f;
		auto inputTextWidth = ImGui::GetContentRegionAvail().x / 1.5f - filterWidth;
		auto totalWidth = inputTextWidth + filterWidth + 2.0f;

		// General search bar for filtering items.
		ImGui::Text(_TFM("GENERAL_SEARCH_RESULTS", ":"));
		ImGui::SetNextItemWidth(inputTextWidth);
		if (ImGui::InputTextWithHint("##ObjectWindow::InputField", _T("GENERAL_CLICK_TO_TYPE"), inputBuffer,
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

		// Secondary Filter for object record types.
		ImGui::Text(_TFM("GENERAL_FILTER_ITEM_TYPE", ":"));
		ImGui::SetNextItemWidth(totalWidth);
		auto filterName = RE::FormTypeToString(primaryFilter).data();
		if (ImGui::BeginCombo("##ObjectWindow::FilterByType", _T(filterName))) {
			if (ImGui::Selectable(_T("None"), primaryFilter == RE::FormType::None)) {
				primaryFilter = RE::FormType::None;
				Refresh();
				ImGui::SetItemDefaultFocus();
			}

			for (auto& filter : filterList) {
				bool isSelected = (filter == primaryFilter);

				std::string formName = RE::FormTypeToString(filter).data();
				if (ImGui::Selectable(_T(formName), isSelected)) {
					primaryFilter = filter;
					Refresh();
				}

				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::NewLine();

		// Plugin List Filter
		const auto& config = Settings::GetSingleton()->GetConfig();
		auto modListVector = Data::GetSingleton()->GetFilteredListOfPluginNames(Data::PLUGIN_TYPE::OBJECT, (Data::SORT_TYPE)config.modListSort, primaryFilter);
		modListVector.insert(modListVector.begin(), "All Mods");
		ImGui::Text(_TFM("GENERAL_FILTER_MODLIST", ":"));
		if (InputTextComboBox("##ObjectWindow::ModField", modSearchBuffer, selectedMod, IM_ARRAYSIZE(modSearchBuffer), modListVector, totalWidth)) {
			auto modList = Data::GetSingleton()->GetModulePluginList(Data::PLUGIN_TYPE::OBJECT);
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