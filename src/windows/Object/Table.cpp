#include "include/C/Console.h"
#include "include/O/Object.h"
#include "include/P/Persistent.h"

namespace Modex
{
	// Draws a Copy to Clipboard button on Context popup.
	void ObjectWindow::ShowObjectListContextMenu(ObjectData& a_object)
	{
		constexpr auto flags = ImGuiSelectableFlags_DontClosePopups;
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));

		if (ImGui::Selectable(_T("OBJECT_PLACE_SELECTED"), false, flags)) {
			if (!itemSelectionList.empty()) {
				for (auto& object : itemSelectionList) {
					Console::PlaceAtMe(object->GetFormID().c_str(), clickToPlaceCount);
				}

				Console::StartProcessThread();
			} else {
				Console::PlaceAtMe(a_object.GetFormID().c_str(), clickToPlaceCount);
				Console::StartProcessThread();
			}

			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Selectable(_T("GENERAL_ADD_FAVORITE"), false, flags)) {
			if (!itemSelectionList.empty()) {
				for (auto& object : itemSelectionList) {
					object->favorite = !object->favorite;
					PersistentData::GetSingleton()->UpdatePersistentData(object);
				}
			} else {
				a_object.favorite = !a_object.favorite;
				PersistentData::GetSingleton()->UpdatePersistentData(a_object);
			}

			ImGui::CloseCurrentPopup();
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		if (ImGui::Selectable(_T("GENERAL_COPY_FORM_ID"), false, flags)) {
			ImGui::LogToClipboard();
			ImGui::LogText(a_object.GetFormID().c_str());
			ImGui::LogFinish();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Selectable(_T("GENERAL_COPY_EDITOR_ID"), false, flags)) {
			ImGui::LogToClipboard();
			ImGui::LogText(a_object.GetEditorID().c_str());
			ImGui::LogFinish();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		if (ImGui::Selectable(_T("GENERAL_COPY_MODEL_PATH"), false, flags)) {
			RE::TESModel* model = a_object.GetForm()->As<RE::TESModel>();

			if (model != nullptr) {
				std::string modelFullPath = model->GetModel();

				ImGui::LogToClipboard();
				ImGui::LogText(modelFullPath.c_str());
				ImGui::LogFinish();
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::PopStyleVar(1);
	}

	// Draw the table of items
	void ObjectWindow::ShowFormTable()
	{
		auto a_style = Settings::GetSingleton()->GetStyle();

		auto results = std::string("Results (") + std::to_string(objectList.size()) + std::string(")");
		ImGui::SeparatorText(results.c_str());

		auto rowBG = a_style.showTableRowBG ? ImGuiTableFlags_RowBg : 0;

		ImVec2 tableSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
		if (ImGui::BeginTable("##ObjectWindow::Table", columnList.GetTotalColumns(), Frame::TABLE_FLAGS | rowBG, tableSize)) {
			ImGui::TableSetupScrollFreeze(1, 1);

			for (auto& column : columnList.columns) {
				ImGui::TableSetupColumn(column.name.c_str(), column.flags, column.width, column.key);
			}

			ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

			int numOfColumn = 0;
			for (auto& column : columnList.columns) {
				ImGui::TableSetColumnIndex(numOfColumn);
				ImGui::PushID(column.key + 10);
				ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
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
					SortColumnsWithSpecs<std::vector<ObjectData*>, ObjectData>(objectList, sort_specs);
					sort_specs->SpecsDirty = false;
					dirty = false;
				}
			}

			ImGuiListClipper clipper;
			ImGuiContext& g = *ImGui::GetCurrentContext();
			ImGuiTable* table = g.CurrentTable;

			int numOfRow = 0;
			clipper.Begin(static_cast<int>(objectList.size()));
			while (clipper.Step()) {
				for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
					auto& obj = objectList[row];

					numOfRow++;
					auto table_id = std::string("##ObjectWindow::TableIndex-") + std::to_string(numOfRow);
					ImGui::PushID(table_id.c_str());

					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					// Overwrite color to hide ugly imgui backdrop on image.
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

					if (ImGui::DisabledCheckbox("##ObjectWindow::FavoriteCheckbox", b_ClickToFavorite, obj->favorite)) {
						PersistentData::GetSingleton()->UpdatePersistentData<ObjectData*>(obj);
					}

					ImGui::PopStyleColor(3);
					ImGui::PopStyleVar(2);

					//	Plugin
					ImGui::TableNextColumn();
					ImGui::Text(obj->GetPluginName().data());

					// Type
					ImGui::TableNextColumn();
					ImGui::Text(obj->GetTypeName().data());

					// FormID
					ImGui::TableNextColumn();
					ImGui::Text(obj->GetFormID().data());

					// EditorID
					ImGui::TableNextColumn();
					ImGui::Text(obj->GetEditorID().data());

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

					// Get the rows which the selection box is hovering over.
					// Two conditions based on inverted selection box.
					if (isMouseSelecting) {
						if (mouseDragEnd.y < mouseDragStart.y) {
							if (row_rect.Overlaps(ImRect(mouseDragEnd, mouseDragStart))) {
								if (ImGui::IsMouseReleased(0)) {
									itemSelectionList.insert(obj);
								}
							}
						} else if (row_rect.Overlaps(ImRect(mouseDragStart, mouseDragEnd))) {
							if (ImGui::IsMouseReleased(0)) {
								itemSelectionList.insert(obj);
							}
						}
					}

					if (itemSelectionList.contains(obj)) {
						table->RowBgColor[1] = ImGui::GetColorU32(ImGuiCol_Border, 0.65f);
					}

					if (bHover || selectedObject == obj) {
						table->RowBgColor[1] = ImGui::GetColorU32(ImGuiCol_Border);
						hoveredObject = obj;

						if (ImGui::IsMouseClicked(1, true)) {
							ImGui::OpenPopup("ShowObjectContextMenu");
						}
					}

					if (ImGui::BeginPopup("ShowObjectContextMenu")) {
						ShowObjectListContextMenu(*obj);
						ImGui::EndPopup();
					}

					ImGui::PopID();
				}
			}

			// TODO: This behavior quickly got a little bigger than anticipated.
			// Should move this system into its own class or interface for table view

			const bool is_popup_open = ImGui::IsPopupOpen("ShowObjectContextMenu", ImGuiPopupFlags_AnyPopup);

			if (!is_popup_open) {
				const ImVec2 mousePos = ImGui::GetMousePos();
				const float outer_width = table->OuterRect.Max.x - ImGui::GetStyle().ScrollbarSize;
				const float outer_height = table->OuterRect.Max.y - ImGui::GetStyle().ScrollbarSize;

				if (ImGui::IsMouseDragging(0, 5.0f) && isMouseSelecting == MOUSE_STATE::DRAG_NONE) {
					if ((mousePos.x > outer_width || mousePos.x < table->OuterRect.Min.x) ||
						(mousePos.y > outer_height || mousePos.y < table->OuterRect.Min.y)) {
						isMouseSelecting = MOUSE_STATE::DRAG_IGNORE;
					} else {
						isMouseSelecting = MOUSE_STATE::DRAG_SELECT;
						itemSelectionList.clear();
						mouseDragStart = ImGui::GetMousePos();
					}
				} else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
					if ((mousePos.x < outer_width && mousePos.x > table->OuterRect.Min.x) &&
						(mousePos.y < outer_height && mousePos.y > table->OuterRect.Min.y)) {
						if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
							if (itemSelectionList.contains(hoveredObject)) {
								itemSelectionList.erase(hoveredObject);
							} else {
								itemSelectionList.insert(hoveredObject);
							}
						} else {
							itemSelectionList.clear();
							itemSelectionList.insert(hoveredObject);
						}

						if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
							if (b_ClickToPlace) {
								Console::PlaceAtMe(hoveredObject->GetFormID().c_str(), clickToPlaceCount);
								Console::StartProcessThread();
							} else if (b_ClickToFavorite) {
								hoveredObject->favorite = !hoveredObject->favorite;
								PersistentData::GetSingleton()->UpdatePersistentData<ObjectData*>(hoveredObject);
							}
						}
					}
				}

				if (isMouseSelecting == MOUSE_STATE::DRAG_SELECT) {
					if (ImGui::IsMouseDragging(0, 5.0f)) {
						mouseDragEnd = ImGui::GetMousePos();
						ImGui::GetWindowDrawList()->AddRect(mouseDragStart, mouseDragEnd, IM_COL32(255, 255, 255, 200));
						ImGui::GetWindowDrawList()->AddRectFilled(mouseDragStart, mouseDragEnd, IM_COL32(255, 255, 255, 10));
					} else {
						isMouseSelecting = MOUSE_STATE::DRAG_NONE;
						mouseDragStart = ImVec2(0.0f, 0.0f);
						mouseDragEnd = ImVec2(0.0f, 0.0f);
					}
				}

				if (ImGui::IsMouseReleased(0)) {
					isMouseSelecting = MOUSE_STATE::DRAG_NONE;
					mouseDragStart = ImVec2(0.0f, 0.0f);
					mouseDragEnd = ImVec2(0.0f, 0.0f);
				}
			}

			ImGui::EndTable();
		}
	}
}