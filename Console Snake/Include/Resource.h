#pragma once
#ifndef SNAKE_RESOURCE_HEADER_
#define SNAKE_RESOURCE_HEADER_

#include "Enum.h"
#include "LocalizedStrings.h"
#include "EncryptedString.h"
#include "WinMacro.h"
#include <Windows.h>

#define GAME_VERSION "pre-2.25"
inline const auto save_file_name = "SnakeSaved.bin"_crypt;
inline constexpr const unsigned char crypto_key[] = {
	0x54, 0xDE, 0x3B, 0xF2, 0xD8, 0x5D, 0x4E, 0x04,
	0xB2, 0xBE, 0x4D, 0xCC, 0xC3, 0xAD, 0xEB, 0x1C
};
inline constexpr const unsigned char crypto_IV[] = {
	0xE9, 0x5C, 0x99, 0x13, 0xCC, 0x94, 0x4A, 0x0C,
	0x92, 0xD1, 0x48, 0x9E, 0x03, 0x9B, 0x4E, 0xA4
};

// --------------- Language Resource ---------------
LANG_DEF(
	en_US,
	zh_CN,
	zh_TW,
	ja_JP
);

TOKEN_DEF(
	console_title,
	title_gaming,
	title_pausing,
	game_version,
	press_any_key,

	menu_start_game,
	menu_setting,
	menu_rank,
	menu_exit,

	setting_speed,
	setting_map_size,
	setting_show_frame,
	setting_theme,
	setting_language,
	setting_save,
	setting_return,
	setting_speed_fast,
	setting_speed_normal,
	setting_speed_slow,
	setting_speed_custom,
	setting_show_frame_yes,
	setting_show_frame_no,

	game_congratulations,
	game_you_win,
	game_you_died,
	game_show_score,
	game_enter_your_name,
	game_Esc_to_return,
	game_Space_to_retry,

	rank_No,
	rank_anonymous,
	rank_win,
	rank_setting,
	rank_no_data,
	rank_clear_all_records,

	about_text,
	about_caption,

	GetConsoleWindow_failed_message,
	message_std_bad_alloc,
	message_init_fail,
	message_init_gamesaving_fail,
	message_init_console_fail,
	message_savefile_not_updated,
	message_process_savedata_fail,
	message_update_savefile_fail,
	message_unknown_error
);

DEF_LOCALIZED_STR();

LANG_DEFAULT(en_US);
MAKE_LOCALIZED_STRS
{
	{

#include "LangENG.inl"

	},
	{

#include "LangCHS.inl"

	},
	{

#include "LangCHT.inl"

	},
	{

#include "LangJPN.inl"

	}
};

// --------------- Enum Lang Resource ---------------
ENUM_DECL(Lang)
{
	ENG, CHS, CHT, JPN
}
ENUM_DEF(Lang, Locale::Lang)
{
	{ Locale::en_US, L"English"_crypt },
	{ Locale::zh_CN, L"简体中文"_crypt },
	{ Locale::zh_TW, L"繁體中文"_crypt },
	{ Locale::ja_JP, L"日本語"_crypt }
};
ENUM_CUSTOM(Lang, {}, L"");
ENUM_DEFAULT(Lang, ENG);

// --------------- Enum Size Resource ---------------
ENUM_DECL(Size)
{
	XS, S, M, L, XL
}
ENUM_DEF(Size, short)
{
	{ 13, L"(XS)"_crypt },
	{ 17, L"(S) "_crypt },
	{ 21, L"(M) "_crypt },
	{ 24, L"(L) "_crypt },
	{ 27, L"(XL)"_crypt }
};
ENUM_CUSTOM(Size, {}, L"(Custom)"_crypt);
ENUM_DEFAULT(Size, S);

// --------------- Enum Speed Resource ---------------
ENUM_DECL(Speed)
{
	FAST, NORMAL, SLOW
}
ENUM_DEF(Speed, short, token::StringName)
{
	{ 8, token::setting_speed_fast },
	{ 5, token::setting_speed_normal },
	{ 1, token::setting_speed_slow }
};
ENUM_CUSTOM(Speed, {}, token::setting_speed_custom);
ENUM_DEFAULT(Speed, NORMAL);

// --------------- Enum Color Resource ---------------
ENUM_DECL(Color)
{
	Gray,
		Blue, LightBlue,
		Green, LightGreen,
		Aqua, LightAqua,
		Red, LightRed,
		Purple, LightPurple,
		Yellow, LightYellow,
		White, LightWhite
}
ENUM_DEF(Color, WORD)
{
	{ 0x08, L"Gray" },
	{ 0x01, L"Blue" }, { 0x09, L"LightBlue" },
	{ 0x02, L"Green" }, { 0x0A, L"LightGreen" },
	{ 0x03, L"Aqua" }, { 0x0B, L"LightAqua" },
	{ 0x04, L"Red" }, { 0x0C, L"LightRed" },
	{ 0x05, L"Purple" }, { 0x0D, L"LightPurple" },
	{ 0x06, L"Yellow" }, { 0x0E, L"LightYellow" },
	{ 0x07, L"White" }, { 0x0F, L"LightWhite" },
};
ENUM_CUSTOM(Color, {}, L"");
ENUM_DEFAULT(Color, White);

// --------------- Theme Resource ---------------
enum struct Element :size_t
{
	blank = 0,
	food,
	snake,
	barrier,

	Mask
};

struct ElementSet
{
	struct Appearance
	{
		wchar_t facade = L'X';
		Color color;

		friend bool operator==(const Appearance& lhs, const Appearance& rhs) noexcept
		{
			return lhs.color == rhs.color && lhs.facade == rhs.facade;
		}
	}elements[static_cast<size_t>(Element::Mask)];

	const auto& operator[](Element Which) const noexcept
	{
		return elements[static_cast<size_t>(Which)];
	}
	auto& operator[](Element Which) noexcept
	{
		return elements[static_cast<size_t>(Which)];
	}
	friend bool operator==(const ElementSet& lhs, const ElementSet& rhs) noexcept
	{
		return lhs[Element::blank] == rhs[Element::blank] &&
			lhs[Element::food] == rhs[Element::food] &&
			lhs[Element::snake] == rhs[Element::snake] &&
			lhs[Element::barrier] == rhs[Element::barrier];
	}
};

ENUM_DECL(Theme)
{
	A, B, C, D, E
}
ENUM_DEF(Theme, ElementSet)
{
	{
		{{
			{ L'□', Color::Blue },
			{ L'★', Color::Red },
			{ L'●', Color::LightYellow },
			{ L'■', Color::Green }
		}},
		L"A"
	},
	{
		{{
			{ L'■', Color::Gray },
			{ L'★', Color::Red },
			{ L'●', Color::LightYellow },
			{ L'■', Color::Aqua }
		}},
		L"B"
	},
	{
		{{
			{ L'□', Color::Red },
			{ L'◆', Color::LightGreen },
			{ L'●', Color::LightBlue },
			{ L'■', Color::LightRed }
		}},
		L"C"
	},
	{
		{{
			{ L'□', Color::LightBlue },
			{ L'★', Color::LightYellow },
			{ L'●', Color::LightPurple },
			{ L'■', Color::LightWhite }
		}},
		L"D"
	},
	{
		{{
			{ L'○', Color::Gray },
			{ L'★', Color::LightWhite },
			{ L'●', Color::LightBlue },
			{ L'◆', Color::Gray }
		}},
		L"E"
	}
};
ENUM_CUSTOM(Theme, {}, L"");
ENUM_DEFAULT(Theme, A);


inline const auto game_title = LR"title(


                 ___          ___          ___          ___          ___     
                /\  \        /\__\        /\  \        /\__\        /\  \    
               /::\  \      /::|  |      /::\  \      /:/  /       /::\  \   
              /:/\ \  \    /:|:|  |     /:/\:\  \    /:/__/       /:/\:\  \  
             _\:\~\ \  \  /:/|:|  |__  /::\~\:\  \  /::\__\____  /::\~\:\  \ 
            /\ \:\ \ \__\/:/ |:| /\__\/:/\:\ \:\__\/:/\:::::\__\/:/\:\ \:\__\
            \:\ \:\ \/__/\/__|:|/:/  /\/__\:\/:/  /\/_|:|~~|~   \:\~\:\ \/__/
             \:\ \:\__\      |:/:/  /      \::/  /    |:|  |     \:\ \:\__\  
              \:\/:/  /      |::/  /       /:/  /     |:|  |      \:\ \/__/  
               \::/  /       /:/  /       /:/  /      |:|  |       \:\__\    
                \/__/        \/__/        \/__/        \|__|        \/__/    
                                                                )title"_crypt;

#endif // SNAKE_RESOURCE_HEADER_