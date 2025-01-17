#pragma once
#include "include/I/ISearch.h"

namespace Modex
{
	// Original Source: https://github.com/ocornut/imgui/issues/718
	// Modified heavily to fit the needs of Modex. This function is used to create a text input field with a dropdown list of items.
	// Equipped with keyboard navigation, mousewheel support, and tab-completion. The dropdown list is filtered based on the input text.
	//
	// @param a_label - The ImGui identifier for the InputTextWithHint (`##Hide`)
	// @param a_buffer - The buffer to store the input text. (`modListBuffer`)
	// @param a_preview - The preview text to display in the input field. (`selectedMod`)
	// @param a_size - The size of the buffer. (`IM_ARRAYSIZE(modListBuffer)`)
	// @param a_items - The list of items to display in the dropdown. Use (`Data::GetFilteredListOfPluginNames(Data::PLUGIN_TYPE, selectedFilter)`)
	// @param a_width - The width of the input field.
	// @return bool - Returns true if the user has selected an item from the dropdown.
	// Additionally returns true if the user hits enter, or tab.
	bool ISearch::InputTextComboBox(const char* a_label, char* a_buffer, std::string& a_preview, size_t a_size, std::vector<std::string> a_items, float a_width)
	{
		bool result = false;
		ImGuiContext& g = *GImGui;

		auto arrowSize = ImGui::GetFrameHeight();
		ImGui::SetNextItemWidth(a_width - arrowSize);
		result = ImGui::InputTextWithHint(a_label, a_preview.c_str(), a_buffer, a_size, ImGuiInputTextFlags_EnterReturnsTrue);
		auto prevItemRectMax = ImGui::GetItemRectMax();
		auto prevItemRectMin = ImGui::GetItemRectMin();
		auto prevItemRectSize = ImGui::GetItemRectSize();

		ImGuiID inputTextID = ImGui::GetItemID();
		const bool inputTextActive = ImGui::IsItemActive();
		ImGuiInputTextState* inputState = inputTextActive ? ImGui::GetInputTextState(inputTextID) : NULL;

		// Block ImGui ItemSpacing to re-create ComboBox style.
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		ImGui::SameLine();
		if (ImGui::ArrowButton("##ModFilterDropDown", ImGuiDir_Down)) {
			if (ImGui::IsPopupOpen("SuggestionPopup")) {
				ImGui::CloseCurrentPopup();
				forceDropdown = false;
			} else {
				ImGui::OpenPopup("SuggestionPopup");
				forceDropdown = true;
			}
		}
		ImGui::PopStyleVar();

		if (inputTextActive) {
			forceDropdown = false;
			ImGui::OpenPopup("SuggestionPopup");
		}

		// Position and size popup
		ImGui::SetNextWindowPos(ImVec2(prevItemRectMin.x, prevItemRectMax.y + ImGui::GetStyle().ItemSpacing.y));

		ImGuiWindowFlags popup_window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
		popup_window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
		popup_window_flags |= ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NavFlattened;
		popup_window_flags |= ImGuiWindowFlags_NoNav;
		popup_window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

		auto minPopupSize = ImVec2(prevItemRectSize.x, 0);
		auto maxPopupSize = ImVec2(prevItemRectSize.x, prevItemRectSize.y * 20);  // 20 items max (?)
		ImGui::SetNextWindowSizeConstraints(minPopupSize, maxPopupSize);
		if (ImGui::BeginPopupEx(ImGui::GetID("SuggestionPopup"), popup_window_flags)) {
			ImGuiWindow* popup_window = g.CurrentWindow;
			const bool popup_is_appearing = ImGui::IsWindowAppearing();

			// Important: Tracks navigation cursor for keyboard input.
			const int cursor_idx_prev = ImGui::GetStateStorage()->GetInt(ImGui::GetID("CursorIdx"), -1);
			int cursor_idx = cursor_idx_prev;

			// Important: Tracks navigation state so that the user can switch between Arrow Keys and Mousewheel seamlessly.
			const bool unlock_scroll_prev = ImGui::GetStateStorage()->GetBool(ImGui::GetID("UnlockScroll"), false);
			bool unlock_scroll = unlock_scroll_prev;

			if (navList.size() <= 0) {
				ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.3f, 1.0f), "No Plugins Found, try clearing your search.");
			}

			// If we are typing, reset navigation state and cursor. The filtered list
			// will also be reset further down in the code.
			for (int i = ImGuiKey_NamedKey_BEGIN; i < ImGuiKey_NamedKey_END; i++) {
				auto key = static_cast<ImGuiKey>(i);
				if (key == ImGuiKey_DownArrow ||
					key == ImGuiKey_UpArrow ||
					key == ImGuiKey_Tab ||
					key == ImGuiKey_Enter ||
					key == ImGuiKey_MouseLeft ||
					key == ImGuiKey_MouseRight)
					continue;

				if (key == ImGuiKey_MouseWheelX || key == ImGuiKey_MouseWheelY) {
					unlock_scroll = true;
					break;
				}

				if (ImGui::IsKeyDown(key)) {
					_lastNavKey = ImGuiKey_None;
					break;
				}
			}

			// Custom keyboard navigation.
			// - Keyboard navigation relies on navList which is a cached SearchList with hidden SearchItems omitted.
			// - This is important: We have to feed navigation a linear list of items to match cursor idx.
			bool rewrite_buf = false;
			if (ImGui::Shortcut(ImGuiKey_DownArrow, ImGuiInputFlags_Repeat, inputTextID) && (navList.size() > 0)) {
				cursor_idx = (cursor_idx + 1) % navList.size();
				navSelection = navList.at(cursor_idx);

				rewrite_buf = true;
				unlock_scroll = false;
				_lastNavKey = ImGuiKey_DownArrow;
			}
			if (ImGui::Shortcut(ImGuiKey_UpArrow, ImGuiInputFlags_Repeat, inputTextID) && (navList.size() > 0)) {
				cursor_idx = (cursor_idx - 1 + static_cast<int>(navList.size())) % static_cast<int>(navList.size());
				navSelection = navList.at(cursor_idx);

				rewrite_buf = true;
				unlock_scroll = false;
				_lastNavKey = ImGuiKey_UpArrow;
			}

			// Makeshift auto-completion to circumvent the need for ImGui's implementation.
			if (ImGui::Shortcut(ImGuiKey_Tab, ImGuiInputFlags_RouteActive, inputTextID) && (navList.size() > 0)) {
				ImFormatString(a_buffer, a_size, "%s", navSelection.name.c_str());
				if (inputState) {
					inputState->ReloadUserBufAndMoveToEnd();
					ImGui::ClearActiveID();
				} else {
					ImGui::ActivateItemByID(inputTextID);
				}

				result = true;
				ImGui::CloseCurrentPopup();
			}

			// Disable PageUp/PageDown keys
			if (ImGui::Shortcut(ImGuiKey_PageUp, 0, inputTextID)) {}
			if (ImGui::Shortcut(ImGuiKey_PageDown, 0, inputTextID)) {}
			if (rewrite_buf) {
				ImFormatString(a_buffer, a_size, "%s", navSelection.name.c_str());

				if (inputState) {
					inputState->ReloadUserBufAndSelectAll();
				} else {
					ImGui::ActivateItemByID(inputTextID);
				}
			}

			// If we are ever using the navigation keys, we do not want to modify the original filterList copy.
			// This is because we want to keep the original list intact for future filtering.
			// Only when accepting input do we clear the previous filterList and navList cache and rebuild.

			if (_lastNavKey == ImGuiKey_DownArrow || _lastNavKey == ImGuiKey_UpArrow) {
				int filtered_idx = 0;
				for (int i = 0; i < filteredList.size(); i++) {
					auto item = filteredList.at(i);
					if (item.show && !item.name.empty()) {
						const char* item_name = item.name.c_str();
						const ImVec2 item_pos = popup_window->DC.CursorPos;

						if (ImGui::Selectable(item_name, filtered_idx == cursor_idx)) {
							ImGui::ClearActiveID();
							// ImFormatString(a_buffer, a_size, "%s", item_name);
							navSelection = item;
							ImGui::CloseCurrentPopup();
							result = true;
							break;
						}

						// TODO: Flickering while scrolling
						if (!unlock_scroll && filtered_idx == cursor_idx) {
							ImGui::SetScrollHereY();
						}

						filtered_idx++;
					}
				}

			} else {
				navList.clear();
				filteredList.clear();
				topComparisonWeight = std::string::npos;

				// (First Pass)
				// The purpose of doing two passes is to first observe the entire list and create
				// a cache of each item. I do this so I can track the userdata and state across those
				// items. The second pass (mostly through the filtered list) does the actual drawing.
				for (int current_item_idx = 0; current_item_idx < a_items.size(); current_item_idx++) {
					std::string compare = a_items.at(current_item_idx);
					std::string input = a_buffer;
					size_t comparator_weight = 0;

					// TODO: Is this an issue for Non-ASCII characters?
					std::transform(compare.begin(), compare.end(), compare.begin(), ::tolower);
					std::transform(input.begin(), input.end(), input.begin(), ::tolower);
					comparator_weight = compare.find(input);

					// Set top match to true item_idx position
					if (comparator_weight < topComparisonWeight) {
						topComparisonWeight = comparator_weight;
						topComparisonIdx = current_item_idx;
					}

					SearchItem a_item;
					a_item.name = a_items.at(current_item_idx);
					a_item.weight = comparator_weight;
					a_item.idx = current_item_idx;
					a_item.show = (comparator_weight == std::string::npos) ? false : true;

					// a_temp.push_back(a_item);
					filteredList.push_back(a_item);

					if (a_item.show) {
						navList.push_back(a_item);
					}
				}

				// (Second Pass)
				// Need to keep track of the filtered index for cursor position since we don't use a vector and
				// don't step through it incrementally. (Because we track shown state based on SearchItem::show).
				// forceDropDown is used to keep bypass the filter and show all items if ArrowButton is clicked.
				int filtered_idx = 0;
				for (int i = 0; i < filteredList.size(); i++) {
					auto item = filteredList.at(i);
					if ((item.show or forceDropdown) && !item.name.empty()) {
						const char* item_name = item.name.c_str();
						const ImVec2 item_pos = popup_window->DC.CursorPos;

						if (strlen(a_buffer) == 0) {  // maybe utf-8 safe?
							if (item_name == a_preview) {
								if (popup_is_appearing) {
									ImGui::SetScrollHereY();
								}
								navSelection = item;
								cursor_idx = filtered_idx;
							}
						} else if (topComparisonIdx == item.idx) {
							if (popup_is_appearing) {
								ImGui::SetScrollHereY();
							}
							navSelection = item;
							cursor_idx = filtered_idx;
						}

						if (ImGui::Selectable(item_name, cursor_idx == filtered_idx)) {
							ImGui::ClearActiveID();
							//ImFormatString(a_buffer, a_size, "%s", item_name);
							navSelection = item;
							ImGui::CloseCurrentPopup();
							result = true;
							break;
						}

						filtered_idx++;
					}
				}
			}

			// Close popup on deactivation (unless we are mouse-clicking in our popup)
			if (!inputTextActive && !ImGui::IsWindowFocused() && !forceDropdown) {
				_lastNavKey = ImGuiKey_None;
				ImGui::CloseCurrentPopup();
			}

			// Additional state handling for ArrowButton drop-down.
			if (forceDropdown && !ImGui::IsWindowFocused() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered()) {
				forceDropdown = false;
				ImGui::CloseCurrentPopup();
			}

			// Store cursor position.
			if (cursor_idx != cursor_idx_prev) {
				ImGui::GetStateStorage()->SetInt(ImGui::GetID("CursorIdx"), cursor_idx);
			}

			// Store scroll position.
			if (unlock_scroll != unlock_scroll_prev) {
				ImGui::GetStateStorage()->SetBool(ImGui::GetID("UnlockScroll"), unlock_scroll);
			}

			ImGui::EndPopup();
		}

		// Because we introduced the InputTextFlags_EnterReturnsTrue flag, we need to manually handle some of these
		// exceptions with fallbacks to prevent undefined behavior or unexpected results.
		// We use navList here since it's `.size()` reflects the number of plugins that are shown in the dropdown.

		// Fallback in-case the user presses enter without a matching plugin or selection.
		if ((result && !forceDropdown) && navList.size() <= 0) {
			a_preview = "(Warning: No Plugin Found \"" + std::string(a_buffer) + "\")";
			ImFormatString(a_buffer, a_size, "%s", "");
			ImGui::CloseCurrentPopup();
			result = false;
		}

		// Fallback in-case the user presses enter with an empty buffer, but with a navList selection.
		if (result && navList.size() > 0) {
			ImFormatString(a_buffer, a_size, "%s", navSelection.name.c_str());
			ImGui::CloseCurrentPopup();
			result = true;
		}

		return result;
	}
}