#include "Object.h"

#include "Console.h"
#include "Utils/Util.h"
#include "Windows/Persistent.h"
// #include "Windows/ItemCards.h"

namespace ModExplorerMenu
{
	// Draws a Copy to Clipboard button on Context popup.
	void ObjectWindow::ShowObjectListContextMenu(StaticObject& a_object)
	{
		constexpr auto flags = ImGuiSelectableFlags_DontClosePopups;
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));

		if (ImGui::Selectable(_T("GENERAL_COPY_FORM_ID"), false, flags)) {
			ImGui::LogToClipboard();
			ImGui::LogText(a_object.GetFormID().c_str());
			ImGui::LogFinish();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Selectable(_T("GENERAL_COPY_EDITOR_ID"), false, flags)) {
			ImGui::LogToClipboard();
			ImGui::LogText(a_object.editorid.c_str());
			ImGui::LogFinish();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		if (ImGui::Selectable(_T("GENERAL_COPY_MODEL_PATH"), false, flags)) {
			RE::TESModel* model = a_object.TESForm->As<RE::TESModel>();

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
	void ObjectWindow::ShowFormTable(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_config;

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
					SortColumnsWithSpecs<std::vector<StaticObject*>, StaticObject>(objectList, sort_specs);
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

					ImGui::DisabledCheckbox("##ObjectWindow::FavoriteCheckbox", b_ClickToFavorite, obj->favorite);

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

					// Input Handlers
					// Input Handlers
					auto curRow = ImGui::TableGetHoveredRow();
					if (curRow == ImGui::TableGetRowIndex()) {
						hoveredObject = obj;

						if (ImGui::IsMouseClicked(0)) {
							if (b_ClickToSelect) {
								selectedObject = obj;
							} else if (b_ClickToFavorite) {
								obj->favorite = !obj->favorite;
								PersistentData::GetSingleton()->UpdatePersistentData<StaticObject*>(obj);
							}
						}

						if (ImGui::IsMouseClicked(1, true)) {
							ImGui::OpenPopup("ShowObjectContextMenu");
						}
					}

					if (ImGui::BeginPopup("ShowObjectContextMenu")) {
						ShowObjectListContextMenu(*obj);
						ImGui::EndPopup();
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

					if (bHover || selectedObject == obj) {
						table->RowBgColor[1] = ImGui::GetColorU32(ImGuiCol_Border);
					}

					ImGui::PopID();
				}
			}

			ImGui::EndTable();
		}
	}
}