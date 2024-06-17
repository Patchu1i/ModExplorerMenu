#include "Window.h"

#include "Console.h"
#include "Utils/Util.h"
#include "Window.h"

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
	bool NPCWindow::SortColumns(const NPC* v1, const NPC* v2)
	{
		const ImGuiTableSortSpecs* sort_specs = s_current_sort_specs;
		const ImGuiID ID = sort_specs->Specs->ColumnUserID;

		int delta = 0;

		switch (ID) {
		// Delta must be +1 or -1 to indicate move. Otherwise comparitor is invalid.
		case Favorite:  // bool
			delta = (v1->favorite < v2->favorite) ? -1 : (v1->favorite > v2->favorite) ? 1 :
			                                                                             0;
			break;
		case FormID:  // std::string
			delta = v1->GetFormID().compare(v2->GetFormID());
			break;
		case Name:  // const char *
			delta = v1->GetName().compare(v2->GetName());
			break;
		case EditorID:  // std::string
			delta = v1->GetEditorID().compare(v2->GetEditorID());
			break;
		case Health:  // float
			delta = (v1->GetHealth() < v2->GetHealth()) ? -1 : (v1->GetHealth() > v2->GetHealth()) ? 1 :
			                                                                                         0;
			break;
		case Magicka:  // float
			delta = (v1->GetMagicka() < v2->GetMagicka()) ? -1 : (v1->GetMagicka() > v2->GetMagicka()) ? 1 :
			                                                                                             0;
			break;
		case Stamina:  // float
			delta = (v1->GetStamina() < v2->GetStamina()) ? -1 : (v1->GetStamina() > v2->GetStamina()) ? 1 :
			                                                                                             0;
			break;
		case CarryWeight:  // float
			delta = (v1->GetCarryWeight() < v2->GetCarryWeight()) ? -1 : (v1->GetCarryWeight() > v2->GetCarryWeight()) ? 1 :
			                                                                                                             0;
			break;
		case Plugin:  // std::string
			delta = v1->GetPluginName().compare(v2->GetPluginName());
			break;
		default:
			break;
		}

		if (delta > 0) {
			return (sort_specs->Specs->SortDirection == ImGuiSortDirection_Ascending) ? true : false;
		}
		if (delta < 0) {
			return (sort_specs->Specs->SortDirection == ImGuiSortDirection_Ascending) ? false : true;
		}

		return false;  // prevent assertion (?)
	}

	// Sorts the columns of referenced list by sort_specs
	void NPCWindow::SortColumnsWithSpecs(ImGuiTableSortSpecs* sort_specs)
	{
		s_current_sort_specs = sort_specs;
		if (npcList.size() > 1)
			std::sort(npcList.begin(), npcList.end(), SortColumns);

		s_current_sort_specs = NULL;
	}

	// Draw the table of items
	void NPCWindow::ShowFormTable(Settings::Style& a_style, Settings::Config& a_config)
	{
		auto results = std::string("Results (") + std::to_string(npcList.size()) + std::string(")");
		ImGui::SeparatorText(results.c_str());

		bool noColumns = !a_config.npcShowAttackDamageColumn && !a_config.npcShowCarryWeightColumn && !a_config.npcShowPluginColumn &&
		                 !a_config.npcShowEditorIDColumn && !a_config.npcShowFormIDColumn && !a_config.npcShowHealthColumn &&
		                 !a_config.npcShowMagickaColumn && !a_config.npcShowNameColumn && !a_config.npcShowStaminaColumn &&
		                 !a_config.npcShowFavoriteColumn;

		if (noColumns) {
			ImGui::Text("No columns are enabled. Please enable at least one column.");
			return;
		}

		auto rowBG = a_style.showTableRowBG ? ImGuiTableFlags_RowBg : 0;

		// TODO: innerWidth of table determines ScrollX buffer. Need to figure out how I can incrementally
		// increase the width of innerWidth based on enabled columns to create a more dynamically sized window.

		ImGuiContext& g = *ImGui::GetCurrentContext();
		ImVec2 table_size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
		if (ImGui::BeginTable("##NPCWindow::Table", column_count, NPCTableFlags | rowBG, table_size, table_size.x + 100.0f)) {
			ImGui::TableSetupScrollFreeze(1, 1);
			ImGui::TableSetupColumn("Favorite", ImGuiTableColumnFlags_WidthFixed, 18.0f, Favorite);
			ImGui::TableSetupColumn("FormID", ImGuiTableColumnFlags_None, 35.0f, FormID);
			ImGui::TableSetupColumn("Plugin", ImGuiTableColumnFlags_None, 0.0f, Plugin);
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None, 0.0f, Name);
			ImGui::TableSetupColumn("EditorID", ImGuiTableColumnFlags_None, 0.0f, EditorID);
			ImGui::TableSetupColumn("Health", ImGuiTableColumnFlags_None, 25.0f, Health);
			ImGui::TableSetupColumn("Magicka", ImGuiTableColumnFlags_None, 25.0f, Magicka);
			ImGui::TableSetupColumn("Stamina", ImGuiTableColumnFlags_None, 25.0f, Stamina);
			ImGui::TableSetupColumn("Carry Weight", ImGuiTableColumnFlags_None, 25.0f, CarryWeight);

			ImGui::PushFont(a_style.font.medium);
			ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
			for (int column = 0; column < column_count; column++) {
				ImGui::TableSetColumnIndex(column);
				const char* column_name = ImGui::TableGetColumnName(column);  // Retrieve name passed to TableSetupColumn()
				ImGui::PushID(column);
				ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

				// if (column != ColumnID_FormID && column != ColumnID_EditorID)
				// 	ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(column_name));

				ImGui::TableHeader(column_name);

				ImGui::PopID();
			}
			ImGui::PopFont();

			ImGui::TableSetColumnEnabled(Favorite, a_config.npcShowFavoriteColumn);
			ImGui::TableSetColumnEnabled(FormID, a_config.npcShowFormIDColumn);
			ImGui::TableSetColumnEnabled(Plugin, a_config.npcShowPluginColumn);
			ImGui::TableSetColumnEnabled(Name, a_config.npcShowNameColumn);
			ImGui::TableSetColumnEnabled(EditorID, a_config.npcShowEditorIDColumn);
			ImGui::TableSetColumnEnabled(Health, a_config.npcShowHealthColumn);
			ImGui::TableSetColumnEnabled(Magicka, a_config.npcShowMagickaColumn);
			ImGui::TableSetColumnEnabled(Stamina, a_config.npcShowStaminaColumn);
			ImGui::TableSetColumnEnabled(CarryWeight, a_config.npcShowCarryWeightColumn);

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
						// if (ImGui::ImageButton("##NPCWindow::FavoriteButton", favorite_state, imageSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(col, col, col, col))) {
						// 	npc->favorite = !npc->favorite;
						// }
						if (ImGui::DisabledImageButton("##NPCWindow::FavoriteButton", b_clickToPlace, favorite_state, imageSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(col, col, col, col))) {
							if (!b_clickToPlace) {
								npc->favorite = !npc->favorite;
							}
						}
					} else {
						ImGui::DisabledCheckbox("##NPCWindow::FavoriteCheckbox", b_clickToPlace, npc->favorite);
					}

					ImGui::PopStyleColor(3);
					ImGui::PopStyleVar(2);

					bool _itemSelected = false;

					// Form ID
					ImGui::TableNextColumn();
					ImGui::Text(npc->GetFormID().c_str());

					//	Plugin
					ImGui::TableNextColumn();
					ImGui::Text(npc->GetPluginName().data());

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
						ImGui::PushFont(a_style.font.tiny);
						ShowItemCard(npc);
						ImGui::PopFont();

						if (ImGui::IsMouseClicked(0)) {
							_itemSelected = true;
							selectedNPC = npc;

							if (b_clickToPlace) {
								Console::PlaceAtMeFormID(npc->GetBaseForm());
								Console::PridLast();
								if (b_placeFrozen) {
									Console::Freeze();
								}
								if (b_placeNaked) {
									Console::UnEquip();
								}
								Console::StartProcessThread(false);

								_itemSelected = false;
								selectedNPC = nullptr;
							}
						}

						// if (ImGui::IsMouseClicked(1, true)) {
						// 	ImGui::OpenPopup("TestItemPopupMenu");
						// }
					}

					// if (ImGui::BeginPopup("TestItemPopupMenu")) {
					// 	ShowItemListContextMenu(*item);
					// 	ImGui::EndPopup();
					// }

					// Shortcut Handlers
					// if (b_ClickToAdd && _itemSelected) {
					// 	ConsoleCommand::AddItem(item->formid.c_str(), clickToAddCount);
					// } else if (b_ClickToPlace && _itemSelected) {
					// 	ConsoleCommand::PlaceAtMe(item->formid.c_str(), clickToPlaceCount);
					// } else if (b_ClickToFavorite && _itemSelected) {
					// 	item->favorite = !item->favorite;
					// } else if (!b_ClickToAdd && _itemSelected) {
					// 	item->selected = true;
					// }

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