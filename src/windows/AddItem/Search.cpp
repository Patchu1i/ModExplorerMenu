#include "AddItem.h"
#include "Console.h"
#include "Utils/Util.h"

namespace ModExplorerMenu
{
	void AddItemWindow::ApplyFilters()
	{
		itemList.clear();

		auto& cached_item_list = Data::GetItemList();

		std::string compare;
		std::string input = inputBuffer;
		std::transform(input.begin(), input.end(), input.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

		// TODO: Implement additional columns
		for (auto& item : cached_item_list) {
			if (selectedMod != "All Mods" && item.GetPluginName() != selectedMod)  // inactive mods
				continue;

			if (item.GetName() == "")
				continue;

			if (item.IsNonPlayable())  // non-useable
				continue;

			if (itemFilters.size() > 0) {
				if (itemFilters.find(item.GetFormType()) == itemFilters.end()) {
					continue;
				}
			}

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
				itemList.push_back(&item);
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

			ImGui::SameLine();

			const auto addAllItems = []() {
				for (auto& item : Data::GetItemList()) {
					if (item.GetPluginName() == selectedMod) {
						Console::AddItem(item.GetFormID());
					}
				}

				Console::StartProcessThread();
			};

			if (ImGui::Button("Add All From Mod")) {
				const auto items = Data::GetItemList();

				auto count = 0;
				for (auto item : items) {
					if (item.GetPluginName() == selectedMod) {
						count++;
					}
				}

				if (count > 500) {
					ImGui::OpenPopup("Large Query Detected");
				} else {
					addAllItems();
				}
			}
			ImGui::ShowWarningPopup("Large Query Detected", addAllItems);

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
				ImGui::InlineCheckbox("Plugin", &a_config.aimShowPluginColumn);
				ImGui::InlineCheckbox("Type", &a_config.aimShowTypeColumn);
				ImGui::InlineCheckbox("Form", &a_config.aimShowFormIDColumn);
				ImGui::NewLine();
				ImGui::InlineCheckbox("Name", &a_config.aimShowNameColumn);
				ImGui::InlineCheckbox("Editor", &a_config.aimShowEditorIDColumn);
				ImGui::InlineCheckbox("Gold", &a_config.aimShowGoldValueColumn);
				ImGui::InlineCheckbox("Damage", &a_config.aimShowBaseDamageColumn);
				ImGui::NewLine();
				ImGui::InlineCheckbox("Speed", &a_config.aimShowSpeedColumn);
				ImGui::InlineCheckbox("Crit", &a_config.aimShowCritDamageColumn);
				ImGui::InlineCheckbox("Skill", &a_config.aimShowSkillColumn);
				ImGui::InlineCheckbox("Weight", &a_config.aimShowWeightColumn);
				ImGui::NewLine();
				ImGui::InlineCheckbox("DPS", &a_config.aimShowDPSColumn);
				ImGui::InlineCheckbox("Armor", &a_config.aimShowArmorRatingColumn);
				ImGui::NewLine();
				ImGui::PopStyleColor(1);
				ImGui::PopStyleVar(1);
			}

			ImGui::PopStyleColor(1);
			ImGui::PopStyleVar(1);
			ImGui::Unindent();
			ImGui::NewLine();
		}
	}
}