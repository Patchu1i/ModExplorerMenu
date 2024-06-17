#include "AddItem.h"
#include "Utils/Util.h"

namespace ModExplorerMenu
{
	int CompareWeaponDamage(RE::TESObjectWEAP* a_lhs, RE::TESObjectWEAP* a_rhs)
	{
		auto baseDamage1 = Utils::CalcBaseDamage(a_lhs);
		auto baseDamage2 = Utils::CalcBaseDamage(a_rhs);
		return (baseDamage1 < baseDamage2) ? -1 : (baseDamage1 > baseDamage2) ? 1 :
		                                                                        0;
	}

	int CompareWeaponSpeed(RE::TESObjectWEAP* a_lhs, RE::TESObjectWEAP* a_rhs)
	{
		auto speed1 = a_lhs->weaponData.speed;
		auto speed2 = a_rhs->weaponData.speed;
		return (speed1 < speed2) ? -1 : (speed1 > speed2) ? 1 :
		                                                    0;
	}

	int CompareWeaponDPS(RE::TESObjectWEAP* a_lhs, RE::TESObjectWEAP* a_rhs)
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

	int CompareCritDamage(RE::TESObjectWEAP* a_lhs, RE::TESObjectWEAP* a_rhs)
	{
		auto critDamage1 = a_lhs->GetCritDamage();
		auto critDamage2 = a_rhs->GetCritDamage();
		return (critDamage1 < critDamage2) ? -1 : (critDamage1 > critDamage2) ? 1 :
		                                                                        0;
	}

	int CompareArmorRating(RE::TESObjectARMO* a_lhs, RE::TESObjectARMO* a_rhs)
	{
		auto armorRating1 = a_lhs->armorRating;
		auto armorRating2 = a_rhs->armorRating;
		return (armorRating1 < armorRating2) ? -1 : (armorRating1 > armorRating2) ? 1 :
		                                                                            0;
	}

	int CompareSkill(RE::TESForm* a_lhs, RE::TESForm* a_rhs)
	{
		auto item1 = a_lhs->GetObjectTypeName();
		auto item2 = a_rhs->GetObjectTypeName();
		return strcmp(item1, item2);
	}

	bool AddItemWindow::SortColumn(const Item* v1, const Item* v2)
	{
		// clang-format off
		const ImGuiTableSortSpecs* sort_specs = s_current_sort_specs;
		const ImGuiID ID = sort_specs->Specs->ColumnUserID;

		int delta = 0;

		switch (ID) {
		case BaseColumn::ID::Favorite:
			delta = (v1->favorite < v2->favorite) ? -1 : (v1->favorite > v2->favorite) ? 1 : 0;
			break;
        case BaseColumn::ID::Plugin:
            delta = v1->filename.compare(v2->filename);
            break;
		case BaseColumn::ID::Type:
			delta = v1->GetTypeName().compare(v2->GetTypeName());
			break;
		case BaseColumn::ID::FormID:
            delta = (v1->FormID < v2->FormID) ? -1 : (v1->FormID > v2->FormID) ? 1 : 0;
			//delta = v1->GetFormID().compare(v2->GetFormID());
			break;
		case BaseColumn::ID::Name:
			//delta = v1->GetName().compare(v2->GetName());
            delta = v1->name.compare(v2->name);
            break;
		case BaseColumn::ID::EditorID:
			//delta = v1->GetEditorID().compare(v2->GetEditorID());
            delta = v1->editorid.compare(v2->editorid);
            break;
		case BaseColumn::ID::GoldValue:
            delta = (v1->value < v2->value) ? -1 : (v1->value > v2->value) ? 1 : 0;
            break;
		case BaseColumn::ID::Weight:
            delta = (v1->weight < v2->weight) ? -1 : (v1->weight > v2->weight) ? 1 : 0;
			break;
		case BaseColumn::ID::BaseDamage:
			if (v1->GetFormType() == RE::FormType::Weapon && v2->GetFormType() == RE::FormType::Weapon) {
				delta = CompareWeaponDamage(v1->TESForm->As<RE::TESObjectWEAP>(), v2->TESForm->As<RE::TESObjectWEAP>());
			} else if (v1->GetFormType() == RE::FormType::Weapon) {
				delta = 1;
			} else if (v2->GetFormType() == RE::FormType::Weapon) {
				delta = -1;
			}
			break;
		case BaseColumn::ID::Speed:
			if (v1->GetFormType() == RE::FormType::Weapon && v2->GetFormType() == RE::FormType::Weapon) {
				delta = CompareWeaponSpeed(v1->TESForm->As<RE::TESObjectWEAP>(), v2->TESForm->As<RE::TESObjectWEAP>());
			} else if (v1->GetFormType() == RE::FormType::Weapon) {
				delta = 1;
			} else if (v2->GetFormType() == RE::FormType::Weapon) {
				delta = -1;
			}
			break;
		case BaseColumn::ID::CritDamage:
			if (v1->GetFormType() == RE::FormType::Weapon && v2->GetFormType() == RE::FormType::Weapon) {
				delta = CompareCritDamage(v1->TESForm->As<RE::TESObjectWEAP>(), v2->TESForm->As<RE::TESObjectWEAP>());
			} else if (v1->GetFormType() == RE::FormType::Weapon) {
				delta = 1;
			} else if (v2->GetFormType() == RE::FormType::Weapon) {
				delta = -1;
			}
			break;
		case BaseColumn::ID::DPS:
			if (v1->GetFormType() == RE::FormType::Weapon && v2->GetFormType() == RE::FormType::Weapon) {
				delta = CompareWeaponDPS(v1->TESForm->As<RE::TESObjectWEAP>(), v2->TESForm->As<RE::TESObjectWEAP>());
			} else if (v1->GetFormType() == RE::FormType::Weapon) {
				delta = 1;
			} else if (v2->GetFormType() == RE::FormType::Weapon) {
				delta = -1;
			}
			break;
		case BaseColumn::ID::ArmorRating:
			if (v1->GetFormType() == RE::FormType::Armor && v2->GetFormType() == RE::FormType::Armor) {
				delta = CompareArmorRating(v1->TESForm->As<RE::TESObjectARMO>(), v2->TESForm->As<RE::TESObjectARMO>());
			} else if (v1->GetFormType() == RE::FormType::Armor) {
				delta = 1;
			} else if (v2->GetFormType() == RE::FormType::Armor) {
				delta = -1;
			}
            break;
		case BaseColumn::ID::Skill:
			if ((v1->GetFormType() == RE::FormType::Weapon && v2->GetFormType() == RE::FormType::Weapon) ||
				(v1->GetFormType() == RE::FormType::Armor && v2->GetFormType() == RE::FormType::Armor) ||
				(v1->GetFormType() == RE::FormType::Weapon && v2->GetFormType() == RE::FormType::Armor) ||
				(v1->GetFormType() == RE::FormType::Armor && v2->GetFormType() == RE::FormType::Weapon)) {
				delta = CompareSkill(v1->TESForm, v2->TESForm);
			} else if (v1->GetFormType() == RE::FormType::Weapon || v1->GetFormType() == RE::FormType::Armor) {
				delta = 1;
			} else if (v2->GetFormType() == RE::FormType::Weapon || v2->GetFormType() == RE::FormType::Armor) {
				delta = -1;
            }
			break;
		default:
			break;
		}

		if (delta > 0)
			return (sort_specs->Specs->SortDirection == ImGuiSortDirection_Ascending) ? false : true;
		if (delta < 0)
			return (sort_specs->Specs->SortDirection == ImGuiSortDirection_Ascending) ? true : false;

		return false;  // prevent assertion (?)
		// clang-format on
	}

	// Sorts the columns of referenced list by sort_specs
	void AddItemWindow::SortColumnsWithSpecs(ImGuiTableSortSpecs* sort_specs)
	{
		s_current_sort_specs = sort_specs;
		if (itemList.size() > 1)
			std::sort(itemList.begin(), itemList.end(), SortColumn);
		s_current_sort_specs = NULL;
	}
}