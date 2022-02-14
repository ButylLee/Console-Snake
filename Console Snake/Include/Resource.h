#pragma once
#ifndef SNAKE_RESOURCE_HEADER_
#define SNAKE_RESOURCE_HEADER_

#include "Enum.h"
#include "LocalizedStrings.h"
#include "EncryptedString.h"
#include "WinMacro.h"
#include <Windows.h>

#define GAME_VERSION "2.11"
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
	setting_customize_theme,
	setting_language,
	setting_save,
	setting_return,
	setting_speed_fast,
	setting_speed_normal,
	setting_speed_slow,
	setting_custom,
	setting_theme_A,
	setting_theme_B,
	setting_theme_C,
	setting_theme_D,
	setting_theme_E,
	setting_show_frame_yes,
	setting_show_frame_no,
	setting_reset_custom,

	custom_theme_list_head,
	custom_theme_blank,
	custom_theme_food,
	custom_theme_snake,
	custom_theme_barrier,

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
struct LangInfo {
	enum Tag {
		ENG, CHS, CHT, JPN,
		DefaultValue = ENG
	};
};
using Lang = Enum<LangInfo, Locale::Lang>;
ENUM_DEFINE(Lang)
{
	{ Locale::en_US, L"English"_crypt },
	{ Locale::zh_CN, L"简体中文"_crypt },
	{ Locale::zh_TW, L"繁體中文"_crypt },
	{ Locale::ja_JP, L"日本語"_crypt }
};

// --------------- Enum Size Resource ---------------
struct SizeInfo {
	enum Tag {
		XS, S, M, L, XL,
		DefaultValue = S
	};
};
using Size = CustomEnum<SizeInfo, short>;
ENUM_DEFINE(Size)
{
	{ 13, L"(XS)"_crypt },
	{ 17, L"(S) "_crypt },
	{ 21, L"(M) "_crypt },
	{ 24, L"(L) "_crypt },
	{ 27, L"(XL)"_crypt }
};
ENUM_CUSTOM(Size)
{
	{}, L"(Custom)"_crypt
};

// --------------- Enum Speed Resource ---------------
struct SpeedInfo {
	enum Tag {
		FAST, NORMAL, SLOW,
		DefaultValue = NORMAL
	};
};
using Speed = CustomEnum<SpeedInfo, short, token::StringName>;
ENUM_DEFINE(Speed)
{
	{ 8, token::setting_speed_fast },
	{ 5, token::setting_speed_normal },
	{ 1, token::setting_speed_slow }
};
ENUM_CUSTOM(Speed)
{
	{}, token::setting_custom
};

// --------------- Enum Color Resource ---------------
struct ColorInfo {
	enum Tag {
		Black,  Gray,
		Blue,   LightBlue,
		Green,  LightGreen,
		Aqua,   LightAqua,
		Red,    LightRed,
		Purple, LightPurple,
		Yellow, LightYellow,
		White , LightWhite,
		DefaultValue = White
	};
};
using Color = Enum<ColorInfo, WORD>;
ENUM_DEFINE(Color)
{
	{ 0x00, L"Black      " }, { 0x08, L"Gray       " },
	{ 0x01, L"Blue       " }, { 0x09, L"LightBlue  " },
	{ 0x02, L"Green      " }, { 0x0A, L"LightGreen " },
	{ 0x03, L"Aqua       " }, { 0x0B, L"LightAqua  " },
	{ 0x04, L"Red        " }, { 0x0C, L"LightRed   " },
	{ 0x05, L"Purple     " }, { 0x0D, L"LightPurple" },
	{ 0x06, L"Yellow     " }, { 0x0E, L"LightYellow" },
	{ 0x07, L"White      " }, { 0x0F, L"LightWhite " },
};

// --------------- Enum Facade Resource ---------------
struct FacadeInfo {
	enum Tag {
		FullStar, FullCircle, FullRect, FullDiamond,
		Star, Circle, Rect, Diamond,
		DefaultValue = FullStar
	};
};
using Facade = Enum<FacadeInfo, wchar_t>;
ENUM_DEFINE(Facade)
{
	{ L'★', L"" },
	{ L'●', L"" },
	{ L'■', L"" },
	{ L'◆', L"" },
	{ L'☆', L"" },
	{ L'○', L"" },
	{ L'□', L"" },
	{ L'◇', L"" },
};

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
		Facade facade;
		Color color;

		friend constexpr bool
		operator==(const Appearance&, const Appearance&) = default;
	}elements[static_cast<size_t>(Element::Mask)];

	constexpr const auto& operator[](Element which) const noexcept
	{
		return elements[static_cast<size_t>(which)];
	}
	constexpr auto& operator[](Element which) noexcept
	{
		return elements[static_cast<size_t>(which)];
	}
	friend constexpr bool
	operator==(const ElementSet&, const ElementSet&) = default;
};

struct ThemeInfo {
	enum Tag {
		A, B, C, D, E,
		DefaultValue = A
	};
};
using Theme = CustomEnum<ThemeInfo, ElementSet, token::StringName>;
ENUM_DEFINE(Theme)
{
	{
		{{
			{ Facade::Rect, Color::Blue },
			{ Facade::FullStar, Color::Red },
			{ Facade::FullCircle, Color::LightYellow },
			{ Facade::FullRect, Color::Green }
		}},
		token::setting_theme_A
	},
	{
		{{
			{ Facade::FullRect, Color::Gray },
			{ Facade::FullStar, Color::Red },
			{ Facade::FullCircle, Color::LightYellow },
			{ Facade::FullRect, Color::Aqua }
		}},
		token::setting_theme_B
	},
	{
		{{
			{ Facade::Rect, Color::Red },
			{ Facade::FullDiamond, Color::LightGreen },
			{ Facade::FullCircle, Color::LightBlue },
			{ Facade::FullRect, Color::LightRed }
		}},
		token::setting_theme_C
	},
	{
		{{
			{ Facade::Rect, Color::LightBlue },
			{ Facade::FullStar, Color::LightYellow },
			{ Facade::FullCircle, Color::LightPurple },
			{ Facade::FullRect, Color::LightWhite }
		}},
		token::setting_theme_D
	},
	{
		{{
			{ Facade::Circle, Color::Gray },
			{ Facade::FullStar, Color::LightWhite },
			{ Facade::FullCircle, Color::LightBlue },
			{ Facade::FullDiamond, Color::Gray }
		}},
		token::setting_theme_E
	}
};
ENUM_CUSTOM(Theme)
{
	{}, token::setting_custom
};


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