#include "include/H/Home.h"
#include "include/G/Graphic.h"

namespace Modex
{

	void HomeWindow::Draw()
	{
		ImGui::Text("I had planned on utilizing this page, but for time sake, I'm going to leave it blank for now.");
		ImGui::NewLine();
		ImGui::Text("Check back on future updates!");
		ImGui::Text("\x01\x01SE\"\x02");

		auto test = MultiByteToWideChar(CP_UTF8, 0, "\x01\x01SE\"\x02", -1, NULL, 0);
		ImGui::Text("Test: %d", test);

		ImGui::Selectable("\x01\x01SE\"\x02", false);
	}
	void HomeWindow::Init()
	{
		// Nothing for now.
	}
}
