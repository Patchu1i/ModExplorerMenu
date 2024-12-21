#include "Console.h"
#include "NPC.h"
#include "Utils/Util.h"

namespace ModExplorerMenu
{
	// Populate list with NPCs according to filters & search results.
	// Will set state to `showAll` and delete any existing CachedNPC objects.
	// TODO: Find alternatives to C style comparison and copying.
	void NPCWindow::ApplyFilters()
	{
		npcList.clear();
		selectedNPC = nullptr;

		auto& cached_item_list = Data::GetNPCList();

		std::string compare;
		std::string input = inputBuffer;
		std::transform(input.begin(), input.end(), input.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

		// TODO: Implement additional columns
		for (auto& item : cached_item_list) {
			switch (searchKey) {
			case BaseColumn::ID::Name:
				compare = item.GetName();
				break;
			case BaseColumn::ID::FormID:
				compare = item.GetFormID();
				break;
			case BaseColumn::ID::EditorID:
				compare = item.GetEditorID();
				break;
			default:
				compare = item.GetName();
				break;
			}

			std::transform(compare.begin(), compare.end(), compare.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

			// If the input is wrapped in quotes, we do an exact match across all parameters.
			if (!input.empty() && input.front() == '"' && input.back() == '"') {
				std::string match = input.substr(1, input.size() - 2);

				if (compare == match) {
					npcList.push_back(&item);
				}
				continue;
			}

			if (selectedMod == "Favorite" && !item.IsFavorite()) {
				continue;
			}

			if (selectedMod != "All Mods" && selectedMod != "Favorite" && item.GetPluginName() != selectedMod) {
				continue;
			}

			if (item.GetName() == "")
				continue;

			if (compare.find(input) != std::string::npos) {
				npcList.push_back(&item);
				continue;
			}
		}

		dirty = true;
	}

	// Draw search bar for filtering items.
	void NPCWindow::ShowSearch(Settings::Style& a_style, Settings::Config& a_config)
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
			if (ImGui::InputTextWithHint("##NPCWindow::InputField", _T("GENERAL_CLICK_TO_TYPE"), inputBuffer,
					IM_ARRAYSIZE(inputBuffer),
					ImGuiInputTextFlags_EscapeClearsAll)) {
				ApplyFilters();
			}

			ImGui::SameLine();

			auto searchByValue = InputSearchMap.at(searchKey);
			auto combo_flags = ImGuiComboFlags_None;
			ImGui::SetNextItemWidth(filterWidth);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5.0f);
			if (ImGui::BeginCombo("##NPCWindow::InputFilter", _T(searchByValue), combo_flags)) {
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

			ImGui::NewLine();

			ImGui::Text(_TFM("GENERAL_FILTER_MODLIST", ":"));
			ImGui::SetNextItemWidth(totalWidth);
			ImGui::InputTextWithHint("##NPCWindow::ModField", _T("GENERAL_CLICK_TO_TYPE"), modListBuffer,
				IM_ARRAYSIZE(modListBuffer),
				Frame::INPUT_FLAGS);

			auto min = ImVec2(0.0f, 0.0f);
			auto max = ImVec2(0.0f, ImGui::GetWindowSize().y / 4);
			ImGui::SetNextItemWidth(totalWidth);
			ImGui::SetNextWindowSizeConstraints(min, max);

			std::string selectedModName = selectedMod == "Favorite" ? _TICON(ICON_RPG_HEART, selectedMod) :
			                              selectedMod == "All Mods" ? _TICON(ICON_RPG_WRENCH, selectedMod) :
			                                                          selectedMod;

			if (ImGui::BeginCombo("##NPCWindow::FilterByMod", selectedModName.c_str())) {
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

				for (auto& mod : Data::GetModList(Data::NPC_MOD_LIST, a_config.modListSort)) {
					const char* modName = mod->GetFilename().data();
					bool is_selected = false;

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