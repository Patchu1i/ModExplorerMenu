#pragma once

#include "Data.h"
#include "Settings.h"
#include "Windows/Columns.h"
#include "Windows/Frame.h"

namespace ModExplorerMenu
{
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
		static inline bool b_ClickToFavorite = false;

		// Local State Variables.
		static inline bool _itemHovered = false;
		static inline bool _itemSelected = false;
		static inline StaticObject* hoveredObject = nullptr;
		static inline StaticObject* selectedObject = nullptr;

		// Filter Buttons.
		static inline bool b_Tree = true;
		static inline bool b_Static = true;
		static inline bool b_Container = true;
		static inline bool b_Activator = true;
		static inline bool b_Light = true;
		static inline bool b_Door = true;
		static inline bool b_Furniture = true;

		// Filtering State Variables.
		static inline std::unordered_set<RE::FormType> objectFilters;
		static inline std::vector<std::tuple<bool*, RE::FormType, std::string>> filterMap = {
			{ &b_Tree, RE::FormType::Tree, "Tree" }, { &b_Static, RE::FormType::Static, "Static" },
			{ &b_Container, RE::FormType::Container, "Container" }, { &b_Activator, RE::FormType::Activator, "Activator" },
			{ &b_Light, RE::FormType::Light, "Light" }, { &b_Door, RE::FormType::Door, "Door" }, { &b_Furniture, RE::FormType::Furniture, "Furniture" }
		};

	public:
		static void Draw(Settings::Style& a_style, Settings::Config& a_config);
		static void Init();

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