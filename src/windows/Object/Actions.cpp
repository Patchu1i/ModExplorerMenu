#include "include/C/Console.h"
#include "include/O/Object.h"

namespace Modex
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

		if (ImGui::m_Selectable(_TICON(ICON_RPG_HAND, "GENERAL_CLICK_TO_PLACE"), b_ClickToPlace, a_style, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_ClickToFavorite = false;
			b_ClickToSelect = false;
		}

		if (ImGui::m_Selectable(_TICON(ICON_RPG_MULTI_NPC, "GENERAL_CLICK_TO_SELECT"), b_ClickToSelect, a_style, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_ClickToFavorite = false;
			b_ClickToPlace = false;
		}

		if (ImGui::m_Selectable(_TICON(ICON_RPG_SPAWNED_NPC, "GENERAL_CLICK_TO_FAVORITE"), b_ClickToFavorite, a_style, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_ClickToSelect = false;
			b_ClickToPlace = false;
		}

		if (b_ClickToPlace) {
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			// Add Count Input.
			ImGui::SetNextItemWidth(button_width);
			ImGui::InputInt("##ObjectPlaceCount", &clickToPlaceCount, 1, 100, ImGuiInputTextFlags_CharsDecimal);
		}

		ImGui::PopStyleColor(3);
		ImGui::SeparatorText(_TFM("Actions", ":"));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(a_style.secondaryButton.x, a_style.secondaryButton.y, a_style.secondaryButton.z, a_style.secondaryButton.w));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(a_style.secondaryButtonActive.x, a_style.secondaryButtonActive.y, a_style.secondaryButtonActive.z, a_style.secondaryButtonActive.w));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(a_style.secondaryButtonHovered.x, a_style.secondaryButtonHovered.y, a_style.secondaryButtonHovered.z, a_style.secondaryButtonHovered.w));

		if (ImGui::m_Button(_T("OBJECT_PLACE_SELECTED"), a_style, ImVec2(button_width, 0))) {
			if (selectedObject != nullptr) {
				Console::PlaceAtMe(selectedObject->GetFormID(), 1);
				Console::StartProcessThread();
			}
		}

		if (ImGui::m_Button(_T("GENERAL_PLACE_ALL"), a_style, ImVec2(button_width, 0))) {
			for (auto& obj : objectList) {
				Console::PlaceAtMe(obj->GetFormID(), 1);
			}

			Console::StartProcessThread();
		}

		ImGui::PopStyleColor(3);  // End of Green Buttons

		if (ImGui::m_Button(_T("GENERAL_GOTO_FAVORITE"), a_style, ImVec2(button_width, 0))) {
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

		auto SplitString = [](std::string a_string, std::string a_delimiter) -> std::vector<std::string> {
			std::vector<std::string> strings;
			std::stringstream ss(a_string);
			std::string string;

			while (std::getline(ss, string, a_delimiter[0])) {
				strings.push_back(string);
			}

			return strings;
		};

		if (obj->editorid.empty()) {
			return;
		}

		ImGui::NewLine();

		RE::TESModel* model = obj->TESForm->As<RE::TESModel>();

		if (model != nullptr) {
			std::string modelFullPath = model->GetModel();

			if (modelFullPath.empty()) {
				return;
			}

			std::vector<std::string> modelPath = SplitString(modelFullPath, "\\");

			if (ImGui::TreeNodeEx(_TFM("Model Path", ":"), ImGuiTreeNodeFlags_DefaultOpen)) {
				for (size_t i = 0; i < modelPath.size(); ++i) {
					if (i < modelPath.size() - 1) {
						ImGui::Text("%s/", modelPath[i].c_str());
						ImGui::Indent(5.0f);
					} else {
						ImGui::Text("%s", modelPath[i].c_str());
					}
				}

				for (size_t i = 0; i < modelPath.size() - 1; ++i) {
					ImGui::Unindent(5.0f);
				}

				ImGui::TreePop();
			}
		}

		ImGui::NewLine();
	}
}