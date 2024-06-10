#include "Window.h"

//TODO: Some poor data structure choices are showing here.
//      Shouldn't need to reinstantiate an entire npc list
//      just to filter it.  Doesn't seem to cause performance
//      issues, but it's still a bad practice I feel.

void NPCWindow::PopulateListWithLocals()
{
	showLocalsOnly = true;  // TODO: Better state management.

	auto references = std::make_shared<std::vector<RE::FormID>>();

	npcList.clear();

	std::vector<MEMData::CachedNPC*> cached_search;
	for (auto& npc : MEMData::GetNPCList()) {
		cached_search.push_back(&npc);
	}

	auto callback = [cached_search, references]() {
		if (references->empty()) {
			return;
		}

		for (int i = 0; i < cached_search.size(); i++) {
			auto npc = cached_search.at(i);
			auto formid = npc->form->formID;
			auto found = std::find(references->begin(), references->end(), formid);
			if (found != references->end()) {
				auto foundIndex = std::distance(references->begin(), found);
				npc->refID = references->at(foundIndex + 1);  // add refID
				npcList.push_back(npc);
			}
		}
	};

	SKSE::GetTaskInterface()->AddTask([references, callback]() {
		auto process = RE::ProcessLists::GetSingleton();
		for (auto& handle : process->highActorHandles) {
			if (!handle.get() || !handle.get().get()) {
				continue;
			}

			auto actor = handle.get().get();
			auto base = actor->GetBaseObject()->GetFormID();
			auto ref = actor->GetFormID();

			// Find object with matching baseid, and store its reference
			// into the references vector.
			references->push_back(base);
			references->push_back(ref);
		}

		callback();  // Callback to main thread upon completion.
	});
}