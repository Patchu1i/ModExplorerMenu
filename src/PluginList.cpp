#pragma once

#include "include/D/Data.h"
#include "include/D/DataTypes.h"
#include "include/P/Persistent.h"

namespace Modex
{

	// Returns the creationjj time of the file at the specified path.
	//
	// @param path - The path to the file.
	// @return std::time_t - The creation time of the file.
	std::time_t Data::GetFileCreationTime(const std::filesystem::path& path)
	{
		HANDLE hFile = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			logger::warn("[Modex] Unable to open file: {}", path.string());
			return std::time_t{};
		}

		FILETIME creationTime;
		if (!GetFileTime(hFile, &creationTime, NULL, NULL)) {
			logger::warn("[Modex] Unable to get file time: {}", path.string());
			CloseHandle(hFile);
			return std::time_t{};
		}

		CloseHandle(hFile);

		ULARGE_INTEGER ull;
		ull.LowPart = creationTime.dwLowDateTime;
		ull.HighPart = creationTime.dwHighDateTime;

		return std::chrono::system_clock::to_time_t(std::chrono::system_clock::time_point(std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::nanoseconds(ull.QuadPart * 100))));
	}

	// Sort Function for Case Insensitive comparing used for Plugin Lists.
	// Doing additional checks to ensure the TESFile* pointer is not null.
	// Due to reports of random crashing. #19 for example.
	bool CaseInsensitiveCompareTESFile(const RE::TESFile* a, const RE::TESFile* b)
	{
		std::string filenameA = ValidateTESFileName(a);
		std::string filenameB = ValidateTESFileName(b);

		return std::lexicographical_compare(
			filenameA.begin(), filenameA.end(),
			filenameB.begin(), filenameB.end(),
			[](unsigned char c1, unsigned char c2) {
				return std::tolower(c1) < std::tolower(c2);
			});
	}

	// Sort Function for compileIndex aka Load Order used for Plugin Lists.
	bool CompileIndexCompareTESFileAsc(const RE::TESFile* a, const RE::TESFile* b)
	{
		return a->GetCombinedIndex() < b->GetCombinedIndex();
	}

	bool CompileIndexCompareTESFileDesc(const RE::TESFile* a, const RE::TESFile* b)
	{
		return a->GetCombinedIndex() > b->GetCombinedIndex();
	}

	// Returns a copy of the specified plugin list containing TESFile* pointers. as an unordered_set.
	// This is primarily used as a quicker lookup than GetModulePluginListSorted (more expensive).
	//
	// @param a_type - The type of plugin list to return.
	// @return std::unordered_set<const RE::TESFile*> - A set of TESFile* pointers.
	std::unordered_set<const RE::TESFile*> Data::GetModulePluginList(PLUGIN_TYPE a_type)
	{
		switch (a_type) {
		case PLUGIN_TYPE::ITEM:
			return _itemModList;
		case PLUGIN_TYPE::NPC:
			return _npcModList;
		case PLUGIN_TYPE::OBJECT:
			return _staticModList;
		case PLUGIN_TYPE::CELL:
			return _cellModList;
		case PLUGIN_TYPE::ALL:
			return _modList;
		}

		throw std::invalid_argument("Invalid PLUGIN_TYPE");
	}

	// Returns a copy of the specified plugin list as a sorted vector.
	//
	// @param a_type - The type of plugin list to return.
	// @return std::vector<const RE::TESFile*> - A list of alphabetically sorted TESFile* pointers.
	std::vector<const RE::TESFile*> Data::GetModulePluginListSorted(PLUGIN_TYPE a_type, SORT_TYPE a_sortType)
	{
		std::vector<const RE::TESFile*> copy;

		switch (a_type) {
		case PLUGIN_TYPE::ITEM:
			copy.assign(_itemModList.begin(), _itemModList.end());
			break;
		case PLUGIN_TYPE::NPC:
			copy.assign(_npcModList.begin(), _npcModList.end());
			break;
		case PLUGIN_TYPE::OBJECT:
			copy.assign(_staticModList.begin(), _staticModList.end());
			break;
		case PLUGIN_TYPE::CELL:
			copy.assign(_cellModList.begin(), _cellModList.end());
			break;
		case PLUGIN_TYPE::ALL:
			copy.assign(_modList.begin(), _modList.end());
			break;
		}

		if (a_sortType == SORT_TYPE::ALPHABETICAL) {
			std::sort(copy.begin(), copy.end(), CaseInsensitiveCompareTESFile);
		} else if (a_sortType == SORT_TYPE::COMPILEINDEX_ASC) {
			std::sort(copy.begin(), copy.end(), CompileIndexCompareTESFileAsc);
		} else if (a_sortType == SORT_TYPE::COMPILEINDEX_DESC) {
			std::sort(copy.begin(), copy.end(), CompileIndexCompareTESFileDesc);
		}

		return copy;
	}

	// Returns an alphabetically sorted vector of plugin names.
	//
	// @return std::vector<std::string> - A vector of plugin names.
	std::vector<std::string> Data::GetSortedListOfPluginNames()
	{
		std::vector<std::string> modList;
		for (auto& mod : _modListSorted) {
			modList.push_back(mod);
		}
		return modList;
	}

	bool Data::IsFormTypeInPlugin(const RE::TESFile* a_plugin, RE::FormType a_formType)
	{
		if (a_plugin == nullptr) {
			return false;
		}

		switch (a_formType) {
		case RE::FormType::Armor:
			return _itemListModFormTypeMap[a_plugin].armor;
		case RE::FormType::Book:
			return _itemListModFormTypeMap[a_plugin].book;
		case RE::FormType::Weapon:
			return _itemListModFormTypeMap[a_plugin].weapon;
		case RE::FormType::Misc:
			return _itemListModFormTypeMap[a_plugin].misc;
		case RE::FormType::KeyMaster:
			return _itemListModFormTypeMap[a_plugin].key;
		case RE::FormType::Ammo:
			return _itemListModFormTypeMap[a_plugin].ammo;
		case RE::FormType::AlchemyItem:
			return _itemListModFormTypeMap[a_plugin].alchemy;
		case RE::FormType::Ingredient:
			return _itemListModFormTypeMap[a_plugin].ingredient;
		case RE::FormType::Scroll:
			return _itemListModFormTypeMap[a_plugin].scroll;
		case RE::FormType::SoulGem:
			return _itemListModFormTypeMap[a_plugin].misc;
		case RE::FormType::Static:
			return _itemListModFormTypeMap[a_plugin].staticObject;
		case RE::FormType::Tree:
			return _itemListModFormTypeMap[a_plugin].tree;
		case RE::FormType::Activator:
			return _itemListModFormTypeMap[a_plugin].activator;
		case RE::FormType::Container:
			return _itemListModFormTypeMap[a_plugin].container;
		case RE::FormType::Door:
			return _itemListModFormTypeMap[a_plugin].door;
		case RE::FormType::Light:
			return _itemListModFormTypeMap[a_plugin].light;
		case RE::FormType::Furniture:
			return _itemListModFormTypeMap[a_plugin].furniture;
		case RE::FormType::NPC:
			return _itemListModFormTypeMap[a_plugin].npc;
		case RE::FormType::Cell:
			return _itemListModFormTypeMap[a_plugin].cell;
		default:
			stl::report_and_fail("Invalid FormType passed to IsFormTypeInPlugin");
		}
	}

	// Returns an alphabetically sorted vector of plugin names that are cross-compared to the supplied
	// ItemFilterType filter selected. Primarily used to populate the "Filter By Modlist" list in the
	// table-view modules.
	//
	// @param a_selectedFilter - The selected filter to cross-compare against. (`RE::FormType`)
	// @param a_sortType - The sort type to use when sorting the list. (`SORT_TYPE`)
	// @param a_primaryFilter - The primary filter to use when selecting the list. (`RE::FormType`)
	// @return std::vector<std::string> - A vector of plugin names that match the selected filter.
	std::vector<std::string> Data::GetFilteredListOfPluginNames(PLUGIN_TYPE a_primaryFilter, SORT_TYPE a_sortType, RE::FormType a_secondaryFilter)
	{
		std::vector<const RE::TESFile*> masterList = GetModulePluginListSorted(a_primaryFilter, a_sortType);
		std::vector<std::string> modList;
		const auto& blacklist = PersistentData::GetBlacklist();

		//for (auto& modName : _modListSorted) {
		for (auto& mod : masterList) {
			auto modName = Modex::ValidateTESFileName(mod);

			if (blacklist.contains(mod)) {
				continue;
			}

			if (a_secondaryFilter == RE::FormType::None) {
				modList.push_back(modName);
			} else {
				auto plugin = RE::TESDataHandler::GetSingleton()->LookupModByName(modName.c_str());
				auto pluginFormTypeFlag = _itemListModFormTypeMap[plugin];

				// Only applicable in modules that utilize a secondary filter.
				// (e.g. AddItemWindow, ObjectWindow).

				switch (a_secondaryFilter) {
				case RE::FormType::Armor:
					if (pluginFormTypeFlag.armor) {
						modList.push_back(modName);
					}
					break;
				case RE::FormType::Book:
					if (pluginFormTypeFlag.book) {
						modList.push_back(modName);
					}
					break;
				case RE::FormType::Weapon:
					if (pluginFormTypeFlag.weapon) {
						modList.push_back(modName);
					}
					break;
				case RE::FormType::Misc:
					if (pluginFormTypeFlag.misc) {
						modList.push_back(modName);
					}
					break;
				case RE::FormType::KeyMaster:
					if (pluginFormTypeFlag.key) {
						modList.push_back(modName);
					}
					break;
				case RE::FormType::Ammo:
					if (pluginFormTypeFlag.ammo) {
						modList.push_back(modName);
					}
					break;
				case RE::FormType::AlchemyItem:
					if (pluginFormTypeFlag.alchemy) {
						modList.push_back(modName);
					}
					break;
				case RE::FormType::Ingredient:
					if (pluginFormTypeFlag.ingredient) {
						modList.push_back(modName);
					}
					break;
				case RE::FormType::Scroll:
					if (pluginFormTypeFlag.scroll) {
						modList.push_back(modName);
					}
					break;
				case RE::FormType::SoulGem:
					if (pluginFormTypeFlag.misc) {
						modList.push_back(modName);
					}
					break;
				case RE::FormType::Static:
					if (pluginFormTypeFlag.staticObject) {
						modList.push_back(modName);
					}
					break;
				case RE::FormType::Tree:
					if (pluginFormTypeFlag.tree) {
						modList.push_back(modName);
					}
					break;
				case RE::FormType::Activator:
					if (pluginFormTypeFlag.activator) {
						modList.push_back(modName);
					}
					break;
				case RE::FormType::Container:
					if (pluginFormTypeFlag.container) {
						modList.push_back(modName);
					}
					break;
				case RE::FormType::Door:
					if (pluginFormTypeFlag.door) {
						modList.push_back(modName);
					}
					break;
				case RE::FormType::Light:
					if (pluginFormTypeFlag.light) {
						modList.push_back(modName);
					}
					break;
				default:
					stl::report_and_fail("Invalid FormType passed to GetFilteredListOfPlugins");
					break;
				}
			}
		}

		return modList;
	}
}