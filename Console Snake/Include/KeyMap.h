#pragma once
#ifndef SNAKE_KEYMAP_HEADER_
#define SNAKE_KEYMAP_HEADER_

#include "wideIO.h"
#include <climits>

// Some keys like function key return 2 values by _getwch().
// for example, K_F7 returns 0 first, then 65. Store these
// two values in an integral reversely, i.e. 65 to MSB and
// 0 to LSB, to avoid conflicting with regular key like K_A.
#define DOUBLE_KEY(first, second) ((first) | (second) << sizeof(decltype(_getwch())) * CHAR_BIT)

// useing wide char
enum Key :decltype(getwch())
{
	K_0 = L'0',
	K_1 = L'1',
	K_2 = L'2',
	K_3 = L'3',
	K_4 = L'4',
	K_5 = L'5',
	K_6 = L'6',
	K_7 = L'7',
	K_8 = L'8',
	K_9 = L'9',

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
	K_Esc = 27,
	K_Space = L' ',
	K_Enter = L'\r',
	K_Ctrl_Enter = L'\n',

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
};

#undef DOUBLE_KEY
#endif // SNAKE_KEYMAP_HEADER_