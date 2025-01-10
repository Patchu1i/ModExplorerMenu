#pragma once

#include "include/C/Columns.h"
#include "include/F/Frame.h"

namespace Modex
{

	using ObjectFilterType = std::pair<RE::FormType, std::string>;

	class ObjectWindow : private ISortable
	{
	private:
		static void ApplyFilters();
		static void ShowActions(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowSearch(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowFormTable(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowObjectListContextMenu(StaticObject& a_object);

		static inline std::vector<StaticObject*> objectList;
		static inline ObjectColumns columnList;

		// Actions / Behavior
		static inline bool b_ClickToSelect = true;
		static inline bool b_ClickToPlace = false;
		static inline bool b_ClickToFavorite = false;
		static inline int clickToPlaceCount = 1;

		// Local State Variables.
		static inline bool _itemHovered = false;
		static inline bool _itemSelected = false;
		static inline StaticObject* hoveredObject = nullptr;
		static inline StaticObject* selectedObject = nullptr;

		// Filter Buttons.
		static inline bool b_Tree = false;
		static inline bool b_Static = false;
		static inline bool b_Container = false;
		static inline bool b_Activator = false;
		static inline bool b_Light = false;
		static inline bool b_Door = false;
		static inline bool b_Furniture = false;

		// Filtering State Variables.
		static inline ObjectFilterType selectedFilter = { RE::FormType::None, "None" };
		static inline std::unordered_set<RE::FormType> objectFilters;

		static inline std::vector<ObjectFilterType> filterMap = {
			{ RE::FormType::Tree, "Tree" },
			{ RE::FormType::Static, "Static" },
			{ RE::FormType::Container, "Container" },
			{ RE::FormType::Activator, "Activator" },
			{ RE::FormType::Light, "Light" },
			{ RE::FormType::Door, "Door" },
			{ RE::FormType::Furniture, "Furniture" }
		};

	public:
		static void Draw(Settings::Style& a_style, Settings::Config& a_config);
		static void Init();
		static void Refresh();

	private:
		// Searching
		static inline BaseColumn::ID searchKey = BaseColumn::ID::EditorID;
		static inline char inputBuffer[256] = "";
		static inline char modListBuffer[256] = "";
		static inline std::string selectedMod = "All Mods";
		static inline bool dirty = true;
		static inline const std::map<BaseColumn::ID, const char*> InputSearchMap = {
			{ BaseColumn::ID::Plugin, "Plugin" },
			{ BaseColumn::ID::Type, "Type" },
			{ BaseColumn::ID::FormID, "Form ID" },
			{ BaseColumn::ID::EditorID, "Editor ID" }
		};
	};
}