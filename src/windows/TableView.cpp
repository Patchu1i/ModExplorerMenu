#include "include/C/Console.h"
#include "include/T/Table.h"
#include <execution>

namespace Modex
{

	template class TableView<ItemData>;
	template class TableView<NPCData>;
	template class TableView<ObjectData>;

	// We instantiate a TableView with a specific Module handle so that
	// some sorting and filtering options are routed correctly, since as of now
	// we store the BaseObject instead of a more polymorphic type.
	void TableView<ItemData>::Init()
	{
		this->inputSearchMap = {
			{ SortType::Name, "Name" },
			{ SortType::EditorID, "Editor ID" },
			{ SortType::FormID, "Form ID" }
		};

		this->primaryFilterList = {
			RE::FormType::Armor,
			RE::FormType::AlchemyItem,
			RE::FormType::Ammo,
			RE::FormType::Book,
			RE::FormType::Ingredient,
			RE::FormType::KeyMaster,
			RE::FormType::Misc,
			RE::FormType::Weapon
		};

		this->sortByList = {
			SortType::Plugin,
			SortType::Type,
			SortType::FormID,
			SortType::Name,
			SortType::EditorID,
			SortType::Weight,
			SortType::Value,
			SortType::Damage,
			SortType::Armor,
			SortType::Slot,
			SortType::Speed,
			SortType::CriticalDamage,
			SortType::Skill,
			SortType::DamagePerSecond
		};
	}

	void TableView<NPCData>::Init()
	{
		this->inputSearchMap = {
			{ SortType::Name, "Name" },
			{ SortType::EditorID, "Editor ID" },
			{ SortType::FormID, "Form ID" }
		};

		this->primaryFilterList = {
			RE::FormType::Class,
			RE::FormType::Race,
			RE::FormType::Faction
		};
	}

	void TableView<ObjectData>::Init()
	{
		this->inputSearchMap = {
			{ SortType::Name, "Name" },
			{ SortType::EditorID, "Editor ID" },
			{ SortType::FormID, "Form ID" }
		};

		this->primaryFilterList = {
			RE::FormType::Activator,
			RE::FormType::Flora,
			RE::FormType::Furniture,
			RE::FormType::Static,
			RE::FormType::Tree
		};
	}

	template <typename DataType>
	void TableView<DataType>::AddDragDropTarget(const std::string a_id, TableView* a_view)
	{
		this->dragDropSourceList[a_id] = a_view;
	}

	template <typename DataType>
	void TableView<DataType>::SetDragDropID(const std::string& a_id)
	{
		this->dragDropSourceID = a_id;
	}

	template <typename DataType>
	void TableView<DataType>::SetGenerator(std::function<std::vector<DataType>()> a_generator)
	{
		this->generator = a_generator;
	}

	template <typename DataType>
	void TableView<DataType>::SetupSearch(const Data::PLUGIN_TYPE& a_pluginType)
	{
		this->pluginType = a_pluginType;
		this->AddFlag(TableFlag::ModexTableFlag_EnableSearch);
	}

	template <typename DataType>
	std::vector<DataType> TableView<DataType>::GetSelection()
	{
		std::vector<DataType> selectedItems;

		for (auto& item : this->tableList) {
			bool is_item_selected = selectionStorage.Contains(item->TableID);
			if (is_item_selected) {
				selectedItems.push_back(*item);
			}
		}

		return selectedItems;
	}

	template <typename DataType>
	void TableView<DataType>::AddPayloadItemToKit(const std::unique_ptr<DataType>& a_item)
	{
		for (auto& item : this->tableList) {
			if (item->GetEditorID() == a_item->GetEditorID()) {
				return;
			}
		}

		this->tableList.emplace_back(std::make_unique<DataType>(*a_item));
	}

	template <typename DataType>
	void TableView<DataType>::RemoveSelected()
	{
		if (this->tableList.empty()) {
			return;
		}

		auto selectedItems = this->GetSelection();

		if (selectedItems.empty()) {
			return;
		}
	}

	// Deprecated, moved into Drag Drop operation directly. (?)

	template <typename DataType>
	void TableView<DataType>::RemovePayloadItemFromKit(const std::unique_ptr<DataType>& a_item)
	{
		IM_UNUSED(a_item);
		// if (this->tableList.empty()) {
		// 	return;
		// }

		// this->tableList.erase(
		// 	std::remove_if(this->tableList.begin(), this->tableList.end(),
		// 		[&a_item](const std::unique_ptr<DataType>& item) {
		// 			return item->GetEditorID() == a_item->GetEditorID();
		// 		}),
		// 	this->tableList.end());
	}

	template <typename DataType>
	void TableView<DataType>::AddSelectionToInventory(int a_count)
	{
		// TODO: Fix or implement this better
		if (a_count < 0) {
			a_count = 1;
		}

		if (this->tableList.empty()) {
			return;
		}

		auto selectedItems = this->GetSelection();

		if (selectedItems.empty()) {
			return;
		}

		for (auto& item : selectedItems) {
			Console::AddItem(item.GetFormID().c_str(), a_count);
		}

		Console::StartProcessThread();
	}

	template <typename DataType>
	void TableView<DataType>::PlaceSelectionOnGround(int a_count)
	{
		// TODO: Fix or implement this better
		if (a_count < 0) {
			a_count = 1;
		}

		if (this->tableList.empty()) {
			return;
		}

		auto selectedItems = this->GetSelection();

		if (selectedItems.empty()) {
			return;
		}

		for (auto& item : selectedItems) {
			Console::PlaceAtMe(item.GetFormID().c_str(), a_count);
		}

		Console::StartProcessThread();
	}

	template <typename DataType>
	void TableView<DataType>::SyncChangesToKit()
	{
		if (this->HasFlag(ModexTableFlag_Kit)) {
			if (this->selectedKit) {
				auto& collection = PersistentData::GetLoadedKits();
				auto& kit = collection.at(*this->selectedKit);

				kit.items.clear();

				for (auto& item : this->tableList) {
					kit.items.emplace(CreateKitItem(*item));
				}

				PersistentData::GetSingleton()->SaveKitToJSON(kit);
			}
		}
	}

	// Until we refactor the DataTypes used in tables to be polymorphic or whatever,
	// we have to write an ApplyFilter behavior for each module individually. Only
	// a little of the search code is shared between modules, independent of class type.
	template <typename DataType>
	void TableView<DataType>::ApplyFilters()
	{
		if (this->generator) {
			this->Filter(this->generator());
		}
	}

	template <typename DataType>
	void TableView<DataType>::UpdateMasterList()
	{
		this->tableList.clear();
		this->ApplyFilters();
		this->ApplySearch();
		this->tableList = std::move(this->searchList);
		this->UpdateImGuiTableIDs();
	}

	template <typename DataType>
	void TableView<DataType>::UpdateSearch()
	{
		this->searchList.clear();
		this->ApplySearch();
		this->tableList = std::move(this->searchList);
		this->UpdateImGuiTableIDs();
	}

	template <typename DataType>
	void TableView<DataType>::Refresh()
	{
		if (this->HasFlag(ModexTableFlag_Kit)) {
			if (this->selectedKit) {
				this->tableList.clear();
				this->ApplyFilters();
				// this->tableList = this->filterList;
				this->tableList = std::move(this->filterList);

				// Automatically sort the kit table.
				// Too heavy for main table.
				this->SortListBySpecs();
				this->UpdateImGuiTableIDs();
			}
		} else {
			this->UpdateMasterList();

			// In-table kit view is disabled if there are no kits in the plugin.
			if (this->kitList.empty()) {
				this->showKitView = false;
			}
		}
	}

	template <typename DataType>
	void TableView<DataType>::Reset()
	{
		this->tableList.clear();
		this->itemPreview = nullptr;
		this->primaryFilter = RE::FormType::None;
		this->secondaryFilter = _T("All");
		this->selectedPlugin = _T("SHOW_ALL_PLUGINS");
		ImFormatString(this->generalSearchBuffer, IM_ARRAYSIZE(this->generalSearchBuffer), "");
		ImFormatString(this->pluginSearchBuffer, IM_ARRAYSIZE(this->pluginSearchBuffer), "");
		ImFormatString(this->secondaryFilterBuffer, IM_ARRAYSIZE(this->secondaryFilterBuffer), "");
		this->BuildPluginList();
		this->Refresh();
	}

	// Abstracting out search behavior from the filter to hopefully increase performance.
	// Since filtering is a lot more common than searching, and filtering is expensive, maybe
	// we can quicken both operations by decoupling them
	template <typename DataType>
	void TableView<DataType>::ApplySearch()
	{
		searchList.clear();
		searchList.reserve(std::ssize(this->filterList));

		if (this->generalSearchBuffer[0] == '\0') {
			// If the search buffer is empty, we just copy the data over.
			for (const auto& item : this->filterList) {
				this->searchList.emplace_back(std::make_unique<DataType>(item.get()->GetForm(), 0));
			}

			return;
		}

		std::string inputString = this->generalSearchBuffer;
		std::transform(inputString.begin(), inputString.end(), inputString.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

		for (const auto& item : this->filterList) {
			const auto& item_data = item.get();
			std::string compareString;

			switch (this->searchKey) {
			case SortType::Name:
				compareString = item_data->GetNameView();
				break;
			case SortType::FormID:
				compareString = item_data->GetFormID();
				break;
			case SortType::EditorID:
				compareString = item_data->GetEditorID();
				break;
			default:
				compareString = item_data->GetNameView();
				break;
			}

			std::transform(compareString.begin(), compareString.end(), compareString.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

			// If the input is wrapped in quotes, we do an exact match across all parameters.
			if (!inputString.empty() && inputString.front() == '"' && inputString.back() == '"') {
				std::string match = inputString.substr(1, inputString.size() - 2);

				if (compareString == match) {
					this->searchList.emplace_back(std::make_unique<DataType>(item_data->GetForm(), 0));
				}

				continue;
			}

			if (compareString.find(inputString) != std::string::npos) {
				this->searchList.emplace_back(std::make_unique<DataType>(item_data->GetForm(), 0));
			}
		}
	}

	template <typename DataType>
	void TableView<DataType>::Filter(const std::vector<DataType>& a_data)
	{
		filterList.clear();
		filterList.reserve(std::ssize(a_data));

		// We need to keep track of the items table ID based on whether an item is shown or not.
		// Table ID is used to identify the item in the table, mostly for multi-selection.
		// Typically, it corresponds to the row index in the table.

		for (const auto& item : a_data) {
			if (this->HasFlag(ModexTableFlag_Kit)) {
				this->filterList.emplace_back(std::make_unique<DataType>(item.GetForm(), 0));
				continue;
			}

			// All Mods vs Selected Mod
			if (this->selectedPlugin != _T("SHOW_ALL_PLUGINS") && item.GetPluginName() != this->selectedPlugin) {
				continue;
			}

			// Blacklist
			if (this->selectedPlugin == _T("SHOW_ALL_PLUGINS")) {
				if (PersistentData::GetBlacklist().contains(item.GetPlugin())) {
					continue;
				}
			}

			// Item Specific compile-time Filters
			if constexpr (std::is_same_v<DataType, ItemData>) {
				if (this->hideNonPlayable && item.IsNonPlayable()) {
					continue;
				}

				if (item.GetFormType() == RE::FormType::Armor && this->hideEnchanted) {
					if (const auto& armor = item.GetForm()->As<RE::TESObjectARMO>()) {
						if (this->hideEnchanted && armor->formEnchanting != nullptr) {
							continue;
						}
					}
				}

				if (item.GetFormType() == RE::FormType::Weapon && this->hideEnchanted) {
					if (const auto& armor = item.GetForm()->As<RE::TESObjectWEAP>()) {
						if (this->hideEnchanted && armor->formEnchanting != nullptr) {
							continue;
						}
					}
				}
			}

			// Primary Filter
			if (this->primaryFilter != RE::FormType::None) {
				if (item.GetFormType() != this->primaryFilter) {
					continue;
				}
			}

			// Secondary Filter
			if (this->secondaryFilter != _T("All")) {
				if (this->primaryFilter == RE::FormType::Armor && item.GetFormType() == RE::FormType::Armor) {
					auto armorSlots = Utils::GetArmorSlots(item.GetForm()->As<RE::TESObjectARMO>());
					if (std::find(armorSlots.begin(), armorSlots.end(), this->secondaryFilter) == armorSlots.end()) {
						continue;
					}
				}

				if (this->primaryFilter == RE::FormType::Weapon && item.GetFormType() == RE::FormType::Weapon) {
					auto weaponType = item.GetForm()->As<RE::TESObjectWEAP>()->GetWeaponType();

					// We gonna just keep pasting this everywhere I guess..
					const char* weaponTypes[] = {
						"Hand to Hand",
						"One Handed Sword",
						"One Handed Dagger",
						"One Handed Axe",
						"One Handed Mace",
						"Two Handed Greatsword",
						"Two Handed Battleaxe",
						"Bow",
						"Staff",
						"Crossbow"
					};

					if (weaponTypes[static_cast<int>(weaponType)] != this->secondaryFilter) {
						continue;
					}
				}
			}

			this->filterList.emplace_back(std::make_unique<DataType>(item.GetForm(), 0));
		}
	}

	// TODO: This gave compile-time issues when building the template class.
	// Is it possible for ItemData to be calling this?
	template <typename DataType>
	void TableView<DataType>::SecondaryNPCFilter(const std::set<std::string>& a_data, const float& a_width)
	{
		std::vector<std::string> list(a_data.begin(), a_data.end());
		list.insert(list.begin(), _T("All"));

		if (InputTextComboBox("##Search::Filter::SecondaryFilter", this->secondaryFilterBuffer, this->secondaryFilter, IM_ARRAYSIZE(this->secondaryFilterBuffer), list, a_width)) {
			this->secondaryFilter = _T("All");

			if (this->secondaryFilter.find(this->secondaryFilterBuffer) != std::string::npos) {
				ImFormatString(this->secondaryFilterBuffer, IM_ARRAYSIZE(this->secondaryFilterBuffer), "");
			} else {
				for (auto& item : list) {
					if (item.empty()) {
						continue;
					}

					if (item.find(this->secondaryFilterBuffer) != std::string::npos) {
						this->secondaryFilter = item;
						ImFormatString(this->secondaryFilterBuffer, IM_ARRAYSIZE(this->secondaryFilterBuffer), "");
					}
				}
			}

			this->Refresh();
		}
	}

	template <class DataType>
	void TableView<DataType>::BuildPluginList()
	{
		const auto& config = Settings::GetSingleton()->GetConfig();
		this->pluginList = Data::GetSingleton()->GetFilteredListOfPluginNames(this->pluginType, (Data::SORT_TYPE)config.modListSort, this->primaryFilter);
		pluginList.insert(pluginList.begin(), _T("SHOW_ALL_PLUGINS"));
	}

	template <class DataType>
	void TableView<DataType>::ShowSearch(const float& a_height)
	{
		const float key_width = ImGui::GetContentRegionAvail().x / 8.0f;
		const float input_width = ImGui::GetContentRegionAvail().x / 2.0f - key_width;
		const float total_width = input_width + key_width + 2.0f;

		// This is kind of a lazy method to detect if the secondary filter is present
		// That way we can expand / reposition the primary filter drop-down to center align
		// it within the context window.
		if (this->primaryFilter == RE::FormType::Armor || this->primaryFilter == RE::FormType::Weapon) {
			secondaryFilterExpanded = true;
		} else {
			secondaryFilterExpanded = false;
		}

		ImGui::BeginColumns("##Search::Columns", 2, ImGuiOldColumnFlags_NoBorder);
		ImGui::SetColumnWidth(0, total_width + ImGui::GetStyle().ItemSpacing.x);  // TODO: Why?

		// Search bar for compare string.
		ImGui::Text(_TFM("GENERAL_SEARCH_RESULTS", ":"));
		ImGui::SetNextItemWidth(input_width);
		if (ImGui::InputTextWithHint("##Search::Input::CompareField", _T("GENERAL_CLICK_TO_TYPE"), this->generalSearchBuffer,
				IM_ARRAYSIZE(this->generalSearchBuffer),
				Frame::INPUT_FLAGS)) {
			// Refresh();
			this->UpdateSearch();
		}

		ImGui::SameLine();

		// Compare string drop-down filter.
		const auto currentFilter = this->inputSearchMap.at(searchKey);
		ImGui::SetNextItemWidth(key_width);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5.0f);
		if (ImGui::BeginCombo("##Search::Input::CompareFilter", _T(currentFilter))) {
			for (auto& compare : this->inputSearchMap) {
				auto searchID = compare.first;
				const char* searchValue = compare.second;
				const bool is_selected = (searchKey == searchID);

				if (ImGui::Selectable(_T(searchValue), is_selected)) {
					searchKey = searchID;
				}

				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		if (secondaryFilterExpanded) {
			ImGui::SetCursorPosY((a_height / 2) - (ImGui::GetFrameHeight() * 1.5f));
		} else {
			ImGui::SetCursorPosY((a_height / 2) - (ImGui::GetFrameHeight()));
		}

		// Primary RE::FormType filter.
		ImGui::Text(_TFM("GENERAL_FILTER_ITEM_TYPE", ":"));
		ImGui::SetNextItemWidth(total_width);
		const std::string filter_name = RE::FormTypeToString(this->primaryFilter).data();  // Localize?
		if (ImGui::BeginCombo("##Search::Filter::PrimaryFilter", filter_name.c_str())) {
			if (ImGui::Selectable(_T("None"), this->primaryFilter == RE::FormType::None)) {
				ImGui::SetItemDefaultFocus();
				this->primaryFilter = RE::FormType::None;
				this->secondaryFilter = _T("All");
				this->UpdateMasterList();
			}

			for (auto& filter : this->primaryFilterList) {
				const bool is_selected = (filter == this->primaryFilter);
				const std::string option = RE::FormTypeToString(filter).data();

				if (ImGui::Selectable(option.c_str(), is_selected)) {
					this->primaryFilter = filter;
					this->secondaryFilter = _T("All");
					this->UpdateMasterList();
				}

				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		// TODO: ImGuiComboFlag_HeightLarge ?!

		// Conditional secondary filter. Not all modules may have these.
		// We do not pass secondaryFilter through FormTypeToString since it's already a string.
		if (this->primaryFilter == RE::FormType::Armor) {
			ImGui::SetNextItemWidth(total_width);

			if (ImGui::BeginCombo("##Search::Filter::SecondaryFilter", this->secondaryFilter.c_str())) {
				if (ImGui::Selectable(_T("All"), this->secondaryFilter == _T("All"))) {
					ImGui::SetItemDefaultFocus();
					this->secondaryFilter = _T("All");
					this->UpdateMasterList();
				}

				// TODO: Does this need localized? If it does, we also need to
				// localize it inside the TableView draw call.
				const auto armor_slots = Utils::GetArmorSlotList();

				for (auto& slot : armor_slots) {
					bool is_selected = (slot == secondaryFilter);

					if (ImGui::Selectable(_T(slot), is_selected)) {
						this->secondaryFilter = slot;
						this->UpdateMasterList();
					}

					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}
		}

		if (this->primaryFilter == RE::FormType::Weapon) {
			ImGui::SetNextItemWidth(total_width);
			if (ImGui::BeginCombo("##Search::Filter::SecondaryFilter", this->secondaryFilter.c_str())) {
				if (ImGui::Selectable(_T("All"), this->secondaryFilter == _T("All"))) {
					ImGui::SetItemDefaultFocus();
					this->secondaryFilter = _T("All");
					this->UpdateMasterList();
				}

				// TODO: Refactor and include this with ItemPreview reference.
				// Also.. localization?
				const char* weapon_types[] = {
					"Hand to Hand",
					"One Handed Sword",
					"One Handed Dagger",
					"One Handed Axe",
					"One Handed Mace",
					"Two Handed Greatsword",
					"Two Handed Battleaxe",
					"Bow",
					"Staff",
					"Crossbow"
				};

				for (auto& type : weapon_types) {
					bool is_selected = (type == secondaryFilter);

					if (ImGui::Selectable(_T(type), is_selected)) {
						this->secondaryFilter = type;
						this->UpdateMasterList();
					}

					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}
		}

		if (this->primaryFilter == RE::FormType::Class) {
			const auto classList = Data::GetSingleton()->GetNPCClassList();
			std::vector<std::string> list(classList.begin(), classList.end());
			list.insert(list.begin(), _T("All"));

			this->SecondaryNPCFilter(classList, total_width);
		}

		if (this->primaryFilter == RE::FormType::Race) {
			const auto raceList = Data::GetSingleton()->GetNPCRaceList();
			std::vector<std::string> list(raceList.begin(), raceList.end());
			list.insert(list.begin(), _T("All"));

			this->SecondaryNPCFilter(raceList, total_width);
		}

		if (this->primaryFilter == RE::FormType::Faction) {
			const auto factionList = Data::GetSingleton()->GetNPCFactionList();
			std::vector<std::string> list(factionList.begin(), factionList.end());
			list.insert(list.begin(), _T("All"));

			this->SecondaryNPCFilter(factionList, total_width);
		}

		ImGui::NewLine();

		ImGui::SetCursorPosY(a_height - ImGui::GetFrameHeightWithSpacing() * 2.0f);
		ImGui::Text(_TFM("GENERAL_FILTER_MODLIST", ":"));
		if (InputTextComboBox("##Search::Filter::PluginField", this->pluginSearchBuffer, this->selectedPlugin, IM_ARRAYSIZE(this->pluginSearchBuffer), this->pluginList, total_width)) {
			auto modList = Data::GetSingleton()->GetModulePluginList(this->pluginType);
			this->selectedPlugin = _T("SHOW_ALL_PLUGINS");

			if (this->selectedPlugin.find(this->pluginSearchBuffer) != std::string::npos) {
				ImFormatString(this->pluginSearchBuffer, IM_ARRAYSIZE(this->pluginSearchBuffer), "");
			} else {
				for (auto& mod : modList) {
					if (PersistentData::GetBlacklist().contains(mod)) {
						continue;
					}

					std::string modName = Modex::ValidateTESFileName(mod);

					if (modName == _T("SHOW_ALL_PLUGINS")) {
						ImFormatString(this->pluginSearchBuffer, IM_ARRAYSIZE(this->pluginSearchBuffer), "");
						break;
					}

					if (modName.find(this->pluginSearchBuffer) != std::string::npos) {
						this->selectedPlugin = modName;
						ImFormatString(this->pluginSearchBuffer, IM_ARRAYSIZE(this->pluginSearchBuffer), "");
						break;
					}
				}
			}

			this->Refresh();
		}

		ImGui::NextColumn();

		ImGui::SubCategoryHeader(_T("GENERAL_TOTAL_HEADER"));

		const float maxWidth = ImGui::GetContentRegionAvail().x;
		const auto InlineText = [maxWidth](const char* label, const char* text) {
			const auto width = std::max(maxWidth - ImGui::CalcTextSize(text).x, ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(text).x);
			ImGui::Text(label);
			ImGui::SameLine(width);
			ImGui::Text(text);
		};

		ImGui::EndColumns();

		// TODO: The hidden_plugins and subsequently other calculations here are probably expensive.
		// Maybe I can store this information in the Data class and just call it here instead?
		// No, I can't, because each module will likely have different results?

		// const auto total_plugins = std::ssize(pluginList);
		// const auto total_items = std::ssize(tableList);
		// const auto total_blacklist = std::ssize(PersistentData::GetBlacklist());
		// std::ptrdiff_t hidden_items;
		// std::ptrdiff_t hidden_plugins;
		// std::ptrdiff_t empty = 0;

		// // Kinda sloppy
		// switch (this->pluginType) {
		// case Data::PLUGIN_TYPE::ITEM:
		// 	hidden_items = std::max(std::ssize(Data::GetSingleton()->GetAddItemList()) - total_plugins, empty);
		// 	hidden_plugins = std::max(std::ssize(Data::GetSingleton()->GetModulePluginList(Data::PLUGIN_TYPE::ITEM)) - total_plugins, empty);
		// 	break;
		// case Data::PLUGIN_TYPE::NPC:
		// 	hidden_items = std::max(std::ssize(Data::GetSingleton()->GetNPCList()) - total_plugins, empty);
		// 	hidden_plugins = std::max(std::ssize(Data::GetSingleton()->GetModulePluginList(Data::PLUGIN_TYPE::NPC)) - total_plugins, empty);
		// 	break;
		// case Data::PLUGIN_TYPE::OBJECT:
		// 	hidden_items = std::max(std::ssize(Data::GetSingleton()->GetObjectList()) - total_plugins, empty);
		// 	hidden_plugins = std::max(std::ssize(Data::GetSingleton()->GetModulePluginList(Data::PLUGIN_TYPE::OBJECT)) - total_plugins, empty);
		// 	break;
		// case Data::PLUGIN_TYPE::CELL:
		// 	hidden_items = std::max(std::ssize(Data::GetSingleton()->GetTeleportList()) - total_plugins, empty);
		// 	hidden_plugins = std::max(std::ssize(Data::GetSingleton()->GetModulePluginList(Data::PLUGIN_TYPE::CELL)) - total_plugins, empty);
		// 	break;
		// default:
		// 	hidden_items = 0;
		// 	hidden_plugins = 0;
		// }

		// InlineText(_TICONM(ICON_LC_PACKAGE_PLUS, "GENERAL_TOTAL_PLUGINS", ":"), std::to_string(total_plugins).c_str());
		// if (ImGui::IsItemHovered()) {
		// 	ImGui::SetTooltip(_T("TOOLTIP_TOTAL_PLUGIN"));
		// }

		// InlineText(_TICONM(ICON_LC_PACKAGE_SEARCH, "GENERAL_TOTAL_BLACKLIST", ":"), std::to_string(total_blacklist).c_str());
		// if (ImGui::IsItemHovered()) {
		// 	ImGui::SetTooltip(_T("TOOLTIP_BLACKLIST_PLUGIN"));
		// }

		// InlineText(_TICONM(ICON_LC_PACKAGE_SEARCH, "GENERAL_TOTAL_HIDDEN", ":"), std::to_string(hidden_plugins).c_str());
		// if (ImGui::IsItemHovered()) {
		// 	ImGui::SetTooltip(_T("TOOLTIP_HIDDEN_PLUGIN"));
		// }
		// ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		// InlineText(_TICONM(ICON_LC_SEARCH, "Search Results", ":"), std::to_string(total_items).c_str());
		// if (ImGui::IsItemHovered()) {
		// 	ImGui::SetTooltip(_T("TOOLTIP_TOTAL_SEARCH"));
		// }

		// InlineText(_TICONM(ICON_LC_EYE_OFF, "GENERAL_TOTAL_HIDDEN", ":"), std::to_string(hidden_items).c_str());
		// if (ImGui::IsItemHovered()) {
		// 	ImGui::SetTooltip(_T("TOOLTIP_TOTAL_FILTERED"));
		// }

		// ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		// // InlineText(_TICONM(ICON_LC_PACKAGE, "GENERAL_KITS_TOTAL", ":"), std::to_string(PersistentData::GetLoadedKits().size()).c_str());
		// // InlineText(_TICONM(ICON_LC_PACKAGE_OPEN, "GENERAL_KITS_IN_PLUGIN", ":"), std::to_string(kitsFound.size()).c_str());

		// ImGui::NewLine();

		// ImGui::EndColumns();
	}

	template <typename DataType>
	void TableView<DataType>::UpdateLayout(float a_width)
	{
		const auto& style = Settings::GetSingleton()->GetStyle();
		const auto& config = Settings::GetSingleton()->GetConfig();

		// Since the font size is variable, we need to recalculate height based on font size.
		const float row_height = style.tableRowHeight + (config.globalFontSize * 2.0f);

		ItemSize = ImVec2(a_width, row_height);
		LayoutItemSize = ImVec2(floorf(ItemSize.x), floorf(ItemSize.y));

		LayoutRowSpacing = style.tableRowSpacing + style.tableRowThickness;
		LayoutHitSpacing = 0.0f;  // TODO: Needs tested.

		LayoutColumnCount = 1;

		if (compactView) {
			ItemSize = ImVec2(a_width, config.globalFontSize * 1.75f);
			LayoutItemSize = ImVec2(floorf(ItemSize.x), floorf(ItemSize.y));

			LayoutRowSpacing = 5.0f;
			LayoutHitSpacing = 0.0f;
		}

		// This is for if and when we want to do a grid layout.
		// Will need entire implementation of LayoutColumnCount
		LayoutLineCount = ((int)tableList.size() + LayoutColumnCount - 1) / LayoutColumnCount;

		// If/When we add a grid layout, we will need to step on the x-axis as well.
		LayoutItemStep = ImVec2(LayoutItemSize.x, LayoutItemSize.y + LayoutRowSpacing);

		// This is used with LayoutHitSpacing to create gaps between items. Sort of like an outline.
		// LayoutSelectableSpacing = std::max(floorf(LayoutRowSpacing) - LayoutHitSpacing, 0.0f);
		LayoutSelectableSpacing = 0.0f;

		// Includes measurement for outline thickness!
		LayoutOuterPadding = floorf(LayoutRowSpacing * 0.5f);
	}

	ImU32 GetFormTypeColor(const RE::FormType& a_type)
	{
		switch (a_type) {
		case RE::FormType::Armor:
			return IM_COL32(128, 0, 0, 255);  // Maroon
		case RE::FormType::AlchemyItem:
			return IM_COL32(0, 128, 128, 255);  // Teal
		case RE::FormType::Ammo:
			return IM_COL32(128, 128, 0, 255);  // Olive
		case RE::FormType::Book:
			return IM_COL32(139, 69, 19, 255);  // Saddle Brown
		case RE::FormType::Ingredient:
			return IM_COL32(34, 139, 34, 255);  // Forest Green
		case RE::FormType::KeyMaster:
			return IM_COL32(255, 140, 0, 255);  // Dark Orange
		case RE::FormType::Misc:
			return IM_COL32(128, 0, 128, 255);  // Purple
		case RE::FormType::Weapon:
			return IM_COL32(178, 34, 34, 255);  // Firebrick
		case RE::FormType::NPC:
			return IM_COL32(70, 130, 180, 255);  // Steel Blue
		case RE::FormType::Tree:
			return IM_COL32(0, 100, 0, 255);  // Dark Green
		case RE::FormType::Static:
			return IM_COL32(75, 0, 130, 255);  // Indigo
		case RE::FormType::Container:
			return IM_COL32(139, 0, 0, 255);  // Dark Red
		case RE::FormType::Activator:
			return IM_COL32(255, 69, 0, 255);  // Red Orange
		case RE::FormType::Light:
			return IM_COL32(255, 215, 0, 255);  // Gold
		case RE::FormType::Door:
			return IM_COL32(0, 128, 0, 255);  // Green
		case RE::FormType::Furniture:
			return IM_COL32(139, 0, 139, 255);  // Dark Magenta
		default:
			return IM_COL32(105, 105, 105, 255);  // Dim Gray
		}
	}

	std::string GetItemIcon(const BaseObject& a_item)
	{
		const RE::FormType formType = a_item.GetFormType();

		switch (formType) {
		case RE::FormType::Armor:  // Start of Additem Module
			return ICON_LC_SHIELD;
		case RE::FormType::AlchemyItem:
			return ICON_LC_FLASK_CONICAL;
		case RE::FormType::Ammo:
			return ICON_LC_CONTAINER;
		case RE::FormType::Book:
			return ICON_LC_BOOK;
		case RE::FormType::Ingredient:
			return ICON_LC_TEST_TUBE;
		case RE::FormType::KeyMaster:
			return ICON_LC_KEY;
		case RE::FormType::Misc:
			return ICON_LC_PUZZLE;
		case RE::FormType::Weapon:
			return ICON_LC_SWORDS;
		case RE::FormType::NPC:  // Start of NPC module
			return ICON_LC_VENETIAN_MASK;
		case RE::FormType::Tree:  // Start of Object Module
			return ICON_LC_TREE_PINE;
		case RE::FormType::Static:
			return ICON_LC_SHAPES;
		case RE::FormType::Container:
			return ICON_LC_PACKAGE;
		case RE::FormType::Activator:
			return ICON_LC_TARGET;
		case RE::FormType::Light:
			return ICON_LC_SUN;
		case RE::FormType::Door:
			return ICON_LC_DOOR_CLOSED;
		case RE::FormType::Furniture:
			return ICON_LC_ARMCHAIR;
		default:
			return ICON_LC_BEAKER;
		}
	}

	// Should we newline instead of ellipsis?
	std::string FormatTextWidth(const std::string& a_text, const float& a_width)
	{
		const float textWidth = ImGui::CalcTextSize(a_text.c_str()).x;

		if (textWidth > a_width) {
			const float ellipsisWidth = ImGui::CalcTextSize("...").x;

			std::string result = "";
			float resultWidth = 0.0f;

			for (const auto& c : a_text) {
				const float charWidth = ImGui::CalcTextSize(std::string(1, c).c_str()).x;

				if (resultWidth + charWidth + ellipsisWidth > a_width) {
					result += "...";
					break;
				}

				result += c;
				resultWidth += charWidth;
			}

			return result;
		}

		return a_text;
	}

	template <typename DataType>
	bool TableView<DataType>::SortFn(const std::unique_ptr<DataType>& lhs, const std::unique_ptr<DataType>& rhs)
	{
		int delta = 0;

		switch (sortBy) {
		case SortType::EditorID:
			delta = lhs->GetEditorIDView().compare(rhs->GetEditorIDView());
			break;
		case SortType::Plugin:
			delta = lhs->GetPluginNameView().compare(rhs->GetPluginNameView());
			break;
		case SortType::Type:
			if constexpr (!std::is_base_of<BaseObject, DataType>::value)
				break;
			else
				delta = lhs->GetTypeName().compare(rhs->GetTypeName());
			break;
		case SortType::FormID:
			if constexpr (!std::is_base_of<BaseObject, DataType>::value)
				break;
			else
				delta = (lhs->GetBaseForm() < rhs->GetBaseForm()) ? -1 : (lhs->GetBaseForm() > rhs->GetBaseForm()) ? 1 :
				                                                                                                     0;
			break;
		case SortType::ReferenceID:
			if constexpr (!std::is_base_of<BaseObject, DataType>::value)
				break;
			else
				delta = (lhs->refID < rhs->refID) ? -1 : (lhs->refID > rhs->refID) ? 1 :
				                                                                     0;
			break;
		case SortType::Name:
			if constexpr (!std::is_base_of<BaseObject, DataType>::value)
				break;
			else
				delta = lhs->GetNameView().compare(rhs->GetNameView());
			break;
		case SortType::Class:
			if constexpr (!std::is_same<DataType, NPCData>::value) {
				break;
			} else {
				std::string_view lhsClass = lhs->GetClass();
				std::string_view rhsClass = rhs->GetClass();

				delta = lhsClass.compare(rhsClass);
				break;
			}
		case SortType::Gender:
			if constexpr (!std::is_same<DataType, NPCData>::value) {
				break;
			} else {
				std::string_view lhsGender = lhs->GetGender();
				std::string_view rhsGender = rhs->GetGender();
				delta = lhsGender.compare(rhsGender);
				break;
			}
		case SortType::Race:
			if constexpr (!std::is_same<DataType, NPCData>::value) {
				break;
			} else {
				std::string_view lhsRace = lhs->GetRace();
				std::string_view rhsRace = rhs->GetRace();

				delta = lhsRace.compare(rhsRace);
				break;
			}
		case SortType::Level:
			if constexpr (!std::is_same<DataType, NPCData>::value) {
				break;
			} else {
				auto lhsLevel = lhs->GetLevel();
				auto rhsLevel = rhs->GetLevel();
				delta = (lhsLevel < rhsLevel) ? -1 : (lhsLevel > rhsLevel) ? 1 :
				                                                             0;
				break;
			}
		case SortType::Health:
			if constexpr (!std::is_same<DataType, NPCData>::value)
				break;
			else
				delta = (lhs->GetHealth() < rhs->GetHealth()) ? -1 : (lhs->GetHealth() > rhs->GetHealth()) ? 1 :
				                                                                                             0;
			break;
		case SortType::Magicka:
			if constexpr (!std::is_same<DataType, NPCData>::value)
				break;
			else
				delta = (lhs->GetMagicka() < rhs->GetMagicka()) ? -1 : (lhs->GetMagicka() > rhs->GetMagicka()) ? 1 :
				                                                                                                 0;
			break;
		case SortType::Stamina:
			if constexpr (!std::is_same<DataType, NPCData>::value)
				break;
			else
				delta = (lhs->GetStamina() < rhs->GetStamina()) ? -1 : (lhs->GetStamina() > rhs->GetStamina()) ? 1 :
				                                                                                                 0;
			break;
		// case SortType::CarryWeight:
		// 	if constexpr (!std::is_same<DataType, NPCData>::value)
		// 		break;
		// 	else
		// 		delta = (lhs->GetCarryWeight() < rhs->GetCarryWeight()) ? -1 : (lhs->GetCarryWeight() > rhs->GetCarryWeight()) ? 1 :
		// 		                                                                                                                 0;
		// 	break;
		case SortType::Value:
			if constexpr (!std::is_same<DataType, ItemData>::value)
				break;
			else
				delta = (lhs->GetValue() < rhs->GetValue()) ? -1 : (lhs->GetValue() > rhs->GetValue()) ? 1 :
				                                                                                         0;
			break;
		case SortType::Space:
			if constexpr (!std::is_same<DataType, CellData>::value)
				break;
			else
				delta = lhs->space.compare(rhs->space);
			break;
		case SortType::Zone:
			if constexpr (!std::is_same<DataType, CellData>::value)
				break;
			else
				delta = lhs->zone.compare(rhs->zone);
			break;
		case SortType::Cell:
			if constexpr (!std::is_same<DataType, CellData>::value)
				break;
			else
				delta = lhs->cellName.compare(rhs->cellName);
			break;
		case SortType::Damage:
			if constexpr (!std::is_same<DataType, ItemData>::value) {
				break;
			} else {
				if (lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Weapon) {
					delta = CompareWeaponDamage(lhs->GetForm()->As<RE::TESObjectWEAP>(), rhs->GetForm()->As<RE::TESObjectWEAP>());
				} else if (lhs->GetFormType() == RE::FormType::Weapon) {
					delta = 1;
				} else if (rhs->GetFormType() == RE::FormType::Weapon) {
					delta = -1;
				}

				break;
			}
		case SortType::Armor:
			if constexpr (!std::is_same<DataType, ItemData>::value) {
				break;
			} else {
				if (lhs->GetFormType() == RE::FormType::Armor && rhs->GetFormType() == RE::FormType::Armor) {
					delta = CompareArmorRating(lhs->GetForm()->As<RE::TESObjectARMO>(), rhs->GetForm()->As<RE::TESObjectARMO>());
				} else if (lhs->GetFormType() == RE::FormType::Armor) {
					delta = 1;
				} else if (rhs->GetFormType() == RE::FormType::Armor) {
					delta = -1;
				}

				break;
			}
		case SortType::Slot:
			if constexpr (!std::is_same<DataType, ItemData>::value) {
				break;
			} else {
				if (lhs->GetFormType() == RE::FormType::Armor && rhs->GetFormType() == RE::FormType::Armor) {
					delta = CompareArmorSlot(lhs->GetForm()->As<RE::TESObjectARMO>(), rhs->GetForm()->As<RE::TESObjectARMO>());
				} else if (lhs->GetFormType() == RE::FormType::Armor) {
					delta = 1;
				} else if (rhs->GetFormType() == RE::FormType::Armor) {
					delta = -1;
				}
				break;
			}
		case SortType::Speed:
			if constexpr (!std::is_same<DataType, ItemData>::value) {
				break;
			} else {
				if (lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Weapon) {
					delta = CompareWeaponSpeed(lhs->GetForm()->As<RE::TESObjectWEAP>(), rhs->GetForm()->As<RE::TESObjectWEAP>());
				} else if (lhs->GetFormType() == RE::FormType::Weapon) {
					delta = 1;
				} else if (rhs->GetFormType() == RE::FormType::Weapon) {
					delta = -1;
				}

				break;
			}
		case SortType::CriticalDamage:
			if constexpr (!std::is_same<DataType, ItemData>::value) {
				break;
			} else {
				if (lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Weapon) {
					delta = CompareCritDamage(lhs->GetForm()->As<RE::TESObjectWEAP>(), rhs->GetForm()->As<RE::TESObjectWEAP>());
				} else if (lhs->GetFormType() == RE::FormType::Weapon) {
					delta = 1;
				} else if (rhs->GetFormType() == RE::FormType::Weapon) {
					delta = -1;
				}

				break;
			}
		case SortType::Weight:
			if constexpr (!std::is_same<DataType, ItemData>::value) {
				break;
			} else {
				delta = (lhs->GetWeight() < rhs->GetWeight()) ? -1 : (lhs->GetWeight() > rhs->GetWeight()) ? 1 :
				                                                                                             0;
				break;
			}
		case SortType::DamagePerSecond:
			if constexpr (!std::is_same<DataType, ItemData>::value) {
				break;
			} else {
				if (lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Weapon) {
					delta = CompareWeaponDPS(lhs->GetForm()->As<RE::TESObjectWEAP>(), rhs->GetForm()->As<RE::TESObjectWEAP>());
				} else if (lhs->GetFormType() == RE::FormType::Weapon) {
					delta = 1;
				} else if (rhs->GetFormType() == RE::FormType::Weapon) {
					delta = -1;
				}

				break;
			}
		case SortType::Skill:
			if constexpr (!std::is_same<DataType, ItemData>::value) {
				break;
			} else {
				if ((lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Weapon) ||
					(lhs->GetFormType() == RE::FormType::Armor && rhs->GetFormType() == RE::FormType::Armor) ||
					(lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Armor) ||
					(lhs->GetFormType() == RE::FormType::Armor && rhs->GetFormType() == RE::FormType::Weapon)) {
					delta = CompareSkill(lhs->GetForm(), rhs->GetForm());
				} else if (lhs->GetFormType() == RE::FormType::Weapon || lhs->GetFormType() == RE::FormType::Armor) {
					delta = 1;
				} else if (rhs->GetFormType() == RE::FormType::Weapon || rhs->GetFormType() == RE::FormType::Armor) {
					delta = -1;
				}

				break;
			}
		}

		if (delta > 0)
			return sortAscending ? false : true;
		if (delta < 0)
			return sortAscending ? true : false;

		return false;
	}

	template <typename DataType>
	void TableView<DataType>::SortListBySpecs()
	{
		std::sort(tableList.begin(), tableList.end(), [this](const std::unique_ptr<DataType>& a, const std::unique_ptr<DataType>& b) {
			return this->SortFn(a, b);
		});

		// if (!sortAscending) {
		// 	std::reverse(tableList.begin(), tableList.end());
		// }

		// for (int i = 0; i < std::ssize(tableList); ++i) {
		// 	tableList[i]->TableID = i;
		// }

		this->UpdateImGuiTableIDs();
	}

	template <typename DataType>
	void TableView<DataType>::UpdateImGuiTableIDs()
	{
		for (int i = 0; i < std::ssize(tableList); ++i) {
			tableList[i]->TableID = i;
		}

		// std::for_each(std::execution::par_unseq, tableList.begin(), tableList.end(), [i = 0](auto& item) mutable {
		// 	item->TableID = i++;
		// });
	}

	template <typename DataType>
	void TableView<DataType>::ShowSort()
	{
		auto IconButton = [](const char* icon, const char* tooltip, bool& condition) -> bool {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			bool pressed = false;
			if (ImGui::Button(icon, ImVec2(ImGui::GetFrameHeightWithSpacing(), ImGui::GetFrameHeightWithSpacing()))) {
				condition = !condition;
				pressed = true;
			}
			ImGui::PopStyleColor(6);
			ImGui::PopStyleVar(2);

			if (tooltip[0] != '\0') {
				if (ImGui::IsItemHovered()) {
					ImGui::SetTooltip(tooltip);
				}
			}

			return pressed;
		};

		const std::string compact = this->compactView ? ICON_LC_ROWS_2 : ICON_LC_ROWS_4;
		if (IconButton(compact.c_str(), "Enable/Disable Compact View", compactView)) {
			// nothing?
		}

		ImGui::SameLine();

		const std::string showid = ICON_LC_SIGNATURE;
		ImGui::PushStyleColor(ImGuiCol_Text, this->showEditorID ? ImGui::GetStyle().Colors[ImGuiCol_Text] : ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
		if (IconButton(showid.c_str(), "Show/Hide EditorID instead of Name", this->showEditorID)) {
			// nothing?
		}
		ImGui::PopStyleColor();

		if (this->HasFlag(ModexTableFlag_EnableKitView)) {
			ImGui::SameLine();

			const std::string showKit = ICON_LC_BOX;
			bool pluginHasKits = !this->kitList.empty();

			if (pluginHasKits && !this->showKitView) {
				float pulse = Utils::PulseMinMax((float)ImGui::GetTime(), 5.0f, 0.5f, 0.0f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, pulse));
			} else if (pluginHasKits && this->showKitView) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			} else {
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
			}

			if (IconButton(showKit.c_str(), "", this->showKitView)) {
				if (!pluginHasKits) {
					this->showKitView = false;
				}
			}

			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered()) {
				if (pluginHasKits) {
					ImGui::SetTooltip("Show/Hide Kits using selected plugin");
				} else {
					ImGui::SetTooltip("No Kits Found in this Plugin");
				}
			}
		}

		ImGui::SameLine();

		constexpr auto combo_flags = ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_HeightLarge;
		const auto preview_size = ImGui::CalcTextSize(SortTypeToString(this->sortBy).c_str());

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));

		float button_offset = ImGui::GetFrameHeightWithSpacing() * 0.5f;

		if (this->HasFlag(ModexTableFlag_Kit)) {
			button_offset = ImGui::GetFrameHeightWithSpacing() * 1.5f;
		}

		ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - preview_size.x + button_offset - (ImGui::GetStyle().FramePadding.x));
		ImGui::SetNextItemWidth(preview_size.x);
		if (ImGui::BeginCombo("##TableView::Sort::Combo", SortTypeToString(this->sortBy).c_str(), combo_flags)) {
			for (auto& sort : sortByList) {
				const bool is_selected = (this->sortBy == sort);
				const std::string sort_text = SortTypeToString(sort);

				if (ImGui::Selectable(sort_text.c_str(), is_selected)) {
					this->sortBy = sort;
					this->SortListBySpecs();
					this->UpdateImGuiTableIDs();
				}

				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();
		ImGui::SameLine();

		const std::string arrow = this->sortAscending ? ICON_LC_ARROW_DOWN_A_Z : ICON_LC_ARROW_UP_Z_A;
		if (IconButton(arrow.c_str(), "Sort Ascending/Descending", this->sortAscending)) {
			this->SortListBySpecs();
			this->UpdateImGuiTableIDs();
		}

		if (this->HasFlag(ModexTableFlag_EnableEnchantmentSort)) {
			ImGui::SameLine();

			const std::string enchanted = this->hideEnchanted ? ICON_LC_SPARKLES : ICON_LC_SPARKLE;
			if (IconButton(enchanted.c_str(), "Show Enchanted Items", hideEnchanted)) {
				this->Refresh();
			}
		}

		if (this->HasFlag(ModexTableFlag_EnableNonPlayableSort)) {
			ImGui::SameLine();

			const std::string hidden = this->hideNonPlayable ? ICON_LC_EYE_CLOSED : ICON_LC_EYE;
			if (IconButton(hidden.c_str(), "Show Hidden Items", this->hideNonPlayable)) {
				this->Refresh();
			}
		}
	}

	template <typename DataType>
	void TableView<DataType>::LoadKitsFromPlugin()
	{
		if (this->HasFlag(ModexTableFlag_EnableKitView)) {
			kitList.clear();

			if (selectedPlugin == _T("SHOW_ALL_PLUGINS")) {
				return;
			}

			const std::string header_label = "Kits found in: " + selectedPlugin;
			ImGui::SubCategoryHeader(header_label.c_str());

			auto& kits = PersistentData::GetLoadedKits();

			int table_id = 0;
			for (auto& [name, kit] : kits) {
				bool kitExists = false;
				for (auto& existingKit : kitList) {
					if (existingKit->name == kit.name) {
						kitExists = true;
						break;
					}
				}
				if (!kitExists) {
					for (auto& item : kit.items) {
						if (item->plugin == selectedPlugin) {
							// auto newKit = std::make_shared<Kit>(kit);
							// newKit->TableID = static_cast<int>(std::ssize(kitList));
							// kitList.push_back(newKit);
							kit.TableID = table_id;
							this->kitList.emplace_back(std::make_unique<Kit>(kit));
							table_id++;
							break;
						}
					}
				}
			}
		}
	}

	template <typename DataType>
	void TableView<DataType>::DrawKit(const Kit& a_kit, const ImVec2& a_pos, const bool& a_selected)
	{
		(void)a_kit;
		(void)a_selected;  // If we want to handle selection visuals manually.

		const auto& style = Settings::GetSingleton()->GetStyle();
		const auto& config = Settings::GetSingleton()->GetConfig();
		const auto fontSize = config.globalFontSize;

		// Setup box and bounding box for positioning and drawing.
		const ImVec2 box_min(a_pos.x - 1, a_pos.y - 1);
		const ImVec2 box_max(box_min.x + LayoutItemSize.x + 2, box_min.y + LayoutItemSize.y + 2);  // Dubious
		ImRect bb(box_min, box_max);

		// Outline & Background
		const ImU32 bg_color = ImGui::ColorConvertFloat4ToU32(style.tableRowBg);
		const ImU32 bg_color_alt = ImGui::ColorConvertFloat4ToU32(style.tableAltRowBg);
		const ImU32 outline_color = ImGui::ColorConvertFloat4ToU32(style.tableRowOutlineColor);
		const ImU32 text_color = ImGui::GetColorU32(ImGuiCol_Text);

		// Background
		if (style.showTableRowBG) {
			if (a_kit.TableID % 2 == 0) {
				DrawList->AddRectFilled(bb.Min, bb.Max, bg_color);
			} else {
				DrawList->AddRectFilled(bb.Min, bb.Max, bg_color_alt);
			}
		} else {
			DrawList->AddRectFilled(bb.Min, bb.Max, bg_color);
		}

		// Outline
		DrawList->AddRect(bb.Min, bb.Max, outline_color, style.tableRowRounding, 0, style.tableRowThickness);

		const float spacing = LayoutItemSize.x / 5.0f;
		const float top_align = bb.Min.y + LayoutOuterPadding;
		const float bot_align = bb.Max.y - LayoutOuterPadding - fontSize;
		const float center_align = bb.Min.y + ((LayoutOuterPadding + LayoutItemSize.y) / 2) - (fontSize / 2.0f);
		const float left_align = bb.Min.x + LayoutOuterPadding;
		const float right_align = bb.Max.x - LayoutOuterPadding - fontSize;
		const ImVec2 top_left_align = ImVec2(left_align, top_align);
		const ImVec2 top_right_align = ImVec2(right_align, top_align);
		const ImVec2 bot_left_align = ImVec2(left_align, bot_align);
		const ImVec2 bot_right_align = ImVec2(right_align, bot_align);
		const ImVec2 center_left_align = ImVec2(left_align, center_align);
		const ImVec2 center_right_align = ImVec2(right_align, center_align);

		// Draw the kit name for now
		const std::string name_string = a_kit.name;
		DrawList->AddText(top_left_align, text_color, name_string.c_str());

		const std::string desc_string = a_kit.desc;

		if (ImGui::CalcTextSize(desc_string.c_str()).x > spacing * 1.5f) {
			std::string first_half = desc_string.substr(0, desc_string.size() / 2);
			std::string second_half = desc_string.substr(desc_string.size() / 2);

			DrawList->AddText(center_left_align, text_color, first_half.c_str());
			DrawList->AddText(bot_left_align, text_color, second_half.c_str());
		} else {
			DrawList->AddText(bot_left_align, text_color, desc_string.c_str());
		}
	}

	template <typename DataType>
	void TableView<DataType>::KitView()
	{
		if (!this->HasFlag(ModexTableFlag_EnableKitView)) {
			return;
		}

		if (!kitList.empty()) {
			DrawList = ImGui::GetWindowDrawList();

			// Setup table layout based on available width
			const float a_width = ImGui::GetContentRegionAvail().x;
			UpdateLayout(a_width);

			// Calculate and store start position of table.
			const ImVec2 start_pos = ImGui::GetCursorScreenPos();
			ImGui::SetCursorScreenPos(start_pos);

			const int COLUMN_COUNT = 1;
			const int ITEMS_COUNT = static_cast<int>(std::ssize(kitList));

			ImGuiListClipper clipper;
			ImGuiMultiSelectIO* ms_io = ImGui::BeginMultiSelect(ms_flags, kitSelectionStorage.Size, ITEMS_COUNT);
			kitSelectionStorage.UserData = (void*)&kitList;
			kitSelectionStorage.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage* self, int idx) {
				KitList* a_items = (KitList*)self->UserData;
				return (*a_items)[idx]->TableID;  // Index -> TableID
			};
			kitSelectionStorage.ApplyRequests(ms_io);

			// Start clipper and iterate through table's item list.
			clipper.Begin(ITEMS_COUNT, LayoutItemStep.y);
			if (ms_io->RangeSrcItem != -1) {
				clipper.IncludeItemByIndex((int)ms_io->RangeSrcItem);  // Ensure RangeSrc item is not clipped.
			}
			while (clipper.Step()) {
				const int item_start = clipper.DisplayStart;
				const int item_end = clipper.DisplayEnd;

				for (int line_idx = item_start; line_idx < item_end; line_idx++) {
					const int item_min_idx_for_current_line = line_idx * COLUMN_COUNT;
					const int item_max_idx_for_current_line = std::min((line_idx + 1) * COLUMN_COUNT, ITEMS_COUNT);

					for (int kit_idx = item_min_idx_for_current_line; kit_idx < item_max_idx_for_current_line; ++kit_idx) {
						auto& item_data = kitList[kit_idx];
						ImGui::PushID((int)item_data->TableID);

						// position item at start
						ImVec2 pos = ImVec2(start_pos.x, start_pos.y + line_idx * LayoutItemStep.y);
						ImGui::SetCursorScreenPos(pos);

						// set next item selection user data
						ImGui::SetNextItemSelectionUserData(kit_idx);
						bool is_item_selected = kitSelectionStorage.Contains(item_data->TableID);
						bool is_item_visible = ImGui::IsRectVisible(LayoutItemSize);

						// If we implement a grid or gapped layout, this will be needed.
						// For now, it just removes the spacing so the selection is within the bounds of the item.
						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(LayoutSelectableSpacing, LayoutSelectableSpacing));
						ImGui::Selectable("", is_item_selected, ImGuiSelectableFlags_None, LayoutItemSize);
						ImGui::PopStyleVar();

						if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
							if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
								// Console::AddItem(item_data->GetFormID().c_str(), 1);
								// Console::StartProcessThread();
							}
						}

						if (ImGui::IsItemToggledSelection()) {
							is_item_selected = !is_item_selected;
						}

						if (is_item_visible) {
							DrawKit(*item_data, pos, is_item_selected);
						}

						ImGui::PopID();
					}
				}
			}

			clipper.End();

			ms_io = ImGui::EndMultiSelect();
			kitSelectionStorage.ApplyRequests(ms_io);
		}
	}

	template <typename DataType>
	void TableView<DataType>::DrawItem(const DataType& a_item, const ImVec2& a_pos, const bool& a_selected)
	{
		(void)a_selected;  // If we want to handle selection visuals manually.

		const auto& style = Settings::GetSingleton()->GetStyle();
		const auto& config = Settings::GetSingleton()->GetConfig();
		const auto fontSize = config.globalFontSize;

		// Setup box and bounding box for positioning and drawing.
		const ImVec2 box_min(a_pos.x - 1, a_pos.y - 1);
		const ImVec2 box_max(box_min.x + LayoutItemSize.x + 2, box_min.y + LayoutItemSize.y + 2);  // Dubious
		ImRect bb(box_min, box_max);

		// Outline & Background
		const ImU32 bg_color = ImGui::ColorConvertFloat4ToU32(style.tableRowBg);
		const ImU32 bg_color_alt = ImGui::ColorConvertFloat4ToU32(style.tableAltRowBg);
		const ImU32 outline_color = ImGui::ColorConvertFloat4ToU32(style.tableRowOutlineColor);
		const ImU32 text_color = ImGui::GetColorU32(ImGuiCol_Text);

		// Background
		if (style.showTableRowBG) {
			if (a_item.TableID % 2 == 0) {
				DrawList->AddRectFilled(bb.Min, bb.Max, bg_color);
			} else {
				DrawList->AddRectFilled(bb.Min, bb.Max, bg_color_alt);
			}
		} else {
			DrawList->AddRectFilled(bb.Min, bb.Max, bg_color);
		}

		// Outline
		DrawList->AddRect(bb.Min, bb.Max, outline_color, style.tableRowRounding, 0, style.tableRowThickness);

		// Type Color Identifier
		const float type_pillar_width = 5.0f;
		DrawList->AddRectFilled(
			ImVec2(bb.Min.x + LayoutOuterPadding, bb.Min.y + LayoutOuterPadding),
			ImVec2(bb.Min.x + LayoutOuterPadding + type_pillar_width, bb.Max.y - LayoutOuterPadding),
			GetFormTypeColor(a_item.GetFormType()));

		// We need to adjust the bounding box to account for the type pillar.
		bb.Min.x += type_pillar_width * 2.0f;

		const float spacing = LayoutItemSize.x / 5.0f;
		const float top_align = bb.Min.y + LayoutOuterPadding;
		const float bot_align = bb.Max.y - LayoutOuterPadding - fontSize;
		const float center_align = bb.Min.y + ((LayoutOuterPadding + LayoutItemSize.y) / 2) - (fontSize / 2.0f);
		const float left_align = bb.Min.x + LayoutOuterPadding;
		const float right_align = bb.Max.x - LayoutOuterPadding - fontSize;
		const ImVec2 top_left_align = ImVec2(left_align, top_align);
		const ImVec2 top_right_align = ImVec2(right_align, top_align);
		const ImVec2 bot_left_align = ImVec2(left_align, bot_align);
		const ImVec2 bot_right_align = ImVec2(right_align, bot_align);
		const ImVec2 center_left_align = ImVec2(left_align, center_align);
		const ImVec2 center_right_align = ImVec2(right_align, center_align);

		// Draw Type:FormID
		if (!compactView) {
			const std::string type_formid = "[" + a_item.GetTypeName() + "] " + a_item.GetFormID();
			DrawList->AddText(top_left_align, text_color, type_formid.c_str());
		}

		// Draw Plugin (Bottom Align)
		if (!compactView) {
			const std::string plugin_name = FormatTextWidth(a_item.GetPluginName(), spacing * 1.25f);
			DrawList->AddText(bot_left_align, text_color, plugin_name.c_str());
		} else {
			const std::string plugin_name = FormatTextWidth(a_item.GetPluginName(), spacing * 1.0f);
			DrawList->AddText(center_left_align, text_color, plugin_name.c_str());
		}

		std::string name_string = "";

		// Initial name formatting based on valid name and width cutoff.
		if (!showEditorID) {
			if (a_item.GetName().empty()) {
				name_string = FormatTextWidth(a_item.GetEditorID(), spacing * 1.5f);
			} else {
				name_string = FormatTextWidth(a_item.GetName(), spacing * 1.5f);
			}
		} else {
			name_string = FormatTextWidth(a_item.GetEditorID(), spacing * 1.5f);
		}

		// Draw Weapon Specific Stuff
		// Check if a_item is of type ItemData
		if (!this->HasFlag(ModexTableFlag_Kit)) {
			if (const ItemData* itemData = dynamic_cast<const ItemData*>(&a_item)) {
				{  // Draw Value (Top-Right Align)
					const std::string string = itemData->GetValueString() + ICON_LC_COINS;
					const ImVec2 size = ImGui::CalcTextSize(string.c_str());
					if (!compactView) {
						const ImVec2 pos = ImVec2(top_right_align.x - size.x + fontSize, top_right_align.y);
						DrawList->AddText(pos, text_color, string.c_str());
					} else {
						const ImVec2 newPos = ImVec2(center_right_align.x - size.x + fontSize, center_right_align.y);
						DrawList->AddText(newPos, text_color, string.c_str());
					}
				}

				if (!compactView) {
					{  // Draw Weight (Bottom-Right Align)
						const int weight = static_cast<int>(floorf(itemData->GetWeight()));
						const std::string string = std::to_string(weight) + ICON_LC_WEIGHT;
						const ImVec2 size = ImGui::CalcTextSize(string.c_str());
						const ImVec2 pos = ImVec2(bot_right_align.x - size.x + fontSize, bot_right_align.y);
						DrawList->AddText(pos, text_color, string.c_str());
					}
				}

				if (itemData->GetFormType() == RE::FormType::Armor) {
					const auto& armor = itemData->GetForm()->As<RE::TESObjectARMO>();

					if (armor != nullptr) {
						const auto armorRating = Utils::CalcBaseArmorRating(armor);
						const auto armorType = Utils::GetArmorType(armor);
						const auto equipSlots = Utils::GetArmorSlots(armor);

						const std::string rating_string = ICON_LC_SHIELD + std::to_string(static_cast<int>(armorRating));
						const std::string type_string = _TICON(ICON_LC_PUZZLE, armorType);
						const std::string slot_string = ICON_LC_BETWEEN_HORIZONTAL_START + equipSlots[0];

						if (!compactView) {
							const ImVec2 armorRating_pos = ImVec2(bb.Min.x + spacing + spacing * 1.5f, top_align);
							DrawList->AddText(armorRating_pos, text_color, rating_string.c_str());

							const ImVec2 armorType_pos = ImVec2(bb.Min.x + spacing + spacing * 1.5f, bot_align);
							DrawList->AddText(armorType_pos, text_color, type_string.c_str());

							const ImVec2 equipSlot_pos = ImVec2(bb.Min.x + spacing + spacing * 2.5f, bot_align);
							DrawList->AddText(equipSlot_pos, text_color, slot_string.c_str());
						} else {
							const ImVec2 armorRating_pos = ImVec2(bb.Min.x + spacing + spacing * 1.5f, center_align);
							DrawList->AddText(armorRating_pos, text_color, rating_string.c_str());

							const ImVec2 equipSlot_pos = ImVec2(bb.Min.x + spacing + spacing * 2.5f, center_align);
							DrawList->AddText(equipSlot_pos, text_color, slot_string.c_str());
						}

						if (armor->formEnchanting != nullptr) {
							name_string = ICON_LC_SPARKLES + name_string;
							const ImU32 enchantment_color = IM_COL32(0, 255, 255, 20);

							// Top to Bottom Gradient
							DrawList->AddRectFilledMultiColor(
								ImVec2(bb.Min.x, bb.Min.y),
								ImVec2(bb.Max.x, bb.Min.y + ((bb.Max.y / 8.0f))),
								enchantment_color,
								enchantment_color,
								IM_COL32(0, 0, 0, 0),
								IM_COL32(0, 0, 0, 0));

							// // Left to Right Gradient
							// DrawList->AddRectFilledMultiColor(
							// 	ImVec2(bb.Min.x, bb.Min.y),
							// 	ImVec2(bb.Min.x + ((bb.Max.x / 8.0f)), bb.Max.y),
							// 	enchantment_color,
							// 	IM_COL32(0, 0, 0, 0),
							// 	IM_COL32(0, 0, 0, 0),
							// 	enchantment_color);

							// // Right to Left Gradient
							// DrawList->AddRectFilledMultiColor(
							// 	ImVec2(bb.Max.x - ((bb.Max.x / 8.0f)), bb.Min.y),
							// 	ImVec2(bb.Max.x, bb.Max.y),
							// 	IM_COL32(0, 0, 0, 0),
							// 	enchantment_color,
							// 	enchantment_color,
							// 	IM_COL32(0, 0, 0, 0));
						}
					}
				}

				// Draw Weapon Specific Stuff
				if (itemData->GetFormType() == RE::FormType::Weapon) {
					const auto& weapon = itemData->GetForm()->As<RE::TESObjectWEAP>();

					if (weapon != nullptr) {
						const char* weaponTypes[] = {
							"Hand to Hand",
							"One Handed Sword",
							"One Handed Dagger",
							"One Handed Axe",
							"One Handed Mace",
							"Two Handed Greatsword",
							"Two Handed Battleaxe",
							"Bow",
							"Staff",
							"Crossbow"
						};  // still need to move this rofl.

						const int damage = static_cast<int>(Utils::CalcBaseDamage(weapon));
						const RE::ActorValue skill = weapon->weaponData.skill.get();
						const auto type = weaponTypes[static_cast<int>(weapon->GetWeaponType())];

						const std::string damage_string = ICON_LC_SWORD + std::to_string(damage);
						const std::string skill_string = ICON_LC_BRAIN + std::to_string(skill);
						const std::string type_string = _TICON(ICON_LC_PUZZLE, type);

						if (!compactView) {
							const ImVec2 damage_pos = ImVec2(bb.Min.x + spacing + spacing * 1.5f, top_align);
							const ImVec2 skill_pos = ImVec2(bb.Min.x + spacing + spacing * 1.5f, center_align);
							const ImVec2 type_pos = ImVec2(bb.Min.x + spacing + spacing * 1.5f, bot_align);

							DrawList->AddText(damage_pos, text_color, damage_string.c_str());
							DrawList->AddText(skill_pos, text_color, skill_string.c_str());
							DrawList->AddText(type_pos, text_color, type_string.c_str());
						} else {
							const ImVec2 damage_pos = ImVec2(bb.Min.x + spacing + spacing * 1.5f, center_align);
							const ImVec2 type_pos = ImVec2(bb.Min.x + spacing + spacing * 2.5f, center_align);
							DrawList->AddText(damage_pos, text_color, damage_string.c_str());
							DrawList->AddText(type_pos, text_color, type_string.c_str());
						}
					}
				}
			}
		}

		// Since the name is conditional based on a few things, we draw it last.
		// For example, we need to first dynamic_cast and check for enchantment to append the symbol.
		if (!compactView) {
			const ImVec2 name_pos = ImVec2(bb.Min.x + spacing, top_align);
			DrawList->AddText(name_pos, text_color, name_string.c_str());
		} else {
			const ImVec2 name_pos = ImVec2(bb.Min.x + spacing, center_align);
			DrawList->AddText(name_pos, text_color, name_string.c_str());
		}
	}

	template <typename DataType>
	void TableView<DataType>::DrawDragDropPayload(DragBehavior a_behavior)
	{
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.0f, 0.0f, 0.0f, 0.75f));
		if (ImGui::BeginTooltip()) {
			std::string text;
			if (a_behavior == DragBehavior::DragBehavior_Add) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				text = ICON_LC_PLUS " Copy";
			} else if (a_behavior == DragBehavior::DragBehavior_Invalid) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
				text = ICON_LC_X " Require's a kit to be selected";
			} else if (a_behavior == DragBehavior::DragBehavior_Remove) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				text = ICON_LC_MINUS " Delete";
			} else {
				ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
				const auto payload_size = ImGui::GetDragDropPayload()->DataSize / (int)sizeof(ImGuiID);
				if (payload_size > 1) {
					text = ICON_LC_BOXES "(" + std::to_string(payload_size) + ") Item's Selected";
				} else {
					text = ICON_LC_BOX "(" + std::to_string(payload_size) + ") Item Selected";
				}
			}
			ImGui::Text("%s", text.c_str());

			ImGui::EndTooltip();
		}
		ImGui::PopStyleColor();
	}

	template <typename DataType>
	void TableView<DataType>::Draw()
	{
		if (ImGui::BeginChild("##TableView::Draw", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoMove)) {
			if (showKitView) {
				KitView();
				ImGui::EndChild();
				return;
			}

			DrawList = ImGui::GetWindowDrawList();

			// Setup table layout based on available width
			const float a_width = ImGui::GetContentRegionAvail().x;
			UpdateLayout(a_width);

			// Calculate and store start position of table.
			const ImVec2 start_pos = ImGui::GetCursorScreenPos();
			ImGui::SetCursorScreenPos(start_pos);

			// Build table meta data and structures
			const int COLUMN_COUNT = LayoutColumnCount;  // shouldn't stay const
			const int ITEMS_COUNT = static_cast<int>(tableList.size());

			ImGuiListClipper clipper;
			ImGuiMultiSelectIO* ms_io = ImGui::BeginMultiSelect(ms_flags, selectionStorage.Size, ITEMS_COUNT);
			selectionStorage.UserData = (void*)&tableList;
			selectionStorage.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage* self, int idx) {
				TableList* a_items = (TableList*)self->UserData;
				return (*a_items)[idx]->TableID;  // Index -> TableID
			};
			selectionStorage.ApplyRequests(ms_io);

			// Start clipper and iterate through table's item list.
			clipper.Begin(ITEMS_COUNT, LayoutItemStep.y);
			if (ms_io->RangeSrcItem != -1) {
				clipper.IncludeItemByIndex((int)ms_io->RangeSrcItem);  // Ensure RangeSrc item is not clipped.
			}

			while (clipper.Step()) {
				const int item_start = clipper.DisplayStart;
				const int item_end = clipper.DisplayEnd;

				for (int line_idx = item_start; line_idx < item_end; line_idx++) {
					const int item_min_idx_for_current_line = line_idx * COLUMN_COUNT;
					const int item_max_idx_for_current_line = std::min((line_idx + 1) * COLUMN_COUNT, ITEMS_COUNT);

					for (int item_idx = item_min_idx_for_current_line; item_idx < item_max_idx_for_current_line; ++item_idx) {
						auto& item_data = tableList[item_idx];
						ImGui::PushID((int)item_data->TableID);

						// position item at start
						ImVec2 pos = ImVec2(start_pos.x, start_pos.y + line_idx * LayoutItemStep.y);
						ImGui::SetCursorScreenPos(pos);

						// set next item selection user data
						ImGui::SetNextItemSelectionUserData(item_idx);
						bool is_item_selected = selectionStorage.Contains(item_data->TableID);
						bool is_item_visible = ImGui::IsRectVisible(LayoutItemSize);

						// If we implement a grid or gapped layout, this will be needed.
						// For now, it just removes the spacing so the selection is within the bounds of the item.
						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(LayoutSelectableSpacing, LayoutSelectableSpacing));
						if (this->HasFlag(ModexTableFlag_Kit)) {
							ImGui::Selectable("", is_item_selected, ImGuiSelectableFlags_None, ImVec2(LayoutItemSize.x / 1.75f, LayoutItemSize.y));
						} else {
							ImGui::Selectable("", is_item_selected, ImGuiSelectableFlags_None, LayoutItemSize);
						}
						ImGui::PopStyleVar();

						if (ImGui::IsItemHovered()) {
							// itemPreview = std::move(item_data);
							itemPreview = std::make_unique<DataType>(*item_data);
						}

						if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
							ImGui::OpenPopup("TableViewContextMenu");
						}

						if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
							if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
								Console::AddItem(item_data->GetFormID().c_str(), 1);
								Console::StartProcessThread();
							}
						}

						// instantly toggle, bypassing multi selection backend
						if (ImGui::IsItemToggledSelection()) {
							is_item_selected = !is_item_selected;
						}

						// constexpr auto drag_drop_flags = ImGuiDragDropFlags_AcceptPeekOnly;
						constexpr auto drag_drop_flags = ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_SourceNoPreviewTooltip;

						// Drag and drop
						if (ImGui::BeginDragDropSource(drag_drop_flags)) {
							// Create payload with full selection OR single unselected item.
							// (the later is only possible when using ImGuiMultiSelectFlags_SelectOnClickRelease)
							if (ImGui::GetDragDropPayload() == NULL) {
								ImVector<ImGuiID> payload_items;
								void* it = NULL;
								ImGuiID id = 0;
								if (!is_item_selected)
									payload_items.push_back(item_data->TableID);
								else
									while (selectionStorage.GetNextSelectedItem(&it, &id))
										payload_items.push_back(id);
								ImGui::SetDragDropPayload(this->dragDropSourceID.c_str(), payload_items.Data, (size_t)payload_items.size_in_bytes());
							}

							ImGuiContext& g = *GImGui;
							const ImGuiPayload* payload = ImGui::GetDragDropPayload();
							const int payload_count = (int)payload->DataSize / (int)sizeof(ImGuiID);

							if (!g.DragDropAcceptIdPrev) {
								if (payload_count == 1) {
									this->DrawDragDropPayload(DragBehavior_None);
								} else {
									this->DrawDragDropPayload(DragBehavior_None);
								}
							}

							ImGui::EndDragDropSource();
						}

						// if the item is visible, offload drawing to the table's draw function
						if (is_item_visible) {
							DrawItem(*item_data, pos, is_item_selected);

							if (this->HasFlag(ModexTableFlag_Kit)) {
								ImVec2 equippable_pos;

								if (compactView) {
									equippable_pos = ImVec2(pos.x + (LayoutItemSize.x / 1.75f), pos.y);
								} else {
									equippable_pos = ImVec2(pos.x + (LayoutItemSize.x / 1.75f), pos.y);
								}

								ImGui::SetCursorScreenPos(equippable_pos);

								auto& collection = PersistentData::GetLoadedKits();
								auto& kit = collection.at(*this->selectedKit);

								for (auto& kit_item : kit.items) {
									if (kit_item->editorid == item_data->GetEditorID()) {
										const auto alpha = kit_item->equipped ? 1.0f : 0.25f;
										const auto text = kit_item->equipped ? _T("Equip") : _T("Equip");
										const auto icon = kit_item->equipped ? ICON_LC_CHECK : ICON_LC_X;
										const auto equip_size = ImVec2(LayoutItemSize.x / 7.0f, LayoutItemSize.y);
										const auto color = ImGui::GetColorU32(ImGuiCol_Button, alpha);
										const auto padding = 5.0f;

										ImGui::PushStyleColor(ImGuiCol_Button, color);
										ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
										ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

										if (item_data->GetForm()->Is(RE::FormType::Armor) || item_data->GetForm()->Is(RE::FormType::Weapon)) {
											if (ImGui::GradientButton((std::string(icon) + text).c_str(), equip_size)) {
												kit_item->equipped = !kit_item->equipped;
												PersistentData::GetSingleton()->SaveKitToJSON(kit);
											}

											ImGui::SameLine(0.0f, padding);
										} else {
											const auto form_type_text = RE::FormTypeToString(item_data->GetFormType());
											if (ImGui::GradientButton(form_type_text.data(), equip_size)) {
												// nothing?
											}
											ImGui::SameLine(0.0f, padding);
										}

										ImGui::PopStyleColor();
										ImGui::PopStyleVar(2);

										ImGui::SetCursorPosY(ImGui::GetCursorPosY() + LayoutOuterPadding);

										ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - LayoutOuterPadding);
										if (ImGui::InputInt("##EquipCount", &kit_item->amount, 0, 0)) {
											PersistentData::GetSingleton()->SaveKitToJSON(kit);
										}

										if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary)) {
											ImGui::SetTooltip(_T("TOOLTIP_AMOUNT_ADD"));
										}

										// item_data->equippable = kit_item->equippable;
										break;
									}
								}
							}
						}

						if (ImGui::BeginPopup("TableViewContextMenu")) {
							if (const ItemData* itemData = dynamic_cast<const ItemData*>(item_data.get())) {
								void* it = NULL;
								ImGuiID id = 0;

								// Add to kit?
								// These need to be abstracted out to Single Purpose Functions

								if (ImGui::MenuItem(_T("AIM_ADD"))) {
									if (selectionStorage.Size == 0) {
										Console::AddItem(item_data->GetFormID().c_str(), 1);
									} else {
										if (item_data == itemPreview && !is_item_selected) {
											Console::AddItem(item_data->GetFormID().c_str(), 1);
										} else {
											while (selectionStorage.GetNextSelectedItem(&it, &id)) {
												const auto& item = tableList[id];
												Console::AddItem(item->GetFormID().c_str(), 1);
											}
										}
									}

									Console::StartProcessThread();
								}

								if (ImGui::MenuItem(_T("AIM_PLACE"))) {
									if (selectionStorage.Size == 0) {
										Console::PlaceAtMe(item_data->GetFormID().c_str(), 1);
									} else {
										while (selectionStorage.GetNextSelectedItem(&it, &id)) {
											const auto& item = tableList[id];
											Console::PlaceAtMe(item->GetFormID().c_str(), 1);
										}
									}

									Console::StartProcessThread();
								}

								ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
							}

							if (ImGui::MenuItem("Copy FormID")) {
								ImGui::SetClipboardText(item_data->GetFormID().c_str());
							}

							if (ImGui::MenuItem("Copy EditorID")) {
								ImGui::SetClipboardText(item_data->GetEditorID().c_str());
							}

							if (ImGui::MenuItem("Copy Name")) {
								ImGui::SetClipboardText(item_data->GetName().c_str());
							}

							if (ImGui::MenuItem("Copy Plugin")) {
								ImGui::SetClipboardText(item_data->GetPluginName().c_str());
							}

							ImGui::EndPopup();
						}

						ImGui::PopID();
					}
				}
			}
			clipper.End();

			ms_io = ImGui::EndMultiSelect();
			selectionStorage.ApplyRequests(ms_io);

			ImGui::EndChild();
		}

		if (ImGui::BeginDragDropTarget()) {
			if (!this->HasFlag(ModexTableFlag_Kit)) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FROM_KIT", ImGuiDragDropFlags_AcceptBeforeDelivery)) {
					if (payload->IsDelivery()) {
						const auto dragDropSourceTable = this->dragDropSourceList.at("FROM_KIT");
						const int payload_count = (int)payload->DataSize / (int)sizeof(ImGuiID);

						std::vector<std::unique_ptr<DataType>> payload_items;
						for (int payload_idx = 0; payload_idx < payload_count; ++payload_idx) {
							const ImGuiID payload_item_id = ((ImGuiID*)payload->Data)[payload_idx];
							const auto& payload_item = (*dragDropSourceTable->GetTableListPtr())[payload_item_id];
							payload_items.emplace_back(std::make_unique<DataType>(*payload_item));
						}

						for (const auto& payload_item : payload_items) {
							dragDropSourceTable->GetTableListPtr()->erase(
								std::remove_if(dragDropSourceTable->GetTableListPtr()->begin(), dragDropSourceTable->GetTableListPtr()->end(),
									[&payload_item](const std::unique_ptr<DataType>& item) {
										return item->GetEditorID() == payload_item->GetEditorID();
									}),
								dragDropSourceTable->GetTableListPtr()->end());
						}

						dragDropSourceTable->SyncChangesToKit();
						dragDropSourceTable->Refresh();

						// We previously did this in Refresh() but really the only time this condition
						// would change, is when adding or removing items from a kit
						this->LoadKitsFromPlugin();

						// In-table kit view is disabled if there are no kits in the plugin.
						if (this->kitList.empty()) {
							this->showKitView = false;
						}
					}

					if (payload->IsPreview()) {
						this->DrawDragDropPayload(DragBehavior_Remove);
					}
				}

				ImGui::EndDragDropTarget();
			}

			// This operation is called within the context of the Kit Table View
			// TODO: Impelement something here so that if items are dragged over while no kit is selected
			// it will prompt the user to create a kit, then store the dragged items in the new kit.

			if (this->HasFlag(ModexTableFlag_Kit)) {
				if (this->selectedKit) {
					const std::string selected_kit = *this->selectedKit;

					if (!selected_kit.empty() && selected_kit != _T("None")) {
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FROM_TABLE", ImGuiDragDropFlags_AcceptBeforeDelivery)) {
							if (payload->IsDelivery()) {
								const auto dragDropSourceTable = this->dragDropSourceList.at("FROM_TABLE");
								const int payload_count = (int)payload->DataSize / (int)sizeof(ImGuiID);

								for (int payload_idx = 0; payload_idx < payload_count; ++payload_idx) {
									const ImGuiID payload_item_id = ((ImGuiID*)payload->Data)[payload_idx];
									const auto& payload_item = (*dragDropSourceTable->GetTableListPtr())[payload_item_id];

									this->AddPayloadItemToKit(payload_item);
								}

								this->SyncChangesToKit();
								this->Refresh();

								// We previously did this in Refresh() but really the only time this condition
								// would change, is when adding or removing items from a kit
								dragDropSourceTable->LoadKitsFromPlugin();

								// In-table kit view is disabled if there are no kits in the plugin.
								if (dragDropSourceTable->kitList.empty()) {
									dragDropSourceTable->showKitView = false;
								}
							}

							if (payload->IsPreview()) {
								// ImGui::SetTooltip("Add to Kit");
								this->DrawDragDropPayload(DragBehavior_Add);
							}
						}
					} else {
						this->DrawDragDropPayload(DragBehavior_Invalid);
						// ImGui::SetTooltip("You need to create and/or select a Kit before dragging items here.");
					}
				}

				ImGui::EndDragDropTarget();
			}

			selectionStorage.Clear();
			kitSelectionStorage.Clear();
		}
	}
}