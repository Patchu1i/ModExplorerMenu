#include "Console.h"
#include "Utils/Util.h"
#include "Window.h"

void NPCWindow::ShowActions(Settings::Style& a_style, Settings::Config& a_config)
{
	(void)a_style;
	(void)a_config;

	ImVec2 barSize = ImVec2(100.0f, ImGui::GetFontSize());
	float popWidth = ImGui::GetContentRegionAvail().x + 10.0f;

	const float button_height = ImGui::GetFontSize() * 1.5f;
	const float button_width = ImGui::GetContentRegionAvail().x;

	if (ImGui::Button("Show Nearby NPCs", ImVec2(button_width, button_height))) {
		PopulateListWithLocals();
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

	auto* npc = selectedNPC->form->As<RE::TESNPC>();

	if (npc == nullptr) {
		return;
	}

	if (ImGui::Button("Place as New", ImVec2(button_width, button_height))) {
		ConsoleCommand::PlaceAtMe(selectedNPC->formid, 1);
	}

	if (showLocalsOnly) {
		if (ImGui::Button("Bring to Me", ImVec2(button_width, button_height))) {
			ConsoleCommand::MoveToPlayer(std::format("{:08x}", selectedNPC->refID));
		}
	}

	// if (ImGui::Button("Bring to Me", ImVec2(button_width, button_height))) {
	// 	RE::FormID formid = selectedNPC->form->formID;

	// 	// Using shared_ptr since AddTask is async and we need
	// 	// to ensure the vector is alive when we return to the main thread/
	// 	auto references = std::make_shared<std::vector<RE::FormID>>();

	// 	auto callback = [references]() {
	// 		if (references->empty()) {
	// 			return;
	// 		}

	// 		// TODO: Implement behavior for multiple references.
	// 		for (auto ref : *references) {
	// 			ConsoleCommand::MoveToPlayer(std::format("{:08x}", ref));
	// 		}
	// 	};

	// 	SKSE::GetTaskInterface()->AddTask([references, formid, callback]() {
	// 		auto process = RE::ProcessLists::GetSingleton();
	// 		for (auto& handle : process->highActorHandles) {
	// 			if (!handle.get() || !handle.get().get()) {
	// 				continue;
	// 			}

	// 			auto actor = handle.get().get();
	// 			auto base = actor->GetBaseObject()->GetFormID();
	// 			auto ref = actor->GetFormID();

	// 			// Find object with matching baseid, and store its reference
	// 			// into the references vector.
	// 			if (base == formid) {
	// 				logger::info("Found reference: {:08x}", ref);
	// 				references->push_back(ref);
	// 			}
	// 		}

	// 		callback();  // Callback to main thread upon completion.
	// 	});
	// }

	if (ImGui::CollapsingHeader("NPC Skills")) {
		const auto skills = npc->playerSkills;
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