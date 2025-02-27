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
		const float MAX_KITBAR_HEIGHT = 400.0f;

		// I probably should handle this better, but It's kind of nice
		// that the search data is in this class, so I can reference it for behavior like this.
		if (this->tableView.GetPrimaryFilter() == RE::FormType::Armor || this->tableView.GetPrimaryFilter() == RE::FormType::Weapon) {
			MIN_SEARCH_HEIGHT = 200.0f;

			// Ensure minimum height after resizing.
			if (ImGui::GetStateStorage()->GetFloat(ImGui::GetID("AddItem::SearchHeight"), MIN_SEARCH_HEIGHT) < MIN_SEARCH_HEIGHT) {
				ImGui::GetStateStorage()->SetFloat(ImGui::GetID("AddItem::SearchHeight"), MIN_SEARCH_HEIGHT);
			}
		}

		const ImGuiChildFlags flags = ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding;
		float search_height = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("AddItem::SearchHeight"), MIN_SEARCH_HEIGHT);
		float search_width = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("AddItem::SearchWidth"), MAX_SEARCH_WIDTH - 75.0f);  // 75.0f is padding for initial size.
		float kitbar_height = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("AddItem::KitBarHeight"), MAX_KITBAR_HEIGHT);
		float window_padding = ImGui::GetStyle().WindowPadding.y;
		const float button_width = ImGui::GetContentRegionAvail().x / static_cast<int>(Viewport::Count);
		const float button_height = ImGui::GetFontSize() * 1.5f;
		const float tab_bar_height = button_height + (window_padding * 2.0f);

		// Tab Button Area
		ImGui::SameLine();
		ImGui::SetCursorPosY(window_padding);

		ImVec2 backup_pos = ImGui::GetCursorPos();
		if (ImGui::BeginChild("##AddItem::TabBar", ImVec2(0.0f, button_height), 0, ImGuiWindowFlags_NoFocusOnAppearing)) {
			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
			if (ImGui::Selectable("Table View", activeViewport == Viewport::TableView, 0, ImVec2(button_width, 0.0f))) {
				activeViewport = Viewport::TableView;
				if (this->tableView.GetTableList().empty()) {
					this->tableView.Refresh();
				}

				this->tableView.BuildPluginList();
				this->tableView.RemoveDragDropTarget(kitTableView.GetDragDropID());
			}

			ImGui::SameLine();

			if (ImGui::Selectable("Blacklist", activeViewport == Viewport::BlacklistView, 0, ImVec2(button_width, 0.0f))) {
				activeViewport = Viewport::BlacklistView;
				Blacklist::GetSingleton()->BuildPluginList();  // TODO: Need to track this in some global state, along with other stuff..
				this->tableView.RemoveDragDropTarget(kitTableView.GetDragDropID());
			}

			ImGui::SameLine();

			if (ImGui::Selectable("Kit Editor", activeViewport == Viewport::KitView, 0, ImVec2(button_width, 0.0f))) {
				activeViewport = Viewport::KitView;
				this->tableView.AddDragDropTarget(kitTableView.GetDragDropID(), &kitTableView);
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
				// ShowSearch();
				tableView.ShowSearch(search_height);
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
				tableView.ShowSort();
				tableView.Draw();
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
				kitTableView.ShowSort();
				kitTableView.Draw();
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
				tableView.ShowSearch(search_height);
			}
			ImGui::EndChild();

			// Horizontal Search / Table Splitter
			float full_width = search_width;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + a_offset);
			ImGui::SetCursorPosY(backup_pos.y + search_height);
			ImGui::DrawSplitter("##AddItem::HorizontalSplitter", true, &search_height, &full_width, MIN_SEARCH_HEIGHT, MAX_SEARCH_HEIGHT);

			// Table Area
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + a_offset);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (window_padding / 2));
			if (ImGui::BeginChild("##AddItem::TableArea", ImVec2(search_width, 0), flags, ImGuiWindowFlags_NoFocusOnAppearing)) {
				tableView.ShowSort();
				tableView.Draw();
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
			ImGui::GetStateStorage()->SetFloat(ImGui::GetID("AddItem::SearchWidth"), search_width);
			ImGui::GetStateStorage()->SetFloat(ImGui::GetID("AddItem::SearchHeight"), search_height);
		}
	}

	void AddItemWindow::Unload()
	{
		tableView.Unload();
		kitTableView.Unload();
	}

	void AddItemWindow::Load()
	{
		tableView.Load();
		kitTableView.Load();
	}

	void AddItemWindow::Init(bool is_default)
	{
		b_AddToInventory = true;
		b_PlaceOnGround = false;
		clickToAddCount = 1;

		activeViewport = Viewport::TableView;

		selectedKit = _T("None");

		tableView.SetGenerator([]() { return Data::GetSingleton()->GetAddItemList(); });
		tableView.SetupSearch(Data::PLUGIN_TYPE::ITEM);
		tableView.SetDragDropID("FROM_TABLE");
		tableView.SetClickAmount(&clickToAddCount);
		tableView.SetDoubleClickBehavior(&b_AddToInventory);  // This is binary, what if we need other behaviors?
		tableView.AddFlag(TableView<ItemData>::ModexTableFlag_EnablePluginKitView);
		tableView.AddFlag(TableView<ItemData>::ModexTableFlag_EnableEnchantmentSort);
		tableView.AddFlag(TableView<ItemData>::ModexTableFlag_EnableNonPlayableSort);
		tableView.Init();

		if (is_default) {
			tableView.Refresh();
			tableView.BuildPluginList();
		}

		kitTableView.SetGenerator([this]() { return PersistentData::GetKitItems(selectedKit); });
		kitTableView.SetKitPointer(&selectedKit);
		kitTableView.SetDragDropID("FROM_KIT");
		kitTableView.AddFlag(TableView<ItemData>::ModexTableFlag_Kit);
		kitTableView.SetCompactView(true);
		kitTableView.Init();

		// The reason for this is kind of annoying, but basically, I need data to refer to
		// when enacting a drag 'n drop event in order to create the ItemData object on either side.
		// Since the table is indexed by ImGuiID, and we utilize ImGuiID for the drag payload.
		// It's easy to just index the original table for the item data based on the ImGuiID provided.

		kitTableView.AddDragDropTarget(tableView.GetDragDropID(), &tableView);
	}
}  // namespace Modex