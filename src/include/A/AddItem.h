#pragma once

#include "extern/DescriptionFrameworkAPI.h"
#include "include/C/Columns.h"
#include "include/F/Frame.h"
#include "include/G/Graphic.h"
#include "include/I/ISearch.h"
#include "include/I/ISortable.h"

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

		void 					Draw();
		void 					ShowBookPreview();
		void 					Refresh();
		void 					Init();

	private:
		void 					ApplyFilters();
		void 					ShowFormTable();
		void 					ShowActions();
		void 					ShowSearch();
		void 					ShowItemListContextMenu(ItemData& a_item);

		// Filtering State Variables.
		std::vector<RE::FormType> filterList = {
			{ RE::FormType::Armor },
			{ RE::FormType::AlchemyItem },
			{ RE::FormType::Ammo },
			{ RE::FormType::Book },
			{ RE::FormType::Ingredient },
			{ RE::FormType::KeyMaster },
			{ RE::FormType::Misc },
			{ RE::FormType::Weapon }
		};

		// Book Popup Window State.
		ItemData* 				openBook;
		ItemData* 				itemPreview;

		// Local State Variables.
		bool 					b_AddToInventory;
		bool 					b_PlaceOnGround;
		bool 					b_AddToFavorites;
		int 					clickToAddCount;

		RE::FormType 			primaryFilter;
		AddItemColumns 			columnList;
		std::vector<ItemData*> 	itemList;

		// Description Framework API.
		DescriptionFrameworkAPI::IDescriptionFrameworkInterface001* g_DescriptionFrameworkInterface;


		// Input Fuzzy Search
		BaseColumn::ID 			searchKey;
		char 					inputBuffer[256];
		bool 					dirty;

		const std::map<BaseColumn::ID, const char*> InputSearchMap = {
			{ BaseColumn::ID::Name, "Name" },
			{ BaseColumn::ID::EditorID, "Editor ID" },
			{ BaseColumn::ID::FormID, "Form ID" }
		};

		// ISearch Interface
		char 					modSearchBuffer[256];
		std::string 			selectedMod;
	};
}