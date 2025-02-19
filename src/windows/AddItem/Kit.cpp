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

	// These functions can't really be abstracted, since they are specific to both KitItem and AddItem
	// For example, we have to call a custom Console Command on a KitItem type. So each module
	// that implements kits will have to write their own functions for this.

	// Maybe we can write an abstract class to implement these functions?

	void AddItemWindow::AddKitToInventory(const Kit& a_kit)
	{
		if (a_kit.items.empty()) {
			return;
		}

		if (a_kit.clearEquipped) {
			Console::ClearEquipped();
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

	// If we ever change or expose the option to configure the GetFile(0) command
	// We will need to reimplement those changes here, so that kits aren't created with
	// the incorrect plugin assignment (not that it really matters).
	void AddItemWindow::CreateKitFromEquipped()
	{
		auto* player = RE::PlayerCharacter::GetSingleton();

		if (!player) {
			return;
		}

		auto& collection = PersistentData::GetLoadedKits();

		if (auto it = collection.find(selectedKit); it != collection.end()) {
			auto& kit = it->second;

			for (auto& item : player->GetInventory()) {
				// auto& boundObject = item.first->As<RE::TESObjectREFR>();
				auto& boundObject = item.first;
				auto& pair = item.second;

				if (pair.second.get() && pair.second->IsWorn()) {
					auto kitItem = std::make_shared<KitItem>();
					kitItem->plugin = boundObject->GetFile(0)->GetFilename();
					kitItem->name = boundObject->GetName();
					kitItem->editorid = po3_GetEditorID(boundObject->GetFormID());
					kitItem->amount = pair.first;
					kitItem->equipped = true;

					if (kit.items.contains(kitItem)) {
						continue;
					} else {
						kit.items.emplace(kitItem);
					}
				} else {
					continue;
				}
			}

			PersistentData::GetSingleton()->SaveKitToJSON(kit);
			this->kitTableView.Refresh();
		}
	}

	// void AddItemWindow::RemoveItemFromKit(const std::shared_ptr<KitItem>& a_item)
	// {
	// 	auto& collection = PersistentData::GetLoadedKits();
	// 	auto& kit = collection.at(selectedKit);

	// 	if (kit.items.empty()) {
	// 		return;
	// 	}

	// 	for (auto it = kit.items.begin(); it != kit.items.end();) {
	// 		if ((*it)->editorid == a_item->editorid) {
	// 			it = kit.items.erase(it);
	// 		} else {
	// 			++it;
	// 		}
	// 	}

	// 	PersistentData::GetSingleton()->SaveKitToJSON(kit);
	// }

	// std::shared_ptr<KitItem> AddItemWindow::CreateKitItem(const ItemData* a_item)
	// {
	// 	assert(a_item);

	// 	auto newItem = std::make_shared<KitItem>();
	// 	newItem->plugin = a_item->GetPluginName();
	// 	newItem->name = a_item->GetName();
	// 	newItem->editorid = a_item->GetEditorID();
	// 	newItem->amount = a_item->kitAmount;
	// 	newItem->equipped = a_item->kitEquipped;

	// 	return newItem;
	// }

	// void AddItemWindow::AddItemToKit(const std::shared_ptr<KitItem>& a_item)
	// {
	// 	assert(a_item);

	// 	auto& collection = PersistentData::GetLoadedKits();
	// 	auto& kit = collection.at(selectedKit);

	// 	if (kit.items.contains(a_item)) {
	// 		return;
	// 	}

	// 	kit.items.emplace(a_item);
	// 	PersistentData::GetSingleton()->SaveKitToJSON(kit);
	// }

	/*
--------------------------------------------------------------------------------
    Kit Action Bar
--------------------------------------------------------------------------------
*/

	// This seems eligible for abstraction into the tableview or another class,
	// but it would require some refactoring of the tableview class.

	void AddItemWindow::ShowKitBar()
	{
		ImGui::SubCategoryHeader(_T("KIT_SELECTION"));

		constexpr auto popup_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

		auto kits = PersistentData::GetLoadedKitNames();
		if (InputTextComboBox("##KitBar::Search", kitSearchBuffer, selectedKit, IM_ARRAYSIZE(kitSearchBuffer), kits, ImGui::GetContentRegionAvail().x)) {
			selectedKit = _T("None");

			if (selectedKit.find(kitSearchBuffer) != std::string::npos) {
				ImFormatString(kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), "");
			} else {
				for (auto& kit : kits) {
					if (kit == kitSearchBuffer) {
						selectedKit = kit;
						ImFormatString(kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), "");
						kitTableView.Refresh();
						break;
					}
				}
			}
		}

		// TODO: Why is this needed? The box should automatically be padded...
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y);

		const float button_width = ((ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) / 2.0f);

		if (ImGui::GradientButton(_T("Create"), ImVec2(button_width, 0))) {
			ImGui::OpenPopup(_T("##Create"));
		}

		ImGui::SetNextWindowSize(ImVec2(button_width * 2.0f, 0));
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal(_T("##Create"), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {
			ImGui::SubCategoryHeader(_T("Create"));
			ImGui::Separator();

			ImGui::NewLine();

			ImGui::PushID("##CreateNewKit");

			const std::string instruction = _T("KIT_CREATE_INSTRUCTION");
			float center_text = ImGui::GetCenterTextPosX(instruction.c_str());
			ImGui::SetCursorPosX(center_text);
			ImGui::Text(instruction.c_str());

			ImGui::SetNextItemWidth(button_width);
			ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x / 2.0f - (button_width / 2.0f));
			if (ImGui::InputText("##KitName", newKitName, IM_ARRAYSIZE(newKitName), ImGuiInputTextFlags_EnterReturnsTrue)) {
				const std::string _name = std::string(newKitName);

				if (std::string(_name).empty()) {
					ImGui::CloseCurrentPopup();
				} else {
					Kit newKit(_name, 0);
					PersistentData::GetLoadedKits().emplace(_name, newKit);
					selectedKit = _name;
					kitTableView.Refresh();
					ImFormatString(kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), "");
					PersistentData::GetSingleton()->SaveKitToJSON(newKit);
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::NewLine();

			if (ImGui::Button(_T("Create"), ImVec2(button_width, 0))) {
				const std::string _name = std::string(newKitName);

				if (std::string(_name).empty()) {
					ImGui::CloseCurrentPopup();
				} else {
					Kit newKit(_name, 0);
					PersistentData::GetLoadedKits().emplace(_name, newKit);
					selectedKit = _name;
					kitTableView.Refresh();
					ImFormatString(kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), "");
					PersistentData::GetSingleton()->SaveKitToJSON(newKit);
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::SameLine();

			if (ImGui::Button(_T("Cancel"), ImVec2(button_width, 0))) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::PopID();

			ImGui::EndPopup();
		}

		ImGui::SameLine();

		if (ImGui::GradientButton(_T("Delete"), ImVec2(button_width, 0))) {
			if (selectedKit != "None") {
				ImGui::OpenPopup(_T("Delete"));
			}
		}

		if (ImGui::BeginPopupModal(_T("Delete"), nullptr, popup_flags)) {
			ImGui::SubCategoryHeader(_T("KIT_DELETE_CONFIRMATION"));
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			ImGui::NewLine();

			float center_text = ImGui::GetCenterTextPosX(selectedKit.c_str());
			ImGui::SetCursorPosX(center_text);
			ImGui::Text(selectedKit.c_str());

			ImGui::NewLine();

			if (ImGui::IsKeyPressed(ImGuiKey_Y, false)) {
				PersistentData::GetSingleton()->DeleteKit(selectedKit);
				selectedKit = _T("None");
				kitTableView.Refresh();
				ImFormatString(kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), "");
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::IsKeyPressed(ImGuiKey_N, false)) {
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("(Y)es", ImVec2(button_width, 0))) {
				PersistentData::GetSingleton()->DeleteKit(selectedKit);
				selectedKit = _T("None");
				kitTableView.Refresh();
				ImFormatString(kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), "");
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("(N)o", ImVec2(button_width, 0))) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::NewLine();

#ifdef DEBUG
		if (ImGui::Button("Patcher", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			auto& collection = PersistentData::GetLoadedKits();

			for (auto& pair : collection) {
				auto& kit = pair.second;

				kit.readOnly = true;

				PersistentData::GetSingleton()->SaveKitToJSON(kit);
			}
		}
#endif

		if (selectedKit != "None") {
			auto& collection = PersistentData::GetLoadedKits();
			if (collection.find(selectedKit) != collection.end()) {
				const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
				auto kit = collection.at(selectedKit);

				// Display Kit Actions
				ImGui::SubCategoryHeader(_T("KIT_ACTIONS"));

				if (ImGui::GradientButton(_T("GENERAL_CLEAR_INVENTORY"), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
					ImGui::OpenPopup("##Kit::ClearInventory::Confirmation");
				}

				if (ImGui::BeginPopupModal(_T("##Kit::ClearInventory::Confirmation"), nullptr, popup_flags)) {
					ImGui::SubCategoryHeader(_T("GENERAL_CLEAR_INVENTORY"));
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

					ImGui::NewLine();

					const std::string instruction = _T("GENERAL_CLEAR_INVENTORY_INSTRUCTION");
					float center_text = ImGui::GetCenterTextPosX(instruction.c_str());
					ImGui::SetCursorPosX(center_text);
					ImGui::Text(instruction.c_str());

					ImGui::NewLine();

					bool confirm = false;
					if (ImGui::IsKeyPressed(ImGuiKey_Y, false)) {
						confirm = true;
					}

					if (ImGui::IsKeyPressed(ImGuiKey_N, false)) {
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::Button("(Y)es", ImVec2(button_width, 0))) {
						confirm = true;
					}

					ImGui::SameLine();

					if (ImGui::Button("(N)o", ImVec2(button_width, 0))) {
						ImGui::CloseCurrentPopup();
					}

					if (confirm) {
						Console::ClearInventory();
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				if (kit.readOnly) {
					ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_Button, 0.25f));
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
				}

				if (ImGui::GradientButton(_T("KIT_ADD_EQUIPPED"), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
					if (!kit.readOnly) {
						CreateKitFromEquipped();
					}
				}

				if (kit.readOnly) {
					ImGui::PopStyleColor(2);
				}

				if (ImGui::IsItemHovered()) {
					if (!kit.readOnly) {
						ImGui::SetTooltip(_T("KIT_ADD_EQUIPPED"));
					} else {
						ImGui::SetTooltip(_T("KIT_IS_READ_ONLY"));
					}
				}

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

				if (ImGui::GradientButton(_T("Copy"), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
					auto new_kit = PersistentData::GetSingleton()->CopyKit(kit);
					selectedKit = new_kit.name;
					kitTableView.Refresh();
					kit = new_kit;
					ImFormatString(kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), "");
				}

				if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary)) {
					ImGui::SetTooltip(_T("TOOLTIP_KIT_COPY"));
				}

				if (kit.readOnly) {
					ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_Button, 0.25f));
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
				}

				if (ImGui::GradientButton(_T("Rename"), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
					if (!kit.readOnly) {
						ImFormatString(kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), selectedKit.c_str());
						ImGui::OpenPopup(_T("Rename"));
					}
				}

				if (kit.readOnly) {
					ImGui::PopStyleColor(2);
				}

				if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary)) {
					if (!kit.readOnly) {
						ImGui::SetTooltip(_T("TOOLTIP_KIT_RENAME"));
					} else {
						ImGui::SetTooltip(_T("KIT_IS_READ_ONLY"));
					}
				}

				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize(ImVec2(button_width * 2.0f, 0));
				if (ImGui::BeginPopupModal(_T("Rename"), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {
					ImGui::SubCategoryHeader(_T("Rename"));

					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					ImGui::InputText("##KitRename", kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), ImGuiInputTextFlags_EnterReturnsTrue);

					ImGui::NewLine();

					if (ImGui::GradientButton(_T("Accept"), ImVec2(ImGui::GetContentRegionAvail().x / 2, 0))) {
						const std::string _name = std::string(kitSearchBuffer);

						if (std::string(_name).empty()) {
							ImGui::CloseCurrentPopup();
						} else if (std::string(_name) == selectedKit) {
							ImGui::CloseCurrentPopup();
						} else {
							kit = PersistentData::GetSingleton()->RenameKit(kit, _name);
							selectedKit = _name;
							kitTableView.Refresh();
							ImFormatString(kitSearchBuffer, IM_ARRAYSIZE(kitSearchBuffer), "");
							ImGui::CloseCurrentPopup();
						}
					}

					ImGui::SameLine();

					if (ImGui::GradientButton(_T("Cancel"), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				if (kit.readOnly) {
					ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_Button, 0.25f));
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
				}
				if (ImGui::GradientButton(_T("Clear"), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
					if (!kit.readOnly) {
						ImGui::OpenPopup(_T("Clear"));
					}
				}

				if (kit.readOnly) {
					ImGui::PopStyleColor(2);
				}

				if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary)) {
					if (!kit.readOnly) {
						ImGui::SetTooltip(_T("TOOLTIP_KIT_CLEAR"));
					} else {
						ImGui::SetTooltip(_T("KIT_IS_READ_ONLY"));
					}
				}

				if (ImGui::BeginPopupModal(_T("Clear"), nullptr, popup_flags)) {
					ImGui::SubCategoryHeader(_T("KIT_CLEAR_CONFIRMATION"));
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

					ImGui::NewLine();

					float center_text = ImGui::GetCenterTextPosX(selectedKit.c_str());
					ImGui::SetCursorPosX(center_text);
					ImGui::Text(selectedKit.c_str());

					ImGui::NewLine();

					bool confirm = false;
					if (ImGui::IsKeyPressed(ImGuiKey_Y, false)) {
						confirm = true;
					}

					if (ImGui::IsKeyPressed(ImGuiKey_N, false)) {
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::Button("(Y)es", ImVec2(button_width, 0))) {
						confirm = true;
					}

					ImGui::SameLine();

					if (ImGui::Button("(N)o", ImVec2(button_width, 0))) {
						ImGui::CloseCurrentPopup();
					}

					if (confirm) {
						auto& kit_ref = collection.at(selectedKit);
						kit_ref.items.clear();
						kitTableView.Refresh();
						PersistentData::GetSingleton()->SaveKitToJSON(kit_ref);
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				if (ImGui::GradientButton(_T("Dependencies"), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
					ImGui::OpenPopup(_T("Dependencies"));
				}

				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize(ImVec2(400, 0));
				if (ImGui::BeginPopupModal(_T("Dependencies"), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {
					ImGui::SubCategoryHeader(_T("Dependencies"));

					std::unordered_set<std::string> dependencies;

					for (auto& item : kit.items) {
						if (!dependencies.contains(item->plugin)) {
							dependencies.insert(item->plugin);
						}
					}

					// Used for dependency checks in Popup and Table view.
					auto pluginList = Data::GetSingleton()->GetModulePluginList(Data::PLUGIN_TYPE::ITEM);

					for (auto& dependency : dependencies) {
						const RE::TESFile* plugin = RE::TESDataHandler::GetSingleton()->LookupModByName(dependency.c_str());

						ImGui::SetCursorPosX(ImGui::GetCenterTextPosX(dependency.c_str()));
						if (pluginList.contains(plugin)) {
							ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), dependency.c_str());
						} else {
							ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), dependency.c_str());
						}
					}

					ImGui::NewLine();

					if (ImGui::GradientButton("Close", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

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

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

				{
					auto& kit_ref = collection.at(selectedKit);
					const float width = 100.0f;
					ImGui::Text(_T("KIT_CLEAR_EQUIPPED"));
					ImGui::SameLine(ImGui::GetContentRegionMax().x - width);
					if (!kit.readOnly) {
						if (ImGui::ToggleButton("##ToggleClearEquip", &kit_ref.clearEquipped, width)) {
							PersistentData::GetSingleton()->SaveKitToJSON(kit_ref);
						}
					} else {
						ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetColorU32(ImGuiCol_FrameBg, 0.25f));
						ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImGui::GetColorU32(ImGuiCol_SliderGrab, 0.25f));
						bool locked = kit_ref.clearEquipped;
						ImGui::ToggleButton("##ToggleClearEquip", &locked, width);
						ImGui::PopStyleColor(2);
					}

					ImGui::Text(_T("KIT_READ_ONLY"));
					ImGui::SameLine(ImGui::GetContentRegionMax().x - width);
					if (ImGui::ToggleButton("##ToggleReadOnly", &kit_ref.readOnly, width)) {
						PersistentData::GetSingleton()->SaveKitToJSON(kit_ref);
					}
				}
			}
		}
	}
}