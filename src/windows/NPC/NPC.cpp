#include "include/N/NPC.h"

namespace Modex
{
	void NPCWindow::Draw(float a_offset)
	{
		const float MIN_SEARCH_HEIGHT = 175.0f;
		const float MIN_SEARCH_WIDTH = 200.0f;
		const float MAX_SEARCH_HEIGHT = ImGui::GetContentRegionAvail().y * 0.75f;
		const float MAX_SEARCH_WIDTH = ImGui::GetContentRegionAvail().x * 0.85f;

		const ImGuiChildFlags flags = ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding;
		float search_height = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("NPC::SearchHeight"), MIN_SEARCH_HEIGHT);
		float search_width = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("NPC::SearchWidth"), MAX_SEARCH_WIDTH);
		float window_padding = ImGui::GetStyle().WindowPadding.y;

		// Search Input Area
		ImGui::SameLine();
		ImGui::SetCursorPosY(window_padding);
		const ImVec2 backup_pos = ImGui::GetCursorPos();
		if (ImGui::BeginChild("##Object::SearchArea", ImVec2(search_width + 1.0f, search_height), flags, ImGuiWindowFlags_NoFocusOnAppearing)) {
			ShowSearch();
		}
		ImGui::EndChild();

		// Horizontal Search / Table Splitter
		float full_width = search_width;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + a_offset);
		ImGui::SetCursorPosY(backup_pos.y + search_height);
		ImGui::DrawSplitter("##NPC::HorizontalSplitter", true, &search_height, &full_width, MIN_SEARCH_HEIGHT, MAX_SEARCH_HEIGHT);

		// Table Area
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + a_offset);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (window_padding / 2));
		if (ImGui::BeginChild("##NPC::TableArea", ImVec2(search_width, 0), flags, ImGuiWindowFlags_NoFocusOnAppearing)) {
			ShowFormTable();
		}
		ImGui::EndChild();

		// Vertical Search Table / Action Splitter
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - window_padding);
		ImGui::SetCursorPosY(window_padding);
		float full_height = ImGui::GetContentRegionAvail().y;
		ImGui::DrawSplitter("##NPC::VerticalSplitter2", false, &search_width, &full_height, MIN_SEARCH_WIDTH, MAX_SEARCH_WIDTH);

		// Action Area
		ImGui::SameLine();
		ImGui::SetCursorPosY(window_padding);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - window_padding);
		if (ImGui::BeginChild("##NPC::ActionArea",
				ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), flags, ImGuiWindowFlags_NoFocusOnAppearing)) {
			ShowActions();
		}
		ImGui::EndChild();

		// Persist Search Area Width/Height
		ImGui::GetStateStorage()->SetFloat(ImGui::GetID("NPC::SearchWidth"), search_width);
		ImGui::GetStateStorage()->SetFloat(ImGui::GetID("NPC::SearchHeight"), search_height);
	}

	void NPCWindow::Init()
	{
		g_DescriptionFrameworkInterface = DescriptionFrameworkAPI::GetDescriptionFrameworkInterface001();

		b_ClickToSelect = true;
		b_ClickToFavorite = false;
		b_ClickToPlace = false;
		clickToPlaceCount = 1;

		_itemHovered = false;
		_itemSelected = false;
		selectedNPC = nullptr;
		hoveredNPC = nullptr;

		primaryFilter = RE::FormType::None;
		searchKey = BaseColumn::ID::Name;
		dirty = true;

		selectedMod = "All Mods";

		columnList = NPCColumns();
		ApplyFilters();
	}
}