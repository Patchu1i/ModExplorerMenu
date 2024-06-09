#include "Data.h"
#include "Utils/Worldspace.h"

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
void MEMData::CacheNPCs(RE::TESDataHandler* a_data)
{
	for (auto form : a_data->GetFormArray<T>()) {
		RE::TESNPC* npc = form->As<RE::TESNPC>();
		RE::TESNPC::Skills skills = npc->playerSkills;

		CachedNPC _npc = {
			.form = form,
			.plugin = form->GetFile()->fileName,
			.name = form->GetFullName(),
			.formid = fmt::format("{:08x}", form->GetFormID()),
			.editorid = GetEditorID(form->GetFormID()),
			.health = npc->GetBaseActorValue(RE::ActorValue::kHealth),
			.magicka = npc->GetBaseActorValue(RE::ActorValue::kMagicka),
			.stamina = npc->GetBaseActorValue(RE::ActorValue::kStamina),
			.carryweight = npc->GetBaseActorValue(RE::ActorValue::kCarryWeight),
			.skills = npc->playerSkills,
			.favorite = false
		};

		_npcCache.push_back(_npc);
	}
}

template <class T>
void MEMData::CacheItems(RE::TESDataHandler* a_data)
{
	for (auto form : a_data->GetFormArray<T>()) {
		const char* name = form->GetFullName();
		RE::FormID _formid = form->GetFormID();
		std::string formid = fmt::format("{:08x}", _formid);
		std::string editorid = GetEditorID(_formid);
		RE::FormType formType = form->GetFormType();
		std::string typeName = static_cast<std::string>(RE::FormTypeToString(formType));
		float weight = form->GetWeight();

		std::int32_t goldValue = 0;
		bool non_playable = false;

		//logger::info("FormType: {}", typeName);

		if (formType == RE::FormType::NPC) {
			auto npc = form->As<RE::TESNPC>();
			if (npc->IsPlayerRef()) {
				continue;
			}

			// TODO: Implement NPC Cache
			// Probably have to do this outside of item cache.
		} else {
			goldValue = form->GetGoldValue();

			if (formType == RE::FormType::Weapon) {
				non_playable = form->As<RE::TESObjectWEAP>()->weaponData.flags.any(RE::TESObjectWEAP::Data::Flag::kNonPlayable);
			}
		}

		RE::TESFile* mod = form->GetFile();

		_cache.push_back({ name, formid, form, editorid, formType, typeName, goldValue, mod, weight, non_playable });

		//Add mod file to list.
		if (!_modList.contains(mod)) {
			_modList.insert(mod);
		}
	}
}

// https://github.com/shad0wshayd3-TES5/BakaHelpExtender | License : MIT
// Absolute unit of code here. Super grateful for the author.
void MEMData::CacheCells(RE::TESFile* a_file, std::vector<CachedCell>& a_cellMap)
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
						a_cellMap.push_back(CachedCell(a_file->fileName, "Unkown", "Unkown", "Unkown", edid, a_file));
						continue;
					}
					break;

				case 'ATAD':
					gotDATA = a_file->ReadData(&data, a_file->actualChunkSize);
					if (gotEDID && gotDATA && ((data & 1) == 0)) {
						a_cellMap.push_back(CachedCell(a_file->fileName, "Unkown", "Unkown", "Unkown", edid, a_file));
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

void MEMData::Run()
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

	// TODO: Implement NPC
	CacheNPCs<RE::TESNPC>(dataHandler);

	// TODO: Implement Cells
	WorldspaceCells cells;

	for (const auto& cell : cells.table) {
		const auto& [_plugin, space, place, name, editorid] = cell;
		std::string plugin = _plugin + ".esm";
		const RE::TESFile* mod = dataHandler->LookupModByName(plugin.c_str());

		_cellCache.push_back(CachedCell(plugin, space, place, name, editorid, mod));
	}

	// Overwrite _cellCache with Baka changes
	for (const auto& file : _modList) {
		CacheCells(file, _cellCache);
	}
}