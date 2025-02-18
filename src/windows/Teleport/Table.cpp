// #include "include/C/Console.h"
// #include "include/M/Menu.h"
// #include "include/P/Persistent.h"
// #include "include/T/Teleport.h"

// namespace Modex
// {
// 	// Draws a Copy to Clipboard button on Context popup.
// 	void TeleportWindow::ShowTeleportContextMenu(CellData& a_cell)
// 	{
// 		constexpr auto flags = ImGuiSelectableFlags_DontClosePopups;
// 		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));

// 		// if (ImGui::Selectable(_T("GENERAL_ADD_FAVORITE"), false, flags)) {
// 		// 	if (!itemSelectionList.empty()) {
// 		// 		for (auto& cell : itemSelectionList) {
// 		// 			cell->favorite = !cell->favorite;
// 		// 			// PersistentData::GetSingleton()->UpdatePersistentData(*cell);
// 		// 		}
// 		// 	} else {
// 		// 		a_cell.favorite = !a_cell.favorite;
// 		// 		// PersistentData::GetSingleton()->UpdatePersistentData(a_cell);
// 		// 	}

// 		// 	ImGui::CloseCurrentPopup();
// 		// }

// 		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

// 		if (ImGui::Selectable(_T("GENERAL_COPY_WORLDSPACE_NAME"), false, flags)) {
// 			ImGui::LogToClipboard();
// 			ImGui::LogText(a_cell.space.c_str());
// 			ImGui::LogFinish();
// 			ImGui::CloseCurrentPopup();
// 		}

// 		if (ImGui::Selectable(_T("GENERAL_COPY_ZONE_NAME"), false, flags)) {
// 			ImGui::LogToClipboard();
// 			ImGui::LogText(a_cell.zone.c_str());
// 			ImGui::LogFinish();
// 			ImGui::CloseCurrentPopup();
// 		}

// 		if (ImGui::Selectable(_T("GENERAL_COPY_CELL_NAME"), false, flags)) {
// 			ImGui::LogToClipboard();
// 			ImGui::LogText(a_cell.cellName.c_str());
// 			ImGui::LogFinish();
// 			ImGui::CloseCurrentPopup();
// 		}

// 		if (ImGui::Selectable(_T("GENERAL_COPY_EDITOR_ID"), false, flags)) {
// 			ImGui::LogToClipboard();
// 			ImGui::LogText(a_cell.editorid.c_str());
// 			ImGui::LogFinish();
// 			ImGui::CloseCurrentPopup();
// 		}

// 		ImGui::PopStyleVar(1);
// 	}

// 	// Draw the table of items
// 	void TeleportWindow::ShowFormTable()
// 	{
// 		auto a_style = Settings::GetSingleton()->GetStyle();

// 		auto results = std::string(_T("Results")) + std::string(" (") + std::to_string(cellList.size()) + std::string(")");
// 		ImGui::SeparatorText(results.c_str());

// 		auto rowBG = a_style.showTableRowBG ? ImGuiTableFlags_RowBg : 0;

// 		ImVec2 table_size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
// 		if (ImGui::BeginTable("##TeleportWindow::Table", columnList.GetTotalColumns(), Frame::TELEPORT_FLAGS | rowBG, table_size)) {
// 			ImGui::TableSetupScrollFreeze(1, 1);
// 			for (auto& column : columnList.columns) {
// 				ImGui::TableSetupColumn(column.name.c_str(), column.flags, column.width, column.key);
// 			}

// 			ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
// 			int column_n = 0;
// 			for (auto& column : columnList.columns) {
// 				ImGui::TableSetColumnIndex(column_n);
// 				ImGui::PushID(column.key + 10);
// 				ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
// 				ImGui::TableHeader(column.name.c_str());
// 				ImGui::PopID();

// 				column_n++;
// 			}

// 			if (dirty) {
// 				ImGui::TableGetSortSpecs()->SpecsDirty = true;
// 			}

// 			// Sort our data if sort specs have been changed!
// 			if (ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs()) {
// 				if (sort_specs->SpecsDirty) {
// 					SortColumnsWithSpecs<std::vector<CellData*>, CellData>(cellList, sort_specs);
// 					sort_specs->SpecsDirty = false;
// 					dirty = false;
// 				}
// 			}

// 			ImGuiListClipper clipper;
// 			ImGuiContext& g = *ImGui::GetCurrentContext();
// 			ImGuiTable* table = g.CurrentTable;

// 			int count = 0;
// 			clipper.Begin(static_cast<int>(cellList.size()));
// 			while (clipper.Step()) {
// 				for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
// 					auto& cell = cellList[row];

// 					count++;
// 					auto table_id = std::string("##TeleportWindow::TableIndex-") + std::to_string(count);
// 					ImGui::PushID(table_id.c_str());

// 					ImGui::TableNextRow();
// 					ImGui::TableNextColumn();

// 					// Overwrite color to hide ugly imgui backdrop on image.
// 					// ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
// 					// ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
// 					// ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
// 					// ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
// 					// ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

// 					// if (ImGui::DisabledCheckbox("##TeleportWindow::FavoriteCheckbox", b_ClickToFavorite, cell->favorite)) {
// 					// 	PersistentData::GetSingleton()->UpdatePersistentData<CellData*>(cell);
// 					// }

// 					// ImGui::PopStyleColor(3);
// 					// ImGui::PopStyleVar(2);

// 					//	Plugin
// 					// ImGui::TableNextColumn();
// 					ImGui::Text(cell->GetPluginName().data());

// 					// Space
// 					ImGui::TableNextColumn();
// 					ImGui::Text(cell->GetSpace().data());

// 					// Zone
// 					ImGui::TableNextColumn();
// 					ImGui::Text(cell->GetZone().data());

// 					// CellName
// 					ImGui::TableNextColumn();
// 					ImGui::Text(cell->GetCellName().data());

// 					// EditorID
// 					ImGui::TableNextColumn();
// 					ImGui::Text(cell->GetEditorID().data());

// 					// https://github.com/ocornut/imgui/issues/6588#issuecomment-1634424774
// 					// Sloppy way to handle row highlighting since ImGui natively doesn't support it.
// 					ImRect row_rect(
// 						table->WorkRect.Min.x,
// 						table->RowPosY1,
// 						table->WorkRect.Max.x,
// 						table->RowPosY2);
// 					row_rect.ClipWith(table->BgClipRect);

// 					bool bHover =
// 						ImGui::IsMouseHoveringRect(row_rect.Min, row_rect.Max, false) &&
// 						ImGui::IsWindowHovered(ImGuiHoveredFlags_None) &&
// 						!ImGui::IsAnyItemHovered();  // optional

// 					// Get the rows which the selection box is hovering over.
// 					// Two conditions based on inverted selection box.
// 					if (isMouseSelecting) {
// 						if (mouseDragEnd.y < mouseDragStart.y) {
// 							if (row_rect.Overlaps(ImRect(mouseDragEnd, mouseDragStart))) {
// 								if (ImGui::IsMouseReleased(0)) {
// 									itemSelectionList.insert(cell);
// 								}
// 							}
// 						} else if (row_rect.Overlaps(ImRect(mouseDragStart, mouseDragEnd))) {
// 							if (ImGui::IsMouseReleased(0)) {
// 								itemSelectionList.insert(cell);
// 							}
// 						}
// 					}

// 					if (itemSelectionList.contains(cell)) {
// 						table->RowBgColor[1] = ImGui::GetColorU32(ImGuiCol_Border, 0.65f);
// 					}

// 					if (bHover) {
// 						table->RowBgColor[1] = ImGui::GetColorU32(ImGuiCol_Border);
// 						selectedCell = cell;

// 						if (ImGui::IsMouseClicked(1, true)) {
// 							ImGui::OpenPopup("ShowTeleportContextMenu");
// 						}
// 					}

// 					if (ImGui::BeginPopup("ShowTeleportContextMenu")) {
// 						ShowTeleportContextMenu(*cell);
// 						ImGui::EndPopup();
// 					}

// 					ImGui::PopID();
// 				}
// 			}

// 			// TODO: This behavior quickly got a little bigger than anticipated.
// 			// Should move this system into its own class or interface for table view

// 			const bool is_popup_open = ImGui::IsPopupOpen("ShowTeleportContextMenu", ImGuiPopupFlags_AnyPopup);

// 			if (!is_popup_open) {
// 				const ImVec2 mousePos = ImGui::GetMousePos();
// 				const float outer_width = table->OuterRect.Max.x - ImGui::GetStyle().ScrollbarSize;
// 				const float outer_height = table->OuterRect.Max.y - ImGui::GetStyle().ScrollbarSize;

// 				if (ImGui::IsMouseDragging(0, 5.0f) && isMouseSelecting == MOUSE_STATE::DRAG_NONE) {
// 					if ((mousePos.x > outer_width || mousePos.x < table->OuterRect.Min.x) ||
// 						(mousePos.y > outer_height || mousePos.y < table->OuterRect.Min.y)) {
// 						isMouseSelecting = MOUSE_STATE::DRAG_IGNORE;
// 					} else {
// 						isMouseSelecting = MOUSE_STATE::DRAG_SELECT;
// 						itemSelectionList.clear();
// 						mouseDragStart = ImGui::GetMousePos();
// 					}
// 				} else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
// 					if ((mousePos.x < outer_width && mousePos.x > table->OuterRect.Min.x) &&
// 						(mousePos.y < outer_height && mousePos.y > table->OuterRect.Min.y)) {
// 						if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
// 							if (itemSelectionList.contains(selectedCell)) {
// 								itemSelectionList.erase(selectedCell);
// 							} else {
// 								itemSelectionList.insert(selectedCell);
// 							}
// 						} else {
// 							itemSelectionList.clear();
// 							itemSelectionList.insert(selectedCell);
// 						}

// 						if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
// 							if (b_ClickToTeleport) {
// 								Console::Teleport(selectedCell->editorid);
// 								Console::StartProcessThread();
// 								Menu::GetSingleton()->Close();

// 								itemSelectionList.clear();
// 								selectedCell = nullptr;
// 							}
// 							// else if (b_ClickToFavorite) {
// 							// 	selectedCell->favorite = !selectedCell->favorite;
// 							// 	// PersistentData::GetSingleton()->UpdatePersistentData<CellData*>(selectedCell);
// 							// }
// 						}
// 					}
// 				}

// 				if (isMouseSelecting == MOUSE_STATE::DRAG_SELECT) {
// 					if (ImGui::IsMouseDragging(0, 5.0f)) {
// 						mouseDragEnd = ImGui::GetMousePos();
// 						ImGui::GetWindowDrawList()->AddRect(mouseDragStart, mouseDragEnd, IM_COL32(255, 255, 255, 200));
// 						ImGui::GetWindowDrawList()->AddRectFilled(mouseDragStart, mouseDragEnd, IM_COL32(255, 255, 255, 10));
// 					} else {
// 						isMouseSelecting = MOUSE_STATE::DRAG_NONE;
// 						mouseDragStart = ImVec2(0.0f, 0.0f);
// 						mouseDragEnd = ImVec2(0.0f, 0.0f);
// 					}
// 				}

// 				if (ImGui::IsMouseReleased(0)) {
// 					isMouseSelecting = MOUSE_STATE::DRAG_NONE;
// 					mouseDragStart = ImVec2(0.0f, 0.0f);
// 					mouseDragEnd = ImVec2(0.0f, 0.0f);
// 				}
// 			}

// 			ImGui::EndTable();
// 		}
// 	}
// }