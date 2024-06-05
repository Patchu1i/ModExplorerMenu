#include "AddItemWindow.h"
#include "Console.h"
#include "Settings.h"
#include "lib/Graphic.h"
#include "lib/Util.h"

// Main Draw function for AddItem, called by Frame::Draw()
void AddItemWindow::Draw(Settings::Style& a_style, Settings::Config& a_config)
{
	constexpr auto _flags = ImGuiOldColumnFlags_NoResize;
	ImGui::BeginColumns("##HorizontalSplit", 2, _flags);

	const float width = ImGui::GetWindowWidth();
	ImGui::SetColumnWidth(0, width * 0.75f);

	// Left Column
	ShowSearch(a_style, a_config);
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
	ShowOptions(a_style, a_config);
	ImGui::NewLine();
	ShowFormTable(a_style, a_config);
	ImGui::NextColumn();
	ShowActions(a_style, a_config);
	ImGui::EndColumns();

	if (openBook != nullptr) {
		ShowBookPreview();
	}

	if (b_ShowPlot) {
		ShowPlotGraph();
	}

	if (b_ShowHistogram) {
		ShowHistogramGraph();
	}
}

void AddItemWindow::ShowBookPreview()
{
	auto& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f));

	constexpr auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration;
	if (ImGui::Begin("##ReadBookFromAIM", nullptr, flags)) {
		const auto desc = openBook->form->As<RE::TESDescription>();
		RE::BSString buf;
		desc->GetDescription(buf, nullptr);
		std::string bufStr = std::string(buf);
		Utils::RemoveHTMLTags(bufStr);
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - ImGui::CalcTextSize(openBook->name).x * 0.5f);
		ImGui::Text(openBook->name);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		ImGui::TextWrapped(bufStr.c_str());
	}

	// Close if we click outside.
	if (ImGui::IsMouseClicked(0, true)) {
		if (!ImGui::IsWindowHovered()) {
			openBook = nullptr;
		}
	}

	ImGui::End();
}

void AddItemWindow::ShowHistogramGraph()
{
	if (histogramData.empty()) {
		histogramData.clear();
		logger::info("Adding data to histogram");
		for (const auto& item : itemList) {
			const auto armorRating = Utils::CalcBaseArmorRating(item->form->As<RE::TESObjectARMO>());
			if (armorRating > 30.0f) {
				continue;
			}
			histogramData.push_back(armorRating);
			histogramBinCount++;
		}

		std::sort(histogramData.begin(), histogramData.end());
	}

	// Create the graph
	auto& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f));
	constexpr auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration;
	ImGui::Begin("Show Histogram Data", nullptr, flags);
	ImGui::PlotHistogram("Histogram", histogramData.data(), histogramBinCount, 0, nullptr, 0.0f, 100.0f, ImGui::GetWindowSize());
	ImGui::End();

	// Close if we click outside.
	if (ImGui::IsMouseClicked(0, true)) {
		if (!ImGui::IsWindowHovered()) {
			b_ShowHistogram = false;
		}
	}
}

void AddItemWindow::ShowPlotGraph()
{
	if (plotA.empty() || plotB.empty()) {
		plotA.clear();
		plotB.clear();

		for (const auto& item : itemList) {
			plotA.push_back(item->weight);
			plotB.push_back((float)(item->form->GetGoldValue()));
		}
	}

	// Then, find the maximum values for scaling
	float maxWeight = *std::max_element(plotA.begin(), plotA.end());
	float maxGoldValue = *std::max_element(plotB.begin(), plotB.end());

	// Create the graph
	auto& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f));
	constexpr auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration;
	if (ImGui::Begin("Weight vs Gold Value", nullptr, flags)) {
		for (size_t i = 0; i < itemList.size(); ++i) {
			// Scale the values to fit in the graph
			float x = plotA[i] / maxWeight;
			float y = plotB[i] / maxGoldValue;

			// Convert the scaled values to screen coordinates
			ImVec2 pos = ImGui::GetCursorScreenPos();
			float graphWidth = ImGui::GetWindowWidth();
			float graphHeight = ImGui::GetWindowHeight();
			x = pos.x + x * graphWidth;
			y = pos.y + (1.0f - y) * graphHeight;  // Flip y axis

			// Draw a small circle at the data point
			ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(x, y), 3.0f, IM_COL32(255, 0, 0, 255));
		}
	}

	// Close if we click
	if (ImGui::IsMouseClicked(0)) {
		b_ShowPlot = false;
	}

	ImGui::End();
}

// Draws a Copy to Clipboard button on Context popup.
void AddItemWindow::ShowItemListContextMenu(MEMData::CachedItem& a_item)
{
	constexpr auto flags = ImGuiSelectableFlags_DontClosePopups;
	ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));

	if (ImGui::Selectable("Copy Form ID", false, flags)) {
		ImGui::LogToClipboard();
		ImGui::LogText(a_item.formid.c_str());
		ImGui::LogFinish();
		ImGui::CloseCurrentPopup();
	}

	if (ImGui::Selectable("Copy Name", false, flags)) {
		ImGui::LogToClipboard();
		ImGui::LogText(a_item.name);
		ImGui::LogFinish();
		ImGui::CloseCurrentPopup();
	}

	if (ImGui::Selectable("Copy Editor ID", false, flags)) {
		ImGui::LogToClipboard();
		ImGui::LogText(a_item.editorid.c_str());
		ImGui::LogFinish();
		ImGui::CloseCurrentPopup();
	}

	if (a_item.formType == RE::FormType::Book) {
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		if (ImGui::Selectable("Read Me!")) {
			openBook = &a_item;
		}
	}

	ImGui::PopStyleVar(1);
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
	case ColumnID_BaseDamage:
		if (v1->formType == RE::FormType::Weapon && v2->formType == RE::FormType::Weapon) {
			auto* weapon1 = v1->form->As<RE::TESObjectWEAP>();
			auto* weapon2 = v2->form->As<RE::TESObjectWEAP>();
			auto baseDamage1 = Utils::CalcBaseDamage(weapon1);
			auto baseDamage2 = Utils::CalcBaseDamage(weapon2);
			delta = (baseDamage1 < baseDamage2) ? -1 : (baseDamage1 > baseDamage2) ? 1 :
			                                                                         0;
		} else if (v1->formType == RE::FormType::Weapon) {
			delta = 1;
		} else if (v2->formType == RE::FormType::Weapon) {
			delta = -1;
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
			delta = 1;
		} else if (v2->formType == RE::FormType::Weapon) {
			delta = -1;
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
			delta = 1;
		} else if (v2->formType == RE::FormType::Weapon) {
			delta = -1;
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
			delta = 1;
		} else if (v2->formType == RE::FormType::Weapon || v2->formType == RE::FormType::Armor) {
			delta = -1;
		}
		break;
	case ColumnID_Weight:
		delta = (v1->weight < v2->weight) ? -1 : (v1->weight > v2->weight) ? 1 :
		                                                                     0;
		break;
	case ColumnID_DPS:  // yikes
		if (v1->formType == RE::FormType::Weapon && v2->formType == RE::FormType::Weapon) {
			auto* weapon1 = v1->form->As<RE::TESObjectWEAP>();
			auto* weapon2 = v2->form->As<RE::TESObjectWEAP>();
			auto baseDamage1 = Utils::CalcBaseDamage(weapon1);
			auto baseDamage2 = Utils::CalcBaseDamage(weapon2);
			auto speed1 = weapon1->weaponData.speed;
			auto speed2 = weapon2->weaponData.speed;
			auto dps1 = (int)(baseDamage1 * speed1);
			auto dps2 = (int)(baseDamage2 * speed2);
			delta = (dps1 < dps2) ? -1 : (dps1 > dps2) ? 1 :
			                                             0;
		} else if (v1->formType == RE::FormType::Weapon) {
			delta = 1;
		} else if (v2->formType == RE::FormType::Weapon) {
			delta = -1;
		}
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

std::string GetItemDescription(RE::TESForm* form)
{
	std::string s_descFramework = "";
	if (g_DescriptionFrameworkInterface != nullptr) {
		std::string desc = g_DescriptionFrameworkInterface->GetDescription(form);
		if (!desc.empty()) {
			Utils::RemoveHTMLTags(desc);
			s_descFramework = std::string(desc) + "\n";
		}
	}

	std::string s_tesDescription = "";
	if (form->As<RE::TESDescription>() != nullptr) {
		const auto desc = form->As<RE::TESDescription>();
		if (desc) {
			RE::BSString buf;
			desc->GetDescription(buf, nullptr);

			if (form->formType == RE::FormType::Book) {
				s_tesDescription = "[Right Click -> Read Me!]";
			} else if (!buf.empty()) {
				s_tesDescription = std::string(buf) + "\n";
			}
		}
	}

	return s_descFramework + s_tesDescription;
}

// FIXME: Width get's messed up on ultra long titles.
void AddItemWindow::ShowItemCard(MEMData::CachedItem* item)
{
	//ImGui::SetNextWindowSize(ImVec2(10, 10));
	if (ImGui::BeginTooltip()) {
		ImVec2 barSize = ImVec2(100.0f, ImGui::GetFontSize());
		float popWidth = barSize.x * 3;

		const auto formType = item->formType;

		constexpr auto ProgressColor = [](const double value, const float max_value) -> ImVec4 {
			const float dampen = 0.7f;
			const float ratio = (float)value / max_value;
			const float r = 1.0f - ratio * dampen;
			const float g = ratio * dampen;
			return ImVec4(r, g, 0.0f, 1.0f);
		};

		const auto InlineBar = [popWidth, barSize, ProgressColor](const char* label, const float value, const float max_value) {
			ImGui::Text(label);
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

		const auto InlineInt = [popWidth, barSize](const char* label, const int value) {
			ImGui::Text(label);
			ImGui::SameLine(popWidth - ImGui::CalcTextSize(std::to_string(value).c_str()).x);
			ImGui::Text("%d", value);
		};

		const auto InlineText = [popWidth](const char* label, const char* text) {
			ImGui::Text(label);
			ImGui::SameLine(popWidth - ImGui::CalcTextSize(text).x);
			ImGui::Text(text);
		};

		if (item == nullptr) {
			ImGui::EndTooltip();
			return;
		}

		// Header
		ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(item->name));
		ImGui::Text(item->name);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		if (formType == RE::FormType::Armor) {
			auto* armor = item->form->As<RE::TESObjectARMO>();

			if (armor == nullptr) {
				ImGui::EndTooltip();
				return;
			}

			const auto armorType = Utils::GetArmorType(armor);
			const float armorRating = Utils::CalcBaseArmorRating(armor);
			const float armorRatingMax = Utils::CalcMaxArmorRating(armorRating, 50);
			const auto equipSlot = Utils::GetArmorSlot(armor);
			const int weight = (int)(item->weight);

			if (armorRating == 0) {
				InlineText("Armor Rating:", "None");
			} else {
				InlineBar("Armor Rating:", armorRating, armorRatingMax);
			}

			InlineText("Armor Type:", armorType);
			InlineText("Equip Slot:", equipSlot);

			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			InlineInt("Weight:", weight);
		}

		if (formType == RE::FormType::Weapon) {
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

			const float damage = Utils::CalcBaseDamage(weapon);
			const float max_damage = Utils::CalcMaxDamage(damage, 50);
			const float speed = weapon->weaponData.speed;
			const int weight = (int)(item->weight);
			const int dps = (int)(damage * speed);
			const uint16_t critDamage = weapon->GetCritDamage();
			const RE::ActorValue skill = weapon->weaponData.skill.get();
			const auto type = weaponTypes[static_cast<int>(weapon->GetWeaponType())];

			if (weapon->IsStaff()) {
				InlineText("Base Damage:", "N/A");
			} else if (weapon->IsBow() || weapon->IsCrossbow()) {
				InlineBar("Base Damage:", damage, max_damage);
				InlineBar("Draw Speed:", speed, 1.5f);
				InlineInt("DPS:", dps);
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				InlineInt("Critical Damage:", critDamage);
				InlineText("Skill:", std::to_string(skill).c_str());
			} else {
				const float reach = (float)(weapon->weaponData.reach);
				const float stagger = weapon->weaponData.staggerValue;
				InlineBar("Base Damage:", damage, max_damage);
				InlineBar("Speed:", speed, 1.5f);
				InlineInt("DPS:", dps);
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				InlineBar("Reach:", reach, 1.5f);
				InlineBar("stagger:", stagger, 2.0f);
				InlineInt("Critical Damage:", critDamage);
				InlineText("Skill:", std::to_string(skill).c_str());
			}

			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			InlineInt("Weight:", weight);
			InlineText("Type:", type);
		}

		// Always show:
		InlineInt("Gold Value:", item->goldValue);

		const std::string desc = GetItemDescription(item->form);
		if (!desc.empty()) {
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			if (formType == RE::FormType::Book) {
				ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(desc.c_str()));  // Read Me!
			}
			ImGui::PushTextWrapPos(popWidth);
			ImGui::TextWrapped(desc.c_str());
			ImGui::PopTextWrapPos();
		}

		ImGui::EndTooltip();
	}
}

// Draw the table of items
void AddItemWindow::ShowFormTable(Settings::Style& a_style, Settings::Config& a_config)
{
	auto results = std::string("Results (") + std::to_string(itemList.size()) + std::string(")");
	ImGui::SeparatorText(results.c_str());

	// It's unfortunate I have to do this with custom column visibility:
	bool noColumns = !a_config.aimShowFavoriteColumn && !a_config.aimShowTypeColumn && !a_config.aimShowFormIDColumn &&
	                 !a_config.aimShowNameColumn && !a_config.aimShowEditorIDColumn && !a_config.aimShowGoldValueColumn &&
	                 !a_config.aimShowBaseDamageColumn && !a_config.aimShowSpeedColumn && !a_config.aimShowCritDamageColumn &&
	                 !a_config.aimShowSkillColumn && !a_config.aimShowWeightColumn && !a_config.aimShowDPSColumn;

	if (noColumns) {
		ImGui::Text("No columns are enabled. Please enable at least one column.");
		return;
	}

	auto rowBG = a_style.showTableRowBG ? ImGuiTableFlags_RowBg : 0;

	ImVec2 table_size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
	if (ImGui::BeginTable("##AddItemWindow::Table", column_count, AddItemTableFlags | rowBG, table_size)) {
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

		ImGui::PushFont(a_style.headerFont.large);
		ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
		for (int column = 0; column < column_count; column++) {
			ImGui::TableSetColumnIndex(column);
			const char* column_name = ImGui::TableGetColumnName(column);  // Retrieve name passed to TableSetupColumn()
			ImGui::PushID(column);
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			if (column != ColumnID_FormID && column != ColumnID_EditorID)
				ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(column_name));

			ImGui::TableHeader(column_name);

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
				ImGui::PushID(table_id.c_str());

				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				// Overwrite color to hide ugly imgui backdrop on image.
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

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
				ImGui::PopStyleVar(2);

				bool _itemSelected = false;

				constexpr auto select_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap;
				ImGui::TableNextColumn();
				ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(item->typeName.c_str()));
				ImGui::Selectable(item->typeName.c_str(), &_itemSelected, select_flags);
				ImGui::TableNextColumn();
				ImGui::Selectable(item->formid.c_str(), &_itemSelected, select_flags);
				ImGui::TableNextColumn();
				ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(item->name));
				ImGui::Selectable(item->name, &_itemSelected, select_flags);
				ImGui::TableNextColumn();
				ImGui::TextWrapped(item->editorid.c_str());
				ImGui::TableNextColumn();
				ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(std::to_string(item->goldValue).c_str()));
				ImGui::Selectable(std::to_string(item->goldValue).c_str(), &_itemSelected, select_flags);
				ImGui::TableNextColumn();
				if (item->formType == RE::FormType::Weapon) {
					auto* weapon = item->form->As<RE::TESObjectWEAP>();
					const auto baseDamage = Utils::CalcBaseDamage(weapon);
					char buffer[12];
					snprintf(buffer, sizeof(buffer), "%.2f", baseDamage);
					ImGui::Selectable(buffer, &_itemSelected, select_flags);
				}
				ImGui::TableNextColumn();
				if (item->formType == RE::FormType::Weapon) {
					auto* weapon = item->form->As<RE::TESObjectWEAP>();
					const auto speed = weapon->weaponData.speed;
					char buffer[12];
					snprintf(buffer, sizeof(buffer), "%.2f", speed);
					ImGui::Selectable(buffer, &_itemSelected, select_flags);
				}
				ImGui::TableNextColumn();
				if (item->formType == RE::FormType::Weapon) {
					auto* weapon = item->form->As<RE::TESObjectWEAP>();
					const uint16_t critDamage = weapon->GetCritDamage();
					ImGui::Selectable(std::to_string(critDamage).c_str(), &_itemSelected, select_flags);
				}
				ImGui::TableNextColumn();
				if (true) {
					auto skill = item->form->GetObjectTypeName();
					ImGui::Selectable(skill, &_itemSelected, select_flags);
				}
				ImGui::TableNextColumn();
				if (true) {
					char buffer[32];
					snprintf(buffer, sizeof(buffer), "%.2f", item->weight);
					ImGui::Selectable(buffer, &_itemSelected, select_flags);
				}
				ImGui::TableNextColumn();
				if (item->formType == RE::FormType::Weapon) {
					auto* weapon = item->form->As<RE::TESObjectWEAP>();
					const float baseDamage = Utils::CalcBaseDamage(weapon);
					const float speed = weapon->weaponData.speed;
					const float dps = baseDamage * speed;
					char buffer[12];
					snprintf(buffer, sizeof(buffer), "%.2f", dps);
					ImGui::Selectable(buffer, &_itemSelected, select_flags);
				}

				auto curRow = ImGui::TableGetHoveredRow();

				if (curRow == ImGui::TableGetRowIndex()) {
					ImGui::PushFont(a_style.tooltipFont.tiny);
					ShowItemCard(item);
					ImGui::PopFont();
				}

				if (ImGui::IsMouseClicked(1, true)) {
					ImGui::OpenPopup("TestItemPopupMenu");
				}

				if (ImGui::BeginPopup("TestItemPopupMenu")) {
					ShowItemListContextMenu(*item);
					ImGui::EndPopup();
				}

				if (b_ClickToAdd && _itemSelected) {
					ConsoleCommand::AddItem(item->formid.c_str(), clickToAddCount);
				} else if (!b_ClickToAdd && _itemSelected) {
					item->selected = true;
				}

				ImGui::PopID();
			}
		}

		ImGui::EndTable();
	}
}

void AddItemWindow::ApplyFilters()
{
	itemList.clear();

	auto& cached_item_list = MEMData::GetItemList();

	char compare[256];
	char input[256];

	strncpy(input, inputBuffer, sizeof(input) - 1);

	bool skip = false;

	// TODO: Implement additional columns
	for (auto& item : cached_item_list) {
		switch (searchKey) {
		case ColumnID_Name:
			strncpy(compare, item.name, sizeof(compare) - 1);
			break;
		case ColumnID_FormID:
			strncpy(compare, item.formid.c_str(), sizeof(compare) - 1);
			break;
		case ColumnID_EditorID:
			strncpy(compare, item.editorid.c_str(), sizeof(compare) - 1);
			break;
		case ColumnID_None:
			skip = true;
			break;
		default:
			strncpy(compare, item.name, sizeof(compare) - 1);
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

		if (strstr(lower_compare, input) != nullptr) {
			itemList.push_back(&item);
			continue;
		}

		if (skip) {
			char _name[256];
			char _editorid[256];
			char _formid[256];

			strcpy(_name, item.name);
			strcpy(_editorid, item.editorid.c_str());
			strcpy(_formid, item.formid.c_str());

			strlwr(_name);
			strlwr(_editorid);
			strlwr(_formid);

			if (strstr(_name, input) != nullptr) {
				itemList.push_back(&item);
			} else if (strstr(_editorid, input) != nullptr) {
				itemList.push_back(&item);
			} else if (strstr(_formid, input) != nullptr) {
				itemList.push_back(&item);
			}

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

	ImGui::Text("Refine your search:");
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
}

// TODO: Implement more here.
void AddItemWindow::ShowActions(Settings::Style& a_style, Settings::Config& a_config)
{
	(void)a_config;

	ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

	ImGui::BeginChild("##AddItemWindow::Actions", ImVec2(ImGui::GetContentRegionAvail()), ImGuiChildFlags_AlwaysUseWindowPadding);
	ImGui::SeparatorText("Selection:");

	const float button_height = ImGui::GetFontSize() * 1.5f;
	const float button_width = ImGui::GetContentRegionAvail().x;

	if (ImGui::BeginTable("##AddItemWindow::ActionBarSelection", 1, ActionBarFlags, ImVec2(ImGui::GetContentRegionAvail().x, 150.0f))) {
		ImGui::PushFont(a_style.headerFont.large);
		ImGui::TableSetupColumn("Item(s)", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableHeadersRow();
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::PopFont();

		for (auto& item : itemList) {
			if (item->selected) {
				if (ImGui::Selectable(item->name, false, ImGuiSelectableFlags_SpanAllColumns)) {
					item->selected = false;
				};
				ImGui::SetItemTooltip("Click to remove.");
			}
		}
		ImGui::EndTable();
	}

	ImGui::PushFont(a_style.buttonFont.medium);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.5f, a_style.button.w));
	if (ImGui::Button("Clear Selection", ImVec2(button_width, button_height))) {
		for (auto& item : itemList) {
			item->selected = false;
		}
	}
	ImGui::PopStyleColor(1);
	ImGui::PopFont();  // Button font

	ImGui::SeparatorText("Selection:");

	ImGui::PushFont(a_style.buttonFont.medium);
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
			} else {
				item->selected = false;
			}
		}
	}

	if (ImGui::Button("Select All", ImVec2(button_width, button_height))) {
		for (auto& item : itemList) {
			item->selected = true;
		}
	}

	ImGui::HelpMarker(
		"Enabling this will allow you to quickly add items to your inventory by left clicking.\n\n"
		"This will disable the ability to select items for other actions.");
	ImGui::InlineCheckbox("Click to AddItem", &b_ClickToAdd);

	ImGui::NewLine();

	if (b_ClickToAdd) {
		ImGui::HelpMarker(
			"Amount of items to add when clicking on an item.");
		ImGui::InputInt("##AddItemWindow::AddItemPosition", &clickToAddCount);
	}

	if (ImGui::Button("Show Plot", ImVec2(button_width, button_height))) {
		b_ShowPlot = true;
	}

	if (ImGui::Button("Show Histogram", ImVec2(button_width, button_height))) {
		b_ShowHistogram = true;
	}

	if (ImGui::Button("Regenerate", ImVec2(button_width, button_height))) {
		MEMData::GetSingleton()->Run();
		ApplyFilters();
	}

	ImGui::PopFont();  // Button Font
	ImGui::PopStyleVar(2);

	ImGui::EndChild();
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
	g_DescriptionFrameworkInterface = DescriptionFrameworkAPI::GetDescriptionFrameworkInterface001();
}