#pragma once
#include "include/I/ISearch.h"
#include <PCH.h>

// clang-format off

namespace Modex
{
	class Blacklist : private ISearch
	{
	public:
		static inline Blacklist* GetSingleton()
		{
			static Blacklist singleton;
			return &singleton;
		}

		void 					Draw(float a_offset);
		void					Init();
	
		// Used in TableView as well. Could this be an abstract class?
		void                                    BuildPluginList();
        std::vector<const RE::TESFile*>         pluginList;
		std::vector<std::string>				pluginListVector;

	private:

		int 					totalPlugins;
		int 					blacklistedPlugins;
		int 					nonBlacklistedPlugins;
		int						hiddenPlugins;
		bool					updateHidden;

		// Plugin Type Filtering
		std::vector<RE::FormType> filterList = {
			{ RE::FormType::Armor },
			{ RE::FormType::AlchemyItem },
			{ RE::FormType::Ammo },
			{ RE::FormType::Book },
			{ RE::FormType::Ingredient },
			{ RE::FormType::KeyMaster },
			{ RE::FormType::Misc },
			{ RE::FormType::Weapon },
			{ RE::FormType::NPC },
			{ RE::FormType::Scroll },
			{ RE::FormType::SoulGem },
			{ RE::FormType::Static },
			{ RE::FormType::Tree },
			{ RE::FormType::Activator },
			{ RE::FormType::Container },
			{ RE::FormType::Door },
			{ RE::FormType::Light },
			{ RE::FormType::Furniture },
			{ RE::FormType::Cell }
		};

		RE::FormType			primaryFilter;

		// ISearch Interface
		char 					modSearchBuffer[256];
		std::string 			selectedMod;
	};
}