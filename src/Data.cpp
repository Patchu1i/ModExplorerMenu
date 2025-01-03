#include "Data.h"
#include "Settings.h"
#include "Utils/Util.h"
#include "Utils/Worldspace.h"
#include "Windows/Persistent.h"

namespace Modex
{
	// https://github.com/Nightfallstorm/DescriptionFramework | License GPL-3.0
	using _GetFormEditorID = const char* (*)(std::uint32_t);

	std::string GetEditorID(RE::FormID a_formID)
	{
		static auto tweaks = GetModuleHandleA("po3_Tweaks");
		static auto function = reinterpret_cast<_GetFormEditorID>(GetProcAddress(tweaks, "GetFormEditorID"));
		if (function) {
			return function(a_formID);
		}
		return {};
	}

	template <class T>
	void Data::CacheNPCs(RE::TESDataHandler* a_data)
	{
		for (RE::TESForm* form : a_data->GetFormArray<T>()) {
			RE::FormID formid = form->GetFormID();
			RE::TESFile* mod = form->GetFile();
			bool favorite = PersistentData::m_favorites[GetEditorID(formid)];

			RE::TESNPC* npc = form->As<RE::TESNPC>();

			if (npc->IsPlayerRef()) {
				continue;
			}

			_npcCache.push_back(Modex::NPC{ form, formid, mod, favorite });

			if (!_npcModList.contains(mod)) {
				_npcModList.insert(mod);
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

	// Merge NPC Reference IDs into the NPC cache list.
	void Data::MergeNPCRefIds(std::shared_ptr<std::unordered_map<RE::FormID, RE::FormID>> npc_ref_map)
	{
		if (npc_ref_map->empty()) {
			logger::warn("No NPC references found.");
		} else {
			for (auto& npc : _npcCache) {
				auto it = npc_ref_map->find(npc.FormID);
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
			Data::MergeNPCRefIds(npc_ref_map);
		});
	}

	template <class T>
	void Data::CacheItems(RE::TESDataHandler* a_data)
	{
		auto dataPath = Settings::GetSingleton()->GetConfig().dataPath;
		dataPath = Utils::RemoveQuotesInPath(dataPath);

		for (RE::TESForm* form : a_data->GetFormArray<T>()) {
			RE::FormID formid = form->GetFormID();
			RE::TESFile* mod = form->GetFile();
			bool favorite = PersistentData::m_favorites[GetEditorID(formid)];

			_cache.push_back(Modex::Item{ form, formid, mod, favorite });

			//Add mod file to list.
			if (!_itemModList.contains(mod)) {
				_itemModList.insert(mod);

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
			RE::FormID formid = form->GetFormID();
			RE::TESFile* mod = form->GetFile();
			bool favorite = PersistentData::m_favorites[GetEditorID(formid)];

			_staticCache.push_back(Modex::StaticObject{ form, formid, mod, favorite });

			if (!_staticModList.contains(mod)) {
				_staticModList.insert(mod);
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
			}
		}
	}

	// https://github.com/shad0wshayd3-TES5/BakaHelpExtender | License : MIT
	// Absolute unit of code here. Super grateful for the author.
	// Modified to include both Interior and Exterior cells, and to also cache fullname record.
	void Data::CacheCells(RE::TESFile* a_file, std::vector<Cell>& a_cellMap)
	{
		if (!a_file->OpenTES(RE::NiFile::OpenMode::kReadOnly, false)) {
			logger::warn(FMT_STRING("failed to open file: {:s}"sv), a_file->fileName);
			return;
		}

		do {
			if (a_file->currentform.form == 'LLEC') {
				char edid[512]{ '\0' };
				bool gotEDID{ false };

				char luff[512]{ '\0' };
				bool gotLUFF{ false };

				do {
					switch (a_file->GetCurrentSubRecordType()) {
					case 'DIDE':
						gotEDID = a_file->ReadData(edid, a_file->actualChunkSize);
						break;
					case 'LLUF':
						gotLUFF = a_file->ReadData(luff, a_file->actualChunkSize);
						break;
					default:
						break;
					}

					if (gotEDID && gotLUFF) {
						bool favorite = PersistentData::m_favorites[edid];

						a_cellMap.push_back(Cell(a_file->fileName, "Unknown", "Unknown", luff, edid, favorite, a_file));

						if (!_cellModList.contains(a_file)) {
							_cellModList.insert(a_file);
						}

						break;
					}
				} while (a_file->SeekNextSubrecord());
			}
		} while (a_file->SeekNextForm(true));

		if (!a_file->CloseTES(false)) {
			logger::warn(FMT_STRING("failed to close file: {:s}"sv), a_file->fileName);
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
			if (auto className = npc.GetClass().data()) {
				if (className == nullptr) {
					continue;
				}

				if (_npcClassList.find(className) == _npcClassList.end()) {
					_npcClassList.insert(className);
				}
			}
		}
	}

	void Data::GenerateNPCRaceList()
	{
		_npcRaceList.clear();

		for (auto& npc : _npcCache) {
			if (auto raceName = npc.GetRace().data()) {
				if (raceName == nullptr) {
					continue;
				}

				if (_npcRaceList.find(raceName) == _npcRaceList.end()) {
					_npcRaceList.insert(raceName);
				}
			}
		}
	}

	void Data::GenerateNPCFactionList()
	{
		_npcFactionList.clear();

		for (auto& npc : _npcCache) {
			auto factionNames = npc.AsTESNPC->factions;
			for (auto& faction : factionNames) {
				std::string factionName = faction.faction->GetFullName();

				if (factionName.empty()) {
					continue;
				}

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
				const RE::TESFile* mod = dataHandler->LookupModByName(plugin.c_str());
				RE::TESFile* modFile = const_cast<RE::TESFile*>(mod);
				bool favorite = PersistentData::m_favorites[editorid];

				if (!_cellModList.contains(modFile)) {
					_cellModList.insert(modFile);
				}

				_cellCache.push_back(Cell(plugin, space, place, name, editorid, favorite, mod));
			}

			for (const RE::TESForm* form : dataHandler->GetFormArray<RE::TESWorldSpace>()) {
				RE::TESFile* mod = form->GetFile();

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
	}
}