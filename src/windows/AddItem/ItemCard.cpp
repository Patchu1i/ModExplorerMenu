
#include "Utils/Util.h"
#include "Window.h"


std::string GetItemDescription(RE::TESForm* form)
{
	std::string s_descFramework = "";
	if (g_DescriptionFrameworkInterface != nullptr) {
		std::string desc = g_DescriptionFrameworkInterface->GetDescription(form);
		if (!desc.empty()) {
			Utils::RemoveHTMLTags(desc);
			s_descFramework = std::string(desc) + "\n";
		}
	}

	std::string s_tesDescription = "";
	if (form->As<RE::TESDescription>() != nullptr) {
		const auto desc = form->As<RE::TESDescription>();
		if (desc) {
			RE::BSString buf;
			desc->GetDescription(buf, nullptr);

			if (form->formType == RE::FormType::Book) {
				s_tesDescription = "[Right Click -> Read Me!]";
			} else if (!buf.empty()) {
				s_tesDescription = std::string(buf) + "\n";
			}
		}
	}

	return s_descFramework + s_tesDescription;
}

// FIXME: Width get's messed up on ultra long titles.
// TODO: Add more details to other items.
void AddItemWindow::ShowItemCard(MEMData::CachedItem* item)
{
	//ImGui::SetNextWindowSize(ImVec2(10, 10));
	if (ImGui::BeginTooltip()) {
		ImVec2 barSize = ImVec2(100.0f, ImGui::GetFontSize());
		float popWidth = barSize.x * 3;

		const auto formType = item->formType;

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

		if (item == nullptr) {
			ImGui::EndTooltip();
			return;
		}

		// Header
		ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(item->name));
		ImGui::Text(item->name);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		if (formType == RE::FormType::Armor) {
			auto* armor = item->form->As<RE::TESObjectARMO>();

			if (armor == nullptr) {
				ImGui::EndTooltip();
				return;
			}

			const auto armorType = Utils::GetArmorType(armor);
			const float armorRating = Utils::CalcBaseArmorRating(armor);
			const float armorRatingMax = Utils::CalcMaxArmorRating(armorRating, 50);
			const auto equipSlot = Utils::GetArmorSlot(armor);
			const int weight = (int)(item->weight);

			if (armorRating == 0) {
				InlineText("Armor Rating:", "None");
			} else {
				InlineBar("Armor Rating:", armorRating, armorRatingMax);
			}

			InlineText("Armor Type:", armorType);
			InlineText("Equip Slot:", equipSlot);

			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			InlineInt("Weight:", weight);
		}

		if (formType == RE::FormType::Weapon) {
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

			auto* weapon = item->form->As<RE::TESObjectWEAP>();

			if (weapon == nullptr) {
				ImGui::EndTooltip();
				return;
			}

			const float damage = Utils::CalcBaseDamage(weapon);
			const float max_damage = Utils::CalcMaxDamage(damage, 50);
			const float speed = weapon->weaponData.speed;
			const int weight = (int)(item->weight);
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
			InlineInt("Weight:", weight);
			InlineText("Type:", type);
		}

		// Always show:
		InlineInt("Gold Value:", item->goldValue);

		const std::string desc = GetItemDescription(item->form);
		if (!desc.empty()) {
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			if (formType == RE::FormType::Book) {
				ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(desc.c_str()));  // Read Me!
			}
			ImGui::PushTextWrapPos(popWidth);
			ImGui::TextWrapped(desc.c_str());
			ImGui::PopTextWrapPos();
		}

		ImGui::EndTooltip();
	}
}