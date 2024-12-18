#pragma once

namespace IniHelper
{
	enum class iComment
	{
		MasterPresetHeader,
		PresetMainHeader,
		PresetAddItemHeader,
		PresetObjectHeader,
		PresetFormLookupHeader,
		PresetNPCHeader,
		PresetTeleportHeader,
		ThemeConfigHeader,
		ThemeFrameHeader,
		ThemeChildHeader,
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
			"# - Themes can be found in the 'Data/Interface/ModExplorerMenu/themes' directory.\n"
			"# - If you're using Mod Organizer 2, custom themes are saved to your overwrite directory.\n"
			"# - Downloaded themes need to be placed in the themes directory to be loaded.\n"
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
		{ iComment::ThemeFrameHeader,
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n"
			"# # Frame configuration\n"
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n" },
		{ iComment::ThemeChildHeader,
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n"
			"# # Child window & Popup window configuration\n"
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n" },
		{ iComment::ThemeTextHeader,
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n"
			"# # Text configuration\n"
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n" },
		{ iComment::ThemeTableHeader,
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n"
			"# # Table & Column configuration\n"
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n" },
		{ iComment::ThemeWidgetsHeader,
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n"
			"# # Buttons, Sliders, & Widgets configuration\n"
			"#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n" },

	};

	[[nodiscard]] const char* GetComment(iComment a_type)
	{
		return comment_map[a_type];
	}
}