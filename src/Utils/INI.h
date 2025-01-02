#pragma once

namespace IniHelper
{
	enum class iComment
	{
		MasterPresetHeader,
		PresetMainHeader,
		PresetMainModules,
		ThemeConfigHeader,
		ThemeImageHeader,
		ThemeTextHeader,
		ThemeTableHeader,
		ThemeWidgetsHeader
	};

	std::map<iComment, const char*> comment_map = {
		{ iComment::MasterPresetHeader,
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n"
			"# # Master Configuration\n"
			"#\n"
			"# This file is used to store variables across sesssions\n"
			"# You should not have to change anything in here. Most of it is handled in-game.\n"
			"#\n"
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n" },
		{ iComment::PresetMainHeader,
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n"
			"# # Preset configuration\n"
			"#\n"
			"# Theme = <filename> (without extention)\n"
			"#\n"
			"# - Themes can be found in the 'Data/Interface/Modex/themes' directory.\n"
			"# - If you're using Mod Organizer 2, custom themes are saved to your overwrite directory.\n"
			"# - Downloaded themes need to be placed in the themes directory to be loaded.\n"
			"#\n"
			"#\n"
			"# ShowMenuKey = <int> (Uses Skyrim Input Codes: https://ck.uesp.net/wiki/Input_Script\n"
			"# ShowMenuModifier = <int>\n"
			"#\n"
			"# Language = <string> (English, Portuguese-BR, Chinese, French, German, Japanese, Korean, Russian, Spanish)\n"
			"#\n"
			"# - Language translation files are located in the 'Data/Interface/Modex/User/Language' directory.\n"
			"# - Languages that require non-latin characters or symbols can be placed in 'Data/Interface/Modex/Fonts/' directory.\n"
			"# - Fonts are automatically loaded based on the language selected and corresponding directory.\n"
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n" },
		{ iComment::PresetMainModules,
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n"
			"# ModListSort = <int> (0 = Default, 1 = Alphabetical, 2 = Install Date)\n"
			"#\n"
			"# - Install Date is only available on Windows. Make sure to set the correct DataPath further down.\n"
			"#\n"
			"#\n"
			"# UI Scale = <int> (80, 90, 100, 110, 120)\n"
			"#\n"
			"#\n"
			"# DefaultShow = <int> (0 = Home, 1 = Add Item, 2 = Object, 3 = NPC, 4 = Teleport, 5 = Settings)\n"
			"#\n"
			"#\n"
			"# Show <> Menu = <bool> (true, false)\n"
			"#\n"
			"# - These settings control which modules are loaded into memory on startup.\n"
			"# - Shouldn't be an issue to keep all enabled, but it's here.\n"
			"#\n"
			"#\n"
			"# DataPath = <string> (Path to your Skyrim Data folder)\n"
			"#\n"
			"# - This is used to determine the install date of mods for sorting.\n"
			"# - Regardless of your Mod Manager, this should point to your *actual* data folder.\n"
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n" },
		{ iComment::ThemeConfigHeader,
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n"
			"# # Theme configuration\n"
			"#\n"
			"# Supported Color formats:\n"
			"# - Hexadecimal: #RRGGBBAA\n"
			"# - RGB: {R,G,B,A} (e.g. {127,127,127,255}) [Do not use decimals]\n"
			"#\n"
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n" },
		{ iComment::ThemeTextHeader,
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n"
			"# # Text configuration\n"
			"#\n"
			"# NoIconText = <bool> (true, false)\n"
			"#\n"
			"# - Removes the icons appended to some text strings.\n"
			"#\n"
			"#\n"
			"# GlobalFontSize = <float> (Default = 1.0)\n"
			"#\n"
			"# - Scales all text globally. Can be used to increase or decrease text size.\n"
			"# - This is a multiplier. Modfifying this may cause alias issues.\n"
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n" },
		{ iComment::ThemeTableHeader,
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n"
			"# # Table & Column configuration\n"
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n" },
		{ iComment::ThemeWidgetsHeader,
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n"
			"# # Buttons, Sliders, & Widgets configuration\n"
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n" },
		{ iComment::ThemeImageHeader,
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n"
			"# # Image configuration\n"
			"#\n"
			"# Supported Image formats:\n"
			"# - PNG\n"
			"#\n"
			"# - Images are loaded from the 'Data/Interface/Modex/images' directory.\n"
			"# - Simply use the filename without the extension to reference the image.\n"
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n" }

	};

	[[nodiscard]] const char* GetComment(iComment a_type)
	{
		return comment_map[a_type];
	}
}