#include "include/B/Blacklist.h"
#include "include/D/Data.h"
#include "include/P/Persistent.h"
#include "include/U/Util.h"

namespace Modex
{

	void Blacklist::Draw(float a_offset)
	{
		(void)a_offset;

		const auto pluginList = Data::GetSingleton()->GetModulePluginListSorted(Data::PLUGIN_TYPE::ALL);
		const auto blacklist = PersistentData::GetSingleton()->m_blacklist;

		if (ImGui::BeginChild("##Blacklist::CompareArea", ImVec2(0, 0), true, ImGuiWindowFlags_NoFocusOnAppearing)) {
			// Do stuff here before the two boxes:

			// Left Box
			ImGui::BeginChild("##Blacklist::LeftBox", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0), true, ImGuiWindowFlags_NoFocusOnAppearing);
			{
				const float center_x = ImGui::GetCenterTextPosX("Whitelist");
				ImGui::SetCursorPosX(center_x);
				ImGui::Text("Whitelist");
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

				for (const auto& plugin : pluginList) {
					auto pluginName = ValidateTESFileName(plugin);

					if (blacklist.find(plugin) != blacklist.end()) {
						continue;
					}

					if (ImGui::Selectable(pluginName.c_str())) {
						PersistentData::GetSingleton()->AddModToBlacklist(plugin);
					}
				}
			}

			ImGui::EndChild();

			// Right Box

			ImGui::SameLine();

			ImGui::BeginChild("##Blacklist::RightBox", ImVec2(ImGui::GetContentRegionAvail().x, 0), true, ImGuiWindowFlags_NoFocusOnAppearing);
			{
				ImGui::Text("Blacklist");
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

				for (const auto& plugin : pluginList) {
					auto pluginName = ValidateTESFileName(plugin);

					// if (std::find(blacklist.begin(), blacklist.end(), pluginName) == blacklist.end()) {
					// 	continue;
					// }

					if (blacklist.find(plugin) == blacklist.end()) {
						continue;
					}

					if (ImGui::Selectable(pluginName.c_str())) {
						PersistentData::GetSingleton()->RemoveModFromBlacklist(plugin);
					}
				}
			}

			ImGui::EndChild();
		}

		ImGui::EndChild();
	}
}