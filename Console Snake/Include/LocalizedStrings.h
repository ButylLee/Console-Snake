#pragma once
#ifndef SNAKE_LOCALIZEDSTRINGS_HEADER_
#define SNAKE_LOCALIZEDSTRINGS_HEADER_

/*
 * macros using sequence:
 *            TOKEN_DEF(...);  LANG_DEF(...);
 *                          ||
 *                          vv
 *                   DEF_LOCALIZED_STR();
 *                          ||
 *                          vv
 *    LANG_DEFAULT(arg);  MAKE_LOCALIZED_STRS{ {...},{...} };
 *
 * !!All the string should use wide char!!
 *
 * to use token to get relevant localized string:
 *     printf(~Token::myname);
 * to set new language:
 *     LocalizedStrings::setLang(Locale::new_lang);
 */

#include <string>

class LocalizedStrings;

struct Token;
struct Locale;

#define TOKEN_DEF(...) \
struct Token \
{ \
	enum StringName :size_t \
	{ \
		__VA_ARGS__, _NameMask \
	}; \
}

#define LANG_DEF(...) \
struct Locale \
{ \
	enum Lang :size_t \
	{ \
		__VA_ARGS__, _LangMask \
	}; \
}

#define DEF_LOCALIZED_STR() \
class LocalizedStrings \
{ \
	friend const std::wstring operator~(Token::StringName) noexcept; \
	LocalizedStrings() = delete; \
public: \
	static void setLang(Locale::Lang) noexcept; \
private: \
	static Locale::Lang lang; \
	static const std::wstring strings[Locale::_LangMask][Token::_NameMask]; \
}; \
inline void LocalizedStrings::setLang(Locale::Lang new_lang) noexcept \
{ \
	if (new_lang != Locale::_LangMask) \
		lang = new_lang; \
} \
inline const std::wstring operator~(Token::StringName name) noexcept \
{ \
	return { LocalizedStrings::strings[LocalizedStrings::lang][name] }; \
}

#define LANG_DEFAULT(default_lang) \
inline Locale::Lang LocalizedStrings::lang = Locale::default_lang

#define MAKE_LOCALIZED_STRS \
inline const std::wstring LocalizedStrings::strings[Locale::_LangMask][Token::_NameMask] = 

#endif // SNAKE_LOCALIZEDSTRINGS_HEADER_