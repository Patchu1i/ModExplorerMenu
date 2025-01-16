#pragma once

#include "include/D/DataTypes.h"
#include "include/P/Persistent.h"
#include <PCH.h>

// clang-format off

namespace Modex
{
	class Data
	{
	public:

		enum PLUGIN_TYPE
		{
			ALL = 0,
			ITEM,
			NPC,
			OBJECT,
			CELL,
		};

		struct ModFileItemFlags
		{
			bool 	alchemy 		= false;
			bool 	ingredient 		= false;
			bool 	ammo 			= false;
			bool 	key 			= false;
			bool 	misc 			= false;
			bool 	armor 			= false;
			bool 	book 			= false;
			bool 	weapon 			= false;
			bool 	scroll 			= false;
			bool 	npc 			= false;
			bool 	staticObject 	= false;
			bool 	tree 			= false;
			bool 	activator 		= false;
			bool 	container 		= false;
			bool 	door 			= false;
			bool 	light 			= false;
			bool 	furniture 		= false;
		};

		static inline Data* GetSingleton()
		{
			static Data singleton;
			return &singleton;
		}

		void Run();
	
		std::unordered_set<const RE::TESFile*>			GetModulePluginList(PLUGIN_TYPE a_type);
		std::vector<const RE::TESFile*> 				GetModulePluginListSorted(PLUGIN_TYPE a_type);
		std::vector<std::string> 						GetSortedListOfPluginNames();
		std::vector<std::string> 						GetFilteredListOfPluginNames(PLUGIN_TYPE a_type, RE::FormType a_selectedFilter);
		void 											CacheNPCRefIds();
		void 											GenerateNPCClassList();
		void 											GenerateNPCRaceList();
		void 											GenerateNPCFactionList();
		
		[[nodiscard]] inline std::vector<ItemData>& 	GetAddItemList() 	{ return _cache; 			}
		[[nodiscard]] inline std::vector<CellData>& 	GetTeleportList() 	{ return _cellCache; 		}
		[[nodiscard]] inline std::vector<NPCData>& 		GetNPCList() 		{ return _npcCache; 		}
		[[nodiscard]] inline std::vector<ObjectData>& 	GetObjectList() 	{ return _staticCache;		}
		[[nodiscard]] inline std::set<std::string> 		GetNPCClassList() 	{ return _npcClassList; 	}
		[[nodiscard]] inline std::set<std::string> 		GetNPCRaceList() 	{ return _npcRaceList; 		}
		[[nodiscard]] inline std::set<std::string> 		GetNPCFactionList() { return _npcFactionList; 	}

	private:
		std::vector<ItemData> 							_cache;
		std::vector<CellData>	 						_cellCache;
		std::vector<NPCData> 							_npcCache;
		std::vector<RE::TESObjectREFR*> 				_npcRefIds;
		std::vector<ObjectData> 						_staticCache;
		std::unordered_set<const RE::TESFile*> 			_modList;
		std::set<std::string> 							_modListSorted;

		std::unordered_set<const RE::TESFile*> 			_itemModList;
		std::unordered_set<const RE::TESFile*> 			_npcModList;
		std::unordered_set<const RE::TESFile*> 			_staticModList;
		std::unordered_set<const RE::TESFile*> 			_cellModList;

		std::set<std::string> 							_npcClassList;
		std::set<std::string> 							_npcRaceList;
		std::set<std::string> 							_npcFactionList;

		std::unordered_map<const RE::TESFile*, ModFileItemFlags> 	_itemListModFormTypeMap;
		std::unordered_map<const RE::TESFile*, std::time_t> 		_modListLastModified;


		template <class T>
		void CacheItems(RE::TESDataHandler* a_data);

		template <class T>
		void CacheNPCs(RE::TESDataHandler* a_data);

		template <class T>
		void CacheStaticObjects(RE::TESDataHandler* a_data);

		void CacheCells(const RE::TESFile* a_file, std::vector<CellData>& a_map);
		void MergeNPCRefIds(std::shared_ptr<std::unordered_map<RE::FormID, RE::FormID>> npc_ref_map);

		void 											GenerateItemList();
		void 											GenerateNPCList();
		void 											GenerateObjectList();
		void 											GenerateCellList();

		// Windows Specific API for File Creation Time
		[[nodiscard]] std::time_t GetFileCreationTime(const std::filesystem::path& path);
	};
}
