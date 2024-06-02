#include "AddItemWindow.h"
#include "Console.h"
#include "Settings.h"
#include "lib/Graphic.h"
#include "lib/Util.h"

// Main Draw function for AddItem, called by Frame::Draw()
void AddItemWindow::Draw(Settings::Style& a_style, Settings::Config& a_config)
{
	const auto _flags = ImGuiOldColumnFlags_NoResize;
	ImGui::BeginColumns("##HorizontalSplit", 2, _flags);

	const float width = ImGui::GetWindowWidth();
	ImGui::SetColumnWidth(0, width * 0.75f);

	// Left Column
	Draw_InputSearch();
	ImGui::NewLine();

	bool _change = false;

	// Filter checkboxes up top.
	for (auto& item : AddItemWindow::filterMap) {
		auto first = std::get<0>(item);
		auto third = std::get<2>(item);

		ImGui::SameLine();
		if (ImGui::Checkbox(third.c_str(), first)) {
			_change = true;
		}
	}

	if (_change) {
		itemFilters.clear();

		for (auto& item : filterMap) {
			auto first = *std::get<0>(item);
			auto second = std::get<1>(item);

			if (first) {
				itemFilters.insert(second);
			}
		}

		ApplyFilters();
		dirty = true;
	}

	ImGui::NewLine();
	ShowOptions(a_style, a_config);
	ImGui::NewLine();
	ShowFormTable(a_style, a_config);
	ImGui::NextColumn();
	ShowActions(a_style, a_config);
	ImGui::EndColumns();
}

// Draws a Copy to Clipboard button on Context popup.
void AddItemWindow::Context_CopyOnly(const char* form, const char* name, const char* editor)
{
	auto& style = Settings::GetSingleton()->GetStyle();

	ImVec2 buttonSize = ImVec2(ImGui::GetContentRegionAvail().x, 30.0f);
	ImGui::PushFont(style.buttonFont);
	if (ImGui::Button("Copy Form ID to Clipboard", buttonSize)) {
		ImGui::LogToClipboard();
		ImGui::LogText(form);
		ImGui::LogFinish();
		ImGui::CloseCurrentPopup();
	}

	if (ImGui::Button("Copy Name to Clipboard", buttonSize)) {
		ImGui::LogToClipboard();
		ImGui::LogText(name);
		ImGui::LogFinish();
		ImGui::CloseCurrentPopup();
	}

	if (ImGui::Button("Copy Editor ID to Clipboard", buttonSize)) {
		ImGui::LogToClipboard();
		ImGui::LogText(editor);
		ImGui::LogFinish();
		ImGui::CloseCurrentPopup();
	}
	ImGui::PopFont();
}

bool AddItemWindow::SortColumn(const MEMData::CachedItem* v1, const MEMData::CachedItem* v2)
{
	const ImGuiTableSortSpecs* sort_specs = s_current_sort_specs;
	const ImGuiID ID = sort_specs->Specs->ColumnUserID;

	int delta = 0;

	switch (ID) {
	// Delta must be +1 or -1 to indicate move. Otherwise comparitor is invalid.
	case ColumnID_Favorite:  // bool
		delta = (v1->favorite < v2->favorite) ? -1 : (v1->favorite > v2->favorite) ? 1 :
		                                                                             0;
		break;
	case ColumnID_Type:  // const char *
		delta = strcmp(v1->typeName.c_str(), v2->typeName.c_str());
		break;
	case ColumnID_FormID:  // std::string
		delta = strcmp(v1->formid.c_str(), v2->formid.c_str());
		break;
	case ColumnID_Name:  // const char *
		delta = strcmp(v1->name, v2->name);
		break;
	case ColumnID_EditorID:  // std::string
		delta = strcmp(v1->editorid.c_str(), v2->editorid.c_str());
		break;
	case ColumnID_GoldValue:  // std::int32_t
		delta = (v1->goldValue < v2->goldValue) ? -1 : (v1->goldValue > v2->goldValue) ? 1 :
		                                                                                 0;
	default:
		break;
	}

	if (delta > 0)
		return (sort_specs->Specs->SortDirection == ImGuiSortDirection_Ascending) ? true : false;
	if (delta < 0)
		return (sort_specs->Specs->SortDirection == ImGuiSortDirection_Ascending) ? false : true;

	return false;  // prevent assertion (?)
}

// Sorts the columns of referenced list by sort_specs
void AddItemWindow::SortColumnsWithSpecs(ImGuiTableSortSpecs* sort_specs)
{
	s_current_sort_specs = sort_specs;
	if (itemList.size() > 1)
		std::sort(itemList.begin(), itemList.end(), SortColumn);
	s_current_sort_specs = NULL;
}

static inline const float center_text_x(const char* text)
{
	return ImGui::GetCursorPosX() + ImGui::GetColumnWidth() / 2 -
	       ImGui::CalcTextSize(text).x / 2;
};

void AddItemWindow::ShowItemCard(MEMData::CachedItem* item)
{
	ImGui::BeginTooltip();

	ImVec2 barSize = ImVec2(100.0f, ImGui::GetFontSize());
	float popWidth = barSize.x * 3;

	auto ProgressColor = [](double value, float max_value) -> ImVec4 {
		float ratio = (float)value / max_value;
		float r = 1.0f - ratio;
		float g = ratio;
		return ImVec4(r, g, 0.0f, 1.0f);
	};

	auto InlineBar = [popWidth, barSize, ProgressColor](const char* label, float value, float max_value) {
		ImGui::Text(label);
		ImGui::SameLine();
		ImGui::InvisibleButton("##AddItemWindow::InlineBar", barSize);
		ImGui::SameLine(popWidth - barSize.x);
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ProgressColor(value, max_value));
		float curr = static_cast<float>(value);
		char buffer[256];
		sprintf(buffer, "%.2f", value);
		ImGui::ProgressBar(curr / max_value, barSize, buffer);

		ImGui::PopStyleColor(1);
	};

	auto InlineInt = [popWidth, barSize](const char* label, int value) {
		ImGui::Text(label);
		ImGui::SameLine(popWidth - ImGui::CalcTextSize(std::to_string(value).c_str()).x);
		ImGui::Text("%d", value);
	};

	auto InlineText = [popWidth](const char* label, const char* text) {
		ImGui::Text(label);
		ImGui::SameLine(popWidth - ImGui::CalcTextSize(text).x);
		ImGui::Text(text);
	};

	if (item == nullptr) {
		ImGui::EndTooltip();
		return;
	}

	// Header
	ImGui::SetCursorPosX(center_text_x(item->name));
	ImGui::Text(item->name);
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

	if (item->formType == RE::FormType::Armor) {
		auto* armor = item->form->As<RE::TESObjectARMO>();

		if (armor == nullptr) {
			ImGui::EndTooltip();
			return;
		}

		auto armorType = Utils::GetArmorType(armor);
		auto armorRating = armor->GetArmorRating();
		auto equipSlot = Utils::GetArmorSlot(armor);
		//auto armorValue = armor->value;
		auto weight = armor->GetWeight();
		//auto enchantments = armor->formEnchanting->effects;

		InlineBar("Armor Rating:", (float)armorRating, 100.0f);
		//InlineBar("Armor Value:", (float)armorValue, 100.0f);
		InlineText("Armor Type:", armorType);
		InlineText("Equip Slot:", equipSlot);
		InlineInt("Weight:", (int)weight);

		//auto enchantmentCharge = armor->GetEnchantmentCharge();
		//auto enchantmentCost = armor->GetEnchantmentCost();
		//auto enchantmentType = armor->GetEnchantmentType();
		//auto enchantmentAmount = armor->GetEnchantmentAmount();
		//auto enchantmentDuration = armor->GetEnchantmentDuration();
		//auto enchantmentArea = armor->GetEnchantmentArea();
		//auto enchantmentActorValue = armor->GetEnchantmentActorValue();
		//auto enchantmentSkill = armor->GetEnchantmentSkill();
		//auto enchantmentMagicEffect = armor->GetEnchantmentMagicEffect();
		//auto enchantmentCondition = armor->GetEnchantmentCondition
	}

	if (item->formType == RE::FormType::Weapon) {
		const char* weaponTypes[] = {
			"Hand to Hand",
			"One Hand Sword",
			"One Hand Dagger",
			"One Hand Axe",
			"One Hand Mace",
			"Two Hand Sword",
			"Two Hand Axe",
			"Bow",
			"Staff",
			"Crossbow"
		};

		auto* weapon = item->form->As<RE::TESObjectWEAP>();

		if (weapon == nullptr) {
			ImGui::EndTooltip();
			return;
		}

		//auto desc = weapon->GetDescription();
		//auto critChance = weapon->GetCrit;
		//uint16_t _dmg = weapon->attackDamage;  // seems like base damage
		//auto _newdmg = weapon->GetAttackDamage();

		//--
		double damage = Utils::CalcBaseDamage(weapon);
		float max_damage = Utils::CalcMaxDamage(damage, 50);
		double reach = weapon->weaponData.reach;                                            // + and - 1 by a little.
		float speed = weapon->weaponData.speed;                                             // 0 - 2?
		uint16_t damage_other = weapon->GetCritDamage();                                    // 1-100x multiplier?
		SKSE::stl::enumeration<RE::ActorValue, uint32_t> skill = weapon->weaponData.skill;  // see below
		float stagger = weapon->weaponData.staggerValue;                                    // 0 - 2 with 1 being median
		auto type = weaponTypes[static_cast<int>(weapon->GetWeaponType())];

		//bool is_bound = weapon->IsBound();
		//bool non_playable = weapon->weaponData.flags.any(RE::TESObjectWEAP::Data::Flag::kNonPlayable);

		InlineBar("Base Damage:", (float)damage, max_damage);
		InlineBar("Reach:", (float)reach, 1.5f);
		InlineBar("Speed:", (float)speed, 2.0f);
		InlineBar("stagger:", stagger, 2.0f);
		InlineInt("Critical Damage:", damage_other);
		InlineText("skill:", std::to_string(skill.get()).c_str());
		InlineText("Type:", type);
		//ImGui::Text("non_playable: %s", (non_playable) ? "true" : "false");
		//ImGui::Text("is_bound: %s", (is_bound) ? "true" : "false");
	}

	// Always show:
	InlineInt("Gold Value:", item->goldValue);

	ImGui::EndTooltip();

	// float maxRange = weapon->weaponData.maxRange;                                       // bow only
	// float minRange = weapon->weaponData.minRange;                                       // bow only
}

// Draw the table of items
void AddItemWindow::ShowFormTable(Settings::Style& a_style, Settings::Config& a_config)
{
	auto results = std::string("Results (") + std::to_string(itemList.size()) + std::string(")");
	ImGui::SeparatorText(results.c_str());

	// TODO: Add RowBG as theme option
	const auto table_flags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Sortable |
	                         ImGuiTableFlags_Borders | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Hideable |
	                         ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_NoBordersInBody |
	                         ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;

	const auto sizing = ImGuiTableFlags_SizingStretchProp;

	int columns_disabled = 0;
	for (auto enabled : column_toggle) {
		if (!enabled) {
			columns_disabled++;
		}
	}
	if (columns_disabled == column_count) {
		ImGui::Text("No columns are enabled. Please enable at least one column.");
		return;
	}

	const auto table_size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
	if (ImGui::BeginTable("AddItemWindow::Table", column_count, table_flags | sizing, table_size)) {
		ImGui::TableSetupScrollFreeze(1, 1);
		ImGui::TableSetupColumn(" ", ImGuiTableColumnFlags_WidthFixed, 16.0f, ColumnID_Favorite);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 65.0f, ColumnID_Type);
		ImGui::TableSetupColumn("Form ID", ImGuiTableColumnFlags_WidthFixed, 100.0f, ColumnID_FormID);
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort, 16.0f, ColumnID_Name);
		ImGui::TableSetupColumn("Editor ID", ImGuiTableColumnFlags_None, 16.0f, ColumnID_EditorID);
		ImGui::TableSetupColumn("Gold", ImGuiTableColumnFlags_WidthFixed, 65.0f, ColumnID_GoldValue);

		ImGui::PushFont(a_style.headerFont);
		ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
		for (int column = 0; column < column_count; column++) {
			ImGui::TableSetColumnIndex(column);
			const char* column_name = ImGui::TableGetColumnName(column);  // Retrieve name passed to TableSetupColumn()
			ImGui::PushID(column);
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			if (column != ColumnID_FormID && column != ColumnID_EditorID)
				ImGui::SetCursorPosX(center_text_x(column_name));

			ImGui::TableHeader(column_name);

			//ImGui::TableSetColumnEnabled(column, column_toggle[column]);
			ImGui::PopID();
		}
		ImGui::PopFont();

		ImGui::TableSetColumnEnabled(ColumnID_Favorite, a_config.aimShowFavoriteColumn);
		ImGui::TableSetColumnEnabled(ColumnID_Type, a_config.aimShowTypeColumn);
		ImGui::TableSetColumnEnabled(ColumnID_FormID, a_config.aimShowFormIDColumn);
		ImGui::TableSetColumnEnabled(ColumnID_Name, a_config.aimShowNameColumn);
		ImGui::TableSetColumnEnabled(ColumnID_EditorID, a_config.aimShowEditorIDColumn);
		ImGui::TableSetColumnEnabled(ColumnID_GoldValue, a_config.aimShowGoldValueColumn);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 10.0f));

		if (dirty) {
			ImGui::TableGetSortSpecs()->SpecsDirty = true;
		}

		// Sort our data if sort specs have been changed!
		if (ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs()) {
			if (sort_specs->SpecsDirty) {
				SortColumnsWithSpecs(sort_specs);
				sort_specs->SpecsDirty = false;
				dirty = false;
			}
		}

		ImGuiListClipper clipper;

		int count = 0;
		clipper.Begin(static_cast<int>(itemList.size()), ImGui::GetTextLineHeightWithSpacing());
		while (clipper.Step()) {
			for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
				auto& item = itemList[row];

				count++;

				auto table_id = std::string("##AddItemMenu::TableIndex-") + std::to_string(count);

				ImGuiTableRowFlags row_flags = ImGuiTableRowFlags_None;

				ImGui::PushID(table_id.c_str());
				ImGui::TableNextRow(row_flags);
				ImGui::TableNextColumn();

				// Overwrite color to hide ugly imgui backdrop on image.
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

				ImTextureID favorite_state = item->favorite ? a_style.favoriteIconEnabled.texture : a_style.favoriteIconDisabled.texture;
				float col = item->favorite ? 1.0f : 0.5f;

				if (favorite_state != nullptr) {
					if (ImGui::ImageButton("##AddItemWindow::FavoriteButton", favorite_state, ImVec2(18.0f, 18.0f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(col, col, col, col))) {
						item->favorite = !item->favorite;
					}
				} else {
					ImGui::Checkbox("##AddItemWindow::FavoriteCheckbox", &item->favorite);
				}

				ImGui::PopStyleColor(3);
				ImGui::PopStyleVar(1);

				const ImGuiSelectableFlags select_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap;
				ImGui::TableNextColumn();
				ImGui::SetCursorPosX(center_text_x(item->typeName.c_str()));
				ImGui::Selectable(item->typeName.c_str(), &item->selected, select_flags);
				ImGui::TableNextColumn();
				ImGui::Selectable(item->formid.c_str(), &item->selected, select_flags);
				ImGui::TableNextColumn();
				ImGui::SetCursorPosX(center_text_x(item->name));
				ImGui::Selectable(item->name, &item->selected, select_flags);
				ImGui::TableNextColumn();
				ImGui::TextWrapped(item->editorid.c_str());
				ImGui::TableNextColumn();
				ImGui::SetCursorPosX(center_text_x(std::to_string(item->goldValue).c_str()));
				ImGui::Selectable(std::to_string(item->goldValue).c_str(), &item->selected, select_flags);

				auto curRow = ImGui::TableGetHoveredRow();

				if (curRow == ImGui::TableGetRowIndex()) {
					ShowItemCard(item);
				}

				//if (ImGui::BeginPopupContextItem())
				//{
				//    Context_CopyOnly(ref.formid.c_str(), ref.name, ref.editorid.c_str());
				//    ImGui::EndPopup();
				//}

				ImGui::PopID();
			}
		}

		ImGui::PopStyleVar(1);

		ImGui::EndTable();
	}
}

void AddItemWindow::ApplyFilters()
{
	itemList.clear();

	auto& cached_item_list = MEMData::GetItemList();

	char compare[256];
	char input[256];

	strcpy(input, inputBuffer);

	bool skip = false;

	// TODO: Implement additional columns
	for (auto& item : cached_item_list) {
		switch (searchKey) {
		case ColumnID_Name:
			strcpy(compare, item.name);  // Copy the value of item.name to compare
			break;
		case ColumnID_FormID:
			strcpy(compare, item.formid.c_str());  // Copy the value of item.formid to compare
			break;
		case ColumnID_EditorID:
			strcpy(compare, item.editorid.c_str());  // Copy the value of item.editorid to compare
			break;
		case ColumnID_None:
			skip = true;
			break;
		default:
			strcpy(compare, item.name);  // Copy the value of item.name to compare
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

		if (skip) {
			int score = 0;

			char _name[256];
			char _editorid[256];
			char _formid[256];

			strcpy(_name, item.name);
			strcpy(_editorid, item.editorid.c_str());
			strcpy(_formid, item.formid.c_str());

			strlwr(_name);
			strlwr(_editorid);
			strlwr(_formid);

			if (strstr(_name, input) != nullptr)
				score++;
			if (strstr(_editorid, input) != nullptr)
				score++;
			if (strstr(_formid, input) != nullptr)
				score++;

			if (score > 0) {
				itemList.push_back(&item);
			}

			continue;
		}

		if (strstr(lower_compare, input) != nullptr) {
			itemList.push_back(&item);
		}
	}

	// Resort the list after applying filters
	dirty = true;
}

// Draw search bar for filtering items.
void AddItemWindow::Draw_InputSearch()
{
	ImGui::Text("Refine your search:");
	// Testing without ImGuiInputTextFlags_EnterReturnsTrue for live updates
	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EscapeClearsAll;

	std::map<ColumnID, const char*> search_map = {
		{ ColumnID_None, "None" },
		{ ColumnID_Name, "Name" },
		{ ColumnID_EditorID, "Editor ID" },
		{ ColumnID_FormID, "Form ID" }
	};

	if (ImGui::InputTextWithHint("##AddItemWindow::InputField", "Enter text to filter results by...", inputBuffer,
			IM_ARRAYSIZE(inputBuffer),
			input_text_flags)) {
		ApplyFilters();
	}

	// Might need to pop id here?

	ImGui::SameLine();

	auto searchByValue = search_map.at(searchKey);
	auto combo_flags = ImGuiComboFlags_WidthFitPreview;
	if (ImGui::BeginCombo("##AddItemWindow::InputFilter", searchByValue, combo_flags)) {
		for (auto& item : search_map) {
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
}

// TODO: Implement more here.
void AddItemWindow::ShowActions(Settings::Style& a_style, Settings::Config& a_config)
{
	(void)a_config;

	ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

	auto _flags = ImGuiChildFlags_AlwaysUseWindowPadding;
	ImGui::BeginChild("##AddItemWindow::Actions", ImVec2(ImGui::GetContentRegionAvail()), _flags);
	ImGui::SeparatorText("Selection:");

	const float button_height = ImGui::GetFontSize() * 1.5f;
	const float button_width = ImGui::GetContentRegionAvail().x;

	const auto table_flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;
	if (ImGui::BeginTable("##AddItemWindow::ActionBarSelection", 1, table_flags, ImVec2(ImGui::GetContentRegionAvail().x, 150.0f))) {
		ImGui::PushFont(a_style.headerFont);
		ImGui::TableSetupColumn("Item(s)", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableHeadersRow();
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::PopFont();

		const auto select_flags = ImGuiSelectableFlags_SpanAllColumns;
		for (auto& item : itemList) {
			if (item->selected) {
				if (ImGui::Selectable(item->editorid.c_str(), false, select_flags)) {
					item->selected = false;
				};
				ImGui::SetItemTooltip("Click to remove.");
			}
		}
		ImGui::EndTable();
	}

	ImGui::PushFont(a_style.buttonFont);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.5f, a_style.button.w));
	if (ImGui::Button("Clear Selection", ImVec2(button_width, button_height))) {
		for (auto& item : itemList) {
			item->selected = false;
		}
	}
	ImGui::PopStyleColor(1);
	ImGui::PopFont();  // Button font

	ImGui::SeparatorText("Selection:");

	ImGui::PushFont(a_style.buttonFont);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 1.0f, 0.5f, a_style.button.w));
	if (ImGui::Button("Add to Inventory", ImVec2(button_width, button_height))) {
		for (auto& item : itemList) {
			if (item->selected) {
				ConsoleCommand::AddItem(item->formid);
				item->selected = false;
				logger::info("Added item: {}", item->formid.c_str());
			}
		}
	}

	if (ImGui::Button("Add to Favorites", ImVec2(button_width, button_height))) {
		for (auto& item : itemList) {
			if (item->selected) {
				item->favorite = !item->favorite;
			}
		}
	}
	ImGui::PopStyleColor(1);

	if (ImGui::Button("Select All Favorites", ImVec2(button_width, button_height))) {
		for (auto& item : itemList) {
			if (item->favorite) {
				item->selected = true;
			}
		}
	}

	if (ImGui::Button("Select All", ImVec2(button_width, button_height))) {
		for (auto& item : itemList) {
			item->selected = true;
		}
	}
	ImGui::PopFont();  // Button Font

	ImGui::PopStyleVar(2);

	ImGui::EndChild();
}

static inline void inline_checkbox(const char* label, bool* v)
{
	ImGui::Checkbox(label, v);
	ImGui::SameLine();
}

void AddItemWindow::ShowOptions(Settings::Style& a_style, Settings::Config& a_config)
{
	(void)a_style;
	(void)a_config;

	ImGui::SeparatorText("Select a mod:");
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
}
// Cache items on initialization
void AddItemWindow::Init()
{
	//AddItemWindow::Cache();
}