#include "include/C/Console.h"
#include "include/M/Menu.h"
#include "include/T/Teleport.h"
#include "include/U/Util.h"

namespace Modex
{
	void TeleportWindow::AddCellToRecent(const std::string& a_editorid)
	{
		PersistentData::AddRecentItem<CellData>(a_editorid, 0);
		this->refreshRecentList = true;
	}

	void TeleportWindow::AddCellToFavorite(const std::string& a_editorid)
	{
		PersistentData::GetSingleton()->AddCellToFavorite(a_editorid);
		this->refreshFavoriteList = true;
	}

	void TeleportWindow::RemoveCellFromFavorite(const std::string& a_editorid)
	{
		PersistentData::GetSingleton()->RemoveCellFromFavorite(a_editorid);
		this->refreshFavoriteList = true;
	}

	void TeleportWindow::LoadRecentCells()
	{
		std::vector<std::pair<std::string, std::uint32_t>> recentItems;
		this->recentList.clear();
		this->recentList.reserve(recentItems.size());  // TODO: Use configurable size.

		// Load recent items from PersistentData
		PersistentData::GetSingleton()->GetRecentItems<CellData>(recentItems);

		if (recentItems.empty()) {
			return;
		}

		for (const auto& pair : recentItems) {
			auto& cellData = Data::GetSingleton()->GetCellByEditorID(pair.first);

			recentList.emplace_back(std::make_unique<CellData>(cellData));
		}
	}

	void TeleportWindow::LoadFavoriteCells()
	{
		this->favoriteList.clear();

		// Load favorite items from PersistentData
		PersistentData::GetSingleton()->GetFavoriteItems(this->favoriteList);

		if (this->favoriteList.empty()) {
			return;
		}
	}

	void TeleportWindow::ShowActions()
	{
		auto a_style = Settings::GetSingleton()->GetStyle();

		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

		const float button_height = ImGui::GetFontSize() * 1.5f;
		const float button_width = ImGui::GetContentRegionAvail().x;

		ImGui::SubCategoryHeader(_T("GENERAL_DOUBLE_CLICK_BEHAVIOR"));

		if (ImGui::GradientSelectableEX(_TICON(ICON_LC_MAP_PIN_PLUS, "TELE_CLICK_TO_TELEPORT"), b_ClickToTeleport, ImVec2(button_width, button_height))) {
			b_ClickToTeleport = true;
		}

		ImGui::PopStyleVar(2);

		ImGui::SubCategoryHeader(_T("Favorite"));

		auto constexpr flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration |
		                       ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;

		bool showScrollArrow = false;
		if (ImGui::BeginChild("##TeleportFavoriteScroll", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2.0f), false, flags)) {
			for (auto& editorid : this->favoriteList) {
				auto cell = Data::GetSingleton()->GetCellByEditorID(editorid);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, ImGui::GetFontSize() * 1.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
				ImGui::PushID(editorid.c_str());
				if (ImGui::Selectable(cell.cellName.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_SpanAllColumns)) {
					Console::Teleport(editorid);
					Console::StartProcessThread();
					this->AddCellToRecent(editorid);

					Menu::GetSingleton()->Close();
				}

				ImGui::PopStyleColor(1);
				ImGui::PopStyleVar(2);

				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("%s %s", Utils::IconMap["EDITORID"].c_str(), editorid.c_str());
					ImGui::Text("%s %s", Utils::IconMap["CELL"].c_str(), cell.cellName.c_str());
					ImGui::Text("%s %s", Utils::IconMap["LAND"].c_str(), cell.filename.c_str());
					ImGui::EndTooltip();
				}

				ImGui::PopID();

				// Remove middle-click handler for recent items as it doesn't apply here

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
					ImGui::OpenPopup("##Teleport::Favorite::ContextMenu");
					this->hoveredCellEditorID = editorid;
				}
			}

			if (ImGui::BeginPopup("##Teleport::Favorite::ContextMenu")) {
				if (auto cell = &Data::GetSingleton()->GetCellByEditorID(this->hoveredCellEditorID)) {
					this->ShowTeleportContextMenu(*cell);
				}

				ImGui::EndPopup();
			}

			if (ImGui::GetScrollY() < ImGui::GetScrollMaxY()) {
				showScrollArrow = true;
			}

			ImGui::EndChild();
		}

		if (this->favoriteList.size() * ImGui::GetFontSize() > (ImGui::GetContentRegionAvail().y / 2.0f)) {
			if (showScrollArrow) {
				auto drawList = ImGui::GetWindowDrawList();
				const float arrowSize = ImGui::GetFontSize();
				ImVec2 pos = ImGui::GetCursorScreenPos() - ImVec2(0, arrowSize + ImGui::GetStyle().ItemSpacing.y);
				ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, arrowSize);
				drawList->AddText(pos, ImGui::GetColorU32(ImGuiCol_Text), ICON_LC_ARROW_DOWN);
				drawList->AddText(pos + ImVec2(size.x - arrowSize, 0.0f), ImGui::GetColorU32(ImGuiCol_Text), ICON_LC_ARROW_DOWN);
			}
		}

		ImGui::SubCategoryHeader(_T("Recent"));

		showScrollArrow = false;
		if (ImGui::BeginChild("##TeleportRecentScroll", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false, flags)) {
			for (const auto& cell : this->recentList) {
				const auto& editorid = cell.get()->editorid;
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, ImGui::GetFontSize() * 1.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
				ImGui::PushID(editorid.c_str());
				if (ImGui::Selectable(cell.get()->cellName.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_SpanAllColumns)) {
					Console::Teleport(editorid);
					Console::StartProcessThread();
					this->AddCellToRecent(editorid);

					Menu::GetSingleton()->Close();
				}

				ImGui::PopStyleColor(1);
				ImGui::PopStyleVar(2);

				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("%s %s", Utils::IconMap["EDITORID"].c_str(), editorid.c_str());
					ImGui::Text("%s %s", Utils::IconMap["CELL"].c_str(), cell.get()->cellName.c_str());
					ImGui::Text("%s %s", Utils::IconMap["LAND"].c_str(), cell.get()->filename.c_str());
					ImGui::EndTooltip();
				}

				ImGui::PopID();

				if (ImGui::IsItemClicked(ImGuiMouseButton_Middle)) {
					PersistentData::GetSingleton()->RemoveCellFromFavorite(editorid);
					this->refreshFavoriteList = true;
				}

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
					ImGui::OpenPopup("##Teleport::Recent::ContextMenu");
					this->hoveredCellEditorID = editorid;
				}
			}

			if (ImGui::BeginPopup("##Teleport::Recent::ContextMenu")) {
				if (auto cell = &Data::GetSingleton()->GetCellByEditorID(this->hoveredCellEditorID)) {
					this->ShowTeleportContextMenu(*cell);
				}

				ImGui::EndPopup();
			}

			if (ImGui::GetScrollY() < ImGui::GetScrollMaxY()) {
				showScrollArrow = true;
			}

			ImGui::EndChild();
		}

		if (this->recentList.size() * ImGui::GetFontSize() > (ImGui::GetContentRegionAvail().y)) {
			if (showScrollArrow) {
				auto drawList = ImGui::GetWindowDrawList();
				const float arrowSize = ImGui::GetFontSize();
				ImVec2 pos = ImGui::GetCursorScreenPos() - ImVec2(0, arrowSize + ImGui::GetStyle().ItemSpacing.y);
				ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, arrowSize);
				drawList->AddText(pos, ImGui::GetColorU32(ImGuiCol_Text), ICON_LC_ARROW_DOWN);
				drawList->AddText(pos + ImVec2(size.x - arrowSize, 0.0f), ImGui::GetColorU32(ImGuiCol_Text), ICON_LC_ARROW_DOWN);
			}
		}
	}
}