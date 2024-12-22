#pragma once

#include "Data.h"
#include "Graphic.h"
#include "Libraries/DescriptionFrameworkAPI.h"
#include "Settings.h"
#include "Windows/Columns.h"
#include "Windows/Frame.h"
#include <unordered_set>

// #include "Console.h"

namespace ModExplorerMenu
{
	class AddItemWindow : private ISortable
	{
	public:
		static void Draw(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowFormTable(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowActions(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowSearch(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowItemListContextMenu(Item& a_item);
		static void ShowBookPreview();
		static void ApplyFilters();
		static void Init();

		enum FilterType
		{
			Alchemy,
			Ingredient,
			Ammo,
			Key,
			Misc,
			Armor,
			Book,
			Weapon
		};

		static inline AddItemColumns columnList;

		// Book Popup Window State.
		static inline Item* openBook = nullptr;
		static inline Item* itemPreview = nullptr;

		// Local State Variables.
		static inline bool b_AddToInventory = true;
		static inline int clickToAddCount = 1;
		static inline bool b_PlaceOnGround = false;
		static inline int clickToPlaceCount = 1;
		static inline bool b_AddToFavorites = false;

		static inline bool b_Alchemy = true;
		static inline bool b_Ingredient = true;
		static inline bool b_Ammo = true;
		static inline bool b_Key = true;
		static inline bool b_Misc = true;
		static inline bool b_Armor = true;
		static inline bool b_Book = true;
		static inline bool b_Weapon = true;

		// Filtering State Variables.
		static inline std::unordered_set<RE::FormType> itemFilters;
		static inline std::vector<Item*> itemList;
		static inline std::vector<std::tuple<bool*, RE::FormType, std::string>> filterMap = {
			{ &b_Alchemy, RE::FormType::AlchemyItem, "Alchemy" }, { &b_Ingredient, RE::FormType::Ingredient, "Ingredient" },
			{ &b_Ammo, RE::FormType::Ammo, "Ammunition" }, { &b_Key, RE::FormType::KeyMaster, "Keys" },
			{ &b_Misc, RE::FormType::Misc, "Misc" }, { &b_Armor, RE::FormType::Armor, "Armor" },
			{ &b_Book, RE::FormType::Book, "Book" }, { &b_Weapon, RE::FormType::Weapon, "Weapon" }
		};

		// Description Framework API.
		static inline DescriptionFrameworkAPI::IDescriptionFrameworkInterface001* g_DescriptionFrameworkInterface = nullptr;

	private:
		static inline BaseColumn::ID searchKey = BaseColumn::ID::Name;
		static inline char inputBuffer[256] = "";
		static inline char modListBuffer[256] = "";
		static inline std::string selectedMod = "All Mods";
		static inline bool dirty = true;
		static inline const std::map<BaseColumn::ID, const char*> InputSearchMap = {
			{ BaseColumn::ID::Name, "Name" },
			{ BaseColumn::ID::EditorID, "Editor ID" },
			{ BaseColumn::ID::FormID, "Form ID" }
		};
	};
}