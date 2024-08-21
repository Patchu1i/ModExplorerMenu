#pragma once

#include "Data.h"
#include "Settings.h"

/*
enum class ActorValue
	{
		kNone = static_cast<std::underlying_type_t<ActorValue>>(-1),
		kAggression = 0,
		kConfidence = 1,
		kEnergy = 2,
		kMorality = 3,
		kMood = 4,
		kAssistance = 5,
		kOneHanded = 6,
		kTwoHanded = 7,
		kArchery = 8,
		kBlock = 9,
		kSmithing = 10,
		kHeavyArmor = 11,
		kLightArmor = 12,
		kPickpocket = 13,
		kLockpicking = 14,
		kSneak = 15,
		kAlchemy = 16,
		kSpeech = 17,
		kAlteration = 18,
		kConjuration = 19,
		kDestruction = 20,
		kIllusion = 21,
		kRestoration = 22,
		kEnchanting = 23,
		kHealth = 24,
		kMagicka = 25,
		kStamina = 26,
		kHealRate = 27,
		kMagickaRate = 28,
		kStaminaRate = 29,
		kSpeedMult = 30,
		kInventoryWeight = 31,
		kCarryWeight = 32,
		kCriticalChance = 33,
		kMeleeDamage = 34,
		kUnarmedDamage = 35,
		kMass = 36,
		kVoicePoints = 37,
		kVoiceRate = 38,
		kDamageResist = 39,
		kPoisonResist = 40,
		kResistFire = 41,
		kResistShock = 42,
		kResistFrost = 43,
		kResistMagic = 44,
		kResistDisease = 45,
		kPerceptionCondition = 46,
		kEnduranceCondition = 47,
		kLeftAttackCondition = 48,
		kRightAttackCondition = 49,
		kLeftMobilityCondition = 50,
		kRightMobilityCondition = 51,
		kBrainCondition = 52,
		kParalysis = 53,
		kInvisibility = 54,
		kNightEye = 55,
		kDetectLifeRange = 56,
		kWaterBreathing = 57,
		kWaterWalking = 58,
		kIgnoreCrippledLimbs = 59,
		kFame = 60,
		kInfamy = 61,
		kJumpingBonus = 62,
		kWardPower = 63,
		kRightItemCharge = 64,
		kArmorPerks = 65,
		kShieldPerks = 66,
		kWardDeflection = 67,
		kVariable01 = 68,
		kVariable02 = 69,
		kVariable03 = 70,
		kVariable04 = 71,
		kVariable05 = 72,
		kVariable06 = 73,
		kVariable07 = 74,
		kVariable08 = 75,
		kVariable09 = 76,
		kVariable10 = 77,
		kBowSpeedBonus = 78,
		kFavorActive = 79,
		kFavorsPerDay = 80,
		kFavorsPerDayTimer = 81,
		kLeftItemCharge = 82,
		kAbsorbChance = 83,
		kBlindness = 84,
		kWeaponSpeedMult = 85,
		kShoutRecoveryMult = 86,
		kBowStaggerBonus = 87,
		kTelekinesis = 88,
		kFavorPointsBonus = 89,
		kLastBribedIntimidated = 90,
		kLastFlattered = 91,
		kMovementNoiseMult = 92,
		kBypassVendorStolenCheck = 93,
		kBypassVendorKeywordCheck = 94,
		kWaitingForPlayer = 95,
		kOneHandedModifier = 96,
		kTwoHandedModifier = 97,
		kMarksmanModifier = 98,
		kBlockModifier = 99,
		kSmithingModifier = 100,
		kHeavyArmorModifier = 101,
		kLightArmorModifier = 102,
		kPickpocketModifier = 103,
		kLockpickingModifier = 104,
		kSneakingModifier = 105,
		kAlchemyModifier = 106,
		kSpeechcraftModifier = 107,
		kAlterationModifier = 108,
		kConjurationModifier = 109,
		kDestructionModifier = 110,
		kIllusionModifier = 111,
		kRestorationModifier = 112,
		kEnchantingModifier = 113,
		kOneHandedSkillAdvance = 114,
		kTwoHandedSkillAdvance = 115,
		kMarksmanSkillAdvance = 116,
		kBlockSkillAdvance = 117,
		kSmithingSkillAdvance = 118,
		kHeavyArmorSkillAdvance = 119,
		kLightArmorSkillAdvance = 120,
		kPickpocketSkillAdvance = 121,
		kLockpickingSkillAdvance = 122,
		kSneakingSkillAdvance = 123,
		kAlchemySkillAdvance = 124,
		kSpeechcraftSkillAdvance = 125,
		kAlterationSkillAdvance = 126,
		kConjurationSkillAdvance = 127,
		kDestructionSkillAdvance = 128,
		kIllusionSkillAdvance = 129,
		kRestorationSkillAdvance = 130,
		kEnchantingSkillAdvance = 131,
		kLeftWeaponSpeedMultiply = 132,
		kDragonSouls = 133,
		kCombatHealthRegenMultiply = 134,
		kOneHandedPowerModifier = 135,
		kTwoHandedPowerModifier = 136,
		kMarksmanPowerModifier = 137,
		kBlockPowerModifier = 138,
		kSmithingPowerModifier = 139,
		kHeavyArmorPowerModifier = 140,
		kLightArmorPowerModifier = 141,
		kPickpocketPowerModifier = 142,
		kLockpickingPowerModifier = 143,
		kSneakingPowerModifier = 144,
		kAlchemyPowerModifier = 145,
		kSpeechcraftPowerModifier = 146,
		kAlterationPowerModifier = 147,
		kConjurationPowerModifier = 148,
		kDestructionPowerModifier = 149,
		kIllusionPowerModifier = 150,
		kRestorationPowerModifier = 151,
		kEnchantingPowerModifier = 152,
		kDragonRend = 153,
		kAttackDamageMult = 154,
		kHealRateMult = 155,
		kMagickaRateMult = 156,
		kStaminaRateMult = 157,
		kWerewolfPerks = 158,
		kVampirePerks = 159,
		kGrabActorOffset = 160,
		kGrabbed = 161,
		kDEPRECATED05 = 162,
		kReflectDamage = 163,

		kTotal
	};
*/

namespace ModExplorerMenu
{
	class PersistentData
	{
	public:
		static inline int iDragonSouls = 0;
		static inline std::vector<RE::TESShout*> shouts;
		static inline std::vector<RE::TESWordOfPower*> words;
		static inline std::vector<std::pair<std::string, RE::ActorValue>> statNames = {
			{ "Health", RE::ActorValue::kHealth },
			{ "Magicka", RE::ActorValue::kMagicka },
			{ "Stamina", RE::ActorValue::kStamina },
			{ "HealRate", RE::ActorValue::kHealRate },
			{ "MagickaRate", RE::ActorValue::kMagickaRate },
			{ "StaminaRate", RE::ActorValue::kStaminaRate },
			{ "AttackDamageMult", RE::ActorValue::kAttackDamageMult },
			//{ "HealRateMult", RE::ActorValue::kHealRateMult },
			//{ "MagickaRateMult", RE::ActorValue::kMagickaRateMult },
			//{ "StaminaRateMult", RE::ActorValue::kStaminaRateMult },
			{ "SpeedMult", RE::ActorValue::kSpeedMult },
			{ "InventoryWeight", RE::ActorValue::kInventoryWeight },
			{ "CarryWeight", RE::ActorValue::kCarryWeight },
			{ "CriticalChance", RE::ActorValue::kCriticalChance },
			{ "MeleeDamage", RE::ActorValue::kMeleeDamage },
			{ "UnarmedDamage", RE::ActorValue::kUnarmedDamage },
			{ "DamageResist (Armor Rating++)", RE::ActorValue::kDamageResist },
			{ "PoisonResist", RE::ActorValue::kPoisonResist },
			{ "ResistFire", RE::ActorValue::kResistFire },
			{ "ResistShock", RE::ActorValue::kResistShock },
			{ "ResistFrost", RE::ActorValue::kResistFrost },
			{ "ResistMagic", RE::ActorValue::kResistMagic },
			{ "ResistDisease", RE::ActorValue::kResistDisease },
			{ "BowSpeedBonus (Slow Time++)", RE::ActorValue::kBowSpeedBonus },
			{ "WeaponSpeedMult", RE::ActorValue::kWeaponSpeedMult },
			{ "ShoutRecoveryMult", RE::ActorValue::kShoutRecoveryMult },
			{ "BowStaggerBonus", RE::ActorValue::kBowStaggerBonus },
			{ "CombatHealthRegenMultiply", RE::ActorValue::kCombatHealthRegenMultiply }
		};
		static inline std::vector<std::pair<std::string, RE::ActorValue>> skillNames = {
			{ "OneHanded", RE::ActorValue::kOneHanded },
			{ "TwoHanded", RE::ActorValue::kTwoHanded },
			{ "Archery", RE::ActorValue::kArchery },
			{ "Block", RE::ActorValue::kBlock },
			{ "Smithing", RE::ActorValue::kSmithing },
			{ "HeavyArmor", RE::ActorValue::kHeavyArmor },
			{ "LightArmor", RE::ActorValue::kLightArmor },
			{ "Pickpocket", RE::ActorValue::kPickpocket },
			{ "Lockpicking", RE::ActorValue::kLockpicking },
			{ "Sneak", RE::ActorValue::kSneak },
			{ "Alchemy", RE::ActorValue::kAlchemy },
			{ "Speech", RE::ActorValue::kSpeech },
			{ "Alteration", RE::ActorValue::kAlteration },
			{ "Conjuration", RE::ActorValue::kConjuration },
			{ "Destruction", RE::ActorValue::kDestruction },
			{ "Illusion", RE::ActorValue::kIllusion },
			{ "Restoration", RE::ActorValue::kRestoration },
			{ "Enchanting", RE::ActorValue::kEnchanting },

		};
		// static inline std::unordered_map<RE::ActorValue, float> skillXPMultiplier = {
		// 	{ RE::ActorValue::kOneHanded, 110.0f },
		// 	{ RE::ActorValue::kTwoHanded, 179.0f },
		// 	{ RE::ActorValue::kAlteration, 132.0f },
		// 	{ RE::ActorValue::kArchery, 43.0f },
		// 	{ RE::ActorValue::kBlock, 86.0f },
		// 	{ RE::ActorValue::kSmithing, 200.0f },
		// 	{ RE::ActorValue::kHeavyArmor, 104.0f },
		// 	{ RE::ActorValue::kLightArmor, 173.0f },
		// 	{ RE::ActorValue::kPickpocket, 37.0f },
		// 	{ RE::ActorValue::kSneak, 20.0f },
		// 	{ RE::ActorValue::kAlchemy, 506.0f },
		// 	{ RE::ActorValue::kSpeech, 1914.0f },
		// 	{ RE::ActorValue::kAlteration, 132.0f },
		// 	{ RE::ActorValue::kConjuration, 188.0f },
		// 	{ RE::ActorValue::kDestruction, 192.0f },
		// 	{ RE::ActorValue::kIllusion, 86.0f },
		// 	{ RE::ActorValue::kRestoration, 197.0f },
		// 	{ RE::ActorValue::kEnchanting, 0.00000003f },
		// };

		static inline std::unordered_map<RE::ActorValue, float> skillXPMultiplier = {
			{ RE::ActorValue::kOneHanded, 2.0f },
			{ RE::ActorValue::kTwoHanded, 2.0f },
			{ RE::ActorValue::kLockpicking, 0.25f },
			{ RE::ActorValue::kArchery, 2.0f },
			{ RE::ActorValue::kBlock, 2.0f },
			{ RE::ActorValue::kSmithing, 0.25f },
			{ RE::ActorValue::kHeavyArmor, 2.0f },
			{ RE::ActorValue::kLightArmor, 2.0f },
			{ RE::ActorValue::kPickpocket, 0.25f },
			{ RE::ActorValue::kSneak, 0.5f },
			{ RE::ActorValue::kAlchemy, 1.6f },
			{ RE::ActorValue::kSpeech, 2.0f },
			{ RE::ActorValue::kAlteration, 2.0f },
			{ RE::ActorValue::kConjuration, 2.0f },
			{ RE::ActorValue::kDestruction, 2.0f },
			{ RE::ActorValue::kIllusion, 2.0f },
			{ RE::ActorValue::kRestoration, 2.0f },
			{ RE::ActorValue::kEnchanting, 1.0f },
		};

		static inline std::unordered_map<RE::ActorValue, float> skillXPOffsets = {
			{ RE::ActorValue::kOneHanded, 0.0f },
			{ RE::ActorValue::kTwoHanded, 0.0f },
			{ RE::ActorValue::kLockpicking, 300.0f },
			{ RE::ActorValue::kArchery, 0.0f },
			{ RE::ActorValue::kBlock, 0.0f },
			{ RE::ActorValue::kSmithing, 300.0f },
			{ RE::ActorValue::kHeavyArmor, 0.0f },
			{ RE::ActorValue::kLightArmor, 0.0f },
			{ RE::ActorValue::kPickpocket, 250.0f },
			{ RE::ActorValue::kSneak, 120.0f },
			{ RE::ActorValue::kAlchemy, 65.0f },
			{ RE::ActorValue::kSpeech, 0.0f },
			{ RE::ActorValue::kAlteration, 0.0f },
			{ RE::ActorValue::kConjuration, 0.0f },
			{ RE::ActorValue::kDestruction, 0.0f },
			{ RE::ActorValue::kIllusion, 0.0f },
			{ RE::ActorValue::kRestoration, 0.0f },
			{ RE::ActorValue::kEnchanting, 170.0f },
		};
	};

	class Tab
	{
	public:
		enum ID
		{
			kSkills,
			kShouts,
			kCharacter,
			kProgression,
			kCombat,
			kWorld,
			kMisc
		};

	private:
		const std::string m_name;
		bool m_enabled;
		const ID m_id;
		const std::function<void(const ID&)> callback;

	public:
		Tab(const std::string a_name, bool a_enabled, const ID m_id, const std::function<void(const ID&)> callback) :
			m_name(a_name),
			m_enabled(a_enabled),
			m_id(m_id),
			callback(std::move(callback))
		{}

		void OnEnter() { callback(m_id); }
		void Disable() { m_enabled = false; }

		void Enable()
		{
			m_enabled = true;
			OnEnter();
		}

		bool IsEnabled() { return m_enabled; }
		ID GetID() const { return m_id; }

		[[nodiscard]] std::string GetName() const { return m_name; }
	};

	class CheatWindow : public PersistentData
	{
	private:
		static inline Tab* activeTab;

		static void EnableTab(Tab& a_tab)
		{
			for (auto& tab : tabs) {
				tab.Disable();
			}

			a_tab.Enable();
			activeTab = &a_tab;
		}

		static void OnEnterCallback(const Tab::ID& a_id);
		static inline Tab m_skills = Tab("Skills", false, Tab::ID::kSkills, OnEnterCallback);
		static inline Tab m_shouts = Tab("Shouts", false, Tab::ID::kShouts, OnEnterCallback);
		static inline Tab m_character = Tab("Character", false, Tab::ID::kCharacter, OnEnterCallback);
		static inline Tab m_progression = Tab("Progression", false, Tab::ID::kProgression, OnEnterCallback);
		static inline Tab m_combat = Tab("Combat", false, Tab::ID::kCombat, OnEnterCallback);
		static inline Tab m_world = Tab("World", false, Tab::ID::kWorld, OnEnterCallback);
		static inline Tab m_misc = Tab("Misc", false, Tab::ID::kMisc, OnEnterCallback);

		static inline std::vector<Tab> tabs = {
			m_skills,
			m_shouts,
			m_character,
			m_progression,
			m_combat,
			m_world,
			m_misc
		};

	public:
		static void Draw(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowSidebar(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowSubMenu(Settings::Style& a_style, Settings::Config& a_config);
		static void Init();

		static void ShowSkills();
		static void ShowShouts();
		static void ShowCharacter();
		static void ShowProgression();
		static void ShowCombat();
		static void ShowWorld();
		static void ShowMisc();

		static inline std::string iSelectedSkill = "None";
		static inline RE::ActorValue iSelectedSkillValue = RE::ActorValue::kTotal;
		static inline float iSelectedSkillSlider = 0.0f;

	private:
	};
}