#pragma once

#include "PCH.h"

namespace ImGui
{
	inline static void InlineCheckbox(const char* label, bool* v)
	{
		ImGui::Checkbox(label, v);
		ImGui::SameLine();
	}

	[[nodiscard]] inline static const float GetCenterTextPosX(const char* text)
	{
		return ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x / 2 -
		       ImGui::CalcTextSize(text).x / 2;
	};

	inline static void SetDelayedTooltip(const char* text, float delay = 1.0f)
	{
		if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > delay) {
			if (ImGui::BeginTooltip()) {
				ImGui::Text(text);
				ImGui::EndTooltip();
			}
		}
	}

	inline static void HelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::BeginItemTooltip()) {
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
		ImGui::SameLine();
	}
}

namespace Utils
{

	inline static void RemoveHTMLTags(std::string& a_string)
	{
		while (a_string.find("<") != std::string::npos) {
			auto startpos = a_string.find("<");
			auto endpos = a_string.find(">") + 1;

			if (endpos != std::string::npos) {
				a_string.erase(startpos, endpos - startpos);
			}
		}

		while (a_string.find("[") != std::string::npos) {
			auto startpos = a_string.find("[");
			auto endpos = a_string.find("]") + 1;

			if (endpos != std::string::npos) {
				a_string.erase(startpos, endpos - startpos);
			}
		}
	};

	static inline std::map<const char*, RE::ActorValue> armorSkillMap = {
		{ "Light Armor", RE::ActorValue::kLightArmor },
		{ "Heavy Armor", RE::ActorValue::kHeavyArmor },
		{ "Clothing", RE::ActorValue::kNone }
	};

	inline static float CalcMaxArmorRating(float a_rating, float a_max)
	{
		return a_rating + a_max;
	};

	// CEILING[ (base armor rating + item quality) × (1 + 0.4 × (skill + skill effect)/100) ] × (1 + unison perk† ) × (1 + Matching Set) × (1 + armor perk‡)
	inline static float CalcBaseArmorRating(RE::TESObjectARMO* a_armor)
	{
		auto* player = RE::PlayerCharacter::GetSingleton();
		auto armorSkill = a_armor->GetObjectTypeName();
		auto baseRating = a_armor->GetArmorRating();

		if (armorSkillMap[armorSkill] == RE::ActorValue::kNone) {
			return 0;
		}

		auto skill = player->AsActorValueOwner()->GetActorValue(armorSkillMap[armorSkill]);

		return (float)std::ceil((baseRating + 0) * (1.0 + 0.4 * (skill + 0) / 100.0));
	}

	inline static float CalcMaxDamage(float a_damage, float a_max)
	{
		return a_damage + a_max;
	};

	// Round[ (base damage + smithing increase) * (1 + skill/200) * (1 + perk effects) * (1 + item effects) * (1 + potion effect) * (1 + Seeker of Might bonus) ]
	inline static float CalcBaseDamage(RE::TESObjectWEAP* a_weapon)
	{
		auto* player = RE::PlayerCharacter::GetSingleton();
		auto weaponSkill = a_weapon->weaponData.skill.get();
		auto baseDamage = a_weapon->GetAttackDamage();

		if (weaponSkill == RE::ActorValue::kNone) {
			return baseDamage;
		}

		auto skill = player->AsActorValueOwner()->GetActorValue(weaponSkill);

		return (float)std::round((baseDamage + 0) * (1.0 + skill / 200.0));
	};

	inline static const char* GetArmorType(RE::TESObjectARMO* a_armor)
	{
		switch (a_armor->GetArmorType()) {
		case RE::TESObjectARMO::ArmorType::kLightArmor:
			return "Light Armor";
		case RE::TESObjectARMO::ArmorType::kHeavyArmor:
			return "Heavy Armor";
		case RE::TESObjectARMO::ArmorType::kClothing:
			return "Clothing";
		default:
			return "Unknown";
		};
	};

	inline static const char* GetArmorSlot(RE::TESObjectARMO* a_armor)
	{
		switch (a_armor->GetSlotMask()) {
		case RE::BIPED_MODEL::BipedObjectSlot::kAmulet:
			return "Amulet";
		case RE::BIPED_MODEL::BipedObjectSlot::kBody:
			return "Body";
		case RE::BIPED_MODEL::BipedObjectSlot::kCalves:
			return "Calves";
		case RE::BIPED_MODEL::BipedObjectSlot::kCirclet:
			return "Circlet";
		case RE::BIPED_MODEL::BipedObjectSlot::kDecapitate:
			return "Decapitate";
		case RE::BIPED_MODEL::BipedObjectSlot::kDecapitateHead:
			return "Decapitate Head";
		case RE::BIPED_MODEL::BipedObjectSlot::kEars:
			return "Ears";
		case RE::BIPED_MODEL::BipedObjectSlot::kFeet:
			return "Feet";
		case RE::BIPED_MODEL::BipedObjectSlot::kForearms:
			return "Forearms";
		case RE::BIPED_MODEL::BipedObjectSlot::kFX01:
			return "FX01";
		case RE::BIPED_MODEL::BipedObjectSlot::kHair:
			return "Hair";
		case RE::BIPED_MODEL::BipedObjectSlot::kHands:
			return "Hands";
		case RE::BIPED_MODEL::BipedObjectSlot::kHead:
			return "Head";
		case RE::BIPED_MODEL::BipedObjectSlot::kLongHair:
			return "Long Hair";
		case RE::BIPED_MODEL::BipedObjectSlot::kModArmLeft:
			return "Mod Arm Left";
		case RE::BIPED_MODEL::BipedObjectSlot::kModArmRight:
			return "Mod Arm Right";
		case RE::BIPED_MODEL::BipedObjectSlot::kModBack:
			return "Mod Back";
		case RE::BIPED_MODEL::BipedObjectSlot::kModChestPrimary:
			return "Mod Chest Primary";
		case RE::BIPED_MODEL::BipedObjectSlot::kModChestSecondary:
			return "Mod Chest Secondary";
		case RE::BIPED_MODEL::BipedObjectSlot::kModFaceJewelry:
			return "Mod Face Jewelry";
		case RE::BIPED_MODEL::BipedObjectSlot::kModLegLeft:
			return "Mod Leg Left";
		case RE::BIPED_MODEL::BipedObjectSlot::kModLegRight:
			return "Mod Leg Right";
		case RE::BIPED_MODEL::BipedObjectSlot::kModMisc1:
			return "Mod Misc1";
		case RE::BIPED_MODEL::BipedObjectSlot::kModMisc2:
			return "Mod Misc2";
		case RE::BIPED_MODEL::BipedObjectSlot::kModMouth:
			return "Mod Mouth";
		case RE::BIPED_MODEL::BipedObjectSlot::kModNeck:
			return "Mod Neck";
		case RE::BIPED_MODEL::BipedObjectSlot::kModPelvisPrimary:
			return "Mod Pelvis Primary";
		case RE::BIPED_MODEL::BipedObjectSlot::kModPelvisSecondary:
			return "Mod Pelvis Secondary";
		case RE::BIPED_MODEL::BipedObjectSlot::kModShoulder:
			return "Mod Shoulder";
		case RE::BIPED_MODEL::BipedObjectSlot::kRing:
			return "Ring";
		case RE::BIPED_MODEL::BipedObjectSlot::kShield:
			return "Shield";
		case RE::BIPED_MODEL::BipedObjectSlot::kTail:
			return "Tail";
		default:
			return "None";
		};
	};

	template <class T>
	[[nodiscard]] inline static std::string GetItemDescription(T& a_interface, RE::TESForm* form)
	{
		std::string s_descFramework = "";
		if (a_interface != nullptr) {
			std::string desc = a_interface->GetDescription(form);
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
}