#pragma once
#ifndef SNAKE_RESOURCE_HEADER_
#define SNAKE_RESOURCE_HEADER_

#include "Enum.h"
#include "LocalizedStrings.h"
#include "EncryptedString.h"

#define GAME_VERSION "pre-2.23"
inline const auto save_file_name = "SnakeSaved.bin"_crypt;
inline constexpr const unsigned char crypto_key[] = {
	0x54, 0xDE, 0x3B, 0xF2, 0xD8, 0x5D, 0x4E, 0x04,
	0xB2, 0xBE, 0x4D, 0xCC, 0xC3, 0xAD, 0xEB, 0x1C
};
inline constexpr const unsigned char crypto_IV[] = {
	0xE9, 0x5C, 0x99, 0x13, 0xCC, 0x94, 0x4A, 0x0C,
	0x92, 0xD1, 0x48, 0x9E, 0x03, 0x9B, 0x4E, 0xA4
};

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


ENUM_DECL(Speed)
{
	FAST, NORMAL, SLOW
}
ENUM_DEF(Speed, int, token::StringName)
{
	{ 8, token::setting_speed_fast },
	{ 5, token::setting_speed_normal },
	{ 1, token::setting_speed_slow }
};
ENUM_CUSTOM(Speed, {}, token::setting_speed_custom);
ENUM_DEFAULT(Speed, NORMAL);


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