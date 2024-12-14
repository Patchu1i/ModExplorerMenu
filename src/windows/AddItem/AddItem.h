#pragma once

#include "Data.h"
#include "Graphic.h"
#include "Libraries/DescriptionFrameworkAPI.h"
#include "Settings.h"
#include "Windows/Columns.h"
#include <unordered_set>

// #include "Console.h"

namespace ModExplorerMenu
{
	class AddItemWindow : private ISortable
	{
	public:
		static void Draw(Settings::Style& a_style, Settings::Config& a_config);
		// static void ShowAdvancedOptions(Settings::Style& a_style, Settings::Config& a_config);
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

		// Show book window.
		static inline Item* openBook = nullptr;
		static inline Item* itemPreview = nullptr;

		// Actions
		static inline bool b_AddToInventory = true;
		static inline int clickToAddCount = 1;
		static inline bool b_PlaceOnGround = false;
		static inline int clickToPlaceCount = 1;
		static inline bool b_AddToFavorites = false;

		// Table & Filtering.
		static inline bool b_Alchemy = true;
		static inline bool b_Ingredient = true;
		static inline bool b_Ammo = true;
		static inline bool b_Key = true;
		static inline bool b_Misc = true;
		static inline bool b_Armor = true;
		static inline bool b_Book = true;
		static inline bool b_Weapon = true;

		//static constexpr int column_count = kTotal;
		static inline std::unordered_set<RE::FormType> itemFilters;
		static inline std::vector<Item*> itemList;
		static inline std::vector<std::tuple<bool*, RE::FormType, std::string>> filterMap = {
			{ &b_Alchemy, RE::FormType::AlchemyItem, "ALCH" }, { &b_Ingredient, RE::FormType::Ingredient, "INGR" },
			{ &b_Ammo, RE::FormType::Ammo, "AMMO" }, { &b_Key, RE::FormType::KeyMaster, "KEYS" },
			{ &b_Misc, RE::FormType::Misc, "MISC" }, { &b_Armor, RE::FormType::Armor, "ARMO" },
			{ &b_Book, RE::FormType::Book, "BOOK" }, { &b_Weapon, RE::FormType::Weapon, "WEAP" }
		};

		// Description Framework API.
		static inline DescriptionFrameworkAPI::IDescriptionFrameworkInterface001* g_DescriptionFrameworkInterface = nullptr;

	private:
		// TODO: Add RowBG as theme option
		static inline constexpr auto AddItemTableFlags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_Sortable |
		                                                 ImGuiTableFlags_Borders | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Hideable |
		                                                 ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_NoBordersInBody |
		                                                 ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY |
		                                                 ImGuiTableFlags_SizingStretchProp;
		static inline constexpr auto InputSearchFlags = ImGuiInputTextFlags_EscapeClearsAll;

		static inline constexpr auto ActionBarFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;

		// Search Input Field.
		//static inline const ImGuiTableSortSpecs* s_current_sort_specs;
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