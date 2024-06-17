#include "AddItem.h"
#include "Console.h"
#include "Utils/Util.h"
#include "Windows/ItemCards.h"


namespace ModExplorerMenu
{
	// Draws a Copy to Clipboard button on Context popup.
	void AddItemWindow::ShowItemListContextMenu(Item& a_item)
	{
		constexpr auto flags = ImGuiSelectableFlags_DontClosePopups;
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));

		if (ImGui::Selectable("Copy Form ID", false, flags)) {
			ImGui::LogToClipboard();
			ImGui::LogText(a_item.GetFormID().c_str());
			ImGui::LogFinish();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Selectable("Copy Name", false, flags)) {
			ImGui::LogToClipboard();
			ImGui::LogText(a_item.GetName().data());
			ImGui::LogFinish();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Selectable("Copy Editor ID", false, flags)) {
			ImGui::LogToClipboard();
			ImGui::LogText(a_item.GetEditorID().data());
			ImGui::LogFinish();
			ImGui::CloseCurrentPopup();
		}

		if (a_item.GetFormType() == RE::FormType::Book) {
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			if (ImGui::Selectable("Read Me!")) {
				openBook = &a_item;
			}
		}

		ImGui::PopStyleVar(1);
	}

	bool AddItemWindow::SortColumn(const Item* v1, const Item* v2)
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
			delta = v1->GetTypeName().compare(v2->GetTypeName());
			break;
		case ColumnID_FormID:  // std::string
			delta = v1->GetFormID().compare(v2->GetFormID());
			break;
		case ColumnID_Name:  // const char *
			delta = v1->GetName().compare(v2->GetName());
			break;
		case ColumnID_EditorID:  // std::string
			delta = v1->GetEditorID().compare(v2->GetEditorID());
			break;
		case ColumnID_GoldValue:  // std::int32_t
			delta = (v1->GetValue() < v2->GetValue()) ? -1 : (v1->GetValue() > v2->GetValue()) ? 1 :
			                                                                                     0;
		case ColumnID_BaseDamage:
			if (v1->GetFormType() == RE::FormType::Weapon && v2->GetFormType() == RE::FormType::Weapon) {
				auto* weapon1 = v1->TESForm->As<RE::TESObjectWEAP>();
				auto* weapon2 = v2->TESForm->As<RE::TESObjectWEAP>();
				auto baseDamage1 = Utils::CalcBaseDamage(weapon1);
				auto baseDamage2 = Utils::CalcBaseDamage(weapon2);
				delta = (baseDamage1 < baseDamage2) ? -1 : (baseDamage1 > baseDamage2) ? 1 :
				                                                                         0;
			} else if (v1->GetFormType() == RE::FormType::Weapon) {
				delta = 1;
			} else if (v2->GetFormType() == RE::FormType::Weapon) {
				delta = -1;
			}
			break;
		case ColumnID_Speed:
			if (v1->GetFormType() == RE::FormType::Weapon && v2->GetFormType() == RE::FormType::Weapon) {
				auto* weapon1 = v1->TESForm->As<RE::TESObjectWEAP>();
				auto* weapon2 = v2->TESForm->As<RE::TESObjectWEAP>();
				auto speed1 = weapon1->weaponData.speed;
				auto speed2 = weapon2->weaponData.speed;
				delta = (speed1 < speed2) ? -1 : (speed1 > speed2) ? 1 :
				                                                     0;
			} else if (v1->GetFormType() == RE::FormType::Weapon) {
				delta = 1;
			} else if (v2->GetFormType() == RE::FormType::Weapon) {
				delta = -1;
			}
			break;
		case ColumnID_ArmorRating:
			if (v1->GetFormType() == RE::FormType::Armor && v2->GetFormType() == RE::FormType::Armor) {
				auto* armor1 = v1->TESForm->As<RE::TESObjectARMO>();
				auto* armor2 = v2->TESForm->As<RE::TESObjectARMO>();
				auto armorRating1 = armor1->armorRating;
				auto armorRating2 = armor2->armorRating;
				delta = (armorRating1 < armorRating2) ? -1 : (armorRating1 > armorRating2) ? 1 :
				                                                                             0;
			} else if (v1->GetFormType() == RE::FormType::Armor) {
				delta = 1;
			} else if (v2->GetFormType() == RE::FormType::Armor) {
				delta = -1;
			}
		case ColumnID_CritDamage:
			if (v1->GetFormType() == RE::FormType::Weapon && v2->GetFormType() == RE::FormType::Weapon) {
				auto* weapon1 = v1->TESForm->As<RE::TESObjectWEAP>();
				auto* weapon2 = v2->TESForm->As<RE::TESObjectWEAP>();
				auto critDamage1 = weapon1->GetCritDamage();
				auto critDamage2 = weapon2->GetCritDamage();
				delta = (critDamage1 < critDamage2) ? -1 : (critDamage1 > critDamage2) ? 1 :
				                                                                         0;
			} else if (v1->GetFormType() == RE::FormType::Weapon) {
				delta = 1;
			} else if (v2->GetFormType() == RE::FormType::Weapon) {
				delta = -1;
			}
			break;
		case ColumnID_Skill:
			if ((v1->GetFormType() == RE::FormType::Weapon && v2->GetFormType() == RE::FormType::Weapon) ||
				(v1->GetFormType() == RE::FormType::Armor && v2->GetFormType() == RE::FormType::Armor) ||
				(v1->GetFormType() == RE::FormType::Weapon && v2->GetFormType() == RE::FormType::Armor) ||
				(v1->GetFormType() == RE::FormType::Armor && v2->GetFormType() == RE::FormType::Weapon)) {
				auto item1 = v1->TESForm->GetObjectTypeName();
				auto item2 = v2->TESForm->GetObjectTypeName();
				delta = strcmp(item1, item2);
			} else if (v1->GetFormType() == RE::FormType::Weapon || v1->GetFormType() == RE::FormType::Armor) {
				delta = 1;
			} else if (v2->GetFormType() == RE::FormType::Weapon || v2->GetFormType() == RE::FormType::Armor) {
				delta = -1;
			}
			break;
		case ColumnID_Weight:
			delta = (v1->GetWeight() < v2->GetWeight()) ? -1 : (v1->GetWeight() > v2->GetWeight()) ? 1 :
			                                                                                         0;
			break;
		case ColumnID_DPS:  // yikes
			if (v1->GetFormType() == RE::FormType::Weapon && v2->GetFormType() == RE::FormType::Weapon) {
				auto* weapon1 = v1->TESForm->As<RE::TESObjectWEAP>();
				auto* weapon2 = v2->TESForm->As<RE::TESObjectWEAP>();
				auto baseDamage1 = Utils::CalcBaseDamage(weapon1);
				auto baseDamage2 = Utils::CalcBaseDamage(weapon2);
				auto speed1 = weapon1->weaponData.speed;
				auto speed2 = weapon2->weaponData.speed;
				auto dps1 = (int)(baseDamage1 * speed1);
				auto dps2 = (int)(baseDamage2 * speed2);
				delta = (dps1 < dps2) ? -1 : (dps1 > dps2) ? 1 :
				                                             0;
			} else if (v1->GetFormType() == RE::FormType::Weapon) {
				delta = 1;
			} else if (v2->GetFormType() == RE::FormType::Weapon) {
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
			ImGui::TableSetupColumn("Armor", ImGuiTableColumnFlags_WidthFixed, 20.0f, ColumnID_ArmorRating);
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
			ImGui::TableSetColumnEnabled(ColumnID_ArmorRating, a_config.aimShowArmorRatingColumn);
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
					ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(item->GetTypeName().c_str()));
					ImGui::Text(item->GetTypeName().c_str());

					// Form ID
					ImGui::TableNextColumn();
					ImGui::Text(item->GetFormID().c_str());

					// Item Name
					ImGui::TableNextColumn();
					ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(item->GetName().data()));
					ImGui::Text(item->GetName().data());

					// Editor ID
					ImGui::TableNextColumn();
					ImGui::Text(item->GetEditorID().data());

					// Gold Value
					ImGui::TableNextColumn();
					ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(item->GetValueString().c_str()));
					ImGui::Text(item->GetValueString().c_str());

					// Base Damage
					ImGui::TableNextColumn();
					if (item->GetFormType() == RE::FormType::Weapon) {
						auto* weapon = item->TESForm->As<RE::TESObjectWEAP>();
						const auto baseDamage = Utils::CalcBaseDamage(weapon);
						char buffer[12];
						snprintf(buffer, sizeof(buffer), "%.2f", baseDamage);
						ImGui::Text(buffer);  // Base Damage
					}

					ImGui::TableNextColumn();
					if (item->GetFormType() == RE::FormType::Armor) {
						auto* armor = item->TESForm->As<RE::TESObjectARMO>();
						const auto armorRating = Utils::CalcBaseArmorRating(armor);
						ImGui::Text(std::to_string(armorRating).c_str());
					}

					// Weapon Speed
					ImGui::TableNextColumn();
					if (item->GetFormType() == RE::FormType::Weapon) {
						auto* weapon = item->TESForm->As<RE::TESObjectWEAP>();
						const auto speed = weapon->weaponData.speed;
						char buffer[12];
						snprintf(buffer, sizeof(buffer), "%.2f", speed);
						ImGui::Text(buffer);
					}

					// Crit Damage
					ImGui::TableNextColumn();
					if (item->GetFormType() == RE::FormType::Weapon) {
						auto* weapon = item->TESForm->As<RE::TESObjectWEAP>();
						const uint16_t critDamage = weapon->GetCritDamage();
						ImGui::Text(std::to_string(critDamage).c_str());
					}

					// Skill Type
					ImGui::TableNextColumn();
					if (true) {
						auto skill = item->TESForm->GetObjectTypeName();
						ImGui::Text(skill);
					}

					// Item Weight
					ImGui::TableNextColumn();
					if (true) {
						char buffer[32];
						snprintf(buffer, sizeof(buffer), "%.2f", item->GetWeight());
						ImGui::Text(buffer);
					}

					// Weapon DPS
					ImGui::TableNextColumn();
					if (item->GetFormType() == RE::FormType::Weapon) {
						auto* weapon = item->TESForm->As<RE::TESObjectWEAP>();
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
						//ShowItemCard(item);
						ShowItemCard<Item>(item);
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
						Console::AddItem(item->GetFormID().c_str(), clickToAddCount);
						Console::StartProcessThread();
					} else if (b_ClickToPlace && _itemSelected) {
						Console::PlaceAtMe(item->GetFormID().c_str(), clickToPlaceCount);
						Console::StartProcessThread();
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
}