#pragma once

#include "include/C/Columns.h"
#include "include/F/Frame.h"
#include "include/G/Graphic.h"
#include "include/I/ISearch.h"
#include "include/I/ISortable.h"
#include "include/I/InputManager.h"

// clang-format off

// 2/9/2025 - Good working state, time to cleanup.

namespace Modex
{
    template <class DataType>
    class TableView : private ISortable, private ISearch
    {

        using TableList             = std::vector<std::unique_ptr<DataType>>;
        using TableItem             = std::unique_ptr<DataType>;
        using KitList               = std::vector<std::unique_ptr<Kit>>;
        using GeneratorList         = std::function<std::vector<DataType>()>;
        using SearchMap             = std::map<SortType, const char*>;
        using FormTypeFilterList    = std::vector<RE::FormType>;
        using SortTypeList          = std::vector<SortType>;
    
    public:
        
        enum TableFlag : uint32_t {
            ModexTableFlag_Kit = 1 << 0,
            ModexTableFlag_EnableSearch = 1 << 1,
            ModexTableFlag_EnableEnchantmentSort = 1 << 2,
            ModexTableFlag_EnableNonPlayableSort = 1 << 3,
            ModexTableFlag_EnableUniqueSort = 1 << 4,
            ModexTableFlag_EnableEssentialSort = 1 << 5,
            ModexTableFlag_EnablePluginKitView = 1 << 6,
            ModexTableFlag_EnableDisabledSort = 1 << 7
        };

        enum DragBehavior {
            DragBehavior_None,
            DragBehavior_Invalid,
            DragBehavior_Add,
            DragBehavior_Remove
        };

        void AddFlag(TableFlag flag) {
            flags |= flag;
        }

        void RemoveFlag(TableFlag flag) {
            flags &= ~flag;
        }

        bool HasFlag(TableFlag flag) const {
            return (flags & flag) != 0;
        }


        TableView() {
            this->searchKey = SortType::Name;
            this->sortBy = SortType::Name;
            this->sortAscending = true;
            this->generalSearchBuffer[0] = '\0';
            this->pluginSearchBuffer[0] = '\0';
            this->data_id = "TableView::EMPTY_ID";

            this->lastSearchBuffer[0] = '\0';
            this->generalSearchDirty = false;

            this->selectedPlugin = _T("SHOW_ALL_PLUGINS");
            this->primaryFilter = RE::FormType::None;
            this->secondaryFilter = _T("All");

            this->hideEnchanted = false;
            this->hideNonPlayable = true;
            this->hideDisabled = false;
            this->compactView = false;
            this->showEditorID = false;
            this->itemPreview = nullptr;

            this->showPluginKitView = false;
            this->generator = nullptr;
            this->selectedKit = nullptr;
            this->confirmDeleteKit = nullptr;
            this->clickAmount = nullptr;
        }

        ~TableView() = default;

        // core behaviors
        void                    Draw(const TableList& a_tableList, int overrideLayout);
        void                    PluginKitView();
        void                    Refresh();
        void                    Unload();
        void                    Load();
        void                    Init();
        void                    Reset();
        void                    SyncChangesToKit();

        // class builder methods
        void                    SetGenerator(std::function<std::vector<DataType>()> a_generator);
        void                    SetupSearch(const Data::PLUGIN_TYPE& a_pluginType);
        void                    SetKitPointer(std::string* a_kit) { selectedKit = a_kit; }
        void                    SetDragDropID(const std::string& a_id);
        void                    SetClickAmount(int* a_amount) { clickAmount = a_amount; }
        void                    SetDoubleClickBehavior(bool* a_clickToAdd) { clickToAdd = a_clickToAdd; }
        void                    SetDataID(const std::string& a_id) { data_id = a_id; } // used for persistent data reference

        void                    SetCompactView(bool a_compact) { compactView = a_compact; }
        void                    SetHideEnchanted(bool a_hide) { hideEnchanted = a_hide; }
        void                    SetHideNonPlayable(bool a_hide) { hideNonPlayable = a_hide; }
        void                    SetHideNonUnique(bool a_hide) { hideNonUnique = a_hide; }
        void                    SetHideNonEssential(bool a_hide) { hideNonEssential = a_hide; }
        void                    SetHideDisabled(bool a_hide) { hideDisabled = a_hide; }
        void                    SetShowEditorID(bool a_show) { showEditorID = a_show; }
        void                    SetShowPluginKitView(bool a_show) { showPluginKitView = a_show; }
        void                    SetSortBy(SortType a_sortBy) { sortBy = a_sortBy; }
        void                    SetSortAscending(bool a_ascending) { sortAscending = a_ascending; }
        
        // drag n drop behaviors
        void                    AddDragDropTarget(const std::string a_id, TableView* a_view);
        void                    RemoveDragDropTarget(const std::string a_id);
        void                    AddPayloadItemToKit(const std::unique_ptr<DataType>& a_item);
        void                    RemovePayloadItemFromKit(const std::unique_ptr<DataType>& a_item);
        const std::string       GetDragDropID() const { return dragDropSourceID; }
        TableList*              GetTableListPtr() { return &tableList; }
        TableList&              GetTableListRefr() { return tableList; }
        
        // search and sort
        void                    ShowSearch(const float& a_height);
        void                    ShowRecent(const float& a_height);
        void                    ShowSearchStatistics();
        void                    ShowSort();

        // selection
        void                    RemoveSelectedFromKit();
        void                    AddSelectedToKit();
        void                    AddSelectionToInventory(int a_count);
        void                    EquipSelection();
        void                    PlaceSelectionOnGround(int a_count);
        void                    AddAll();
        void                    PlaceAll();

        std::vector<DataType>   GetSelection();
        uint32_t                GetSelectionCount() const;
        TableItem&              GetItemPreview() { return itemPreview; }

        // Recently Used
        void                    AddItemToRecent(const std::unique_ptr<DataType>& a_item);
        void                    LoadRecentList();
        bool                    refreshRecentList = false;

        // some backported accessors
        RE::FormType&                           GetPrimaryFilter() { return primaryFilter; }
        const std::vector<std::string>&         GetPluginList() const { return pluginList; };
        const TableList&                        GetTableList() const { return tableList; }
        void                                    BuildPluginList();

    private:

        // table meta data
        TableList               tableList;
        TableList               recentList;
        TableList               searchList;
        TableList               filterList;
        KitList                 kitList;

        ImDrawList*             DrawList; // unused?
        uint32_t                flags = 0;
        GeneratorList           generator;
        Data::PLUGIN_TYPE       pluginType;
        std::string             data_id; // used for persistent data reference

        TableItem               itemPreview;

        // kit specific stuff
        void                    LoadKitsFromSelectedPlugin();
        std::string*            selectedKit;
        int*                    clickAmount;
        bool*                   clickToAdd;

        // search and filter behavior
        void                    Filter(const std::vector<DataType>& a_data);
        void                    SecondaryNPCFilter(const std::set<std::string>& a_data, const float& a_width);
        
        char                    generalSearchBuffer[256];
        char                    secondaryFilterBuffer[256];
        char                    pluginSearchBuffer[256];
        bool                    generalSearchDirty;
        char                    lastSearchBuffer[256];
        SortType                searchKey;
        SearchMap               inputSearchMap;
        
        std::string             selectedPlugin;
        RE::FormType            primaryFilter;
        FormTypeFilterList      primaryFilterList;
        std::string             secondaryFilter; // FormType?
        bool                    secondaryFilterExpanded;
        
        // abstractions for compiling tablelist
        void                    UpdateImGuiTableIDs();
        void                    UpdateKitItemData();
        
        // sorting behavior
        bool                    SortFnKit(const std::unique_ptr<Kit>& a, const std::unique_ptr<Kit>& b);
        bool                    SortFn(const std::unique_ptr<DataType>& a, const std::unique_ptr<DataType>& b);
        void                    SortListBySpecs();

        SortType                sortBy;
        SortTypeList            sortByList;
        SortTypeList            sortByListKit;
        bool                    sortAscending;

        // view behavior
        bool                    hideEnchanted;
        bool                    hideNonPlayable;
        bool                    hideNonUnique;
        bool                    hideNonEssential;
        bool                    hideDisabled;
        bool                    compactView;
        bool                    showEditorID;
        bool                    showPluginKitView;

        // layout and drawing
        std::string             GetSortProperty(const DataType& a_item);
        void                    UpdateLayout(float a_width, int overrideLayout);
        void                    DrawItem(const DataType& a_item, const ImVec2& a_pos, const bool& a_selected, const int& overrideLayout);
        void                    DrawKit(const Kit& a_kit, const ImVec2& a_pos, const bool& a_selected);
        
		float                   LayoutRowSpacing;
		float                   LayoutSelectableSpacing;
		float                   LayoutOuterPadding;
		float                   LayoutHitSpacing;
		int                     LayoutColumnCount;
		int                     LayoutLineCount;
		ImVec2                  LayoutItemSize;
		ImVec2                  LayoutItemStep;
		ImVec2                  ItemSize;

        // drag n drop stuff
        std::string					                      dragDropSourceID;
        std::map<std::string, TableView*>                 dragDropSourceList;
        
        // multi-select
        ImGuiMultiSelectFlags ms_flags = 
        ImGuiMultiSelectFlags_ClearOnClickVoid | ImGuiMultiSelectFlags_SelectOnClickRelease | 
        ImGuiMultiSelectFlags_NoAutoSelect | ImGuiMultiSelectFlags_BoxSelect1d;
    
        ImGuiSelectionBasicStorage selectionStorage;

        // This is a new implementation. We are storing pluginList in the instance of table view
        // so that it can be cached for performance. We need to update it if any parameters change.
        // In this case, primaryFilter changes the results of this. Blacklist too?
        std::vector<std::string>                pluginList;
        std::unordered_set<const RE::TESFile*>  pluginSet;
        std::ptrdiff_t                          totalGenerated;
        std::unique_ptr<Kit>                    confirmDeleteKit;

        // test
        std::string                             dragDropTooltip;
        void                                    DrawDragDropPayload(DragBehavior a_behavior);


    };
}