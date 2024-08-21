#include "Console.h"
#include "Utils/Util.h"
#include "Windows/Cheat/Cheat.h"

namespace ModExplorerMenu
{
	void CheatWindow::ShowShouts()
	{
		auto* player = RE::PlayerCharacter::GetSingleton();
		auto* actor = player->AsActorValueOwner();

		if (actor == nullptr) {
			return;
		}

		ImGui::NewLine();

		ImGui::Text("Dragon Souls:");
		ImGui::NewLine();
		ImGui::InputInt("##DragonSoulAmount", &iDragonSouls);
		ImGui::SameLine();
		if (ImGui::Button("Set")) {
			actor->SetActorValue(RE::ActorValue::kDragonSouls, (float)iDragonSouls);
		}
		ImGui::SameLine();
		if (ImGui::Button("Refresh")) {
			iDragonSouls = (int)actor->GetActorValue(RE::ActorValue::kDragonSouls);
		}

		ImGui::NewLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		ImGui::NewLine();

		ImGui::Text("Shouts:");
		ImGui::NewLine();

		constexpr auto flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;
		auto size = ImGui::GetContentRegionAvail();

		if (ImGui::BeginTable("##ShoutTable", 4, flags, size)) {
			ImGui::TableSetupColumn("Shout");
			ImGui::TableSetupColumn("First");
			ImGui::TableSetupColumn("Second");
			ImGui::TableSetupColumn("Third");
			ImGui::TableHeadersRow();

			// Hovered
			auto curRow = ImGui::TableGetHoveredRow();
			auto curCol = ImGui::TableGetHoveredColumn();

			auto isShoutKnown = [](RE::TESShout* shout) {
				return shout->GetKnown();
			};

			auto isWordKnown = [](RE::TESWordOfPower* word) {
				return word->GetKnown();
			};

			for (RE::TESShout* shout : shouts) {
				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				bool shoutIsKnown = isShoutKnown(shout);
				ImGui::Selectable(shout->GetFullName(), shoutIsKnown || (curCol >= 0) && (curRow == ImGui::TableGetRowIndex()));

				if (shoutIsKnown) {
					ImGui::SelectableColor(IM_COL32(25, 255, 25, 50));
				}

				int wordNum = 0;
				for (auto var : shout->variations) {
					wordNum++;

					ImGui::TableNextColumn();
					bool isKnown = shoutIsKnown && isWordKnown(var.word);
					if (ImGui::Selectable(var.word->GetName(), isKnown || (curCol >= wordNum) && (curRow == ImGui::TableGetRowIndex()))) {
						for (int i = 0; i < wordNum; i++) {
							auto wordID = shout->variations[i].word->formID;
							auto stringID = std::format("0{:x}", wordID);
							logger::info("shout1 formid: {}", stringID);
							Console::SendConsoleCommand("player.teachword " + stringID);
							Console::SendConsoleCommand("player.unlockword " + stringID);
						}
					}

					if (isKnown) {
						ImGui::SelectableColor(IM_COL32(25, 255, 25, 50));
					}
				}
			}

			ImGui::EndTable();
		}

		// int count = 0;
		// if (ImGui::BeginChild("##ShoutsWindow")) {
		// 	ImGui::PushID("Shouts");
		// 	for (RE::TESShout* shout : shouts) {
		// 		count++;
		// 		if (shout == nullptr) {
		// 			continue;
		// 		}

		// 		int wordCount = 0;
		// 		for (auto var : shout->variations) {
		// 			if (var.word) {
		// 				wordCount++;

		// 				if (strcmp(var.word->GetFullName(), "DragonFire01") == 0) {
		// 					wordCount = 0;
		// 					break;
		// 				}
		// 			}
		// 		}

		// 		if (wordCount < 3) {
		// 			continue;
		// 		}

		// 		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		// 		ImGui::NewLine();
		// 		if (ImGui::Button(shout->GetFullName())) {
		// 			logger::info("shout formid: {}", std::format("{:x}", shout->formID));
		// 			// Console::SendConsoleCommand("player.teachword " + std::to_string(shout->formID));
		// 		}

		// 		for (auto var : shout->variations) {
		// 			if (var.word) {
		// 				ImGui::SameLine();
		// 				ImGui::Text(var.word->GetName());
		// 				ImGui::Text(std::format("{:x}", var.word->formID).c_str());
		// 			}
		// 		}
		// 		ImGui::NewLine();
		// 	}
		// 	ImGui::PopID();
		// }
		// ImGui::EndChild();
	}

}

// if (ImGui::Button("##TestShout" + name)) {
// 	player->AddShout(shout);
// }

// ImGui::BeginChild("##ShoutVariationsWindow");
// for (auto var : shout->variations) {
// 	if (var.word) {
// 		ImGui::SameLine();
// 		auto formid = var.spell;
// 		if (formid) {
// 			if (ImGui::Button("##Test" + formid->GetName())) {
// 				logger::info("shout formid: {}", shout->formID);
// 				logger::info("word formid: {}", var.word->formID);
// 				//Console::SendConsoleCommand("player.teachword " + std::to_string(shout->formID) + " " + std::to_string(var.word->formID));
// 				//player->AddShout(shout);
// 				//player->AddSpell(formid);
// 			}
// 		}
// 	}
// }
// ImGui::EndChild();