#include "Windows/Cheat/Cheat.h"

namespace ModExplorerMenu
{
	void CheatWindow::ShowCharacter()
	{
		auto* actor = RE::PlayerCharacter::GetSingleton()->AsActorValueOwner();

		if (actor == nullptr) {
			return;
		}

		ImGui::Text("Character:");
	}

}