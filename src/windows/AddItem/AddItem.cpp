#include "include/A/AddItem.h"
#include "include/B/Blacklist.h"

namespace Modex
{
	// https://github.com/ocornut/imgui/issues/319
	void AddItemWindow::Draw(float a_offset)
	{
		float MIN_SEARCH_HEIGHT = 175.0f;
		const float MIN_SEARCH_WIDTH = 200.0f;
		const float MIN_KITBAR_HEIGHT = MIN_SEARCH_HEIGHT;
		const float MAX_SEARCH_HEIGHT = ImGui::GetContentRegionAvail().y * 0.75f;
		const float MAX_SEARCH_WIDTH = ImGui::GetContentRegionAvail().x * 0.85f;
		const float MAX_KITBAR_HEIGHT = 300.0f;

		// I probably should handle this better, but It's kind of nice
		// that the search data is in this class, so I can reference it for behavior like this.
		if (primaryFilter == RE::FormType::Armor || primaryFilter == RE::FormType::Weapon) {
			MIN_SEARCH_HEIGHT = 200.0f;

			// Ensure minimum height after resizing.
			if (ImGui::GetStateStorage()->GetFloat(ImGui::GetID("AddItem::SearchHeight"), MIN_SEARCH_HEIGHT) < MIN_SEARCH_HEIGHT) {
				ImGui::GetStateStorage()->SetFloat(ImGui::GetID("AddItem::SearchHeight"), MIN_SEARCH_HEIGHT);
			}
		}

		const ImGuiChildFlags flags = ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding;
		float search_height = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("AddItem::SearchHeight"), MIN_SEARCH_HEIGHT);
		float search_width = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("AddItem::SearchWidth"), MAX_SEARCH_WIDTH);
		float kitview_height = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("AddItem::KitViewHeight"), MIN_SEARCH_HEIGHT / 2);
		float kitbar_height = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("AddItem::KitBarHeight"), MAX_KITBAR_HEIGHT);
		float window_padding = ImGui::GetStyle().WindowPadding.y;
		const float button_width = ImGui::GetContentRegionAvail().x / static_cast<int>(Viewport::Count);
		const float button_height = ImGui::GetFontSize() * 1.5f;
		const float tab_bar_height = button_height + (window_padding * 2);

		if (kitsFound.size() > 0) {
			// If we have kits, we adjust the size of the view to fit the kits to a maximum of MAX_KITBAR_HEIGHT / 2.
			const float size = std::max(kitview_height, std::min(kitsFound.size() * ImGui::GetFrameHeightWithSpacing(), MAX_KITBAR_HEIGHT / 2));

			// Only Increase, do not decrease.
			if (kitview_height < size && kitview_height == ImGui::GetFrameHeightWithSpacing() * 1.75f) {
				kitview_height = size;
			}
		} else {
			kitview_height = ImGui::GetFrameHeightWithSpacing() * 1.75f;  // SubCategoryHeader Height
		}

		// Tab Button Area
		ImGui::SameLine();
		ImGui::SetCursorPosY(window_padding);

		ImVec2 backup_pos = ImGui::GetCursorPos();
		if (ImGui::BeginChild("##AddItem::TabBar", ImVec2(0.0f, button_height), 0, ImGuiWindowFlags_NoFocusOnAppearing)) {
			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
			if (ImGui::Selectable("Table View", activeViewport == Viewport::TableView, 0, ImVec2(button_width, 0.0f))) {
				activeViewport = Viewport::TableView;
				Refresh();
			}

			ImGui::SameLine();

			if (ImGui::Selectable("Blacklist", activeViewport == Viewport::BlacklistView, 0, ImVec2(button_width, 0.0f))) {
				activeViewport = Viewport::BlacklistView;
			}

			ImGui::SameLine();

			if (ImGui::Selectable("Kit Editor", activeViewport == Viewport::KitView, 0, ImVec2(button_width, 0.0f))) {
				activeViewport = Viewport::KitView;
			}

			ImGui::PopStyleVar();
		}
		ImGui::EndChild();

		if (activeViewport == Viewport::KitView) {
			search_width = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("AddItem::SearchKitWidth"), ImGui::GetContentRegionAvail().x / 2);

			ImVec2 temp_cursor = backup_pos;
			// Search Input Area
			ImGui::SameLine();
			ImGui::SetCursorPos(temp_cursor);
			ImGui::SetCursorPosY(tab_bar_height - window_padding);
			temp_cursor = ImGui::GetCursorPos();
			if (ImGui::BeginChild("##AddItem::SearchArea", ImVec2(search_width + 1.0f, search_height), flags, ImGuiWindowFlags_NoFocusOnAppearing)) {
				ShowSearch();
			}
			ImGui::EndChild();

			// Horizontal Search / Table Splitter
			float full_width = search_width;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + a_offset);
			ImGui::SetCursorPosY(temp_cursor.y + search_height);
			ImGui::DrawSplitter("##AddItem::HorizontalSplitter", true, &search_height, &full_width, MIN_SEARCH_HEIGHT, MAX_SEARCH_HEIGHT);

			// Table Area
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + a_offset);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (window_padding / 2));
			if (ImGui::BeginChild("##AddItem::TableArea", ImVec2(search_width, 0), flags, ImGuiWindowFlags_NoFocusOnAppearing)) {
				ShowFormTable();
			}
			ImGui::EndChild();

			// Vertical Search Table / Action Splitter
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - window_padding);
			ImGui::SetCursorPosY(tab_bar_height - window_padding);
			float full_height = ImGui::GetContentRegionAvail().y;
			ImGui::DrawSplitter("##AddItem::VerticalSplitter2", false, &search_width, &full_height, MIN_SEARCH_WIDTH, MAX_SEARCH_WIDTH);

			// Action Area
			ImGui::SameLine();
			temp_cursor = ImGui::GetCursorPos();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - window_padding);
			ImGui::SetCursorPosY(tab_bar_height - window_padding);
			float action_width = ImGui::GetContentRegionAvail().x;
			if (ImGui::BeginChild("##AddItem::KitbarArea",
					ImVec2(ImGui::GetContentRegionAvail().x, kitbar_height), flags, ImGuiWindowFlags_NoFocusOnAppearing)) {
				ShowKitBar();
			}
			ImGui::EndChild();

			// Horizontal KitBar / KitTable splitter
			ImGui::SetCursorPos(temp_cursor);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - window_padding);
			ImGui::SetCursorPosY(tab_bar_height + kitbar_height - window_padding);
			ImGui::DrawSplitter("##AddItem::KitActionTableSplitter", true, &kitbar_height, &action_width, MIN_KITBAR_HEIGHT, MAX_KITBAR_HEIGHT);

			ImGui::SameLine();
			ImGui::SetCursorPosX(temp_cursor.x - window_padding);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + window_padding);
			if (ImGui::BeginChild("##AddItem::KitBarTableViewThing", ImVec2(0, 0), flags, ImGuiWindowFlags_NoFocusOnAppearing)) {
				ShowKitTable();
			}
			ImGui::EndChild();

			// Persist Search Area Width/Height
			ImGui::GetStateStorage()->SetFloat(ImGui::GetID("AddItem::KitBarHeight"), kitbar_height);
			ImGui::GetStateStorage()->SetFloat(ImGui::GetID("AddItem::SearchKitWidth"), search_width);
			ImGui::GetStateStorage()->SetFloat(ImGui::GetID("AddItem::SearchHeight"), search_height);
		}

		if (activeViewport == Viewport::BlacklistView) {
			ImGui::SetCursorPos(backup_pos);
			ImGui::SetCursorPosY(tab_bar_height - window_padding);
			Blacklist::GetSingleton()->Draw(0.0f);
		}

		if (activeViewport == Viewport::TableView) {
			// Search Input Area
			ImGui::SameLine();
			ImGui::SetCursorPos(backup_pos);
			ImGui::SetCursorPosY(tab_bar_height - window_padding);
			backup_pos = ImGui::GetCursorPos();
			if (ImGui::BeginChild("##AddItem::SearchArea", ImVec2(search_width + 1.0f, search_height), flags, ImGuiWindowFlags_NoFocusOnAppearing)) {
				ShowSearch();
			}
			ImGui::EndChild();

			// Horizontal Search / Table Splitter
			float full_width = search_width;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + a_offset);
			ImGui::SetCursorPosY(backup_pos.y + search_height);
			ImGui::DrawSplitter("##AddItem::HorizontalSplitter", true, &search_height, &full_width, MIN_SEARCH_HEIGHT, MAX_SEARCH_HEIGHT);

			if (showKitsForPlugin && selectedMod != _T("All Mods")) {
				backup_pos = ImGui::GetCursorPos();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + a_offset);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (window_padding / 2));
				if (ImGui::BeginChild("##AddItem::KitViewTableThing", ImVec2(search_width, kitview_height), flags, ImGuiWindowFlags_NoFocusOnAppearing)) {
					ShowKitPluginView();
				}
				ImGui::EndChild();

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + a_offset);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (window_padding / 2));
				ImGui::DrawSplitter("##AddItem::KitViewSplitter", true, &kitview_height, &search_width, MIN_SEARCH_HEIGHT / 2, MAX_KITBAR_HEIGHT);

				ImGui::SetCursorPos(backup_pos);
				ImGui::SetCursorPosY(backup_pos.y + kitview_height + window_padding);
			}

			// Table Area
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + a_offset);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (window_padding / 2));
			if (ImGui::BeginChild("##AddItem::TableArea", ImVec2(search_width, 0), flags, ImGuiWindowFlags_NoFocusOnAppearing)) {
				ShowFormTable();
			}
			ImGui::EndChild();

			// Vertical Search Table / Action Splitter
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - window_padding);
			ImGui::SetCursorPosY(tab_bar_height - window_padding);
			float full_height = ImGui::GetContentRegionAvail().y;
			ImGui::DrawSplitter("##AddItem::VerticalSplitter2", false, &search_width, &full_height, MIN_SEARCH_WIDTH, MAX_SEARCH_WIDTH);

			// Action Area
			ImGui::SameLine();
			ImGui::SetCursorPosY(tab_bar_height - window_padding);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - window_padding);
			if (ImGui::BeginChild("##AddItem::ActionArea",
					ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), flags, ImGuiWindowFlags_NoFocusOnAppearing)) {
				ShowActions();
			}
			ImGui::EndChild();

			// Persist Search Area Width/Height
			ImGui::GetStateStorage()->SetFloat(ImGui::GetID("AddItem::KitViewHeight"), kitview_height);
			ImGui::GetStateStorage()->SetFloat(ImGui::GetID("AddItem::SearchWidth"), search_width);
			ImGui::GetStateStorage()->SetFloat(ImGui::GetID("AddItem::SearchHeight"), search_height);
		}

		// Book Prompt
		if (openBook != nullptr) {
			ShowBookPreview();
		}
	}

	void AddItemWindow::Init()
	{
		g_DescriptionFrameworkInterface = DescriptionFrameworkAPI::GetDescriptionFrameworkInterface001();

		primaryFilter = RE::FormType::None;
		secondaryFilter = "All";
		openBook = nullptr;
		itemPreview = nullptr;

		b_AddToInventory = true;
		b_PlaceOnGround = false;
		clickToAddCount = 1;

		activeViewport = Viewport::TableView;
		wasMouseInBounds = false;
		wasMouseInBoundsKit = false;
		showKitsForPlugin = true;

		searchKey = BaseColumn::ID::Name;
		dirty = true;

		selectedMod = "All Mods";
		selectedKit = "None";

		columnList = AddItemColumns();
		kitColumnList = KitColumns();
		ApplyFilters();
	}
}  // namespace Modex

// Saving this code snippet for if/when I return to implementing drag 'n drop functionality.
// This was used to draw an alternative Drag Target with a gradient texture and color.

// void AddItemWindow::ShowDragTarget(DRAG_TARGET a_target)
// {
// 	const auto& drawList = ImGui::GetWindowDrawList();
// 	const ImVec2 start = ImGui::GetCursorScreenPos();
// 	const ImVec2 end = ImGui::GetContentRegionAvail();
// 	const ImVec2 totalSize = ImVec2(start.x + end.x, start.y + end.y);

// 	std::string a_label;
// 	ImU32 color_a;
// 	ImU32 color_b;

// 	switch (a_target) {
// 	case DRAG_TARGET::RemoveFromKit:
// 		a_label = "Remove From Kit";
// 		color_a = IM_COL32(255, 0, 0, 55);
// 		color_b = IM_COL32(255, 55, 5, 55);
// 		break;
// 	case DRAG_TARGET::ReplaceSearch:
// 		a_label = "Replace Search";
// 		color_a = IM_COL32(25, 155, 255, 55);
// 		color_b = IM_COL32(5, 55, 255, 55);
// 		break;
// 	case DRAG_TARGET::AddToKit:
// 		a_label = "Add To Kit";
// 		color_a = IM_COL32(0, 255, 0, 55);
// 		color_b = IM_COL32(55, 255, 5, 55);
// 		break;
// 	default:
// 		color_a = IM_COL32(255, 0, 0, 255);
// 		color_b = IM_COL32(0, 0, 0, 255);
// 		a_label = "MISSING DRAG_TARGET";
// 		break;
// 	}

// 	bool hovered = ImGui::IsMouseHoveringRect(start, totalSize);
// 	if (hovered) {
// 		// Modify colors (ultimately this can be prebaked in the style)
// 		float h_increase = hovered ? 0.02f : 0.02f;
// 		float v_increase = hovered ? 10.0f : 0.07f;

// 		ImVec4 bg1f = ImGui::ColorConvertU32ToFloat4(color_a);
// 		ImGui::ColorConvertRGBtoHSV(bg1f.x, bg1f.y, bg1f.z, bg1f.x, bg1f.y, bg1f.z);
// 		bg1f.x = ImMin(bg1f.x + h_increase, 1.0f);
// 		bg1f.z = ImMin(bg1f.z + v_increase, 1.0f);
// 		ImGui::ColorConvertHSVtoRGB(bg1f.x, bg1f.y, bg1f.z, bg1f.x, bg1f.y, bg1f.z);
// 		color_a = ImGui::GetColorU32(bg1f);

// 		ImVec4 bg2f = ImGui::ColorConvertU32ToFloat4(color_b);
// 		ImGui::ColorConvertRGBtoHSV(bg2f.x, bg2f.y, bg2f.z, bg2f.x, bg2f.y, bg2f.z);
// 		bg2f.z = ImMin(bg2f.z + h_increase, 1.0f);
// 		bg2f.z = ImMin(bg2f.z + v_increase, 1.0f);
// 		ImGui::ColorConvertHSVtoRGB(bg2f.x, bg2f.y, bg2f.z, bg2f.x, bg2f.y, bg2f.z);
// 		color_b = ImGui::GetColorU32(bg2f);
// 	}

// 	const ImVec2 textPos = ImVec2(start.x + ((end.x / 2) - (ImGui::CalcTextSize(a_label.c_str()).x / 2)), start.y + (end.y / 2));

// 	int vert_start_idx = drawList->VtxBuffer.Size;
// 	drawList->AddRectFilled(start, totalSize, IM_COL32(0, 0, 0, 50));
// 	int vert_end_idx = drawList->VtxBuffer.Size;

// 	ImGui::ShadeVertsLinearColorGradientKeepAlpha(
// 		drawList,
// 		vert_start_idx,
// 		vert_end_idx,
// 		start,
// 		ImVec2(start.x, totalSize.y),
// 		color_a,
// 		color_b);

// 	drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), a_label.c_str());

// 	if (ImGui::IsMouseReleased(0)) {
// 		if (ImGui::IsMouseHoveringRect(start, totalSize)) {
// 			switch (a_target) {
// 			case DRAG_TARGET::AddToKit:
// 				AddItemToKit(kitDragObject);
// 				InputManager::ClearMouseState();
// 				break;
// 			case DRAG_TARGET::RemoveFromKit:
// 				RemoveItemFromKit(kitDragObject);
// 				InputManager::ClearMouseState();
// 				break;
// 			case DRAG_TARGET::ReplaceSearch:
// 				searchKey = BaseColumn::ID::Name;
// 				ImFormatString(inputBuffer, IM_ARRAYSIZE(inputBuffer), kitDragObject->name.c_str());
// 				Refresh();
// 				InputManager::ClearMouseState();
// 				break;
// 			default:
// 				InputManager::ClearMouseState();
// 				break;
// 			}
// 		}
// 	}
//}