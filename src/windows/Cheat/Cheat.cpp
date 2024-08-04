#include "Cheat.h"

// If I need to add filtering for items like Interior/Exterior or something, reference this:
// std::vector<std::tuple<bool*, AddItemWindow::ItemType, std::string>> AddItemWindow::_filterMap = {
// 	{ &_Alchemy, AddItemWindow::Alchemy, "ALCH" }, { &_Ingredient, AddItemWindow::Ingredient, "INGR" },
// 	{ &_Ammo, AddItemWindow::Ammo, "AMMO" }, { &_Key, AddItemWindow::Key, "KEYS" },
// 	{ &_Misc, AddItemWindow::Misc, "MISC" }, { &_Armor, AddItemWindow::Armor, "ARMO" },
// 	{ &_Book, AddItemWindow::Book, "BOOK" }, { &_Weapon, AddItemWindow::Weapon, "WEAP" }
// };

namespace ModExplorerMenu
{
	void CheatWindow::Draw(Settings::Style& a_style, Settings::Config& a_config)
	{
		constexpr auto flags = ImGuiOldColumnFlags_NoResize;
		ImGui::BeginColumns("##HorizontalSplit", 2, flags);

		const float width = ImGui::GetWindowWidth();
		ImGui::SetColumnWidth(0, width * 0.25f);

		// Left Column
		ShowSidebar(a_style, a_config);

		// Get ImGui to extend horizontal split to the bottom of the window
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 5.0f);
		ImGui::InvisibleButton("##VerticalSplit", ImVec2(5.0f, 5.0f));

		// Right Column
		ImGui::NextColumn();
		ShowSubMenu(a_style, a_config);
		ImGui::EndColumns();
	}

	void CheatWindow::OnEnterCallback(const Tab::ID& a_id)
	{
		auto* actor = RE::PlayerCharacter::GetSingleton()->AsActorValueOwner();

		if (actor == nullptr) {
			return;
		}

		if (a_id == Tab::ID::kShouts) {
			iDragonSouls = (int)actor->GetActorValue(RE::ActorValue::kDragonSouls);

			auto* handler = RE::TESDataHandler::GetSingleton();
			auto shoutList = handler->GetFormArray<RE::TESShout>();
			auto wordList = handler->GetFormArray<RE::TESWordOfPower>();

			shouts.clear();
			words.clear();

			for (RE::TESShout* shout : shoutList) {
				if (shout == nullptr) {
					continue;
				}

				bool isValidWord = true;
				for (auto var : shout->variations) {
					if (var.word == nullptr) {
						isValidWord = false;
						break;
					}

					if (strlen(var.word->GetFullName()) >= 9) {
						isValidWord = false;
						break;
					}

					if (strlen(var.word->GetFullName()) == 0) {
						isValidWord = false;
						break;
					}
				}

				if (isValidWord == false) {
					continue;
				}

				logger::info("Shout: {}", shout->GetFullName());
				for (auto var : shout->variations) {
					logger::info("Word: {}", var.word->GetFullName());
				}

				logger::info("-----");

				shouts.push_back(shout);
			}

			for (RE::TESWordOfPower* word : wordList) {
				words.push_back(word);
			}
		}
	}

	void CheatWindow::Init()
	{
		//activeTab = &m_shouts;
	}
}