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

		auto& cachedNPCList = Data::GetNPCList();

		std::string compareString;
		std::string inputString = inputBuffer;
		std::transform(inputString.begin(), inputString.end(), inputString.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

		// TODO: Implement additional columns
		for (auto& npc : cachedNPCList) {
			switch (searchKey) {
			case BaseColumn::ID::Name:
				compareString = npc.GetName();
				break;
			case BaseColumn::ID::FormID:
				compareString = npc.GetFormID();
				break;
			case BaseColumn::ID::EditorID:
				compareString = npc.GetEditorID();
				break;
			default:
				compareString = npc.GetName();
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

			if (npc.GetName() == "")
				continue;

			if (compareString.find(inputString) != std::string::npos) {
				npcList.push_back(&npc);
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

				auto modFormTypeMap = Data::GetModFormTypeMap();
				for (auto& mod : Data::GetModList(Data::NPC_MOD_LIST, a_config.modListSort)) {
					const char* modName = mod->GetFilename().data();
					bool bSelected = false;

					auto match = false;
					for (auto& modMap : modFormTypeMap) {
						if (mod == modMap.first && modMap.second.npc) {
							match = true;
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