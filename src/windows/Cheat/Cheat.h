#pragma once

#include "Data.h"
#include "Settings.h"

namespace ModExplorerMenu
{

	class PersistentData
	{
	public:
		static inline int iDragonSouls = 0;
		static inline std::vector<RE::TESShout*> shouts;
		static inline std::vector<RE::TESWordOfPower*> words;
	};

	class Tab
	{
	public:
		enum ID
		{
			kPerks,
			kSkills,
			kShouts,
			kCharacter,
			kProgression,
			kCombat,
			kWorld,
			kMisc
		};

	private:
		const std::string m_name;
		bool m_enabled;
		const ID m_id;
		const std::function<void(const ID&)> callback;

	public:
		Tab(const std::string a_name, bool a_enabled, const ID m_id, const std::function<void(const ID&)> callback) :
			m_name(a_name),
			m_enabled(a_enabled),
			m_id(m_id),
			callback(std::move(callback))
		{}

		void OnEnter() { callback(m_id); }
		void Disable() { m_enabled = false; }

		void Enable()
		{
			m_enabled = true;
			OnEnter();
		}

		bool IsEnabled() { return m_enabled; }
		ID GetID() const { return m_id; }

		[[nodiscard]] std::string GetName() const { return m_name; }
	};

	class CheatWindow : public PersistentData
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

		static void OnEnterCallback(const Tab::ID& a_id);
		static inline Tab m_perks = Tab("Perks", false, Tab::ID::kPerks, OnEnterCallback);
		static inline Tab m_skills = Tab("Skills", false, Tab::ID::kSkills, OnEnterCallback);
		static inline Tab m_shouts = Tab("Shouts", false, Tab::ID::kShouts, OnEnterCallback);
		static inline Tab m_character = Tab("Character", false, Tab::ID::kCharacter, OnEnterCallback);
		static inline Tab m_progression = Tab("Progression", false, Tab::ID::kProgression, OnEnterCallback);
		static inline Tab m_combat = Tab("Combat", false, Tab::ID::kCombat, OnEnterCallback);
		static inline Tab m_world = Tab("World", false, Tab::ID::kWorld, OnEnterCallback);
		static inline Tab m_misc = Tab("Misc", false, Tab::ID::kMisc, OnEnterCallback);

		static inline std::vector<Tab> tabs = {
			m_perks,
			m_skills,
			m_shouts,
			m_character,
			m_progression,
			m_combat,
			m_world,
			m_misc
		};

	public:
		static void Draw(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowSidebar(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowSubMenu(Settings::Style& a_style, Settings::Config& a_config);
		static void Init();

		static void ShowPerks();
		static void ShowSkills();
		static void ShowShouts();
		static void ShowCharacter();
		static void ShowProgression();
		static void ShowCombat();
		static void ShowWorld();
		static void ShowMisc();

	private:
	};
}