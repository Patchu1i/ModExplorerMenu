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

// TODO: Optimize this a little more. Starting to slow down. also see ApplyFilters()
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
	case ColumnID_BaseDamage:
		if (v1->formType == RE::FormType::Weapon && v2->formType == RE::FormType::Weapon) {
			auto* weapon1 = v1->form->As<RE::TESObjectWEAP>();
			auto* weapon2 = v2->form->As<RE::TESObjectWEAP>();
			auto baseDamage1 = Utils::CalcBaseDamage(weapon1);
			auto baseDamage2 = Utils::CalcBaseDamage(weapon2);
			delta = (baseDamage1 < baseDamage2) ? -1 : (baseDamage1 > baseDamage2) ? 1 :
			                                                                         0;
		} else if (v1->formType == RE::FormType::Weapon) {
			delta = 1;  // force above
		} else if (v2->formType == RE::FormType::Weapon) {
			delta = -1;  // force below
		}
		break;
	case ColumnID_Speed:
		if (v1->formType == RE::FormType::Weapon && v2->formType == RE::FormType::Weapon) {
			auto* weapon1 = v1->form->As<RE::TESObjectWEAP>();
			auto* weapon2 = v2->form->As<RE::TESObjectWEAP>();
			auto speed1 = weapon1->weaponData.speed;
			auto speed2 = weapon2->weaponData.speed;
			delta = (speed1 < speed2) ? -1 : (speed1 > speed2) ? 1 :
			                                                     0;
		} else if (v1->formType == RE::FormType::Weapon) {
			delta = 1;  // force above
		} else if (v2->formType == RE::FormType::Weapon) {
			delta = -1;  // force below
		}
		break;
	case ColumnID_CritDamage:
		if (v1->formType == RE::FormType::Weapon && v2->formType == RE::FormType::Weapon) {
			auto* weapon1 = v1->form->As<RE::TESObjectWEAP>();
			auto* weapon2 = v2->form->As<RE::TESObjectWEAP>();
			auto critDamage1 = weapon1->GetCritDamage();
			auto critDamage2 = weapon2->GetCritDamage();
			delta = (critDamage1 < critDamage2) ? -1 : (critDamage1 > critDamage2) ? 1 :
			                                                                         0;
		} else if (v1->formType == RE::FormType::Weapon) {
			delta = 1;  // force above
		} else if (v2->formType == RE::FormType::Weapon) {
			delta = -1;  // force below
		}
		break;
	case ColumnID_Skill:
		if ((v1->formType == RE::FormType::Weapon && v2->formType == RE::FormType::Weapon) ||
			(v1->formType == RE::FormType::Armor && v2->formType == RE::FormType::Armor) ||
			(v1->formType == RE::FormType::Weapon && v2->formType == RE::FormType::Armor) ||
			(v1->formType == RE::FormType::Armor && v2->formType == RE::FormType::Weapon)) {
			auto item1 = v1->form->GetObjectTypeName();
			auto item2 = v2->form->GetObjectTypeName();
			delta = strcmp(item1, item2);
		} else if (v1->formType == RE::FormType::Weapon || v1->formType == RE::FormType::Armor) {
			delta = 1;  // force above
		} else if (v2->formType == RE::FormType::Weapon || v2->formType == RE::FormType::Armor) {
			delta = -1;  // force below
		}
		break;
	case ColumnID_Weight:
		delta = (v1->form->GetWeight() < v2->form->GetWeight()) ? -1 : (v1->form->GetWeight() > v2->form->GetWeight()) ? 1 :
		                                                                                                                 0;
		break;
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
		//ImGui::SameLine();
		//ImGui::InvisibleButton("##AddItemWindow::InlineBar", barSize);
		ImGui::SameLine(popWidth - barSize.x);
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ProgressColor(value, max_value));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);  // tight fit
		float curr = static_cast<float>(value);
		char buffer[256];
		sprintf(buffer, "%.2f", value);
		ImGui::ProgressBar(curr / max_value, barSize, buffer);

		ImGui::PopStyleColor(1);
		ImGui::PopStyleVar(1);
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
		auto weight = armor->GetWeight();

		InlineBar("Armor Rating:", (float)armorRating, 100.0f);
		InlineText("Armor Type:", armorType);
		InlineText("Equip Slot:", equipSlot);
		InlineInt("Weight:", (int)weight);
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

		auto damage = Utils::CalcBaseDamage(weapon);
		auto max_damage = Utils::CalcMaxDamage(damage, 50);
		auto speed = weapon->weaponData.speed;      // 0 - 2?
		auto critDamage = weapon->GetCritDamage();  // 1-100x multiplier?
		auto skill = weapon->weaponData.skill;      // see below
		auto type = weaponTypes[static_cast<int>(weapon->GetWeaponType())];
		auto weight = weapon->GetWeight();
		auto dps = damage * speed;

		if (weapon->IsStaff()) {
			InlineText("Base Damage:", "N/A");
		} else if (weapon->IsBow() || weapon->IsCrossbow()) {
			// auto maxRange = weapon->weaponData.maxRange; // Doesn't seem to matter.
			// auto minRange = weapon->weaponData.minRange; // Always 2000, 500.
			InlineBar("Base Damage:", (float)damage, max_damage);
			InlineBar("Draw Speed:", (float)speed, 1.5f);
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			InlineInt("DPS:", (int)dps);
			InlineInt("Critical Damage:", critDamage);
			InlineText("Skill:", std::to_string(skill.get()).c_str());
		} else {
			double reach = weapon->weaponData.reach;          // + and - 1 by a little.
			float stagger = weapon->weaponData.staggerValue;  // 0 - 2 with 1 being median
			InlineBar("Base Damage:", (float)damage, max_damage);
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			InlineInt("DPS:", (int)dps);
			InlineBar("Reach:", (float)reach, 1.5f);
			InlineBar("Speed:", (float)speed, 1.5f);
			InlineBar("stagger:", stagger, 2.0f);
			InlineInt("Critical Damage:", critDamage);
			InlineText("Skill:", std::to_string(skill.get()).c_str());
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		InlineInt("Weight:", (int)weight);
		InlineText("Type:", type);
	}

	// Always show:
	InlineInt("Gold Value:", item->goldValue);

	if (g_DescriptionFrameworkInterface != nullptr) {
		auto description = g_DescriptionFrameworkInterface->GetDescription(item->form);
		if (description != nullptr) {
			ImGui::Text(std::to_string(g_DescriptionFrameworkInterface->GetBuildNumber()).c_str());
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			ImGui::TextWrapped(description);
		}
	}

	// if (item->form != nullptr && item->form->As<RE::TESDescription>() != nullptr) {
	// 	const auto desc = item->form->As<RE::TESDescription>();
	// 	if (desc) {
	// 		RE::BSString buf;
	// 		desc->GetDescription(buf, nullptr);
	// 		if (!buf.empty()) {
	// 			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
	// 			ImGui::TextWrapped(buf.c_str());
	// 		}
	// 	}
	// }

	ImGui::EndTooltip();
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

	// It's unfortunate I have to do this with custom column visibility:
	bool noColumns = !a_config.aimShowFavoriteColumn && !a_config.aimShowTypeColumn && !a_config.aimShowFormIDColumn &&
	                 !a_config.aimShowNameColumn && !a_config.aimShowEditorIDColumn && !a_config.aimShowGoldValueColumn &&
	                 !a_config.aimShowBaseDamageColumn && !a_config.aimShowSpeedColumn && !a_config.aimShowCritDamageColumn &&
	                 !a_config.aimShowSkillColumn && !a_config.aimShowWeightColumn && !a_config.aimShowDPSColumn;

	if (noColumns) {
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
		ImGui::TableSetupColumn("Base Damage", ImGuiTableColumnFlags_WidthFixed, 20.0f, ColumnID_BaseDamage);
		ImGui::TableSetupColumn("Speed", ImGuiTableColumnFlags_WidthFixed, 20.0f, ColumnID_Speed);
		ImGui::TableSetupColumn("Crit Damage", ImGuiTableColumnFlags_WidthFixed, 20.0f, ColumnID_CritDamage);
		ImGui::TableSetupColumn("Skill", ImGuiTableColumnFlags_None, 20.0f, ColumnID_Skill);
		ImGui::TableSetupColumn("Weight", ImGuiTableColumnFlags_WidthFixed, 20.0f, ColumnID_Weight);
		ImGui::TableSetupColumn("DPS", ImGuiTableColumnFlags_WidthFixed, 20.0f, ColumnID_DPS);

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
		ImGui::TableSetColumnEnabled(ColumnID_BaseDamage, a_config.aimShowBaseDamageColumn);
		ImGui::TableSetColumnEnabled(ColumnID_Speed, a_config.aimShowSpeedColumn);
		ImGui::TableSetColumnEnabled(ColumnID_CritDamage, a_config.aimShowCritDamageColumn);
		ImGui::TableSetColumnEnabled(ColumnID_Skill, a_config.aimShowSkillColumn);
		ImGui::TableSetColumnEnabled(ColumnID_Weight, a_config.aimShowWeightColumn);
		ImGui::TableSetColumnEnabled(ColumnID_DPS, a_config.aimShowDPSColumn);

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
				ImGui::TableNextColumn();
				if (item->formType == RE::FormType::Weapon) {
					auto* weapon = item->form->As<RE::TESObjectWEAP>();
					auto baseDamage = Utils::CalcBaseDamage(weapon);
					char buffer[256];
					sprintf(buffer, "%.2f", baseDamage);
					ImGui::Selectable(buffer, &item->selected, select_flags);
				}
				ImGui::TableNextColumn();
				if (item->formType == RE::FormType::Weapon) {
					auto* weapon = item->form->As<RE::TESObjectWEAP>();
					auto speed = weapon->weaponData.speed;
					char buffer[256];
					sprintf(buffer, "%.2f", speed);
					ImGui::Selectable(buffer, &item->selected, select_flags);
				}
				ImGui::TableNextColumn();
				if (item->formType == RE::FormType::Weapon) {
					auto* weapon = item->form->As<RE::TESObjectWEAP>();
					auto critDamage = weapon->GetCritDamage();
					ImGui::Selectable(std::to_string(critDamage).c_str(), &item->selected, select_flags);
				}
				ImGui::TableNextColumn();
				auto skill = item->form->GetObjectTypeName();
				ImGui::Selectable(skill, &item->selected, select_flags);

				ImGui::TableNextColumn();
				auto weight = item->form->GetWeight();
				char wBuffer[256];
				sprintf(wBuffer, "%.2f", weight);
				ImGui::Selectable(wBuffer, &item->selected, select_flags);

				ImGui::TableNextColumn();
				if (item->formType == RE::FormType::Weapon) {
					auto* weapon = item->form->As<RE::TESObjectWEAP>();
					auto baseDamage = Utils::CalcBaseDamage(weapon);
					auto speed = weapon->weaponData.speed;
					auto dps = baseDamage * speed;
					char dpsBuffer[256];
					sprintf(dpsBuffer, "%.2f", dps);
					ImGui::Selectable(dpsBuffer, &item->selected, select_flags);
				}

				auto curRow = ImGui::TableGetHoveredRow();

				if (curRow == ImGui::TableGetRowIndex()) {
					ImGui::PushFont(a_style.tooltipFont);
					ShowItemCard(item);
					ImGui::PopFont();
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
				if (ImGui::Selectable(item->name, false, select_flags)) {
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

void AddItemWindow::Init()
{
	// do stuff

	g_DescriptionFrameworkInterface = DescriptionFrameworkAPI::GetDescriptionFrameworkInterface001();
}