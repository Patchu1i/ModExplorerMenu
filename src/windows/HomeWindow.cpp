#include "HomeWindow.h"

#include "lib/Graphic.h"

void HomeWindow::Draw()
{
	auto _fontSmall = GraphicManager::GetFont("Coolvetica");
	auto _fontBig = GraphicManager::GetFont("Coolvetica");

	ImGui::PushFont(_fontBig.large);
	ImGui::Text("Welcome to the Home Window!");
	ImGui::PopFont();

	ImGui::PushFont(_fontSmall.tiny);
	ImGui::Text("Small Font");
	ImGui::PopFont();
}

void HomeWindow::Init()
{
	// Open = true;
}
