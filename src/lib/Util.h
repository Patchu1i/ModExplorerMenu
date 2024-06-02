#pragma once

#include "PCH.h"

namespace Utils
{

	inline static float CalcMaxDamage(double a_damage, float a_max)
	{
		return (float)a_damage + a_max;
	};

	// Round[ (base damage + smithing increase) * (1 + skill/200) * (1 + perk effects) * (1 + item effects) * (1 + potion effect) * (1 + Seeker of Might bonus) ]
	inline static double CalcBaseDamage(RE::TESObjectWEAP* a_weapon)
	{
		if (a_weapon->attackDamage == 0) {
			return 0;
		}

		auto* player = RE::PlayerCharacter::GetSingleton();
		auto weaponSkill = a_weapon->weaponData.skill.get();
		auto skill = player->AsActorValueOwner()->GetActorValue(weaponSkill);
		auto baseDamage = a_weapon->GetAttackDamage();

		return std::round((baseDamage + 0) * (1.0 + skill / 200.0));
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
}