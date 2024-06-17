#pragma once

namespace ModExplorerMenu
{
	// https://github.com/Nightfallstorm/DescriptionFramework | License GPL-3.0
	using _GetFormEditorID = const char* (*)(std::uint32_t);

	inline std::string GetEditorIDString(RE::FormID a_formID)
	{
		static auto tweaks = GetModuleHandleA("po3_Tweaks");
		static auto function = reinterpret_cast<_GetFormEditorID>(GetProcAddress(tweaks, "GetFormEditorID"));
		if (function) {
			return function(a_formID);
		}
		return {};
	}

	class Base
	{
	public:
		RE::TESForm* TESForm;  // cannot be const
		const RE::FormID FormID;
		const RE::TESFile* TESFile;

		const std::string name = TESForm->GetName();
		const std::string editorid = GetEditorIDString(FormID);
		const std::string filename = TESFile->fileName;

		bool favorite = false;
		bool selected = false;

		[[nodiscard]] inline std::string GetFormID() const { return fmt::format("{:08x}", TESForm->GetFormID()); }
		[[nodiscard]] inline std::string_view GetName() const { return name; }
		[[nodiscard]] inline std::string_view GetEditorID() const { return editorid; }
		[[nodiscard]] inline std::string_view GetPluginName() const { return filename; }
		[[nodiscard]] inline RE::FormID GetBaseForm() const { return FormID; }
		[[nodiscard]] inline RE::FormType GetFormType() const { return TESForm->GetFormType(); }
		[[nodiscard]] inline bool IsFavorite() const { return favorite; }
		[[nodiscard]] inline bool IsSelected() const { return selected; }

		[[nodiscard]] inline std::string GetTypeName() const
		{
			return static_cast<std::string>(RE::FormTypeToString(TESForm->GetFormType()));
		}
	};

	class Item : public Base
	{
	public:
		const int32_t value = TESForm->GetGoldValue();
		const float weight = TESForm->GetWeight();

		[[nodiscard]] inline float GetWeight() const { return TESForm->GetWeight(); }
		[[nodiscard]] inline std::int32_t GetValue() const { return TESForm->GetGoldValue(); }

		[[nodiscard]] inline std::string GetValueString() const
		{
			return fmt::format("{:d}", TESForm->GetGoldValue());
		}

		[[nodiscard]] inline bool IsNonPlayable() const
		{
			const auto formType = TESForm->GetFormType();
			switch (formType) {
			case RE::FormType::Weapon:
				return TESForm->As<RE::TESObjectWEAP>()->weaponData.flags.any(RE::TESObjectWEAP::Data::Flag::kNonPlayable);
			default:
				return false;
			};
		}
	};

	class NPC : public Base
	{
	public:
		[[nodiscard]] inline float GetHealth() const { return TESForm->As<RE::TESNPC>()->GetBaseActorValue(RE::ActorValue::kHealth); }
		[[nodiscard]] inline float GetMagicka() const { return TESForm->As<RE::TESNPC>()->GetBaseActorValue(RE::ActorValue::kMagicka); }
		[[nodiscard]] inline float GetStamina() const { return TESForm->As<RE::TESNPC>()->GetBaseActorValue(RE::ActorValue::kStamina); }
		[[nodiscard]] inline float GetCarryWeight() const { return TESForm->As<RE::TESNPC>()->GetBaseActorValue(RE::ActorValue::kCarryWeight); }

		[[nodiscard]] inline RE::TESNPC::Skills GetSkills() const { return TESForm->As<RE::TESNPC>()->playerSkills; }
	};

	// 	RE::TESForm* form;
	// RE::FormID refID;
	// std::string plugin;
	// std::string name;
	// std::string formid;
	// std::string editorid;

	// float health;
	// float magicka;
	// float stamina;
	// float carryweight;

	// RE::TESNPC::Skills skills;

	// bool favorite;

	class Cell
	{
	public:
		std::string plugin;
		std::string space;
		std::string zone;
		std::string fullName;
		std::string editorid;
		uint32_t cellid;
		bool favorite = false;
		const RE::TESFile* mod;

		Cell(std::string plugin, std::string space, std::string zone, std::string fullName, std::string editorid, const RE::TESFile* mod = nullptr) :
			plugin(plugin), space(space), zone(zone), fullName(fullName), editorid(editorid), mod(mod) {}
	};
}