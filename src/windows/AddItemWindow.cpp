#include "AddItemWindow.h"

#include "lib/Graphic.h"

#include "Console.h"

// Feature Ideas:

// - Add to Favorites
// - Add to Inventory
// - Search by mod added forms
// - Add '?' help icons
// - Sort by favorited

using _GetFormEditorID = const char *(*)(std::uint32_t);

// Empty until we fill them during initialization.
std::vector<AddItemWindow::ListItemType> AddItemWindow::_cachedList;
std::vector<AddItemWindow::ListItemType *> AddItemWindow::_activeList;
std::unordered_set<AddItemWindow::ItemType> AddItemWindow::_filters;
std::unordered_set<RE::TESFile *> AddItemWindow::_modList;

const ImGuiTableSortSpecs* AddItemWindow::s_current_sort_specs;

// For ImGui Checkbox state tracking
bool AddItemWindow::_Alchemy = false;
bool AddItemWindow::_Ingredient = false;
bool AddItemWindow::_Ammo = false;
bool AddItemWindow::_Key = false;
bool AddItemWindow::_Misc = false;
bool AddItemWindow::_Armor = false;
bool AddItemWindow::_Book = false;
bool AddItemWindow::_Weapon = false;

std::vector<std::tuple<bool*, AddItemWindow::ItemType, std::string>> AddItemWindow::_filterMap = {
    {&_Alchemy, AddItemWindow::Alchemy, "ALCH"},    {&_Ingredient, AddItemWindow::Ingredient, "INGR"},
    {&_Ammo, AddItemWindow::Ammo, "AMMO"},          {&_Key, AddItemWindow::Key, "KEYS"},
    {&_Misc, AddItemWindow::Misc, "MISC"},          {&_Armor, AddItemWindow::Armor, "ARMO"},
    {&_Book, AddItemWindow::Book, "BOOK"},          {&_Weapon, AddItemWindow::Weapon, "WEAP"}};


// Sourced from dTry @ https://github.com/D7ry
std::string AddItemWindow::getFormEditorID(const RE::TESForm *a_form)
{
    switch (a_form->GetFormType())
    {
    case RE::FormType::Keyword:
    case RE::FormType::LocationRefType:
    case RE::FormType::Action:
    case RE::FormType::MenuIcon:
    case RE::FormType::Global:
    case RE::FormType::HeadPart:
    case RE::FormType::Race:
    case RE::FormType::Sound:
    case RE::FormType::Script:
    case RE::FormType::Navigation:
    case RE::FormType::Cell:
    case RE::FormType::WorldSpace:
    case RE::FormType::Land:
    case RE::FormType::NavMesh:
    case RE::FormType::Dialogue:
    case RE::FormType::Quest:
    case RE::FormType::Idle:
    case RE::FormType::AnimatedObject:
    case RE::FormType::ImageAdapter:
    case RE::FormType::VoiceType:
    case RE::FormType::Ragdoll:
    case RE::FormType::DefaultObject:
    case RE::FormType::MusicType:
    case RE::FormType::StoryManagerBranchNode:
    case RE::FormType::StoryManagerQuestNode:
    case RE::FormType::StoryManagerEventNode:
    case RE::FormType::SoundRecord:
        return a_form->GetFormEditorID();
    default: {
        static auto tweaks = GetModuleHandleA("po3_Tweaks");
        static auto func = reinterpret_cast<_GetFormEditorID>(GetProcAddress(tweaks, "GetFormEditorID"));
        if (func)
        {
            return func(a_form->formID);
        }
            return {};
        }
    }
}

// Called to cache items individually by type into _cachedList
template <class T>
void AddItemWindow::CacheItems(RE::TESDataHandler *a_data, const AddItemWindow::ItemType a_formType)
{
    for (auto form : a_data->GetFormArray<T>())
    {
        const char *name = form->GetFullName();
		const char* typeName = "None";

        // Associate typeName from formType table.
        for (auto& item : _filterMap)
        {
            if (std::get<1>(item) == a_formType)
            {
                typeName = std::get<2>(item).c_str();
			}
		}

        RE::FormID _formid = form->GetFormID();
        std::string formid = fmt::format("{:08x}", _formid);
        //std::string formid_short = fmt::format("{:x}", _formid);
        //std::string formid = fmt::format("{} ({})", formid_long, formid_short);
        std::string editorid = getFormEditorID(form);
        std::int32_t goldValue = form->GetGoldValue();
        RE::TESFile* mod = form->GetFile();

        bool non_playable = false;

        if (a_formType == AddItemWindow::Weapon) {
            non_playable = form->As<RE::TESObjectWEAP>()->weaponData.flags.any(RE::TESObjectWEAP::Data::Flag::kNonPlayable);
        }

        _cachedList.push_back({name, formid, form, editorid, a_formType, typeName, goldValue, mod, non_playable});

        // Add mod file to list.
        if (!_modList.contains(mod))
        {
			_modList.insert(mod);
            // .first->GetFilename().data()
		}
    }
}

// Called to cache specified item types into _cachedList
void AddItemWindow::Cache()
{

    logger::info("Cache Called");
    _cachedList.clear();

    RE::TESDataHandler *data = RE::TESDataHandler::GetSingleton();

    if (!data)
        return;

    CacheItems<RE::IngredientItem>(data, Ingredient);
    CacheItems<RE::AlchemyItem>(data, Alchemy);
    CacheItems<RE::TESAmmo>(data, Ammo);
    CacheItems<RE::TESKey>(data, Key);
    CacheItems<RE::TESObjectMISC>(data, Misc);
    CacheItems<RE::TESObjectARMO>(data, Armor);
    CacheItems<RE::TESObjectBOOK>(data, Book);
    CacheItems<RE::TESObjectWEAP>(data, Weapon);

    logger::info("Cached {} items", _cachedList.size());
}

// Draws a Copy to Clipboard button on Context popup.
void AddItemWindow::Context_CopyOnly(const char *form, const char *name, const char *editor)
{
    ImVec2 buttonSize = ImVec2(ImGui::GetContentRegionAvail().x, 30.0f);
    if (ImGui::Button("Copy Form ID to Clipboard", buttonSize))
    {
        ImGui::LogToClipboard();
        ImGui::LogText(form);
        ImGui::LogFinish();
        ImGui::CloseCurrentPopup();
    }

    if (ImGui::Button("Copy Name to Clipboard", buttonSize))
    {
        ImGui::LogToClipboard();
        ImGui::LogText(name);
        ImGui::LogFinish();
        ImGui::CloseCurrentPopup();
    }

    if (ImGui::Button("Copy Editor ID to Clipboard", buttonSize))
    {
        ImGui::LogToClipboard();
        ImGui::LogText(editor);
        ImGui::LogFinish();
        ImGui::CloseCurrentPopup();
    }
}

// args are AddItemWindow::ListItemType *a, AddItemWindow::ListItemType *b
bool sortcol(const AddItemWindow::ListItemType* v1, const AddItemWindow::ListItemType* v2)
{
    const ImGuiTableSortSpecs *sort_specs = AddItemWindow::s_current_sort_specs;
    const ImGuiID ID = sort_specs->Specs->ColumnUserID;
    
    int delta = 0;

    switch (ID)
    {
        // Delta must be +1 or -1 to indicate move. Otherwise comparitor is invalid.
        case AddItemWindow::ColumnID_Favorite: // bool
            delta = (v1->favorite < v2->favorite) ? -1 : (v1->favorite > v2->favorite) ? 1 : 0;
            break;
        case AddItemWindow::ColumnID_Type: // const char *
            delta = strcmp(v1->typeName, v2->typeName);
		    break;
        case AddItemWindow::ColumnID_FormID: // std::string
		    delta = strcmp(v1->formid.c_str(), v2->formid.c_str());
            break;
        case AddItemWindow::ColumnID_Name: // const char *
            delta = strcmp(v1->name, v2->name);
            break;
        case AddItemWindow::ColumnID_EditorID: // std::string
			delta = strcmp(v1->editorid.c_str(), v2->editorid.c_str());
			break;
        case AddItemWindow::ColumnID_GoldValue: // std::int32_t
            delta = (v1->goldValue < v2->goldValue) ? -1 : (v1->goldValue > v2->goldValue) ? 1 : 0;
        default:
            break;
    }

    if (delta > 0)
        return (sort_specs->Specs->SortDirection == ImGuiSortDirection_Ascending) ? true : false;
    if (delta < 0)
        return (sort_specs->Specs->SortDirection == ImGuiSortDirection_Ascending) ? false : true;


    // Fallback to prevent assertion.
    return false;
}

// Sorts the columns of referenced list by sort_specs
void AddItemWindow::SortColumnsWithSpecs(ImGuiTableSortSpecs *sort_specs, std::vector<AddItemWindow::ListItemType *> *list, const int a_size)
{
    s_current_sort_specs = sort_specs;
    if (a_size > 1)
        std::sort(list->begin(), list->end(), sortcol);
    s_current_sort_specs = NULL;
}

static inline const float center_text_x(const char *text)
{
    return ImGui::GetCursorPosX() + ImGui::GetColumnWidth() / 2 -
                                ImGui::CalcTextSize(text).x / 2;
};

// Draw the table of items
void AddItemWindow::Draw_FormTable()
{

    //const auto child_flags = ImGuiChildFlags_None;
    //const auto window_flags = 0;
    //ImGui::BeginChild("##AddItemWindowListChild", ImVec2(ImGui::GetContentRegionAvail()), child_flags, window_flags);

    //ImGuiTableFlags_SizingStretchProp
    const auto table_flags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Sortable |
                             ImGuiTableFlags_Borders | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Hideable |
                             ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_NoBordersInBody |
                             ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;

    //const auto sizing = (_activeList.empty() ? ImGuiTableFlags_SizingFixedFit : ImGuiTableFlags_SizingStretchProp);
    const auto sizing = ImGuiTableFlags_SizingStretchProp;
    //const auto column_flags = (_activeList.empty() ? ImGuiTableColumnFlags_WidthFixed : ImGuiTableColumnFlags_WidthStretch);

    const auto table_size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

    if (ImGui::BeginTable("AddItemWindowTable", column_count, table_flags | sizing, table_size))
    {
        ImGui::TableSetupScrollFreeze(1, 1);
        ImGui::TableSetupColumn(" ", ImGuiTableColumnFlags_WidthFixed, 16.0f, ColumnID_Favorite);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 65.0f, ColumnID_Type);
        ImGui::TableSetupColumn("Form ID", ImGuiTableColumnFlags_WidthFixed, 100.0f, ColumnID_FormID);
        ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_DefaultSort, 16.0f, ColumnID_Name);
        ImGui::TableSetupColumn("Editor ID", ImGuiTableColumnFlags_None, 16.0f, ColumnID_EditorID);
        ImGui::TableSetupColumn("Gold", ImGuiTableColumnFlags_WidthFixed, 65.0f, ColumnID_GoldValue);

        ImGui::PushFont(header_font);
        ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
        for (int column = 0; column < column_count; column++)
        {
            ImGui::TableSetColumnIndex(column);
            const char *column_name = ImGui::TableGetColumnName(column); // Retrieve name passed to TableSetupColumn()
            ImGui::PushID(column);
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

            if (column != ColumnID_FormID && column != ColumnID_EditorID)
                ImGui::SetCursorPosX(center_text_x(column_name));

            ImGui::TableHeader(column_name);

            ImGui::TableSetColumnEnabled(column, column_toggle[column]);
            ImGui::PopID();
        }
        ImGui::PopFont();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 10.0f));

        if (_dirtyFilter)
            ImGui::TableGetSortSpecs()->SpecsDirty = true;

        // Sort our data if sort specs have been changed!
        if (ImGuiTableSortSpecs *sort_specs = ImGui::TableGetSortSpecs())
            if (sort_specs->SpecsDirty)
            {
                SortColumnsWithSpecs(sort_specs, &_activeList, static_cast<int>(_activeList.size()));
                sort_specs->SpecsDirty = false;
                _dirtyFilter = false;
            }

        int count = 0;
        for (auto *a_item : _activeList)
        {
            count++;

            if (count > 1000)
                break;

            if (!a_item)
                continue;

            auto table_id = std::string("##AddItemMenu::Table-") + std::to_string(count);

            ImGuiTableRowFlags row_flags = ImGuiTableRowFlags_None;

            ImGui::PushID(table_id.c_str());
            ImGui::TableNextRow(row_flags);
            ImGui::TableNextColumn();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

            auto &ref = *a_item;
            ImTextureID favorite_state = ref.favorite ? favorite_enabled_texture : favorite_disabled_texture;
            float col = ref.favorite ? 1.0f : 0.5f;

            if (ImGui::ImageButton("##FavoriteButton", favorite_state, ImVec2(18.0f, 18.0f), ImVec2(0,0), ImVec2(1,1), ImVec4(0,0,0,0), ImVec4(col,col,col,col)))
            {
                ref.favorite = !ref.favorite;
            }

            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(1);

            const ImGuiSelectableFlags select_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap;
            ImGui::PushFont(list_font);
            ImGui::TableNextColumn();
                ImGui::SetCursorPosX(center_text_x(ref.typeName));
                ImGui::Selectable(ref.typeName, &ref.selected, select_flags);
            ImGui::TableNextColumn();
                ImGui::Selectable(ref.formid.c_str(), &ref.selected, select_flags);
            ImGui::TableNextColumn();
                ImGui::SetCursorPosX(center_text_x(ref.name));
                ImGui::Selectable(ref.name, &ref.selected, select_flags);
            ImGui::TableNextColumn();
                //ImGui::SetCursorPosX(center_text_x(ref.editorid.c_str()));
                //ImGui::Selectable(ref.editorid.c_str(), &ref.selected, select_flags);
                ImGui::TextWrapped(ref.editorid.c_str());
            ImGui::TableNextColumn();
                ImGui::SetCursorPosX(center_text_x(std::to_string(ref.goldValue).c_str()));
                ImGui::Selectable(std::to_string(ref.goldValue).c_str(), &ref.selected, select_flags);

                if (ref.formType == AddItemWindow::ItemType::Weapon)
                {
                    //auto curCol = ImGui::TableGetHoveredColumn();
                    auto curRow = ImGui::TableGetHoveredRow();

                    if (curRow == ImGui::TableGetRowIndex())
                    {
                        //enum WEAPON_TYPE : std::uint32_t
                        //{
                        //    kHandToHandMelee = 0,
                        //    kOneHandSword = 1,
                        //    kOneHandDagger = 2,
                        //    kOneHandAxe = 3,
                        //    kOneHandMace = 4,
                        //    kTwoHandSword = 5,
                        //    kTwoHandAxe = 6,
                        //    kBow = 7,
                        //    kStaff = 8,
                        //    kCrossbow = 9,

                        //    kTotal = 10
                        //};

                        std::vector<const char*> weaponTypes = {
							"Hand to Hand",
							"One Hand Sword",
							"One Hand Dagger",
							"One Hand Axe",
							"One Hand Mace",
							"Two Hand Sword",
							"Two Hand Axe",
							"Bow",
							"Staff",
							"Crossbow"
						};

                        uint16_t damage = ref.form->As<RE::TESObjectWEAP>()->attackDamage; // seems like base damage
                        uint16_t damage_other = ref.form->As<RE::TESObjectWEAP>()->criticalData.damage; // 1-100x multiplier?
                        auto maxRange = ref.form->As<RE::TESObjectWEAP>()->weaponData.maxRange; // bow only
                        auto minRange = ref.form->As<RE::TESObjectWEAP>()->weaponData.minRange; // bow only
                        //auto desc = ref.form->As<RE::TESObjectWEAP>()->GetDescription();
                        auto reach = ref.form->As<RE::TESObjectWEAP>()->weaponData.reach; // + and - 1 by a little.
                        auto speed = ref.form->As<RE::TESObjectWEAP>()->weaponData.speed; // 0 - 2?
                        SKSE::stl::enumeration<RE::ActorValue, uint32_t> skill = ref.form->As<RE::TESObjectWEAP>()->weaponData.skill; // see below
                        auto stagger = ref.form->As<RE::TESObjectWEAP>()->weaponData.staggerValue; // 0 - 2 with 1 being median
                        auto crit = ref.form->As<RE::TESObjectWEAP>()->GetCritDamage();
                        //auto critChance = ref.form->As<RE::TESObjectWEAP>()->GetCrit;
                        auto type = weaponTypes[static_cast<int>(ref.form->As<RE::TESObjectWEAP>()->GetWeaponType())];

                        auto is_bound = ref.form->As<RE::TESObjectWEAP>()->IsBound();
                        auto non_playable = ref.form->As<RE::TESObjectWEAP>()->weaponData.flags.any(RE::TESObjectWEAP::Data::Flag::kNonPlayable);
                     
                        auto skill_value = skill.get(); // ActorValue enum ng.commonlib.dev/_actor_values_8h_source.html

				        ImGui::BeginTooltip();
                        ImGui::Text("attackDamage: %d", damage);
                        ImGui::Text("criticalData.damage: %d", damage_other);
                        ImGui::Text("maxRange: %f", maxRange);
                        ImGui::Text("minRange: %f", minRange);
                        ImGui::Text("reach: %f", reach);
                        ImGui::Text("speed: %f", speed);
                        ImGui::Text("stagger: %f", stagger);
                        ImGui::Text("skill: %d", skill_value);
                        ImGui::Text("crit: %d", crit);
                        ImGui::Text("type: %s", type);
                        ImGui::Text("non_playable: %s", (non_playable) ? "true" : "false");
                        ImGui::Text("is_bound: %s", (is_bound) ? "true" : "false");

				        ImGui::EndTooltip();
			        }
             
                    // uint16_t damage = ref.form->As<RE::TESObjectWEAP>()->attackDamage;
                    //auto flags = ref.form->As<RE::TESObjectWEAP>()->data->flags;
                    //RE::BGSEquipSlot* slot = ref.form->As<RE::TESObjectWEAP>()->equipSlot;
                    //auto test = ref.form->As<RE::TESObjectWEAP>()->descriptionText;
                    // ImGui::BeginItemTooltip();
					// ImGui::Text("Damage: %d", static_cast<int>(damage));
					//ImGui::Text("Flags: %s", flags);
                    //ImGui::Text("Slot: %s", slot->flags.get());
                    //ImGui::Text("test: %s", test);
                    // ImGui::EndTooltip();
			    }

            ImGui::PopFont();


            //if (ImGui::BeginPopupContextItem())
            //{
            //    Context_CopyOnly(ref.formid.c_str(), ref.name, ref.editorid.c_str());
            //    ImGui::EndPopup();
            //}

            ImGui::PopID();

            // These needs to be outside Push/Pop ID otherwise expression != 0 assertion;
            //auto input_id = std::string("##AddItemMenu::InputText-") + std::to_string(count);
            //auto height = ImGui::GetTextLineHeight();
            //auto width = ImGui::GetColumnWidth() / 4;
            ////ImGui::Dummy(ImVec2(width, height));
            //ImGui::SetCursorPosY(ImGui::GetCursorPosY() - height/2);
            //ImGui::InputInt(input_id.c_str(), &ref.quantity, 0);
        }



        ImGui::PopStyleVar(1);

        if (count > 1000)
            ImGui::SeparatorText("Too many results, please refine your search. (1000+ items)");
    }
        ImGui::EndTable();

    //ImGui::EndChild();
}

// To-Do: Rename this to something more descriptive
// This is the callback for the search bar, it's a stub for now.
int AddItemWindow::TextEditCallbackStub(ImGuiInputTextCallbackData *data)
{
    AddItemWindow *item = reinterpret_cast<AddItemWindow *>(data->UserData);
    return item->TextEditCallback(data);
}

// To-Do: Rename this to something more descriptive
// This is the callback for the search bar, it's a stub for now.
int AddItemWindow::TextEditCallback(ImGuiInputTextCallbackData *data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackCompletion)
    {
        data->InsertChars(data->CursorPos, "..");
    }
    else if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory)
    {
        SKSE::log::info("CallbackHistory");
        if (data->EventKey == ImGuiKey_UpArrow)
        {
            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, "Pressed Up"); // TODO: Cache search stack
            data->SelectAll();
        }
        else if (data->EventKey == ImGuiKey_DownArrow)
        {
            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, "Pressed Down"); // TODO: Cache search stack
            data->SelectAll();
        }
    }
    return 0;
}

void AddItemWindow::ApplyFilters()
{
    _activeList.clear();

    static char compare[256]; // Declare a char array to store the value of compare

    for (auto &item : _cachedList)
    {
        switch (_searchBy)
        {
        case FullName:
            strcpy(compare, item.name); // Copy the value of item.name to compare
            break;
        case FormID:
            strcpy(compare, item.formid.c_str()); // Copy the value of item.formid to compare
            break;
        case EditorID:
            strcpy(compare, item.editorid.c_str()); // Copy the value of item.editorid to compare
            break;
        default:
            strcpy(compare, item.name); // Copy the value of item.name to compare
            break;
        }

        auto lower_input = strlwr(_searchBuffer);
        auto lower_compare = strlwr(compare);

        if (strstr(lower_compare, lower_input) != nullptr)
        {
			if (_currentMod != nullptr && item.mod != _currentMod)  // skip non-active mods
				continue;

            if (item.nonPlayable) // skip non-usable
                continue;

            if (strcmp(item.name, "") == 0) // skip empty names
                continue;

            // Only append if the item is in the filter list.
            if (_filters.count(item.formType) > 0)
            {
                _activeList.push_back(&item);
            }
        }
    }

    // Resort the list after applying filters
    _dirtyFilter = true;
}

// Draw search bar for filtering items.
void AddItemWindow::Draw_InputSearch()
{
    static char str0[32] = "";
    ImGui::Text("Type in search terms to filter your results:");
    // Testing without ImGuiInputTextFlags_EnterReturnsTrue for live updates
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EscapeClearsAll |
                                           ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;


    std::map<SearchBy, const char*> searchByMap = {
		{FullName, "Name"},
        {EditorID, "Editor ID"},
        {FormID, "Form ID"}
	};

    if (ImGui::InputTextWithHint("##InputField", "Enter text to filter results by...", _searchBuffer,
                                 IM_ARRAYSIZE(_searchBuffer),
                                 input_text_flags, &TextEditCallbackStub, (void *)0))
        ApplyFilters();


    ImGui::SameLine();
    // With the following line:
    auto searchByValue = searchByMap.at(_searchBy);
    auto combo_flags = ImGuiComboFlags_WidthFitPreview;
    if (ImGui::BeginCombo("##FilterSearchBy", searchByValue, combo_flags))
    {
        for (auto &item : searchByMap)
        {
			auto searchBy = item.first;
			auto _searchByValue = item.second;
			bool is_selected = (_searchBy == searchBy);
            if (ImGui::Selectable(_searchByValue, is_selected))
            {
                _searchBy = searchBy;
                ApplyFilters();
            }

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}

        ImGui::EndCombo();
    }
}

// TODO: Implement more here.
void AddItemWindow::Draw_Actions()
{
    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

    auto _flags = ImGuiChildFlags_AlwaysUseWindowPadding;
    ImGui::BeginChild("##AddItemWindowMenuActions", ImVec2(ImGui::GetContentRegionAvail()), _flags);
    ImGui::SeparatorText("Selection:");

    const float button_height = 30.0f;
    const float button_width = ImGui::GetContentRegionAvail().x;

    const auto table_flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;
    if (ImGui::BeginTable("ActionBarSelection", 1, table_flags, ImVec2(ImGui::GetContentRegionAvail().x, 150.0f)))
    {
		ImGui::TableSetupColumn("Item(s)", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableHeadersRow();
		ImGui::TableNextRow();
		ImGui::TableNextColumn();



        const auto select_flags = ImGuiSelectableFlags_SpanAllColumns;
        for (auto& item : _activeList)
        {
            if (item->selected)
            {
                //ImGui::SetCursorPosX(center_text_x(item->editorid.c_str()));
                if (ImGui::Selectable(item->editorid.c_str(), false, select_flags))
                {
                    item->selected = false;
                };
                ImGui::SetItemTooltip("Click to remove.");
			}
		}
		ImGui::EndTable();
	}

    if (ImGui::Button("Clear Selection", ImVec2(button_width, 20.0f)))
    {
        for (auto &item : _activeList)
        {
            item->selected = false;
        }
    }

    ImGui::SeparatorText("Quick Actions:");


    if (ImGui::Button("Add selected to Inventory", ImVec2(button_width, button_height)))
    {
        for (auto& item : _activeList)
        {
            if (item->selected)
            {
                ConsoleCommand::AddItem(item->formid);
				item->selected = false;
                logger::info("Added item: {}", item->formid.c_str());
			}
		}
    }

    if (ImGui::Button("Add selected to Favorites", ImVec2(button_width, button_height)))
    {
        for (auto& item : _activeList)
        {
			if (item->selected)
			{
                item->favorite = true;
            }
        }
    }

    if (ImGui::Button("Select All Favorites", ImVec2(button_width, button_height)))
    {
        for (auto& item : _activeList)
        {
            if (item->favorite)
            {
				item->selected = true;
			}
		}
	}

    if (ImGui::Button("Select All", ImVec2(button_width, button_height)))
    {
        for (auto &item : _activeList)
        {
            item->selected = true;
        }
    }

    ImGui::PopStyleVar(3);


    ImGui::EndChild();
}

static inline void inline_checkbox(const char* label, bool* v)
{
	ImGui::Checkbox(label, v);
	ImGui::SameLine();
}

void AddItemWindow::Draw_AdvancedOptions()
{
    const auto header_flags = ImGuiTreeNodeFlags_SpanAvailWidth;
    if (ImGui::CollapsingHeader("Advanced Options", header_flags))
    {

        ImGui::SeparatorText("Enable/Disable Columns:");

        inline_checkbox("Show Favorite", &column_toggle[ColumnID_Favorite]);
        inline_checkbox("Show Type", &column_toggle[ColumnID_Type]);
        inline_checkbox("Show Form ID", &column_toggle[ColumnID_FormID]);
        inline_checkbox("Show Name", &column_toggle[ColumnID_Name]);
        inline_checkbox("Show Editor ID", &column_toggle[ColumnID_EditorID]);
        inline_checkbox("Show Gold Value", &column_toggle[ColumnID_GoldValue]);

        ImGui::NewLine();

        ImGui::SeparatorText("Select a mod to search:");
        auto combo_text = _currentMod ? _currentMod->GetFilename().data() : "Filter by mods";

        if (ImGui::BeginCombo("##FilterByMod", combo_text))
        {
            for (auto &mod : _modList)
            {
				const char* modName = mod->GetFilename().data();
				bool is_selected = false;
                if (ImGui::Selectable(modName, is_selected))
                {
                    _currentMod = mod;
                    ApplyFilters();
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
        }

    }
}

// Main Draw function for AddItem, called by Frame::Draw()
void AddItemWindow::Draw()
{
    const auto _flags = ImGuiOldColumnFlags_NoResize;
    ImGui::BeginColumns("##HorizontalSplit", 2, _flags);

    const float width = ImGui::GetWindowWidth();
    ImGui::SetColumnWidth(0, width * 0.75f);

    // Left Column
    Draw_InputSearch();
    ImGui::NewLine();

    bool _change = false;

    for (auto &item : AddItemWindow::_filterMap)
    {
        //auto [first, second, third] = item.;
        auto first = std::get<0>(item);
        //auto second = std::get<1>(item);
        auto third = std::get<2>(item);

        ImGui::SameLine();
        if (ImGui::Checkbox(third.c_str(), first))
        {
            _change = true;
        }

	}

    if (_change)
    {
        _filters.clear();

        for (auto &item : _filterMap)
        {
            auto first = *std::get<0>(item);
            auto second = std::get<1>(item);

            if (first)
            {
			    _filters.insert(second);
		    }
	    }

        //FilterList(_filters);
        ApplyFilters();
        _dirtyFilter = true;
    }

    ImGui::NewLine();

    Draw_AdvancedOptions();

    ImGui::NewLine();

    // To-do: Stop being a lazy POS.
    auto results = std::string("Results (") + std::to_string(_activeList.size()) + std::string(")");
    ImGui::SeparatorText(results.c_str());

    Draw_FormTable();

    // Start of Right Column
    ImGui::NextColumn();

    Draw_Actions();

    ImGui::EndColumns();
}

// Cache items on initialization
void AddItemWindow::Init()
{
    AddItemWindow::Cache();

    list_font = GraphicManager::GetFont("Gravity-Book-Small");
    header_font = GraphicManager::GetFont("Coolvetica-Medium");
    _searchBy = AddItemWindow::SearchBy::FullName;

    favorite_disabled_texture = GraphicManager::GetImage("favorite-disabled-new").texture;
    favorite_enabled_texture = GraphicManager::GetImage("favorite-enabled-new").texture;
}