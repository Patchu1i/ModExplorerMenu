#pragma once

#include <PCH.h>
#include <fstream>
#include <nlohmann/json.hpp>
namespace ModExplorerMenu
{

	class PersistentData
	{
	public:
		static inline PersistentData* GetSingleton()
		{
			static PersistentData singleton;
			return &singleton;
		}

		// TODO: Is it a good idea to be dumping changes to the file every time a change is made?
		template <class BaseObject>
		static inline void UpdatePersistentData(BaseObject& obj)
		{
			nlohmann::json json;

			// Read existing data from the file, catch empty file.
			std::ifstream inputFile(std::wstring(json_favorite_path) + L"userdata.json");
			if (inputFile.is_open()) {
				try {
					if (inputFile.peek() != std::ifstream::traits_type::eof()) {
						inputFile >> json;
					}
				} catch (const std::exception& e) {
					stl::report_and_fail(std::string("[JSON] Error reading userdata file during Update: ") + e.what());
				}
				inputFile.close();
			} else {
				// If the file does not exist, create it
				std::ofstream outputFile(std::wstring(json_favorite_path) + L"userdata.json");
				if (!outputFile.is_open()) {
					stl::report_and_fail("[JSON] Unable to create userdata file for Favorites.");
				}
				outputFile.close();
			}

			if (obj->favorite) {
				json["Favorite"][obj->filename][obj->editorid] = true;
			} else {
				if (json["Favorite"].contains(obj->filename) && json["Favorite"][obj->filename].contains(obj->editorid)) {
					json["Favorite"][obj->filename].erase(obj->editorid);

					// Purge empty filenames in JSON.
					if (json["Favorite"][obj->filename].empty()) {
						json["Favorite"].erase(obj->filename);
					}
				}
			}

			// Write the updated JSON data back to the file
			std::ofstream outputFile(std::wstring(json_favorite_path) + L"userdata.json");
			if (outputFile.is_open()) {
				try {
					outputFile << json.dump(4);
				} catch (const std::exception& e) {
					stl::report_and_fail(std::string("[JSON] Error writing userdata file changes: ") + e.what());
				}
				outputFile.close();
			} else {
				stl::report_and_fail("[JSON] Unable to open userdata file for writing.");
			}
		}

		static void LoadFromFile();
		static std::unordered_map<std::string, bool> m_favorites;

		constexpr inline static const wchar_t* json_favorite_path = L"Data/Interface/ModExplorerMenu/User/";
	};

}