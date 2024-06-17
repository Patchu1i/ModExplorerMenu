#include "Console.h"
#include "NPC.h"

namespace ModExplorerMenu
{
	// This function clears the current npcList cache, and populates it
	// with *new* CachedNPC objects from the spawned actors vector.
	void NPCWindow::PopulateListWithSpawned()
	{
		SetState(showSpawned);
		npcList.clear();

		std::vector<NPC*> cached_search;
		for (auto& npc : Data::GetNPCList()) {
			cached_search.push_back(&npc);
		}

		auto* spawnedVector = Console::GetSpawnedActorsVector();

		if (!spawnedVector) {
			stl::report_and_fail("Failed to get spawned actors vector.");
		}

		for (RE::FormID refID : *spawnedVector) {
			auto ObjectREFR = RE::TESForm::LookupByID<RE::TESObjectREFR>(refID);
			if (ObjectREFR) {
				auto TESNPC = ObjectREFR->GetBaseObject()->As<RE::TESNPC>();
				if (TESNPC) {
					auto newCachedNPC = Data::CreateCachedNPC(TESNPC);
					npcList.push_back(newCachedNPC);
				}
			}
		}
	}
}