#include "AddItemWindow.h"
#include "Console.h"
#include "Settings.h"
#include "lib/Graphic.h"

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

	// TODO: Write functionality for None filter

	if (delta > 0)
		return (sort_specs->Specs->SortDirection == ImGuiSortDirection_Ascending) ? true : false;
	if (delta < 0)
		return (sort_specs->Specs->SortDirection == ImGuiSortDirection_Ascending) ? false : true;

	// Fallback to prevent assertion.
	return false;
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

// Draw the table of items
void AddItemWindow::Draw_FormTable()
{
	auto& style = Settings::GetSingleton()->GetStyle();

	const auto table_flags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Sortable |
	                         ImGuiTableFlags_Borders | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Hideable |
	                         ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_NoBordersInBody |
	                         ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;

	const auto sizing = ImGuiTableFlags_SizingStretchProp;

	const auto table_size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

	if (ImGui::BeginTable("AddItemWindowTable", column_count, table_flags | sizing, table_size)) {
		ImGui::TableSetupScrollFreeze(1, 1);
		ImGui::TableSetupColumn(" ", ImGuiTableColumnFlags_WidthFixed, 16.0f, ColumnID_Favorite);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 65.0f, ColumnID_Type);
		ImGui::TableSetupColumn("Form ID", ImGuiTableColumnFlags_WidthFixed, 100.0f, ColumnID_FormID);
		ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_DefaultSort, 16.0f, ColumnID_Name);
		ImGui::TableSetupColumn("Editor ID", ImGuiTableColumnFlags_None, 16.0f, ColumnID_EditorID);
		ImGui::TableSetupColumn("Gold", ImGuiTableColumnFlags_WidthFixed, 65.0f, ColumnID_GoldValue);

		ImGui::PushFont(style.headerFont);
		ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
		for (int column = 0; column < column_count; column++) {
			ImGui::TableSetColumnIndex(column);
			const char* column_name = ImGui::TableGetColumnName(column);  // Retrieve name passed to TableSetupColumn()
			ImGui::PushID(column);
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			if (column != ColumnID_FormID && column != ColumnID_EditorID)
				ImGui::SetCursorPosX(center_text_x(column_name));

			ImGui::TableHeader(column_name);

			ImGui::TableSetColumnEnabled(column, column_toggle[column]);
			ImGui::PopID();
		}
		ImGui::PopFont();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 10.0f));

		if (dirty)
			ImGui::TableGetSortSpecs()->SpecsDirty = true;

		// Sort our data if sort specs have been changed!
		if (ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs())
			if (sort_specs->SpecsDirty) {
				SortColumnsWithSpecs(sort_specs);
				sort_specs->SpecsDirty = false;
				dirty = false;
			}

		int count = 0;
		for (auto& item : itemList) {
			count++;

			if (count > 1000)
				break;

			auto table_id = std::string("##AddItemMenu::Table-") + std::to_string(count);

			ImGuiTableRowFlags row_flags = ImGuiTableRowFlags_None;

			ImGui::PushID(table_id.c_str());
			ImGui::TableNextRow(row_flags);
			ImGui::TableNextColumn();

			// FIXME: Why is this here?
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

			ImTextureID favorite_state = item->favorite ? style.favoriteIconEnabled.texture : style.favoriteIconDisabled.texture;
			float col = item->favorite ? 1.0f : 0.5f;

			if (favorite_state != nullptr) {
				if (ImGui::ImageButton("##FavoriteButton", favorite_state, ImVec2(18.0f, 18.0f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(col, col, col, col))) {
					item->favorite = !item->favorite;
				}
			} else {
				ImGui::Checkbox("##FavoriteCheckbox", &item->favorite);
			}

			ImGui::PopStyleColor(3);
			ImGui::PopStyleVar(1);

			const ImGuiSelectableFlags select_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap;
			ImGui::TableNextColumn();
			ImGui::SetCursorPosX(center_text_x(item->typeName.c_str()));  // TODO: Why does this want const char*?
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

			if (item->formType == RE::FormType::Weapon) {
				auto curRow = ImGui::TableGetHoveredRow();

				if (curRow == ImGui::TableGetRowIndex()) {
					//enum WEAPON_TYPE : std::uint32_t
					//{
					//    kHandToHandMelee = 0,
					//    kOneHandSword = 1,
					//    kOneHandDagger = 2,
					//    kOneHandAxe = 3,
					//    kOneHandMace = 4,
					//    kTwoHandSword = 5,
					//    kTwoHandAxe = 6,
					//    kBow = 7,
					//    kStaff = 8,
					//    kCrossbow = 9,

					//    kTotal = 10
					//};

					std::vector<const char*> weaponTypes = {
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

					//auto desc = item->form->As<RE::TESObjectWEAP>()->GetDescription();
					//auto critChance = item->form->As<RE::TESObjectWEAP>()->GetCrit;
					uint16_t damage = item->form->As<RE::TESObjectWEAP>()->attackDamage;                                             // seems like base damage
					uint16_t damage_other = item->form->As<RE::TESObjectWEAP>()->criticalData.damage;                                // 1-100x multiplier?
					auto maxRange = item->form->As<RE::TESObjectWEAP>()->weaponData.maxRange;                                        // bow only
					auto minRange = item->form->As<RE::TESObjectWEAP>()->weaponData.minRange;                                        // bow only
					auto reach = item->form->As<RE::TESObjectWEAP>()->weaponData.reach;                                              // + and - 1 by a little.
					auto speed = item->form->As<RE::TESObjectWEAP>()->weaponData.speed;                                              // 0 - 2?
					SKSE::stl::enumeration<RE::ActorValue, uint32_t> skill = item->form->As<RE::TESObjectWEAP>()->weaponData.skill;  // see below
					auto stagger = item->form->As<RE::TESObjectWEAP>()->weaponData.staggerValue;                                     // 0 - 2 with 1 being median
					auto crit = item->form->As<RE::TESObjectWEAP>()->GetCritDamage();
					auto type = weaponTypes[static_cast<int>(item->form->As<RE::TESObjectWEAP>()->GetWeaponType())];

					auto is_bound = item->form->As<RE::TESObjectWEAP>()->IsBound();
					auto non_playable = item->form->As<RE::TESObjectWEAP>()->weaponData.flags.any(RE::TESObjectWEAP::Data::Flag::kNonPlayable);

					auto skill_value = skill.get();  // ActorValue enum ng.commonlib.dev/_actor_values_8h_source.html

					ImGui::BeginTooltip();
					ImGui::Text("attackDamage: %d", damage);
					ImGui::Text("criticalData.damage: %d", damage_other);
					ImGui::Text("maxRange: %f", maxRange);
					ImGui::Text("minRange: %f", minRange);
					ImGui::Text("reach: %f", reach);
					ImGui::Text("speed: %f", speed);
					ImGui::Text("stagger: %f", stagger);
					ImGui::Text("skill: %d", skill_value);
					ImGui::Text("crit: %d", crit);
					ImGui::Text("type: %s", type);
					ImGui::Text("non_playable: %s", (non_playable) ? "true" : "false");
					ImGui::Text("is_bound: %s", (is_bound) ? "true" : "false");
					ImGui::EndTooltip();
				}
			}

			//if (ImGui::BeginPopupContextItem())
			//{
			//    Context_CopyOnly(ref.formid.c_str(), ref.name, ref.editorid.c_str());
			//    ImGui::EndPopup();
			//}

			ImGui::PopID();
		}

		ImGui::PopStyleVar(1);

		// TODO: Implement settings variable for count
		if (count > 1000) {
			ImGui::SeparatorText("Too many results, please refine your search. (1000+ items)");
		}

		ImGui::EndTable();
	}
}

void AddItemWindow::ApplyFilters()
{
	itemList.clear();

	auto& cached_item_list = MEMData::GetItemList();

	// FIXME: Does this need to be static?
	static char compare[256];
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

		if (selectedMod != nullptr && item.mod != selectedMod)  // skip non-active mods
			continue;

		auto lower_input = strlwr(inputBuffer);
		auto lower_compare = strlwr(compare);

		if (skip) {
			int score = 0;

			if (strstr(strlwr((char*)item.name), lower_input) != nullptr)
				score++;
			if (strstr(strlwr((char*)item.editorid.c_str()), lower_input) != nullptr)
				score++;
			if (strstr(strlwr((char*)item.formid.c_str()), lower_input) != nullptr)
				score++;

			if (score > 0) {
				itemList.push_back(&item);
			}

			continue;
		}

		if (strstr(lower_compare, lower_input) != nullptr) {
			if (item.nonPlayable)  // skip non-usable
				continue;

			if (strcmp(item.name, "") == 0)  // skip empty names
				continue;

			// Only append if the item is in the filter list.
			if (itemFilters.count(item.formType) > 0) {
				itemList.push_back(&item);
			}
		}
	}

	// Resort the list after applying filters
	dirty = true;
}

// Draw search bar for filtering items.
void AddItemWindow::Draw_InputSearch()
{
	ImGui::Text("Type in search terms to filter your results:");
	// Testing without ImGuiInputTextFlags_EnterReturnsTrue for live updates
	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EscapeClearsAll;

	std::map<ColumnID, const char*> search_map = {
		{ ColumnID_None, "None" },
		{ ColumnID_Name, "Name" },
		{ ColumnID_EditorID, "Editor ID" },
		{ ColumnID_FormID, "Form ID" }
	};

	if (ImGui::InputTextWithHint("##InputField", "Enter text to filter results by...", inputBuffer,
			IM_ARRAYSIZE(inputBuffer),
			input_text_flags)) {
		ApplyFilters();
	}

	// Might need to pop id here?

	ImGui::SameLine();

	auto searchByValue = search_map.at(searchKey);
	auto combo_flags = ImGuiComboFlags_WidthFitPreview;
	if (ImGui::BeginCombo("##FilterSearchBy", searchByValue, combo_flags)) {
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
void AddItemWindow::Draw_Actions()
{
	auto& style = Settings::GetSingleton()->GetStyle();

	ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

	auto _flags = ImGuiChildFlags_AlwaysUseWindowPadding;
	ImGui::BeginChild("##AddItemWindowMenuActions", ImVec2(ImGui::GetContentRegionAvail()), _flags);
	ImGui::SeparatorText("Selection:");

	const float button_height = ImGui::GetFontSize() * 1.5f;
	const float button_width = ImGui::GetContentRegionAvail().x;

	const auto table_flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;
	if (ImGui::BeginTable("ActionBarSelection", 1, table_flags, ImVec2(ImGui::GetContentRegionAvail().x, 150.0f))) {
		ImGui::PushFont(style.headerFont);
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

	ImGui::PushFont(style.buttonFont);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.5f, style.button.w));
	if (ImGui::Button("Clear Selection", ImVec2(button_width, button_height))) {
		for (auto& item : itemList) {
			item->selected = false;
		}
	}
	ImGui::PopStyleColor(1);
	ImGui::PopFont();  // Button font

	ImGui::SeparatorText("Selection:");

	ImGui::PushFont(style.buttonFont);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 1.0f, 0.5f, style.button.w));
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

	ImGui::PopStyleVar(3);

	ImGui::EndChild();
}

static inline void inline_checkbox(const char* label, bool* v)
{
	ImGui::Checkbox(label, v);
	ImGui::SameLine();
}

void AddItemWindow::Draw_AdvancedOptions()
{
	const auto header_flags = ImGuiTreeNodeFlags_SpanAvailWidth;
	if (ImGui::CollapsingHeader("Advanced Options", header_flags)) {
		ImGui::SeparatorText("Enable/Disable Columns:");

		inline_checkbox("Show Favorite", &column_toggle[ColumnID_Favorite]);
		inline_checkbox("Show Type", &column_toggle[ColumnID_Type]);
		inline_checkbox("Show Form ID", &column_toggle[ColumnID_FormID]);
		inline_checkbox("Show Name", &column_toggle[ColumnID_Name]);
		inline_checkbox("Show Editor ID", &column_toggle[ColumnID_EditorID]);
		inline_checkbox("Show Gold Value", &column_toggle[ColumnID_GoldValue]);

		ImGui::NewLine();

		ImGui::SeparatorText("Select a mod to search:");
		auto combo_text = selectedMod ? selectedMod->GetFilename().data() : "Filter by mods";

		//float width = static_cast<float>(list_arrow.width);
		//float height = static_cast<float>(list_arrow.height);
		//ImGui::ImageButton("##FilterByModHelp", list_arrow.texture, ImVec2(width / 3, height / 3));

		ImGui::NewLine();

		if (ImGui::BeginCombo("##FilterByMod", combo_text)) {
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
}

// Main Draw function for AddItem, called by Frame::Draw()
void AddItemWindow::Draw()
{
	const auto _flags = ImGuiOldColumnFlags_NoResize;
	ImGui::BeginColumns("##HorizontalSplit", 2, _flags);

	const float width = ImGui::GetWindowWidth();
	ImGui::SetColumnWidth(0, width * 0.75f);

	// Left Column
	Draw_InputSearch();
	ImGui::NewLine();

	bool _change = false;

	for (auto& item : AddItemWindow::filterMap) {
		//auto [first, second, third] = item.;
		auto first = std::get<0>(item);
		//auto second = std::get<1>(item);
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

	Draw_AdvancedOptions();

	ImGui::NewLine();

	// FIXME: This is a hack to get the results count to update.
	auto results = std::string("Results (") + std::to_string(itemList.size()) + std::string(")");
	ImGui::SeparatorText(results.c_str());

	Draw_FormTable();

	// Start of Right Column
	ImGui::NextColumn();

	Draw_Actions();

	ImGui::EndColumns();
}

// Cache items on initialization
void AddItemWindow::Init()
{
	//AddItemWindow::Cache();
}