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

		if (activeTab == &m_dragonSouls) {
			// Show Dragon Souls tab
			ImGui::Text("Dragon Souls: %d", 0);
		} else if (activeTab == &m_perks) {
			// Show Perks tab
		} else if (activeTab == &m_skills) {
			// Show Skills tab
		} else if (activeTab == &m_shouts) {
			// Show Shouts tab
		} else if (activeTab == &m_character) {
			// Show Character tab
		} else if (activeTab == &m_progression) {
			// Show Progression tab
		} else if (activeTab == &m_combat) {
			// Show Combat tab
		} else if (activeTab == &m_world) {
			// Show World tab
		} else if (activeTab == &m_misc) {
			// Show Misc tab
		}
	}
}