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
		bool enabled;

		enum ID
		{
			Favorite, Plugin, Type, FormID, Name, EditorID, Health,
            Magicka, Stamina, CarryWeight, GoldValue, BaseDamage,
            ArmorRating, Speed, CritDamage, Skill, Weight, DPS,
            Space, Zone, CellName, ReferenceID, Race, Gender, Class, Level
		};

        ID key;
	};

    class BaseColumnList
    {
    public:
        std::vector<BaseColumn> columns;

        // Deprecated
        // bool IsColumnEnabled(BaseColumn::ID a_id)
        // {
        //     for (auto& column : columns) {
        //         if (column.key == a_id) {
        //             return *column.enabled;
        //         }
        //     }
        //     return false;
        // }

        // Deprecated
        // bool IsAllColumnsDisabled()
        // {
        //     for (auto& column : columns) {
        //         if (*column.enabled) {
        //             return false;
        //         }
        //     }
        //     return true;
        // }

        inline int GetTotalColumns() { return static_cast<int>(columns.size()); }

        // Deprecated
        // inline int EnabledColumns() {
        //     int count = 0;
        //     for (auto& column : columns) {
        //         if (*column.enabled) {
        //             count++;
        //         }
        //     }
        //     return count;
        // }
    };

    class TeleportColumns : public BaseColumnList
    {
    public:
        static inline auto flag = ImGuiTableColumnFlags_WidthStretch;

        TeleportColumns()
        {
            columns.push_back({ ICON_RPG_HEART, ImGuiTableColumnFlags_WidthFixed, 15.0f, false, BaseColumn::ID::Favorite });
            columns.push_back({ _T("Plugin"), flag, 0.0f, false, BaseColumn::ID::Plugin });
            columns.push_back({ _T("Space"), flag, 0.0f, false, BaseColumn::ID::Space });
            columns.push_back({ _T("Zone"), flag, 0.0f, false, BaseColumn::ID::Zone });
            columns.push_back({ _T("Cell"), flag, 0.0f, false, BaseColumn::ID::CellName });
            columns.push_back({ _T("Editor ID"), flag, 0.0f, false, BaseColumn::ID::EditorID });
        }
    };

    class ObjectColumns : public BaseColumnList
    {
    public:
        static inline auto flag = ImGuiTableColumnFlags_WidthStretch;

        ObjectColumns()
        {
            columns.push_back({ ICON_RPG_HEART, ImGuiTableColumnFlags_WidthFixed, 15.0f, false, BaseColumn::ID::Favorite });
            columns.push_back({ _T("Plugin"), flag, 0.0f, false, BaseColumn::ID::Plugin });
            columns.push_back({ _T("Type"), flag, 0.0f, false, BaseColumn::ID::Type });
            columns.push_back({ _T("Form ID"), flag, 0.0f, false, BaseColumn::ID::FormID });
            columns.push_back({ _T("Editor ID"), flag, 0.0f, false, BaseColumn::ID::EditorID });
        }
    };

    class NPCColumns : public BaseColumnList
    {
    public:
        static inline auto flag = ImGuiTableColumnFlags_WidthStretch;

        NPCColumns()
        {
            columns.push_back({ ICON_RPG_HEART, ImGuiTableColumnFlags_WidthFixed, 15.0f, false, BaseColumn::ID::Favorite });
            columns.push_back({ _T("Plugin"), flag, 0.0f, false, BaseColumn::ID::Plugin });
            columns.push_back({ _T("Form ID"), flag, 0.0f, false, BaseColumn::ID::FormID });
            columns.push_back({ _T("Reference ID"), flag, 0.0f, false, BaseColumn::ID::ReferenceID });
            columns.push_back({ _T("Name"), flag, 0.0f, false, BaseColumn::ID::Name });
            columns.push_back({ _T("Editor ID"), flag, 0.0f, false, BaseColumn::ID::EditorID });
            columns.push_back({ _T("Race"), flag, 0.0f, false, BaseColumn::Race});
            columns.push_back({ _T("Gender"), flag, 0.0f, false, BaseColumn::Gender});
            columns.push_back({ _T("Class"), flag, 0.0f, false, BaseColumn::Class});
            columns.push_back({ _T("Level"), flag, 0.0f, false, BaseColumn::Level});
            columns.push_back({ _T("Health"), flag, 30.0f, false, BaseColumn::ID::Health });
            columns.push_back({ _T("Magicka"), flag, 30.0f, false, BaseColumn::ID::Magicka });
            columns.push_back({ _T("Stamina"), flag, 30.0f, false, BaseColumn::ID::Stamina });
            columns.push_back({ _T("Weight"), flag, 30.0f, false, BaseColumn::ID::CarryWeight });
        }
    };
    
	class AddItemColumns : public BaseColumnList
	{
	public:
        static inline auto flag = ImGuiTableColumnFlags_WidthStretch;

        AddItemColumns()
        {
            columns.push_back({ ICON_RPG_HEART, ImGuiTableColumnFlags_WidthFixed, 15.0f, false, BaseColumn::ID::Favorite });
            columns.push_back({ _T("Plugin"), flag, 0.0f, false, BaseColumn::ID::Plugin });
            columns.push_back({ _T("Type"), flag, 0.0f, false, BaseColumn::ID::Type});
            columns.push_back({ _T("Form ID"), flag, 0.0f, false, BaseColumn::ID::FormID});
            columns.push_back({ _T("Name"), flag, 0.0f, false, BaseColumn::ID::Name});
            columns.push_back({ _T("Editor ID"), flag, 0.0f, false, BaseColumn::ID::EditorID});
            columns.push_back({ _T("Value"), flag, 0.0f, false, BaseColumn::ID::GoldValue});
            columns.push_back({ _T("Damage"), flag, 0.0f, false, BaseColumn::ID::BaseDamage});
            columns.push_back({ _T("Armor"), flag, 0.0f, false, BaseColumn::ID::ArmorRating});
            columns.push_back({ _T("Speed"), flag, 0.0f, false, BaseColumn::ID::Speed});
            columns.push_back({ _T("Crit"), flag, 0.0f, false, BaseColumn::ID::CritDamage});
            columns.push_back({ _T("Skill"), flag, 0.0f, false, BaseColumn::ID::Skill});
            columns.push_back({ _T("Weight"), flag, 0.0f, false, BaseColumn::ID::Weight});
            columns.push_back({ _T("DPS"), flag, 0.0f, false, BaseColumn::ID::DPS});
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
            case BaseColumn::ID::EditorID:
                delta = lhs->editorid.compare(rhs->editorid);
                break;
            case BaseColumn::ID::Plugin:
                delta = lhs->filename.compare(rhs->filename);
                break;
            case BaseColumn::ID::Type:
                if constexpr (!std::is_base_of<BaseObject, Object>::value)
                    break;
                else delta = lhs->GetTypeName().compare(rhs->GetTypeName());
                    break;
            case BaseColumn::ID::FormID:
                if constexpr (!std::is_base_of<BaseObject, Object>::value)
                    break;
                else delta = (lhs->FormID < rhs->FormID) ? -1 : (lhs->FormID > rhs->FormID) ? 1 : 0;
                    break;
            case BaseColumn::ID::ReferenceID:
                if constexpr (!std::is_base_of<BaseObject, Object>::value)
                    break;
                else delta = (lhs->refID < rhs->refID) ? -1 : (lhs->refID > rhs->refID) ? 1 : 0;
                    break;
            case BaseColumn::ID::Name:
                if constexpr (!std::is_base_of<BaseObject, Object>::value)
                    break;
                else delta = lhs->name.compare(rhs->name);
                    break;
            case BaseColumn::ID::Class:
                if constexpr (!std::is_same<Object, NPC>::value) {
                    break;
                } else {
                    auto lhsTesNPC = lhs->TESForm->As<RE::TESNPC>();
                    auto rhsTesNPC = rhs->TESForm->As<RE::TESNPC>();
                    std::string lhsClass = lhsTesNPC->npcClass->GetFullName();
                    std::string rhsClass = rhsTesNPC->npcClass->GetFullName();

                    delta = lhsClass.compare(rhsClass);
                    break;
                }
            case BaseColumn::ID::Gender:
                if constexpr (!std::is_same<Object, NPC>::value) {
                    break;
                } else {
                    auto lhsTesNPC = lhs->TESForm->As<RE::TESNPC>();
                    auto rhsTesNPC = rhs->TESForm->As<RE::TESNPC>();
                    bool lhsGender = lhsTesNPC->IsFemale();
                    bool rhsGender = rhsTesNPC->IsFemale();
                    
                    delta = (lhsGender < rhsGender) ? -1 : (lhsGender > rhsGender) ? 1 : 0;
                    break;
                }
            case BaseColumn::ID::Race:
                if constexpr (!std::is_same<Object, NPC>::value) {
                    break;
                } else {
                    auto lhsTesNPC = lhs->TESForm->As<RE::TESNPC>();
                    auto rhsTesNPC = rhs->TESForm->As<RE::TESNPC>();
                    std::string lhsRace = lhsTesNPC->race->GetFullName();
                    std::string rhsRace = rhsTesNPC->race->GetFullName();

                    delta = lhsRace.compare(rhsRace);
                    break;
                }
            case BaseColumn::ID::Level:
                if constexpr (!std::is_same<Object, NPC>::value) {
                    break;
                } else {
                    auto lhsTesNPC = lhs->TESForm->As<RE::TESNPC>();
                    auto rhsTesNPC = rhs->TESForm->As<RE::TESNPC>();

                    delta = (lhsTesNPC->GetLevel() < rhsTesNPC->GetLevel()) ? -1 : (lhsTesNPC->GetLevel() > rhsTesNPC->GetLevel()) ? 1 : 0;
                    break;
                }
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
            case BaseColumn::ID::Space:
                if constexpr (!std::is_same<Object, Cell>::value)
                    break;
                else delta = lhs->space.compare(rhs->space);
                    break;
            case BaseColumn::ID::Zone:
                if constexpr (!std::is_same<Object, Cell>::value)
                    break;
                else delta = lhs->zone.compare(rhs->zone);
                    break;
            case BaseColumn::ID::CellName:
                if constexpr (!std::is_same<Object, Cell>::value)
                    break;
                else delta = lhs->cellName.compare(rhs->cellName);
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
	// clang-format on
}