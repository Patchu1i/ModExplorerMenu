#include "include/D/Data.h"
#include "include/D/DataTypes.h"
#include "include/P/Persistent.h"
#include "include/S/Settings.h"
#include "include/U/Util.h"
#include "include/W/Worldspace.h"

namespace Modex
{
	// https://github.com/Nightfallstorm/DescriptionFramework | License GPL-3.0
	using _GetFormEditorID = const char* (*)(std::uint32_t);

	std::string po3_GetEditorID(RE::FormID a_formID)
	{
		static auto tweaks = GetModuleHandleA("po3_Tweaks");
		static auto function = reinterpret_cast<_GetFormEditorID>(GetProcAddress(tweaks, "GetFormEditorID"));
		if (function) {
			return function(a_formID);
		}
		return {};
	}

	// Attempts to "Validate" the fileName member of a RE:TESFile* pointer.
	// Returns "`MODEX_ERR`" if the pointer is null or the fileName member is null.
	std::string ValidateTESFileName(const RE::TESFile* a_file)
	{
		if (a_file == nullptr) {
			return "MODEX_ERR";
		}

		if (a_file->fileName == nullptr) {
			return "MODEX_ERR";
		}

		if (a_file->GetFilename().data() == nullptr) {
			return "MODEX_ERR";
		}

		// std::string_view to const char* to std::string. utf-8 entry?
		return std::string(a_file->fileName);
	}

	// Attempts to "Validate" the name member of a RE:TESForm or RE::TESName pointer.
	// Returns "`MODEX_ERR`" if the pointer is null or the name member is null.
	template <class TESObject>
	const char* ValidateTESName(const TESObject* a_object)
	{
		if (a_object == nullptr) {
			return "MODEX_ERR";
		}

		if (a_object->GetName() == nullptr) {
			return "MODEX_ERR";
		}

		// Ensure the name is a valid, null-terminated C-string
		return a_object->GetName();
	}

	template <class T>
	void Data::CacheNPCs(RE::TESDataHandler* a_data)
	{
		for (RE::TESForm* form : a_data->GetFormArray<T>()) {
			const RE::TESFile* mod = form->GetFile(0);

			// RE::TESNPC* npc = form->As<RE::TESNPC>();

			if (RE::TESNPC* npc = form->As<RE::TESNPC>(); npc->IsPlayerRef()) {
				continue;
			}

			_npcCache.push_back(NPCData{ form });

			if (!_npcModList.contains(mod)) {
				_npcModList.insert(mod);
				_modList.insert(mod);
			}

			if (_npcModList.contains(mod)) {
				auto it = _itemListModFormTypeMap.find(mod);
				if (it == _itemListModFormTypeMap.end()) {
					_itemListModFormTypeMap[mod] = ModFileItemFlags();
				}

				_itemListModFormTypeMap[mod].npc = true;
			}
		}
	}

	// Called Internally from Data::CacheNPCRefIds.
	// Merges newfound NPC references with the master list.
	void Data::MergeNPCRefIds(std::shared_ptr<std::unordered_map<RE::FormID, RE::FormID>> npc_ref_map)
	{
		if (npc_ref_map->empty()) {
			logger::warn("No NPC references found.");
		} else {
			for (auto& npc : _npcCache) {
				auto it = npc_ref_map->find(npc.GetBaseForm());
				if (it != npc_ref_map->end()) {
					npc.refID = it->second;
				}
			}
		}
	}

	// Best I can currently do to cache NPC references for the time being.
	// Some unique NPCs are not captured until their cell (or world) is loaded.
	// Hroki in Markarth Silverblood-inn is an example of this.
	void Data::CacheNPCRefIds()
	{
		// This is shared so that it's lifetime persists until the SKSE task is complete.
		// Passing solely by reference does not seem to work, causes a CTD.
		auto npc_ref_map = std::make_shared<std::unordered_map<RE::FormID, RE::FormID>>();

		SKSE::GetTaskInterface()->AddTask([npc_ref_map]() {
			auto process = RE::ProcessLists::GetSingleton();

			// Captures most NPCs in the game.
			for (auto& handle : process->lowActorHandles) {
				if (!handle.get() || !handle.get().get()) {
					continue;
				}

				auto actor = handle.get().get();
				RE::FormID base = actor->GetBaseObject()->GetFormID();
				RE::FormID ref = actor->GetFormID();

				npc_ref_map->insert_or_assign(base, ref);
			}

			// Captures some unloaded NPCs based on cell.
			for (auto& handle : process->middleLowActorHandles) {
				if (!handle.get() || !handle.get().get()) {
					continue;
				}

				auto actor = handle.get().get();
				RE::FormID base = actor->GetBaseObject()->GetFormID();
				RE::FormID ref = actor->GetFormID();

				if (npc_ref_map->find(base) != npc_ref_map->end()) {
					logger::warn("Duplicate NPC reference found (middleLow): {}", actor->GetName());
				} else {
					npc_ref_map->insert_or_assign(base, ref);
				}
			}

			// Same as above.
			for (auto& handle : process->highActorHandles) {
				if (!handle.get() || !handle.get().get()) {
					continue;
				}

				auto actor = handle.get().get();
				RE::FormID base = actor->GetBaseObject()->GetFormID();
				RE::FormID ref = actor->GetFormID();

				if (npc_ref_map->find(base) != npc_ref_map->end()) {
					logger::warn("Duplicate NPC reference found (highActorHandle): {}", actor->GetName());
				} else {
					npc_ref_map->insert_or_assign(base, ref);
				}
			}

			// Callback to Data to merge with master list.
			Data::GetSingleton()->MergeNPCRefIds(npc_ref_map);
		});
	}

	template <class T>
	void Data::CacheItems(RE::TESDataHandler* a_data)
	{
		auto dataPath = Settings::GetSingleton()->GetConfig().dataPath;
		dataPath = Utils::RemoveQuotesInPath(dataPath);

		for (RE::TESForm* form : a_data->GetFormArray<T>()) {
			const RE::TESFile* mod = form->GetFile(0);

			_cache.push_back(ItemData{ form });

			//Add mod file to list.
			if (!_itemModList.contains(mod)) {
				_itemModList.insert(mod);
				_modList.insert(mod);

				std::filesystem::path path = dataPath + "/" + mod->fileName;
				if (std::filesystem::exists(path)) {
					std::time_t creationTime = GetFileCreationTime(path);
					_modListLastModified[mod] = creationTime;
				} else {
					logger::warn("File does not exist: {}", path.string());
				}
			}

			if (_itemModList.contains(mod)) {
				// Check if the mod is already in the map
				auto it = _itemListModFormTypeMap.find(mod);
				if (it == _itemListModFormTypeMap.end())
					_itemListModFormTypeMap[mod] = ModFileItemFlags();

				if (_itemListModFormTypeMap[mod].armor == false)
					_itemListModFormTypeMap[mod].armor = form->GetFormType() == RE::FormType::Armor;

				if (_itemListModFormTypeMap[mod].book == false)
					_itemListModFormTypeMap[mod].book = form->GetFormType() == RE::FormType::Book;

				if (_itemListModFormTypeMap[mod].weapon == false) {
					_itemListModFormTypeMap[mod].weapon = form->GetFormType() == RE::FormType::Weapon;
				}

				if (_itemListModFormTypeMap[mod].misc == false)
					_itemListModFormTypeMap[mod].misc = form->GetFormType() == RE::FormType::Misc;

				if (_itemListModFormTypeMap[mod].misc == false)
					_itemListModFormTypeMap[mod].misc = form->GetFormType() == RE::FormType::SoulGem;

				if (_itemListModFormTypeMap[mod].key == false)
					_itemListModFormTypeMap[mod].key = form->GetFormType() == RE::FormType::KeyMaster;

				if (_itemListModFormTypeMap[mod].ammo == false)
					_itemListModFormTypeMap[mod].ammo = form->GetFormType() == RE::FormType::Ammo;

				if (_itemListModFormTypeMap[mod].alchemy == false)
					_itemListModFormTypeMap[mod].alchemy = form->GetFormType() == RE::FormType::AlchemyItem;

				if (_itemListModFormTypeMap[mod].ingredient == false)
					_itemListModFormTypeMap[mod].ingredient = form->GetFormType() == RE::FormType::Ingredient;

				if (_itemListModFormTypeMap[mod].scroll == false)
					_itemListModFormTypeMap[mod].scroll = form->GetFormType() == RE::FormType::Scroll;
			}
		}
	}

	template <class T>
	void Data::CacheStaticObjects(RE::TESDataHandler* a_data)
	{
		for (RE::TESForm* form : a_data->GetFormArray<T>()) {
			const RE::TESFile* mod = form->GetFile(0);

			_staticCache.push_back(ObjectData{ form });

			if (!_staticModList.contains(mod)) {
				_staticModList.insert(mod);
				_modList.insert(mod);
			}

			if (_staticModList.contains(mod)) {
				auto it = _itemListModFormTypeMap.find(mod);
				if (it == _itemListModFormTypeMap.end()) {
					_itemListModFormTypeMap[mod] = ModFileItemFlags();
				}

				if (_itemListModFormTypeMap[mod].tree == false)
					_itemListModFormTypeMap[mod].tree = form->GetFormType() == RE::FormType::Tree;

				if (_itemListModFormTypeMap[mod].activator == false)
					_itemListModFormTypeMap[mod].activator = form->GetFormType() == RE::FormType::Activator;

				if (_itemListModFormTypeMap[mod].container == false)
					_itemListModFormTypeMap[mod].container = form->GetFormType() == RE::FormType::Container;

				if (_itemListModFormTypeMap[mod].door == false)
					_itemListModFormTypeMap[mod].door = form->GetFormType() == RE::FormType::Door;

				if (_itemListModFormTypeMap[mod].light == false)
					_itemListModFormTypeMap[mod].light = form->GetFormType() == RE::FormType::Light;

				if (_itemListModFormTypeMap[mod].staticObject == false)
					_itemListModFormTypeMap[mod].staticObject = form->GetFormType() == RE::FormType::Static;

				if (_itemListModFormTypeMap[mod].furniture == false)
					_itemListModFormTypeMap[mod].furniture = form->GetFormType() == RE::FormType::Furniture;

				if (_itemListModFormTypeMap[mod].flora == false)
					_itemListModFormTypeMap[mod].flora = form->GetFormType() == RE::FormType::Flora;
			}
		}
	}

	// https://github.com/shad0wshayd3-TES5/BakaHelpExtender | License : MIT
	// Absolute unit of code here. Super grateful for the author.
	// NOTE: This doesn't seem to collect all cells, specifically cells under worldspaces which are typically
	// exterior cells. Can't find a work around as of 2/19/2025. Since worldspace cellmap hash only contains
	// a grid of loaded cells, not every cell. Still need to figure a more reliable method out.
	// TODO: Need to re-implement idx for load order (?)
	void Data::CacheCells(const RE::TESFile* a_file, std::vector<CellData>& a_cellMap)
	{
		auto tesFile = const_cast<RE::TESFile*>(a_file);
		if (!tesFile->OpenTES(RE::NiFile::OpenMode::kReadOnly, false)) {
			logger::warn(FMT_STRING("failed to open file: {:s}"sv), a_file->fileName);
			return;
		}

		do {
			if (tesFile->currentform.form == 'LLEC') {
				char edid[512]{ '\0' };
				bool gotEDID{ false };

				char luff[512]{ '\0' };
				bool gotLUFF{ false };

				do {
					switch (tesFile->GetCurrentSubRecordType()) {
					case 'DIDE':
						gotEDID = tesFile->ReadData(edid, tesFile->actualChunkSize);
						break;
					case 'LLUF':
						gotLUFF = tesFile->ReadData(luff, tesFile->actualChunkSize);
						break;
					default:
						break;
					}

					if (gotEDID) {
						if (gotLUFF) {
							a_cellMap.push_back(CellData(ValidateTESFileName(tesFile), "Unknown", "Unknown", luff, edid, tesFile));
						} else {
							a_cellMap.push_back(CellData(ValidateTESFileName(tesFile), "Unknown", "Unknown", "Unknown", edid, tesFile));
						}

						if (!_cellModList.contains(tesFile)) {
							_cellModList.insert(tesFile);
							_modList.insert(tesFile);
						}

						if (_cellModList.contains(tesFile)) {
							auto it = _itemListModFormTypeMap.find(tesFile);
							if (it == _itemListModFormTypeMap.end()) {
								_itemListModFormTypeMap[tesFile] = ModFileItemFlags();
							}

							_itemListModFormTypeMap[tesFile].cell = true;
						}

						break;
					}
				} while (tesFile->SeekNextSubrecord());
			}
		} while (tesFile->SeekNextForm(true));

		if (!tesFile->CloseTES(false)) {
			logger::warn(FMT_STRING("failed to close file: {:s}"sv), tesFile->fileName);
		}
	}

	void Data::GenerateItemList()
	{
		_cache.clear();

		if (auto dataHandler = RE::TESDataHandler::GetSingleton()) {
			CacheItems<RE::TESObjectARMO>(dataHandler);
			CacheItems<RE::TESObjectBOOK>(dataHandler);
			CacheItems<RE::TESObjectWEAP>(dataHandler);
			CacheItems<RE::TESObjectMISC>(dataHandler);
			CacheItems<RE::TESAmmo>(dataHandler);
			CacheItems<RE::AlchemyItem>(dataHandler);
			CacheItems<RE::IngredientItem>(dataHandler);
			CacheItems<RE::TESKey>(dataHandler);
			CacheItems<RE::ScrollItem>(dataHandler);
			CacheItems<RE::TESSoulGem>(dataHandler);
		}
	}

	void Data::GenerateNPCList()
	{
		_npcCache.clear();

		if (auto dataHandler = RE::TESDataHandler::GetSingleton()) {
			CacheNPCs<RE::TESNPC>(dataHandler);
			CacheNPCRefIds();
		}
	}

	void Data::GenerateNPCClassList()
	{
		_npcClassList.clear();

		for (auto& npc : _npcCache) {
			auto className = npc.GetClass();

			if (_npcClassList.find(className) == _npcClassList.end()) {
				_npcClassList.insert(className);
			}
		}
	}

	void Data::GenerateNPCRaceList()
	{
		_npcRaceList.clear();

		for (auto& npc : _npcCache) {
			auto raceName = npc.GetRace();

			if (_npcRaceList.find(raceName) == _npcRaceList.end()) {
				_npcRaceList.insert(raceName);
			}
		}
	}

	void Data::GenerateNPCFactionList()
	{
		_npcFactionList.clear();

		for (auto& npc : _npcCache) {
			auto factionNames = npc.GetFactions();
			for (auto& faction : factionNames) {
				std::string factionName = ValidateTESName(faction.faction);

				if (_npcFactionList.find(factionName) == _npcFactionList.end()) {
					_npcFactionList.insert(factionName);
				}
			}
		}
	}

	void Data::GenerateObjectList()
	{
		_staticCache.clear();

		if (auto dataHandler = RE::TESDataHandler::GetSingleton()) {
			CacheStaticObjects<RE::TESObjectTREE>(dataHandler);
			CacheStaticObjects<RE::TESObjectACTI>(dataHandler);
			CacheStaticObjects<RE::TESObjectDOOR>(dataHandler);
			CacheStaticObjects<RE::TESObjectSTAT>(dataHandler);
			CacheStaticObjects<RE::TESObjectCONT>(dataHandler);
			CacheStaticObjects<RE::TESObjectLIGH>(dataHandler);
			CacheStaticObjects<RE::TESFlora>(dataHandler);
			CacheStaticObjects<RE::TESFurniture>(dataHandler);
		}
	}

	void Data::GenerateCellList()
	{
		_cellCache.clear();

		WorldspaceCells cells;

		if (auto dataHandler = RE::TESDataHandler::GetSingleton()) {
			for (const auto& cell : cells.table) {
				const auto& [_plugin, space, place, name, editorid] = cell;
				std::string plugin = _plugin + ".esm";
				const RE::TESFile* modFile = dataHandler->LookupModByName(plugin.c_str());

				if (!_cellModList.contains(modFile)) {
					_cellModList.insert(modFile);
				}

				_cellCache.push_back(CellData(plugin, space, place, name, editorid, modFile));
			}

			for (const RE::TESForm* form : dataHandler->GetFormArray<RE::TESWorldSpace>()) {
				const RE::TESFile* mod = form->GetFile(0);

				if (!_cellModList.contains(mod)) {
					CacheCells(mod, _cellCache);
					_cellModList.insert(mod);
				}
			}
		}
	}

	void Data::Run()
	{
		Settings::Config& config = Settings::GetSingleton()->GetConfig();

		if (config.showAddItemMenu == true) {
			GenerateItemList();
		}

		if (config.showNPCMenu) {
			GenerateNPCList();
		}

		if (config.showObjectMenu) {
			GenerateObjectList();
		}

		if (config.showTeleportMenu) {
			GenerateCellList();
		}

		for (auto& file : _modList) {
			std::string modName = ValidateTESFileName(file);
			_modListSorted.insert(modName);
		}

		// Moved from TableView. 2/4/2025
		GenerateNPCClassList();
		GenerateNPCRaceList();
		GenerateNPCFactionList();

		Utils::SetDescriptionFrameworkInterface(DescriptionFrameworkAPI::GetDescriptionFrameworkInterface001());
	}
}