#include "Data.h"
#include "Utils/Worldspace.h"

// Saving this for later:
// 		auto forms = RE::TESForm::GetAllForms();
// 		for (auto iter = forms.first->begin(); iter != forms.first->end(); ++iter) {

namespace ModExplorerMenu
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

	NPC* Data::CreateCachedNPC(RE::TESNPC* a_npc)
	{
		auto form = a_npc->As<RE::TESForm>();
		return new NPC({ form,
			form->GetFormID(),
			form->GetFile() });
	}

	template <class T>
	void Data::CacheNPCs(RE::TESDataHandler* a_data)
	{
		for (RE::TESForm* form : a_data->GetFormArray<T>()) {
			RE::TESNPC* npc = form->As<RE::TESNPC>();
			RE::TESFile* mod = form->GetFile();

			RE::FormID formid = form->GetFormID();

			if (npc->IsPlayerRef()) {
				continue;
			}

			_npcCache.push_back({ form, formid, mod });

			//Add mod file to list.
			if (!_npcModList.contains(mod)) {
				_npcModList.insert(mod);
			}
		}
	}

	template <class T>
	void Data::CacheItems(RE::TESDataHandler* a_data)
	{
		for (RE::TESForm* form : a_data->GetFormArray<T>()) {
			RE::FormID formid = form->GetFormID();
			RE::TESFile* mod = form->GetFile();

			_cache.push_back(ModExplorerMenu::Item{ form, formid, mod });
			//_cache.push_back({ name, formid, form, editorid, formType, typeName, goldValue, mod, weight, non_playable });

			//Add mod file to list.
			if (!_itemModList.contains(mod)) {
				_itemModList.insert(mod);

				std::filesystem::path path = std::string("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Skyrim Special Edition\\Data\\") + mod->fileName;
				if (std::filesystem::exists(path)) {
					std::time_t creationTime = GetFileCreationTime(path);
					_modListLastModified[mod] = creationTime;
				} else {
					logger::info("File does not exist: {}", path.string());
				}
			}
		}
	}

	template <class T>
	void Data::CacheStaticObjects(RE::TESDataHandler* a_data)
	{
		for (RE::TESForm* form : a_data->GetFormArray<T>()) {
			RE::FormID formid = form->GetFormID();
			RE::TESFile* mod = form->GetFile();

			_staticCache.push_back(ModExplorerMenu::StaticObject{ form, formid, mod });

			if (!_staticModList.contains(mod)) {
				_staticModList.insert(mod);
			}
		}
	}

	// https://github.com/shad0wshayd3-TES5/BakaHelpExtender | License : MIT
	// Absolute unit of code here. Super grateful for the author.
	// Doesn't capture every cell, but it's a start.
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

				std::uint16_t data{ 0 };
				bool gotDATA{ false };

				std::uint32_t cidx{ 0 };
				cidx += a_file->compileIndex << 24;
				cidx += a_file->smallFileCompileIndex << 12;

				do {
					switch (a_file->GetCurrentSubRecordType()) {
					case 'DIDE':
						gotEDID = a_file->ReadData(edid, a_file->actualChunkSize);
						if (gotEDID && gotDATA && ((data & 1) == 0)) {
							a_cellMap.push_back(Cell(a_file->fileName, "", "", "", edid, a_file));

							if (!_cellModList.contains(a_file)) {
								_cellModList.insert(a_file);
							}

							continue;
						}
						break;

					case 'ATAD':
						gotDATA = a_file->ReadData(&data, a_file->actualChunkSize);
						if (gotEDID && gotDATA && ((data & 1) == 0)) {
							a_cellMap.push_back(Cell(a_file->fileName, "", "", "", edid, a_file));

							if (!_cellModList.contains(a_file)) {
								_cellModList.insert(a_file);
							}
							continue;
						}
						break;

					default:
						break;
					}
				} while (a_file->SeekNextSubrecord());
			}
		} while (a_file->SeekNextForm(true));

		if (!a_file->CloseTES(false)) {
			logger::warn(FMT_STRING("failed to close file: {:s}"sv), a_file->fileName);
		}
	}

	void Data::Run()
	{
		_cache.clear();
		_cellCache.clear();

		RE::TESDataHandler* dataHandler = RE::TESDataHandler::GetSingleton();

		if (!dataHandler) {
			logger::error("Failed to get data handler.");
			return;
		}

		// AddItem Specific Forms
		CacheItems<RE::TESObjectARMO>(dataHandler);
		CacheItems<RE::TESObjectBOOK>(dataHandler);
		CacheItems<RE::TESObjectWEAP>(dataHandler);
		CacheItems<RE::TESObjectMISC>(dataHandler);
		CacheItems<RE::TESAmmo>(dataHandler);
		CacheItems<RE::AlchemyItem>(dataHandler);
		CacheItems<RE::IngredientItem>(dataHandler);
		CacheItems<RE::TESKey>(dataHandler);

		CacheNPCs<RE::TESNPC>(dataHandler);

		// TODO: Did I miss any?
		CacheStaticObjects<RE::TESObjectTREE>(dataHandler);
		CacheStaticObjects<RE::TESObjectACTI>(dataHandler);
		CacheStaticObjects<RE::TESObjectDOOR>(dataHandler);
		CacheStaticObjects<RE::TESObjectSTAT>(dataHandler);
		CacheStaticObjects<RE::TESObjectCONT>(dataHandler);
		CacheStaticObjects<RE::TESObjectLIGH>(dataHandler);

		WorldspaceCells cells;
		for (const auto& cell : cells.table) {
			const auto& [_plugin, space, place, name, editorid] = cell;
			std::string plugin = _plugin + ".esm";
			const RE::TESFile* mod = dataHandler->LookupModByName(plugin.c_str());
			RE::TESFile* modFile = const_cast<RE::TESFile*>(mod);

			if (!_cellModList.contains(modFile)) {
				_cellModList.insert(modFile);
			}

			_cellCache.push_back(Cell(plugin, space, place, name, editorid, mod));
		}

		// Overwrite _cellCache with Baka changes
		for (const auto& file : _modList) {
			CacheCells(file, _cellCache);
		}
	}
}