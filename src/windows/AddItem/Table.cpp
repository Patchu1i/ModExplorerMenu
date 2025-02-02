#include "include/A/AddItem.h"
#include "include/C/Console.h"
#include "include/P/Persistent.h"

namespace Modex
{
	// Draws a Right-Click context menu for additional behaviors.
	// a_item is the item that was right-clicked (last hovered).
	// We use this to fallback if no items are selected.
	// TODO Pass by const reference!
	void AddItemWindow::ShowItemListContextMenu(ItemData& a_item)
	{
		constexpr auto flags = ImGuiSelectableFlags_DontClosePopups;
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));

		if (activeViewport == Viewport::KitView) {
			if (!itemSelectionList.empty()) {
				if (ImGui::Selectable(_T("KIT_ADD"), false, flags)) {
					for (auto& item : itemSelectionList) {
						const auto tmp = CreateKitItem(item);
						AddItemToKit(tmp);
					}

					ImGui::CloseCurrentPopup();
				}
			}
		}

		if (ImGui::Selectable(_T("AIM_ADD"), false, flags)) {
			if (!itemSelectionList.empty()) {
				for (auto& item : itemSelectionList) {
					Console::AddItem(item->GetFormID().c_str(), clickToAddCount);
				}

				Console::StartProcessThread();
			} else {
				Console::AddItem(a_item.GetFormID().c_str(), clickToAddCount);
				Console::StartProcessThread();
			}

			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Selectable(_T("AIM_PLACE"), false, flags)) {
			if (!itemSelectionList.empty()) {
				for (auto& item : itemSelectionList) {
					Console::PlaceAtMe(item->GetFormID().c_str(), clickToAddCount);
				}

				Console::StartProcessThread();
			} else {
				Console::PlaceAtMe(a_item.GetFormID().c_str(), clickToAddCount);
				Console::StartProcessThread();
			}

			ImGui::CloseCurrentPopup();
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		if (ImGui::Selectable(_T("GENERAL_COPY_FORM_ID"), false, flags)) {
			ImGui::LogToClipboard();
			ImGui::LogText(a_item.GetFormID().c_str());
			ImGui::LogFinish();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Selectable(_T("GENERAL_COPY_NAME"), false, flags)) {
			ImGui::LogToClipboard();
			ImGui::LogText(a_item.GetName().data());
			ImGui::LogFinish();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Selectable(_T("GENERAL_COPY_EDITOR_ID"), false, flags)) {
			ImGui::LogToClipboard();
			ImGui::LogText(a_item.GetEditorID().data());
			ImGui::LogFinish();
			ImGui::CloseCurrentPopup();
		}

		if (a_item.GetFormType() == RE::FormType::Book) {
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			if (ImGui::Selectable(_T("GENERAL_READ_ME"))) {
				openBook = &a_item;
			}
		}

		ImGui::PopStyleVar(1);
	}

	// Draw the table of items
	void AddItemWindow::ShowFormTable()
	{
		auto a_style = Settings::GetSingleton()->GetStyle();

		auto rowBG = a_style.showTableRowBG ? ImGuiTableFlags_RowBg : 0;
		ImVec2 tableSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
		if (ImGui::BeginTable("##AddItemWindow::Table", columnList.GetTotalColumns(), Frame::TABLE_FLAGS | rowBG, tableSize, tableSize.x)) {
			ImGui::TableSetupScrollFreeze(1, 1);

			for (auto& column : columnList.columns) {
				ImGui::TableSetupColumn(column.name.c_str(), column.flags, column.width, column.key);
			}

			ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
			int numOfColumn = 0;
			for (auto& column : columnList.columns) {
				ImGui::TableSetColumnIndex(numOfColumn);
				ImGui::PushID(column.key);
				ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);  // (?)

				if (column.key != BaseColumn::ID::FormID && column.key != BaseColumn::ID::EditorID) {
					ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(column.name.c_str()));
				}

				ImGui::TableHeader(column.name.c_str());
				ImGui::PopID();

				numOfColumn++;
			}

			if (dirty) {
				ImGui::TableGetSortSpecs()->SpecsDirty = true;
			}

			// Sort our data if sort specs have been changed!
			if (ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs()) {
				if (sort_specs->SpecsDirty) {
					SortColumnsWithSpecs<std::vector<ItemData*>, ItemData>(itemList, sort_specs);
					sort_specs->SpecsDirty = false;
					dirty = false;
				}
			}

			ImGuiListClipper clipper;
			ImGuiContext& g = *ImGui::GetCurrentContext();
			ImGuiTable* table = g.CurrentTable;

			int numOfRow = 0;
			clipper.Begin(static_cast<int>(itemList.size()));
			while (clipper.Step()) {
				for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
					auto& item = itemList[row];

					numOfRow++;
					auto tableID = std::string("##AddItemMenu::TableIndex-") + std::to_string(numOfRow);
					ImGui::PushID(tableID.c_str());

					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					// Overwrite color to hide ugly imgui backdrop on image.
					// ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
					// ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
					// ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
					// ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
					// ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

					// if (ImGui::DisabledCheckbox("##AddItemWindow::FavoriteCheckbox", b_AddToFavorites, item->favorite)) {
					// 	PersistentData::GetSingleton()->UpdatePersistentData<ItemData*>(item);
					// }

					// ImGui::PopStyleColor(3);
					// ImGui::PopStyleVar(2);

					//	Plugin
					// ImGui::TableNextColumn();
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
						auto* weapon = item->GetForm()->As<RE::TESObjectWEAP>();
						const auto baseDamage = Utils::CalcBaseDamage(weapon);
						char buffer[12];
						snprintf(buffer, sizeof(buffer), "%.0f", baseDamage);
						ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(buffer));
						ImGui::Text(buffer);
					}

					// Armor Rating
					ImGui::TableNextColumn();
					if (item->GetFormType() == RE::FormType::Armor) {
						auto* armor = item->GetForm()->As<RE::TESObjectARMO>();
						const auto armorRating = Utils::CalcBaseArmorRating(armor);
						char buffer[12];
						snprintf(buffer, sizeof(buffer), "%.0f", armorRating);
						ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(buffer));
						ImGui::Text(buffer);
					}

					// Equip Slot
					ImGui::TableNextColumn();
					if (item->GetFormType() == RE::FormType::Armor) {
						auto* armor = item->GetForm()->As<RE::TESObjectARMO>();
						const auto equipSlots = Utils::GetArmorSlots(armor);
						ImGui::Text(equipSlots[0].c_str());
					}

					// Weapon Speed
					ImGui::TableNextColumn();
					if (item->GetFormType() == RE::FormType::Weapon) {
						auto* weapon = item->GetForm()->As<RE::TESObjectWEAP>();
						const auto speed = weapon->weaponData.speed;
						char buffer[12];
						snprintf(buffer, sizeof(buffer), "%.2f", speed);
						ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(buffer));
						ImGui::Text(buffer);
					}

					// Crit Damage
					ImGui::TableNextColumn();
					if (item->GetFormType() == RE::FormType::Weapon) {
						auto* weapon = item->GetForm()->As<RE::TESObjectWEAP>();
						const uint16_t critDamage = weapon->GetCritDamage();
						ImGui::Text(std::to_string(critDamage).c_str());
					}

					// Skill Type
					ImGui::TableNextColumn();
					if (true) {
						auto skill = item->GetForm()->GetObjectTypeName();
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
						auto* weapon = item->GetForm()->As<RE::TESObjectWEAP>();
						const float baseDamage = Utils::CalcBaseDamage(weapon);
						const float speed = weapon->weaponData.speed;
						const float dps = baseDamage * speed;
						char buffer[12];
						snprintf(buffer, sizeof(buffer), "%.2f", dps);
						ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(buffer));
						ImGui::Text(buffer);
					}

					// https://github.com/ocornut/imgui/issues/6588#issuecomment-1634424774
					// 1/11/2025 - Merged Table Input Handler function into this to better detect selected item.
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

					// Get the rows which the selection box is hovering over.
					// Two conditions based on inverted selection box.
					if (InputManager::GetMouseState() == InputManager::MOUSE_STATE::kBoxSelection) {
						// const auto selection_box = ImRect(InputManager::GetMouseSelectionStart(), InputManager::GetMouseSelectionEnd());
						const auto start = InputManager::GetMouseSelectionStart();
						const auto end = InputManager::GetMouseSelectionEnd();

						if (start.y > end.y) {
							if (row_rect.Overlaps(ImRect(end, start))) {
								if (!itemSelectionList.contains(item)) {
									itemSelectionList.insert(item);
								}
							} else {
								if (itemSelectionList.contains(item)) {
									itemSelectionList.erase(item);
								}
							}
						} else if (row_rect.Overlaps(ImRect(start, end))) {
							if (!itemSelectionList.contains(item)) {
								itemSelectionList.insert(item);
							}
						} else {
							if (itemSelectionList.contains(item)) {
								itemSelectionList.erase(item);
							}
						}
					}

					if (itemSelectionList.contains(item)) {
						table->RowBgColor[1] = ImGui::GetColorU32(ImGuiCol_Border, 0.65f);
					}

					if (bHover) {
						table->RowBgColor[1] = ImGui::GetColorU32(ImGuiCol_Border);
						itemPreview = item;

						if (ImGui::IsMouseClicked(1, true)) {
							ImGui::OpenPopup("ShowItemContextMenu");
						}
					}

					if (kitHoveredInTableView != nullptr) {
						for (auto& item_in_kit : kitHoveredInTableView->items) {
							if (item_in_kit->editorid == item->GetEditorID()) {
								const float alpha = Utils::PulseMinMax(static_cast<float>(ImGui::GetTime()), 5.0f, 2.0f, 0.2f, 0.8f);
								table->RowBgColor[1] = ImGui::GetColorU32(ImGuiCol_Border, alpha);
							}
						}
					}

					if (ImGui::BeginPopup("ShowItemContextMenu")) {
						ShowItemListContextMenu(*item);
						ImGui::EndPopup();
					}

					ImGui::PopID();
				}
			}

			// TODO: This behavior quickly got a little bigger than anticipated.
			// Should move this system into its own class or interface for table view

			const bool is_popup_open = ImGui::IsPopupOpen("ShowItemContextMenu", ImGuiPopupFlags_AnyPopup);

			if (!is_popup_open) {
				const ImVec2 mousePos = ImGui::GetMousePos();

				// We shouldn't have a horizontal scroll bar, so we do not need to account for it.
				// Doing so results in the last item of the table not being selectable.
				const float outer_width = table->OuterRect.Max.x - ImGui::GetStyle().ScrollbarSize;
				const float outer_height = table->OuterRect.Max.y;

				auto IsMouseInBounds = [&]() -> bool {
					return (mousePos.x < outer_width && mousePos.x > table->OuterRect.Min.x) &&
					       (mousePos.y < outer_height && mousePos.y > table->OuterRect.Min.y);
				};

				if (IsMouseInBounds()) {
					if (ImGui::IsMouseClicked(0)) {
						InputManager::SetMouseSelectionStart(ImGui::GetMousePos());
						wasMouseInBounds = true;

						if (ImGui::IsMouseDoubleClicked(0)) {
							if (itemPreview != nullptr) {
								if (b_AddToInventory) {
									Console::AddItem(itemPreview->GetFormID().c_str(), clickToAddCount);
								} else if (b_PlaceOnGround) {
									Console::PlaceAtMe(itemPreview->GetFormID().c_str(), clickToAddCount);
								}

								Console::StartProcessThread();
							}
						}
					}

					if (wasMouseInBounds && InputManager::GetMouseState() == InputManager::MOUSE_STATE::kIdle) {
						if (ImGui::IsMouseDragging(0, 15.0f)) {
							InputManager::SetMouseState(InputManager::MOUSE_STATE::kBoxSelection);
							itemSelectionList.clear();
						}
					}
				}

				if (ImGui::IsMouseReleased(0)) {
					if (wasMouseInBounds) {
						switch (InputManager::GetMouseState()) {
						case InputManager::MOUSE_STATE::kIdle:
							if (!itemSelectionList.contains(itemPreview)) {
								itemSelectionList.clear();
								itemSelectionList.insert(itemPreview);
							} else {
								if (itemSelectionList.size() > 1) {
									itemSelectionList.clear();
									itemSelectionList.insert(itemPreview);
								}
							}
							break;
						case InputManager::MOUSE_STATE::kBoxSelection:
							break;
						}
					} else {
						itemSelectionList.clear();
					}

					wasMouseInBounds = false;
					InputManager::ClearMouseState();
				}

				// Draw Selection Rectangle
				if (InputManager::GetMouseState() == InputManager::MOUSE_STATE::kBoxSelection) {
					const auto mouseEnd = ImVec2(
						std::clamp(
							ImGui::GetMousePos().x,
							table->OuterRect.Min.x,
							table->OuterRect.Max.x - ImGui::GetStyle().ScrollbarSize),
						std::clamp(
							ImGui::GetMousePos().y,
							table->OuterRect.Min.y,
							table->OuterRect.Max.y - 1.0f));

					// We bound selection box to the table. In the future,
					// This will solve some issues with handling mouse events.
					InputManager::SetMouseSelectionEnd(mouseEnd);

					const auto mouseDragStart = InputManager::GetMouseSelectionStart();
					const auto mouseDragEnd = InputManager::GetMouseSelectionEnd();

					ImGui::GetWindowDrawList()->AddRect(mouseDragStart, mouseDragEnd, IM_COL32(255, 255, 255, 200));
					ImGui::GetWindowDrawList()->AddRectFilled(mouseDragStart, mouseDragEnd, IM_COL32(255, 255, 255, 10));
				}
			}

			ImGui::EndTable();
		}
	}
}