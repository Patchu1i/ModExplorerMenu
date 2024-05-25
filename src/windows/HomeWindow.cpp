#include "HomeWindow.h"

#include "lib/Graphic.h"

void HomeWindow::Draw()
{
    auto _fontSmall = GraphicManager::GetFont("Coolvetica-Small");
    auto _fontBig = GraphicManager::GetFont("Coolvetica-Large");

    ImGui::PushFont(_fontBig);
	ImGui::Text("Welcome to the Home Window!");
    ImGui::PopFont();

    ImGui::PushFont(_fontSmall);
    ImGui::Text("Small Font");
    ImGui::PopFont();
}

void HomeWindow::Init()
{
    // Open = true;
}
