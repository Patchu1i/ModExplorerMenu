#include "Window.h"

#include "Utils/Util.h"
#include "Window.h"

void NPCWindow::ApplyFilters()
{
	npcList.clear();

	auto& cached_item_list = MEMData::GetNPCList();

	char compare[256];
	char input[256];

	strncpy(input, inputBuffer, sizeof(input) - 1);

	bool skip = false;

	// TODO: Implement additional columns
	for (auto& item : cached_item_list) {
		switch (searchKey) {
		case Name:
			strncpy(compare, item.name.c_str(), sizeof(compare) - 1);
			break;
		case FormID:
			strncpy(compare, item.formid.c_str(), sizeof(compare) - 1);
			break;
		case EditorID:
			strncpy(compare, item.editorid.c_str(), sizeof(compare) - 1);
			break;
		case None:
			skip = true;
			break;
		default:
			strncpy(compare, item.name.c_str(), sizeof(compare) - 1);
			break;
		}

		if (selectedMod != "All Mods" && item.plugin != selectedMod)  // inactive mods
			continue;

		if (item.name == "")  // skip empty names
			continue;

		auto lower_compare = strlwr(compare);

		if (strstr(lower_compare, input) != nullptr) {
			npcList.push_back(&item);
			continue;
		}

		if (skip) {
			char _name[256];
			char _editorid[256];
			char _formid[256];

			strcpy(_name, item.name.c_str());
			strcpy(_editorid, item.editorid.c_str());
			strcpy(_formid, item.formid.c_str());

			strlwr(_name);
			strlwr(_editorid);
			strlwr(_formid);

			if (strstr(_name, input) != nullptr) {
				npcList.push_back(&item);
			} else if (strstr(_editorid, input) != nullptr) {
				npcList.push_back(&item);
			} else if (strstr(_formid, input) != nullptr) {
				npcList.push_back(&item);
			}

			continue;
		}
	}

	dirty = true;
}

// Draw search bar for filtering items.
void NPCWindow::ShowSearch(Settings::Style& a_style, Settings::Config& a_config)
{
	(void)a_style;
	(void)a_config;

	if (ImGui::CollapsingHeader("Refine your search:", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::NewLine();
		ImGui::Indent();

		if (ImGui::InputTextWithHint("##AddItemWindow::InputField", "Enter text to filter results by...", inputBuffer,
				IM_ARRAYSIZE(inputBuffer),
				ImGuiInputTextFlags_EscapeClearsAll)) {
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

		if (ImGui::Checkbox("Show Local NPCs Only", &showLocalsOnly)) {
			PopulateListWithLocals();
		}

		ImGui::Unindent();
		ImGui::NewLine();
	}
}

void NPCWindow::ShowModSelection(Settings::Style& a_style, Settings::Config& a_config)
{
	(void)a_style;
	(void)a_config;

	if (ImGui::CollapsingHeader("Filter by Mod:")) {
		ImGui::NewLine();
		ImGui::Indent();
		if (ImGui::BeginCombo("##AddItemWindow::FilterByMod", selectedMod.c_str())) {
			if (ImGui::Selectable("All Mods", selectedMod == "All Mods")) {
				selectedMod = "All Mods";
				ApplyFilters();
				ImGui::SetItemDefaultFocus();
			}
			for (auto& mod : MEMData::GetModList()) {
				const char* modName = mod->GetFilename().data();
				bool is_selected = false;
				if (ImGui::Selectable(modName, is_selected)) {
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

void InlineCheckboxConfig(const char* label, bool* v)
{
	if (ImGui::InlineCheckbox(label, v)) {
		Settings::GetSingleton()->SaveSettings();
		logger::info("Saved settings.");
	}
}

void NPCWindow::ShowAdvancedOptions(Settings::Style& a_style, Settings::Config& a_config)
{
	(void)a_style;
	(void)a_config;

	if (ImGui::CollapsingHeader("Advanced Options:")) {
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::NewLine();
		ImGui::Indent();
		if (ImGui::TreeNode("Column Visiblity:")) {
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, a_style.frameBorderSize);
			ImGui::PushStyleColor(ImGuiCol_Header, a_style.frameBg);
			InlineCheckboxConfig("Favorite", &a_config.npcShowFavoriteColumn);
			InlineCheckboxConfig("FormID", &a_config.npcShowFormIDColumn);
			InlineCheckboxConfig("Plugin", &a_config.npcShowPluginColumn);
			InlineCheckboxConfig("Name", &a_config.npcShowNameColumn);
			ImGui::NewLine();
			InlineCheckboxConfig("EditorID", &a_config.npcShowEditorIDColumn);
			InlineCheckboxConfig("Health", &a_config.npcShowHealthColumn);
			InlineCheckboxConfig("Magicka", &a_config.npcShowMagickaColumn);
			InlineCheckboxConfig("Stamina", &a_config.npcShowStaminaColumn);
			ImGui::NewLine();
			InlineCheckboxConfig("Carry Weight", &a_config.npcShowCarryWeightColumn);
			ImGui::NewLine();
			ImGui::PopStyleColor(1);
			ImGui::PopStyleVar(1);
			ImGui::TreePop();
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