#pragma once
#include "include/D/Data.h"
#include "include/D/DataTypes.h"
#include "include/S/Settings.h"
#include "include/U/Util.h"

// clang-format off
    
namespace Modex
{

    // TODO: Pending Teleport Module refactor. Since it's the only module left using the ImGui Table API
    // this was formerly used to instantiate and construct columns for each module. See SORT_TYPE now.
	class BaseColumn
	{
	public:
		enum ID
		{
			// Favorite, 
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

        // members
        std::string             name;
		ImGuiTableColumnFlags   flags;

		float                   width;
		bool                    enabled;
        ID                      key;
	};
}