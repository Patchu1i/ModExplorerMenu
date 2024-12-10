#include "Persistent.h"

#include "Objects.h"
#include <fstream>
#include <nlohmann/json.hpp>

namespace ModExplorerMenu
{

	std::vector<std::string> PersistentData::LoadFromFile()
	{
		std::ifstream file(std::wstring(json_favorite_path) + L"favorites.json");
		if (!file.is_open()) {
			stl::report_and_fail("Unable to open file for writing JSON");
		}

		nlohmann::json json;
		file >> json;
		file.close();

		return json["favorites"].get<std::vector<std::string>>();
	}
}