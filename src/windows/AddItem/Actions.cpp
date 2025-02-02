#include "include/A/AddItem.h"
#include "include/C/Console.h"
#include "include/I/ItemPreview.h"

namespace Modex
{
	void AddItemWindow::ShowActions()
	{
		auto a_style = Settings::GetSingleton()->GetStyle();

		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

		ImGui::BeginChild("##AddItemWindow::Actions", ImVec2(ImGui::GetContentRegionAvail()), ImGuiChildFlags_AlwaysUseWindowPadding);

		const float button_height = ImGui::GetFontSize() * 1.5f;
		const float button_width = ImGui::GetContentRegionAvail().x;

		// ImGui::SubCategoryHeader(_T("Behavior"), ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::SubCategoryHeader(_T("Behavior"));

		// Add To Inventory Toggle.
		if (ImGui::GradientSelectableEX(_TICON(ICON_LC_CIRCLE_PLUS, "AIM_ADD"), b_AddToInventory, ImVec2(button_width, button_height))) {
			b_PlaceOnGround = false;
			// b_AddToFavorites = false;
			b_AddToInventory = true;
		};

		// Place On Ground Toggle.
		if (ImGui::GradientSelectableEX(_TICON(ICON_LC_MAP_PIN_PLUS, "AIM_PLACE"), b_PlaceOnGround, ImVec2(button_width, button_height))) {
			b_AddToInventory = false;
			// b_AddToFavorites = false;
			b_PlaceOnGround = true;
		}

		// Add To Favorites Toggle.
		// if (ImGui::GradientSelectableEX(_TICON(ICON_RPG_HEART, "AIM_FAVORITE"), b_AddToFavorites, ImVec2(button_width, button_height))) {
		// 	b_AddToInventory = false;
		// 	b_PlaceOnGround = false;
		// 	// b_AddToFavorites = true;
		// }

		if (b_AddToInventory || b_PlaceOnGround) {
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			// Add Count Input.
			ImGui::SetNextItemWidth(button_width);
			ImGui::InputInt("##AddItemCount", &clickToAddCount, 1, 100, ImGuiInputTextFlags_CharsDecimal);
		}

		ImGui::Spacing();
		// ImGui::SubCategoryHeader(_T("Actions"), ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::SubCategoryHeader(_T("Actions"));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(a_style.secondaryButton.x, a_style.secondaryButton.y, a_style.secondaryButton.z, a_style.secondaryButton.w));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(a_style.secondaryButtonActive.x, a_style.secondaryButtonActive.y, a_style.secondaryButtonActive.z, a_style.secondaryButtonActive.w));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(a_style.secondaryButtonHovered.x, a_style.secondaryButtonHovered.y, a_style.secondaryButtonHovered.z, a_style.secondaryButtonHovered.w));

		// Add All From Table Shortcut.
		if (ImGui::GradientButton(_T("AIM_ADD_ALL"), ImVec2(button_width, 0))) {
			for (auto& item : itemList) {
				Console::AddItem(item->GetFormID());
			}

			Console::StartProcessThread();
		}

		// Place All from Table Shortcut.
		if (ImGui::GradientButton(_T("GENERAL_PLACE_ALL"), ImVec2(button_width, 0))) {
			if (itemList.size() > 30) {
				ImGui::OpenPopup(_T("AIM_LARGE_QUERY"));
			} else {
				for (auto& item : itemList) {
					Console::PlaceAtMe(item->GetFormID());
				}
			}

			Console::StartProcessThread();
		}

		ImGui::PopStyleColor(3);  // End of Secondary Buttons

		ImGui::ShowWarningPopup(_T("AIM_LARGE_QUERY"), [&]() {
			for (auto& item : itemList) {
				Console::PlaceAtMe(item->GetFormID());
			}
		});

		// Spawn Container with Table contents
		if (ImGui::GradientButton(_T("AIM_ADD_TO_CONTAINER"), ImVec2(button_width, 0))) {
			if (auto dataHandler = RE::TESDataHandler::GetSingleton()) {
				auto form = RE::TESForm::LookupByID(RE::FormID(0x000D762F));
				auto player = RE::PlayerCharacter::GetSingleton();

				if (form) {
					auto position = player->GetPosition();
					auto angle = player->GetAngle();

					// TODO: Maybe position it better?

					auto objHandle = dataHandler->CreateReferenceAtLocation(
						form->As<RE::TESBoundObject>(),  // RE::TESBoundObject* a_baseObj,
						position,                        // const RE::NiPoint3& a_position,
						{ 0.0f, angle.y, 0.0f },         // const RE::NiPoint3& a_rotation,
						player->GetParentCell(),         // RE::TESObjectCELL* a_parentCell,
						player->GetWorldspace(),         // RE::TESWorldSpace* a_worldspace,
						nullptr,                         // RE::TESObjectREFR *a_alreadyCreatedRef,
						nullptr,                         // RE::BGSPrimitive *a_primitive,
						RE::ObjectRefHandle(),           // const RE::ObjectRefHandle &a_linkedRoomRefHandle,
						false,                           // bool a_forcePersist,
						true                             // bool a_arg11,
					);

					if (objHandle) {
						if (auto ref = objHandle.get()) {
							for (auto& item : itemList) {
								RE::ExtraDataList* extraDataList = nullptr;  // ??

								ref->AddObjectToContainer(
									item->GetForm()->As<RE::TESBoundObject>(),  // RE::TESBoundObject* a_item,
									extraDataList,                              // RE::ExtraDataList* a_extraData,
									1,                                          // std::uint32_t a_count,
									player                                      // RE::TESObjectREFR* a_fromRefr,
								);
							}
						}
					}
				}
			}
		}

		if (ImGui::GradientButton("Reset Search", ImVec2(button_width, 0.0f))) {
			ImFormatString(inputBuffer, IM_ARRAYSIZE(inputBuffer), "");
			ImFormatString(modSearchBuffer, IM_ARRAYSIZE(modSearchBuffer), "");
			selectedMod = "All Mods";
			primaryFilter = RE::FormType::None;
			secondaryFilter = "All";
			Refresh();
		}

		bool plugin_view_visible = showKitsForPlugin && selectedMod != _T("All Mods");
		if (ImGui::GradientSelectableEX(_T("KIT_SHOW_IN_PLUGIN_VIEW"), plugin_view_visible, ImVec2(button_width, button_height))) {
			showKitsForPlugin = !showKitsForPlugin;
		}

		ImGui::PopStyleVar(2);  // End of SelectableTextAlign and ButtonTextAlign

		ImGui::Spacing();
		// ImGui::SubCategoryHeader(_T("Preview"), ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::SubCategoryHeader(_T("Preview"));

		ShowItemPreview<ItemData>(itemPreview);

		ImGui::EndChild();
	}
}