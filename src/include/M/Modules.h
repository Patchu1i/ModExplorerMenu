#pragma once

// clang-format off

namespace Modex
{
    class ISearch
    {
        struct SearchItem
        {
            std::string name;
            size_t      weight;
            int         idx;
            bool        show;
        };

        typedef std::vector<SearchItem> SearchList;

        public:
            virtual ~ISearch() = default;
            
            // implementation
            // virtual const std::string&      GetModSearchBuffer() = 0;
            // virtual const std::string&      GetSelectedMod() = 0; 


            // interface
            bool        InputTextComboBox(const char* a_label, char* a_buffer, std::string& a_preview, size_t a_size, std::vector<std::string> a_items, float a_width);

            ImGuiKey                    _lastNavKey = ImGuiKey_None;
            bool                        forceDropdown;
            size_t                      topComparisonWeight;
            int                         topComparisonIdx;
            SearchList                  filteredList;
            SearchList                  navList;
            SearchItem                  navSelection;

    };
}