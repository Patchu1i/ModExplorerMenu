#include "Data.h"
#include "Menu.h"
#include "Quest.h"
#include "Utils/Util.h"

namespace Modex
{
	void QuestWindow::ShowActions(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_style;
		(void)a_config;

		if (selectedQuest == nullptr) {
			return;
		}

		ImGui::Text("Selected Quest: %s", selectedQuest->TESForm->GetName());

		auto TESQuest = selectedQuest->TESForm->As<RE::TESQuest>();

		RE::BSSimpleList objectives = TESQuest->objectives;

		for (auto& objective : objectives) {
			auto objectiveText = objective->displayText;
			auto objectiveOwner = objective->ownerQuest;
			auto objectiveTargets = objective->targets;
			// auto objectiveFlags = objective->flags;

			ImGui::Text("Objective Text: %s", objectiveText.c_str());
			ImGui::Text("Objective Type: %d", objectiveTargets);

			if (!objectiveOwner->objectives.empty()) {
				for (auto& subObjective : objectiveOwner->objectives) {
					auto subObjectiveText = subObjective->displayText;
					// auto subObjectiveOwner = subObjective->ownerQuest;
					auto subObjectiveTargets = subObjective->targets;
					// auto subObjectiveFlags = subObjective->flags;

					ImGui::Text("Sub-Objective Text: %s", subObjectiveText.c_str());
					ImGui::Text("Sub-Objective Type: %d", subObjectiveTargets);
				}
			}
		}
	}
}