#include "Data.h"
#include "lib/Worldspace.h"

using _GetFormEditorID = const char* (*)(std::uint32_t);
std::string getFormEditorID(const RE::TESForm* a_form)
{
	switch (a_form->GetFormType()) {
	case RE::FormType::Keyword:
	case RE::FormType::LocationRefType:
	case RE::FormType::Action:
	case RE::FormType::MenuIcon:
	case RE::FormType::Global:
	case RE::FormType::HeadPart:
	case RE::FormType::Race:
	case RE::FormType::Sound:
	case RE::FormType::Script:
	case RE::FormType::Navigation:
	case RE::FormType::Cell:
	case RE::FormType::WorldSpace:
	case RE::FormType::Land:
	case RE::FormType::NavMesh:
	case RE::FormType::Dialogue:
	case RE::FormType::Quest:
	case RE::FormType::Idle:
	case RE::FormType::AnimatedObject:
	case RE::FormType::ImageAdapter:
	case RE::FormType::VoiceType:
	case RE::FormType::Ragdoll:
	case RE::FormType::DefaultObject:
	case RE::FormType::MusicType:
	case RE::FormType::StoryManagerBranchNode:
	case RE::FormType::StoryManagerQuestNode:
	case RE::FormType::StoryManagerEventNode:
	case RE::FormType::SoundRecord:
		return a_form->GetFormEditorID();
	default:
		{
			static auto tweaks = GetModuleHandleA("po3_Tweaks");
			static auto func = reinterpret_cast<_GetFormEditorID>(GetProcAddress(tweaks, "GetFormEditorID"));
			if (func) {
				return func(a_form->formID);
			}
			return {};
		}
	}
}

template <class T>
void MEMData::CacheItems(RE::TESDataHandler* a_data)
{
	for (auto form : a_data->GetFormArray<T>()) {
		const char* name = form->GetFullName();
		RE::FormID _formid = form->GetFormID();
		std::string formid = fmt::format("{:08x}", _formid);
		std::string editorid = getFormEditorID(form);
		RE::FormType formType = form->GetFormType();
		std::string typeName = static_cast<std::string>(RE::FormTypeToString(formType));

		std::int32_t goldValue = 0;
		bool non_playable = false;

		//logger::info("FormType: {}", typeName);

		if (formType == RE::FormType::NPC) {
			auto npc = form->As<RE::TESNPC>();
			if (npc->IsPlayerRef()) {
				continue;
			}

			// npc specific stuff
			//logger::info("NPC: {}", form->GetFullName());
		} else if (formType == RE::FormType::WorldSpace) {
			// GetFullName is the worldspace not cell.
			//logger::info("CELL: {}", form->GetFullName());
		} else {
			goldValue = form->GetGoldValue();

			if (formType == RE::FormType::Weapon) {
				non_playable = form->As<RE::TESObjectWEAP>()->weaponData.flags.any(RE::TESObjectWEAP::Data::Flag::kNonPlayable);
			}
		}

		RE::TESFile* mod = form->GetFile();

		_cache.push_back({ name, formid, form, editorid, formType, typeName, goldValue, mod, non_playable });

		// Add mod file to list.
		// if (!_modList.contains(mod)) {
		// 	_modList.insert(mod);
		// 	// .first->GetFilename().data()
		// }

		// write a logger::info() here to detail the size of _cache in memory and the amount of items added
	}
}

void MEMData::Run()
{
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

	// NPC -- TODO: Implement
	CacheItems<RE::TESNPC>(dataHandler);

	// Cells -- TODO:: Implement
	WorldspaceCells cells;

	for (const auto& cell : cells.table) {
		const auto& [plugin, space, place, name, id] = cell;
		//logger::info("Cell: {}, {}, {}, {}, {}", plugin, space, place, name, id);
	}
}