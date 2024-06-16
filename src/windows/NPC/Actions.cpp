#include "Console.h"
#include "Menu.h"
#include "Utils/Util.h"
#include "Window.h"

namespace ModExplorerMenu
{
	void NPCWindow::ShowActions(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_style;
		(void)a_config;

		ImVec2 barSize = ImVec2(100.0f, ImGui::GetFontSize());
		float popWidth = ImGui::GetContentRegionAvail().x + 10.0f;

		const float button_height = ImGui::GetFontSize() * 1.5f;
		const float button_width = ImGui::GetContentRegionAvail().x;

		if (ImGui::Button("Riverwood", ImVec2(button_width, button_height))) {
			Console::MoveToREFR("00013485");
			Console::StartProcessThread();
		}

		switch (currentState) {
		case showAll:
			if (ImGui::Button("Show Nearby NPCs", ImVec2(button_width, button_height))) {
				PopulateListWithLocals();
			}
			if (ImGui::Button("Show Spawned NPCs", ImVec2(button_width, button_height))) {
				PopulateListWithSpawned();
			}
			break;
		case showLocal:
		case showSpawned:
			if (ImGui::Button("Show All NPCs", ImVec2(button_width, button_height))) {
				ApplyFilters();
			}
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		if (selectedNPC == nullptr) {
			return;
		}

		// TODO: Stop being lazy and move these lambda's to a class or function body.
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

		if (ImGui::Button("Place as New", ImVec2(button_width, button_height))) {
			Console::PlaceAtMe(selectedNPC->GetFormID(), 1);
		}

		// if (showLocalsOnly) {
		// 	ImGui::Checkbox("Apply to All Nearby NPCS", &applyActionsToAll);

		// 	const auto refID = std::format("{:08x}", selectedNPC->refID);
		// 	if (ImGui::Button("Bring", ImVec2(button_width, button_height))) {
		// 		if (applyActionsToAll) {
		// 			for (auto& local : npcList) {
		// 				if (local->refID == selectedNPC->refID) {
		// 					continue;
		// 				}

		// 				//const auto localRefID = std::format("{:08x}", local->refID);
		// 				//ConsoleCommand::MoveToPlayer(localRefID);
		// 			}
		// 		} else {
		// 			//ConsoleCommand::MoveToPlayer(refID);
		// 		}
		// 	}

		// 	if (ImGui::Button("Goto", ImVec2(button_width, button_height))) {
		// 		//ConsoleCommand::MoveTo(refID);
		// 	}

		// 	if (ImGui::Button("Kill", ImVec2(button_width, button_height))) {
		// 		if (applyActionsToAll) {
		// 			for (auto& local : npcList) {
		// 				if (local->refID == selectedNPC->refID) {
		// 					continue;
		// 				}

		// 				//const auto localRefID = std::format("{:08x}", local->refID);
		// 				//ConsoleCommand::Kill(localRefID);
		// 			}
		// 		} else {
		// 			//ConsoleCommand::Kill(refID);
		// 		}
		// 	}

		// 	if (ImGui::Button("Resurrect", ImVec2(button_width, button_height))) {
		// 		if (applyActionsToAll) {
		// 			for (auto& local : npcList) {
		// 				if (local->refID == selectedNPC->refID) {
		// 					continue;
		// 				}

		// 				//const auto localRefID = std::format("{:08x}", local->refID);
		// 				//ConsoleCommand::Resurrect(localRefID);
		// 			}
		// 		} else {
		// 			//ConsoleCommand::Resurrect(refID);
		// 		}
		// 	}

		// 	if (ImGui::Button("Unequip All", ImVec2(button_width, button_height))) {
		// 		if (applyActionsToAll) {
		// 			for (auto& local : npcList) {
		// 				if (local->refID == selectedNPC->refID) {
		// 					continue;
		// 				}

		// 				//const auto localRefID = std::format("{:08x}", local->refID);
		// 				//ConsoleCommand::UnEquipAll(localRefID);
		// 			}
		// 		} else {
		// 			//ConsoleCommand::UnEquipAll(refID);
		// 		}
		// 	}

		// 	if (ImGui::Button("Toggle Freeze", ImVec2(button_width, button_height))) {
		// 		if (applyActionsToAll) {
		// 			for (auto& local : npcList) {
		// 				if (local->refID == selectedNPC->refID) {
		// 					continue;
		// 				}

		// 				//const auto localRefID = std::format("{:08x}", local->refID);
		// 				//ConsoleCommand::ToggleFreeze(localRefID);
		// 			}
		// 		} else {
		// 			//ConsoleCommand::ToggleFreeze(refID);
		// 		}
		// 	}

		// 	if (ImGui::Button("Save Reference", ImVec2(button_width, button_height))) {
		// 		//TODO: Implement saved references of unique actors (?)
		// 		// https://elderscrolls.fandom.com/wiki/Console_Commands_(Skyrim)/Characters
		// 	}
		// }

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		auto* npc = selectedNPC->TESForm->As<RE::TESNPC>();

		if (npc == nullptr) {
			return;
		}

		if (ImGui::CollapsingHeader("NPC Skills")) {
			const auto skills = selectedNPC->GetSkills();
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
	}
}