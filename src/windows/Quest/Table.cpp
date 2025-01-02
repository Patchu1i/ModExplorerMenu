#include "Quest.h"
#include "Utils/Util.h"
#include "Windows/Persistent.h"

namespace Modex
{
	void QuestWindow::ShowQuestListContextMenu(Quest& a_quest)
	{
		(void)a_quest;
		// todo
	}

	void QuestWindow::ShowFormTable(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_config;

		auto results = std::string(_T("Results")) + std::string(" (") + std::to_string(questList.size()) + std::string(")");
		ImGui::SeparatorText(results.c_str());

		auto rowBG = a_style.showTableRowBG ? ImGuiTableFlags_RowBg : 0;

		ImVec2 tableSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
		if (ImGui::BeginTable("##QuestWindow::Table", columnList.GetTotalColumns(), Frame::TABLE_FLAGS | rowBG, tableSize, tableSize.x + 100.0f)) {
			ImGui::TableSetupScrollFreeze(1, 1);

			for (auto& column : columnList.columns) {
				ImGui::TableSetupColumn(column.name.c_str(), column.flags, column.width);
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

			if (ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs()) {
				if (sort_specs->SpecsDirty) {
					SortColumnsWithSpecs<std::vector<Quest*>, Quest>(questList, sort_specs);
					sort_specs->SpecsDirty = false;
					dirty = false;
				}
			}

			ImGuiListClipper clipper;
			ImGuiContext& g = *ImGui::GetCurrentContext();
			ImGuiTable* table = g.CurrentTable;

			int numOfRow = 0;
			clipper.Begin(static_cast<int>(questList.size()));
			while (clipper.Step()) {
				for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
					auto& quest = questList[row];

					numOfRow++;
					auto tableID = std::string("##QuestWindow::TableIndex-") + std::to_string(numOfRow);
					ImGui::PushID(tableID.c_str());

					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					// Overwrite color to hide ugly imgui backdrop on image
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

					if (ImGui::DisabledCheckbox("##NPCWindow::FavoriteCheckbox", b_ClickToFavorite, quest->favorite)) {
						PersistentData::GetSingleton()->UpdatePersistentData<Quest*>(quest);
					}

					ImGui::PopStyleColor(3);
					ImGui::PopStyleVar(2);

					// Plugin
					ImGui::TableNextColumn();
					ImGui::Text(quest->filename.c_str());

					// Quest Name
					ImGui::TableNextColumn();
					ImGui::Text(quest->name.c_str());

					// FormID
					// ImGui::TableNextColumn();
					// ImGui::Text(quest->GetFormID().c_str());

					// Editor ID
					ImGui::TableNextColumn();
					ImGui::Text(quest->editorid.c_str());

					// Quest Type
					// ImGui::TableNextColumn();
					// ImGui::Text(quest->GetTypeName().c_str());

					// Quest State
					// ImGui::TableNextColumn();
					// ImGui::Text(quest->GetQuestState().c_str());

					// Input Handlers
					auto curRow = ImGui::TableGetHoveredRow();
					if (curRow == ImGui::TableGetRowIndex()) {
						hoveredQuest = quest;

						if (ImGui::IsMouseClicked(0)) {
							if (b_ClickToSelect) {
								selectedQuest = quest;
							} else if (b_ClickToFavorite) {
								quest->favorite = !quest->favorite;
								PersistentData::GetSingleton()->UpdatePersistentData<Quest*>(quest);
							}
						}

						if (ImGui::IsMouseClicked(1, true)) {
							ImGui::OpenPopup("ShowQuestContextMenu");
						}
					}

					if (ImGui::BeginPopup("ShowQuestContextMenu")) {
						ShowQuestListContextMenu(*quest);
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

					if (bHover || selectedQuest == quest) {
						table->RowBgColor[1] = ImGui::GetColorU32(ImGuiCol_Border);
					}

					ImGui::PopID();
				}
			}

			ImGui::EndTable();
		}
	}
}