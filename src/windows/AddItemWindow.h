#pragma once

#include "WindowBaseClass.h"
#include <unordered_set>

// #include "Console.h"

class AddItemWindow : public WindowBaseClass
{
public:
    static void Draw();
    static void Init();

    enum ItemType
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

    // If types are changed here
    // Update the _filterMap in AddItemWindow.cpp
    // Update the sort function in AddItemWindow.cpp
    struct ListItemType
    {
        const char *name;
        std::string formid;
        RE::TESForm* form;
        std::string editorid;
        AddItemWindow::ItemType formType;
        const char *typeName;
        std::int32_t goldValue;
        RE::TESFile *mod;
        bool nonPlayable;
        bool favorite;
        int quantity;
        bool selected;
    };

    enum ColumnID
    {
        ColumnID_Favorite,
        ColumnID_Type,
        ColumnID_FormID,
        ColumnID_Name,
        ColumnID_EditorID,
        ColumnID_GoldValue
    };

    enum SearchBy
    {
        FullName,
        FormID,
        EditorID
    };

    AddItemWindow()
    {
        Name = "Add Item Menu";
        Type = WindowType::window_additem;
        Open = false;
    }

    static std::vector<AddItemWindow::ListItemType> _cachedList;
    static std::vector<AddItemWindow::ListItemType *> _activeList;
    static std::unordered_set<RE::TESFile *> _modList;
    static std::unordered_set<AddItemWindow::ItemType> _filters;

    static const int column_count = 6;
    static inline ImGuiTableColumnFlags column_flags_out[column_count] = {0, 0, 0};
    static inline std::array<bool, 6> column_toggle = {true, true, false, true, true, false};

    static inline RE::TESFile *_currentMod;
    static const ImGuiTableSortSpecs *s_current_sort_specs;

    static inline ID3D11ShaderResourceView *favorite_enabled_texture;
    static inline ID3D11ShaderResourceView *favorite_disabled_texture;
    static inline ImFont* list_font;
    static inline ImFont *header_font;

    static inline SearchBy _searchBy;
    static inline char _searchBuffer[256] = "";

    static inline bool _dirtyFilter = false;

    // For ImGui Checkbox state tracking
    static bool _Alchemy;
    static bool _Ingredient;
    static bool _Ammo;
    static bool _Key;
    static bool _Misc;
    static bool _Armor;
    static bool _Book;
    static bool _Weapon;

    // Write a list of elements, where each element is a bool pointer, an ItemType, and a string.
    static std::vector<std::tuple<bool *, AddItemWindow::ItemType, std::string>> _filterMap;

    // ImGui related calls.
    static void Draw_InputSearch();
    static void Draw_FormTable();
    static void Draw_Actions();
    static void Draw_AdvancedOptions();
    static void Context_CopyOnly(const char *form, const char *name, const char *editor);
    static int TextEditCallbackStub(ImGuiInputTextCallbackData *data);
    static int TextEditCallback(ImGuiInputTextCallbackData *data);
    static void ApplyFilters();

    static void SortColumnsWithSpecs(ImGuiTableSortSpecs *sort_specs, std::vector<AddItemWindow::ListItemType *> *list, int a_size);
    //static int CompareWithSortSpecs(const void *lhs, const void *rhs)

    // Cache related calls.
    template <class T> static void CacheItems(RE::TESDataHandler *a_data, const AddItemWindow::ItemType a_formType);
    static std::string getFormEditorID(const RE::TESForm *a_form);

    static void Cache();


    // Not in use anymore
    //// Generate a cache of all relevant items in the game.
    //static std::vector<ListItemType> cachedList;

    //// Store active lists as pointers to generated cache.
    //static std::vector<ListItemType *> activeList;

    //// List of filters to apply to cache.
    //static std::unordered_set<ItemType> filters;
};
