#include "include/C/Console.h"
#include "include/M/Menu.h"
#include "include/N/NPC.h"

namespace Modex
{
	void NPCWindow::ShowActions()
	{
		auto a_style = Settings::GetSingleton()->GetStyle();

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
			ImGui::InputInt("##NPCPlaceCount", &clickToPlaceCount, 1, 100, ImGuiInputTextFlags_CharsDecimal);
		}

		ImGui::PopStyleColor(3);
		ImGui::SeparatorText(_TFM("Actions", ":"));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(a_style.secondaryButton.x, a_style.secondaryButton.y, a_style.secondaryButton.z, a_style.secondaryButton.w));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(a_style.secondaryButtonActive.x, a_style.secondaryButtonActive.y, a_style.secondaryButtonActive.z, a_style.secondaryButtonActive.w));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(a_style.secondaryButtonHovered.x, a_style.secondaryButtonHovered.y, a_style.secondaryButtonHovered.z, a_style.secondaryButtonHovered.w));

		if (ImGui::m_Button(_T("NPC_PLACE_SELECTED"), a_style, ImVec2(button_width, 0))) {
			if (selectedNPC != nullptr) {
				Console::PlaceAtMe(selectedNPC->GetFormID(), 1);
				Console::StartProcessThread();
			}
		}

		if (ImGui::m_Button(_T("GENERAL_PLACE_ALL"), a_style, ImVec2(button_width, 0))) {
			for (auto& npc : npcList) {
				Console::PlaceAtMe(npc->GetFormID(), 1);
			}

			Console::StartProcessThread();
		}

		ImGui::PopStyleColor(3);  // End of Green Buttons

		if (ImGui::m_Button(_T("GENERAL_GOTO_FAVORITE"), a_style, ImVec2(button_width, 0))) {
			this->selectedMod = "Favorite";
			ApplyFilters();
		}
		if (ImGui::m_Button(_T("NPC_UPDATE_REFERENCES"), a_style, ImVec2(button_width, 0))) {
			Data::GetSingleton()->CacheNPCRefIds();
		}

		if (selectedNPC != nullptr || hoveredNPC != nullptr) {
			if ((selectedNPC != nullptr && hoveredNPC == nullptr && selectedNPC->refID != 0) ||
				(selectedNPC != nullptr && hoveredNPC != nullptr && hoveredNPC->refID != 0) ||
				(hoveredNPC != nullptr && selectedNPC == nullptr && hoveredNPC->refID != 0)) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(a_style.button.x, a_style.button.y, a_style.button.z, a_style.button.w));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(a_style.buttonHovered.x, a_style.buttonHovered.y, a_style.buttonHovered.z, a_style.buttonHovered.w));
			} else {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(a_style.button.x, a_style.button.y, a_style.button.z, a_style.button.w - 0.35f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(a_style.button.x, a_style.button.y, a_style.button.z, a_style.button.w - 0.35f));
			}

			if (ImGui::m_Button(_T("NPC_GOTO_REFERENCE"), a_style, ImVec2(button_width, 0))) {
				if (selectedNPC != nullptr) {
					if (selectedNPC->refID != 0) {
						if (auto playerREF = RE::PlayerCharacter::GetSingleton()->AsReference()) {
							if (auto ref = RE::TESForm::LookupByID<RE::TESObjectREFR>(selectedNPC->refID)) {
								playerREF->MoveTo(ref);
								Menu::GetSingleton()->Close();
							}
						}
					}
				}
			}

			if (ImGui::m_Button(_T("NPC_BRING_REFERENCE"), a_style, ImVec2(button_width, 0))) {
				if (selectedNPC != nullptr) {
					if (selectedNPC->refID != 0) {
						if (auto playerREF = RE::PlayerCharacter::GetSingleton()->AsReference()) {
							if (auto ref = RE::TESForm::LookupByID<RE::TESObjectREFR>(selectedNPC->refID)) {
								ref->MoveTo(playerREF);
								Menu::GetSingleton()->Close();
							}
						}
					}
				}
			}

			ImGui::PopStyleColor(2);
		}

		if (selectedNPC == nullptr && hoveredNPC == nullptr) {
			ImGui::PopStyleVar(2);  // End of SelectableTextAlign and ButtonTextAlign
			return;
		}

		ImGui::SeparatorText(_TFM("Info", ":"));

		ImVec2 barSize = ImVec2(100.0f, ImGui::GetFontSize());

		constexpr auto ProgressColor = [](const double value, const float max_value) -> ImVec4 {
			const float dampen = 0.7f;
			const float ratio = (float)value / max_value;
			const float r = 1.0f - ratio * dampen;
			const float g = ratio * dampen;
			return ImVec4(r, g, 0.0f, 0.75f);
		};

		const auto InlineBar = [barSize, ProgressColor](const char* label, const float value, const float max_value) {
			ImGui::Text(label);
			ImGui::SameLine(ImGui::GetContentRegionMax().x - barSize.x);
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ProgressColor(value, max_value));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);  // tight fit
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			std::string sv = std::format("{:.0f}", value);
			float curr = static_cast<float>(value);
			ImGui::ProgressBar(curr / max_value, barSize, sv.c_str());
			ImGui::PopStyleColor(1);
			ImGui::PopStyleVar(2);
		};

		const auto InlineBarBool = [barSize](const char* label, const bool value) {
			value ? ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.5f, 0.3f, 0.55f)) :
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 0.35f));

			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);  // tight fit
			ImGui::Selectable(label, value, ImGuiSelectableFlags_Disabled, ImVec2(ImGui::GetContentRegionMax().x - 10.0f, 0));
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		};

		const auto InlineText = [](const char* label, const char* text) {
			ImGui::Text(label);
			ImGui::SameLine(ImGui::GetContentRegionMax().x - ImGui::CalcTextSize(text).x);
			ImGui::Text(text);
		};

		NPCData* npc = nullptr;

		if (hoveredNPC != nullptr) {
			npc = hoveredNPC;
		} else if (selectedNPC != nullptr) {
			npc = selectedNPC;
		}

		if (npc == nullptr) {
			ImGui::PopStyleVar(2);  // End of SelectableTextAlign and ButtonTextAlign
			return;
		}

		// Name Bar
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		const auto cursor = ImGui::GetCursorScreenPos();
		const auto size = ImGui::GetContentRegionAvail();

		auto isEssential = npc->GetTESNPC()->IsEssential();
		auto isUnique = npc->GetTESNPC()->IsUnique();

		// Color Name Bar based on NPC Essential/Unique state.
		auto color = isEssential ? ImVec4(0.40f, 0.12f, 0.45f, 0.65f) :
		             isUnique    ? ImVec4(0.92f, 0.65f, 0.24f, 0.65f) :
		                           ImGui::GetStyleColorVec4(ImGuiCol_Border);

		drawList->AddRectFilled(cursor, ImVec2(cursor.x + size.x, cursor.y + ImGui::GetFontSize() * 2.5f), ImGui::ColorConvertFloat4ToU32(ImVec4(color.x, color.y, color.z, color.w - 0.32f)));
		drawList->AddRect(cursor, ImVec2(cursor.x + size.x, cursor.y + ImGui::GetFontSize() * 2.5f), ImGui::ColorConvertFloat4ToU32(color), 0.0f, 0, 2.0f);

		// this is showing wrong
		const auto name = npc->GetName();
		ImGui::NewLine();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(name.c_str()));
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetFontSize() / 2);
		ImGui::Text(name.c_str());
		ImGui::NewLine();
		ImGui::PopStyleVar();

		auto constexpr flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration |
		                       ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;

		if (ImGui::BeginChild("##NPCInfoScroll", ImVec2(ImGui::GetContentRegionAvail().x, 0), false, flags)) {
			if (ImGui::TreeNode(_T("Info"))) {
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

				InlineText(_TFM("Class", ":"), npc->GetClass().data());
				InlineText(_TFM("Race", ":"), npc->GetRace().data());
				InlineText(_TFM("Gender", ":"), npc->GetGender().data());
				InlineBar(_TFM("Level", ":"), npc->GetLevel(), 100.0f);

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

				InlineBar(_TFM("Health", ":"), npc->GetHealth(), 100.0f);
				InlineBar(_TFM("Magicka", ":"), npc->GetMagicka(), 100.0f);
				InlineBar(_TFM("Stamina", ":"), npc->GetStamina(), 100.0f);

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				ImGui::TreePop();
			}

			// Faction
			if (ImGui::TreeNode(_T("Faction"))) {
				auto factions = npc->GetFactions();

				if (factions.size() > 0) {
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				}

				for (auto& faction : factions) {
					const auto factionRank = faction.rank;
					std::string factionName = faction.faction->GetFullName();

					if (factionName.empty()) {
						continue;
					}

					ImGui::Text(factionName.c_str());
					ImGui::SameLine(ImGui::GetContentRegionMax().x - ImGui::CalcTextSize(std::to_string(factionRank).c_str()).x);
					ImGui::Text(std::to_string(factionRank).c_str());
				}

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode(_T("NPC_SKILLS"))) {
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

				RE::TESNPC::Skills skills;

				if (hoveredNPC != nullptr) {
					skills = hoveredNPC->GetSkills();
				} else if (selectedNPC != nullptr) {
					skills = selectedNPC->GetSkills();
				}

				const auto skillNames = Utils::GetSkillNames();
				for (int i = 0; i < 18; i++) {
					const auto skillName = skillNames[i];
					const auto skillLevel = skills.values[i];

					InlineBar(skillName.c_str(), skillLevel, 100.0f);
				}

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode(_T("NPC_SPELLS"))) {
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				auto spellData = npc->GetTESNPC()->GetSpellList();

				if (spellData != nullptr) {
					for (uint32_t i = 0; i < spellData->numSpells; i++) {
						if (spellData->spells[i] == nullptr)
							continue;

						const auto* spell = spellData->spells[i];

						if (spell == nullptr) {
							continue;
						}

						const auto spellName = spell->GetFullName();

						// Weird bug here where the spell name is empty. Patched with ##.
						if (ImGui::TreeNode((std::string(spellName) + "##SpellName").c_str())) {
							auto castType = Utils::GetCastingType(spell->data.castingType);
							auto spellType = Utils::GetSpellType(spell->data.spellType);
							auto delType = Utils::GetDeliveryType(spell->data.delivery);
							auto cost = spell->CalculateMagickaCost(npc->GetForm()->As<RE::Actor>());

							float costPercent = cost / npc->GetMagicka() * 100.0f;
							std::string costPercentStr = std::format("{:.0f}", costPercent) + std::string("%%");

							InlineText(_TFM("NPC_CAST_TYPE", ":"), castType);
							InlineText(_TFM("NPC_SPELL_TYPE", ":"), spellType);
							InlineText(_TFM("NPC_DELIVERY_TYPE", ":"), delType);
							InlineText(_TFM("Cost", ":"), std::format("{:.0f}", cost).c_str());
							InlineText((std::string(_T("Cost")) + "%%" + ":").c_str(), costPercentStr.c_str());  // https://github.com/ocornut/imgui/issues/7679

							ImGui::TreePop();
						}
					}
				}

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				ImGui::TreePop();
			}
			ImGui::EndChild();
		}

		ImGui::PopStyleVar(2);  // End of SelectableTextAlign and ButtonTextAlign
	}
}