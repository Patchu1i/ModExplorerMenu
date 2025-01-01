#include "Visual.h"

namespace Modex
{
	void VisualizationWindow::Draw(Settings::Style& a_style, Settings::Config& a_config)
	{
		(void)a_style;
		(void)a_config;

		ShowPlot(a_style, a_config);

		// ImGui::Begin("Visualization", nullptr, ImGuiOldWindowFlags_NoCollapse | ImGuiOldWindowFlags_NoResize | ImGuiOldWindowFlags_NoMove | ImGuiOldWindowFlags_NoTitleBar);
		// ImGui::SetWindowSize(ImVec2(800, 600));
		// ImGui::SetWindowPos(ImVec2(0, 0));
		// ImGui::End();
	}
}