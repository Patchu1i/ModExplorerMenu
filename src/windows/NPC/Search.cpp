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
		// Since these are manually allocated, we need to delete them.
		if (GetState() == showSpawned || GetState() == showLocal) {
			for (auto& npc : npcList) {
				delete npc;
			}
		}

		SetState(showAll);
		npcList.clear();
		selectedNPC = nullptr;

		auto& cached_item_list = Data::GetNPCList();

		std::string compare;
		std::string input = inputBuffer;
		std::transform(input.begin(), input.end(), input.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

		// TODO: Implement additional columns
		for (auto& item : cached_item_list) {
			if (selectedMod != "All Mods" && item.GetPluginName() != selectedMod)  // inactive mods
				continue;

			if (item.GetName() == "")  // skip empty names
				continue;

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

			// Will probably need to revisit this during localization. :(
			std::transform(compare.begin(), compare.end(), compare.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

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

			ImGui::Checkbox("Click to Place", &b_clickToPlace);
			ImGui::SameLine();
			ImGui::Checkbox("Place Frozen", &b_placeFrozen);
			ImGui::SameLine();
			ImGui::Checkbox("Place Naked", &b_placeNaked);

			ImGui::NewLine();

			if (ImGui::BeginCombo("##AddItemWindow::FilterByMod", selectedMod.c_str())) {
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

			auto spawnAllNPCS = []() {
				for (auto& npc : npcList) {
					Console::PlaceAtMe(npc->GetFormID(), 1);
					Console::PridLast();

					if (b_placeFrozen)
						Console::Freeze();

					if (b_placeNaked)
						Console::UnEquip();
				}

				Console::StartProcessThread();
			};

			if (ImGui::Button("Place All NPCs from Selected Mod")) {
				if (selectedMod != "All Mods") {
					if (npcList.size() > 50) {
						ImGui::OpenPopup("Large Query Detected");
					} else {
						spawnAllNPCS();
					}
				}
			}
			ImGui::ShowWarningPopup("Large Query Detected", spawnAllNPCS);

			ImGui::Unindent();
			ImGui::NewLine();
		}
	}
}