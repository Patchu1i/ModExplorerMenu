#include "include/O/Object.h"
#include "include/B/Blacklist.h"

namespace Modex
{
	void ObjectWindow::Draw(float a_offset)
	{
		const float MIN_SEARCH_HEIGHT = 175.0f;
		const float MIN_SEARCH_WIDTH = 200.0f;
		const float MAX_SEARCH_HEIGHT = ImGui::GetContentRegionAvail().y * 0.75f;
		const float MAX_SEARCH_WIDTH = ImGui::GetContentRegionAvail().x * 0.85f;

		const ImGuiChildFlags flags = ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding;
		float search_height = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("Object::SearchHeight"), MIN_SEARCH_HEIGHT);
		float search_width = ImGui::GetStateStorage()->GetFloat(ImGui::GetID("Object::SearchWidth"), MAX_SEARCH_WIDTH);
		float window_padding = ImGui::GetStyle().WindowPadding.y;
		const float button_width = ImGui::GetContentRegionMax().x / 2.0f;
		const float button_height = ImGui::GetFontSize() * 1.5f;
		const float tab_bar_height = button_height + (window_padding * 2);

		ImGui::SameLine();
		ImGui::SetCursorPosY(window_padding);
		ImGui::SetCursorPosX(window_padding + a_offset);
		ImVec2 backup_pos = ImGui::GetCursorPos();

		// Tab Button Area
		if (ImGui::BeginChild("##Object::Blacklist", ImVec2(0.0f, button_height), 0, ImGuiWindowFlags_NoFocusOnAppearing)) {
			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
			if (ImGui::Selectable("Table View", activeViewport == Viewport::TableView, 0, ImVec2(button_width, 0.0f))) {
				activeViewport = Viewport::TableView;
				if (this->tableView.GetTableList().empty()) {
					this->tableView.Refresh();
				}

				this->tableView.BuildPluginList();
			}

			ImGui::SameLine();

			if (ImGui::Selectable("Blacklist", activeViewport == Viewport::BlacklistView, 0, ImVec2(button_width, 0.0f))) {
				activeViewport = Viewport::BlacklistView;
				Blacklist::GetSingleton()->BuildPluginList();
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
			if (ImGui::BeginChild("##Object::SearchArea", ImVec2(search_width - a_offset, search_height), flags, ImGuiWindowFlags_NoFocusOnAppearing)) {
				if (ImGui::IsWindowAppearing()) {
					ImGui::SetKeyboardFocusHere();
				}

				this->tableView.ShowSearch(search_height);
			}
			ImGui::EndChild();

			// Horizontal Search / Table Splitter
			float full_width = search_width - a_offset;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + a_offset);
			ImGui::SetCursorPosY(backup_pos.y + search_height);
			ImGui::DrawSplitter("##Object::HorizontalSplitter", true, &search_height, &full_width, MIN_SEARCH_HEIGHT, MAX_SEARCH_HEIGHT);

			// Table Area
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + a_offset);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (window_padding / 2));
			if (ImGui::BeginChild("##Object::TableArea", ImVec2(search_width - a_offset, 0), flags, ImGuiWindowFlags_NoFocusOnAppearing)) {
				this->tableView.ShowSort();
				this->tableView.Draw();
			}
			ImGui::EndChild();

			// Vertical Search Table / Action Splitter
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - window_padding);
			ImGui::SetCursorPosY(tab_bar_height - window_padding);
			float full_height = ImGui::GetContentRegionAvail().y;
			ImGui::DrawSplitter("##Object::VerticalSplitter2", false, &search_width, &full_height, MIN_SEARCH_WIDTH, MAX_SEARCH_WIDTH);

			// Action Area
			ImGui::SameLine();
			ImGui::SetCursorPosY(tab_bar_height - window_padding);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - window_padding);
			if (ImGui::BeginChild("##Object::ActionArea",
					ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), flags, ImGuiWindowFlags_NoFocusOnAppearing)) {
				this->ShowActions();
			}
			ImGui::EndChild();

			// Persist Search Area Width/Height
			ImGui::GetStateStorage()->SetFloat(ImGui::GetID("Object::SearchWidth"), search_width);
			ImGui::GetStateStorage()->SetFloat(ImGui::GetID("Object::SearchHeight"), search_height);
		}
	}

	void ObjectWindow::Unload()
	{
		tableView.Unload();
	}

	void ObjectWindow::Load()
	{
		tableView.Load();
	}

	void ObjectWindow::Init(bool is_default)
	{
		b_ClickToPlace = true;
		clickToPlaceCount = 1;

		activeViewport = Viewport::TableView;

		tableView.SetGenerator([]() { return Data::GetSingleton()->GetObjectList(); });
		tableView.SetupSearch(Data::PLUGIN_TYPE::OBJECT);
		tableView.SetClickAmount(&clickToPlaceCount);
		tableView.Init();

		if (is_default) {
			tableView.Refresh();
			tableView.BuildPluginList();
		}
	}
}