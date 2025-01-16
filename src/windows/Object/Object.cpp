#include "include/O/Object.h"

// If I need to add filtering for items like Interior/Exterior or something, reference this:
// std::vector<std::tuple<bool*, AddItemWindow::ItemType, std::string>> AddItemWindow::_filterMap = {
// 	{ &_Alchemy, AddItemWindow::Alchemy, "ALCH" }, { &_Ingredient, AddItemWindow::Ingredient, "INGR" },
// 	{ &_Ammo, AddItemWindow::Ammo, "AMMO" }, { &_Key, AddItemWindow::Key, "KEYS" },
// 	{ &_Misc, AddItemWindow::Misc, "MISC" }, { &_Armor, AddItemWindow::Armor, "ARMO" },
// 	{ &_Book, AddItemWindow::Book, "BOOK" }, { &_Weapon, AddItemWindow::Weapon, "WEAP" }
// };

namespace Modex
{
	void ObjectWindow::Draw()
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

	void ObjectWindow::Init()
	{
		b_ClickToSelect = true;
		b_ClickToPlace = false;
		b_ClickToFavorite = false;
		clickToPlaceCount = 1;

		_itemHovered = false;
		_itemSelected = false;
		hoveredObject = nullptr;
		selectedObject = nullptr;

		primaryFilter = RE::FormType::None;
		columnList = ObjectColumns();

		searchKey = BaseColumn::ID::EditorID;
		dirty = true;

		ApplyFilters();
	}
}