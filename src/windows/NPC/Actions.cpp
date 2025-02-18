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

		ImGui::SubCategoryHeader(_T("GENERAL_DOUBLE_CLICK_BEHAVIOR"));

		// Click To Place Toggle
		if (ImGui::GradientSelectableEX(_TICON(ICON_LC_MAP_PIN_PLUS, "GENERAL_CLICK_TO_PLACE"), b_ClickToPlace, ImVec2(button_width, button_height))) {
			b_ClickToPlace = true;
		}

		if (b_ClickToPlace) {
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			// Add Count Input.
			ImGui::SetNextItemWidth(button_width);
			ImGui::InputInt("##NPCPlaceCount", &clickToPlaceCount, 1, 100, ImGuiInputTextFlags_CharsDecimal);
		}

		ImGui::Spacing();
		ImGui::SubCategoryHeader(_T("Actions"));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(a_style.secondaryButton.x, a_style.secondaryButton.y, a_style.secondaryButton.z, a_style.secondaryButton.w));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(a_style.secondaryButtonActive.x, a_style.secondaryButtonActive.y, a_style.secondaryButtonActive.z, a_style.secondaryButtonActive.w));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(a_style.secondaryButtonHovered.x, a_style.secondaryButtonHovered.y, a_style.secondaryButtonHovered.z, a_style.secondaryButtonHovered.w));

		if (ImGui::GradientButton(_T("NPC_PLACE_SELECTED"), ImVec2(button_width, 0))) {
			this->tableView.PlaceSelectionOnGround(clickToPlaceCount);
		}

		ImGui::PopStyleColor(3);  // End of Green Buttons

		if (ImGui::GradientButton(_T("NPC_UPDATE_REFERENCES"), ImVec2(button_width, 0))) {
			Data::GetSingleton()->CacheNPCRefIds();
			this->tableView.Refresh();
		}

		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip(_T("TOOLTIP_NPC_UPDATE"));
		}

		const auto& selectedNPC = this->GetTableView().GetItemPreview();

		if (selectedNPC != nullptr) {
			if (selectedNPC->refID != 0) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(a_style.button.x, a_style.button.y, a_style.button.z, a_style.button.w));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(a_style.buttonHovered.x, a_style.buttonHovered.y, a_style.buttonHovered.z, a_style.buttonHovered.w));
			} else {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(a_style.button.x, a_style.button.y, a_style.button.z, a_style.button.w - 0.35f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(a_style.button.x, a_style.button.y, a_style.button.z, a_style.button.w - 0.35f));
			}

			if (ImGui::GradientButton(_T("NPC_GOTO_REFERENCE"), ImVec2(button_width, 0))) {
				if (selectedNPC->refID != 0) {
					if (auto playerREF = RE::PlayerCharacter::GetSingleton()->AsReference()) {
						if (auto ref = RE::TESForm::LookupByID<RE::TESObjectREFR>(selectedNPC->refID)) {
							playerREF->MoveTo(ref);
							Menu::GetSingleton()->Close();
						}
					}
				}
			}

			if (ImGui::GradientButton(_T("NPC_BRING_REFERENCE"), ImVec2(button_width, 0))) {
				if (selectedNPC->refID != 0) {
					if (auto playerREF = RE::PlayerCharacter::GetSingleton()->AsReference()) {
						if (auto ref = RE::TESForm::LookupByID<RE::TESObjectREFR>(selectedNPC->refID)) {
							ref->MoveTo(playerREF);
							Menu::GetSingleton()->Close();
						}
					}
				}
			}

			ImGui::PopStyleColor(2);
		}

		if (selectedNPC == nullptr) {
			ImGui::PopStyleVar(2);  // End of SelectableTextAlign and ButtonTextAlign
			return;
		}

		ImGui::Spacing();
		ImGui::SubCategoryHeader(_T("Preview"));

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

		NPCData* npc = selectedNPC.get();

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
			if (ImGui::TreeNode(_TICON(Utils::IconMap["INFO"], "Info"))) {
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

				InlineText(_TICONM(Utils::GetFormTypeIcon(RE::FormType::Class), "Class", ":"), npc->GetClass().data());
				InlineText(_TICONM(Utils::GetFormTypeIcon(RE::FormType::Race), "Race", ":"), npc->GetRace().data());
				InlineText(_TICONM(Utils::IconMap["GENDER"], "Gender", ":"), npc->GetGender().data());
				InlineBar(_TICONM(Utils::IconMap["LEVEL"], "Level", ":"), npc->GetLevel(), 100.0f);

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

				InlineBar(_TICONM(Utils::IconMap["HEALTH"], "Health", ":"), npc->GetHealth(), 100.0f);
				InlineBar(_TICONM(Utils::IconMap["MAGICKA"], "Magicka", ":"), npc->GetMagicka(), 100.0f);
				InlineBar(_TICONM(Utils::IconMap["STAMINA"], "Stamina", ":"), npc->GetStamina(), 100.0f);

				// Load Order Info Pane
				// See ItemPreview.h for other implementation.
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(_T("Load Order")));
				ImGui::Text(_T("Load Order"));
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				const auto sourceFiles = npc->GetForm()->sourceFiles.array;

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

				ImGui::TreePop();
			}

			// Faction
			if (ImGui::TreeNode(_TICON(Utils::GetFormTypeIcon(RE::FormType::Faction), "Faction"))) {
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

			if (ImGui::TreeNode(_TICON(Utils::IconMap["SKILL"], "NPC_SKILLS"))) {
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

				RE::TESNPC::Skills skills = selectedNPC->GetSkills();

				const auto skillNames = Utils::GetSkillNames();
				for (int i = 0; i < 18; i++) {
					const auto skillName = skillNames[i];
					const auto skillLevel = skills.values[i];

					InlineBar(skillName.c_str(), skillLevel, 100.0f);
				}

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode(_TICON(Utils::IconMap["SPELL"], "NPC_SPELLS"))) {
				auto spellData = npc->GetTESNPC()->GetSpellList();

				if (spellData != nullptr && spellData->numSpells > 0) {
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
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

							InlineText(_TICONM(Utils::IconMap["SKILL"], "NPC_CAST_TYPE", ":"), castType);
							InlineText(_TICONM(Utils::IconMap["SKILL"], "NPC_SPELL_TYPE", ":"), spellType);
							InlineText(_TICONM(Utils::IconMap["SPELL"], "NPC_DELIVERY_TYPE", ":"), delType);
							InlineText(_TICONM(Utils::IconMap["MAGICKA"], "Cost", ":"), std::format("{:.0f}", cost).c_str());
							InlineText((std::string(_TICON(Utils::IconMap["MAGICKA"], "Cost")) + "%%" + ":").c_str(), costPercentStr.c_str());  // https://github.com/ocornut/imgui/issues/7679

							ImGui::TreePop();
						}
					}
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				}

				ImGui::TreePop();
			}
			ImGui::EndChild();
		}

		ImGui::PopStyleVar(2);  // End of SelectableTextAlign and ButtonTextAlign
	}
}