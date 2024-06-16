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

	class BaseItem
	{
	public:
		RE::TESForm* TESForm;  // cannot be const
		const RE::FormID FormID;
		const RE::TESFile* TESFile;

		bool favorite = false;
		bool selected = false;

		[[nodiscard]] inline std::string GetName() const { return TESForm->GetName(); }
		[[nodiscard]] inline RE::FormID GetBaseForm() const { return FormID; }
		[[nodiscard]] inline std::string GetFormID() const { return fmt::format("{:08x}", TESForm->GetFormID()); }
		[[nodiscard]] inline std::string GetEditorID() const { return GetEditorIDString(TESForm->GetFormID()); }
		[[nodiscard]] inline std::string GetPluginName() const { return TESFile->fileName; }
		[[nodiscard]] inline RE::FormType GetFormType() const { return TESForm->GetFormType(); }
		[[nodiscard]] inline bool IsFavorite() const { return favorite; }
		[[nodiscard]] inline bool IsSelected() const { return selected; }

		[[nodiscard]] inline std::string GetTypeName() const
		{
			return static_cast<std::string>(RE::FormTypeToString(TESForm->GetFormType()));
		}
	};

	class Item : public BaseItem
	{
	public:
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