#pragma once
#include "Objects.h"

namespace ModExplorerMenu
{

	template <class Object>
	void ShowItemCard(Object* a_object)
	{
		if (a_object == nullptr) {
			ImGui::EndTooltip();
			return;
		}

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
				const auto defaultWidth = popWidth - ImGui::CalcTextSize(std::to_string(value).c_str()).x;
				const auto width = std::max(defaultWidth, ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(std::to_string(value).c_str()).x);
				ImGui::Text(label);
				ImGui::SameLine(width);
				ImGui::Text("%d", value);
			};

			const auto InlineText = [popWidth](const char* label, const char* text) {
				const auto width = std::max(popWidth - ImGui::CalcTextSize(text).x, ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(text).x);
				ImGui::Text(label);
				ImGui::SameLine(width);
				ImGui::Text(text);
			};

			const auto name = a_object->GetName();
			ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(name.data()));
			ImGui::Text(name.data());
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			// NPC Specific Item Card details:
			if constexpr (std::is_same<Object, NPC>::value) {
				auto* npc = a_object->TESForm->As<RE::TESNPC>();

				if (npc == nullptr) {
					ImGui::EndTooltip();
					return;
				}

				InlineBar("Health:", a_object->GetHealth(), 100);
				InlineBar("Magicka:", a_object->GetMagicka(), 100);
				InlineBar("Stamina:", a_object->GetStamina(), 100);
				InlineBar("Carry Weight:", a_object->GetCarryWeight(), 100);

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			}

			// constexpr allows for compile-time evaluation of struct passed
			// allowing for member access to be determined by control paths.
			if constexpr (std::is_same<Object, Item>::value) {
				if (a_object->GetFormType() == RE::FormType::Armor) {
					auto* armor = a_object->TESForm->As<RE::TESObjectARMO>();

					if (armor == nullptr) {
						ImGui::EndTooltip();
						return;
					}

					const auto armorType = Utils::GetArmorType(armor);
					const float armorRating = Utils::CalcBaseArmorRating(armor);
					const float armorRatingMax = Utils::CalcMaxArmorRating(armorRating, 50);
					const auto equipSlot = Utils::GetArmorSlot(armor);

					if (armorRating == 0) {
						InlineText("Armor Rating:", "None");
					} else {
						InlineBar("Armor Rating:", armorRating, armorRatingMax);
					}

					InlineText("Armor Type:", armorType);
					InlineText("Equip Slot:", equipSlot);
				}

				if (a_object->GetFormType() == RE::FormType::Weapon) {
					auto* weapon = a_object->TESForm->As<RE::TESObjectWEAP>();

					if (weapon == nullptr) {
						ImGui::EndTooltip();
						return;
					}

					const char* weaponTypes[] = {
						"Hand to Hand",
						"One Hand Sword",
						"One Hand Dagger",
						"One Hand Axe",
						"One Hand Mace",
						"Two Hand Sword",
						"Two Hand Axe",
						"Bow",
						"Staff",
						"Crossbow"
					};

					const float damage = Utils::CalcBaseDamage(weapon);
					const float max_damage = Utils::CalcMaxDamage(damage, 50);
					const float speed = weapon->weaponData.speed;
					const int dps = (int)(damage * speed);
					const uint16_t critDamage = weapon->GetCritDamage();
					const RE::ActorValue skill = weapon->weaponData.skill.get();
					const auto type = weaponTypes[static_cast<int>(weapon->GetWeaponType())];

					if (weapon->IsStaff()) {
						InlineText("Base Damage:", "N/A");
					} else if (weapon->IsBow() || weapon->IsCrossbow()) {
						InlineBar("Base Damage:", damage, max_damage);
						InlineBar("Draw Speed:", speed, 1.5f);
						InlineInt("DPS:", dps);
						ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
						InlineInt("Critical Damage:", critDamage);
						InlineText("Skill:", std::to_string(skill).c_str());
					} else {
						const float reach = (float)(weapon->weaponData.reach);
						const float stagger = weapon->weaponData.staggerValue;
						InlineBar("Base Damage:", damage, max_damage);
						InlineBar("Speed:", speed, 1.5f);
						InlineInt("DPS:", dps);
						ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
						InlineBar("Reach:", reach, 1.5f);
						InlineBar("stagger:", stagger, 2.0f);
						InlineInt("Critical Damage:", critDamage);
						InlineText("Skill:", std::to_string(skill).c_str());
					}

					ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
					InlineText("Type:", type);
				}

				InlineInt("Weight:", (int)a_object->GetWeight());
				InlineInt("Gold Value:", a_object->GetValue());

				const std::string desc = Utils::GetItemDescription(a_object->TESForm, g_DescriptionFrameworkInterface);
				if (!desc.empty()) {
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
					if (a_object->GetFormType() == RE::FormType::Book) {
						ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(desc.c_str()));  // Read Me!
					}
					ImGui::PushTextWrapPos(popWidth);
					ImGui::TextWrapped(desc.c_str());
					ImGui::PopTextWrapPos();
				}
			}

			ImGui::EndTooltip();
		}
	}
}