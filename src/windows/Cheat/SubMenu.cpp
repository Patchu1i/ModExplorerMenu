#include "Cheat.h"

namespace ModExplorerMenu
{
	void CheatWindow::ShowSubMenu(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_style;
		(void)a_config;

		if (activeTab == nullptr) {
			return;
		}

		ImGui::SeparatorText(activeTab->GetName().c_str());

		switch (activeTab->GetID()) {
		case Tab::ID::kSkills:
			ShowSkills();
			break;
		case Tab::ID::kShouts:
			ShowShouts();
			break;
		case Tab::ID::kCharacter:
			ShowCharacter();
			break;
		case Tab::ID::kProgression:
			ShowProgression();
			break;
		case Tab::ID::kCombat:
			ShowCombat();
			break;
		case Tab::ID::kWorld:
			ShowWorld();
			break;
		case Tab::ID::kMisc:
			ShowMisc();
			break;
		}
	}
}