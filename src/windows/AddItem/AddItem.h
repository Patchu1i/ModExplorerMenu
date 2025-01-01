#pragma once

#include "Data.h"
#include "Graphic.h"
#include "Libraries/DescriptionFrameworkAPI.h"
#include "Settings.h"
#include "Windows/Columns.h"
#include "Windows/Frame.h"
#include <unordered_set>

// #include "Console.h"

namespace Modex
{
	using ItemFilterType = std::pair<RE::FormType, std::string>;

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
		static void Refresh();
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

		// Filtering State Variables.
		static inline ItemFilterType selectedFilter = { RE::FormType::None, "None" };
		static inline std::vector<Item*> itemList;

		static inline std::vector<ItemFilterType> filterMap = {
			{ RE::FormType::Armor, "Armor" },
			{ RE::FormType::AlchemyItem, "Alchemy" },
			{ RE::FormType::Ammo, "Ammunition" },
			{ RE::FormType::Book, "Book" },
			{ RE::FormType::Ingredient, "Ingredient" },
			{ RE::FormType::KeyMaster, "Keys" },
			{ RE::FormType::Misc, "Misc" },
			{ RE::FormType::Scroll, "Scroll" },
			{ RE::FormType::Weapon, "Weapon" },
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