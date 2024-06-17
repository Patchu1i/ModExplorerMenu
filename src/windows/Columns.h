#pragma once
#include "Objects.h"
#include "Settings.h"
#include "Utils/Util.h"
#include <PCH.h>

namespace ModExplorerMenu
{
	// clang-format off
	class BaseColumn
	{
	public:
		std::string name;
		ImGuiTableColumnFlags flags;

		float width;
		bool* enabled;

		enum ID
		{
			Favorite,
			Plugin,
			Type,
			FormID,
			Name,
			EditorID,
            Health,
            Magicka,
            Stamina,
            CarryWeight,
			GoldValue,
			BaseDamage,
			ArmorRating,
			Speed,
			CritDamage,
			Skill,
			Weight,
			DPS
		};

        ID key;
	};

    class BaseColumnList
    {
    public:
        std::vector<BaseColumn> columns;

        bool IsColumnEnabled(BaseColumn::ID a_id)
        {
            for (auto& column : columns) {
                if (column.key == a_id) {
                    return *column.enabled;
                }
            }
            return false;
        }

        bool IsAllColumnsDisabled()
        {
            for (auto& column : columns) {
                if (*column.enabled) {
                    return false;
                }
            }
            return true;
        }

        inline int GetTotalColumns() { return static_cast<int>(columns.size()); }
    };

    class NPCColumns : public BaseColumnList
    {
    public:
        NPCColumns(
            bool* showFavorite = nullptr, bool* showPlugin = nullptr, bool* showFormID = nullptr, bool* showName = nullptr, bool* showEditorID = nullptr,
            bool* showHealth = nullptr, bool* showMagicka = nullptr, bool* showStamina = nullptr, bool* showCarryWeight = nullptr)
        {
            columns.push_back({ "*", ImGuiTableColumnFlags_WidthFixed, 15.0f, showFavorite, BaseColumn::ID::Favorite });
            columns.push_back({ "Plugin", ImGuiTableColumnFlags_None, 0.0f, showPlugin, BaseColumn::ID::Plugin });
            columns.push_back({ "FormID", ImGuiTableColumnFlags_WidthFixed, 50.0f, showFormID, BaseColumn::ID::FormID });
            columns.push_back({ "Name", ImGuiTableColumnFlags_None, 0.0f, showName, BaseColumn::ID::Name });
            columns.push_back({ "EditorID", ImGuiTableColumnFlags_None, 0.0f, showEditorID, BaseColumn::ID::EditorID });
            columns.push_back({ "Health", ImGuiTableColumnFlags_WidthFixed, 30.0f, showHealth, BaseColumn::ID::Health });
            columns.push_back({ "Magicka", ImGuiTableColumnFlags_WidthFixed, 30.0f, showMagicka, BaseColumn::ID::Magicka });
            columns.push_back({ "Stamina", ImGuiTableColumnFlags_WidthFixed, 30.0f, showStamina, BaseColumn::ID::Stamina });
            columns.push_back({ "CarryWeight", ImGuiTableColumnFlags_WidthFixed, 30.0f, showCarryWeight, BaseColumn::ID::CarryWeight });
        }
    };
    
	class AddItemColumns : public BaseColumnList
	{
	public:       
        AddItemColumns(
            bool* showFavorite = nullptr, bool* showPlugin = nullptr, bool* showType = nullptr, bool* showFormID = nullptr,
            bool* showName = nullptr, bool* showEditorID = nullptr, bool* showGoldValue = nullptr, bool* showBaseDamage = nullptr,
            bool* showArmorRating = nullptr, bool* showSpeed = nullptr, bool* showCritDamage = nullptr, bool* showSkill = nullptr,
            bool* showWeight = nullptr, bool* showDPS = nullptr)
        {
            columns.push_back({ "*", ImGuiTableColumnFlags_WidthFixed, 15.0f, showFavorite, BaseColumn::ID::Favorite });
            columns.push_back({ "Plugin", ImGuiTableColumnFlags_None, 0.0f, showPlugin, BaseColumn::ID::Plugin });
            columns.push_back({ "Type", ImGuiTableColumnFlags_WidthFixed, 50.0f, showType, BaseColumn::ID::Type});
            columns.push_back({ "FormID", ImGuiTableColumnFlags_WidthFixed, 50.0f, showFormID, BaseColumn::ID::FormID});
            columns.push_back({ "Name", ImGuiTableColumnFlags_None, 0.0f, showName, BaseColumn::ID::Name});
            columns.push_back({ "EditorID", ImGuiTableColumnFlags_None, 0.0f, showEditorID, BaseColumn::ID::EditorID});
            columns.push_back({ "GoldValue", ImGuiTableColumnFlags_None, 0.0f, showGoldValue, BaseColumn::ID::GoldValue});
            columns.push_back({ "BaseDamage", ImGuiTableColumnFlags_WidthFixed, 30.0f, showBaseDamage, BaseColumn::ID::BaseDamage});
            columns.push_back({ "ArmorRating", ImGuiTableColumnFlags_WidthFixed, 30.0f, showArmorRating, BaseColumn::ID::ArmorRating});
            columns.push_back({ "Speed", ImGuiTableColumnFlags_WidthFixed, 30.0f, showSpeed, BaseColumn::ID::Speed});
            columns.push_back({ "Crit", ImGuiTableColumnFlags_WidthFixed, 30.0f, showCritDamage, BaseColumn::ID::CritDamage});
            columns.push_back({ "Skill", ImGuiTableColumnFlags_WidthFixed, 0.0f, showSkill, BaseColumn::ID::Skill});
            columns.push_back({ "Weight", ImGuiTableColumnFlags_WidthFixed, 30.0f, showWeight, BaseColumn::ID::Weight});
            columns.push_back({ "DPS", ImGuiTableColumnFlags_WidthFixed, 30.0f, showDPS, BaseColumn::ID::DPS});
        }
	};


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

		template <class Object>
		inline static bool SortColumns(const Object* lhs, const Object* rhs)
		{
			const ImGuiTableSortSpecs* sort_specs = s_current_sort_specs;
			const ImGuiID ID = sort_specs->Specs->ColumnUserID;

			int delta = 0;

			switch (ID) {
			case BaseColumn::ID::Favorite:
                delta = (lhs->favorite < rhs->favorite) ? -1 : (lhs->favorite > rhs->favorite) ? 1 : 0;
                break;
            case BaseColumn::ID::Plugin:
                delta = lhs->filename.compare(rhs->filename);
                break;
            case BaseColumn::ID::Type:
                delta = lhs->GetTypeName().compare(rhs->GetTypeName());
                break;
            case BaseColumn::ID::FormID:
                delta = (lhs->FormID < rhs->FormID) ? -1 : (lhs->FormID > rhs->FormID) ? 1 : 0;
                break;
            case BaseColumn::ID::Name:
                delta = lhs->name.compare(rhs->name);
                break;
            case BaseColumn::ID::EditorID:
                delta = lhs->editorid.compare(rhs->editorid);
                break;
            case BaseColumn::ID::Health:
                if constexpr (!std::is_same<Object, NPC>::value)
                    break;
                else delta = (lhs->GetHealth() < rhs->GetHealth()) ? -1 : (lhs->GetHealth() > rhs->GetHealth()) ? 1 : 0;
                    break;
            case BaseColumn::ID::Magicka:
                if constexpr (!std::is_same<Object, NPC>::value)
                    break;
                else delta = (lhs->GetMagicka() < rhs->GetMagicka()) ? -1 : (lhs->GetMagicka() > rhs->GetMagicka()) ? 1 : 0;
                    break;
            case BaseColumn::ID::Stamina:
                if constexpr (!std::is_same<Object, NPC>::value)
                    break;
                else delta = (lhs->GetStamina() < rhs->GetStamina()) ? -1 : (lhs->GetStamina() > rhs->GetStamina()) ? 1 : 0;
                    break;
            case BaseColumn::ID::CarryWeight:
                if constexpr (!std::is_same<Object, NPC>::value)
                    break;
                else delta = (lhs->GetCarryWeight() < rhs->GetCarryWeight()) ? -1 : (lhs->GetCarryWeight() > rhs->GetCarryWeight()) ? 1 : 0;
                    break;
            case BaseColumn::ID::GoldValue:
                if constexpr (!std::is_same<Object, Item>::value) 
                    break;
                else delta = (lhs->value < rhs->value) ? -1 : (lhs->value > rhs->value) ? 1 : 0;
                    break;
            case BaseColumn::ID::BaseDamage:
                if constexpr (!std::is_same<Object, Item>::value) {
                    break;
                } else {
                    if (lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Weapon) {
                        delta = CompareWeaponDamage(lhs->TESForm->As<RE::TESObjectWEAP>(), rhs->TESForm->As<RE::TESObjectWEAP>());
                    } else if (lhs->GetFormType() == RE::FormType::Weapon) {
                        delta = 1;
                    } else if (rhs->GetFormType() == RE::FormType::Weapon) {
                        delta = -1;
                    }

                    break;
                }
            case BaseColumn::ID::ArmorRating:
                if constexpr (!std::is_same<Object, Item>::value) {
                    break;
                } else {
                    if (lhs->GetFormType() == RE::FormType::Armor && rhs->GetFormType() == RE::FormType::Armor) {
                        delta = CompareArmorRating(lhs->TESForm->As<RE::TESObjectARMO>(), rhs->TESForm->As<RE::TESObjectARMO>());
                    } else if (lhs->GetFormType() == RE::FormType::Armor) {
                        delta = 1;
                    } else if (rhs->GetFormType() == RE::FormType::Armor) {
                        delta = -1;
                    }

                    break;
                }
            case BaseColumn::ID::Speed:
                if constexpr (!std::is_same<Object, Item>::value) {
                    break;
                } else {
                    if (lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Weapon) {
                        delta = CompareWeaponSpeed(lhs->TESForm->As<RE::TESObjectWEAP>(), rhs->TESForm->As<RE::TESObjectWEAP>());
                    } else if (lhs->GetFormType() == RE::FormType::Weapon) {
                        delta = 1;
                    } else if (rhs->GetFormType() == RE::FormType::Weapon) {
                        delta = -1;
                    }
                    
                    break;
                }
            case BaseColumn::ID::CritDamage:
                if constexpr (!std::is_same<Object, Item>::value) {
                    break;
                } else {
                    if (lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Weapon) {
                        delta = CompareCritDamage(lhs->TESForm->As<RE::TESObjectWEAP>(), rhs->TESForm->As<RE::TESObjectWEAP>());
                    } else if (lhs->GetFormType() == RE::FormType::Weapon) {
                        delta = 1;
                    } else if (rhs->GetFormType() == RE::FormType::Weapon) {
                        delta = -1;
                    }
                    
                    break;
                }
            case BaseColumn::ID::DPS:
                if constexpr (!std::is_same<Object, Item>::value) {
                    break;
                } else {
                    if (lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Weapon) {
                        delta = CompareWeaponDPS(lhs->TESForm->As<RE::TESObjectWEAP>(), rhs->TESForm->As<RE::TESObjectWEAP>());
                    } else if (lhs->GetFormType() == RE::FormType::Weapon) {
                        delta = 1;
                    } else if (rhs->GetFormType() == RE::FormType::Weapon) {
                        delta = -1;
                    }

                    break;
                }
            case BaseColumn::ID::Skill:
                if constexpr (!std::is_same<Object, Item>::value) {
                    break;
                } else {
                    if ((lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Weapon) ||
                        (lhs->GetFormType() == RE::FormType::Armor && rhs->GetFormType() == RE::FormType::Armor) ||
                        (lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Armor) ||
                        (lhs->GetFormType() == RE::FormType::Armor && rhs->GetFormType() == RE::FormType::Weapon)) {
                        delta = CompareSkill(lhs->TESForm, rhs->TESForm);
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
	};
	// clang-format on
}