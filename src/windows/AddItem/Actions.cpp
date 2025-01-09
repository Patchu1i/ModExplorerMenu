#include "include/A/AddItem.h"
#include "include/C/Console.h"
#include "include/I/ItemPreview.h"

namespace Modex
{
	void AddItemWindow::ShowActions(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_config;

		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

		ImGui::BeginChild("##AddItemWindow::Actions", ImVec2(ImGui::GetContentRegionAvail()), ImGuiChildFlags_AlwaysUseWindowPadding);

		const float button_height = ImGui::GetFontSize() * 1.5f;
		const float button_width = ImGui::GetContentRegionAvail().x;

		ImGui::SeparatorText(_TFM("Behavior", ":"));

		ImGui::PushStyleColor(ImGuiCol_Header, a_style.button);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, a_style.buttonActive);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, a_style.buttonHovered);

		// Add To Inventory Toggle.
		if (ImGui::m_Selectable(_TICON(ICON_RPG_HAND, "AIM_ADD"), b_AddToInventory, a_style, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_PlaceOnGround = false;
			b_AddToFavorites = false;
		};
		ImGui::SetDelayedTooltip(_T("AIM_ADD_HELP"));

		// Place On Ground Toggle.
		if (ImGui::m_Selectable(_TICON(ICON_RPG_GRASS, "AIM_PLACE"), b_PlaceOnGround, a_style, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_AddToInventory = false;
			b_AddToFavorites = false;
		}
		ImGui::SetDelayedTooltip(_T("AIM_PLACE_HELP"));

		// Add To Favorites Toggle.
		if (ImGui::m_Selectable(_TICON(ICON_RPG_HEART, "AIM_FAVORITE"), b_AddToFavorites, a_style, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_AddToInventory = false;
			b_PlaceOnGround = false;
		}
		ImGui::SetDelayedTooltip(_T("AIM_FAVORITE_HELP"));

		if (b_AddToInventory || b_PlaceOnGround) {
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			// Add Count Input.
			ImGui::SetNextItemWidth(button_width);
			ImGui::InputInt("##AddItemCount", &clickToAddCount, 1, 100, ImGuiInputTextFlags_CharsDecimal);
		}

		ImGui::PopStyleColor(3);
		ImGui::SeparatorText(_TFM("Shortcuts", ":"));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(a_style.secondaryButton.x, a_style.secondaryButton.y, a_style.secondaryButton.z, a_style.secondaryButton.w));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(a_style.secondaryButtonActive.x, a_style.secondaryButtonActive.y, a_style.secondaryButtonActive.z, a_style.secondaryButtonActive.w));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(a_style.secondaryButtonHovered.x, a_style.secondaryButtonHovered.y, a_style.secondaryButtonHovered.z, a_style.secondaryButtonHovered.w));

		// Add All From Table Shortcut.
		if (ImGui::m_Button(_T("AIM_ADD_ALL"), a_style, ImVec2(button_width, 0))) {
			for (auto& item : itemList) {
				Console::AddItem(item->GetFormID());
			}

			Console::StartProcessThread();
		}
		ImGui::SetDelayedTooltip(_T("AIM_ADD_ALL_HELP"));

		// Place All from Table Shortcut.
		if (ImGui::m_Button(_T("GENERAL_PLACE_ALL"), a_style, ImVec2(button_width, 0))) {
			if (itemList.size() > 30) {
				ImGui::OpenPopup(_T("AIM_LARGE_QUERY"));
			} else {
				for (auto& item : itemList) {
					Console::PlaceAtMe(item->GetFormID());
				}
			}

			Console::StartProcessThread();
		}
		ImGui::SetDelayedTooltip(_T("AIM_PLACE_ALL_HELP"));

		ImGui::PopStyleColor(3);  // End of Secondary Buttons

		ImGui::ShowWarningPopup(_T("AIM_LARGE_QUERY"), [&]() {
			for (auto& item : itemList) {
				Console::PlaceAtMe(item->GetFormID());
			}
		});

		// Spawn Container with Table contents
		if (ImGui::m_Button(_T("AIM_ADD_TO_CONTAINER"), a_style, ImVec2(button_width, 0))) {
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
									item->TESForm->As<RE::TESBoundObject>(),  // RE::TESBoundObject* a_item,
									extraDataList,                            // RE::ExtraDataList* a_extraData,
									1,                                        // std::uint32_t a_count,
									player                                    // RE::TESObjectREFR* a_fromRefr,
								);
							}
						}
					}
				}
			}
		}
		ImGui::SetDelayedTooltip(_T("AIM_ADD_TO_CONTAINER_HELP"));

		// Show Favorites
		if (ImGui::m_Button(_T("GENERAL_GOTO_FAVORITE"), a_style, ImVec2(button_width, 0))) {
			selectedMod = "Favorite";
			ApplyFilters();
		}

		ImGui::PopStyleVar(2);  // End of SelectableTextAlign and ButtonTextAlign

		ImGui::SeparatorText(_TFM("Preview", ":"));
		ShowItemPreview<Item>(itemPreview);

		ImGui::EndChild();
	}
}