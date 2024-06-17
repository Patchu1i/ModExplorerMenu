#pragma once
#include "Settings.h"
#include <PCH.h>

// clang-format off
namespace ModExplorerMenu
{
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
}
// clang-format on