// #include "include/C/Console.h"
// #include "include/O/Object.h"
// #include "include/P/Persistent.h"

// namespace Modex
// {
// 	// Draws a Copy to Clipboard button on Context popup.
// 	void ObjectWindow::ShowObjectListContextMenu(ObjectData& a_object)
// 	{
// 		constexpr auto flags = ImGuiSelectableFlags_DontClosePopups;
// 		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));

// 		if (ImGui::Selectable(_T("OBJECT_PLACE_SELECTED"), false, flags)) {
// 			if (!itemSelectionList.empty()) {
// 				for (auto& object : itemSelectionList) {
// 					Console::PlaceAtMe(object->GetFormID().c_str(), clickToPlaceCount);
// 				}

// 				Console::StartProcessThread();
// 			} else {
// 				Console::PlaceAtMe(a_object.GetFormID().c_str(), clickToPlaceCount);
// 				Console::StartProcessThread();
// 			}

// 			ImGui::CloseCurrentPopup();
// 		}

// 		// if (ImGui::Selectable(_T("GENERAL_ADD_FAVORITE"), false, flags)) {
// 		// 	if (!itemSelectionList.empty()) {
// 		// 		for (auto& object : itemSelectionList) {
// 		// 			object->favorite = !object->favorite;
// 		// 			// PersistentData::GetSingleton()->UpdatePersistentData(*object);
// 		// 		}
// 		// 	} else {
// 		// 		a_object.favorite = !a_object.favorite;
// 		// 		// PersistentData::GetSingleton()->UpdatePersistentData(a_object);
// 		// 	}

// 		// 	ImGui::CloseCurrentPopup();
// 		// }

// 		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

// 		if (ImGui::Selectable(_T("GENERAL_COPY_FORM_ID"), false, flags)) {
// 			ImGui::LogToClipboard();
// 			ImGui::LogText(a_object.GetFormID().c_str());
// 			ImGui::LogFinish();
// 			ImGui::CloseCurrentPopup();
// 		}

// 		if (ImGui::Selectable(_T("GENERAL_COPY_EDITOR_ID"), false, flags)) {
// 			ImGui::LogToClipboard();
// 			ImGui::LogText(a_object.GetEditorID().c_str());
// 			ImGui::LogFinish();
// 			ImGui::CloseCurrentPopup();
// 		}

// 		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

// 		if (ImGui::Selectable(_T("GENERAL_COPY_MODEL_PATH"), false, flags)) {
// 			RE::TESModel* model = a_object.GetForm()->As<RE::TESModel>();

// 			if (model != nullptr) {
// 				std::string modelFullPath = model->GetModel();

// 				ImGui::LogToClipboard();
// 				ImGui::LogText(modelFullPath.c_str());
// 				ImGui::LogFinish();
// 				ImGui::CloseCurrentPopup();
// 			}
// 		}

// 		ImGui::PopStyleVar(1);
// 	}

// 	void ObjectWindow::UpdateLayoutSizes(float avail_width)
// 	{
// 		ItemSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 2.0f);
// 		// Layout: when not stretching: allow extending into right-most spacing.
// 		LayoutItemSpacing = ImGui::GetStyle().ItemSpacing.y;
// 		LayoutHitSpacing = 0.0f;

// 		avail_width += floorf(LayoutItemSpacing * 0.5f);

// 		// Layout: calculate number of icon per line and number of lines
// 		LayoutItemSize = ImVec2(floorf(ItemSize.x), floorf(ItemSize.y));
// 		LayoutColumnCount = 1;
// 		LayoutLineCount = ((int)objectList.size() + LayoutColumnCount - 1) / LayoutColumnCount;

// 		LayoutItemStep = ImVec2(LayoutItemSize.x + LayoutItemSpacing, LayoutItemSize.y + LayoutItemSpacing);
// 		LayoutSelectableSpacing = std::max(floorf(LayoutItemSpacing) - LayoutHitSpacing, 0.0f);
// 		LayoutOuterPadding = floorf(LayoutItemSpacing * 0.5f);
// 	}

// 	// std::string GetIconType(RE::FormType a_type)
// 	// {
// 	// 	return ICON_LC_ALARM_CLOCK;
// 	// }

// 	void ObjectWindow::DrawObject(const ObjectData& a_object, const ImVec2& pos)
// 	{
// 		(void)a_object;
// 		(void)pos;
// 		// const auto& style = Settings::GetSingleton()->GetStyle();
// 		// ImDrawList* draw_list = ImGui::GetWindowDrawList();

// 		// ImVec2 box_min(pos.x - 1, pos.y - 1);
// 		// ImVec2 box_max(box_min.x + LayoutItemSize.x + 2, box_min.y + LayoutItemSize.y + 2);  // Dubious

// 		// bool is_item_select = selectionStorage.Contains(a_object.TableID);

// 		// const ImU32 bg_color = ImGui::ColorConvertFloat4ToU32(style.tableRowBg);
// 		// const ImU32 text_color = ImGui::GetColorU32(ImGuiCol_Text);
// 		// draw_list->AddRectFilled(box_min, box_max, bg_color);  // Background color

// 		// // Draw icon
// 		// const std::string icon_type = GetIconType(a_object.GetFormType());

// 		// ImVec2 icon_pos = ImVec2(box_min.x + LayoutOuterPadding, box_min.y + LayoutOuterPadding);
// 		// ImVec2 icon_size = ImVec2(LayoutItemSize.y - LayoutOuterPadding * 2, LayoutItemSize.y - LayoutOuterPadding * 2);
// 		// draw_list->AddText(icon_pos, text_color, icon_type.c_str());
// 	}

// 	// Draw the table of items
// 	void ObjectWindow::ShowFormTable()
// 	{
// 		// auto a_style = Settings::GetSingleton()->GetStyle();

// 		// // auto results = std::string("Results (") + std::to_string(objectList.size()) + std::string(")");
// 		// // ImGui::SeparatorText(results.c_str());

// 		// // auto rowBG = a_style.showTableRowBG ? ImGuiTableFlags_RowBg : 0;

// 		// // ImVec2 tableSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
// 		// // if (ImGui::BeginTable("##ObjectWindow::Table", columnList.GetTotalColumns(), Frame::TABLE_FLAGS | rowBG, tableSize)) {
// 		// if (ImGui::BeginChild("Assets", ImVec2(0.0f, -ImGui::GetTextLineHeightWithSpacing()), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoMove)) {
// 		// 	ImDrawList* draw_list = ImGui::GetWindowDrawList();

// 		// 	const float avail_width = ImGui::GetContentRegionAvail().x;
// 		// 	UpdateLayoutSizes(avail_width);

// 		// 	// Rendering parameters
// 		// 	const ImU32 icon_type_overlay_colors[3] = { 0, IM_COL32(200, 70, 70, 255), IM_COL32(70, 170, 70, 255) };
// 		// 	const ImU32 icon_bg_color = ImGui::GetColorU32(IM_COL32(35, 35, 35, 220));
// 		// 	const ImVec2 icon_type_overlay_size = ImVec2(4.0f, 4.0f);
// 		// 	const bool display_label = (LayoutItemSize.x >= ImGui::CalcTextSize("999").x);

// 		// 	// Calculate and store start position.
// 		// 	ImVec2 start_pos = ImGui::GetCursorScreenPos();
// 		// 	start_pos = ImVec2(start_pos.x, start_pos.y);
// 		// 	ImGui::SetCursorScreenPos(start_pos);

// 		// 	const int ITEMS_COUNT = static_cast<int>(objectList.size());
// 		// 	ImGuiMultiSelectFlags flags = ImGuiMultiSelectFlags_ClearOnEscape | ImGuiMultiSelectFlags_BoxSelect1d | ImGuiMultiSelectFlags_ScopeWindow;
// 		// 	ImGuiMultiSelectIO* ms_io = ImGui::BeginMultiSelect(flags, selectionStorage.Size, ITEMS_COUNT);
// 		// 	selectionStorage.UserData = (void*)&objectList;
// 		// 	selectionStorage.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage* self, int idx) {
// 		// 		std::vector<ObjectData*>* p_items = (std::vector<ObjectData*>*)self->UserData;
// 		// 		return (*p_items)[idx]->TableID;
// 		// 	};  // Index -> ID

// 		// 	selectionStorage.ApplyRequests(ms_io);

// 		// 	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(LayoutSelectableSpacing, LayoutSelectableSpacing));

// 		// 	const int column_count = LayoutColumnCount;
// 		// 	ImGuiListClipper clipper;

// 		// 	clipper.Begin(ITEMS_COUNT);
// 		// 	if (ms_io->RangeSrcItem != -1)
// 		// 		clipper.IncludeItemByIndex((int)ms_io->RangeSrcItem);  // Ensure RangeSrc item is not clipped.
// 		// 	while (clipper.Step()) {
// 		// 		const int item_start = clipper.DisplayStart;
// 		// 		const int item_end = clipper.DisplayEnd;

// 		// 		for (int line_idx = item_start; line_idx < item_end; line_idx++) {
// 		// 			const int item_min_idx_for_current_line = line_idx * column_count;
// 		// 			const int item_max_idx_for_current_line = std::min((line_idx + 1) * column_count, static_cast<int>(objectList.size()));

// 		// 			for (int item_idx = item_min_idx_for_current_line; item_idx < item_max_idx_for_current_line; ++item_idx) {
// 		// 				auto& obj = objectList[item_idx];
// 		// 				ImGui::PushID((int)obj->TableID);

// 		// 				// Position item
// 		// 				ImVec2 pos = ImVec2(start_pos.x, start_pos.y + line_idx * LayoutItemStep.y);
// 		// 				ImGui::SetCursorScreenPos(pos);

// 		// 				ImGui::SetNextItemSelectionUserData(item_idx);
// 		// 				bool is_item_selected = selectionStorage.Contains(obj->TableID);
// 		// 				bool is_item_visible = ImGui::IsRectVisible(LayoutItemSize);
// 		// 				ImGui::Selectable("", is_item_selected, ImGuiSelectableFlags_None, LayoutItemSize);

// 		// 				if (ImGui::IsItemToggledSelection())
// 		// 					is_item_selected = !is_item_selected;

// 		// 				if (is_item_visible) {
// 		// 					DrawObject(*obj, pos);
// 		// 				}

// 		// 				ImGui::PopID();
// 		// 			}
// 		// 		}
// 		// 	}
// 		// 	clipper.End();

// 		// 	ImGui::PopStyleVar();

// 		// 	if (ImGui::BeginPopupContextWindow()) {
// 		// 		ShowObjectListContextMenu(*objectList[ms_io->RangeSrcItem]);
// 		// 	}

// 		// 	ms_io = ImGui::EndMultiSelect();
// 		// 	selectionStorage.ApplyRequests(ms_io);

// 		// 	// ImGui::EndTable();
// 		// 	ImGui::EndChild();
// 		// }
// 	}
// }