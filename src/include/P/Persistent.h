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

		static void AddCellToFavorite(const std::string& a_editorid)
		{
			auto& favorites = GetSingleton()->m_teleport_favorites;

			// Check if the item is already in the list
			for (const auto& favorite : favorites) {
				if (favorite == a_editorid) {
					return;  // Already in favorites, no need to add again
				}
			}

			// If not found, add a new item to the front
			favorites.emplace_front(a_editorid);
		}

		static void RemoveCellFromFavorite(const std::string& a_editorid)
		{
			auto& favorites = GetSingleton()->m_teleport_favorites;

			// Find and remove the item from the list
			for (auto it = favorites.begin(); it != favorites.end(); ++it) {
				if (*it == a_editorid) {
					favorites.erase(it);
					return;  // Item found and removed
				}
			}
		}

		static bool IsCellFavorite(const std::string& a_editorid)
		{
			const auto& favorites = GetSingleton()->m_teleport_favorites;

			// Check if the item is in the favorites list
			for (const auto& favorite : favorites) {
				if (favorite == a_editorid) {
					return true;  // Item is a favorite
				}
			}

			return false;  // Item not found in favorites
		}

		template <typename DataType>
		static void AddRecentItem(const std::string& a_editorid, uint32_t a_refID)
		{
			std::list<std::pair<std::string, std::uint32_t>>* recently_used;

			if (std::is_same_v<DataType, ItemData>) {
				recently_used = &GetSingleton()->m_additem_recently_used;
			} else if (std::is_same_v<DataType, NPCData>) {
				recently_used = &GetSingleton()->m_npc_recently_used;
			} else if (std::is_same_v<DataType, ObjectData>) {
				recently_used = &GetSingleton()->m_object_recently_used;
			} else if (std::is_same_v<DataType, CellData>) {
				recently_used = &GetSingleton()->m_teleport_recently_used;
			} else {
				return;  // Unsupported type
			}

			if (recently_used == nullptr) {
				return;  // No list to add to
			}

			// Check if the item is already in the list
			for (auto it = recently_used->begin(); it != recently_used->end(); ++it) {
				if (it->first == a_editorid) {
					// If found, update the refID and move to the front
					it->second = a_refID;
					recently_used->splice(recently_used->begin(), *recently_used, it);
					return;
				}
			}

			// If not found, add a new item to the front
			recently_used->emplace_front(a_editorid, a_refID);

			// If the list exceeds a certain size, remove the oldest item
			if (recently_used->size() > 21) {  // Example size limit
				recently_used->pop_back();     // Remove the oldest item
			}
		}

		template <typename DataType>
		static void ClearRecentItems()
		{
			if (std::is_same_v<DataType, ItemData>) {
				GetSingleton()->m_additem_recently_used.clear();
			} else if (std::is_same_v<DataType, NPCData>) {
				GetSingleton()->m_npc_recently_used.clear();
			} else if (std::is_same_v<DataType, ObjectData>) {
				GetSingleton()->m_object_recently_used.clear();
			} else if (std::is_same_v<DataType, CellData>) {
				GetSingleton()->m_teleport_recently_used.clear();
			} else {
				return;  // Unsupported type
			}
		}

		template <typename DataType>
		static void GetRecentItems(std::vector<std::pair<std::string, std::uint32_t>>& a_out)
		{
			std::list<std::pair<std::string, std::uint32_t>>* recently_used;

			// auto& recently_used = GetSingleton()->m_additem_recently_used;

			if (std::is_same_v<DataType, ItemData>) {
				recently_used = &GetSingleton()->m_additem_recently_used;
			} else if (std::is_same_v<DataType, NPCData>) {
				recently_used = &GetSingleton()->m_npc_recently_used;
			} else if (std::is_same_v<DataType, ObjectData>) {
				recently_used = &GetSingleton()->m_object_recently_used;
			} else if (std::is_same_v<DataType, CellData>) {
				recently_used = &GetSingleton()->m_teleport_recently_used;
			} else {
				return;
			}

			if (recently_used == nullptr) {
				return;  // No list to retrieve from
			}

			a_out.clear();

			for (const auto& pair : *recently_used) {
				a_out.push_back(pair);
			}
		}

		// Can set this up as a template function for additional types in the future.
		static void GetFavoriteItems(std::list<std::string>& a_out)
		{
			auto& favorites = GetSingleton()->m_teleport_favorites;

			a_out.clear();
			
			for (const auto& favorite : favorites) {
				a_out.push_back(favorite);
			}
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
		std::list<std::pair<std::string, std::uint32_t>> m_additem_recently_used;
		std::list<std::pair<std::string, std::uint32_t>> m_npc_recently_used;
		std::list<std::pair<std::string, std::uint32_t>> m_object_recently_used;
		std::list<std::pair<std::string, std::uint32_t>> m_teleport_recently_used;
		std::list<std::string> m_teleport_favorites;

		const std::string json_user_path = "Data/Interface/Modex/User/";
	};

}