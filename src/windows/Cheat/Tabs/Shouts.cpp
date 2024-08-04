#include "Console.h"
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
		ImGui::SameLine();
		if (ImGui::InputInt("##DragonSoulAmount", &iDragonSouls)) {
			actor->SetActorValue(RE::ActorValue::kDragonSouls, (float)iDragonSouls);
		}
		ImGui::SameLine();
		if (ImGui::Button("Set")) {
			actor->SetActorValue(RE::ActorValue::kDragonSouls, (float)iDragonSouls);
		}

		ImGui::NewLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		ImGui::NewLine();

		ImGui::Text("Shouts:");
		ImGui::NewLine();

		//ImGuiContext& g = *ImGui::GetCurrentContext();
		//ImGuiTable* table = g.CurrentTable;

		if (ImGui::BeginTable("##ShoutTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
			ImGui::TableSetupColumn("Shout");
			ImGui::TableSetupColumn("First");
			ImGui::TableSetupColumn("Second");
			ImGui::TableSetupColumn("Third");
			ImGui::TableHeadersRow();

			// Hovered
			auto curRow = ImGui::TableGetHoveredRow();
			auto curCol = ImGui::TableGetHoveredColumn();

			auto isKnown = [](RE::TESShout* shout) {
				return shout->GetKnown();
			};

			for (RE::TESShout* shout : shouts) {
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Selectable(shout->GetFullName(), isKnown(shout) || (curCol >= 0) && (curRow == ImGui::TableGetRowIndex()));

				shout->GetKnown();

				int wordNum = 0;
				for (auto var : shout->variations) {
					wordNum++;

					ImGui::TableNextColumn();
					if (ImGui::Selectable(var.word->GetName(), isKnown(shout) || (curCol >= wordNum) && (curRow == ImGui::TableGetRowIndex()))) {
						//std::string formid = std::format("0{:x}", shout->formID);

						for (int i = 0; i < wordNum; i++) {
							//logger::info("shout word: {}", shout->variations[i].word->fullName);
							auto wordID = shout->variations[i].word->formID;
							auto stringID = std::format("0{:x}", wordID);
							logger::info("shout1 formid: {}", stringID);
							Console::SendConsoleCommand("player.teachword " + stringID);
							Console::SendConsoleCommand("player.unlockword " + stringID);
						}
						// Console::SendConsoleCommand("player.teachword " + std::to_string(shout->formID));
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