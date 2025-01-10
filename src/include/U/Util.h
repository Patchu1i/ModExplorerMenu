#pragma once

#include "include/S/Settings.h"
#include <PCH.h>

namespace ImGui
{
	[[nodiscard]] inline static const float GetCenterTextPosX(const char* text)
	{
		return ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x / 2 -
		       ImGui::CalcTextSize(text).x / 2;
	};

	inline static void ShowLanguagePopup()
	{
		// auto& style = Modex::Settings::GetSingleton()->GetStyle();

		auto width = ImGui::GetMainViewport()->Size.x * 0.25f;
		auto height = ImGui::GetMainViewport()->Size.y * 0.20f;
		const float center_x = ImGui::GetMainViewport()->Size.x * 0.5f;
		const float center_y = ImGui::GetMainViewport()->Size.y * 0.5f;

		const float pos_x = center_x - (width * 0.5f);
		const float pos_y = center_y - (height * 0.5f);

		const float buttonHeight = ImGui::GetFontSize() * 1.5f;

		ImGui::SetNextWindowSize(ImVec2(width, height));
		ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y));
		if (ImGui::BeginPopupModal("Non-latin Alphabetical Language", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
			ImGui::SetCursorPosX(ImGui::GetCenterTextPosX("Non-latin Alphabetical Language"));
			// ImGui::PushFont(style.font.normal);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
			ImGui::Text("Non-latin Alphabetical Language");
			ImGui::PopStyleColor(1);
			// ImGui::PopFont();

			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			ImGui::TextWrapped(
				"You have selected a non-latin alphabetical language. As a result, you"
				" may need to restart the game for the changes to take effect.");

			ImGui::NewLine();
			ImGui::Text("Do you understand, and wish to proceed?");
			ImGui::NewLine();

			ImGui::SetCursorPosY(ImGui::GetWindowSize().y - (buttonHeight * 2) - 20.0f);  // subtract button size * 2 + separator
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			if (ImGui::Button("Yes, I understand", ImVec2(ImGui::GetContentRegionAvail().x, buttonHeight))) {
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("No, take me back!", ImVec2(ImGui::GetContentRegionAvail().x, buttonHeight))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	// TODO: Implement more use cases for this.
	// Call ImGui::OpenPopup(<popup_name>) inside a user event.
	// Call ImGui::ShowWarningPopup(<popup_name>, <callback>) outside of conditional logic.
	inline static void ShowWarningPopup(const char* warning, std::function<void()> callback)
	{
		// auto& style = Modex::Settings::GetSingleton()->GetStyle();

		auto width = ImGui::GetMainViewport()->Size.x * 0.25f;
		auto height = ImGui::GetMainViewport()->Size.y * 0.20f;
		const float center_x = ImGui::GetMainViewport()->Size.x * 0.5f;
		const float center_y = ImGui::GetMainViewport()->Size.y * 0.5f;

		const float pos_x = center_x - (width * 0.5f);
		const float pos_y = center_y - (height * 0.5f);

		const float buttonHeight = ImGui::GetFontSize() * 1.5f;

		ImGui::SetNextWindowSize(ImVec2(width, height));
		ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y));
		if (ImGui::BeginPopupModal(warning, nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
			ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(warning));
			// ImGui::PushFont(style.font.normal);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
			ImGui::Text(warning);
			ImGui::PopStyleColor(1);
			// ImGui::PopFont();

			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			ImGui::TextWrapped(
				"You are about to spawn a large number of objects. This may result in"
				" undefined behavior such as your game freezing, crashing, or weird things"
				" occuring. It's not recommended to use this outside of testing purposes. ");

			ImGui::NewLine();
			ImGui::Text("Do you understand, and wish to proceed?");
			ImGui::NewLine();

			ImGui::SetCursorPosY(ImGui::GetWindowSize().y - (buttonHeight * 2) - 20.0f);  // subtract button size * 2 + separator
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			if (ImGui::Button("Yes, I understand", ImVec2(ImGui::GetContentRegionAvail().x, buttonHeight))) {
				callback();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("No, take me back!", ImVec2(ImGui::GetContentRegionAvail().x, buttonHeight))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	//
	// ImGui Replacements specifically for theme integration.
	//

	inline static bool m_Button(const char* label, Modex::Settings::Style& style, const ImVec2& size = ImVec2(0, 0))
	{
		(void)style;

		// ImGui::PushFont(style.buttonFont.normal);
		auto result = ImGui::Button(label, size);
		// ImGui::PopFont();
		return result;
	}

	inline static bool m_Selectable(
		const char* label,
		bool& selected,
		Modex::Settings::Style& style,
		ImGuiSelectableFlags flag = ImGuiSelectableFlags_None,
		const ImVec2& size = ImVec2(0, 0))
	{
		auto innerPadding = style.widgetPadding.y;
		auto newSize = ImVec2(size.x, size.y + innerPadding);

		// ImGui::PushFont(style.buttonFont.normal);
		auto result = ImGui::Selectable(label, &selected, flag, newSize);
		// ImGui::PopFont();
		return result;
	}

	//
	// End of ImGui Replacements.
	//

	inline static bool DisabledButton(const char* label, bool& disabled, const ImVec2& size = ImVec2(0, 0))
	{
		float alpha = disabled ? 1.0f : 0.5f;
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * alpha);
		auto result = ImGui::Button(label, size);
		ImGui::PopStyleVar();
		return result;
	}

	inline static bool DisabledCheckbox(const char* label, bool& disabled, bool& v)
	{
		float alpha = disabled ? 0.5f : 1.0f;
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * alpha);

		bool result;
		if (disabled) {
			auto preview = v;
			result = ImGui::Checkbox(label, &preview);
		} else {
			result = ImGui::Checkbox(label, &v);
		}
		ImGui::PopStyleVar();
		return result;
	}

	inline static bool DisabledImageButton(
		const char* label, bool& disabled, ImTextureID user_texture_id, const ImVec2& size = ImVec2(0, 0),
		const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& bg_col = ImVec4(0, 0, 0, 0),
		const ImVec4& tint_col = ImVec4(1, 1, 1, 1))
	{
		float alpha = disabled ? 0.5f : 1.0f;
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * alpha);
		auto result = ImGui::ImageButton(label, user_texture_id, size, uv0, uv1, bg_col, tint_col);
		ImGui::PopStyleVar();
		return result;
	}

	inline static bool InlineCheckbox(const char* label, bool* v)
	{
		bool changed = false;
		if (ImGui::Checkbox(label, v)) {
			changed = true;
		}
		ImGui::SameLine();
		return changed;
	}

	inline static void SetDelayedTooltip(const char* text, float delay = 1.0f)
	{
		if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > delay) {
			if (ImGui::BeginTooltip()) {
				ImGui::Text(text);
				ImGui::EndTooltip();
			}
		}
	}

	inline static void SetQuickTooltip(const char* text)
	{
		if (ImGui::IsItemHovered()) {
			if (ImGui::BeginTooltip()) {
				ImGui::Text(text);
				ImGui::EndTooltip();
			}
		}
	}

	inline static void HelpMarker(const char* desc)
	{
		ImGui::TextDisabled(ICON_RPG_QUESTION);
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

	inline static std::vector<std::string> GetSkillNames()
	{
		return {
			"Block",
			"Two-Handed",
			"One-Handed",
			"Archery",
			"Light Armor",
			"Pickpocket",
			"Lockpicking",
			"Sneak",
			"Alchemy",
			"Speech",
			"Illusion",
			"Conjuration",
			"Destruction",
			"Restoration",
			"Alteration",
			"Enchanting",
			"Smithing",
			"Heavy Armor"
		};
	}

	// Conversion functions for ANSI <-> UTF-8 <-> UTF-16 (wchar_t) that use std::string and std::wstring
	// Source: https://github.com/Kerite/SkyrimInputMethod/blob/a2e3ba47fe35d000e0eea01b32c666a625cccafd/src/Utils.cpp#L284

	// Convert a wide Unicode string to an UTF8 string
	inline static std::string utf8_encode(const std::wstring& wstr)
	{
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

	// Convert an UTF8 string to a wide Unicode String
	inline static std::wstring utf8_decode(const std::string& str)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	// Convert an wide Unicode string to ANSI string
	inline static std::string unicode2ansi(const std::wstring& wstr)
	{
		int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], -1, NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

	// Convert an ANSI string to a wide Unicode String
	inline static std::wstring ansi2unicode(const std::string& str)
	{
		int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	inline static std::string RemoveQuotesInPath(const std::string& path)
	{
		std::string newPath = path;
		newPath.erase(std::remove(newPath.begin(), newPath.end(), '\"'), newPath.end());
		return newPath;
	}

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

	inline static const char* GetSpellType(RE::MagicSystem::SpellType a_type)
	{
		switch (a_type) {
		case RE::MagicSystem::SpellType::kSpell:
			return "Spell";
		case RE::MagicSystem::SpellType::kDisease:
			return "Disease";
		case RE::MagicSystem::SpellType::kPower:
			return "Power";
		case RE::MagicSystem::SpellType::kLesserPower:
			return "Lesser Power";
		case RE::MagicSystem::SpellType::kAbility:
			return "Ability";
		case RE::MagicSystem::SpellType::kPoison:
			return "Poison";
		case RE::MagicSystem::SpellType::kEnchantment:
			return "Enchantment";
		case RE::MagicSystem::SpellType::kPotion:
			return "Potion";
		default:
			return "Unknown";
		}
	}

	inline static const char* GetCastingType(RE::MagicSystem::CastingType a_type)
	{
		switch (a_type) {
		case RE::MagicSystem::CastingType::kConstantEffect:
			return "Constant Effect";
		case RE::MagicSystem::CastingType::kFireAndForget:
			return "Fire and Forget";
		case RE::MagicSystem::CastingType::kConcentration:
			return "Concentration";
		case RE::MagicSystem::CastingType::kScroll:
			return "Scroll";
		default:
			return "Unknown";
		}
	}

	inline static const char* GetDeliveryType(RE::MagicSystem::Delivery a_delivery)
	{
		switch (a_delivery) {
		case RE::MagicSystem::Delivery::kSelf:
			return "Self";
		case RE::MagicSystem::Delivery::kTouch:
			return "Touch";
		case RE::MagicSystem::Delivery::kAimed:
			return "Aimed";
		case RE::MagicSystem::Delivery::kTargetActor:
			return "Target Actor";
		case RE::MagicSystem::Delivery::kTargetLocation:
			return "Target Location";
		default:
			return "Unknown";
		}
	}

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

	inline static const char* GetArmorSlotName(RE::BIPED_MODEL::BipedObjectSlot a_slot)
	{
		switch (a_slot) {
		case RE::BIPED_MODEL::BipedObjectSlot::kAmulet:
			return "35 - Amulet";
		case RE::BIPED_MODEL::BipedObjectSlot::kBody:
			return "32 - Body";
		case RE::BIPED_MODEL::BipedObjectSlot::kCalves:
			return "38 - Calves";
		case RE::BIPED_MODEL::BipedObjectSlot::kCirclet:
			return "42 - Circlet";
		case RE::BIPED_MODEL::BipedObjectSlot::kDecapitate:
			return "51 - Decapitate";
		case RE::BIPED_MODEL::BipedObjectSlot::kDecapitateHead:
			return "50 - Decapitate Head";
		case RE::BIPED_MODEL::BipedObjectSlot::kEars:
			return "43 - Ears";
		case RE::BIPED_MODEL::BipedObjectSlot::kFeet:
			return "37 - Feet";
		case RE::BIPED_MODEL::BipedObjectSlot::kForearms:
			return "34 - Forearms";
		case RE::BIPED_MODEL::BipedObjectSlot::kFX01:
			return "61 - FX01";
		case RE::BIPED_MODEL::BipedObjectSlot::kHair:
			return "31 - Hair";
		case RE::BIPED_MODEL::BipedObjectSlot::kHands:
			return "33 - Hands";
		case RE::BIPED_MODEL::BipedObjectSlot::kHead:
			return "30 - Head";
		case RE::BIPED_MODEL::BipedObjectSlot::kLongHair:
			return "41 - Long Hair";
		case RE::BIPED_MODEL::BipedObjectSlot::kModArmLeft:
			return "58 - Mod Arm Left";
		case RE::BIPED_MODEL::BipedObjectSlot::kModArmRight:
			return "59 - Mod Arm Right";
		case RE::BIPED_MODEL::BipedObjectSlot::kModBack:
			return "47 - Mod Back";
		case RE::BIPED_MODEL::BipedObjectSlot::kModChestPrimary:
			return "46 - Mod Chest Primary";
		case RE::BIPED_MODEL::BipedObjectSlot::kModChestSecondary:
			return "56 - Mod Chest Secondary";
		case RE::BIPED_MODEL::BipedObjectSlot::kModFaceJewelry:
			return "55 - Mod Face Jewelry";
		case RE::BIPED_MODEL::BipedObjectSlot::kModLegLeft:
			return "54 - Mod Leg Left";
		case RE::BIPED_MODEL::BipedObjectSlot::kModLegRight:
			return "53 - Mod Leg Right";
		case RE::BIPED_MODEL::BipedObjectSlot::kModMisc1:
			return "48 - Mod Misc1";
		case RE::BIPED_MODEL::BipedObjectSlot::kModMisc2:
			return "60 - Mod Misc2";
		case RE::BIPED_MODEL::BipedObjectSlot::kModMouth:
			return "44 - Mod Mouth";
		case RE::BIPED_MODEL::BipedObjectSlot::kModNeck:
			return "45 - Mod Neck";
		case RE::BIPED_MODEL::BipedObjectSlot::kModPelvisPrimary:
			return "49 - Mod Pelvis Primary";
		case RE::BIPED_MODEL::BipedObjectSlot::kModPelvisSecondary:
			return "52 - Mod Pelvis Secondary";
		case RE::BIPED_MODEL::BipedObjectSlot::kModShoulder:
			return "57 - Mod Shoulder";
		case RE::BIPED_MODEL::BipedObjectSlot::kRing:
			return "36 - Ring";
		case RE::BIPED_MODEL::BipedObjectSlot::kShield:
			return "39 - Shield";
		case RE::BIPED_MODEL::BipedObjectSlot::kTail:
			return "40 - Tail";
		default:
			return "MODEX_ERR";
		};
	};

	// Returns a list of armor slots assigned to the armor.
	// Returns "None" if no slots are assigned to prevent out-of-range access. (Issue #21)
	inline static std::vector<std::string> GetArmorSlots(RE::TESObjectARMO* a_armor)
	{
		std::vector<std::string> slots;
		auto bipedObject = a_armor->As<RE::BGSBipedObjectForm>();
		if (bipedObject) {
			auto slotMask = bipedObject->GetSlotMask();
			for (int i = 0; i < 32; i++) {
				if (static_cast<int>(slotMask) & (1 << i)) {
					slots.push_back(GetArmorSlotName(static_cast<RE::BIPED_MODEL::BipedObjectSlot>(1 << i)));
				}
			}
		}

		if (slots.empty()) {
			slots.push_back("None");
		}

		return slots;
	}

	template <class T>
	[[nodiscard]] inline static std::string GetItemDescription(RE::TESForm* form, T& a_interface = nullptr)
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