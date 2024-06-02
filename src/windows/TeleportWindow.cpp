#include "TeleportWindow.h"
#include "Data.h"
//#include "Settings.h"

// If I need to add filtering for items like Interior/Exterior or something, reference this:
// std::vector<std::tuple<bool*, AddItemWindow::ItemType, std::string>> AddItemWindow::_filterMap = {
// 	{ &_Alchemy, AddItemWindow::Alchemy, "ALCH" }, { &_Ingredient, AddItemWindow::Ingredient, "INGR" },
// 	{ &_Ammo, AddItemWindow::Ammo, "AMMO" }, { &_Key, AddItemWindow::Key, "KEYS" },
// 	{ &_Misc, AddItemWindow::Misc, "MISC" }, { &_Armor, AddItemWindow::Armor, "ARMO" },
// 	{ &_Book, AddItemWindow::Book, "BOOK" }, { &_Weapon, AddItemWindow::Weapon, "WEAP" }
// };

static inline const float center_text_x(const char* text)
{
	return ImGui::GetCursorPosX() + ImGui::GetColumnWidth() / 2 -
	       ImGui::CalcTextSize(text).x / 2;
};

void TeleportWindow::ShowTable(Settings::Style& a_style)
{
	//auto& cellMap = MEMData::GetCellMap();

	auto& config = Settings::GetSingleton()->GetConfig();

	//ImGuiTableFlags_SizingStretchProp
	const auto table_flags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Sortable |
	                         ImGuiTableFlags_Borders | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Hideable |
	                         ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_NoBordersInBody |
	                         ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;

	//const auto sizing = (_activeList.empty() ? ImGuiTableFlags_SizingFixedFit : ImGuiTableFlags_SizingStretchProp);
	const auto sizing = ImGuiTableFlags_SizingStretchProp;
	//const auto column_flags = (_activeList.empty() ? ImGuiTableColumnFlags_WidthFixed : ImGuiTableColumnFlags_WidthStretch);

	const auto table_size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

	if (ImGui::BeginTable("##TeleportTable", column_count, table_flags | sizing, table_size)) {
		ImGui::TableSetupScrollFreeze(1, 1);
		ImGui::TableSetupColumn("##Favorite", ImGuiTableColumnFlags_WidthFixed, 18.0f, ColumnID_Favorite);
		ImGui::TableSetupColumn("Plugin", ImGuiTableColumnFlags_WidthStretch, 75.0f, ColumnID_Plugin);
		ImGui::TableSetupColumn("Worldspace", ImGuiTableColumnFlags_WidthStretch, 25.0f, ColumnID_Space);
		ImGui::TableSetupColumn("Zone", ImGuiTableColumnFlags_WidthStretch, 35.0f, ColumnID_Zone);
		ImGui::TableSetupColumn("Full Name", ImGuiTableColumnFlags_WidthStretch, 75.0f, ColumnID_FullName);
		ImGui::TableSetupColumn("Editor ID", ImGuiTableColumnFlags_WidthStretch, 75.0f, ColumnID_EditorID);

		ImGui::PushFont(a_style.headerFont);
		ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
		for (int column = 0; column < column_count; column++) {
			ImGui::TableSetColumnIndex(column);
			const char* column_name = ImGui::TableGetColumnName(column);  // Retrieve name passed to TableSetupColumn()
			ImGui::PushID(column);
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			ImGui::TableHeader(column_name);

			//ImGui::TableSetColumnEnabled(column, column_toggle[column]);
			ImGui::PopID();
		}
		ImGui::PopFont();

		ImGui::TableSetColumnEnabled(ColumnID_Favorite, config.teleShowFavoriteColumn);
		ImGui::TableSetColumnEnabled(ColumnID_Plugin, config.teleShowPluginColumn);
		ImGui::TableSetColumnEnabled(ColumnID_Space, config.teleShowSpaceColumn);
		ImGui::TableSetColumnEnabled(ColumnID_Zone, config.teleShowZoneColumn);
		ImGui::TableSetColumnEnabled(ColumnID_FullName, config.teleShowFullNameColumn);
		ImGui::TableSetColumnEnabled(ColumnID_EditorID, config.teleShowEditorIDColumn);

		if (dirty) {
			ImGui::TableGetSortSpecs()->SpecsDirty = true;
		}

		if (ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs()) {
			if (sort_specs->SpecsDirty) {
				SortColumnsWithSpecs(sort_specs);
				sort_specs->SpecsDirty = false;
				dirty = false;
			}
		}

		int count = 0;
		for (auto& cell : cellMap) {
			count++;

			auto table_id = std::string("##TeleportTable::Table-") + std::to_string(count);

			ImGui::PushID(table_id.c_str());
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			ImTextureID favorite_state = cell->favorite ? a_style.favoriteIconEnabled.texture : a_style.favoriteIconDisabled.texture;
			float col = cell->favorite ? 1.0f : 0.5f;

			if (favorite_state != nullptr) {
				if (ImGui::ImageButton("##FavoriteButton", favorite_state, ImVec2(18.0f, 18.0f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(col, col, col, col))) {
					cell->favorite = !cell->favorite;
				}
			} else {
				if (ImGui::Checkbox((std::string("##CheckboxTest") + table_id).c_str(), &cell->favorite)) {
					// do nothing
				}
			}

			ImGui::TableNextColumn();

			ImGui::Text(cell->plugin.c_str());
			ImGui::TableNextColumn();
			ImGui::Text(cell->space.c_str());
			ImGui::TableNextColumn();
			ImGui::Text(cell->zone.c_str());
			ImGui::TableNextColumn();
			ImGui::Text(cell->fullName.c_str());
			ImGui::TableNextColumn();
			ImGui::Text(cell->editorid.c_str());
			ImGui::PopID();
		}

		ImGui::EndTable();
	}
}

bool TeleportWindow::SortColumn(const MEMData::CachedCell* v1, const MEMData::CachedCell* v2)
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
	case ColumnID_Plugin:  // const char *
		delta = strcmp(v1->plugin.c_str(), v2->plugin.c_str());
		break;
	case ColumnID_Space:  // std::string
		delta = strcmp(v1->space.c_str(), v2->space.c_str());
		break;
	case ColumnID_Zone:  // const char *
		delta = strcmp(v1->zone.c_str(), v2->zone.c_str());
		break;
	case ColumnID_FullName:  // std::string
		delta = strcmp(v1->fullName.c_str(), v2->fullName.c_str());
		break;
	case ColumnID_EditorID:  // std::int32_t
		delta = strcmp(v1->editorid.c_str(), v2->editorid.c_str());
		break;
	default:
		break;
	}

	if (delta > 0) {
		return (sort_specs->Specs->SortDirection == ImGuiSortDirection_Ascending) ? true : false;
	}
	if (delta < 0) {
		return (sort_specs->Specs->SortDirection == ImGuiSortDirection_Ascending) ? false : true;
	}

	return false;  // prevent assertion (?)
}

void TeleportWindow::SortColumnsWithSpecs(ImGuiTableSortSpecs* sort_specs)
{
	s_current_sort_specs = sort_specs;
	if (cellMap.size() > 1) {
		std::sort(cellMap.begin(), cellMap.end(), SortColumn);
	}
	s_current_sort_specs = NULL;
}

void TeleportWindow::ApplyFilters()
{
	cellMap.clear();

	auto& cached_cell_map = MEMData::GetCellMap();

	static char compare[256];  // Declare a char array to store the value of compare
	bool skip = false;

	for (auto& cell : cached_cell_map) {
		switch (searchKey) {
		case ColumnID_Plugin:
			strcpy(compare, cell.plugin.c_str());  // Copy the value of item.name to compare
			break;
		case ColumnID_Space:
			strcpy(compare, cell.space.c_str());  // Copy the value of item.formid to compare
			break;
		case ColumnID_Zone:
			strcpy(compare, cell.zone.c_str());  // Copy the value of item.editorid to compare
			break;
		case ColumnID_FullName:
			strcpy(compare, cell.fullName.c_str());  // Copy the value of item.editorid to compare
			break;
		case ColumnID_EditorID:
			strcpy(compare, cell.editorid.c_str());  // Copy the value of item.editorid to compare
			break;
		case ColumnID_None:
			skip = true;
			break;
		default:
			strcpy(compare, cell.fullName.c_str());  // Copy the value of item.name to compare
			break;
		}

		if (selectedMod != nullptr && cell.mod != selectedMod)
			continue;

		auto lower_input = strlwr(inputBuffer);
		auto lower_compare = strlwr(compare);

		if (skip) {
			int score = 0;

			if (strstr(strlwr((char*)cell.plugin.c_str()), lower_input) != nullptr)
				score++;
			if (strstr(strlwr((char*)cell.space.c_str()), lower_input) != nullptr)
				score++;
			if (strstr(strlwr((char*)cell.zone.c_str()), lower_input) != nullptr)
				score++;
			if (strstr(strlwr((char*)cell.fullName.c_str()), lower_input) != nullptr)
				score++;
			if (strstr(strlwr((char*)cell.editorid.c_str()), lower_input) != nullptr)
				score++;

			if (score > 0) {
				cellMap.push_back(&cell);
			}

			continue;
		}

		if (strstr(lower_compare, lower_input) != nullptr) {
			if (selectedMod != nullptr && cell.mod != selectedMod)  // skip non-active mods
				continue;

			cellMap.push_back(&cell);
		}
	}

	dirty = true;
}

void TeleportWindow::ShowInputSearch(Settings::Style& a_style)
{
	ImGui::PushID("##InputSearchTeleportWindowUniqueID");
	(void)a_style;

	static char str0[32] = "";
	// Testing without ImGuiInputTextFlags_EnterReturnsTrue for live updates
	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EscapeClearsAll;

	std::map<ColumnID, const char*> search_map = {
		{ ColumnID_None, "None" },
		{ ColumnID_FullName, "Name" },
		{ ColumnID_EditorID, "Editor ID" },
		{ ColumnID_Plugin, "Plugin Name" },
		{ ColumnID_Space, "Space" },
		{ ColumnID_Zone, "Zone" },
		{ ColumnID_Favorite, "Favorite" },
	};

	ImGui::SeparatorText("Search:");
	if (ImGui::InputTextWithHint("##InputFieldTeleportWindow", "Enter text to filter results by...", inputBuffer,
			IM_ARRAYSIZE(inputBuffer),
			input_text_flags)) {
		ApplyFilters();
	}
	ImGui::PopID();

	ImGui::SameLine();

	auto searchByValue = search_map.at(searchKey);
	auto combo_flags = ImGuiComboFlags_WidthFitPreview;
	if (ImGui::BeginCombo("##FilterSearchByTeleport", searchByValue, combo_flags)) {
		for (auto& item : search_map) {
			if (item.first == ColumnID_Favorite)
				continue;

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

void TeleportWindow::ShowOptions(Settings::Style& a_style)
{
	(void)a_style;

	ImGui::SeparatorText("Select a mod to search:");
	auto combo_text = selectedMod ? selectedMod->GetFilename().data() : "Filter by mods";

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

void TeleportWindow::Draw(Settings::Style& a_style)
{
	//auto& config = Settings::GetSingleton()->GetConfig();
	// const auto _flags = ImGuiOldColumnFlags_NoResize;
	// ImGui::BeginColumns("##HorizontalSplit", 2, _flags);

	// const float width = ImGui::GetWindowWidth();
	// ImGui::SetColumnWidth(0, width * 0.75f);

	// // Left Column
	// Draw_InputSearch();
	ShowInputSearch(a_style);

	// ImGui::NewLine();

	// Draw_AdvancedOptions();

	ShowOptions(a_style);

	// ImGui::NewLine();

	// // FIXME: This is a hack to get the results count to update.
	auto results = std::string("Results (") + std::to_string(cellMap.size()) + std::string(")");
	ImGui::SeparatorText(results.c_str());

	ShowTable(a_style);

	// // Start of Right Column
	// ImGui::NextColumn();

	// Draw_Actions();

	// ImGui::EndColumns();
}