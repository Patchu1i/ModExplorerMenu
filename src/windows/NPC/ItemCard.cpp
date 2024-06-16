
#include "Utils/Util.h"
#include "Window.h"

namespace ModExplorerMenu
{
	// FIXME: Width get's messed up on ultra long titles.
	// TODO: Add more details to other items.
	void NPCWindow::ShowItemCard(Data::CachedNPC* a_npc)
	{
		//ImGui::SetNextWindowSize(ImVec2(10, 10));
		if (ImGui::BeginTooltip()) {
			ImVec2 barSize = ImVec2(100.0f, ImGui::GetFontSize());
			float popWidth = barSize.x * 3;

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
				float curr = static_cast<float>(value);
				char buffer[256];
				sprintf(buffer, "%.2f", value);
				ImGui::ProgressBar(curr / max_value, barSize, buffer);
				ImGui::PopStyleColor(1);
				ImGui::PopStyleVar(1);
			};

			const auto InlineInt = [popWidth, barSize](const char* label, const int value) {
				ImGui::Text(label);
				ImGui::SameLine(popWidth - ImGui::CalcTextSize(std::to_string(value).c_str()).x);
				ImGui::Text("%d", value);
			};

			const auto InlineText = [popWidth](const char* label, const char* text) {
				ImGui::Text(label);
				ImGui::SameLine(popWidth - ImGui::CalcTextSize(text).x);
				ImGui::Text(text);
			};

			if (a_npc == nullptr) {
				ImGui::EndTooltip();
				return;
			}

			// Header
			const auto name = a_npc->name;
			ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(name.c_str()));
			ImGui::Text(name.c_str());
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			if (a_npc->form->formType == RE::FormType::NPC) {
				auto* npc = a_npc->form->As<RE::TESNPC>();

				if (npc == nullptr) {
					ImGui::EndTooltip();
					return;
				}

				const float health = a_npc->health;
				const float magicka = a_npc->magicka;
				const float stamina = a_npc->stamina;
				const float carryweight = a_npc->carryweight;

				InlineBar("Health:", health, 100);
				InlineBar("Magicka:", magicka, 100);
				InlineBar("Stamina:", stamina, 100);
				InlineBar("Carry Weight:", carryweight, 100);

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			}

			// if (formType == RE::FormType::Armor) {
			// 	auto* armor = item->form->As<RE::TESObjectARMO>();

			// 	if (armor == nullptr) {
			// 		ImGui::EndTooltip();
			// 		return;
			// 	}

			// 	const auto armorType = Utils::GetArmorType(armor);
			// 	const float armorRating = Utils::CalcBaseArmorRating(armor);
			// 	const float armorRatingMax = Utils::CalcMaxArmorRating(armorRating, 50);
			// 	const auto equipSlot = Utils::GetArmorSlot(armor);
			// 	const int weight = (int)(item->weight);

			// 	if (armorRating == 0) {
			// 		InlineText("Armor Rating:", "None");
			// 	} else {
			// 		InlineBar("Armor Rating:", armorRating, armorRatingMax);
			// 	}

			// 	InlineText("Armor Type:", armorType);
			// 	InlineText("Equip Slot:", equipSlot);

			// 	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			// 	InlineInt("Weight:", weight);
			// }

			// Always show:
			//InlineInt("Gold Value:", item->goldValue);

			// const std::string desc = Utils::GetItemDescription(g_DescriptionFrameworkInterface, item->form);
			// if (!desc.empty()) {
			// 	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			// 	if (formType == RE::FormType::Book) {
			// 		ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(desc.c_str()));  // Read Me!
			// 	}
			// 	ImGui::PushTextWrapPos(popWidth);
			// 	ImGui::TextWrapped(desc.c_str());
			// 	ImGui::PopTextWrapPos();
			// }

			ImGui::EndTooltip();
		}
	}
}