#pragma once
#include <PCH.h>

// clang-format off

namespace Modex
{
	template <class TESObject>
	const char* ValidateTESName(const TESObject* a_object);
	std::string ValidateTESFileName(const RE::TESFile* a_file);

	// https://github.com/Nightfallstorm/DescriptionFramework | License GPL-3.0
	using _GetFormEditorID = const char* (*)(std::uint32_t);
	std::string po3_GetEditorID(RE::FormID a_formID);

	class BaseObject
	{
	private:
		RE::TESForm* 		TESForm;  // cannot be const
		
		const std::string 	name;
		const std::string 	editorid;
		const std::string 	plugin;
		const std::string 	formid;
		const RE::FormID 	baseid;

	public:
		BaseObject& operator=(const BaseObject& other)
		{
			if (this == &other) {
				return *this;
			}

			TESForm = other.TESForm;
			refID = other.refID;
			TableID = other.TableID;
			const_cast<std::string&>(name) = other.name;
			const_cast<std::string&>(editorid) = other.editorid;
			const_cast<std::string&>(plugin) = other.plugin;
			const_cast<std::string&>(formid) = other.formid;
			const_cast<RE::FormID&>(baseid) = other.baseid;
			return *this;
		}
	public:
		RE::FormID 			refID;
		ImGuiID 			TableID = 0;

		// I'm storing this kit "meta" data in the base object class because when I instantiate kit
		// items during sync, I need to reference kit specific information. Since the kit table is
		// a vector of ItemData and not KitItem, this additional data is required.

		// With this implementation, Kit data is only used in conversions to and from JSON really.
		// I instantiate ItemData objects for the Kit table view, and then manually update these
		// variables by doing a match against the KitItem editorID in the current TableList.

		// It's not a very efficient model, but I can't imagine Kit's being large enough to care.
		bool				kitEquipped = false;
		int					kitAmount = 1;

		// Initializer which stores key information about the object to avoid cache misses (?)
		// during SortColumns and other heavy operations. Speeds up iterations and sorting.
		BaseObject(RE::TESForm* form, ImGuiID a_id = 0) :
			TESForm{ form },
			TableID{ a_id },
			name{ form != nullptr ? ValidateTESName(form) : "NULL_ERR" },
			editorid{ form != nullptr ? po3_GetEditorID(form->GetFormID()) : "NULL_ERR" },
			formid{ form != nullptr ? fmt::format("{:08x}", form->GetFormID()) : "NULL_ERR" },
			baseid{ form != nullptr ? form->GetFormID() : 0 },
			refID{ 0 }
		{}

		virtual ~BaseObject() = default;
		
		// TODO: Derefencing char* without safety checks. Danger zone.
		[[nodiscard]] inline RE::TESForm* GetForm() const { return TESForm; }
		[[nodiscard]] inline const std::string GetFormID() const { return formid; }
		[[nodiscard]] inline const RE::FormID GetBaseForm() const { return baseid; }
		[[nodiscard]] inline const std::string GetEditorID() const { return editorid; }
		[[nodiscard]] inline const std::string_view GetEditorIDView() const { return editorid; }
		[[nodiscard]] inline const std::string GetName() const { return name; }
		[[nodiscard]] inline const std::string_view GetNameView() const { 
			if (!name.empty()) {
				return name;
			} else {
				return editorid;
			}
		}

		[[nodiscard]] inline const RE::TESFile* GetPlugin(int32_t a_idx = 0) const
		{
			return SafeAccess<const RE::TESFile*>(&RE::TESForm::GetFile, a_idx, nullptr);
		}

		[[nodiscard]] inline const std::string GetPluginName(int32_t a_idx = 0) const
		{
			return ValidateTESFileName(GetPlugin(a_idx));
		}
		[[nodiscard]] inline const std::string_view GetPluginNameView(int32_t a_idx = 0) const
		{
			return ValidateTESFileName(GetPlugin(a_idx));
		}

		// Categorize SoulGem's as Misc.
		[[nodiscard]] inline const RE::FormType GetFormType() const
		{
			if (GetForm()->GetFormType() == RE::FormType::SoulGem) {
				return RE::FormType::Misc;
			} else {
				return SafeAccess<RE::FormType>(&RE::TESForm::GetFormType, RE::FormType::None);
			}
		}

		[[nodiscard]] inline const std::string GetTypeName() const
		{
			if (GetForm() == nullptr) {
				return "NULL_ERR";
			}

			return static_cast<std::string>(RE::FormTypeToString(GetFormType()));
		}

	protected:
		template <typename ReturnType, typename Func, typename Default>
		[[nodiscard]] inline ReturnType SafeAccess(Func func, Default defaultValue) const
		{
			if (TESForm == nullptr) {
				return defaultValue;
			}
			return (TESForm->*func)();
		}

		template <typename ReturnType, typename Arg, typename Func, typename Default>
		[[nodiscard]] inline ReturnType SafeAccess(Func func, Arg a_arg, Default defaultValue) const
		{
			if (TESForm == nullptr) {
				return defaultValue;
			}
			return (TESForm->*func)(a_arg);
		}

		template <typename ReturnType, typename Func, typename Default, typename Transform>
		[[nodiscard]] inline auto SafeAccess(Func func, Default defaultValue, Transform transform) const -> decltype(transform((TESForm->*func)()))
		{
			if (TESForm == nullptr) {
				return defaultValue;
			}
			return transform((TESForm->*func)());
		}
	};

	class ObjectData : public BaseObject
	{
	public:
		ObjectData(RE::TESForm* a_form, ImGuiID a_id = 0) :
			BaseObject{ a_form, a_id } {}


	};

	class ItemData : public BaseObject
	{
	public:
		ItemData(RE::TESForm* a_form, ImGuiID a_id = 0) :
			BaseObject{ a_form, a_id } {}

		[[nodiscard]] inline float GetWeight() const
		{
			return SafeAccess<float>(&RE::TESForm::GetWeight, 0.0f);
		}

		[[nodiscard]] inline std::int32_t GetValue() const
		{
			return SafeAccess<std::int32_t>(&RE::TESForm::GetGoldValue, 0);
		}

		[[nodiscard]] inline std::string GetValueString() const
		{
			return fmt::format("{:d}", GetForm()->GetGoldValue());
		}

		// TODO: This is only working for weapons, not armors or other items.
		[[nodiscard]] inline bool IsNonPlayable() const
		{
			const auto formType = GetForm()->GetFormType();
			switch (formType) {
			case RE::FormType::Weapon:
				return GetForm()->As<RE::TESObjectWEAP>()->weaponData.flags.any(RE::TESObjectWEAP::Data::Flag::kNonPlayable);
			case RE::FormType::Armor:
				return GetForm()->As<RE::TESObjectARMO>()->GetFormFlags() & RE::TESObjectARMO::RecordFlags::kNonPlayable;
			default:
				return false;
			};
		}
	};

	class NPCData : public BaseObject
	{
	public:
		NPCData(RE::TESForm* a_form, ImGuiID a_id = 0) :
			BaseObject{ a_form, a_id } {}


		[[nodiscard]] inline RE::TESNPC* GetTESNPC() const
		{
			try {
				return GetForm()->As<RE::TESNPC>();
			} catch (const std::exception& e) {
				stl::report_and_fail(std::string("Failed to capture TESForm as TESNPC reference!\n") + e.what());
			}
		}
		[[nodiscard]] inline const std::string GetClass() const { return ValidateTESName(GetTESNPC()->npcClass); }
		[[nodiscard]] inline const std::string GetRace() const { return ValidateTESName(GetTESNPC()->race); }
		[[nodiscard]] inline const std::string GetGender() const { return GetTESNPC()->IsFemale() ? "Female" : "Male"; }
		[[nodiscard]] inline const uint16_t GetLevel() const { return GetTESNPC()->GetLevel(); }

		[[nodiscard]] inline float GetHealth() const
		{
			return SafeAccessNPC<float>(&RE::TESNPC::GetBaseActorValue, RE::ActorValue::kHealth, 0.0f);
		}

		[[nodiscard]] inline float GetMagicka() const
		{
			return SafeAccessNPC<float>(&RE::TESNPC::GetBaseActorValue, RE::ActorValue::kMagicka, 0.0f);
		}
		[[nodiscard]] inline float GetStamina() const
		{
			return SafeAccessNPC<float>(&RE::TESNPC::GetBaseActorValue, RE::ActorValue::kStamina, 0.0f);
		}

		// [[nodiscard]] inline float GetCarryWeight() const
		// {
		// 	return SafeAccessNPC<float>(&RE::TESNPC::GetBaseActorValue, RE::ActorValue::kCarryWeight, 0.0f);
		// }

		[[nodiscard]] inline RE::TESNPC::Skills GetSkills() const
		{
			return GetTESNPC()->playerSkills;
		}

		[[nodiscard]] inline RE::BSTArray<RE::FACTION_RANK> GetFactions() const
		{
			if (!GetTESNPC()->factions.empty()) {
				return GetTESNPC()->factions;
			} else {
				return {};
			}
		}

	protected:
		template <typename ReturnType, typename Arg, typename Func, typename Default>
		[[nodiscard]] inline ReturnType SafeAccessNPC(Func func, Arg a_arg, Default defaultValue) const
		{
			if (GetTESNPC() == nullptr) {
				return defaultValue;
			}
			return (GetTESNPC()->*func)(a_arg);
		}
	};

	// TODO: Pickup refactoring above changes past here.

	class CellData
	{
	public:
		const std::string 	filename;
		const std::string 	space;
		const std::string 	zone;
		const std::string 	cellName;
		const std::string 	editorid;

		const RE::TESFile* 	mod;

		[[nodiscard]] inline std::string_view GetPluginName() const { return filename; }
		[[nodiscard]] inline std::string_view GetPluginNameView() const { return filename; }
		[[nodiscard]] inline std::string_view GetSpace() const { return space; }
		[[nodiscard]] inline std::string_view GetZone() const { return zone; }
		[[nodiscard]] inline std::string_view GetCellName() const { return cellName; }
		[[nodiscard]] inline std::string_view GetEditorIDView() const { return editorid; }
		[[nodiscard]] inline std::string_view GetEditorID() const { return editorid; }  // TODO: separate these.

		CellData(std::string filename, std::string space, std::string zone, std::string cellName, std::string editorid, const RE::TESFile* mod = nullptr) :
			filename(filename), 
			space(space),
			 zone(zone), 
			 cellName(cellName), 
			 editorid(editorid), 
			 mod(mod) 
		{}
	};

	struct KitBase
	{
		std::string  	plugin;
		std::string 	name;
		std::string 	editorid;

		ImGuiID 		TableID;
	};

	struct KitItem : KitBase
	{
		int					amount;
		bool				equipped;
		ItemData*			ref;

		// Custom comparator for equality based on editorid
		bool operator==(const KitItem& other) const
		{
			return this->editorid == other.editorid;
		}
	};

	struct KitPerk : KitBase
	{
		int					rank;
	};

	struct KitSpell : KitBase // unused
	{
	};

	struct KitShout : KitBase // unused
	{
	};

	class Kit
	{
	public:
		// serialized
		std::string 				name; // unique?
		std::string					collection;
		std::string					desc;
		bool						clearEquipped;
		bool						readOnly;

    	std::unordered_set<std::shared_ptr<KitItem>> items;
    	// std::unordered_set<std::shared_ptr<KitPerk>> perks;
    	// std::unordered_set<std::shared_ptr<KitSpell>> spells;
    	// std::unordered_set<std::shared_ptr<KitShout>> shouts;

		// not serialized - meta data
		ImGuiID 					TableID;
		int							weaponCount;
		int							armorCount;
		int							miscCount;


		// constructor
		Kit(std::string name, ImGuiID id) : name(name), TableID(id) {}
	};

	template <typename DataType>
	static std::shared_ptr<KitItem> CreateKitItem(const DataType& a_item)
	{
		auto new_item = std::make_shared<KitItem>();
		new_item->plugin = a_item.GetPluginName();
		new_item->name = a_item.GetName();
		new_item->editorid = a_item.GetEditorID();
		new_item->amount = a_item.kitAmount;
		new_item->equipped = a_item.kitEquipped;

		return new_item;
	}
}