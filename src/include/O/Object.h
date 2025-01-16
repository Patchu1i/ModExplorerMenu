#pragma once

#include "include/C/Columns.h"
#include "include/F/Frame.h"
#include "include/I/ISearch.h"
#include "include/I/ISortable.h"

// clang-format off

namespace Modex
{
	class ObjectWindow : private ISortable, private ISearch
	{
	public:
		static inline ObjectWindow* GetSingleton()
		{
			static ObjectWindow singleton;
			return &singleton;
		}

		ObjectWindow() = default;
		~ObjectWindow() = default;

		void 						Draw();
		void 						Init();
		void 						Refresh();

	private:
		void 						ApplyFilters();
		void 						ShowActions();
		void 						ShowSearch();
		void 						ShowFormTable();
		void 						ShowObjectListContextMenu(ObjectData& a_object);
		
		// Filtering State Variables.
		std::vector<RE::FormType> filterList = {
			{ RE::FormType::Tree },
			{ RE::FormType::Static },
			{ RE::FormType::Container },
			{ RE::FormType::Activator },
			{ RE::FormType::Light },
			{ RE::FormType::Door },
			{ RE::FormType::Furniture }
		};

		// Local State Variables.
		bool 						b_ClickToSelect;
		bool 						b_ClickToPlace;
		bool 						b_ClickToFavorite;
		int 						clickToPlaceCount;

		bool 						_itemHovered;
		bool 						_itemSelected;
		ObjectData* 				hoveredObject;
		ObjectData* 				selectedObject;

		RE::FormType 				primaryFilter;
		ObjectColumns 				columnList;
		std::vector<ObjectData*>	objectList;


		// Input Fuzzy Search
		BaseColumn::ID 				searchKey;
		char 						inputBuffer[256];
		bool 						dirty;

		const std::map<BaseColumn::ID, const char*> InputSearchMap = {
			{ BaseColumn::ID::Plugin, "Plugin" },
			{ BaseColumn::ID::Type, "Type" },
			{ BaseColumn::ID::FormID, "Form ID" },
			{ BaseColumn::ID::EditorID, "Editor ID" }
		};

		// ISearch Interface
		char 						modSearchBuffer[256];
		std::string 				selectedMod;
	};
}