#include "NPC.h"

namespace ModExplorerMenu
{
	// This function clears the current npcList cache, and populates it
	// with *new* CachedNPC objects from `RE::ProcessLists->highActorHandles`.
	void NPCWindow::PopulateListWithLocals()
	{
		b_ShowNearbyNPC = true;
		b_ShowSpawnedNPC = false;
		b_ShowAllNPC = false;

		npcList.clear();

		SKSE::GetTaskInterface()->AddTask([]() {
			auto process = RE::ProcessLists::GetSingleton();
			for (auto& handle : process->highActorHandles) {
				if (!handle.get() || !handle.get().get()) {
					continue;
				}

				auto* actor = handle.get().get();
				auto* TESNPC = actor->GetBaseObject()->As<RE::TESNPC>();

				if (TESNPC) {
					auto newCachedNPC = Data::CreateCachedNPC(TESNPC);
					npcList.push_back(newCachedNPC);
				}
			}
		});
	}
}