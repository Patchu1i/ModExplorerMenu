#include "Console.h"
#include "Utils/Util.h"
#include "Windows/Cheat/Cheat.h"

namespace ModExplorerMenu
{
	void CheatWindow::ShowSkills()
	{
		auto* actor = RE::PlayerCharacter::GetSingleton()->AsActorValueOwner();
		auto* player = RE::PlayerCharacter::GetSingleton();

		if (actor == nullptr) {
			return;
		}

		if (player == nullptr) {
			return;
		}

		ImGui::Text("Skills:");
		ImGui::NewLine();

		if (ImGui::BeginCombo("##SkillSelection", iSelectedSkill.c_str())) {
			for (int i = 0; i < RE::PlayerCharacter::PlayerSkills::Data::Skill::kTotal; i++) {
				bool isSelected = iSelectedSkill == skillNames[i].first;
				if (ImGui::Selectable(skillNames[i].first.c_str(), isSelected)) {
					iSelectedSkill = skillNames[i].first;
					iSelectedSkillValue = skillNames[i].second;
					iSelectedSkillSlider = actor->GetActorValue(iSelectedSkillValue);
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (iSelectedSkill != "None") {
			auto skillLevel = actor->GetActorValue(iSelectedSkillValue);
			auto skillName = &iSelectedSkill;
			auto skillLevelMult = skillXPMultiplier[iSelectedSkillValue];

			ImGui::Text("Skill: %s", skillName->c_str());
			ImGui::Text("Level: %f", skillLevel);
			ImGui::Text("XP Per Level: %f", skillLevelMult);

			ImGui::SliderFloat("##SkillLevel", &iSelectedSkillSlider, 1.0f, 100.0f, "Level: %.0f");
			ImGui::SameLine();
			if (ImGui::Button("Set Level")) {
				if (iSelectedSkillSlider > skillLevel) {
					float xpCurrentLevel = 0.0f;
					for (float i = 1; i < skillLevel; i++) {
						xpCurrentLevel += skillXPMultiplier[iSelectedSkillValue] * pow(i, 1.95f) + skillXPOffsets[iSelectedSkillValue];
					}

					float xpTargetLevelCumulative = 0.0f;
					for (float i = 1; i <= iSelectedSkillSlider; i++) {
						xpTargetLevelCumulative += skillXPMultiplier[iSelectedSkillValue] * pow(i, 1.95f) + skillXPOffsets[iSelectedSkillValue];
					}

					auto xpToLevel = xpTargetLevelCumulative - xpCurrentLevel;
					logger::info("XP to level: {}", xpToLevel);

					Console::SendConsoleCommand("player.advskill " + iSelectedSkill + " " + std::to_string(xpToLevel));
				} else if (iSelectedSkillSlider < skillLevel) {
					Console::SendConsoleCommand("player.setav " + iSelectedSkill + " " + std::to_string(iSelectedSkillSlider));
				} else if (iSelectedSkillSlider == skillLevel) {
					return;
				}
			}
		}
	}

}