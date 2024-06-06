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
	ShowModSelection(a_style, a_config);
	ShowAdvancedOptions(a_style, a_config);
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

// BETA feature, so it's ugly.
void AddItemWindow::ShowHistogramGraph()
{
	auto& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f));
	constexpr auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_Modal;
	if (ImGui::Begin("Show Histogram Data", nullptr, flags)) {
		ImGui::TextWrapped(
			"This feature is in beta, and it may not work as expected. "
			"Please report any issues or suggestions to the Mod page as feedback. "
			"Considering it's status, using this feature may result in a CTD. "
			"Use at your own risk, and please don't post about crashes in the comments. "
			"Finally: You have to reselect data after picking a new mod file!");

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		auto modText = selectedMod ? selectedMod->GetFilename().data() : "Filter by mod..";
		if (ImGui::BeginCombo("##AddItemWindow::FilterByMod", modText)) {
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
		ImGui::SameLine();
		auto dataText = histogramData.empty() ? "Select Data" : "Change Data";
		if (ImGui::BeginCombo("##HistogramDataSelection", dataText)) {
			if (ImGui::Selectable("Armor Rating")) {
				histogramData.clear();
				histogramBinCount = 0;
				for (const auto& item : itemList) {
					if (item->formType != RE::FormType::Armor) {
						histogramData.push_back(0.0f);
						continue;
					}

					const auto armorRating = Utils::CalcBaseArmorRating(item->form->As<RE::TESObjectARMO>());

					histogramData.push_back(armorRating);
					histogramBinCount++;
				}
				//std::sort(histogramData.begin(), histogramData.end());
			}
			if (ImGui::Selectable("Gold Value")) {
				histogramData.clear();
				histogramBinCount = 0;
				for (const auto& item : itemList) {
					histogramData.push_back((float)(item->form->GetGoldValue()));
					histogramBinCount++;
				}
				//std::sort(histogramData.begin(), histogramData.end());
			}
			if (ImGui::Selectable("Weight")) {
				histogramData.clear();
				histogramBinCount = 0;
				for (const auto& item : itemList) {
					histogramData.push_back(item->weight);
					histogramBinCount++;
				}
				//std::sort(histogramData.begin(), histogramData.end());
			}
			if (ImGui::Selectable("Base Damage")) {
				histogramData.clear();
				histogramBinCount = 0;
				for (const auto& item : itemList) {
					if (item->formType != RE::FormType::Weapon) {
						histogramData.push_back(0.0f);
						continue;
					}

					const auto baseDamage = Utils::CalcBaseDamage(item->form->As<RE::TESObjectWEAP>());

					histogramData.push_back(baseDamage);
					histogramBinCount++;
				}
				//std::sort(histogramData.begin(), histogramData.end());
			}

			// More to come...

			ImGui::EndCombo();
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		if (histogramData.empty()) {
			ImGui::Text("No data to display.");

			if (ImGui::Button("Close", ImVec2(ImGui::GetContentRegionMax().x, 15.0f))) {
				b_ShowHistogram = false;
			}

			ImGui::End();
			return;
		}

		//auto max = *std::max_element(histogramData.begin(), histogramData.end());
		//auto min = *std::min_element(histogramData.begin(), histogramData.end());
		//ImGui::PlotHistogram("Histogram", histogramData.data(), histogramBinCount, 0, nullptr, min, max, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 20.0f));
		if (ImPlot::BeginPlot("Histogram", nullptr, nullptr, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 20.0f))) {
			ImPlot::SetupAxes("Item", "Value", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
			//ImPlot::PlotBarGroups(ilabels, histogramData.data(), histogramBinCount, 1, 10.0);
			ImPlot::SetupAxisZoomConstraints(ImAxis_X1, 0.0, histogramBinCount);
			ImPlot::PlotBars("Histogram", histogramData.data(), histogramBinCount, 1, 10.0);

			int select = (int)(std::floor(ImPlot::GetPlotMousePos().x));
			if (select < itemList.size()) {
				auto item = itemList.at(select);

				if (ImPlot::IsPlotHovered() && item != nullptr) {
					ShowItemCard(item);
				}
			} else {
				logger::info("Select: {}", select);
			}
			ImPlot::EndPlot();
		}

		// Close if we click outside.
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
		if (ImGui::Button("Close", ImVec2(ImGui::GetContentRegionMax().x, 15.0f))) {
			b_ShowHistogram = false;
		}
		ImGui::PopStyleColor(1);
	}
	ImGui::End();
}

// DEPRECATED: Should consider using ImPlot library.
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
// TODO: Add more details to other items.
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

	ImGuiContext& g = *ImGui::GetCurrentContext();

	ImVec2 table_size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
	if (ImGui::BeginTable("##AddItemWindow::Table", column_count, AddItemTableFlags | rowBG, table_size)) {
		ImGui::TableSetupScrollFreeze(1, 1);
		ImGui::TableSetupColumn(" ", ImGuiTableColumnFlags_WidthFixed, 18.0f, ColumnID_Favorite);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch, 0.0f, ColumnID_Type);
		ImGui::TableSetupColumn("Form ID", ImGuiTableColumnFlags_WidthFixed, 50.0f, ColumnID_FormID);
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 50.0f, ColumnID_Name);
		ImGui::TableSetupColumn("Editor ID", ImGuiTableColumnFlags_WidthStretch, 50.0f, ColumnID_EditorID);
		ImGui::TableSetupColumn("Gold", ImGuiTableColumnFlags_WidthFixed, 20.0f, ColumnID_GoldValue);
		ImGui::TableSetupColumn("DMG", ImGuiTableColumnFlags_WidthFixed, 20.0f, ColumnID_BaseDamage);
		ImGui::TableSetupColumn("Speed", ImGuiTableColumnFlags_WidthFixed, 20.0f, ColumnID_Speed);
		ImGui::TableSetupColumn("Crit", ImGuiTableColumnFlags_WidthFixed, 20.0f, ColumnID_CritDamage);
		ImGui::TableSetupColumn("Skill", ImGuiTableColumnFlags_WidthFixed, 20.0f, ColumnID_Skill);
		ImGui::TableSetupColumn("Weight", ImGuiTableColumnFlags_WidthFixed, 20.0f, ColumnID_Weight);
		ImGui::TableSetupColumn("DPS", ImGuiTableColumnFlags_WidthFixed, 20.0f, ColumnID_DPS);

		ImGui::PushFont(a_style.font.medium);
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

		ImGuiTable* table = g.CurrentTable;

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
					const auto imageSize = ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize());
					if (ImGui::ImageButton("##AddItemWindow::FavoriteButton", favorite_state, imageSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(col, col, col, col))) {
						item->favorite = !item->favorite;
					}
				} else {
					ImGui::Checkbox("##AddItemWindow::FavoriteCheckbox", &item->favorite);
				}

				ImGui::PopStyleColor(3);
				ImGui::PopStyleVar(2);

				bool _itemSelected = false;

				//	Type Name
				ImGui::TableNextColumn();
				ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(item->typeName.c_str()));
				ImGui::Text(item->typeName.c_str());

				// Form ID
				ImGui::TableNextColumn();
				ImGui::Text(item->formid.c_str());

				// Item Name
				ImGui::TableNextColumn();
				ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(item->name));
				ImGui::Text(item->name);

				// Editor ID
				ImGui::TableNextColumn();
				ImGui::Text(item->editorid.c_str());

				// Gold Value
				ImGui::TableNextColumn();
				ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(std::to_string(item->goldValue).c_str()));
				ImGui::Text(std::to_string(item->goldValue).c_str());

				// Base Damage
				ImGui::TableNextColumn();
				if (item->formType == RE::FormType::Weapon) {
					auto* weapon = item->form->As<RE::TESObjectWEAP>();
					const auto baseDamage = Utils::CalcBaseDamage(weapon);
					char buffer[12];
					snprintf(buffer, sizeof(buffer), "%.2f", baseDamage);
					ImGui::Text(buffer);  // Base Damage
				}

				// Weapon Speed
				ImGui::TableNextColumn();
				if (item->formType == RE::FormType::Weapon) {
					auto* weapon = item->form->As<RE::TESObjectWEAP>();
					const auto speed = weapon->weaponData.speed;
					char buffer[12];
					snprintf(buffer, sizeof(buffer), "%.2f", speed);
					ImGui::Text(buffer);
				}

				// Crit Damage
				ImGui::TableNextColumn();
				if (item->formType == RE::FormType::Weapon) {
					auto* weapon = item->form->As<RE::TESObjectWEAP>();
					const uint16_t critDamage = weapon->GetCritDamage();
					ImGui::Text(std::to_string(critDamage).c_str());
				}

				// Skill Type
				ImGui::TableNextColumn();
				if (true) {
					auto skill = item->form->GetObjectTypeName();
					ImGui::Text(skill);
				}

				// Item Weight
				ImGui::TableNextColumn();
				if (true) {
					char buffer[32];
					snprintf(buffer, sizeof(buffer), "%.2f", item->weight);
					ImGui::Text(buffer);
				}

				// Weapon DPS
				ImGui::TableNextColumn();
				if (item->formType == RE::FormType::Weapon) {
					auto* weapon = item->form->As<RE::TESObjectWEAP>();
					const float baseDamage = Utils::CalcBaseDamage(weapon);
					const float speed = weapon->weaponData.speed;
					const float dps = baseDamage * speed;
					char buffer[12];
					snprintf(buffer, sizeof(buffer), "%.2f", dps);
					ImGui::Text(buffer);
				}

				// Input Handlers
				auto curRow = ImGui::TableGetHoveredRow();
				if (curRow == ImGui::TableGetRowIndex()) {
					ImGui::PushFont(a_style.font.tiny);
					ShowItemCard(item);
					ImGui::PopFont();

					if (ImGui::IsMouseClicked(0)) {
						_itemSelected = true;
					}

					if (ImGui::IsMouseClicked(1, true)) {
						ImGui::OpenPopup("TestItemPopupMenu");
					}
				}

				if (ImGui::BeginPopup("TestItemPopupMenu")) {
					ShowItemListContextMenu(*item);
					ImGui::EndPopup();
				}

				// Shortcut Handlers
				if (b_ClickToAdd && _itemSelected) {
					ConsoleCommand::AddItem(item->formid.c_str(), clickToAddCount);
				} else if (b_ClickToPlace && _itemSelected) {
					ConsoleCommand::PlaceAtMe(item->formid.c_str(), clickToPlaceCount);
				} else if (b_ClickToFavorite && _itemSelected) {
					item->favorite = !item->favorite;
				} else if (!b_ClickToAdd && _itemSelected) {
					item->selected = true;
				}

				// https://github.com/ocornut/imgui/issues/6588#issuecomment-1634424774
				// Sloppy way to handle row highlighting since ImGui natively doesn't support it.
				ImRect row_rect(
					table->WorkRect.Min.x,
					table->RowPosY1,
					table->WorkRect.Max.x,
					table->RowPosY2);
				row_rect.ClipWith(table->BgClipRect);

				bool bHover =
					ImGui::IsMouseHoveringRect(row_rect.Min, row_rect.Max, false) &&
					ImGui::IsWindowHovered(ImGuiHoveredFlags_None) &&
					!ImGui::IsAnyItemHovered();  // optional

				if (bHover) {
					table->RowBgColor[1] = ImGui::GetColorU32(ImGuiCol_Border);
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
		ImGui::PushFont(a_style.font.medium);
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
				ImGui::SetDelayedTooltip("Click to remove.");
			}
		}
		ImGui::EndTable();
	}

	ImGui::PushFont(a_style.font.medium);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.5f, a_style.button.w));
	if (ImGui::Button("Clear Selection", ImVec2(button_width, button_height))) {
		for (auto& item : itemList) {
			item->selected = false;
		}
	}
	ImGui::PopStyleColor(1);
	ImGui::PopFont();  // Button font

	ImGui::SeparatorText("Selection:");

	ImGui::PushFont(a_style.font.medium);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 1.0f, 0.5f, a_style.button.w));

	ImGui::InlineCheckbox("Sticky Select", &b_StickySelect);
	ImGui::SetDelayedTooltip(
		"Enabling this will keep your selected items select when you interact with\n"
		"actions shown in this sidebar. If this is disabled, items will be cleared once\n"
		"they are acted upon");
	ImGui::NewLine();

	if (ImGui::Button("Add to Inventory", ImVec2(button_width, button_height))) {
		for (auto& item : itemList) {
			if (item->selected) {
				ConsoleCommand::AddItem(item->formid);
				item->selected = b_StickySelect;
				logger::info("Added item: {}", item->formid.c_str());
			}
		}
	}
	ImGui::SetDelayedTooltip(
		"Directly add the selected item(s) above to your inventory.\n"
		"You can optionally enable \"Click To Add\" or \"Click To Place\" to quickly\n"
		"add or place items simply by clicking them in the left pane.");

	if (ImGui::Button("Place At Me", ImVec2(button_width, button_height))) {
		for (auto& item : itemList) {
			if (item->selected) {
				ConsoleCommand::PlaceAtMe(item->formid);
				item->selected = b_StickySelect;
			}
		}
	}
	ImGui::SetDelayedTooltip(
		"Directly place the selected item(s) above in the world.\n"
		"You can optionally enable \"Click To Add\" or \"Click To Place\" to quickly\n"
		"add or place items simply by clicking them in the left pane.");

	if (ImGui::Button("Add to Favorites", ImVec2(button_width, button_height))) {
		for (auto& item : itemList) {
			if (item->selected) {
				item->favorite = !item->favorite;
			}
		}
	}
	ImGui::SetDelayedTooltip(
		"Add the selected items above to your favorites.\n"
		"Clicking this repeatedly will toggle the favorite status of the selected items.");
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
	ImGui::SetDelayedTooltip(
		"Select all favorited items from the currently indexed search results.");

	if (ImGui::Button("Select All", ImVec2(button_width, button_height))) {
		for (auto& item : itemList) {
			item->selected = true;
		}
	}

	ImGui::SeparatorText("Shortcuts:");

	ImGui::SetDelayedTooltip(
		"Select all items from the currently indexed search results.");
	ImGui::InlineCheckbox("Click to Add", &b_ClickToAdd);

	ImGui::SetDelayedTooltip(
		"Enabling this will allow you to quickly add items to your inventory by left clicking.\n\n"
		"This will disable the ability to select items for other actions.");

	ImGui::NewLine();

	if (b_ClickToAdd) {
		ImGui::InputInt("##AddItemWindow::ClickToAddInput", &clickToAddCount);
		ImGui::SetDelayedTooltip(
			"Amount of items to add when clicking on an item.");
	}

	ImGui::InlineCheckbox("Click to Place", &b_ClickToPlace);

	ImGui::SetDelayedTooltip(
		"Enabling this will allow you to quickly place items in the world by left clicking.\n\n"
		"This will disable the ability to select items for other actions.");

	ImGui::NewLine();

	if (b_ClickToPlace) {
		ImGui::InputInt("##AddItemWindow::ClickToPlaceInput", &clickToPlaceCount);
		ImGui::SetDelayedTooltip(
			"Amount of items to place when clicking on an item.");
	}

	ImGui::InlineCheckbox("Click to Favorite", &b_ClickToFavorite);

	ImGui::SetDelayedTooltip(
		"Enabling this will allow you to quickly favorite items by left clicking.\n\n"
		"This will disable the ability to select items for other actions.");

	ImGui::PopFont();  // Button Font
	ImGui::PopStyleVar(2);

	ImGui::EndChild();
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

		ImGui::Unindent();
		ImGui::NewLine();
	}
}

void AddItemWindow::ShowModSelection(Settings::Style& a_style, Settings::Config& a_config)
{
	(void)a_style;
	(void)a_config;

	if (ImGui::CollapsingHeader("Filter by Mod:")) {
		ImGui::NewLine();
		ImGui::Indent();
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
			ImGui::InlineCheckbox("Type", &a_config.aimShowTypeColumn);
			ImGui::InlineCheckbox("Form", &a_config.aimShowFormIDColumn);
			ImGui::InlineCheckbox("Name", &a_config.aimShowNameColumn);
			ImGui::NewLine();
			ImGui::InlineCheckbox("Editor", &a_config.aimShowEditorIDColumn);
			ImGui::InlineCheckbox("Gold", &a_config.aimShowGoldValueColumn);
			ImGui::InlineCheckbox("Damage", &a_config.aimShowBaseDamageColumn);
			ImGui::InlineCheckbox("Speed", &a_config.aimShowSpeedColumn);
			ImGui::NewLine();
			ImGui::InlineCheckbox("Crit", &a_config.aimShowCritDamageColumn);
			ImGui::InlineCheckbox("Skill", &a_config.aimShowSkillColumn);
			ImGui::InlineCheckbox("Weight", &a_config.aimShowWeightColumn);
			ImGui::InlineCheckbox("DPS", &a_config.aimShowDPSColumn);
			ImGui::NewLine();
			ImGui::PopStyleColor(1);
			ImGui::PopStyleVar(1);
		}

		if (ImGui::CollapsingHeader("(Beta) Plot & Histogram:", flags)) {
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, a_style.frameBorderSize);
			ImGui::PushStyleColor(ImGuiCol_Header, a_style.frameBg);
			const ImVec2 button_size = ImVec2(ImGui::GetContentRegionAvail().x / 3, ImGui::GetFontSize() * 1.15f);
			ImGui::HelpMarker(
				"This will show a plot diagram of the current data set.\n\n"
				"This is a BETA feature meaning it is hardly implemented/tested as of now.");
			if (ImGui::Button("Show Plot", button_size)) {
				// b_ShowPlot = !b_ShowPlot; Kinda shit, but I'll tease it anyway.
			}

			ImGui::HelpMarker(
				"This will show a histogram window of the current data set.\n\n"
				"This is a BETA feature meaning it is hardly implemented/tested as of now.");
			if (ImGui::Button("Show Histogram", button_size)) {
				b_ShowHistogram = !b_ShowHistogram;
			}

			ImGui::HelpMarker(
				"Regenerate the list of cached forms from the game. This should only ever be needed if runtime changes are made.\n\n"
				"(WARNING): This will take a second or two to complete and will freeze your game in doing so.");
			if (ImGui::Button("Regenerate Cache", button_size)) {
				MEMData::GetSingleton()->Run();
				ApplyFilters();
			}
			ImGui::PopStyleColor(1);
			ImGui::PopStyleVar(1);
		}

		ImGui::PopStyleColor(1);
		ImGui::PopStyleVar(1);
		ImGui::Unindent();
		ImGui::NewLine();
	}
}

void AddItemWindow::Init()
{
	g_DescriptionFrameworkInterface = DescriptionFrameworkAPI::GetDescriptionFrameworkInterface001();
}