#include "LocalizedStrings.h"
#include "Resource.h"

const std::wstring operator~(Token name) noexcept
{
	return { LocalizedStrings::strings[static_cast<size_t>(LocalizedStrings::lang)][static_cast<size_t>(name)] };
}

void LocalizedStrings::setLang(Locale new_lang) noexcept
{
	if (new_lang != Locale::Mask_)
		lang = new_lang;
}

Locale LocalizedStrings::lang = Locale::en_US;

const std::wstring LocalizedStrings::strings[static_cast<size_t>(Locale::Mask_)][static_cast<size_t>(Token::Mask_)] =
{
	{
#include "Langs/LangENG.inl"
	},
	{
#include "Langs/LangCHS.inl"
	},
	{
#include "Langs/LangCHT.inl"
	},
	{
#include "Langs/LangJPN.inl"
	},
};