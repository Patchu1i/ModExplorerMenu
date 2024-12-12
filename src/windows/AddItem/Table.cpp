#include "AddItem.h"
#include "Console.h"
#include "Utils/Util.h"
#include "Windows/Persistent.h"

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

	// Draw the table of items
	void AddItemWindow::ShowFormTable(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_config;

		auto results = std::string("Results (") + std::to_string(itemList.size()) + std::string(")");
		ImGui::SeparatorText(results.c_str());

		auto rowBG = a_style.showTableRowBG ? ImGuiTableFlags_RowBg : 0;
		ImVec2 table_size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
		// auto extendWidth = (columnList.EnabledColumns() > 4) ? columnList.EnabledColumns() * 50.0f : 0;
		if (ImGui::BeginTable("##AddItemWindow::Table", columnList.GetTotalColumns(), AddItemTableFlags | rowBG, table_size, table_size.x)) {
			ImGui::TableSetupScrollFreeze(1, 1);
			for (auto& column : columnList.columns) {
				ImGui::TableSetupColumn(column.name.c_str(), column.flags, column.width, column.key);
			}

			ImGui::PushFont(a_style.font.medium);
			ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
			int column_n = 0;
			for (auto& column : columnList.columns) {
				ImGui::TableSetColumnIndex(column_n);
				ImGui::PushID(column.key);
				ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);  // (?)

				if (column.key != BaseColumn::ID::FormID && column.key != BaseColumn::ID::EditorID) {
					ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(column.name.c_str()));
				}

				ImGui::TableHeader(column.name.c_str());
				ImGui::PopID();

				column_n++;

				// Saving for later if I need to refer to column state.
				// if (ImGui::TableGetColumnFlags(column_n) & ImGuiTableColumnFlags_IsEnabled) {
				// 	logger::info("Column {} is enabled", column.name);
				// }

				// This works, but disables context functionality. Would have to create
				// explicit header buttons to enable/disable columns.
				//ImGui::TableSetColumnEnabled(column_n, *column.enabled);
			}
			ImGui::PopFont();

			if (dirty) {
				ImGui::TableGetSortSpecs()->SpecsDirty = true;
			}

			// Sort our data if sort specs have been changed!
			if (ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs()) {
				if (sort_specs->SpecsDirty) {
					SortColumnsWithSpecs<std::vector<Item*>, Item>(itemList, sort_specs);
					sort_specs->SpecsDirty = false;
					dirty = false;
				}
			}

			ImGuiListClipper clipper;
			ImGuiContext& g = *ImGui::GetCurrentContext();
			ImGuiTable* table = g.CurrentTable;

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
					bool clickToAct = b_AddToInventory || b_PlaceOnGround || b_AddToFavorites;

					if (favorite_state != nullptr) {
						const auto imageSize = ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize());
						if (ImGui::DisabledImageButton("##AddItemWindow::FavoriteButton", clickToAct, favorite_state, imageSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(col, col, col, col))) {
							if (!clickToAct) {
								item->favorite = !item->favorite;
							}
						}
					} else {
						ImGui::DisabledCheckbox("##AddItemWindow::FavoriteCheckbox", clickToAct, item->favorite);
					}

					ImGui::PopStyleColor(3);
					ImGui::PopStyleVar(2);

					bool _itemSelected = false;

					//	Plugin
					ImGui::TableNextColumn();
					ImGui::Text(item->GetPluginName().data());

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
						snprintf(buffer, sizeof(buffer), "%.0f", baseDamage);
						ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(buffer));
						ImGui::Text(buffer);  // Base Damage
					}

					// Armor Rating
					ImGui::TableNextColumn();
					if (item->GetFormType() == RE::FormType::Armor) {
						auto* armor = item->TESForm->As<RE::TESObjectARMO>();
						const auto armorRating = Utils::CalcBaseArmorRating(armor);
						char buffer[12];
						snprintf(buffer, sizeof(buffer), "%.0f", armorRating);
						ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(buffer));
						ImGui::Text(buffer);
					}

					// Weapon Speed
					ImGui::TableNextColumn();
					if (item->GetFormType() == RE::FormType::Weapon) {
						auto* weapon = item->TESForm->As<RE::TESObjectWEAP>();
						const auto speed = weapon->weaponData.speed;
						char buffer[12];
						snprintf(buffer, sizeof(buffer), "%.2f", speed);
						ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(buffer));
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
						snprintf(buffer, sizeof(buffer), "%.0f", item->GetWeight());
						ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(buffer));
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
						ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(buffer));
						ImGui::Text(buffer);
					}

					// Input Handlers
					auto curRow = ImGui::TableGetHoveredRow();
					if (curRow == ImGui::TableGetRowIndex()) {
						ImGui::PushFont(a_style.font.tiny);
						itemPreview = item;
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
					if (b_AddToInventory && _itemSelected) {
						Console::AddItem(item->GetFormID().c_str(), clickToAddCount);
						Console::StartProcessThread();
					} else if (b_PlaceOnGround && _itemSelected) {
						Console::PlaceAtMe(item->GetFormID().c_str(), clickToAddCount);
						Console::StartProcessThread();
					} else if (b_AddToFavorites && _itemSelected) {
						item->favorite = !item->favorite;
						PersistentData::GetSingleton()->UpdatePersistentData<Item*>(item);
					} else if (!b_AddToInventory && _itemSelected) {
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