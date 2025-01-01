#pragma once

#include "Data.h"
#include "Settings.h"

// #include "Console.h"

namespace Modex
{
	class VisualizationWindow
	{
	public:
		static void Draw(Settings::Style& a_style, Settings::Config& a_config);
		static void ShowPlot(Settings::Style& a_style, Settings::Config& a_config);
	};
}