#pragma once

#include "extern/DescriptionFrameworkAPI.h"
#include "include/C/Columns.h"
#include "include/F/Frame.h"
#include "include/G/Graphic.h"
#include "include/M/Modules.h"

// clang-format off

namespace Modex
{
	class AddItemWindow : private ISortable, private ISearch
	{
	public:

		static inline AddItemWindow* GetSingleton()
		{
			static AddItemWindow singleton;
			return &singleton;
		}

		AddItemWindow() = default;
		~AddItemWindow() = default;


		void Draw(Settings::Style& a_style, Settings::Config& a_config);
		void ShowFormTable(Settings::Style& a_style, Settings::Config& a_config);
		void ShowActions(Settings::Style& a_style, Settings::Config& a_config);
		void ShowSearch(Settings::Style& a_style, Settings::Config& a_config);
		void ShowItemListContextMenu(ItemData& a_item);
		void ShowBookPreview();
		void ApplyFilters();
		void Refresh();
		void Init();

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

		AddItemColumns columnList;

		// Book Popup Window State.
		ItemData* openBook = nullptr;
		ItemData* itemPreview = nullptr;

		// Local State Variables.
		bool b_AddToInventory = true;
		bool b_PlaceOnGround = false;
		bool b_AddToFavorites = false;
		int clickToAddCount = 1;

		// Filtering State Variables.
		ItemFilterType selectedFilter = { RE::FormType::None, "None" };
		std::vector<ItemData*> itemList;

		std::vector<ItemFilterType> filterMap = {
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
		DescriptionFrameworkAPI::IDescriptionFrameworkInterface001* g_DescriptionFrameworkInterface = nullptr;


	private:
		BaseColumn::ID searchKey = BaseColumn::ID::Name;
		char inputBuffer[256] = "";
		// static inline char modListBuffer[256] = "";
		// static inline std::string selectedMod = "All Mods";
		bool dirty = true;
		const std::map<BaseColumn::ID, const char*> InputSearchMap = {
			{ BaseColumn::ID::Name, "Name" },
			{ BaseColumn::ID::EditorID, "Editor ID" },
			{ BaseColumn::ID::FormID, "Form ID" }
		};

		// ISearch Interface
		char modSearchBuffer[256];
		std::string selectedMod;
	};
}