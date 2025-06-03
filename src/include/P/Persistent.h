#pragma once

#include "include/D/DataTypes.h"
#include <PCH.h>

namespace Modex
{
	typedef std::unordered_map<std::string, Kit> Collection;

	// TODO: Write a proper class with some helper functions to aid in code readability, jesus.
	class PersistentData
	{
	public:
		static inline PersistentData* GetSingleton()
		{
			static PersistentData singleton;
			return std::addressof(singleton);
		}

		void LoadBlacklist();
		void AddPluginToBlacklist(const RE::TESFile* mod);
		void RemovePluginFromBlacklist(const RE::TESFile* mod);

		void LoadUserdata();
		void SaveUserdata();

		void LoadKit(const std::string& a_name);
		void LoadAllKits();
		void SaveKitToJSON(const Kit& a_kit);
		void DeleteKit(const Kit& a_kit);
		void DeleteKit(const std::string& a_name);
		Kit RenameKit(Kit& a_kit, const std::string& a_new_name);
		Kit CopyKit(const Kit& a_kit);

		static inline std::unordered_set<const RE::TESFile*>& GetBlacklist()
		{
			return GetSingleton()->m_blacklist;
		}

		static inline Collection& GetLoadedKits()
		{
			return GetSingleton()->m_kits;
		}

		template <typename T>
		static inline T GetUserdata(const std::string& a_key, const T& a_default)
		{
			auto& userdata = GetSingleton()->m_userdata;

			if (userdata.find(a_key) == userdata.end()) {
				userdata[a_key] = a_default;
			}

			return std::any_cast<T>(userdata[a_key]);
		}

		template <typename T>
		static void SetUserdata(const std::string& a_key, const T& a_value)
		{
			GetSingleton()->m_userdata[a_key] = a_value;
		}

		// Handles conversion from KitItem to ItemData
		static inline std::vector<ItemData> GetKitItems(const std::string& a_name)
		{
			std::vector<ItemData> items;

			if (GetLoadedKits().find(a_name) != GetLoadedKits().end()) {
				for (auto& item : GetLoadedKits().at(a_name).items) {
					RE::TESForm* form = RE::TESForm::LookupByEditorID(item->editorid);

					if (form == nullptr) {
						continue;
					}

					items.push_back(ItemData(form, 0));
				}
			}

			return items;
		}

		static inline std::vector<std::string> GetLoadedKitNames()
		{
			std::vector<std::string> names;
			for (auto& [name, kit] : GetLoadedKits()) {
				names.emplace_back(name);
			}

			if (!names.empty()) {
				std::sort(names.begin(), names.end());
			}

			return names;
		}

	private:
		std::unordered_set<const RE::TESFile*> m_blacklist;
		Collection m_kits;
		std::unordered_map<std::string, std::any> m_userdata;

		const std::string json_user_path = "Data/Interface/Modex/User/";
	};

}