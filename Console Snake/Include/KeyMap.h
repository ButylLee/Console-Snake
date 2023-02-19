#pragma once
#ifndef SNAKE_KEYMAP_HEADER_
#define SNAKE_KEYMAP_HEADER_

#include "WideIO.h"
#include <climits>

// Some keys like function key return 2 values by _getwch().
// for example, K_F7 returns 0 first, then 65. Store these
// two values in an integral reversely, i.e. 65 to MSB and
// 0 to LSB, to avoid conflicting with regular key like K_A.
#define DOUBLE_KEY(first, second) ((first) | (second) << sizeof(decltype(_getwch())) * CHAR_BIT)

// using wide char
enum Key :decltype(getwch())
{
	K_0 = L'0',
	K_1,
	K_2,
	K_3,
	K_4,
	K_5,
	K_6,
	K_7,
	K_8,
	K_9,

	K_A = L'A',
	K_B, K_C, K_D, K_E, K_F, K_G, K_H, K_I, K_J, K_K, K_L, K_M, K_N,
	K_O, K_P, K_Q, K_R, K_S, K_T, K_U, K_V, K_W, K_X, K_Y, K_Z,

	K_a = L'a',
	K_b, K_c, K_d, K_e, K_f, K_g, K_h, K_i, K_j, K_k, K_l, K_m, K_n,
	K_o, K_p, K_q, K_r, K_s, K_t, K_u, K_v, K_w, K_x, K_y, K_z,

	K_UP = DOUBLE_KEY(224, 72),
	K_LEFT = DOUBLE_KEY(224, 75),
	K_RIGHT = DOUBLE_KEY(224, 77),
	K_DOWN = DOUBLE_KEY(224, 80),

	K_Esc = 27,
	K_Tab = L'\t',
	K_Space = L' ',
	K_Enter = L'\r',
	K_Backspace = 8,
	K_Ctrl_Enter = L'\n',
	K_Insert = DOUBLE_KEY(224, 82),
	K_Delete = DOUBLE_KEY(224, 83),
	K_Home = DOUBLE_KEY(224, 71),
	K_End = DOUBLE_KEY(224, 79),
	K_PageUp = DOUBLE_KEY(224, 73),
	K_PageDown = DOUBLE_KEY(224, 81),

	K_Ctrl_Aa = 1,
	K_Ctrl_Bb,
	K_Ctrl_Cc,
	K_Ctrl_Dd,
	K_Ctrl_Ee,
	K_Ctrl_Ff,
	K_Ctrl_Gg,
	K_Ctrl_Hh,
	K_Ctrl_Ii,
	K_Ctrl_Jj,
	K_Ctrl_Kk,
	K_Ctrl_Ll,
	K_Ctrl_Mm,
	K_Ctrl_Nn,
	K_Ctrl_Oo,
	K_Ctrl_Pp,
	K_Ctrl_Qq,
	K_Ctrl_Rr,
	K_Ctrl_Ss,
	K_Ctrl_Tt,
	K_Ctrl_Uu,
	K_Ctrl_Vv,
	K_Ctrl_Ww,
	K_Ctrl_Xx,
	K_Ctrl_Yy,
	K_Ctrl_Zz,

	K_Ctrl_Insert = DOUBLE_KEY(224, 146),
	K_Ctrl_Delete = DOUBLE_KEY(224, 147),
	K_Ctrl_Home = DOUBLE_KEY(224, 119),
	K_Ctrl_End = DOUBLE_KEY(224, 117),
	K_Ctrl_PageUp = DOUBLE_KEY(224, 134),
	K_Ctrl_PageDown = DOUBLE_KEY(224, 118),

	K_Alt_Insert = DOUBLE_KEY(0, 162),
	K_Alt_Delete = DOUBLE_KEY(0, 163),
	K_Alt_Home = DOUBLE_KEY(0, 151),
	K_Alt_End = DOUBLE_KEY(0, 159),
	K_Alt_PageUp = DOUBLE_KEY(0, 153),
	K_Alt_PageDown = DOUBLE_KEY(0, 161),

	K_F1 = DOUBLE_KEY(0, 59),
	K_F2 = DOUBLE_KEY(0, 60),
	K_F3 = DOUBLE_KEY(0, 61),
	K_F4 = DOUBLE_KEY(0, 62),
	K_F5 = DOUBLE_KEY(0, 63),
	K_F6 = DOUBLE_KEY(0, 64),
	K_F7 = DOUBLE_KEY(0, 65),
	K_F8 = DOUBLE_KEY(0, 66),
	K_F9 = DOUBLE_KEY(0, 67),
	K_F10 = DOUBLE_KEY(0, 68),
	K_F11 = DOUBLE_KEY(224, 133),
	K_F12 = DOUBLE_KEY(224, 134),

	K_Shift_F1 = DOUBLE_KEY(0, 84),
	K_Shift_F2 = DOUBLE_KEY(0, 85),
	K_Shift_F3 = DOUBLE_KEY(0, 86),
	K_Shift_F4 = DOUBLE_KEY(0, 87),
	K_Shift_F5 = DOUBLE_KEY(0, 88),
	K_Shift_F6 = DOUBLE_KEY(0, 89),
	K_Shift_F7 = DOUBLE_KEY(0, 90),
	K_Shift_F8 = DOUBLE_KEY(0, 91),
	K_Shift_F9 = DOUBLE_KEY(0, 92),
	K_Shift_F10 = DOUBLE_KEY(0, 93),
	K_Shift_F11 = DOUBLE_KEY(224, 135),
	K_Shift_F12 = DOUBLE_KEY(224, 136),

	K_Ctrl_F1 = DOUBLE_KEY(0, 94),
	K_Ctrl_F2 = DOUBLE_KEY(0, 95),
	K_Ctrl_F3 = DOUBLE_KEY(0, 96),
	K_Ctrl_F4 = DOUBLE_KEY(0, 97),
	K_Ctrl_F5 = DOUBLE_KEY(0, 98),
	K_Ctrl_F6 = DOUBLE_KEY(0, 99),
	K_Ctrl_F7 = DOUBLE_KEY(0, 100),
	K_Ctrl_F8 = DOUBLE_KEY(0, 101),
	K_Ctrl_F9 = DOUBLE_KEY(0, 102),
	K_Ctrl_F10 = DOUBLE_KEY(0, 103),
	K_Ctrl_F11 = DOUBLE_KEY(224, 137),
	K_Ctrl_F12 = DOUBLE_KEY(224, 138),

	K_Alt_F1 = DOUBLE_KEY(0, 104),
	K_Alt_F2 = DOUBLE_KEY(0, 105),
	K_Alt_F3 = DOUBLE_KEY(0, 106),
	K_Alt_F4 = DOUBLE_KEY(0, 107),
	K_Alt_F5 = DOUBLE_KEY(0, 108),
	K_Alt_F6 = DOUBLE_KEY(0, 109),
	K_Alt_F7 = DOUBLE_KEY(0, 110),
	K_Alt_F8 = DOUBLE_KEY(0, 111),
	K_Alt_F9 = DOUBLE_KEY(0, 112),
	K_Alt_F10 = DOUBLE_KEY(0, 113),
	K_Alt_F11 = DOUBLE_KEY(224, 139),
	K_Alt_F12 = DOUBLE_KEY(224, 140),
};

#undef DOUBLE_KEY
#endif // SNAKE_KEYMAP_HEADER_