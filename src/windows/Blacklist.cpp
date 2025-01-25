#include "include/B/Blacklist.h"
#include "include/D/Data.h"
#include "include/I/ISearch.h"
#include "include/P/Persistent.h"
#include "include/U/Util.h"

namespace Modex
{

	void Blacklist::Draw(float a_offset)
	{
		(void)a_offset;

		const auto pluginList = Data::GetSingleton()->GetModulePluginListSorted(Data::PLUGIN_TYPE::ALL);
		const auto blacklist = PersistentData::GetSingleton()->m_blacklist;

		totalPlugins = static_cast<int>(pluginList.size());
		blacklistedPlugins = static_cast<int>(blacklist.size());
		nonBlacklistedPlugins = totalPlugins - blacklistedPlugins;

		if (ImGui::BeginChild("##Blacklist::CompareArea", ImVec2(0, 0), true, ImGuiWindowFlags_NoFocusOnAppearing)) {
			ImGui::SubCategoryHeader(_T("Refine Plugins Shown Below"));
			ImGui::NewLine();
			ImGui::BeginColumns("##Blacklist::Column", 2, ImGuiOldColumnFlags_NoBorder);
			ImGui::Indent();

			// FormType Filter Box.
			ImGui::Text("Only show plugins with the following FormType(s):");
			const auto primary_filter_text = RE::FormTypeToString(primaryFilter);
			if (ImGui::BeginCombo("##Blacklist::PluginType", primary_filter_text.data())) {
				if (ImGui::Selectable(_T("None"), primaryFilter == RE::FormType::None)) {
					updateHidden = true;
					hiddenPlugins = 0;
					primaryFilter = RE::FormType::None;
					ImGui::SetItemDefaultFocus();
				}

				for (auto& filter : filterList) {
					bool isSelected = (filter == primaryFilter);

					std::string formName = RE::FormTypeToString(filter).data();
					if (ImGui::Selectable(_T(formName), isSelected)) {
						primaryFilter = filter;
						updateHidden = true;
						hiddenPlugins = 0;
					}

					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			const auto FilterModByRecord = [](const RE::TESFile* mod, RE::FormType formType) {
				if (formType == RE::FormType::None) {
					return true;
				}

				return Data::GetSingleton()->IsFormTypeInPlugin(mod, formType);
			};

			ImGui::NewLine();

			// Plugin Name Fuzzy Search
			ImGui::Text("Prune using Fuzzy Search:");
			// ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 2);
			if (ImGui::InputTextWithHint("##Blacklist::ModSearch", _T("GENERAL_SEARCH_FILTER"), modSearchBuffer, IM_ARRAYSIZE(modSearchBuffer))) {
				updateHidden = true;
				hiddenPlugins = 0;
			}

			std::string pluginFilter = modSearchBuffer;
			std::transform(pluginFilter.begin(), pluginFilter.end(), pluginFilter.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

			ImGui::NewLine();

			ImGui::Unindent();

			ImGui::NextColumn();

			ImGui::Indent();

			// Plugin Count
			ImGui::Text("Total Plugins: %d", totalPlugins);
			ImGui::Text("Blacklisted: %d", blacklistedPlugins);
			ImGui::Text("Non-Blacklisted: %d", nonBlacklistedPlugins);

			if (hiddenPlugins > 0) {
				ImGui::TextColored(ImVec4(0.9f, 0.1f, 0.1f, 1.0f), "Hidden: %d", hiddenPlugins);
				ImGui::TextColored(ImVec4(0.9f, 0.1f, 0.1f, 1.0f), "Some plugins are hidden due to the current filter settings (!)");
			} else {
				ImGui::Text("Hidden: %d", hiddenPlugins);
			}

			ImGui::Unindent();

			ImGui::EndColumns();

			// Left and Right Sections
			ImGui::BeginChild("##Blacklist::LeftBox", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0), true, ImGuiWindowFlags_NoFocusOnAppearing);
			{
				ImGui::SubCategoryHeader(_T("Whitelist"));

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				ImGui::NewLine();

				for (const auto& plugin : pluginList) {
					auto pluginName = ValidateTESFileName(plugin);
					auto pluginNameLower = pluginName;

					std::transform(pluginNameLower.begin(), pluginNameLower.end(), pluginNameLower.begin(),
						[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

					if (pluginNameLower.find(pluginFilter) == std::string::npos) {
						if (updateHidden) {
							hiddenPlugins++;
						}
						continue;
					}

					if (!FilterModByRecord(plugin, primaryFilter)) {
						if (updateHidden) {
							hiddenPlugins++;
						}
						continue;
					}

					if (blacklist.find(plugin) != blacklist.end()) {
						continue;
					}

					if (ImGui::Selectable(pluginName.c_str())) {
						PersistentData::GetSingleton()->AddModToBlacklist(plugin);
					}
				}
			}

			ImGui::EndChild();

			ImGui::SameLine(0.0f, -1.0f);

			ImGui::BeginChild("##Blacklist::RightBox", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f), true, ImGuiWindowFlags_NoFocusOnAppearing);
			{
				ImGui::SubCategoryHeader(_T("Blacklist"));
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				ImGui::NewLine();

				for (const auto& plugin : pluginList) {
					auto pluginName = ValidateTESFileName(plugin);
					auto pluginNameLower = pluginName;

					std::transform(pluginNameLower.begin(), pluginNameLower.end(), pluginNameLower.begin(),
						[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

					if (pluginNameLower.find(pluginFilter) == std::string::npos) {
						continue;
					}

					if (!FilterModByRecord(plugin, primaryFilter)) {
						continue;
					}

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

		updateHidden = false;
	}

	void Blacklist::Init()
	{
		hiddenPlugins = 0;
		updateHidden = true;
		selectedMod = "All Mods";
	}
}