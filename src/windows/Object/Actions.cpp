#include "Console.h"
#include "Object.h"

namespace ModExplorerMenu
{
	void ObjectWindow::ShowActions(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_config;

		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

		const float button_height = ImGui::GetFontSize() * 1.5f;
		const float button_width = ImGui::GetContentRegionAvail().x;

		ImGui::SeparatorText(_TFM("Behavior", ":"));

		ImGui::PushStyleColor(ImGuiCol_Header, a_style.button);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, a_style.buttonActive);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, a_style.buttonHovered);

		if (ImGui::Selectable(_TICON(ICON_RPG_MULTI_NPC, "GENERAL_CLICK_TO_SELECT"), &b_ClickToSelect, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_ClickToFavorite = false;
		}

		if (ImGui::Selectable(_TICON(ICON_RPG_SPAWNED_NPC, "GENERAL_CLICK_TO_FAVORITE"), &b_ClickToFavorite, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_ClickToSelect = false;
		}

		ImGui::PopStyleColor(3);
		ImGui::SeparatorText(_TFM("Actions", ":"));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(a_style.button.x, a_style.button.y + 0.3f, a_style.button.y, a_style.button.w));

		if (ImGui::Button(_T("OBJECT_PLACE_SELECTED"), ImVec2(button_width, button_height))) {
			if (selectedObject != nullptr) {
				Console::PlaceAtMe(selectedObject->GetFormID(), 1);
				Console::StartProcessThread();
			}
		}

		if (ImGui::Button(_T("GENERAL_PLACE_ALL"), ImVec2(button_width, button_height))) {
			for (auto& obj : objectList) {
				Console::PlaceAtMe(obj->GetFormID(), 1);
			}

			Console::StartProcessThread();
		}

		ImGui::PopStyleColor(1);  // End of Green Buttons

		if (ImGui::Button(_T("GENERAL_GOTO_FAVORITE"), ImVec2(button_width, button_height))) {
			selectedMod = "Favorite";
			ApplyFilters();
		}

		if (selectedObject == nullptr && hoveredObject == nullptr) {
			ImGui::PopStyleVar(2);  // End of SelectableTextAlign and ButtonTextAlign
			return;
		}

		ImGui::SeparatorText(_TFM("Info", ":"));

		StaticObject* obj = nullptr;

		if (hoveredObject != nullptr) {
			obj = hoveredObject;
		} else if (selectedObject != nullptr) {
			obj = selectedObject;
		}

		if (obj == nullptr) {
			ImGui::PopStyleVar(2);  // End of SelectableTextAlign and ButtonTextAlign
			return;
		}

		// Name Bar

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		const auto cursor = ImGui::GetCursorScreenPos();
		const auto size = ImGui::GetContentRegionAvail();
		const auto color = ImGui::GetStyleColorVec4(ImGuiCol_Border);
		draw_list->AddRectFilled(cursor, ImVec2(cursor.x + size.x, cursor.y + ImGui::GetFontSize() * 3.5f), ImGui::ColorConvertFloat4ToU32(ImVec4(0.15f, 0.15f, 0.15f, 0.5f)));
		draw_list->AddRect(cursor, ImVec2(cursor.x + size.x, cursor.y + ImGui::GetFontSize() * 3.5f), ImGui::ColorConvertFloat4ToU32(color));

		const auto name = obj->editorid.c_str();
		ImGui::NewLine();
		ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(name));
		ImGui::Text(name);
		ImGui::NewLine();

		ImGui::PopStyleVar(2);  // End of SelectableTextAlign and ButtonTextAlign
	}
}