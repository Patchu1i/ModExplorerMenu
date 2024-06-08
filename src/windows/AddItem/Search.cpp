#include "Utils/Util.h"
#include "Window.h"

void AddItemWindow::ApplyFilters()
{
	itemList.clear();

	auto& cached_item_list = MEMData::GetItemList();

	char compare[256];
	char input[256];

	strncpy(input, inputBuffer, sizeof(input) - 1);

	bool skip = false;

	// TODO: Implement additional columns
	for (auto& item : cached_item_list) {
		switch (searchKey) {
		case ColumnID_Name:
			strncpy(compare, item.name, sizeof(compare) - 1);
			break;
		case ColumnID_FormID:
			strncpy(compare, item.formid.c_str(), sizeof(compare) - 1);
			break;
		case ColumnID_EditorID:
			strncpy(compare, item.editorid.c_str(), sizeof(compare) - 1);
			break;
		case ColumnID_None:
			skip = true;
			break;
		default:
			strncpy(compare, item.name, sizeof(compare) - 1);
			break;
		}

		if (selectedMod != nullptr && item.mod != selectedMod)  // inactive mods
			continue;

		if (itemFilters.count(item.formType) <= 0)  // non-selected filter
			continue;

		if (item.nonPlayable)  // non-useable
			continue;

		if (strcmp(item.name, "") == 0)  // skip empty names
			continue;

		auto lower_compare = strlwr(compare);

		if (strstr(lower_compare, input) != nullptr) {
			itemList.push_back(&item);
			continue;
		}

		if (skip) {
			char _name[256];
			char _editorid[256];
			char _formid[256];

			strcpy(_name, item.name);
			strcpy(_editorid, item.editorid.c_str());
			strcpy(_formid, item.formid.c_str());

			strlwr(_name);
			strlwr(_editorid);
			strlwr(_formid);

			if (strstr(_name, input) != nullptr) {
				itemList.push_back(&item);
			} else if (strstr(_editorid, input) != nullptr) {
				itemList.push_back(&item);
			} else if (strstr(_formid, input) != nullptr) {
				itemList.push_back(&item);
			}

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

		ImGui::Unindent();
		ImGui::NewLine();
	}
}

void AddItemWindow::ShowModSelection(Settings::Style& a_style, Settings::Config& a_config)
{
	(void)a_style;
	(void)a_config;

	if (ImGui::CollapsingHeader("Filter by Mod:")) {
		ImGui::NewLine();
		ImGui::Indent();
		auto combo_text = selectedMod ? selectedMod->GetFilename().data() : "Filter by mod..";
		if (ImGui::BeginCombo("##AddItemWindow::FilterByMod", combo_text)) {
			if (ImGui::Selectable("All Mods", selectedMod == nullptr)) {
				selectedMod = nullptr;
				ApplyFilters();
				ImGui::SetItemDefaultFocus();
			}
			for (auto& mod : MEMData::GetModList()) {
				const char* modName = mod->GetFilename().data();
				bool is_selected = false;
				if (ImGui::Selectable(modName, is_selected)) {
					selectedMod = mod;
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

void AddItemWindow::ShowAdvancedOptions(Settings::Style& a_style, Settings::Config& a_config)
{
	(void)a_style;
	(void)a_config;

	if (ImGui::CollapsingHeader("Advanced Options:")) {
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::NewLine();
		ImGui::Indent();
		constexpr auto flags = ImGuiTreeNodeFlags_Bullet;
		if (ImGui::CollapsingHeader("Column Visiblity:", flags)) {
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, a_style.frameBorderSize);
			ImGui::PushStyleColor(ImGuiCol_Header, a_style.frameBg);
			ImGui::InlineCheckbox("Favorite", &a_config.aimShowFavoriteColumn);
			ImGui::InlineCheckbox("Type", &a_config.aimShowTypeColumn);
			ImGui::InlineCheckbox("Form", &a_config.aimShowFormIDColumn);
			ImGui::InlineCheckbox("Name", &a_config.aimShowNameColumn);
			ImGui::NewLine();
			ImGui::InlineCheckbox("Editor", &a_config.aimShowEditorIDColumn);
			ImGui::InlineCheckbox("Gold", &a_config.aimShowGoldValueColumn);
			ImGui::InlineCheckbox("Damage", &a_config.aimShowBaseDamageColumn);
			ImGui::InlineCheckbox("Speed", &a_config.aimShowSpeedColumn);
			ImGui::NewLine();
			ImGui::InlineCheckbox("Crit", &a_config.aimShowCritDamageColumn);
			ImGui::InlineCheckbox("Skill", &a_config.aimShowSkillColumn);
			ImGui::InlineCheckbox("Weight", &a_config.aimShowWeightColumn);
			ImGui::InlineCheckbox("DPS", &a_config.aimShowDPSColumn);
			ImGui::NewLine();
			ImGui::InlineCheckbox("Armor", &a_config.aimShowArmorRatingColumn);
			ImGui::NewLine();
			ImGui::PopStyleColor(1);
			ImGui::PopStyleVar(1);
		}

		// DEPRECATED:: Moved to Plot window.
		// if (ImGui::CollapsingHeader("(Beta) Plot & Histogram:", flags)) {
		// 	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, a_style.frameBorderSize);
		// 	ImGui::PushStyleColor(ImGuiCol_Header, a_style.frameBg);
		// 	const ImVec2 button_size = ImVec2(ImGui::GetContentRegionAvail().x / 3, ImGui::GetFontSize() * 1.15f);

		// 	ImGui::HelpMarker(
		// 		"Regenerate the list of cached forms from the game. This should only ever be needed if runtime changes are made.\n\n"
		// 		"(WARNING): This will take a second or two to complete and will freeze your game in doing so.");
		// 	if (ImGui::Button("Regenerate Cache", button_size)) {
		// 		MEMData::GetSingleton()->Run();
		// 		ApplyFilters();
		// 	}
		// 	ImGui::PopStyleColor(1);
		// 	ImGui::PopStyleVar(1);
		// }

		ImGui::PopStyleColor(1);
		ImGui::PopStyleVar(1);
		ImGui::Unindent();
		ImGui::NewLine();
	}
}