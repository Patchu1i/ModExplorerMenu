#include "include/V/Vortex.h"
#include "extern/imnodes.h"
#include "include/D/Data.h"

namespace Modex
{
	void VortexWindow::Draw(float a_offset)
	{
		(void)a_offset;
		// const int hardcoded_node_id = 1;

		const auto pluginList = Data::GetSingleton()->GetModulePluginList(Data::PLUGIN_TYPE::ALL);

		ImNodes::BeginNodeEditor();

		ImNodes::GetStyle().NodeCornerRounding = 0.0f;
		ImNodes::GetStyle().GridSpacing = 12.0f;
		ImNodes::GetStyle().LinkThickness = 3.0f;
		ImNodes::GetStyle().Colors[ImNodesCol_GridBackground] = IM_COL32(5, 5, 5, 255);
		ImNodes::GetStyle().Colors[ImNodesCol_GridLine] = IM_COL32(25, 25, 25, 255);
		ImNodes::GetStyle().Colors[ImNodesCol_GridLinePrimary] = IM_COL32(35, 35, 35, 255);
		ImNodes::GetStyle().Colors[ImNodesCol_NodeBackground] = IM_COL32(22, 22, 22, 255);
		ImNodes::GetStyle().Colors[ImNodesCol_TitleBar] = IM_COL32(22, 22, 22, 255);

		float lastScale = fontScale;
		fontScale += (ImGui::GetIO().MouseWheel / 10.0f);
		ImGui::SetWindowFontScale(fontScale);

		for (auto& plugin : pluginList) {
			if (plugin->compileIndex == (uint32_t)254) {
				continue;
			}
			// logger::info("Plugin Index: {}", plugin->compileIndex);
			ImNodes::BeginNode(plugin->compileIndex);
			const ImVec2 nodePos = ImNodes::GetNodeEditorSpacePos(plugin->compileIndex);
			if (lastScale != fontScale) {
				ImNodes::SetNodeGridSpacePos(plugin->compileIndex, ImVec2(nodePos.x * fontScale, nodePos.y * fontScale));
			}
			ImNodes::BeginNodeTitleBar();
			ImGui::Text(plugin->fileName);
			ImNodes::EndNodeTitleBar();
			ImGui::Dummy(ImVec2(0.0f, 0.0f));
			ImNodes::EndNode();
		}
		// ImNodes::BeginNode(hardcoded_node_id);

		// ImNodes::EndNode();

		ImNodes::MiniMap();
		ImNodes::EndNodeEditor();
	}

	void VortexWindow::Init()
	{
	}
}