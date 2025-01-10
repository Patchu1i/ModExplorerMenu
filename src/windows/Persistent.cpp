#include "include/P/Persistent.h"
#include "include/D/Data.h"

namespace Modex
{

	std::unordered_map<std::string, bool> PersistentData::m_favorites;
	std::unordered_set<const RE::TESFile*> PersistentData::m_blacklist;

	void PersistentData::LoadBlacklist()
	{
		std::ifstream file(std::wstring(json_favorite_path) + L"blacklist.json");
		if (!file.is_open()) {
			return;
		}

		// If the file is empty, don't bother parsing it.
		file.seekg(0, std::ios::end);
		if (file.tellg() == 0) {
			file.close();
			return;
		}

		// Reset pointer to beginning.
		file.seekg(0, std::ios::beg);

		try {
			nlohmann::json json;
			file >> json;
			file.close();

			if (!json.contains("Blacklist")) {
				return;
			}

			for (auto& [filename, nil] : json["Blacklist"].items()) {
				m_blacklist.insert(RE::TESDataHandler::GetSingleton()->LookupModByName(filename));
			}
		} catch (const nlohmann::json::parse_error& e) {
			file.close();
			stl::report_and_fail(std::string("[JSON] Error parsing blacklist file:\n\nDid you modify it manually?\n") + e.what());
		} catch (const nlohmann::json::exception& e) {
			file.close();
			stl::report_and_fail(std::string("[JSON] Error Exception reading blacklist file: ") + e.what());
		}
	}

	void PersistentData::LoadFromFile()
	{
		std::ifstream file(std::wstring(json_favorite_path) + L"userdata.json");
		if (!file.is_open()) {
			return;
		}

		// If the file is empty, don't bother parsing it.
		file.seekg(0, std::ios::end);
		if (file.tellg() == 0) {
			file.close();
			return;
		}

		// Reset pointer to beginning.
		file.seekg(0, std::ios::beg);

		try {
			nlohmann::json json;
			file >> json;
			file.close();

			if (!json.contains("Favorite")) {
				return;
			}

			for (auto& [filename, list] : json["Favorite"].items()) {
				for (auto& [editorid, favorite] : list.items()) {
					m_favorites[editorid] = favorite;
				}
			}
		} catch (const nlohmann::json::parse_error& e) {
			file.close();
			stl::report_and_fail(std::string("[JSON] Error parsing userdata file:\n\nDid you modify it manually?\n") + e.what());
		} catch (const nlohmann::json::exception& e) {
			file.close();
			stl::report_and_fail(std::string("[JSON] Error Exception reading userdata file: ") + e.what());
		}
	}
}