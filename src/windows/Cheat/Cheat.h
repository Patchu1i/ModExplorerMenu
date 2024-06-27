#pragma once

#include "Data.h"
#include "Settings.h"

namespace ModExplorerMenu
{
	class Tab
	{
	private:
		std::string m_name;
		bool m_enabled;

	public:
		Tab(std::string a_name, bool a_enabled) :
			m_name(a_name),
			m_enabled(a_enabled)
		{}

		void Enable() { m_enabled = true; }
		void Disable() { m_enabled = false; }

		bool IsEnabled() { return m_enabled; }

		[[nodiscard]] std::string GetName() { return m_name; }
	};

	class CheatWindow
	{
	private:
		static inline Tab* activeTab;

		static void EnableTab(Tab& a_tab)
		{
			for (auto& tab : tabs) {
				tab.Disable();
			}
			a_tab.Enable();

			activeTab = &a_tab;
		}

		static inline Tab m_dragonSouls = Tab("Dragon Souls", true);
		static inline Tab m_perks = Tab("Perks", false);
		static inline Tab m_skills = Tab("Skills", false);
		static inline Tab m_shouts = Tab("Shouts", false);
		static inline Tab m_character = Tab("Character", false);
		static inline Tab m_progression = Tab("Progression", false);
		static inline Tab m_combat = Tab("Combat", false);
		static inline Tab m_world = Tab("World", false);
		static inline Tab m_misc = Tab("Misc", false);

		static inline std::vector<Tab> tabs = {
			m_dragonSouls,
			m_perks,
			m_skills,
			m_shouts,
			m_character,
			m_progression,
			m_combat,
			m_world,
			m_misc
		};

		// enum Tab
		// {
		// 	kDragonSouls,
		// 	kPerks,
		// 	kSkills,
		// 	kShouts,
		// 	kCharacter,
		// 	kProgression,
		// 	kCombat,
		// 	kWorld,
		// 	kMisc
		// };

		// std::map<Tab, bool> m_tabs = {
		// 	{ kDragonSouls, true },
		// 	{ kPerks, false },
		// 	{ kSkills, false },
		// 	{ kShouts, false },
		// 	{ kCharacter, false },
		// 	{ kProgression, false },
		// 	{ kCombat, false },
		// 	{ kWorld, false },
		// 	{ kMisc, false }
		// };

	public:
		static void Draw(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowSidebar(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowSubMenu(Settings::Style& a_style, Settings::Config& a_config);
		static void Init();

	private:
	};
}