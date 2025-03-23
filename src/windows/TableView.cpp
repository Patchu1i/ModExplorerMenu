#include "include/C/Console.h"
#include "include/I/ItemPreview.h"
#include "include/M/Menu.h"
#include "include/S/SimpleImeIntegration.h"
#include "include/T/Table.h"
#include <execution>


namespace Modex
{

	template class TableView<ItemData>;
	template class TableView<NPCData>;
	template class TableView<ObjectData>;

	void AddKitToInventory(const std::unique_ptr<Kit>& a_kit)
	{
		if (a_kit->items.empty()) {
			return;
		}

		if (a_kit->clearEquipped) {
			Console::ClearEquipped();
		}

		for (auto& item : a_kit->items) {
			if (RE::TESForm* form = RE::TESForm::LookupByEditorID(item->editorid)) {
				Console::AddItemEx(form->GetFormID(), item->amount, item->equipped);
			}
		}

		Console::StartProcessThread();
	}

	void DeleteKit(const std::unique_ptr<Kit>& a_kit)
	{
		PersistentData::GetSingleton()->DeleteKit(a_kit->name);
	}

	template <typename DataType>
	void TableView<DataType>::AddSelectionToInventory(int a_count)
	{
		if (this->tableList.empty()) {
			return;
		}

		void* it = NULL;
		ImGuiID id = 0;

		while (selectionStorage.GetNextSelectedItem(&it, &id)) {
			if (id < std::ssize(tableList) && id >= 0) {
				const auto& item = tableList[id];
				Console::AddItem(item->GetFormID().c_str(), a_count);
			}
		}

		Console::StartProcessThread();
		this->selectionStorage.Clear();
	}

	template <typename DataType>
	void TableView<DataType>::EquipSelection()
	{
		if (this->tableList.empty()) {
			return;
		}

		void* it = NULL;
		ImGuiID id = 0;

		while (selectionStorage.GetNextSelectedItem(&it, &id)) {
			if (id < std::ssize(tableList) && id >= 0) {
				const auto& item = tableList[id];
				Console::AddItemEx(item->GetBaseForm(), 1, true);
			}
		}

		Console::StartProcessThread();
		this->selectionStorage.Clear();
	}

	template <typename DataType>
	void TableView<DataType>::PlaceSelectionOnGround(int a_count)
	{
		if (this->tableList.empty()) {
			return;
		}

		void* it = NULL;
		ImGuiID id = 0;

		while (selectionStorage.GetNextSelectedItem(&it, &id)) {
			if (id < std::ssize(tableList) && id >= 0) {
				const auto& item = tableList[id];
				Console::PlaceAtMe(item->GetFormID().c_str(), a_count);
			}
		}

		Console::StartProcessThread();
		this->selectionStorage.Clear();
	}

	// Since "All" commands may obscure large operations, we want to add a warning.
	// This is also because passing a lambda with a while loop is a bit of a pain.

	template <typename DataType>
	void TableView<DataType>::AddAll()
	{
		if (this->tableList.empty()) {
			return;
		}

		for (auto& item : this->tableList) {
			Console::AddItem(item->GetFormID().c_str(), 1);
		}

		Console::StartProcessThread();
		this->selectionStorage.Clear();
	}

	template <typename DataType>
	void TableView<DataType>::PlaceAll()
	{
		if (this->tableList.empty()) {
			return;
		}

		for (auto& item : this->tableList) {
			Console::PlaceAtMe(item->GetFormID().c_str(), 1);
		}

		Console::StartProcessThread();
		this->selectionStorage.Clear();
	}

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
			RE::FormType::Scroll,
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

		this->sortByListKit = {
			SortType::Name
		};

		this->BuildPluginList();
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

		this->sortByList = {
			SortType::Plugin,
			SortType::FormID,
			SortType::Name,
			SortType::EditorID,
			SortType::Class,
			SortType::Race,
			SortType::Gender,
			SortType::Level,
			SortType::ReferenceID,
			SortType::Health,
			SortType::Magicka,
			SortType::Stamina
		};

		this->BuildPluginList();
	}

	void TableView<ObjectData>::Init()
	{
		this->inputSearchMap = {
			{ SortType::Name, "Name" },
			{ SortType::EditorID, "Editor ID" },
			{ SortType::FormID, "Form ID" }
		};

		this->primaryFilterList = {
			RE::FormType::Tree,
			RE::FormType::Static,
			RE::FormType::Container,
			RE::FormType::Activator,
			RE::FormType::Light,
			RE::FormType::Door,
			RE::FormType::Furniture,
			RE::FormType::Flora
		};

		this->sortByList = {
			SortType::Plugin,
			SortType::FormID,
			SortType::Name,
			SortType::EditorID
		};

		this->BuildPluginList();
	}

	void TableView<CellData>::Init()
	{
		this->inputSearchMap = {
			{ SortType::Name, "Name" },
			{ SortType::EditorID, "Editor ID" },
			{ SortType::FormID, "Form ID" }
		};

		this->primaryFilterList = {
			RE::FormType::None
		};

		this->sortByList = {
			SortType::Plugin,
			SortType::Name
		};

		this->BuildPluginList();
	}

	template <typename DataType>
	void TableView<DataType>::Unload()
	{
		this->tableList.clear();
		this->pluginList.clear();
		this->selectionStorage.Clear();
		this->dragDropSourceList.clear();
	}

	template <typename DataType>
	void TableView<DataType>::Load()
	{
		this->Reset();
	}

	template <typename DataType>
	void TableView<DataType>::AddDragDropTarget(const std::string a_id, TableView* a_view)
	{
		this->dragDropSourceList[a_id] = a_view;
	}

	template <class DataType>
	void TableView<DataType>::RemoveDragDropTarget(const std::string a_id)
	{
		auto it = this->dragDropSourceList.find(a_id);
		if (it != this->dragDropSourceList.end()) {
			this->dragDropSourceList.erase(it);
		}
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
	uint32_t TableView<DataType>::GetSelectionCount() const
	{
		return selectionStorage.Size;
	}

	template <typename DataType>
	void TableView<DataType>::AddPayloadItemToKit(const std::unique_ptr<DataType>& a_item)
	{
		const auto& collection = PersistentData::GetLoadedKits();
		if (auto it = collection.find(*this->selectedKit); it != collection.end()) {
			auto& kit = it->second;

			if (kit.readOnly) {
				return;
			}
		}

		for (auto& item : this->tableList) {
			if (item->GetEditorID() == a_item->GetEditorID()) {
				return;
			}
		}

		this->tableList.emplace_back(std::make_unique<DataType>(*a_item));
	}

	template <typename DataType>
	void TableView<DataType>::AddSelectedToKit()
	{
		if (auto map = this->dragDropSourceList.find("FROM_KIT"); map != this->dragDropSourceList.end()) {
			const auto dragDropSourceTable = map->second->GetTableListPtr();

			void* it = NULL;
			ImGuiID id = 0;

			while (selectionStorage.GetNextSelectedItem(&it, &id)) {
				if (id < tableList.size() && id >= 0) {
					const auto& item = this->tableList[id];

					auto iter = std::find_if(dragDropSourceTable->begin(), dragDropSourceTable->end(),
						[&item](const std::unique_ptr<DataType>& a_item) {
							return a_item->GetEditorID() == item->GetEditorID();
						});

					if (iter == dragDropSourceTable->end()) {
						dragDropSourceTable->emplace_back(std::make_unique<DataType>(*item));
					}
				}
			}
		}
	}

	template <typename DataType>
	void TableView<DataType>::RemoveSelectedFromKit()
	{
		if (this->tableList.empty()) {
			return;
		}

		void* it = NULL;
		ImGuiID id = 0;

		std::vector<std::unique_ptr<DataType>> items_to_remove;

		while (selectionStorage.GetNextSelectedItem(&it, &id)) {
			if (id < tableList.size() && id >= 0) {
				items_to_remove.emplace_back(std::make_unique<DataType>(*this->tableList[id]));
			}
		}

		for (const auto& item : items_to_remove) {
			this->RemovePayloadItemFromKit(item);
		}
	}

	template <typename DataType>
	void TableView<DataType>::RemovePayloadItemFromKit(const std::unique_ptr<DataType>& a_item)
	{
		if (this->tableList.empty()) {
			return;
		}

		const auto& collection = PersistentData::GetLoadedKits();
		if (auto it = collection.find(*this->selectedKit); it != collection.end()) {
			auto& kit = it->second;

			if (kit.readOnly) {
				return;
			}
		}

		auto it = std::remove_if(this->tableList.begin(), this->tableList.end(),
			[&a_item](const std::unique_ptr<DataType>& item) {
				return item->GetEditorID() == a_item->GetEditorID();
			});

		this->tableList.erase(it, this->tableList.end());
	}

	template <typename DataType>
	void TableView<DataType>::SyncChangesToKit()
	{
		if (this->HasFlag(ModexTableFlag_Kit)) {
			if (this->selectedKit) {
				auto& collection = PersistentData::GetLoadedKits();
				if (auto it = collection.find(*this->selectedKit); it != collection.end()) {
					auto& kit = it->second;

					kit.items.clear();

					if (!this->tableList.empty()) {
						for (auto& item : this->tableList) {
							kit.items.emplace(CreateKitItem(*item));
						}
					}

					PersistentData::GetSingleton()->SaveKitToJSON(kit);
				}
			}
		}
	}

	template <typename DataType>
	void TableView<DataType>::Refresh()
	{
		assert(this->generator);

		selectionStorage.Clear();

		if (this->HasFlag(ModexTableFlag_Kit)) {
			if (this->selectedKit) {
				this->Filter(this->generator());

				this->SortListBySpecs();
				this->UpdateImGuiTableIDs();
				this->UpdateKitItemData();
			}
		} else {
			this->Filter(this->generator());
			this->SortListBySpecs();
			this->UpdateImGuiTableIDs();

			// In-table kit view is disabled if there are no kits in the plugin.
			if (this->kitList.empty()) {
				this->showPluginKitView = false;
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

	template <typename DataType>
	void TableView<DataType>::Filter(const std::vector<DataType>& a_data)
	{
		tableList.clear();
		tableList.reserve(std::ssize(a_data));

		this->generalSearchDirty = false;
		ImFormatString(this->lastSearchBuffer, IM_ARRAYSIZE(this->lastSearchBuffer), "%s", this->generalSearchBuffer);

		this->totalGenerated = std::ssize(a_data);

		std::string inputString = this->generalSearchBuffer;
		std::transform(inputString.begin(), inputString.end(), inputString.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

		for (const auto& item : a_data) {
			if (this->HasFlag(ModexTableFlag_Kit)) {
				this->tableList.emplace_back(std::make_unique<DataType>(item.GetForm(), 0, item.refID));
				continue;
			}

			std::string compareString;

			if (!inputString.empty()) {
				switch (this->searchKey) {
				case SortType::Name:
					compareString = item.GetNameView();
					break;
				case SortType::FormID:
					compareString = item.GetFormID();
					break;
				case SortType::EditorID:
					compareString = item.GetEditorID();
					break;
				default:
					compareString = item.GetNameView();
					break;
				}

				std::transform(compareString.begin(), compareString.end(), compareString.begin(),
					[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

				// If the input is wrapped in quotes, we do an exact match across all parameters.
				if (!inputString.empty() && inputString.front() == '"' && inputString.back() == '"') {
					std::string match = inputString.substr(1, inputString.size() - 2);

					if (compareString == match) {
						this->tableList.emplace_back(std::make_unique<DataType>(item.GetForm(), 0, item.refID));
					}

					continue;
				}
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

			if constexpr (std::is_same_v<DataType, ItemData>) {
				// Non-playable
				if (this->hideNonPlayable && item.IsNonPlayable()) {
					continue;
				}

				// Hide Enchanted
				if (this->hideEnchanted) {
					if (item.GetFormType() == RE::FormType::Armor || item.GetFormType() == RE::FormType::Weapon) {
						if (const auto& enchantable = item.GetForm()->As<RE::TESEnchantableForm>()) {
							if (enchantable->formEnchanting != nullptr) {
								continue;
							}
						}
					}
				}
			}

			if constexpr (std::is_same_v<DataType, NPCData>) {
				if (this->hideNonUnique && !item.GetTESNPC()->IsUnique()) {
					continue;
				}

				if (this->hideNonEssential && !item.GetTESNPC()->IsEssential()) {
					continue;
				}
			}

			// Primary Filter
			if (this->primaryFilter != RE::FormType::None) {
				if constexpr (std::is_same_v<DataType, ItemData>) {
					if (item.GetFormType() != this->primaryFilter) {
						continue;
					}
				}

				if constexpr (std::is_same_v<DataType, ObjectData>) {
					if (item.GetFormType() != this->primaryFilter) {
						continue;
					}
				}
			}

			// Secondary Filter
			if (this->primaryFilter != RE::FormType::None && this->secondaryFilter != _T("All")) {
				if constexpr (std::is_same_v<DataType, ItemData>) {
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

				if constexpr (std::is_same_v<DataType, NPCData>) {
					if (this->primaryFilter == RE::FormType::Class) {
						auto npcClass = item.GetClass();

						if (npcClass != this->secondaryFilter) {
							continue;
						}
					}

					if (this->primaryFilter == RE::FormType::Race) {
						auto npcRace = item.GetRace();

						if (npcRace != this->secondaryFilter) {
							continue;
						}
					}

					if (this->primaryFilter == RE::FormType::Faction) {
						auto npcFaction = item.GetFactions();

						bool skip = true;
						for (auto& faction : npcFaction) {
							if (ValidateTESName(faction.faction) == this->secondaryFilter) {
								skip = false;
								break;
							}
						}

						if (skip) {
							continue;
						}
					}
				}
			}

			if (!inputString.empty()) {
				if (compareString.find(inputString) != std::string::npos) {
					this->tableList.emplace_back(std::make_unique<DataType>(item.GetForm(), 0, item.refID));
				}
			} else {
				this->tableList.emplace_back(std::make_unique<DataType>(item.GetForm(), 0, item.refID));
			}
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
				for (auto& filter : list) {
					if (filter.empty()) {
						continue;
					}

					if (filter.find(this->secondaryFilterBuffer) != std::string::npos) {
						this->secondaryFilter = filter;
						ImFormatString(this->secondaryFilterBuffer, IM_ARRAYSIZE(this->secondaryFilterBuffer), "%s", this->secondaryFilter.c_str());
						break;
					}
				}
			}

			this->selectionStorage.Clear();
			this->Refresh();
		}
	}

	// TODO: Do I need to reimplement this for each data type so that the plugin set is correctly attributed?
	// We removed the correlation between the plugin set and primary form type to speed up filtering when
	// selecting a primary filter. So reverting this would reintroduce those changes because we'd have to
	// then build the list everytime we filter, no?

	// This should only be called once when the module is loaded, and then only when the module is selected.
	// Since the only thing that modifies these members is the blacklist.
	// We could implement some sort of smart cross-compare to further reduce the calls made to this.

	// 2/12/2025 - Just going to hold off on implementing the FormType filter since it's probably
	// confusing to users anyways.

	template <class DataType>
	void TableView<DataType>::BuildPluginList()
	{
		const auto& config = Settings::GetSingleton()->GetConfig();
		this->pluginList = Data::GetSingleton()->GetFilteredListOfPluginNames(this->pluginType, (Data::SORT_TYPE)config.modListSort, this->primaryFilter);
		this->pluginSet = Data::GetSingleton()->GetModulePluginList(this->pluginType);
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
		if (this->primaryFilter == RE::FormType::Armor || this->primaryFilter == RE::FormType::Weapon ||
			this->primaryFilter == RE::FormType::Class || this->primaryFilter == RE::FormType::Race ||
			this->primaryFilter == RE::FormType::Faction) {
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
			this->Refresh();
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
		if (ImGui::BeginCombo("##Search::Filter::PrimaryFilter", filter_name.c_str(), ImGuiComboFlags_HeightLarge)) {
			if (ImGui::Selectable(_T("None"), this->primaryFilter == RE::FormType::None)) {
				ImGui::SetItemDefaultFocus();
				this->primaryFilter = RE::FormType::None;
				this->secondaryFilter = _T("All");
				this->Refresh();
			}

			for (auto& filter : this->primaryFilterList) {
				const bool is_selected = (filter == this->primaryFilter);
				const std::string option = RE::FormTypeToString(filter).data();

				if (ImGui::Selectable(option.c_str(), is_selected)) {
					this->primaryFilter = filter;
					this->secondaryFilter = _T("All");
					this->Refresh();
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

			if (ImGui::BeginCombo("##Search::Filter::SecondaryFilter", this->secondaryFilter.c_str(), ImGuiComboFlags_HeightLarge)) {
				if (ImGui::Selectable(_T("All"), this->secondaryFilter == _T("All"))) {
					ImGui::SetItemDefaultFocus();
					this->secondaryFilter = _T("All");
					this->Refresh();
				}

				// TODO: Does this need localized? If it does, we also need to
				// localize it inside the TableView draw call.
				const auto armor_slots = Utils::GetArmorSlotList();

				for (auto& slot : armor_slots) {
					bool is_selected = (slot == secondaryFilter);

					if (ImGui::Selectable(_T(slot), is_selected)) {
						this->secondaryFilter = slot;
						this->Refresh();
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
			if (ImGui::BeginCombo("##Search::Filter::SecondaryFilter", this->secondaryFilter.c_str(), ImGuiComboFlags_HeightLarge)) {
				if (ImGui::Selectable(_T("All"), this->secondaryFilter == _T("All"))) {
					ImGui::SetItemDefaultFocus();
					this->secondaryFilter = _T("All");
					this->Refresh();
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
						this->Refresh();
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
			this->SecondaryNPCFilter(classList, total_width);
		}

		if (this->primaryFilter == RE::FormType::Race) {
			const auto raceList = Data::GetSingleton()->GetNPCRaceList();
			this->SecondaryNPCFilter(raceList, total_width);
		}

		if (this->primaryFilter == RE::FormType::Faction) {
			const auto factionList = Data::GetSingleton()->GetNPCFactionList();
			this->SecondaryNPCFilter(factionList, total_width);
		}

		ImGui::NewLine();

		ImGui::SetCursorPosY(a_height - ImGui::GetFrameHeightWithSpacing() * 2.0f);
		ImGui::Text(_TFM("GENERAL_FILTER_PLUGINS", ":"));

		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip(_T("TOOLTIP_FILTER_PLUGINS"));
		}

		if (InputTextComboBox("##Search::Filter::PluginField", this->pluginSearchBuffer, this->selectedPlugin, IM_ARRAYSIZE(this->pluginSearchBuffer), this->pluginList, total_width)) {
			this->selectedPlugin = _T("SHOW_ALL_PLUGINS");

			if (this->selectedPlugin.find(this->pluginSearchBuffer) != std::string::npos) {
				ImFormatString(this->pluginSearchBuffer, IM_ARRAYSIZE(this->pluginSearchBuffer), "");
			} else {
				for (auto& plugin : this->pluginSet) {
					if (PersistentData::GetBlacklist().contains(plugin)) {
						continue;
					}

					std::string pluginName = Modex::ValidateTESFileName(plugin);

					if (pluginName == _T("SHOW_ALL_PLUGINS")) {
						ImFormatString(this->pluginSearchBuffer, IM_ARRAYSIZE(this->pluginSearchBuffer), "");
						break;
					}

					if (pluginName.find(this->pluginSearchBuffer) != std::string::npos) {
						this->selectedPlugin = pluginName;
						ImFormatString(this->pluginSearchBuffer, IM_ARRAYSIZE(this->pluginSearchBuffer), "");
						break;
					}
				}
			}

			this->LoadKitsFromSelectedPlugin();
			this->selectionStorage.Clear();
			this->Refresh();
		}

		ImGui::NextColumn();

		const float maxWidth = ImGui::GetContentRegionAvail().x;
		const auto InlineText = [maxWidth](const char* label, const char* text, const char* tooltip) {
			const auto width = std::max(maxWidth - ImGui::CalcTextSize(text).x, ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(text).x);
			ImGui::Text(label);
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip(tooltip);
			}
			ImGui::SameLine(width);
			ImGui::Text(text);
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip(tooltip);
			}
		};

		const auto total_plugins = std::ssize(pluginList);
		const auto total_items = std::ssize(tableList);
		const auto total_blacklist = std::ssize(PersistentData::GetBlacklist());
		const auto total_kits = std::ssize(PersistentData::GetLoadedKits());
		const auto total_kits_in_plugin = std::ssize(this->kitList);

		// Calling this->generator() to compute hidden items should be okay since it's a reference
		// to a cached table, and we're not re-generating it every frame.

		std::ptrdiff_t empty = 0;
		std::ptrdiff_t hidden_items = std::max(totalGenerated - total_items, empty);
		// std::ptrdiff_t hidden_plugins = std::max(std::ssize(pluginList) - total_plugins, empty);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 4.0f));
		InlineText(
			_TICONM(ICON_LC_PACKAGE_PLUS, "GENERAL_TOTAL_PLUGINS", ":"),
			std::to_string(total_plugins).c_str(),
			_T("TOOLTIP_TOTAL_PLUGIN"));

		InlineText(
			_TICONM(ICON_LC_PACKAGE_SEARCH, "GENERAL_TOTAL_BLACKLIST", ":"),
			std::to_string(total_blacklist).c_str(),
			_T("TOOLTIP_BLACKLIST_PLUGIN"));

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		InlineText(
			_TICONM(ICON_LC_SEARCH, "Search Results", ":"),
			std::to_string(total_items).c_str(),
			_T("TOOLTIP_TOTAL_SEARCH"));

		InlineText(
			_TICONM(ICON_LC_EYE_OFF, "GENERAL_TOTAL_HIDDEN", ":"),
			std::to_string(hidden_items).c_str(),
			_T("TOOLTIP_TOTAL_FILTERED"));

		if (this->HasFlag(ModexTableFlag_EnablePluginKitView)) {
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			InlineText(
				_TICONM(ICON_LC_PACKAGE, "GENERAL_KITS_TOTAL", ":"),
				std::to_string(total_kits).c_str(),
				_T("TOOLTIP_TOTAL_KITS"));

			InlineText(
				_TICONM(ICON_LC_PACKAGE_OPEN, "GENERAL_KITS_IN_PLUGIN", ":"),
				std::to_string(total_kits_in_plugin).c_str(),
				_T("TOOLTIP_TOTAL_KITS_IN_PLUGIN"));
		}

		ImGui::PopStyleVar();
		ImGui::EndColumns();

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
	std::string TableView<DataType>::GetSortProperty(const DataType& a_item)
	{
		if constexpr (std::is_base_of<BaseObject, DataType>::value) {
			switch (this->sortBy) {
			case SortType::Name:
			case SortType::Plugin:
				return "";
			case SortType::FormID:
				return a_item.GetFormID();
			case SortType::Type:
				if (a_item.GetFormType() == RE::FormType::Weapon) {
					if (const auto& weapon = a_item.GetForm()->As<RE::TESObjectWEAP>()) {
						return Utils::IconMap["WEAPON"] + Utils::GetWeaponType(weapon);
					}
				}

				if (a_item.GetFormType() == RE::FormType::Armor) {
					if (const auto& armor = a_item.GetForm()->As<RE::TESObjectARMO>()) {
						return Utils::IconMap["ARMOR"] + Utils::GetArmorType(armor);
					}
				}

				return "";
			case SortType::EditorID:
				return a_item.GetEditorID();
			}
		}

		if constexpr (std::is_same<DataType, ItemData>::value) {
			switch (this->sortBy) {
			case SortType::Value:
			case SortType::Weight:
				return "";
			case SortType::Damage:
				if (a_item.GetFormType() == RE::FormType::Weapon) {
					if (const auto& weapon = a_item.GetForm()->As<RE::TESObjectWEAP>()) {
						return Utils::IconMap["WEAPON"] + std::to_string(static_cast<int>(Utils::CalcBaseDamage(weapon)));
					}
				}

				return "";
			case SortType::Armor:
				if (a_item.GetFormType() == RE::FormType::Armor) {
					if (const auto& armor = a_item.GetForm()->As<RE::TESObjectARMO>()) {
						return Utils::IconMap["ARMOR"] + std::to_string(static_cast<int>(Utils::CalcBaseArmorRating(armor)));
					}
				}

				return "";
			case SortType::Slot:
				if (a_item.GetFormType() == RE::FormType::Armor) {
					if (const auto& armor = a_item.GetForm()->As<RE::TESObjectARMO>()) {
						const auto equipSlots = Utils::GetArmorSlots(armor);
						if (!equipSlots.empty()) {
							return Utils::IconMap["SLOT"] + equipSlots[0];
						}
					}
				}

				return "";
			case SortType::Speed:
				if (a_item.GetFormType() == RE::FormType::Weapon) {
					if (const auto& weapon = a_item.GetForm()->As<RE::TESObjectWEAP>()) {
						const float speed = weapon->GetSpeed();
						const std::string out = std::format("{:.2f}", speed);
						return Utils::IconMap["SPEED"] + out;
					}
				}

				return "";
			case SortType::CriticalDamage:
				if (a_item.GetFormType() == RE::FormType::Weapon) {
					if (const auto& weapon = a_item.GetForm()->As<RE::TESObjectWEAP>()) {
						float critical_damage = weapon->GetCritDamage();
						const std::string out = std::format("{:.2f}", critical_damage);
						return Utils::IconMap["CRIT"] + out;
					}
				}

				return "";
			case SortType::Skill:
				if (a_item.GetFormType() == RE::FormType::Weapon) {
					if (const auto& weapon = a_item.GetForm()->As<RE::TESObjectWEAP>()) {
						return Utils::IconMap["SKILL"] + std::to_string(weapon->weaponData.skill.get());
					}
				}

				return "";
			case SortType::DamagePerSecond:
				if (a_item.GetFormType() == RE::FormType::Weapon) {
					if (const auto& weapon = a_item.GetForm()->As<RE::TESObjectWEAP>()) {
						const float damage = Utils::CalcBaseDamage(weapon);
						const float speed = weapon->GetSpeed();
						const float dps = (damage * speed);
						const std::string out = std::format("{:.2}", dps);
						return Utils::IconMap["DAMAGE"] + out;
					}
				}

				return "";
			}
		}

		if constexpr (std::is_same<DataType, NPCData>::value) {
			switch (this->sortBy) {
			case SortType::Class:
				if constexpr (std::is_same<DataType, NPCData>::value) {
					if (a_item.GetFormType() == RE::FormType::NPC) {
						return Utils::IconMap["CLASS"] + a_item.GetClass();
					}
				}

				return "";
			case SortType::Race:
				if constexpr (std::is_same<DataType, NPCData>::value) {
					if (a_item.GetFormType() == RE::FormType::NPC) {
						return Utils::IconMap["RACE"] + a_item.GetRace();
					}
				}

				return "";
			case SortType::Gender:
				if constexpr (std::is_same<DataType, NPCData>::value) {
					if (a_item.GetFormType() == RE::FormType::NPC) {
						return Utils::IconMap["GENDER"] + a_item.GetGender();
					}
				}

				return "";
			case SortType::Level:
				if constexpr (std::is_same<DataType, NPCData>::value) {
					if (a_item.GetFormType() == RE::FormType::NPC) {
						return Utils::IconMap["LEVEL"] + std::to_string(a_item.GetLevel());
					}
				}

				return "";
			case SortType::Health:
				if constexpr (std::is_same<DataType, NPCData>::value) {
					if (a_item.GetFormType() == RE::FormType::NPC) {
						int health = static_cast<int>(a_item.GetHealth());
						return Utils::IconMap["HEALTH"] + std::to_string(health);
					}
				}

				return "";
			case SortType::Magicka:
				if constexpr (std::is_same<DataType, NPCData>::value) {
					if (a_item.GetFormType() == RE::FormType::NPC) {
						int magicka = static_cast<int>(a_item.GetMagicka());
						return Utils::IconMap["MAGICKA"] + std::to_string(magicka);
					}
				}

				return "";
			case SortType::Stamina:
				if constexpr (std::is_same<DataType, NPCData>::value) {
					if (a_item.GetFormType() == RE::FormType::NPC) {
						int stamina = static_cast<int>(a_item.GetStamina());
						return Utils::IconMap["STAMINA"] + std::to_string(stamina);
					}
				}

				return "";
			case SortType::ReferenceID:
				if (a_item.GetFormType() == RE::FormType::NPC) {
					if (a_item.refID != 0) {
						return Utils::IconMap["REFID"] + std::format("{:08x}", a_item.refID);
					} else {
						return "";
					}
				}
			}
		}

		return "MISSING_SORT_PROPERTY";
	}

	template <typename DataType>
	bool TableView<DataType>::SortFnKit(const std::unique_ptr<Kit>& lhs, const std::unique_ptr<Kit>& rhs)
	{
		int delta = 0;
		switch (sortBy) {
		case SortType::Name:
			delta = lhs->name.compare(rhs->name);
			break;
		}

		if (delta > 0)
			return sortAscending ? false : true;
		if (delta < 0)
			return sortAscending ? true : false;

		return false;
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
				delta = (lhs->GetBaseForm() > rhs->GetBaseForm()) ? -1 : (lhs->GetBaseForm() < rhs->GetBaseForm()) ? 1 :
				                                                                                                     0;
			break;
		case SortType::ReferenceID:
			if constexpr (!std::is_base_of<BaseObject, DataType>::value) {
				break;
			} else {
				if (lhs->refID != 0 && rhs->refID != 0) {
					delta = (lhs->refID > rhs->refID) ? -1 : (lhs->refID < rhs->refID) ? 1 :
					                                                                     0;
				} else if (lhs->refID != 0) {
					delta = -1;
				} else if (rhs->refID != 0) {
					delta = 1;
				}
			}
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
				delta = lhs->GetClass().compare(rhs->GetClass());
				break;
			}
		case SortType::Gender:
			if constexpr (!std::is_same<DataType, NPCData>::value) {
				break;
			} else {
				delta = lhs->GetGender().compare(rhs->GetGender());
				break;
			}
		case SortType::Race:
			if constexpr (!std::is_same<DataType, NPCData>::value) {
				break;
			} else {
				delta = lhs->GetRace().compare(rhs->GetRace());
				break;
			}
		case SortType::Level:
			if constexpr (!std::is_same<DataType, NPCData>::value) {
				break;
			} else {
				auto lhsLevel = lhs->GetLevel();
				auto rhsLevel = rhs->GetLevel();
				delta = (lhsLevel > rhsLevel) ? -1 : (lhsLevel < rhsLevel) ? 1 :
				                                                             0;
				break;
			}
		case SortType::Health:
			if constexpr (!std::is_same<DataType, NPCData>::value)
				break;
			else
				delta = (lhs->GetHealth() > rhs->GetHealth()) ? -1 : (lhs->GetHealth() < rhs->GetHealth()) ? 1 :
				                                                                                             0;
			break;
		case SortType::Magicka:
			if constexpr (!std::is_same<DataType, NPCData>::value)
				break;
			else
				delta = (lhs->GetMagicka() > rhs->GetMagicka()) ? -1 : (lhs->GetMagicka() < rhs->GetMagicka()) ? 1 :
				                                                                                                 0;
			break;
		case SortType::Stamina:
			if constexpr (!std::is_same<DataType, NPCData>::value)
				break;
			else
				delta = (lhs->GetStamina() > rhs->GetStamina()) ? -1 : (lhs->GetStamina() < rhs->GetStamina()) ? 1 :
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
				delta = (lhs->GetValue() > rhs->GetValue()) ? -1 : (lhs->GetValue() < rhs->GetValue()) ? 1 :
				                                                                                         0;
			break;
		// case SortType::Space:
		// 	if constexpr (!std::is_same<DataType, CellTEST>::value)
		// 		break;
		// 	else
		// 		delta = lhs->space.compare(rhs->space);
		// 	break;
		// case SortType::Zone:
		// 	if constexpr (!std::is_same<DataType, CellTEST>::value)
		// 		break;
		// 	else
		// 		delta = lhs->zone.compare(rhs->zone);
		// 	break;
		// case SortType::Cell:
		// 	if constexpr (!std::is_same<DataType, CellTEST>::value)
		// 		break;
		// 	else
		// 		delta = lhs->cellName.compare(rhs->cellName);
		// 	break;
		case SortType::Damage:
			if constexpr (!std::is_same<DataType, ItemData>::value) {
				break;
			} else {
				if (lhs->GetFormType() == RE::FormType::Weapon && rhs->GetFormType() == RE::FormType::Weapon) {
					delta = CompareWeaponDamage(lhs->GetForm()->As<RE::TESObjectWEAP>(), rhs->GetForm()->As<RE::TESObjectWEAP>());
				} else if (lhs->GetFormType() == RE::FormType::Weapon) {
					delta = -1;
				} else if (rhs->GetFormType() == RE::FormType::Weapon) {
					delta = 1;
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
					delta = -1;
				} else if (rhs->GetFormType() == RE::FormType::Armor) {
					delta = 1;
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
					delta = -1;
				} else if (rhs->GetFormType() == RE::FormType::Armor) {
					delta = 1;
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
					delta = -1;
				} else if (rhs->GetFormType() == RE::FormType::Weapon) {
					delta = 1;
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
					delta = -1;
				} else if (rhs->GetFormType() == RE::FormType::Weapon) {
					delta = 1;
				}

				break;
			}
		case SortType::Weight:
			if constexpr (!std::is_same<DataType, ItemData>::value) {
				break;
			} else {
				delta = (lhs->GetWeight() > rhs->GetWeight()) ? -1 : (lhs->GetWeight() < rhs->GetWeight()) ? 1 :
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
					delta = -1;
				} else if (rhs->GetFormType() == RE::FormType::Weapon) {
					delta = 1;
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
					delta = -1;
				} else if (rhs->GetFormType() == RE::FormType::Weapon || rhs->GetFormType() == RE::FormType::Armor) {
					delta = 1;
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
		if (!this->showPluginKitView) {
			std::sort(tableList.begin(), tableList.end(), [this](const std::unique_ptr<DataType>& a, const std::unique_ptr<DataType>& b) {
				return this->SortFn(a, b);
			});
		} else {
			std::sort(kitList.begin(), kitList.end(), [this](const std::unique_ptr<Kit>& a, const std::unique_ptr<Kit>& b) {
				return this->SortFnKit(a, b);
			});
		}
	}

	template <typename DataType>
	void TableView<DataType>::UpdateImGuiTableIDs()
	{
		if (!this->showPluginKitView) {
			for (int i = 0; i < std::ssize(tableList); i++) {
				tableList[i]->TableID = i;
			}
		} else {
			for (int i = 0; i < std::ssize(kitList); i++) {
				kitList[i]->TableID = i;
			}
		}
	}

	template <typename DataType>
	void TableView<DataType>::UpdateKitItemData()
	{
		if (!this->selectedKit) {
			return;
		}

		const auto& collection = PersistentData::GetLoadedKits();
		if (const auto it = collection.find(*this->selectedKit); it != collection.end()) {
			const auto& kit = it->second;

			if (kit.items.empty()) {
				return;
			}

			for (const auto& kit_item : kit.items) {
				for (const auto& table_item : tableList) {
					if (table_item->GetEditorID() == kit_item->editorid) {
						table_item->kitAmount = kit_item->amount;
						table_item->kitEquipped = kit_item->equipped;
					}
				}
			}
		}
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

		float button_offset = 0.0f;
		const float center_x = ImGui::GetContentRegionAvail().x * 0.5f;

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

		if (this->HasFlag(ModexTableFlag_EnablePluginKitView)) {
			ImGui::SameLine();

			const std::string showKit = ICON_LC_BOX;
			bool pluginHasKits = !this->kitList.empty();

			if (pluginHasKits && !this->showPluginKitView) {
				float pulse = Utils::PulseMinMax((float)ImGui::GetTime(), 5.0f, 0.5f, 0.0f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, pulse));
			} else if (pluginHasKits && this->showPluginKitView) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			} else {
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
			}

			if (IconButton(showKit.c_str(), "", this->showPluginKitView)) {
				if (!pluginHasKits) {
					this->showPluginKitView = false;
				} else {
					this->SortListBySpecs();
					this->UpdateImGuiTableIDs();
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

			button_offset -= ImGui::GetFrameHeightWithSpacing() * 0.5f;
		}

		ImGui::SameLine();

		// If we ever add a UI messaging system, this would be a good candidate for it.
		const float prev_pos = ImGui::GetCursorPos().x;
		const float search_pos = center_x - (ImGui::CalcTextSize(this->lastSearchBuffer).x * 0.5f);
		ImGui::SetCursorPosX(search_pos);
		ImGui::Text("\"%s\"", this->lastSearchBuffer);
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Current search query being applied to results");
		}
		ImGui::SameLine();
		ImGui::SetCursorPosX(prev_pos);

		constexpr auto combo_flags = ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_HeightLarge;
		const auto preview_size = ImGui::CalcTextSize(SortTypeToString(this->sortBy).c_str()).x;

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));

		if (this->HasFlag(ModexTableFlag_Kit)) {
			button_offset -= ImGui::GetFrameHeightWithSpacing() * 1.5f;
		}

		// Surely a better way to do this, lol..
		// 2/18/2025 GOOD ENOUGH STOP TOUCHING IT FFS.

		if (this->HasFlag(ModexTableFlag_EnableEnchantmentSort)) {
			button_offset += ImGui::GetFrameHeightWithSpacing() * 0.5f;
		} else if (this->HasFlag(ModexTableFlag_EnableNonPlayableSort)) {
			button_offset += ImGui::GetFrameHeightWithSpacing() * 0.5f;
		}

		if (this->HasFlag(ModexTableFlag_EnableUniqueSort)) {
			button_offset += ImGui::GetFrameHeightWithSpacing() * 0.5f;
		}

		if (this->HasFlag(ModexTableFlag_EnableEssentialSort)) {
			button_offset += ImGui::GetFrameHeightWithSpacing() * 0.5f;
		}

		ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - preview_size - button_offset);
		ImGui::SetNextItemWidth(preview_size + 5.0f);
		if (ImGui::BeginCombo("##TableView::Sort::Combo", SortTypeToString(this->sortBy).c_str(), combo_flags)) {
			const auto& sort_list = this->showPluginKitView ? sortByListKit : sortByList;

			for (auto& sort : sort_list) {
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

		if (this->HasFlag(ModexTableFlag_EnableUniqueSort)) {
			ImGui::SameLine();

			const std::string tooltip = this->hideNonUnique ? "Show Non-Unique NPC's" : "Hide Non-Unique NPC's";
			const ImU32 icon_color = this->hideNonUnique ? ImGui::GetColorU32(ImGuiCol_Text) : ImGui::GetColorU32(ImGuiCol_TextDisabled);

			ImGui::PushStyleColor(ImGuiCol_Text, icon_color);
			if (IconButton(Utils::IconMap["UNIQUE"].c_str(), tooltip.c_str(), this->hideNonUnique)) {
				this->Refresh();
			}
			ImGui::PopStyleColor();
		}

		if (this->HasFlag(ModexTableFlag_EnableEssentialSort)) {
			ImGui::SameLine();

			const std::string tooltip = this->hideNonEssential ? "Show Non-Essential NPC's" : "Hide Non-Essential NPC's";
			const ImU32 icon_color = this->hideNonEssential ? ImGui::GetColorU32(ImGuiCol_Text) : ImGui::GetColorU32(ImGuiCol_TextDisabled);

			ImGui::PushStyleColor(ImGuiCol_Text, icon_color);
			if (IconButton(Utils::IconMap["ESSENTIAL"].c_str(), tooltip.c_str(), this->hideNonEssential)) {
				this->Refresh();
			}
			ImGui::PopStyleColor();
		}
	}

	// This is called when we select a plugin from the plugin list to alter the view. This updates the
	// KitList member to populate using a compare algorithm against all selected kits. So we also call it
	// when updating changes to our current kit.

	template <typename DataType>
	void TableView<DataType>::LoadKitsFromSelectedPlugin()
	{
		if (this->HasFlag(ModexTableFlag_EnablePluginKitView)) {
			kitList.clear();

			// // We don't need to correlate kits to All Plugins since it's not a valid plugin.
			// if (selectedPlugin == _T("SHOW_ALL_PLUGINS")) {
			// 	return;
			// }

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
					// We first need to check if any item in any given kit is from the selected plugin.
					// We iterate over the items inside every kit to identify this. So far at 500+ kits this is not an issue.
					// Once we detected that this kit *should* show inside the Plugin Kit View, we then create
					// the meta data for the kit by iterating over its items again. The key point is that we
					// early out of the first iteration on the first item found. So we don't actually iterate
					// over the entire kit twice. We use one to match, then the other to build.

					for (auto& item : kit.items) {
						if (selectedPlugin == _T("SHOW_ALL_PLUGINS") || item->plugin == selectedPlugin) {
							kit.TableID = table_id;

							kit.weaponCount = 0;
							kit.armorCount = 0;
							kit.miscCount = 0;

							for (auto& found : kit.items) {
								const auto& form = RE::TESForm::LookupByEditorID(found->editorid);

								if (form) {
									if (form->IsWeapon()) {
										kit.weaponCount++;
									} else if (form->IsArmor()) {
										kit.armorCount++;
									} else {
										kit.miscCount++;
									}
								}
							}

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
		(void)a_selected;  // If we want to handle selection visuals manually.

		const auto& style = Settings::GetSingleton()->GetStyle();
		const auto& config = Settings::GetSingleton()->GetConfig();
		const float fontSize = config.globalFontSize;

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

		const float spacing = LayoutItemSize.x / 3.0f;
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
		// const std::string name_string = a_kit.name;
		const std::string name_string = FormatTextWidth(a_kit.name, spacing);
		if (!this->compactView) {
			DrawList->AddText(top_left_align, text_color, name_string.c_str());
		} else {
			DrawList->AddText(center_left_align, text_color, name_string.c_str());
		}

		const std::string weaponCount = a_kit.weaponCount == 0 ? _T("None") : std::to_string(a_kit.weaponCount);
		const std::string armorCount = a_kit.armorCount == 0 ? _T("None") : std::to_string(a_kit.armorCount);
		const std::string miscCount = a_kit.miscCount == 0 ? _T("None") : std::to_string(a_kit.miscCount);
		const std::string totalCount = std::to_string(a_kit.weaponCount + a_kit.armorCount + a_kit.miscCount);

		// Draw the kit meta data
		if (!this->compactView) {
			const ImVec2 weapon_count_pos = ImVec2(left_align + spacing, top_align);
			const ImVec2 armor_count_pos = ImVec2(left_align + spacing, center_align);
			const ImVec2 misc_count_pos = ImVec2(left_align + spacing, bot_align);
			const std::string weapon_count_string = ICON_LC_SWORDS + weaponCount;
			const std::string armor_count_string = ICON_LC_SHIELD + armorCount;
			const std::string misc_count_string = ICON_LC_PUZZLE + miscCount;

			DrawList->AddText(weapon_count_pos, a_kit.weaponCount == 0 ? ImGui::GetColorU32(ImGuiCol_TextDisabled) : text_color, weapon_count_string.c_str());
			DrawList->AddText(armor_count_pos, a_kit.armorCount == 0 ? ImGui::GetColorU32(ImGuiCol_TextDisabled) : text_color, armor_count_string.c_str());
			DrawList->AddText(misc_count_pos, a_kit.miscCount == 0 ? ImGui::GetColorU32(ImGuiCol_TextDisabled) : text_color, misc_count_string.c_str());
		} else {
			const ImVec2 total_count_pos = ImVec2(left_align + spacing, center_align);
			const std::string total_count_string = ICON_LC_BOX + totalCount;
			DrawList->AddText(total_count_pos, text_color, total_count_string.c_str());
		}

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
	void TableView<DataType>::PluginKitView()
	{
		if (!this->HasFlag(ModexTableFlag_EnablePluginKitView)) {
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
			// ImGuiMultiSelectIO* ms_io = ImGui::BeginMultiSelect(ms_flags, kitSelectionStorage.Size, ITEMS_COUNT);
			// kitSelectionStorage.UserData = (void*)&kitList;
			// kitSelectionStorage.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage* self, int idx) {
			// 	KitList* a_items = (KitList*)self->UserData;
			// 	return (*a_items)[idx]->TableID;  // Index -> TableID
			// };
			// kitSelectionStorage.ApplyRequests(ms_io);

			// Start clipper and iterate through table's item list.
			clipper.Begin(ITEMS_COUNT, LayoutItemStep.y);

			// if (ms_io->RangeSrcItem != -1) {
			// 	clipper.IncludeItemByIndex((int)ms_io->RangeSrcItem);  // Ensure RangeSrc item is not clipped.
			// }

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
						// ImGui::SetNextItemSelectionUserData(kit_idx);
						// bool is_item_selected = kitSelectionStorage.Contains(item_data->TableID);
						bool is_item_visible = ImGui::IsRectVisible(LayoutItemSize);
						const float button_width = std::max(LayoutItemSize.x / 7.0f, 100.0f);

						// If we implement a grid or gapped layout, this will be needed.
						// For now, it just removes the spacing so the selection is within the bounds of the item.
						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(LayoutSelectableSpacing, LayoutSelectableSpacing));
						bool is_selected = false;
						ImGui::GradientSelectableEX("", is_selected, ImVec2(LayoutItemSize.x - (button_width), LayoutItemSize.y));
						ImGui::PopStyleVar();

						// if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
						// 	if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
						// 		// Anything?
						// 	}
						// }

						if (is_item_visible) {
							DrawKit(*item_data, pos, false);

							ImVec2 use_size;

							if (this->compactView) {
								use_size = ImVec2(button_width, LayoutItemSize.y);
							} else {
								use_size = ImVec2(button_width, LayoutItemSize.y / 2.0f);
							}

							const ImVec2 use_pos = ImVec2(pos.x + (LayoutItemSize.x - use_size.x), pos.y);
							const ImVec2 delete_pos = ImVec2(use_pos.x, use_pos.y + use_size.y);

							ImGui::SetCursorScreenPos(use_pos);

							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
							ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

							ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_Button, 0.50f));
							if (ImGui::GradientButton(ICON_LC_CHECK "Use", use_size)) {
								AddKitToInventory(item_data);
							}
							ImGui::PopStyleColor();

							if (!this->compactView) {
								ImGui::SetCursorScreenPos(delete_pos);
								// ImGui::SetCursorPosY(ImGui::GetCursorPosY() + use_size.y);
								ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.3f, 0.3f, 0.50f));
								if (ImGui::GradientButton(ICON_LC_TRASH "Delete", use_size)) {
									confirmDeleteKit = std::make_unique<Kit>(*item_data);
									ImGui::OpenPopup("##KitPluginView::Delete::Confirmation");
								}
								ImGui::PopStyleColor();
							}

							ImGui::PopStyleVar(2);
						}

						constexpr auto popup_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

						// Have to create and destroy this popup within the context of the table Pop and Push ID because
						// otherwise the Popup ID won't register.

						if (confirmDeleteKit) {
							if (ImGui::BeginPopupModal("##KitPluginView::Delete::Confirmation", nullptr, popup_flags)) {
								ImGui::SubCategoryHeader(_T("KIT_DELETE_CONFIRMATION"));
								ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

								ImGui::NewLine();

								float center_text = ImGui::GetCenterTextPosX(confirmDeleteKit->name.c_str());
								ImGui::SetCursorPosX(center_text);
								ImGui::Text(confirmDeleteKit->name.c_str());

								ImGui::NewLine();

								if (ImGui::IsKeyPressed(ImGuiKey_Y, false)) {
									PersistentData::GetSingleton()->DeleteKit(confirmDeleteKit->name);
									// selectedKit = _T("None");
									this->LoadKitsFromSelectedPlugin();
									this->SortListBySpecs();
									this->UpdateImGuiTableIDs();

									confirmDeleteKit = nullptr;

									ImGui::CloseCurrentPopup();
								}

								if (ImGui::IsKeyPressed(ImGuiKey_N, false)) {
									ImGui::CloseCurrentPopup();
								}

								if (ImGui::Button("(Y)es", ImVec2(250.0f, 0))) {
									PersistentData::GetSingleton()->DeleteKit(confirmDeleteKit->name);
									this->LoadKitsFromSelectedPlugin();
									this->SortListBySpecs();
									this->UpdateImGuiTableIDs();

									confirmDeleteKit = nullptr;

									ImGui::CloseCurrentPopup();
								}

								ImGui::SameLine();

								if (ImGui::Button("(N)o", ImVec2(250.0f, 0))) {
									ImGui::CloseCurrentPopup();
								}

								ImGui::EndPopup();
							}
						}

						ImGui::PopID();
					}
				}
			}

			clipper.End();

			// ms_io = ImGui::EndMultiSelect();
			// kitSelectionStorage.ApplyRequests(ms_io);
		}
	}

	template <typename DataType>
	void TableView<DataType>::DrawItem(const DataType& a_item, const ImVec2& a_pos, const bool& a_selected)
	{
		(void)a_selected;  // If we want to handle selection visuals manually.

		const auto& style = Settings::GetSingleton()->GetStyle();
		const auto& config = Settings::GetSingleton()->GetConfig();
		const float fontSize = config.globalFontSize;

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
			Utils::GetFormTypeColor(a_item.GetFormType()));

		// We need to adjust the bounding box to account for the type pillar.
		bb.Min.x += type_pillar_width * 2.0f;

		const float spacing = LayoutItemSize.x / 6.0f;
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
			const std::string type_icon = Utils::GetFormTypeIcon(a_item.GetFormType());
			const std::string type_formid = type_icon + "[" + a_item.GetTypeName() + "] " + a_item.GetFormID();
			DrawList->AddText(top_left_align, text_color, type_formid.c_str());
		}

		std::string name_string = "";
		float name_offset = 5.0f;  // initial offset

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

		// Only apply below drawing to main table, not kit table.
		if (!this->HasFlag(ModexTableFlag_Kit)) {
			// Draw NPCData specific stuff
			if (const NPCData* npcData = dynamic_cast<const NPCData*>(&a_item)) {
				if (npcData->GetFormType() == RE::FormType::NPC) {
					const auto& npc = npcData->GetForm()->As<RE::TESNPC>();

					if (npc != nullptr) {
						const ImU32 unique_color = IM_COL32(255, 179, 102, 20);
						const ImU32 essential_color = IM_COL32(204, 153, 255, 20);

						const ImVec2 unique_pos = ImVec2(bb.Min.x + (spacing * 1.5f) + spacing * 1.5f, this->compactView ? center_align : top_align);
						const ImVec2 essential_pos = ImVec2(unique_pos.x + fontSize + 2.0f, unique_pos.y);

						const std::string unique_string = Utils::IconMap["UNIQUE"];
						const std::string essential_string = Utils::IconMap["ESSENTIAL"];

						if (npc->IsUnique() && !npc->IsEssential()) {
							DrawList->AddText(unique_pos, text_color, unique_string.c_str());
							DrawList->AddRectFilledMultiColor(
								ImVec2(bb.Min.x, bb.Min.y),
								ImVec2(bb.Max.x, bb.Max.y),
								unique_color,
								unique_color,
								IM_COL32(0, 0, 0, 0),
								IM_COL32(0, 0, 0, 0));
						} else if (npc->IsEssential() && !npc->IsUnique()) {
							DrawList->AddText(unique_pos, text_color, essential_string.c_str());
							DrawList->AddRectFilledMultiColor(
								ImVec2(bb.Min.x, bb.Min.y),
								ImVec2(bb.Max.x, bb.Max.y),
								essential_color,
								essential_color,
								IM_COL32(0, 0, 0, 0),
								IM_COL32(0, 0, 0, 0));
						} else if (npc->IsEssential() && npc->IsUnique()) {
							DrawList->AddText(unique_pos, text_color, unique_string.c_str());
							DrawList->AddText(essential_pos, text_color, essential_string.c_str());
							DrawList->AddRectFilledMultiColor(
								ImVec2(bb.Min.x, bb.Min.y),
								ImVec2(bb.Max.x, bb.Max.y),
								unique_color,
								unique_color,
								essential_color,
								essential_color);
						}

						if (ImGui::IsMouseHoveringRect(unique_pos, ImVec2(unique_pos.x + fontSize, unique_pos.y + fontSize))) {
							if (npc->IsUnique()) {
								ImGui::SetTooltip(_T("TOOLTIP_UNIQUE"));
							} else if (npc->IsEssential()) {
								ImGui::SetTooltip(_T("TOOLTIP_ESSENTIAL"));
							}
						}

						if (ImGui::IsMouseHoveringRect(essential_pos, ImVec2(essential_pos.x + fontSize, essential_pos.y + fontSize))) {
							if (npc->IsEssential()) {
								ImGui::SetTooltip(_T("TOOLTIP_ESSENTIAL"));
							}
						}
					}

					if (npcData->refID != 0) {
						name_offset += ImGui::CalcTextSize(Utils::IconMap["REFID"].c_str()).x;
						name_string = FormatTextWidth(Utils::IconMap["REFID"] + name_string, (spacing * 1.5f) + name_offset);
					}
				}
			}

			// Draw ItemData specific stuff
			if (const ItemData* itemData = dynamic_cast<const ItemData*>(&a_item)) {
				{  // Draw Value (Top-Right Align)
					const std::string string = itemData->GetValueString() + " " + ICON_LC_COINS;
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
						const std::string string = std::to_string(weight) + " " + ICON_LC_WEIGHT;
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
						// const std::string slot_string = ICON_LC_BETWEEN_HORIZONTAL_START + equipSlots[0];

						if (!compactView) {
							const ImVec2 armorRating_pos = ImVec2(bb.Min.x + (spacing * 1.5f) + spacing * 1.5f, top_align);
							DrawList->AddText(armorRating_pos, text_color, rating_string.c_str());

							const ImVec2 armorType_pos = ImVec2(bb.Min.x + (spacing * 1.5f) + spacing * 1.5f, bot_align);
							DrawList->AddText(armorType_pos, text_color, type_string.c_str());
						} else {
							const ImVec2 armorRating_pos = ImVec2(bb.Min.x + (spacing * 1.5f) + spacing * 1.5f, center_align);
							DrawList->AddText(armorRating_pos, text_color, rating_string.c_str());
						}

						if (armor->formEnchanting != nullptr) {
							name_offset = ImGui::CalcTextSize(Utils::IconMap["ENCHANTED"].c_str()).x;
							name_string = FormatTextWidth(Utils::IconMap["ENCHANTED"] + name_string, (spacing * 1.5f) + name_offset);
							const ImU32 enchantment_color = IM_COL32(0, 255, 255, 20);

							// Top to Bottom Gradient
							DrawList->AddRectFilledMultiColor(
								ImVec2(bb.Min.x, bb.Min.y),
								ImVec2(bb.Max.x, bb.Min.y + ((bb.Max.y / 8.0f))),
								enchantment_color,
								enchantment_color,
								IM_COL32(0, 0, 0, 0),
								IM_COL32(0, 0, 0, 0));
						}
					}
				}

				if (itemData->GetFormType() == RE::FormType::Book) {
					const auto& book = itemData->GetForm()->As<RE::TESObjectBOOK>();

					if (book != nullptr) {
						const auto teaches_skill = book->TeachesSkill();
						const auto teaches_spell = book->TeachesSpell();

						ImVec2 teach_pos;

						if (!compactView) {
							teach_pos = ImVec2(bb.Min.x + (spacing * 1.5f) + spacing * 1.5f, top_align);
						} else {
							teach_pos = ImVec2(bb.Min.x + (spacing * 1.5f) + spacing * 1.5f, center_align);
						}

						if (teaches_skill) {
							const std::string skill_string = ICON_LC_BRAIN + Utils::SkillMap[book->GetSkill()];
							DrawList->AddText(teach_pos, text_color, skill_string.c_str());
						} else if (teaches_spell) {  // Issue #54 : Spell name on tome is causing crash, maybe due to not validating string?
							const auto spell = book->GetSpell();
							if (spell) {
								const auto string = spell->GetFullName();

								if (string) {
									const std::string spell_string = ICON_LC_WAND + std::string(string);
									DrawList->AddText(teach_pos, text_color, spell_string.c_str());
								}
							}
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
							const ImVec2 damage_pos = ImVec2(bb.Min.x + (spacing * 1.5f) + spacing * 1.5f, top_align);
							const ImVec2 skill_pos = ImVec2(bb.Min.x + (spacing * 1.5f) + spacing * 1.5f, bot_align);
							// const ImVec2 type_pos = ImVec2(bb.Min.x + spacing + spacing * 1.5f, bot_align);

							DrawList->AddText(damage_pos, text_color, damage_string.c_str());
							DrawList->AddText(skill_pos, text_color, skill_string.c_str());
							// DrawList->AddText(type_pos, text_color, type_string.c_str());
						} else {
							const ImVec2 damage_pos = ImVec2(bb.Min.x + (spacing * 1.5f) + spacing * 1.5f, center_align);
							// const ImVec2 type_pos = ImVec2(bb.Min.x + spacing + spacing * 2.5f, center_align);
							DrawList->AddText(damage_pos, text_color, damage_string.c_str());
							// DrawList->AddText(type_pos, text_color, type_string.c_str());
						}
					}
				}
			}
		}

		if (compactView) {
			const std::string plugin_name = Utils::GetFormTypeIcon(a_item.GetFormType()) + FormatTextWidth(a_item.GetPluginName(), ((spacing * 1.5f) - fontSize * 2.0f) - name_offset);
			DrawList->AddText(center_left_align, text_color, plugin_name.c_str());
		} else {
			const std::string plugin_name = a_item.GetPluginName();
			DrawList->AddText(bot_left_align, text_color, plugin_name.c_str());
		}

		const std::string sort_text = GetSortProperty(a_item);

		if (compactView) {
			const ImVec2 name_pos = ImVec2(bb.Min.x + (spacing * 1.5f) - name_offset, center_align);
			DrawList->AddText(name_pos, text_color, name_string.c_str());

			const ImVec2 sort_pos = ImVec2(bb.Min.x + (spacing * 1.5f) + spacing * 2.5f, center_align);
			DrawList->AddText(sort_pos, text_color, sort_text.c_str());
		} else {
			const ImVec2 name_pos = ImVec2(bb.Min.x + (spacing * 1.5f) - name_offset, top_align);
			DrawList->AddText(name_pos, text_color, name_string.c_str());

			const ImVec2 sort_pos = ImVec2(bb.Min.x + (spacing * 1.5f) + spacing * 2.5f, bot_align);
			DrawList->AddText(sort_pos, text_color, sort_text.c_str());
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
		if (!this->generalSearchDirty) {
			if (strcmp(this->generalSearchBuffer, this->lastSearchBuffer) != 0) {
				this->generalSearchDirty = true;
			}
		}

		// Should this be routed through the input manager?
		if (!ImGui::GetIO().WantTextInput) {
			if (ImGui::IsKeyPressed(ImGuiKey_A, false) && ImGui::GetIO().KeyCtrl) {
				for (auto& item : tableList) {
					if (item) {
						selectionStorage.SetItemSelected(item->TableID, true);
					}
				}
			}
		}

		if (ImGui::BeginChild("##TableView::Draw", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoMove)) {
			if (this->showPluginKitView) {
				PluginKitView();
				ImGui::EndChild();
				return;
			}

			if (std::ssize(this->pluginList) <= 1) {
				ImGui::TextColored(ImVec4(1.0f, 0.1f, 0.1f, 1.0f), "Error: Moduled failed to load. Try clicking the Sidebar Button for this module!");
				ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "If you don't see a button for this module in the sidebar, it's because it's disabled! You have to enable it in settings!");
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
						if (item_idx >= tableList.size()) {
							continue;
						}

						if (!tableList[item_idx]) {
							continue;
						}

						auto& item_data = tableList.at(item_idx);
						ImGui::PushID((int)item_data->TableID);

						// Double-click add amount behavior.
						const int click_amount = HasFlag(ModexTableFlag_Kit) ? item_data->kitAmount : clickAmount ? *clickAmount :
						                                                                                            1;

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

							if (this->HasFlag(ModexTableFlag_Kit)) {
								const auto& g = *GImGui;
								if (g.MouseStationaryTimer > 0.6f) {
									ImGui::SetNextWindowSize(ImVec2(300.0f, 0));
									ImGui::SetNextWindowPos(ImVec2(ImGui::GetMousePos().x - 300.0f, ImGui::GetMousePos().y));
									if (ImGui::BeginTooltip()) {
										if constexpr (std::is_same_v<DataType, ItemData>) {
											ShowItemPreview<DataType>(item_data);
										}
										ImGui::EndTooltip();
									}
								}
							} else {
								if (auto it = this->dragDropSourceList.find("FROM_KIT"); it != this->dragDropSourceList.end()) {
									const auto& g = *GImGui;
									if (g.MouseStationaryTimer > 0.6f) {
										ImGui::SetNextWindowSize(ImVec2(300.0f, 0));
										ImGui::SetNextWindowPos(ImVec2(ImGui::GetMousePos().x - 300.0f, ImGui::GetMousePos().y));
										if (ImGui::BeginTooltip()) {
											if constexpr (std::is_same_v<DataType, ItemData>) {
												ShowItemPreview<DataType>(item_data);
											}
											ImGui::EndTooltip();
										}
									}
								}
							}
						}

						if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
							ImGui::OpenPopup("TableViewContextMenu");
						}

						if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
							if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
								if constexpr (std::is_same_v<DataType, ItemData>) {
									if (this->clickToAdd != nullptr) {
										if (*this->clickToAdd == true) {
											Console::AddItem(item_data->GetFormID().c_str(), click_amount);
											Console::StartProcessThread();
										} else {
											Console::PlaceAtMe(item_data->GetFormID().c_str(), click_amount);
											Console::StartProcessThread();
										}
									}
								}

								if constexpr (std::is_same_v<DataType, NPCData>) {
									Console::PlaceAtMe(item_data->GetFormID().c_str(), click_amount);
									Console::StartProcessThread();
								}

								if constexpr (std::is_same_v<DataType, ObjectData>) {
									Console::PlaceAtMe(item_data->GetFormID().c_str(), click_amount);
									Console::StartProcessThread();
								}
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
								if (!is_item_selected) {
									payload_items.push_back(item_data->TableID);
								} else {
									if (id < tableList.size() && id >= 0) {
										while (selectionStorage.GetNextSelectedItem(&it, &id)) {
											payload_items.push_back(id);
										}
									}
								}
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
						if (is_item_visible && item_data != nullptr) {
							DrawItem(*item_data, pos, is_item_selected);

							if (this->HasFlag(ModexTableFlag_Kit)) {
								ImVec2 equippable_pos;

								if (compactView) {
									equippable_pos = ImVec2(pos.x + (LayoutItemSize.x / 1.75f), pos.y);
								} else {
									equippable_pos = ImVec2(pos.x + (LayoutItemSize.x / 1.75f), pos.y);
								}

								ImGui::SetCursorScreenPos(equippable_pos);

								const auto alpha = item_data->kitEquipped ? 1.0f : 0.25f;
								const auto text = item_data->kitEquipped ? _T("Equip") : _T("Equip");
								const auto icon = item_data->kitEquipped ? ICON_LC_CHECK : ICON_LC_X;
								const auto equip_size = ImVec2(LayoutItemSize.x / 7.0f, LayoutItemSize.y);
								const auto color = ImGui::GetColorU32(ImGuiCol_Button, alpha);
								const auto padding = 5.0f;

								ImGui::PushStyleColor(ImGuiCol_Button, color);
								ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
								ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

								// TODO: Implement a check for readOnly kits. A problem though, is that if I do it
								// here directly, I will be iterating over the entire kit list and pulling out the
								// selected kit reference everytime which is kind of costly. Need a more uniform
								// way to track the kit state or reference.

								if (item_data->GetForm()->Is(RE::FormType::Armor) || item_data->GetForm()->Is(RE::FormType::Weapon)) {
									if (ImGui::GradientButton((std::string(icon) + text).c_str(), equip_size)) {
										item_data->kitEquipped = !item_data->kitEquipped;
										this->SyncChangesToKit();
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
								if (ImGui::InputInt("##EquipCount", &item_data->kitAmount, 0, 0)) {
									this->SyncChangesToKit();
								}

								if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary)) {
									ImGui::SetTooltip(_T("TOOLTIP_AMOUNT_ADD"));
								}
							}
						}

						if (ImGui::BeginPopup("TableViewContextMenu")) {
							if (const ItemData* itemData = dynamic_cast<const ItemData*>(item_data.get())) {
								if (ImGui::MenuItem(_T("AIM_ADD"))) {
									if (selectionStorage.Size == 0) {
										Console::AddItem(item_data->GetFormID().c_str(), click_amount);
										Console::StartProcessThread();
									} else {
										if (item_data == itemPreview && !is_item_selected) {
											Console::AddItem(item_data->GetFormID().c_str(), click_amount);
											Console::StartProcessThread();
										} else {
											this->AddSelectionToInventory(click_amount);
											this->selectionStorage.Clear();
										}
									}
								}

								if (itemData->GetFormType() == RE::FormType::Armor || itemData->GetFormType() == RE::FormType::Weapon) {
									if (ImGui::MenuItem(_T("AIM_EQUIP"))) {
										if (selectionStorage.Size == 0) {
											Console::AddItemEx(item_data->GetBaseForm(), click_amount, true);
											Console::StartProcessThread();
										} else {
											if (item_data == itemPreview && !is_item_selected) {
												Console::AddItemEx(item_data->GetBaseForm(), click_amount, true);
												Console::StartProcessThread();
											} else {
												void* it = NULL;
												ImGuiID id = 0;

												// TODO: Create a helper func for AddItemEx in TableView context?
												while (selectionStorage.GetNextSelectedItem(&it, &id)) {
													if (id < tableList.size() && id >= 0) {
														const auto& item = tableList[id];
														Console::AddItemEx(item->GetBaseForm(), click_amount, true);
													}
												}

												Console::StartProcessThread();
												this->selectionStorage.Clear();
											}
										}

										Console::StartProcessThread();
									}
								}

								if (ImGui::MenuItem(_T("AIM_PLACE"))) {
									if (selectionStorage.Size == 0) {
										Console::PlaceAtMe(item_data->GetFormID().c_str(), click_amount);
										Console::StartProcessThread();
									} else {
										if (item_data == itemPreview && !is_item_selected) {
											Console::PlaceAtMe(item_data->GetFormID().c_str(), click_amount);
											Console::StartProcessThread();
										} else {
											this->PlaceSelectionOnGround(click_amount);
											this->selectionStorage.Clear();
										}
									}
								}

								if (itemData->GetFormType() == RE::FormType::Book) {
									ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

									if (ImGui::MenuItem(_T("GENERAL_READ_ME"))) {
										Console::ReadBook(itemData->GetFormID());
										Console::StartProcessThread();
										Menu::GetSingleton()->Close();
									}
								}

								if (this->selectedKit) {
									const std::string selected_kit = *this->selectedKit;

									if (!selected_kit.empty() && selected_kit != _T("None")) {
										ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
										if (ImGui::MenuItem(_T("KIT_REMOVE"))) {
											if (selectionStorage.Size == 0) {
												this->RemovePayloadItemFromKit(item_data);
											} else {
												if (item_data == itemPreview && !is_item_selected) {
													this->RemovePayloadItemFromKit(item_data);
												} else {
													this->RemoveSelectedFromKit();
												}
											}

											this->SyncChangesToKit();

											if (auto it = this->dragDropSourceList.find("FROM_KIT"); it != this->dragDropSourceList.end()) {
												const auto dragDropSourceTable = it->second;

												dragDropSourceTable->LoadKitsFromSelectedPlugin();
											}

											this->Refresh();
											this->selectionStorage.Clear();
										}
									}
								}

								if (auto it = this->dragDropSourceList.find("FROM_KIT"); it != this->dragDropSourceList.end()) {
									const auto dragDropSourceTable = it->second;

									if (dragDropSourceTable->selectedKit) {
										const std::string selected_kit = *dragDropSourceTable->selectedKit;

										if (!selected_kit.empty() && selected_kit != _T("None")) {
											ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
											if (ImGui::MenuItem(_T("KIT_ADD"))) {
												if (selectionStorage.Size == 0) {
													dragDropSourceTable->AddPayloadItemToKit(item_data);
												} else {
													if (item_data == itemPreview && !is_item_selected) {
														dragDropSourceTable->AddPayloadItemToKit(item_data);
													} else {
														this->AddSelectedToKit();
													}
												}

												dragDropSourceTable->SyncChangesToKit();
												this->LoadKitsFromSelectedPlugin();

												dragDropSourceTable->Refresh();
												this->selectionStorage.Clear();
											}
										}
									}
								}
							}

							// TODO: Should we close menu?
							if (const NPCData* npc_data = dynamic_cast<const NPCData*>(item_data.get())) {
								if (ImGui::MenuItem(_T("AIM_PLACE"))) {
									if (selectionStorage.Size == 0) {
										Console::PlaceAtMe(npc_data->GetFormID().c_str(), click_amount);
										Console::StartProcessThread();
									} else {
										if (item_data == itemPreview && !is_item_selected) {
											Console::PlaceAtMe(npc_data->GetFormID().c_str(), click_amount);
											Console::StartProcessThread();
										} else {
											void* it = NULL;
											ImGuiID id = 0;

											while (selectionStorage.GetNextSelectedItem(&it, &id)) {
												if (id < tableList.size() && id >= 0) {
													const auto& item = tableList[id];
													Console::PlaceAtMe(item->GetFormID().c_str(), click_amount);
												}
											}

											Console::StartProcessThread();
											this->selectionStorage.Clear();
										}
									}
								}

								// TODO: What if we have a selection that contains some npcs with references
								// and some without? This relies on the user hovering over a valid ref at the
								// time of the context window. It will work regardless of the above case.

								if (npc_data->refID != 0) {
									if (ImGui::MenuItem(_T("NPC_BRING_REFERENCE"))) {
										if (selectionStorage.Size == 0) {
											Console::BringNPC(npc_data->refID, true);
										} else {
											if (item_data == itemPreview && !is_item_selected) {
												Console::BringNPC(npc_data->refID, true);
											} else {
												void* it = NULL;
												ImGuiID id = 0;

												while (selectionStorage.GetNextSelectedItem(&it, &id)) {
													if (id < tableList.size() && id >= 0) {
														const auto& item = tableList[id];
														Console::BringNPC(item->refID, true);
													}
												}

												this->selectionStorage.Clear();
											}
										}
									}
								}

								if (npc_data->refID != 0) {
									if (ImGui::MenuItem(_T("NPC_GOTO_REFERENCE"))) {
										if (selectionStorage.Size == 0) {
											Console::GotoNPC(npc_data->refID, true);
										} else {
											if (item_data == itemPreview && !is_item_selected) {
												Console::GotoNPC(npc_data->refID, true);
											} else {
												void* it = NULL;
												ImGuiID id = 0;

												while (selectionStorage.GetNextSelectedItem(&it, &id)) {
													if (id < tableList.size() && id >= 0) {
														const auto& item = tableList[id];
														Console::GotoNPC(item->refID, true);

														break;  // Because how else do we handle multiple cases?
													}
												}

												this->selectionStorage.Clear();
											}
										}
									}
								}
							}

							ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

							if (ImGui::BeginMenu(_T("Copy"))) {
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

								if (item_data->GetFormType() == RE::FormType::NPC) {
									if (ImGui::MenuItem("Copy Reference ID")) {
										ImGui::SetClipboardText(std::format("{:08x}", item_data->refID).c_str());
									}
								}

								// TODO: Implement additional options for item types and NPC data.

								ImGui::EndMenu();
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
							dragDropSourceTable->RemovePayloadItemFromKit(payload_item);
						}

						dragDropSourceTable->SyncChangesToKit();
						this->LoadKitsFromSelectedPlugin();
						dragDropSourceTable->Refresh();

						if (this->kitList.empty()) {
							this->showPluginKitView = false;
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
								dragDropSourceTable->LoadKitsFromSelectedPlugin();
								this->Refresh();

								// In-table kit view is disabled if there are no kits in the plugin.
								if (dragDropSourceTable->kitList.empty()) {
									dragDropSourceTable->showPluginKitView = false;
								}
							}

							if (payload->IsPreview()) {
								this->DrawDragDropPayload(DragBehavior_Add);
							}
						}
					} else {
						this->DrawDragDropPayload(DragBehavior_Invalid);
					}
				}

				ImGui::EndDragDropTarget();
			}

			selectionStorage.Clear();
			// kitSelectionStorage.Clear();
		}
	}
}