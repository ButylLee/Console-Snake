#pragma once
#ifndef SNAKE_LOCALIZEDSTRINGS_HEADER_
#define SNAKE_LOCALIZEDSTRINGS_HEADER_

#include <string>

enum struct Locale :size_t
{
	en_US,
	zh_CN,
	zh_TW,
	ja_JP,
	Mask_
};

enum struct Token :size_t
{
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
	setting_map,
	setting_customize_map,
	setting_show_frame,
	setting_opening_pause,
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
	setting_yes,
	setting_no,
	setting_reset_custom,

	custom_theme_list_head,
	custom_theme_blank,
	custom_theme_food,
	custom_theme_snake,
	custom_theme_barrier,

	custom_map_prev,
	custom_map_next,
	custom_map_edit_map,
	custom_map_delete_map,
	custom_map_delete_map_confirm,
	custom_map_switch_size,
	custom_map_rename,
	custom_map_curr_size,
	custom_map_curr_pos,
	custom_map_move_cursor,
	custom_map_switch_block,
	custom_map_all_blank,
	custom_map_save_edit,
	custom_map_cancel_edit,

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
	message_unknown_error,
	Mask_
};

class LocalizedStrings
{
	friend const std::wstring operator~(Token) noexcept;
	LocalizedStrings() = delete;

public:
	static void setLang(Locale) noexcept;

private:
	static Locale lang;
	static const std::wstring strings[static_cast<size_t>(Locale::Mask_)][static_cast<size_t>(Token::Mask_)];
};

const std::wstring operator~(Token) noexcept;

#endif // SNAKE_LOCALIZEDSTRINGS_HEADER_