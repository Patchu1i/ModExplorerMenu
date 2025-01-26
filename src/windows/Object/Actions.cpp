#include "include/C/Console.h"
#include "include/O/Object.h"

namespace Modex
{
	void ObjectWindow::ShowActions()
	{
		auto a_style = Settings::GetSingleton()->GetStyle();

		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

		const float button_height = ImGui::GetFontSize() * 1.5f;
		const float button_width = ImGui::GetContentRegionAvail().x;

		ImGui::SubCategoryHeader(_T("Behavior"), ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		// Click To Place Toggle
		if (ImGui::GradientSelectableEX(_TICON(ICON_RPG_HAND, "GENERAL_CLICK_TO_PLACE"), b_ClickToPlace, ImVec2(button_width, button_height))) {
			b_ClickToFavorite = false;
			b_ClickToSelect = false;
		}

		// Click To Select Toggle
		if (ImGui::GradientSelectableEX(_TICON(ICON_RPG_MULTI_NPC, "GENERAL_CLICK_TO_SELECT"), b_ClickToSelect, ImVec2(button_width, button_height))) {
			b_ClickToFavorite = false;
			b_ClickToPlace = false;
		}

		// Click To Favorite Toggle
		if (ImGui::GradientSelectableEX(_TICON(ICON_RPG_SPAWNED_NPC, "GENERAL_CLICK_TO_FAVORITE"), b_ClickToFavorite, ImVec2(button_width, button_height))) {
			b_ClickToSelect = false;
			b_ClickToPlace = false;
		}

		if (b_ClickToPlace) {
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			// Add Count Input.
			ImGui::SetNextItemWidth(button_width);
			ImGui::InputInt("##ObjectPlaceCount", &clickToPlaceCount, 1, 100, ImGuiInputTextFlags_CharsDecimal);
		}

		ImGui::Spacing();
		ImGui::SubCategoryHeader(_T("Actions"), ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(a_style.secondaryButton.x, a_style.secondaryButton.y, a_style.secondaryButton.z, a_style.secondaryButton.w));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(a_style.secondaryButtonActive.x, a_style.secondaryButtonActive.y, a_style.secondaryButtonActive.z, a_style.secondaryButtonActive.w));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(a_style.secondaryButtonHovered.x, a_style.secondaryButtonHovered.y, a_style.secondaryButtonHovered.z, a_style.secondaryButtonHovered.w));

		if (ImGui::GradientButton(_T("OBJECT_PLACE_SELECTED"), ImVec2(button_width, 0))) {
			if (selectedObject != nullptr) {
				Console::PlaceAtMe(selectedObject->GetFormID(), 1);
				Console::StartProcessThread();
			}
		}

		if (ImGui::GradientButton(_T("GENERAL_PLACE_ALL"), ImVec2(button_width, 0))) {
			if (objectList.size() > 30) {
				ImGui::OpenPopup(_T("AIM_LARGE_QUERY"));
			} else {
				for (auto& obj : objectList) {
					Console::PlaceAtMe(obj->GetFormID(), 1);
				}
			}

			Console::StartProcessThread();
		}

		ImGui::PopStyleColor(3);  // End of Green Buttons

		ImGui::ShowWarningPopup(_T("AIM_LARGE_QUERY"), [&]() {
			for (auto& obj : objectList) {
				Console::PlaceAtMe(obj->GetFormID(), 1);
			}
		});

		// Goto Favorite List
		if (ImGui::GradientButton(_T("GENERAL_GOTO_FAVORITE"), ImVec2(button_width, 0))) {
			selectedMod = "Favorite";
			ApplyFilters();
		}

		if (selectedObject == nullptr && hoveredObject == nullptr) {
			ImGui::PopStyleVar(2);  // End of SelectableTextAlign and ButtonTextAlign
			return;
		}

		ImGui::Spacing();
		ImGui::SubCategoryHeader(_T("Info"), ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		ObjectData* obj = nullptr;

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
		draw_list->AddRectFilled(cursor, ImVec2(cursor.x + size.x, cursor.y + ImGui::GetFontSize() * 2.5f), ImGui::ColorConvertFloat4ToU32(ImVec4(0.15f, 0.15f, 0.15f, 0.5f)));
		draw_list->AddRect(cursor, ImVec2(cursor.x + size.x, cursor.y + ImGui::GetFontSize() * 2.5f), ImGui::ColorConvertFloat4ToU32(color));

		auto name = obj->GetName();

		if (name.empty()) {
			name = obj->GetEditorID();
		}

		ImGui::NewLine();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(name.c_str()));
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetFontSize() / 2);
		ImGui::Text(name.c_str());
		ImGui::NewLine();
		ImGui::PopStyleVar();

		ImGui::PopStyleVar(2);  // End of SelectableTextAlign and ButtonTextAlign

		// Load Order Info Pane
		// See ItemPreview.h for other implementation.
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(_T("Load Order")));
		ImGui::Text(_T("Load Order"));
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		const auto sourceFiles = obj->GetForm()->sourceFiles.array;

		if (sourceFiles) {
			for (uint32_t i = 0; i < sourceFiles->size(); i++) {
				if (const auto file = (*sourceFiles)[i]) {
					auto fileName = ValidateTESFileName(file);

					if (i == 0 && sourceFiles->size() > 1) {
						ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", fileName.c_str());
					} else if (i == sourceFiles->size() - 1) {
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", fileName.c_str());
					} else {
						ImGui::Text("%s", fileName.c_str());
					}
				}
			}
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		// Model Info
		auto SplitString = [](std::string a_string, std::string a_delimiter) -> std::vector<std::string> {
			std::vector<std::string> strings;
			std::stringstream ss(a_string);
			std::string string;

			while (std::getline(ss, string, a_delimiter[0])) {
				strings.push_back(string);
			}

			return strings;
		};

		if (obj->GetEditorID().empty()) {
			return;
		}

		ImGui::NewLine();

		RE::TESModel* model = obj->GetForm()->As<RE::TESModel>();

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