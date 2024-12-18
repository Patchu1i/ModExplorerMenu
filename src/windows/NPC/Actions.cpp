#include "Console.h"
#include "Data.h"
#include "Menu.h"
#include "NPC.h"
#include "Utils/Util.h"

namespace ModExplorerMenu
{
	void NPCWindow::ShowActions(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_config;

		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

		const float button_height = ImGui::GetFontSize() * 1.5f;
		const float button_width = ImGui::GetContentRegionAvail().x;

		ImGui::SeparatorText("Shortcuts:");

		ImGui::PushFont(a_style.font.medium);
		ImGui::PushStyleColor(ImGuiCol_Header, a_style.button);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, a_style.buttonActive);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, a_style.buttonHovered);

		if (ImGui::Selectable(ICON_RPG_MULTI_NPC " Click to Select", &b_ClickToSelect, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_ClickToFavorite = false;
		}

		if (ImGui::Selectable(ICON_RPG_SPAWNED_NPC " Click to Favorite", &b_ClickToFavorite, ImGuiSelectableFlags_SelectOnClick, ImVec2(button_width, button_height))) {
			b_ClickToSelect = false;
		}

		ImGui::PopStyleColor(3);

		ImGui::SeparatorText("Actions:");

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(a_style.button.x, a_style.button.y + 0.3f, a_style.button.y, a_style.button.w));

		if (ImGui::Button("Place Selected NPC", ImVec2(button_width, button_height))) {
			if (selectedNPC != nullptr) {
				Console::PlaceAtMe(selectedNPC->GetFormID(), 1);
				Console::StartProcessThread();
			}
		}

		if (ImGui::Button("Place All From Table", ImVec2(button_width, button_height))) {
			for (auto& npc : npcList) {
				Console::PlaceAtMe(npc->GetFormID(), 1);
			}

			Console::StartProcessThread();
		}

		ImGui::PopStyleColor(1);

		if (ImGui::Button("Goto Favorite", ImVec2(button_width, button_height))) {
			selectedMod = ICON_RPG_HEART " Favorite";
			ApplyFilters();
		}

		if (ImGui::Button("Print Refs", ImVec2(button_width, button_height))) {
			Data::GetSingleton()->CacheNPCRefIds();
		}

		if (selectedNPC == nullptr && hoveredNPC == nullptr) {
			ImGui::PopFont();
			ImGui::PopStyleVar(2);
			return;
		}

		ImGui::SeparatorText("Info:");

		ImVec2 barSize = ImVec2(100.0f, ImGui::GetFontSize());
		float popWidth = ImGui::GetContentRegionAvail().x + 10.0f;

		constexpr auto ProgressColor = [](const double value, const float max_value) -> ImVec4 {
			const float dampen = 0.7f;
			const float ratio = (float)value / max_value;
			const float r = 1.0f - ratio * dampen;
			const float g = ratio * dampen;
			return ImVec4(r, g, 0.0f, 1.0f);
		};

		const auto InlineBar = [popWidth, barSize, ProgressColor](const char* label, const float value, const float max_value) {
			ImGui::Text(label);
			ImGui::SameLine(popWidth - barSize.x);
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ProgressColor(value, max_value));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);  // tight fit
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			std::string sv = std::format("{:.0f}", value);
			float curr = static_cast<float>(value);
			ImGui::ProgressBar(curr / max_value, barSize, sv.c_str());
			ImGui::PopStyleColor(1);
			ImGui::PopStyleVar(2);
		};

		const auto InlineText = [popWidth](const char* label, const char* text) {
			ImGui::Text(label);
			ImGui::SameLine(popWidth - ImGui::CalcTextSize(text).x);
			ImGui::Text(text);
		};

		RE::TESNPC* npc = nullptr;

		if (hoveredNPC != nullptr) {
			npc = hoveredNPC->TESForm->As<RE::TESNPC>();
		} else if (selectedNPC != nullptr) {
			npc = selectedNPC->TESForm->As<RE::TESNPC>();
		}

		if (npc == nullptr) {
			ImGui::PopFont();
			ImGui::PopStyleVar(2);
			return;
		}

		// Name Bar
		const auto name = npc->GetName();

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		const auto cursor = ImGui::GetCursorScreenPos();
		const auto size = ImGui::GetContentRegionAvail();
		const auto color = ImGui::GetStyleColorVec4(ImGuiCol_Border);
		draw_list->AddRectFilled(cursor, ImVec2(cursor.x + size.x, cursor.y + ImGui::GetFontSize() * 3.5f), ImGui::ColorConvertFloat4ToU32(ImVec4(0.15f, 0.15f, 0.15f, 0.5f)));
		draw_list->AddRect(cursor, ImVec2(cursor.x + size.x, cursor.y + ImGui::GetFontSize() * 3.5f), ImGui::ColorConvertFloat4ToU32(color));

		ImGui::NewLine();
		ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(name));
		ImGui::Text(name);
		ImGui::NewLine();

		if (ImGui::CollapsingHeader("NPC Skills", ImGuiTreeNodeFlags_DefaultOpen)) {
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
		}

		if (ImGui::CollapsingHeader("NPC Spells", ImGuiTreeNodeFlags_DefaultOpen)) {
			auto* spellData = npc->GetSpellList();

			if (spellData == nullptr) {
				return;
			}

			for (uint32_t i = 0; i < spellData->numSpells; i++) {
				if (spellData->spells[i] == nullptr)
					continue;

				const auto* spell = spellData->spells[i];
				const auto spellName = spell->GetFullName();

				if (ImGui::TreeNode(spellName)) {
					auto castType = Utils::GetCastingType(spell->data.castingType);
					auto spellType = Utils::GetSpellType(spell->data.spellType);
					auto delType = Utils::GetDeliveryType(spell->data.delivery);
					auto cost = spell->CalculateMagickaCost(npc->As<RE::Actor>());

					float costPercent = cost / npc->GetBaseActorValue(RE::ActorValue::kMagicka) * 100.0f;
					std::string costPercentStr = std::format("{:.0f}", costPercent) + std::string("%%");

					InlineText("Cast Type:", castType);
					InlineText("Spell Type:", spellType);
					InlineText("Delivery Type:", delType);
					InlineText("Cost:", std::format("{:.0f}", cost).c_str());
					InlineText("Cost %%:", costPercentStr.c_str());  // https://github.com/ocornut/imgui/issues/7679

					ImGui::TreePop();
				}
			}
		}

		ImGui::PopFont();
		ImGui::PopStyleVar(2);
	}
}