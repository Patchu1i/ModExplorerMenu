#include "include/N/NPC.h"

namespace Modex
{
	void NPCWindow::Draw()
	{
		constexpr auto flags = ImGuiOldColumnFlags_NoResize;
		ImGui::BeginColumns("##HorizontalSplit", 2, flags);

		const float width = ImGui::GetWindowWidth();
		ImGui::SetColumnWidth(0, width * 0.75f);

		// Left Column
		ShowSearch();
		ShowFormTable();

		// Right Column
		ImGui::NextColumn();
		ShowActions();
		ImGui::EndColumns();
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