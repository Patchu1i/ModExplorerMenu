#pragma once
#include "include/U/Util.h"
#include <PCH.h>

namespace Modex
{
	class ISortable
	{
	private:
		static int CompareWeaponDamage(RE::TESObjectWEAP* a_lhs, RE::TESObjectWEAP* a_rhs)
		{
			auto baseDamage1 = Utils::CalcBaseDamage(a_lhs);
			auto baseDamage2 = Utils::CalcBaseDamage(a_rhs);
			return (baseDamage1 < baseDamage2) ? -1 : (baseDamage1 > baseDamage2) ? 1 :
			                                                                        0;
		}

		static int CompareWeaponSpeed(RE::TESObjectWEAP* a_lhs, RE::TESObjectWEAP* a_rhs)
		{
			auto speed1 = a_lhs->weaponData.speed;
			auto speed2 = a_rhs->weaponData.speed;
			return (speed1 < speed2) ? -1 : (speed1 > speed2) ? 1 :
			                                                    0;
		}

		static int CompareArmorSlot(RE::TESObjectARMO* a_lhs, RE::TESObjectARMO* a_rhs)
		{
			auto slot1 = Utils::GetArmorSlots(a_lhs);
			auto slot2 = Utils::GetArmorSlots(a_rhs);

			if (slot1.empty() && slot2.empty()) {
				return 0;
			} else if (slot1.empty()) {
				return -1;
			} else if (slot2.empty()) {
				return 1;
			}

			return slot1[0].compare(slot2[0]);
		}

		static int CompareWeaponDPS(RE::TESObjectWEAP* a_lhs, RE::TESObjectWEAP* a_rhs)
		{
			auto baseDamage1 = Utils::CalcBaseDamage(a_lhs);
			auto baseDamage2 = Utils::CalcBaseDamage(a_rhs);
			auto speed1 = a_lhs->weaponData.speed;
			auto speed2 = a_rhs->weaponData.speed;
			auto dps1 = (int)(baseDamage1 * speed1);
			auto dps2 = (int)(baseDamage2 * speed2);
			return (dps1 < dps2) ? -1 : (dps1 > dps2) ? 1 :
			                                            0;
		}

		static int CompareCritDamage(RE::TESObjectWEAP* a_lhs, RE::TESObjectWEAP* a_rhs)
		{
			auto critDamage1 = a_lhs->GetCritDamage();
			auto critDamage2 = a_rhs->GetCritDamage();
			return (critDamage1 < critDamage2) ? -1 : (critDamage1 > critDamage2) ? 1 :
			                                                                        0;
		}

		static int CompareArmorRating(RE::TESObjectARMO* a_lhs, RE::TESObjectARMO* a_rhs)
		{
			auto armorRating1 = a_lhs->armorRating;
			auto armorRating2 = a_rhs->armorRating;
			return (armorRating1 < armorRating2) ? -1 : (armorRating1 > armorRating2) ? 1 :
			                                                                            0;
		}

		static int CompareSkill(RE::TESForm* a_lhs, RE::TESForm* a_rhs)
		{
			auto item1 = a_lhs->GetObjectTypeName();
			auto item2 = a_rhs->GetObjectTypeName();
			return strcmp(item1, item2);
		}

	public:
		static inline ImGuiTableSortSpecs* s_current_sort_specs;

		// TODO: A lot of this has repeating behavior. Could probably be refactored by checking
		// lhs and rhs types and then comparing them based on type instead of Object.

		template <class Object>
		inline static bool SortColumns(const Object* lhs, const Object* rhs)
		{
			const ImGuiTableSortSpecs* sort_specs = s_current_sort_specs;
			const ImGuiID ID = sort_specs->Specs->ColumnUserID;

			int delta = 0;

			switch (ID) {
			case BaseColumn::ID::Favorite:
				delta = (lhs->favorite < rhs->favorite) ? -1 : (lhs->favorite > rhs->favorite) ? 1 :
				                                                                                 0;
				break;
			case BaseColumn::ID::EditorID:
				delta = lhs->GetEditorIDView().compare(rhs->GetEditorIDView());
				break;
			case BaseColumn::ID::Plugin:
				delta = lhs->GetPluginNameView().compare(rhs->GetPluginNameView());
				break;
			case BaseColumn::ID::Type:
				if constexpr (!std::is_base_of<BaseObject, Object>::value)
					break;
				else
					delta = lhs->GetTypeName().compare(rhs->GetTypeName());
				break;
			case BaseColumn::ID::FormID:
				if constexpr (!std::is_base_of<BaseObject, Object>::value)
					break;
				else
					delta = (lhs->GetBaseForm() < rhs->GetBaseForm()) ? -1 : (lhs->GetBaseForm() > rhs->GetBaseForm()) ? 1 :
					                                                                                                     0;
				break;
			case BaseColumn::ID::ReferenceID:
				if constexpr (!std::is_base_of<BaseObject, Object>::value)
					break;
				else
					delta = (lhs->refID < rhs->refID) ? -1 : (lhs->refID > rhs->refID) ? 1 :
					                                                                     0;
				break;
			case BaseColumn::ID::Name:
				if constexpr (!std::is_base_of<BaseObject, Object>::value)
					break;
				else
					delta = lhs->GetNameView().compare(rhs->GetNameView());
				break;
			case BaseColumn::ID::Class:
				if constexpr (!std::is_same<Object, NPCData>::value) {
					break;
				} else {
					std::string_view lhsClass = lhs->GetClass();
					std::string_view rhsClass = rhs->GetClass();

					delta = lhsClass.compare(rhsClass);
					break;
				}
			case BaseColumn::ID::Gender:
				if constexpr (!std::is_same<Object, NPCData>::value) {
					break;
				} else {
					std::string_view lhsGender = lhs->GetGender();
					std::string_view rhsGender = rhs->GetGender();
					delta = lhsGender.compare(rhsGender);
					break;
				}
			case BaseColumn::ID::Race:
				if constexpr (!std::is_same<Object, NPCData>::value) {
					break;
				} else {
					std::string_view lhsRace = lhs->GetRace();
					std::string_view rhsRace = rhs->GetRace();

					delta = lhsRace.compare(rhsRace);
					break;
				}
			case BaseColumn::ID::Level:
				if constexpr (!std::is_same<Object, NPCData>::value) {
					break;
				} else {
					auto lhsLevel = lhs->GetLevel();
					auto rhsLevel = rhs->GetLevel();
					delta = (lhsLevel < rhsLevel) ? -1 : (lhsLevel > rhsLevel) ? 1 :
					                                                             0;
					break;
				}
			case BaseColumn::ID::Health:
				if constexpr (!std::is_same<Object, NPCData>::value)
					break;
				else
					delta = (lhs->GetHealth() < rhs->GetHealth()) ? -1 : (lhs->GetHealth() > rhs->GetHealth()) ? 1 :
					                                                                                             0;
				break;
			case BaseColumn::ID::Magicka:
				if constexpr (!std::is_same<Object, NPCData>::value)
					break;
				else
					delta = (lhs->GetMagicka() < rhs->GetMagicka()) ? -1 : (lhs->GetMagicka() > rhs->GetMagicka()) ? 1 :
					                                                                                                 0;
				break;
			case BaseColumn::ID::Stamina:
				if constexpr (!std::is_same<Object, NPCData>::value)
					break;
				else
					delta = (lhs->GetStamina() < rhs->GetStamina()) ? -1 : (lhs->GetStamina() > rhs->GetStamina()) ? 1 :
					                                                                                                 0;
				break;
			case BaseColumn::ID::CarryWeight:
				if constexpr (!std::is_same<Object, NPCData>::value)
					break;
				else
					delta = (lhs->GetCarryWeight() < rhs->GetCarryWeight()) ? -1 : (lhs->GetCarryWeight() > rhs->GetCarryWeight()) ? 1 :
					                                                                                                                 0;
				break;
			case BaseColumn::ID::GoldValue:
				if constexpr (!std::is_same<Object, ItemData>::value)
					break;
				else
					delta = (lhs->GetValue() < rhs->GetValue()) ? -1 : (lhs->GetValue() > rhs->GetValue()) ? 1 :
					                                                                                         0;
				break;
			case BaseColumn::ID::Space:
				if constexpr (!std::is_same<Object, CellData>::value)
					break;
				else
					delta = lhs->space.compare(rhs->space);
				break;
			case BaseColumn::ID::Zone:
				if constexpr (!std::is_same<Object, CellData>::value)
					break;
				else
					delta = lhs->zone.compare(rhs->zone);
				break;
			case BaseColumn::ID::CellName:
				if constexpr (!std::is_same<Object, CellData>::value)
					break;
				else
					delta = lhs->cellName.compare(rhs->cellName);
				break;
			case BaseColumn::ID::BaseDamage:
				if constexpr (!std::is_same<Object, ItemData>::value) {
					break;
				} else {
					if (lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Weapon) {
						delta = CompareWeaponDamage(lhs->GetForm()->As<RE::TESObjectWEAP>(), rhs->GetForm()->As<RE::TESObjectWEAP>());
					} else if (lhs->GetFormType() == RE::FormType::Weapon) {
						delta = 1;
					} else if (rhs->GetFormType() == RE::FormType::Weapon) {
						delta = -1;
					}

					break;
				}
			case BaseColumn::ID::ArmorRating:
				if constexpr (!std::is_same<Object, ItemData>::value) {
					break;
				} else {
					if (lhs->GetFormType() == RE::FormType::Armor && rhs->GetFormType() == RE::FormType::Armor) {
						delta = CompareArmorRating(lhs->GetForm()->As<RE::TESObjectARMO>(), rhs->GetForm()->As<RE::TESObjectARMO>());
					} else if (lhs->GetFormType() == RE::FormType::Armor) {
						delta = 1;
					} else if (rhs->GetFormType() == RE::FormType::Armor) {
						delta = -1;
					}

					break;
				}
			case BaseColumn::ID::Slot:
				if constexpr (!std::is_same<Object, ItemData>::value) {
					break;
				} else {
					if (lhs->GetFormType() == RE::FormType::Armor && rhs->GetFormType() == RE::FormType::Armor) {
						delta = CompareArmorSlot(lhs->GetForm()->As<RE::TESObjectARMO>(), rhs->GetForm()->As<RE::TESObjectARMO>());
					} else if (lhs->GetFormType() == RE::FormType::Armor) {
						delta = 1;
					} else if (rhs->GetFormType() == RE::FormType::Armor) {
						delta = -1;
					}
					break;
				}
			case BaseColumn::ID::Speed:
				if constexpr (!std::is_same<Object, ItemData>::value) {
					break;
				} else {
					if (lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Weapon) {
						delta = CompareWeaponSpeed(lhs->GetForm()->As<RE::TESObjectWEAP>(), rhs->GetForm()->As<RE::TESObjectWEAP>());
					} else if (lhs->GetFormType() == RE::FormType::Weapon) {
						delta = 1;
					} else if (rhs->GetFormType() == RE::FormType::Weapon) {
						delta = -1;
					}

					break;
				}
			case BaseColumn::ID::CritDamage:
				if constexpr (!std::is_same<Object, ItemData>::value) {
					break;
				} else {
					if (lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Weapon) {
						delta = CompareCritDamage(lhs->GetForm()->As<RE::TESObjectWEAP>(), rhs->GetForm()->As<RE::TESObjectWEAP>());
					} else if (lhs->GetFormType() == RE::FormType::Weapon) {
						delta = 1;
					} else if (rhs->GetFormType() == RE::FormType::Weapon) {
						delta = -1;
					}

					break;
				}
			case BaseColumn::ID::Weight:
				if constexpr (!std::is_same<Object, ItemData>::value) {
					break;
				} else {
					delta = (lhs->GetWeight() < rhs->GetWeight()) ? -1 : (lhs->GetWeight() > rhs->GetWeight()) ? 1 :
					                                                                                             0;
					break;
				}
			case BaseColumn::ID::DPS:
				if constexpr (!std::is_same<Object, ItemData>::value) {
					break;
				} else {
					if (lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Weapon) {
						delta = CompareWeaponDPS(lhs->GetForm()->As<RE::TESObjectWEAP>(), rhs->GetForm()->As<RE::TESObjectWEAP>());
					} else if (lhs->GetFormType() == RE::FormType::Weapon) {
						delta = 1;
					} else if (rhs->GetFormType() == RE::FormType::Weapon) {
						delta = -1;
					}

					break;
				}
			case BaseColumn::ID::Skill:
				if constexpr (!std::is_same<Object, ItemData>::value) {
					break;
				} else {
					if ((lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Weapon) ||
						(lhs->GetFormType() == RE::FormType::Armor && rhs->GetFormType() == RE::FormType::Armor) ||
						(lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Armor) ||
						(lhs->GetFormType() == RE::FormType::Armor && rhs->GetFormType() == RE::FormType::Weapon)) {
						delta = CompareSkill(lhs->GetForm(), rhs->GetForm());
					} else if (lhs->GetFormType() == RE::FormType::Weapon || lhs->GetFormType() == RE::FormType::Armor) {
						delta = 1;
					} else if (rhs->GetFormType() == RE::FormType::Weapon || rhs->GetFormType() == RE::FormType::Armor) {
						delta = -1;
					}

					break;
				}
			}

			if (delta > 0)
				return (sort_specs->Specs->SortDirection == ImGuiSortDirection_Ascending) ? false : true;
			if (delta < 0)
				return (sort_specs->Specs->SortDirection == ImGuiSortDirection_Ascending) ? true : false;

			return false;
		}

		template <class List, class Object>
		inline static void SortColumnsWithSpecs(List& a_list, ImGuiTableSortSpecs* sort_specs)
		{
			s_current_sort_specs = sort_specs;
			if (a_list.size() > 1)
				std::sort(a_list.begin(), a_list.end(), [](const Object* a, const Object* b) {
					return SortColumns<Object>(a, b);
				});
			s_current_sort_specs = NULL;
		}
	};
}