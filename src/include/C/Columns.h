#pragma once
#include "include/D/Data.h"
#include "include/D/DataTypes.h"
#include "include/S/Settings.h"
#include "include/U/Util.h"

// clang-format off
    
namespace Modex
{
	class BaseColumn
	{
	public:
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
            Slot, 
            Speed, 
            CritDamage, 
            Skill, 
            Weight, 
            DPS,
            Space, 
            Zone, 
            CellName, 
            ReferenceID, 
            Race, 
            Gender, 
            Class, 
            Level
		};

        std::string             name;
		ImGuiTableColumnFlags   flags;

		float                   width;
		bool                    enabled;
        ID                      key;
	};

    class BaseColumnList
    {
    public:
        std::vector<BaseColumn> columns;

        inline const int GetTotalColumns() const { return static_cast<int>(columns.size()); }
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
            columns.push_back({ _T("Slot"), flag, 0.0f, false, BaseColumn::ID::Slot});
            columns.push_back({ _T("Speed"), flag, 0.0f, false, BaseColumn::ID::Speed});
            columns.push_back({ _T("Crit"), flag, 0.0f, false, BaseColumn::ID::CritDamage});
            columns.push_back({ _T("Skill"), flag, 0.0f, false, BaseColumn::ID::Skill});
            columns.push_back({ _T("Weight"), flag, 0.0f, false, BaseColumn::ID::Weight});
            columns.push_back({ _T("DPS"), flag, 0.0f, false, BaseColumn::ID::DPS});
        }
	};
}