#include "include/A/AddItem.h"
#include "include/B/Blacklist.h"

namespace Modex
{

	// https://github.com/ocornut/imgui/issues/319
	void AddItemWindow::Draw(float a_offset)
	{
		float MIN_SEARCH_HEIGHT = 175.0f;
		const float MIN_SEARCH_WIDTH = 200.0f;
		const float MAX_SEARCH_HEIGHT = ImGui::GetContentRegionAvail().y * 0.75f;
		const float MAX_SEARCH_WIDTH = ImGui::GetContentRegionAvail().x * 0.85f;

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
		float window_padding = ImGui::GetStyle().WindowPadding.y;
		const float button_width = ImGui::GetContentRegionAvail().x / 2.0f;
		const float button_height = ImGui::GetFontSize() * 1.5f;
		const float tab_bar_height = button_height + (window_padding * 2);

		// Tab Button Area
		ImGui::SameLine();
		ImGui::SetCursorPosY(window_padding);
		ImVec2 backup_pos = ImGui::GetCursorPos();
		if (ImGui::BeginChild("##AddItem::Blacklist", ImVec2(0.0f, button_height), 0, ImGuiWindowFlags_NoFocusOnAppearing)) {
			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
			if (ImGui::Selectable("Table View", activeViewport == Viewport::TableView, 0, ImVec2(button_width, 0.0f))) {
				activeViewport = Viewport::TableView;
				Refresh();
			}

			ImGui::SameLine();

			if (ImGui::Selectable("Blacklist", activeViewport == Viewport::BlacklistView, 0, ImVec2(button_width, 0.0f))) {
				activeViewport = Viewport::BlacklistView;
			}
			ImGui::PopStyleVar();
		}
		ImGui::EndChild();

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
		b_AddToFavorites = false;
		clickToAddCount = 1;

		activeViewport = Viewport::TableView;

		searchKey = BaseColumn::ID::Name;
		dirty = true;

		selectedMod = "All Mods";

		columnList = AddItemColumns();
		ApplyFilters();
	}
}  // namespace Modex