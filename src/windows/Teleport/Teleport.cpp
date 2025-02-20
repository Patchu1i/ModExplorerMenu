#include "include/T/Teleport.h"

// If I need to add filtering for items like Interior/Exterior or something, reference this:
// std::vector<std::tuple<bool*, AddItemWindow::ItemType, std::string>> AddItemWindow::_filterMap = {
// 	{ &_Alchemy, AddItemWindow::Alchemy, "ALCH" }, { &_Ingredient, AddItemWindow::Ingredient, "INGR" },
// 	{ &_Ammo, AddItemWindow::Ammo, "AMMO" }, { &_Key, AddItemWindow::Key, "KEYS" },
// 	{ &_Misc, AddItemWindow::Misc, "MISC" }, { &_Armor, AddItemWindow::Armor, "ARMO" },
// 	{ &_Book, AddItemWindow::Book, "BOOK" }, { &_Weapon, AddItemWindow::Weapon, "WEAP" }
// };

namespace Modex
{
	void TeleportWindow::Draw(float a_offset)
	{
		const float MIN_SEARCH_HEIGHT = 150.0f;
		const float MIN_SEARCH_WIDTH = 200.0f;
		const float MAX_SEARCH_HEIGHT = ImGui::GetContentRegionAvail().y * 0.75f;
		const float MAX_SEARCH_WIDTH = ImGui::GetContentRegionAvail().x * 0.85f;

		const ImGuiChildFlags flags = ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding;
		float search_height = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("Teleport::SearchHeight"), MIN_SEARCH_HEIGHT);
		float search_width = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("Teleport::SearchWidth"), MAX_SEARCH_WIDTH);
		float window_padding = ImGui::GetStyle().WindowPadding.y;

		// Search Input Area
		ImGui::SameLine();
		ImGui::SetCursorPosY(window_padding);
		const ImVec2 backup_pos = ImGui::GetCursorPos();
		if (ImGui::BeginChild("##Teleport::SearchArea", ImVec2(search_width + 1.0f, search_height), flags, ImGuiWindowFlags_NoFocusOnAppearing)) {
			ShowSearch();
		}
		ImGui::EndChild();

		// Horizontal Search / Table Splitter
		float full_width = search_width;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + a_offset);
		ImGui::SetCursorPosY(backup_pos.y + search_height);
		ImGui::DrawSplitter("##Teleport::HorizontalSplitter", true, &search_height, &full_width, MIN_SEARCH_HEIGHT, MAX_SEARCH_HEIGHT);

		// Table Area
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + a_offset);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (window_padding / 2));
		if (ImGui::BeginChild("##Teleport::TableArea", ImVec2(search_width, 0), flags, ImGuiWindowFlags_NoFocusOnAppearing)) {
			ShowFormTable();
		}
		ImGui::EndChild();

		// Vertical Search Table / Action Splitter
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - window_padding);
		ImGui::SetCursorPosY(window_padding);
		float full_height = ImGui::GetContentRegionAvail().y;
		ImGui::DrawSplitter("##Teleport::VerticalSplitter2", false, &search_width, &full_height, MIN_SEARCH_WIDTH, MAX_SEARCH_WIDTH);

		// Action Area
		ImGui::SameLine();
		ImGui::SetCursorPosY(window_padding);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - window_padding);
		if (ImGui::BeginChild("##Teleport::ActionArea",
				ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), flags, ImGuiWindowFlags_NoFocusOnAppearing)) {
			ShowActions();
		}
		ImGui::EndChild();

		// Persist Search Area Width/Height
		ImGui::GetStateStorage()->SetFloat(ImGui::GetID("Teleport::SearchWidth"), search_width);
		ImGui::GetStateStorage()->SetFloat(ImGui::GetID("Teleport::SearchHeight"), search_height);
	}

	void TeleportWindow::Unload()
	{
		cellList.clear();
		selectedCell = nullptr;
		selectedMod = _T("Show All Plugins");
	}

	void TeleportWindow::Load()
	{
		Data::GetSingleton()->GenerateCellList();
		this->Refresh();
	}

	void TeleportWindow::Init(bool is_default)
	{
		b_ClickToTeleport = true;
		// b_ClickToFavorite = false;

		columnList = TeleportColumns();
		selectedCell = nullptr;

		searchKey = BaseColumn::ID::CellName;
		dirty = true;

		selectedMod = _T("Show All Plugins");

		if (is_default) {
			this->Refresh();
		}
	}
}