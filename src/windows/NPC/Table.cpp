#include "include/C/Console.h"
#include "include/N/NPC.h"
#include "include/P/Persistent.h"

namespace Modex
{
	// Draws a Copy to Clipboard button on Context popup.
	void NPCWindow::ShowNPCListContextMenu(NPC& a_npc)
	{
		constexpr auto flags = ImGuiSelectableFlags_DontClosePopups;
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));

		if (ImGui::Selectable(_T("GENERAL_COPY_FORM_ID"), false, flags)) {
			ImGui::LogToClipboard();
			ImGui::LogText(std::format("{:08x}", a_npc.FormID).c_str());
			ImGui::LogFinish();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Selectable(_T("GENERAL_COPY_NAME"), false, flags)) {
			ImGui::LogToClipboard();
			ImGui::LogText(a_npc.GetName().data());
			ImGui::LogFinish();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Selectable(_T("GENERAL_COPY_EDITOR_ID"), false, flags)) {
			ImGui::LogToClipboard();
			ImGui::LogText(a_npc.editorid.c_str());
			ImGui::LogFinish();
			ImGui::CloseCurrentPopup();
		}

		if (a_npc.refID != 0) {
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			if (ImGui::Selectable(_T("GENERAL_COPY_REFERENCE_ID"), false, flags)) {
				ImGui::LogToClipboard();
				ImGui::LogText(std::format("{:08x}", a_npc.refID).c_str());
				ImGui::LogFinish();
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::PopStyleVar(1);
	}

	// Draw the table of items
	void NPCWindow::ShowFormTable(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_config;

		auto results = std::string(_T("Results")) + std::string(" (") + std::to_string(npcList.size()) + std::string(")");
		ImGui::SeparatorText(results.c_str());

		auto rowBG = a_style.showTableRowBG ? ImGuiTableFlags_RowBg : 0;

		ImVec2 tableSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
		if (ImGui::BeginTable("##NPCWindow::Table", columnList.GetTotalColumns(), Frame::TABLE_FLAGS | rowBG, tableSize, tableSize.x + 100.0f)) {
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
					SortColumnsWithSpecs<std::vector<NPC*>, NPC>(npcList, sort_specs);
					sort_specs->SpecsDirty = false;
					dirty = false;
				}
			}

			ImGuiListClipper clipper;
			ImGuiContext& g = *ImGui::GetCurrentContext();
			ImGuiTable* table = g.CurrentTable;

			int numOfRow = 0;
			clipper.Begin(static_cast<int>(npcList.size()));
			while (clipper.Step()) {
				for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
					auto& npc = npcList[row];

					numOfRow++;
					auto tableID = std::string("##NPCWindow::TableIndex-") + std::to_string(numOfRow);
					ImGui::PushID(tableID.c_str());

					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					// Overwrite color to hide ugly imgui backdrop on image.
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

					if (ImGui::DisabledCheckbox("##NPCWindow::FavoriteCheckbox", b_ClickToFavorite, npc->favorite)) {
						PersistentData::GetSingleton()->UpdatePersistentData<NPC*>(npc);
					}

					ImGui::PopStyleColor(3);
					ImGui::PopStyleVar(2);

					//	Plugin
					ImGui::TableNextColumn();
					ImGui::Text(npc->GetPluginName().data());

					// Form ID
					ImGui::TableNextColumn();
					ImGui::Text(npc->GetFormID().c_str());

					// Reference ID
					ImGui::TableNextColumn();
					if (npc->refID == 0) {
						ImGui::Text(_T("Unknown"));
					} else {
						ImGui::Text(std::format("{:08x}", npc->refID).c_str());
					}

					// Item Name
					ImGui::TableNextColumn();
					ImGui::Text(npc->GetName().data());

					// Editor ID
					ImGui::TableNextColumn();
					ImGui::Text(npc->GetEditorID().data());

					// Race
					ImGui::TableNextColumn();
					ImGui::Text(npc->GetRace().data());

					// Gender
					ImGui::TableNextColumn();
					ImGui::Text(_T(npc->GetGender().data()));

					// Class
					ImGui::TableNextColumn();
					ImGui::Text(npc->GetClass().data());

					// Level
					ImGui::TableNextColumn();
					ImGui::Text(std::to_string(npc->GetLevel()).c_str());

					// Health
					ImGui::TableNextColumn();
					const auto health = std::format("{:.0f}", npc->GetHealth());
					ImGui::Text(health.c_str());

					// Magicka
					ImGui::TableNextColumn();
					const auto magicka = std::format("{:.0f}", npc->GetMagicka());
					ImGui::Text(magicka.c_str());

					// Stamina
					ImGui::TableNextColumn();
					const auto stamina = std::format("{:.0f}", npc->GetStamina());
					ImGui::Text(stamina.c_str());

					// Carry Weight
					ImGui::TableNextColumn();
					const auto carry = std::format("{:.0f}", npc->GetCarryWeight());
					ImGui::Text(carry.c_str());

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

					if (bHover || selectedNPC == npc) {
						table->RowBgColor[1] = ImGui::GetColorU32(ImGuiCol_Border);
						hoveredNPC = npc;

						if (ImGui::IsMouseClicked(0)) {
							if (b_ClickToSelect) {
								selectedNPC = npc;
							} else if (b_ClickToPlace) {
								Console::PlaceAtMe(npc->GetFormID().c_str(), clickToPlaceCount);
								Console::StartProcessThread();
							} else if (b_ClickToFavorite) {
								npc->favorite = !npc->favorite;
								PersistentData::GetSingleton()->UpdatePersistentData<NPC*>(npc);
							}
						}

						if (ImGui::IsMouseClicked(1, true)) {
							ImGui::OpenPopup("ShowNPCContextMenu");
						}
					}

					if (ImGui::BeginPopup("ShowNPCContextMenu")) {
						ShowNPCListContextMenu(*npc);
						ImGui::EndPopup();
					}

					ImGui::PopID();
				}
			}

			ImGui::EndTable();
		}
	}
}