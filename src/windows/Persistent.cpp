#include "include/P/Persistent.h"
#include "include/D/Data.h"

namespace Modex
{

	// This will create the designated file if it doesn't exist.
	// Loads a JSON file into memory, then returns the JSON object.
	nlohmann::json OpenJSONFile(const std::string& a_path)
	{
		std::ifstream file(a_path);

		if (!file.is_open()) {
			return {};
		}

		// Error prone if trying to parse empty file.
		file.seekg(0, std::ios::end);
		if (file.tellg() == 0) {
			file.close();
			return {};
		}

		file.seekg(0, std::ios::beg);

		nlohmann::json JSON;

		// Handle exceptions here to try and prevent user data from being overwritten
		// or corrupted if there is a parsing issue.

		try {
			file >> JSON;
			file.close();
		} catch (const nlohmann::json::parse_error& e) {
			file.close();
			stl::report_and_fail(std::string("[JSON] Error parsing JSON file:\n\nDid you modify it manually?\n") + a_path + e.what());
		} catch (const nlohmann::json::exception& e) {
			file.close();
			stl::report_and_fail(std::string("[JSON] Error Exception reading JSON file: ") + e.what());
		}

		return JSON;
	}

	// Saves a modified JSON object to a specific file.
	bool SaveJSONFile(const std::string& a_path, const nlohmann::json& a_json)
	{
		std::ofstream file(a_path);

		if (!file.is_open()) {
			return false;
		}

		// Don't need to report_and_fail here, since an exception wouldn't
		// cause any data loss, just a failed JSON save. (?)

		try {
			file << a_json.dump(4);
		} catch (const std::exception& e) {
			logger::warn("Failed to save JSON file: {}", e.what());
			return false;
		}

		file.close();
		return true;
	}

	// Loads the blacklist JSON into the blacklist set.
	void PersistentData::LoadBlacklist()
	{
		nlohmann::json JSON = OpenJSONFile(json_user_path + "blacklist.json");

		if (!JSON.contains("Blacklist")) {
			return;
		}

		for (auto& [filename, nil] : JSON["Blacklist"].items()) {
			m_blacklist.insert(RE::TESDataHandler::GetSingleton()->LookupModByName(filename));
		}
	}

	void PersistentData::AddPluginToBlacklist(const RE::TESFile* a_plugin)
	{
		nlohmann::json JSON = OpenJSONFile(json_user_path + "blacklist.json");
		const std::string plugin_name = ValidateTESFileName(a_plugin);

		JSON["Blacklist"][plugin_name] = true;

		if (!m_blacklist.contains(a_plugin)) {
			m_blacklist.insert(a_plugin);
		}

		if (!SaveJSONFile(json_user_path + "blacklist.json", JSON)) {
			logger::warn("Failed to save changes to blacklist.json.");
		}
	}

	void PersistentData::RemovePluginFromBlacklist(const RE::TESFile* a_plugin)
	{
		nlohmann::json JSON = OpenJSONFile(json_user_path + "blacklist.json");
		const std::string plugin_name = ValidateTESFileName(a_plugin);

		if (JSON["Blacklist"].contains(plugin_name)) {
			JSON["Blacklist"].erase(plugin_name);
		}

		if (m_blacklist.contains(a_plugin)) {
			m_blacklist.erase(a_plugin);
		}

		if (!SaveJSONFile(json_user_path + "blacklist.json", JSON)) {
			logger::warn("Failed to save changes to blacklist.json.");
		}
	}

	void PersistentData::LoadAllKits()
	{
		std::filesystem::path path(json_user_path + "/kits");
		for (auto& entry : std::filesystem::directory_iterator(path)) {
			if (entry.is_regular_file() && entry.path().extension() == ".json") {
				LoadKit(entry.path().filename().string());
			}
		}
	}

	Kit PersistentData::CopyKit(const Kit& a_kit)
	{
		Kit new_kit = a_kit;
		new_kit.name += " (Copy)";

		SaveKitToJSON(new_kit);

		// Update runtime kit collection.
		m_kits.emplace(new_kit.name, new_kit);

		return new_kit;
	}

	// Instead of simply renaming, we duplicate the kit into a new JSON file,
	// and then we discard the old kit entirely. This is to ensure that the
	// kit name and address are unique and not shared between two kits.
	Kit PersistentData::RenameKit(Kit& a_kit, const std::string& a_new_name)
	{
		std::string old_name = a_kit.name;

		try {
			if (std::filesystem::exists(json_user_path + "/kits/" + old_name + ".json")) {
				std::filesystem::remove(json_user_path + "/kits/" + old_name + ".json");
			}
		} catch (const std::exception& e) {
			logger::warn("Failed to rename kit: {}", e.what());
		}

		a_kit.name = a_new_name;
		SaveKitToJSON(a_new_name);

		// Update runtime kit collection.
		m_kits.emplace(a_new_name, a_kit);
		m_kits.erase(old_name);

		return a_kit;
	}

	// Kit JSON files should only contain a single kit. Otherwise, bad.
	void PersistentData::DeleteKit(const std::string& a_name)
	{
		try {
			if (std::filesystem::exists(json_user_path + "/kits/" + a_name + ".json")) {
				std::filesystem::remove(json_user_path + "/kits/" + a_name + ".json");
			}

		} catch (const std::exception& e) {
			logger::warn("Failed to delete kit JSON file: {}", e.what());
		}

		m_kits.erase(a_name);
	}

	void PersistentData::DeleteKit(const Kit& a_kit)
	{
		DeleteKit(a_kit.name);
	}

	// TODO: Safety for nonexistent directory or paths
	void PersistentData::LoadKit(const std::string& a_name)
	{
		nlohmann::json JSON = OpenJSONFile(json_user_path + "/kits/" + a_name);
		auto& collection = GetLoadedKits();

		if (JSON.empty()) {
			logger::warn("Failed to load kit JSON file: {}", a_name);
			return;
		}

		for (auto& [kit_name, category] : JSON.items()) {
			Kit kit(kit_name);

			try {
				for (auto& [item_eid, data] : category["Items"].items()) {
					auto item = std::make_shared<KitItem>();
					item->plugin = data["Plugin"].get<std::string>();
					item->name = data["Name"].get<std::string>();
					item->editorid = item_eid;
					item->amount = data["Amount"].get<int>();
					item->equipped = data["Equipped"].get<bool>();

					if (kit.items.contains(item)) {
						logger::warn("Duplicate item found in kit {}. (EditorID: {})", kit_name, item_eid);
						continue;
					}

					kit.items.emplace(item);
				}
			} catch (const nlohmann::json::exception& e) {
				logger::warn("Failed to load items from kit {}. (ERROR:  {})", kit_name, e.what());
				logger::warn("If you manually edited the JSON file, please ensure the format is correct!");
				break;
			}

			// Going to omit the following for now, since they're not implemented.
			// Will add them back in once they're implemented.

			// try {
			// 	for (auto& [spell_eid, data] : category["Spells"].items()) {
			// 		auto spell = std::make_shared<KitSpell>();
			// 		spell->plugin = data["Plugin"].get<std::string>();
			// 		spell->name = data["Name"].get<std::string>();
			// 		spell->editorid = spell_eid;
			// 		kit.spells.emplace(spell);
			// 	}
			// } catch (const nlohmann::json::exception& e) {
			// 	logger::warn("Failed to load spells from kit {}. (ERROR:  {})", kit_name, e.what());
			// 	logger::warn("If you manually edited the JSON file, please ensure the format is correct!");
			// 	break;
			// }

			// try {
			// 	for (auto& [perk_eid, data] : category["Perks"].items()) {
			// 		auto perk = std::make_shared<KitPerk>();
			// 		perk->plugin = data["Plugin"].get<std::string>();
			// 		perk->name = data["Name"].get<std::string>();
			// 		perk->editorid = perk_eid;
			// 		perk->rank = data["Rank"].get<int>();
			// 		kit.perks.emplace(perk);
			// 	}

			// } catch (const nlohmann::json::exception& e) {
			// 	logger::warn("Failed to load perks from kit {}. (ERROR:  {})", kit_name, e.what());
			// 	logger::warn("If you manually edited the JSON file, please ensure the format is correct!");
			// 	break;
			// }

			// try {
			// 	for (auto& [shout_eid, data] : category["Shouts"].items()) {
			// 		auto shout = std::make_shared<KitShout>();
			// 		shout->plugin = data["Plugin"].get<std::string>();
			// 		shout->name = data["Name"].get<std::string>();
			// 		shout->editorid = shout_eid;
			// 		kit.shouts.emplace(shout);
			// 	}
			// } catch (const nlohmann::json::exception& e) {
			// 	logger::warn("Failed to load shouts from kit {}. (ERROR:  {})", kit_name, e.what());
			// 	logger::warn("If you manually edited the JSON file, please ensure the format is correct!");
			// 	break;
			// }

			collection.emplace(kit_name, kit);
		}
	}

	void PersistentData::SaveKitToJSON(const Kit& a_kit)
	{
		nlohmann::json JSON = OpenJSONFile(json_user_path + "/kits/" + a_kit.name + ".json");

		JSON[a_kit.name] = {};
		JSON[a_kit.name]["Items"] = {};
		JSON[a_kit.name]["Spells"] = {};
		JSON[a_kit.name]["Perks"] = {};
		JSON[a_kit.name]["Shouts"] = {};

		for (auto& item : a_kit.items) {
			JSON[a_kit.name]["Items"][item->editorid] = {
				{ "Plugin", item->plugin },
				{ "Name", item->name },
				{ "Amount", item->amount },
				{ "Equipped", item->equipped }
			};
		}

		// Going to omit the following for now, since they're not implemented.
		// Will add them back in once they're implemented.

		// for (auto& spell : a_kit.spells) {
		// 	JSON[a_kit.name]["Spells"][spell->editorid] = {
		// 		{ "Plugin", spell->plugin },
		// 		{ "Name", spell->name }
		// 	};
		// }

		// for (auto& perk : a_kit.perks) {
		// 	JSON[a_kit.name]["Perks"][perk->editorid] = {
		// 		{ "Plugin", perk->plugin },
		// 		{ "Name", perk->name },
		// 		{ "Rank", perk->rank }
		// 	};
		// }

		// for (auto& shout : a_kit.shouts) {
		// 	JSON[a_kit.name]["Shouts"][shout->editorid] = {
		// 		{ "Plugin", shout->plugin },
		// 		{ "Name", shout->name }
		// 	};
		// }

		if (!SaveJSONFile(json_user_path + "/kits/" + a_kit.name + ".json", JSON)) {
			logger::warn("Failed to save changes to kit JSON file.");
		}
	}

}