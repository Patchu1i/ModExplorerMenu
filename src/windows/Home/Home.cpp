#include "Home.h"
#include "Graphic.h"

namespace ModExplorerMenu
{

	void HomeWindow::Draw()
	{
		if (ImGui::Button("Test")) {
			Translate::GetSingleton()->LoadLanguage("English");
		}
	}
	void HomeWindow::Init()
	{
		// Open = true;
	}
}
