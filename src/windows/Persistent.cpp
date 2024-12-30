#include "Persistent.h"

#include "Objects.h"
#include <fstream>
#include <nlohmann/json.hpp>

namespace ModExplorerMenu
{

	std::unordered_map<std::string, bool> PersistentData::m_favorites;

	void PersistentData::LoadFromFile()
	{
		std::ifstream file(std::wstring(json_favorite_path) + L"userdata.json");
		if (!file.is_open()) {
			//stl::report_and_fail("Unable to open file for writing JSON");
			return;
		}

		// TODO: Check if the file is empty.
		// This causes a crash if the file is not empty.?.
		file.seekg(0, std::ios::end);
		if (file.tellg() == 0) {
			file.close();
			return;
		}

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