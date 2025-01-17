#include "include/A/AddItem.h"

namespace Modex
{
	// Main Draw function for AddItem, called by Frame::Draw()
	void AddItemWindow::Draw()
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

		// Book Prompt
		if (openBook != nullptr) {
			ShowBookPreview();
		}
	}

	void AddItemWindow::Init()
	{
		g_DescriptionFrameworkInterface = DescriptionFrameworkAPI::GetDescriptionFrameworkInterface001();

		primaryFilter = RE::FormType::None;
		openBook = nullptr;
		itemPreview = nullptr;

		b_AddToInventory = true;
		b_PlaceOnGround = false;
		b_AddToFavorites = false;
		clickToAddCount = 1;

		searchKey = BaseColumn::ID::Name;
		dirty = true;

		selectedMod = "All Mods";

		columnList = AddItemColumns();
		ApplyFilters();
	}
}