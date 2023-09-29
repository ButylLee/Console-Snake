#pragma once
#ifndef SNAKE_WINMACRO_HEADER_
#define SNAKE_WINMACRO_HEADER_

/* Include this header file to include Windows.h */

//#define WIN32_LEAN_AND_MEAN
#define NOSERVICE
#define NOMCX
#define NOIME
#define NOMINMAX

#include <Windows.h>

#pragma comment(lib, "winmm.lib")

// enable Windows Visual Style
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' \
version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#endif // SNAKE_WINMACRO_HEADER_