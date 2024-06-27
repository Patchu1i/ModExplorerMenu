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
			if (selectedMod != "All Mods" && item.GetPluginName() != selectedMod)  // inactive mods
				continue;

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

			// Will probably need to revisit this during localization. :(
			std::transform(compare.begin(), compare.end(), compare.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

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
		(void)a_config;

		if (ImGui::CollapsingHeader("Refine your search:", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::NewLine();
			ImGui::Indent();

			if (ImGui::InputTextWithHint("##ObjectWindow::InputField", "Enter text to filter results by...", inputBuffer,
					IM_ARRAYSIZE(inputBuffer),
					ImGuiInputTextFlags_EscapeClearsAll)) {
				ApplyFilters();
			}

			ImGui::SameLine();

			auto searchByValue = InputSearchMap.at(searchKey);
			auto combo_flags = ImGuiComboFlags_WidthFitPreview;
			if (ImGui::BeginCombo("##ObjectWindow::InputFilter", searchByValue, combo_flags)) {
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

			ImGui::Checkbox("Click to Place", &b_clickToPlace);
			ImGui::SameLine();

			ImGui::NewLine();

			if (ImGui::BeginCombo("##ObjectWindow::FilterByMod", selectedMod.c_str())) {
				if (ImGui::Selectable("All Mods", selectedMod == "All Mods")) {
					selectedMod = "All Mods";
					ApplyFilters();
					ImGui::SetItemDefaultFocus();
				}
				for (auto& mod : Data::GetModList()) {
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

			ImGui::NewLine();

			ImGui::Unindent();
			ImGui::NewLine();
		}
	}

	void ObjectWindow::ShowAdvancedOptions(Settings::Style& a_style, Settings::Config& a_config)
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
				ImGui::InlineCheckbox("Favorite", &a_config.soShowFavoriteColumn);
				ImGui::InlineCheckbox("Plugin", &a_config.soShowPluginColumn);
				ImGui::InlineCheckbox("Type", &a_config.soShowTypeColumn);
				ImGui::InlineCheckbox("FormID", &a_config.soShowFormIDColumn);
				ImGui::NewLine();
				ImGui::InlineCheckbox("Editor ID", &a_config.soShowEditorIDColumn);
				ImGui::NewLine();
				ImGui::PopStyleColor(1);
				ImGui::PopStyleVar(1);
				ImGui::TreePop();
			}

			ImGui::PopStyleColor(1);
			ImGui::PopStyleVar(1);
			ImGui::Unindent();
			ImGui::NewLine();
		}
	}
}