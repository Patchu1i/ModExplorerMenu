#pragma once

#include "include/C/Columns.h"
#include "include/F/Frame.h"
#include "include/I/ISearch.h"
#include "include/I/ISortable.h"
#include "include/I/InputManager.h"

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

		void 						Draw(float a_offset);
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
		bool 						b_ClickToPlace;
		int 						clickToPlaceCount;

		bool 						_itemHovered;
		bool 						_itemSelected;
		ObjectData* 				hoveredObject;

		RE::FormType 				primaryFilter;
		ObjectColumns 				columnList;
		std::vector<ObjectData*>	objectList;

		// Mouse Draggin
		enum MOUSE_STATE
		{
			DRAG_IGNORE,
			DRAG_SELECT,
			DRAG_NONE
		};

		void							DrawObject(const ObjectData& a_object, const ImVec2& pos);
		void							UpdateLayoutSizes(float avail_width);
		std::unordered_set<ObjectData*> itemSelectionList;
		bool 							wasMouseInBounds;
		int 							lastItem;

		float LayoutItemSpacing;
		float LayoutSelectableSpacing;
		float LayoutOuterPadding;
		float LayoutHitSpacing;
		int LayoutColumnCount;
		int LayoutLineCount;
		ImVec2 ItemSize;
		ImVec2 LayoutItemSize;
		ImVec2 LayoutItemStep;


		// Menu State Variables.
		enum class Viewport
		{
			TableView,
			BlacklistView,
			SettingsView // TODO.
		};

		Viewport 					activeViewport;


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


		static inline ImGuiSelectionBasicStorage selectionStorage;
	};
}