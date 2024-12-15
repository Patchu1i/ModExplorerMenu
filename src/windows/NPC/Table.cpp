#include "NPC.h"

#include "Console.h"
#include "Utils/Util.h"
#include "Windows/Persistent.h"

// Draws a Copy to Clipboard button on Context popup.
// void NPCWindow::ShowItemListContextMenu(Data::CachedItem& a_item)
// {
// 	constexpr auto flags = ImGuiSelectableFlags_DontClosePopups;
// 	ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));

// 	if (ImGui::Selectable("Copy Form ID", false, flags)) {
// 		ImGui::LogToClipboard();
// 		ImGui::LogText(a_item.formid.c_str());
// 		ImGui::LogFinish();
// 		ImGui::CloseCurrentPopup();
// 	}

// 	if (ImGui::Selectable("Copy Name", false, flags)) {
// 		ImGui::LogToClipboard();
// 		ImGui::LogText(a_item.name);
// 		ImGui::LogFinish();
// 		ImGui::CloseCurrentPopup();
// 	}

// 	if (ImGui::Selectable("Copy Editor ID", false, flags)) {
// 		ImGui::LogToClipboard();
// 		ImGui::LogText(a_item.editorid.c_str());
// 		ImGui::LogFinish();
// 		ImGui::CloseCurrentPopup();
// 	}

// 	if (a_item.formType == RE::FormType::Book) {
// 		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
// 		if (ImGui::Selectable("Read Me!")) {
// 			openBook = &a_item;
// 		}
// 	}

// 	ImGui::PopStyleVar(1);
// }

namespace ModExplorerMenu
{
	// Draw the table of items
	void NPCWindow::ShowFormTable(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_config;

		auto results = std::string("Results (") + std::to_string(npcList.size()) + std::string(")");
		ImGui::SeparatorText(results.c_str());

		auto rowBG = a_style.showTableRowBG ? ImGuiTableFlags_RowBg : 0;

		ImVec2 table_size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
		if (ImGui::BeginTable("##NPCWindow::Table", columnList.GetTotalColumns(), Frame::TABLE_FLAGS | rowBG, table_size, table_size.x + 100.0f)) {
			ImGui::TableSetupScrollFreeze(1, 1);
			for (auto& column : columnList.columns) {
				ImGui::TableSetupColumn(column.name.c_str(), column.flags, column.width, column.key);
			}

			ImGui::PushFont(a_style.font.medium);
			ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
			int column_n = 0;
			for (auto& column : columnList.columns) {
				ImGui::TableSetColumnIndex(column_n);
				ImGui::PushID(column.key + 10);
				ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
				ImGui::TableHeader(column.name.c_str());
				ImGui::PopID();

				// ImGui::TableSetColumnEnabled(column_n, *column.enabled);
				column_n++;
			}
			ImGui::PopFont();

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

			hoveredNPC = nullptr;

			int count = 0;
			clipper.Begin(static_cast<int>(npcList.size()), ImGui::GetTextLineHeightWithSpacing());
			while (clipper.Step()) {
				for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
					auto& npc = npcList[row];

					count++;
					auto table_id = std::string("##NPCWindow::TableIndex-") + std::to_string(count);
					ImGui::PushID(table_id.c_str());

					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					// Overwrite color to hide ugly imgui backdrop on image.
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

					ImTextureID favorite_state = npc->favorite ? a_style.favoriteIconEnabled.texture : a_style.favoriteIconDisabled.texture;
					float col = npc->favorite ? 1.0f : 0.5f;

					if (favorite_state != nullptr) {
						const auto imageSize = ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize());
						if (ImGui::DisabledImageButton("##NPCWindow::FavoriteButton", b_ClickToFavorite, favorite_state, imageSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(col, col, col, col))) {
							if (!b_ClickToFavorite) {
								npc->favorite = !npc->favorite;
								PersistentData::GetSingleton()->UpdatePersistentData<NPC*>(npc);
							}
						}
					} else {
						ImGui::DisabledCheckbox("##NPCWindow::FavoriteCheckbox", b_ClickToFavorite, npc->favorite);
					}

					ImGui::PopStyleColor(3);
					ImGui::PopStyleVar(2);

					//	Plugin
					ImGui::TableNextColumn();
					ImGui::Text(npc->GetPluginName().data());

					// Form ID
					ImGui::TableNextColumn();
					ImGui::Text(npc->GetFormID().c_str());

					// Item Name
					ImGui::TableNextColumn();
					ImGui::Text(npc->GetName().data());

					// Editor ID
					ImGui::TableNextColumn();
					ImGui::Text(npc->GetEditorID().data());

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

					// Input Handlers
					auto curRow = ImGui::TableGetHoveredRow();
					if (curRow == ImGui::TableGetRowIndex()) {
						hoveredNPC = npc;

						if (ImGui::IsMouseClicked(0)) {
							if (b_ClickToSelect) {
								selectedNPC = npc;
							} else if (b_ClickToFavorite) {
								npc->favorite = !npc->favorite;
								PersistentData::GetSingleton()->UpdatePersistentData<NPC*>(npc);
							}
						}
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

					if (bHover || selectedNPC == npc) {
						table->RowBgColor[1] = ImGui::GetColorU32(ImGuiCol_Border);
					}

					ImGui::PopID();
				}
			}

			ImGui::EndTable();
		}
	}
}