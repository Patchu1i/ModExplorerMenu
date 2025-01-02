#include "Quest.h"
#include "Utils/Util.h"

namespace Modex
{
	void QuestWindow::ApplyFilters()
	{
		questList.clear();
		selectedQuest = nullptr;

		auto& cachedQuestList = Data::GetQuestList();

		std::string compareString;
		std::string inputString = inputBuffer;
		std::transform(inputString.begin(), inputString.end(), inputString.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

		// TODO: Implement additional columns
		for (auto& quest : cachedQuestList) {
			switch (searchKey) {
			case BaseColumn::ID::Name:
				compareString = quest.GetName();
				break;
			case BaseColumn::ID::FormID:
				compareString = quest.GetFormID();
				break;
			case BaseColumn::ID::EditorID:
				compareString = quest.GetEditorID();
				break;
			default:
				compareString = quest.GetName();
				break;
			}

			std::transform(compareString.begin(), compareString.end(), compareString.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

			// If the input is wrapped in quotes, we do an exact match across all parameters.
			if (!inputString.empty() && inputString.front() == '"' && inputString.back() == '"') {
				std::string match = inputString.substr(1, inputString.size() - 2);

				if (compareString == match) {
					questList.push_back(&quest);
				}
				continue;
			}

			if (selectedMod == "Favorite" && !quest.IsFavorite()) {
				continue;
			}

			if (selectedMod != "All Mods" && selectedMod != "Favorite" && quest.GetPluginName() != selectedMod) {
				continue;
			}

			if (quest.GetName() == "")
				continue;

			if (compareString.find(inputString) != std::string::npos) {
				questList.push_back(&quest);
				continue;
			}
		}

		dirty = true;
	}

	void QuestWindow::Refresh()
	{
		ApplyFilters();
	}

}