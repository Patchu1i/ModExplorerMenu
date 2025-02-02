#include "include/A/AddItem.h"
#include "include/C/Console.h"
#include "include/I/ItemPreview.h"

namespace Modex
{

	/*
--------------------------------------------------------------------------------
	Kit Functions
--------------------------------------------------------------------------------
*/

	void AddItemWindow::AddKitToInventory(const Kit& a_kit)
	{
		if (a_kit.items.empty()) {
			return;
		}

		for (auto& item : a_kit.items) {
			if (RE::TESForm* form = RE::TESForm::LookupByEditorID(item->editorid)) {
				Console::AddItemEx(form->GetFormID(), item->amount, item->equipped);
			}
		}

		Console::StartProcessThread();
	}

	void AddItemWindow::PlaceKitAtMe(const Kit& a_kit)
	{
		if (a_kit.items.empty()) {
			return;
		}

		for (auto& item : a_kit.items) {
			if (RE::TESForm* form = RE::TESForm::LookupByEditorID(item->editorid)) {
				Console::PlaceAtMeFormID(form->GetFormID(), item->amount);
			}
		}

		Console::StartProcessThread();
	}

	void AddItemWindow::RemoveItemFromKit(const std::shared_ptr<KitItem>& a_item)
	{
		auto& collection = PersistentData::GetLoadedKits();
		auto& kit = collection.at(selectedKit);

		if (kit.items.empty()) {
			return;
		}

		for (auto it = kit.items.begin(); it != kit.items.end();) {
			if ((*it)->editorid == a_item->editorid) {
				it = kit.items.erase(it);
			} else {
				++it;
			}
		}

		PersistentData::GetSingleton()->SaveKitToJSON(kit);
	}

	// TODO: Const Correctness
	std::shared_ptr<KitItem> AddItemWindow::CreateKitItem(ItemData* a_item)
	{
		assert(a_item);

		auto newItem = std::make_shared<KitItem>();
		newItem->plugin = a_item->GetPluginName();
		newItem->name = a_item->GetName();
		newItem->editorid = a_item->GetEditorID();
		newItem->amount = 1;
		newItem->equipped = false;

		return newItem;
	}

	void AddItemWindow::AddItemToKit(const std::shared_ptr<KitItem>& a_item)
	{
		assert(a_item);

		auto& collection = PersistentData::GetLoadedKits();
		auto& kit = collection.at(selectedKit);

		// TODO: If we wrote a custom comparator operation for KitItem*, we could avoid this loop.
		if (!kit.items.empty()) {
			for (auto& item : kit.items) {
				if (item->editorid == a_item->editorid) {
					return;
				}
			}
		}

		kit.items.emplace(a_item);
		PersistentData::GetSingleton()->SaveKitToJSON(kit);
	}

	/*
--------------------------------------------------------------------------------
    Kit Context Menu
--------------------------------------------------------------------------------
*/

	void AddItemWindow::ShowKitItemContext(const std::shared_ptr<KitItem>& a_item)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));

		if (ImGui::Selectable(_T("Delete"), false, ImGuiSelectableFlags_DontClosePopups)) {
			RemoveItemFromKit(a_item);
			ImGui::CloseCurrentPopup();
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		if (ImGui::Selectable(_T("GENERAL_COPY_NAME"), false, ImGuiSelectableFlags_DontClosePopups)) {
			ImGui::LogToClipboard();
			ImGui::LogText(a_item->name.c_str());
			ImGui::LogFinish();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Selectable(_T("GENERAL_COPY_EDITOR_ID"), false, ImGuiSelectableFlags_DontClosePopups)) {
			ImGui::LogToClipboard();
			ImGui::LogText(a_item->editorid.c_str());
			ImGui::LogFinish();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Selectable(_T("GENERAL_KITS_SHOW_PLUGIN"), false, ImGuiSelectableFlags_DontClosePopups)) {
			selectedMod = a_item->plugin;
			primaryFilter = RE::FormType::None;
			itemSelectionList.clear();
			activeViewport = Viewport::TableView;
			ImGui::CloseCurrentPopup();
		}

		ImGui::PopStyleVar();
	}

	/*
--------------------------------------------------------------------------------
    Kit Action Bar
--------------------------------------------------------------------------------
*/

	void AddItemWindow::ShowKitBar()
	{
		ImGui::SubCategoryHeader(_T("KIT_SELECTION"));

		auto kits = PersistentData::GetLoadedKitNames();
		if (InputTextComboBox("##KitBar::Search", kitSearchBuffer, selectedKit, IM_ARRAYSIZE(kitSearchBuffer), kits, ImGui::GetContentRegionAvail().x)) {
			selectedKit = "None";

			if (selectedKit.find(kitSearchBuffer) != std::string::npos) {
				ImFormatString(kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), "");
			} else {
				for (auto& kit : kits) {
					if (kit == kitSearchBuffer) {
						selectedKit = kit;
						ImFormatString(kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), "");
						break;
					}
				}
			}
		}

		// TODO: Why is this needed? The box should automatically be padded...
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y);

		const float button_width = ((ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) / 2.0f);
		// const float window_padding = ImGui::GetStyle().ItemSpacing.x * 2;

		if (ImGui::GradientButton(_T("Create"), ImVec2(button_width, 0))) {
			ImGui::OpenPopup(_T("Create"));
		}

		if (ImGui::BeginPopupModal(_T("Create"), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text(_T("Create"));
			ImGui::Separator();

			if (ImGui::InputText("##KitName", newKitName, IM_ARRAYSIZE(newKitName), ImGuiInputTextFlags_EnterReturnsTrue)) {
				const std::string _name = std::string(newKitName);

				if (std::string(_name).empty()) {
					ImGui::CloseCurrentPopup();
				} else {
					Kit newKit(_name);
					PersistentData::GetLoadedKits().emplace(_name, newKit);
					selectedKit = _name;
					ImFormatString(kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), "");
					PersistentData::GetSingleton()->SaveKitToJSON(newKit);
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::SameLine();

			if (ImGui::Button(_T("Create"))) {
				const std::string _name = std::string(newKitName);

				if (std::string(_name).empty()) {
					ImGui::CloseCurrentPopup();
				} else {
					Kit newKit(_name);
					PersistentData::GetLoadedKits().emplace(_name, newKit);
					selectedKit = _name;
					ImFormatString(kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), "");
					PersistentData::GetSingleton()->SaveKitToJSON(newKit);
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndPopup();
		}

		ImGui::SameLine();

		if (ImGui::GradientButton(_T("Delete"), ImVec2(button_width, 0))) {
			if (selectedKit != "None") {
				ImGui::OpenPopup(_T("Delete"));
			}
		}

		if (ImGui::BeginPopupModal(_T("Delete"), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text(_T("KIT_DELETE_CONFIRMATION"));
			ImGui::NewLine();
			float center_text = ImGui::GetCenterTextPosX(selectedKit.c_str());
			ImGui::SetCursorPosX(center_text);
			ImGui::Text(selectedKit.c_str());
			ImGui::NewLine();

			if (ImGui::IsKeyPressed(ImGuiKey_Y, false)) {
				PersistentData::GetSingleton()->DeleteKit(selectedKit);
				selectedKit = "None";
				ImFormatString(kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), "");
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::IsKeyPressed(ImGuiKey_N, false)) {
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("(Y)es", ImVec2(button_width, 0))) {
				PersistentData::GetSingleton()->DeleteKit(selectedKit);
				selectedKit = "None";
				ImFormatString(kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), "");
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("(N)o", ImVec2(button_width, 0))) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (selectedKit != "None") {
			auto& collection = PersistentData::GetLoadedKits();
			if (collection.find(selectedKit) != collection.end()) {
				auto kit = collection.at(selectedKit);

				// Used for dependency checks in Popup and Table view.
				auto pluginList = Data::GetSingleton()->GetModulePluginList(Data::PLUGIN_TYPE::ITEM);

				// Display Kit Actions
				ImGui::SubCategoryHeader(_T("KIT_ACTIONS"));
				if (ImGui::GradientButton(_T("KIT_VIEW_IN_TABLE"), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
					std::vector<ItemData*> new_item_list;
					auto& all_items = Data::GetSingleton()->GetAddItemList();

					for (auto& data : all_items) {
						for (auto& kit_item : kit.items) {
							if (data.GetEditorID() == kit_item->editorid) {
								new_item_list.push_back(&data);
							}
						}
					}

					itemList = new_item_list;
					selectedMod = "All Mods";
					primaryFilter = RE::FormType::None;
					itemSelectionList.clear();
					activeViewport = Viewport::TableView;
				}

				if (ImGui::GradientButton(_T("Copy"), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
					auto new_kit = PersistentData::GetSingleton()->CopyKit(kit);
					selectedKit = new_kit.name;
					kit = new_kit;
					ImFormatString(kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), "");
				}

				if (ImGui::GradientButton(_T("Rename"), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
					ImGui::OpenPopup(_T("Rename"));
				}

				if (ImGui::BeginPopupModal(_T("Rename"), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
					ImGui::Text(_T("Rename"));
					ImGui::Separator();

					ImGui::InputText("##KitRename", kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::SameLine();
					if (ImGui::Button(_T("Rename"), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
						const std::string _name = std::string(kitSearchBuffer);

						if (std::string(_name).empty()) {
							ImGui::CloseCurrentPopup();
						} else if (std::string(_name) == selectedKit) {
							ImGui::CloseCurrentPopup();
						} else {
							kit = PersistentData::GetSingleton()->RenameKit(kit, _name);
							selectedKit = _name;
							ImFormatString(kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), "");
							ImGui::CloseCurrentPopup();
						}
					}

					ImGui::EndPopup();
				}

				if (ImGui::GradientButton(_T("Clear"), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
					auto& kit_ref = collection.at(selectedKit);
					kit_ref.items.clear();
					PersistentData::GetSingleton()->SaveKitToJSON(kit_ref);
				}

				if (ImGui::GradientButton(_T("Dependencies"), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
					ImGui::OpenPopup(_T("Dependencies"));
				}

				if (ImGui::BeginPopupModal(_T("Dependencies"), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
					ImGui::SubCategoryHeader(_T("Dependencies"));

					std::unordered_set<std::string> dependencies;

					for (auto& item : kit.items) {
						if (!dependencies.contains(item->plugin)) {
							dependencies.insert(item->plugin);
						}
					}

					for (auto& dependency : dependencies) {
						const RE::TESFile* plugin = RE::TESDataHandler::GetSingleton()->LookupModByName(dependency.c_str());

						if (pluginList.contains(plugin)) {
							ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), dependency.c_str());
						} else {
							ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), dependency.c_str());
						}
					}

					if (ImGui::GradientButton("Close", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				// const float maxWidth = ImGui::GetContentRegionAvail().x;
				// const auto InlineText = [maxWidth](const char* label, const char* text) {
				// 	const auto width = std::max(maxWidth - ImGui::CalcTextSize(text).x, ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(text).x);
				// 	ImGui::Text(label);
				// 	ImGui::SameLine(width);
				// 	ImGui::Text(text);
				// };

				// InlineText(_T("Name"), selectedKit.c_str());
				// InlineText(_T("Item Count"), std::to_string(kit.items.size()).c_str());

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

				const auto& style = Settings::GetSingleton()->GetStyle();
				ImGui::PushStyleColor(ImGuiCol_Button, style.secondaryButton);
				if (ImGui::GradientButton(_T("AIM_ADD"), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
					AddKitToInventory(kit);
				}

				if (ImGui::GradientButton(_T("AIM_PLACE"), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
					PlaceKitAtMe(kit);
				}
				ImGui::PopStyleColor();
				// }
				// ImGui::EndChild();
			}
		}
	}

	/*
--------------------------------------------------------------------------------
    Kit Table View
--------------------------------------------------------------------------------
*/

	// Display Kit Table
	void AddItemWindow::ShowKitTable()
	{
		ImGui::SubCategoryHeader(_T("KIT_ITEM_LIST"));

		auto& collection = PersistentData::GetLoadedKits();

		if (!collection.contains(selectedKit)) {
			return;
		}

		auto& style = Settings::GetSingleton()->GetStyle();
		auto& kit = collection.at(selectedKit);
		auto pluginList = Data::GetSingleton()->GetModulePluginList(Data::PLUGIN_TYPE::ITEM);

		auto rowBG = style.showTableRowBG ? ImGuiTableFlags_RowBg : 0;
		if (ImGui::BeginTable("##AddItemWindow::KitTableView", kitColumnList.GetTotalColumns(), Frame::TABLE_FLAGS | rowBG)) {
			ImGui::TableSetupScrollFreeze(1, 1);

			// Display Table Headers
			for (auto& column : kitColumnList.columns) {
				ImGui::TableSetupColumn(column.name.c_str(), column.flags, column.width, column.key);
			}

			ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
			int numOfColumn = 0;
			for (auto& column : kitColumnList.columns) {
				ImGui::TableSetColumnIndex(numOfColumn);
				ImGui::PushID(column.key);
				ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);  // (?)

				if (column.key == KitColumns::KitID::Equipped || column.key == KitColumns::KitID::Amount) {
					ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(column.name.c_str()));
				}

				ImGui::TableHeader(column.name.c_str());
				ImGui::PopID();

				numOfColumn++;
			}

			ImGuiListClipper clipper;
			ImGuiContext& g = *ImGui::GetCurrentContext();
			ImGuiTable* table = g.CurrentTable;

			// TODO FIX THIS. Issue with modifying the kit.items vector while iterating over it.
			std::vector<std::shared_ptr<KitItem>> kitItemList;

			if (kit.items.empty() || kit.items.size() == 0) {
				ImGui::EndTable();
				return;
			}

			for (auto& item : kit.items) {
				kitItemList.push_back(item);
			}

			int numOfRow = 0;
			clipper.Begin(static_cast<int>(kitItemList.size()));
			while (clipper.Step()) {
				for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
					auto& item = kitItemList[row];

					numOfRow++;

					auto tableID = std::string("##AddItemMenu::KitTableIndex-") + std::to_string(numOfRow);
					ImGui::PushID(tableID.c_str());

					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					// Plugin
					// This is a bit of a hack, but considering we're working inside a relatively small
					// clipper. There shouldn't be a need for further optimization. Since at MAX, maybe 50 calls
					// are going to be made to LookupModByName each frame.

					bool missing_plugin = false;
					const RE::TESFile* plugin = RE::TESDataHandler::GetSingleton()->LookupModByName(item->plugin.c_str());
					if (pluginList.contains(plugin)) {
						ImGui::Text(item->plugin.c_str());
					} else {
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), item->plugin.c_str());
						missing_plugin = true;
					}
					ImGui::TableNextColumn();

					// Name
					ImGui::Text(item->name.c_str());
					ImGui::TableNextColumn();

					// Editor ID
					ImGui::Text(item->editorid.c_str());
					ImGui::TableNextColumn();

					// Equipped
					ImGui::SetCursorPosX(ImGui::GetCenterTextPosX("N/A"));
					const RE::TESForm* form = RE::TESForm::LookupByEditorID(item->editorid);
					if (form != nullptr && (form->Is(RE::FormType::Armor) || form->Is(RE::FormType::Weapon))) {
						if (ImGui::Checkbox("##KitItem::Equipped", &item->equipped)) {
							PersistentData::GetSingleton()->SaveKitToJSON(kit);
						}
					} else {
						ImGui::TextDisabled("N/A");
					}

					ImGui::TableNextColumn();

					// Amount
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					if (ImGui::InputInt("##KitItem::Amount", &item->amount, 0, 0)) {
						PersistentData::GetSingleton()->SaveKitToJSON(kit);
					}

					// https://github.com/ocornut/imgui/issues/6588#issuecomment-1634424774
					// 1/11/2025 - Merged Table Input Handler function into this to better detect selected item.
					ImRect row_rect(
						table->WorkRect.Min.x,
						table->RowPosY1,
						table->WorkRect.Max.x,
						table->RowPosY2);
					row_rect.ClipWith(table->BgClipRect);

					bool bHover =
						ImGui::IsMouseHoveringRect(row_rect.Min, row_rect.Max, false) &&
						ImGui::IsWindowHovered(ImGuiHoveredFlags_None) &&
						!ImGui::IsAnyItemHovered();  // optional

					// Mirror Selection
					// if (!itemSelectionList.empty()) {
					// 	for (auto& selection : itemSelectionList) {
					// 		if (selection->GetEditorID() == item->editorid) {
					// 			table->RowBgColor[1] = ImGui::GetColorU32(ImGuiCol_Border, 0.65f);
					// 			kitSelectionList.insert(item);
					// 		}
					// 	}
					// }

					// Get the rows which the selection box is hovering over.
					// Two conditions based on inverted selection box.
					if (InputManager::GetMouseKitState() == InputManager::MOUSE_STATE::kBoxSelection) {
						// const auto selection_box = ImRect(InputManager::GetMouseSelectionStart(), InputManager::GetMouseSelectionEnd());
						const auto start = InputManager::GetMouseKitSelectionStart();
						const auto end = InputManager::GetMouseKitSelectionEnd();

						if (start.y > end.y) {
							if (row_rect.Overlaps(ImRect(end, start))) {
								if (!kitSelectionList.contains(item)) {
									kitSelectionList.insert(item);
								}
							} else {
								if (kitSelectionList.contains(item)) {
									kitSelectionList.erase(item);
								}
							}
						} else if (row_rect.Overlaps(ImRect(start, end))) {
							if (!kitSelectionList.contains(item)) {
								kitSelectionList.insert(item);
							}
						} else {
							if (kitSelectionList.contains(item)) {
								kitSelectionList.erase(item);
							}
						}
					}

					if (kitSelectionList.contains(item)) {
						table->RowBgColor[1] = ImGui::GetColorU32(ImGuiCol_Border, 0.65f);
					}

					if (bHover) {
						table->RowBgColor[1] = ImGui::GetColorU32(ImGuiCol_Border);
						kitPreview = item;

						if (ImGui::IsMouseClicked(1)) {
							ImGui::OpenPopup("ShowKitItemContextMenu");
						}

						if (missing_plugin) {
							ImGui::SetTooltip(_T("TOOLTIP_MISSING_PLUGIN"));
						}
					}

					if (!pluginList.contains(plugin)) {
						table->RowBgColor[1] = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.0f, 0.0f, 0.1f));
					}

					if (ImGui::BeginPopup("ShowKitItemContextMenu")) {
						ShowKitItemContext(item);
						ImGui::EndPopup();
					}

					ImGui::PopID();
				}
			}

			// TODO: This behavior quickly got a little bigger than anticipated.
			// Should move this system into its own class or interface for table view

			const bool is_popup_open = ImGui::IsPopupOpen("ShowKitItemContextMenu", ImGuiPopupFlags_AnyPopup);

			if (!is_popup_open) {
				const ImVec2 mousePos = ImGui::GetMousePos();

				// We shouldn't have a horizontal scroll bar, so we do not need to account for it.
				// Doing so results in the last item of the table not being selectable.
				const float outer_width = table->OuterRect.Max.x - ImGui::GetStyle().ScrollbarSize;
				const float outer_height = table->OuterRect.Max.y;

				auto IsMouseInBoundsKit = [&]() -> bool {
					return (mousePos.x < outer_width && mousePos.x > table->OuterRect.Min.x) &&
					       (mousePos.y < outer_height && mousePos.y > table->OuterRect.Min.y);
				};

				if (IsMouseInBoundsKit()) {
					if (ImGui::IsMouseClicked(0)) {
						InputManager::SetMouseKitSelectionStart(ImGui::GetMousePos());
						wasMouseInBoundsKit = true;
					}

					if (wasMouseInBoundsKit && InputManager::GetMouseKitState() == InputManager::MOUSE_STATE::kIdle) {
						if (ImGui::IsMouseDragging(0, 15.0f)) {
							InputManager::SetMouseKitState(InputManager::MOUSE_STATE::kBoxSelection);
							kitSelectionList.clear();
						}
					}
				}

				if (ImGui::IsMouseReleased(0)) {
					if (wasMouseInBoundsKit) {
						switch (InputManager::GetMouseKitState()) {
						case InputManager::MOUSE_STATE::kIdle:
							if (!kitSelectionList.contains(kitPreview)) {
								kitSelectionList.clear();
								kitSelectionList.insert(kitPreview);
							} else {
								if (kitSelectionList.size() > 1) {
									kitSelectionList.clear();
									kitSelectionList.insert(kitPreview);
								}
							}
							break;
						case InputManager::MOUSE_STATE::kBoxSelection:
							break;
						}
					} else {
						kitSelectionList.clear();
					}

					wasMouseInBoundsKit = false;
					InputManager::ClearMouseKitState();
				}

				// Draw Selection Rectangle
				if (InputManager::GetMouseKitState() == InputManager::MOUSE_STATE::kBoxSelection) {
					const auto mouseEnd = ImVec2(
						std::clamp(
							ImGui::GetMousePos().x,
							table->OuterRect.Min.x,
							table->OuterRect.Max.x - ImGui::GetStyle().ScrollbarSize),
						std::clamp(
							ImGui::GetMousePos().y,
							table->OuterRect.Min.y,
							table->OuterRect.Max.y - 1.0f));

					InputManager::SetMouseKitSelectionEnd(mouseEnd);
					const auto mouseDragStart = InputManager::GetMouseKitSelectionStart();

					ImGui::GetWindowDrawList()->AddRect(mouseDragStart, mouseEnd, IM_COL32(255, 255, 255, 200));
					ImGui::GetWindowDrawList()->AddRectFilled(mouseDragStart, mouseEnd, IM_COL32(255, 255, 255, 10));
				}
			}

			ImGui::EndTable();
		}
	}

	/*
--------------------------------------------------------------------------------
    Kit Plugin View
--------------------------------------------------------------------------------
*/

	void AddItemWindow::ShowKitPluginView()
	{
		// TODO: Optimize this better. Iterating over every item in every kit to determine
		// whether it's part of a plugin is atrocious. We can maybe add a "Plugin" or "Dependency"
		// list to the JSON object to more easily reference since Dependencies also require this same
		// style of iterator. Or we can cache and update each kits plugin list on save/load, and keep
		// it in memory when it's loaded.

		const auto& a_style = Settings::GetSingleton()->GetStyle();

		if (showKitsForPlugin && selectedMod != _T("All Mods")) {
			const std::string header_label = "Kits found using " + selectedMod;
			ImGui::SubCategoryHeader(header_label.c_str());
			// if (ImGui::BeginChild("##AddItem::PluginKitView", ImVec2(ImGui::GetContentRegionAvail().x, 150.0f))) {
			auto& kits = PersistentData::GetLoadedKits();

			for (auto& [name, kit] : kits) {
				for (auto& item : kit.items) {
					if (item->plugin == selectedMod) {
						if (!kitsFound.contains(&kit)) {
							kitsFound.insert(&kit);
							continue;
						}
					}
				}
			}

			kitHoveredInTableView = nullptr;

			if (kitsFound.empty()) {
				return;
			} else {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(a_style.button.x, a_style.button.y, a_style.button.z, a_style.button.w * 0.50f));
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(a_style.text.x, a_style.text.y, a_style.text.z, a_style.text.w * 0.50f));
				for (auto& kit : kitsFound) {
					if (ImGui::GradientButton(kit->name.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
						if (b_AddToInventory) {
							AddKitToInventory(*kit);
						} else if (b_PlaceOnGround) {
							PlaceKitAtMe(*kit);
						}
					}

					if (ImGui::IsItemHovered()) {
						kitHoveredInTableView = kit;
					}
				}
				ImGui::PopStyleColor(2);
			}
		}
	}
}