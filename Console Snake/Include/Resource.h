﻿#pragma once
#ifndef SNAKE_RESOURCE_HEADER_
#define SNAKE_RESOURCE_HEADER_

#include "Enum.h"
#include "LocalizedStrings.h"
#include "EncryptedString.h"
#include "WinHeader.h"
#include <random>
#include <iterator>
#include <cstddef>
#include <cassert>

#define GAME_VERSION "2.36"

namespace Resource {
	inline constexpr const char* SaveFileName = "SnakeSaved.bin";
	inline constexpr const unsigned char CryptoKey[] = {
		0x54, 0xDE, 0x3B, 0xF2, 0xD8, 0x5D, 0x4E, 0x04,
		0xB2, 0xBE, 0x4D, 0xCC, 0xC3, 0xAD, 0xEB, 0x1C,
	};
	inline constexpr const unsigned char CryptoIV[] = {
		0xE9, 0x5C, 0x99, 0x13, 0xCC, 0x94, 0x4A, 0x0C,
		0x92, 0xD1, 0x48, 0x9E, 0x03, 0x9B, 0x4E, 0xA4,
	};
	inline const wchar_t* const GameTitle = []
	{
		static constexpr const wchar_t* titles[] = { LR"title(


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
                                                                )title", // Isometric1
			LR"title(


                 ___          ___          ___          ___          ___     
                /  /\        /__/\        /  /\        /__/|        /  /\    
               /  /:/_       \  \:\      /  /::\      |  |:|       /  /:/_   
              /  /:/ /\       \  \:\    /  /:/\:\     |  |:|      /  /:/ /\  
             /  /:/ /::\  _____\__\:\  /  /:/~/::\  __|  |:|     /  /:/ /:/_ 
            /__/:/ /:/\:\/__/::::::::\/__/:/ /:/\:\/__/\_|:|____/__/:/ /:/ /\
            \  \:\/:/~/:/\  \:\~~\~~\/\  \:\/:/__\/\  \:\/:::::/\  \:\/:/ /:/
             \  \::/ /:/  \  \:\  ~~~  \  \::/      \  \::/~~~~  \  \::/ /:/ 
              \__\/ /:/    \  \:\       \  \:\       \  \:\       \  \:\/:/  
                /__/:/      \  \:\       \  \:\       \  \:\       \  \::/   
                \__\/        \__\/        \__\/        \__\/        \__\/    
                                                                )title", // Isometric3
			LR"title(




                ________   ________    ________   ___  __     _______      
               |\   ____\ |\   ___  \ |\   __  \ |\  \|\  \  |\  ___ \     
               \ \  \___|_\ \  \\ \  \\ \  \|\  \\ \  \/  /|_\ \   __/|    
                \ \_____  \\ \  \\ \  \\ \   __  \\ \   ___  \\ \  \_|/__  
                 \|____|\  \\ \  \\ \  \\ \  \ \  \\ \  \\ \  \\ \  \_|\ \ 
                   ____\_\  \\ \__\\ \__\\ \__\ \__\\ \__\\ \__\\ \_______\
                  |\_________\\|__| \|__| \|__|\|__| \|__| \|__| \|_______|
                  \|_________|                                             
                                                                )title", // 3D-ASCII
			LR"title(

                                  ,--.                        ,--.           
              .--.--.           ,--.'|    ,---,           ,--/  /|     ,---,.
             /  /    '.     ,--,:  : |   '  .' \       ,---,': / '   ,'  .' |
            |  :  /`. /  ,`--.'`|  ' :  /  ;    '.     :   : '/ /  ,---.'   |
            ;  |  |--`   |   :  :  | | :  :       \    |   '   ,   |   |   .'
            |  :  ;_     :   |   \ | : :  |   /\   \   '   |  /    :   :  |-,
             \  \    `.  |   : '  '; | |  :  ' ;.   :  |   ;  ;    :   |  ;/|
              `----.   \ '   ' ;.    ; |  |  ;/  \   \ :   '   \   |   :   .'
              __ \  \  | |   | | \   | '  :  | \  \ ,' |   |    '  |   |  |-,
             /  /`--'  / '   : |  ; .' |  |  '  '--'   '   : |.  \ '   :  ;/|
            '--'.     /  |   | '`--'   |  :  :         |   | '_\.' |   |    \
              `--'---'   '   : |       |  | ,'         '   : |     |   :   .'
                         ;   |.'       `--''           ;   |,'     |   | ,'  
                         '---'                         '---'       `----'    
                                                                )title", // 3D Diagonal
			LR"title(




                     $$$$$$\                      $$\                 
                    $$  __$$\                     $$ |                
                    $$ /  \__|$$$$$$$\   $$$$$$\  $$ |  $$\  $$$$$$\  
                    \$$$$$$\  $$  __$$\  \____$$\ $$ | $$  |$$  __$$\ 
                     \____$$\ $$ |  $$ | $$$$$$$ |$$$$$$  / $$$$$$$$ |
                    $$\   $$ |$$ |  $$ |$$  __$$ |$$  _$$<  $$   ____|
                    \$$$$$$  |$$ |  $$ |\$$$$$$$ |$$ | \$$\ \$$$$$$$\ 
                     \______/ \__|  \__| \_______|\__|  \__| \_______|
                                                                )title", // Big Money-nw
		};

		std::random_device engine;
		std::uniform_int_distribution<size_t> dis;
		using param_type = typename decltype(dis)::param_type;
		return titles[dis(engine, param_type{ 0, std::size(titles) - 1 })];
	}();
} // namespace Resource

// --------------- Enum Lang Resource ---------------
struct LangEnum {
	enum Tag {
		ENG, CHS, CHT, JPN,
		DefaultValue = ENG
	};
};
using Lang = Enum<LangEnum, Locale>;
ENUM_DEFINE(Lang)
{
	{ Locale::en_US, L"English" },
	{ Locale::zh_CN, L"简体中文" },
	{ Locale::zh_TW, L"繁體中文" },
	{ Locale::ja_JP, L"日本語" }
};

// --------------- Enum Size Resource ---------------
struct SizeEnum {
	enum Tag {
		S, M, L,
		Mask_,
		DefaultValue = S
	};
};
using Size = Enum<SizeEnum, unsigned short>;
ENUM_DEFINE(Size)
{
	{ 15, L"Small  " },
	{ 20, L"Middle " },
	{ 24, L"Large  " }
};

// --------------- Enum Speed Resource ---------------
struct SpeedEnum {
	enum Tag {
		Slow, Normal, Fast,
		DefaultValue = Normal
	};
};
using Speed = CustomEnum<SpeedEnum, short, Token>;
ENUM_DEFINE(Speed)
{
	{ 1, Token::setting_speed_slow },
	{ 5, Token::setting_speed_normal },
	{ 8, Token::setting_speed_fast }
};
ENUM_CUSTOM(Speed)
{
	{}, Token::setting_custom
};

// --------------- Enum Color Resource ---------------
struct ColorEnum {
	enum Tag {
		Black,  Gray,
		Blue,   LightBlue,
		Green,  LightGreen,
		Aqua,   LightAqua,
		Red,    LightRed,
		Purple, LightPurple,
		Yellow, LightYellow,
		White , LightWhite,
		Mask_,
		DefaultValue = White
	};
};
using Color = Enum<ColorEnum, WORD>;
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
struct FacadeEnum {
	enum Tag {
		FullStar, FullCircle, FullRect, FullDiamond,
		Star, Circle, Rect, Diamond,
		Mask_,
		DefaultValue = FullStar
	};
};
using Facade = Enum<FacadeEnum, wchar_t>;
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

// --------------- Enum Theme Resource ---------------
enum struct Element :size_t
{
	Blank = 0,
	Food,
	Snake,
	Barrier,

	Mask_
};

struct ElementSet
{
	struct Appearance
	{
		Facade facade;
		Color color;

		friend bool operator==(const Appearance&, const Appearance&) = default;
	}elements[static_cast<size_t>(Element::Mask_)];

	constexpr auto& operator[](auto which) noexcept
	{
		return elements[static_cast<size_t>(which)];
	}
	constexpr auto& operator[](auto which) const noexcept
	{
		return elements[static_cast<size_t>(which)];
	}
	friend bool operator==(const ElementSet&, const ElementSet&) = default;
};

struct ThemeEnum {
	enum Tag {
		A, B, C, D, E,
		DefaultValue = A
	};
};
using Theme = CustomEnum<ThemeEnum, ElementSet, Token>;
ENUM_DEFINE(Theme)
{
	{
		{{
			{ Facade::Rect, Color::Blue },
			{ Facade::FullStar, Color::Red },
			{ Facade::FullCircle, Color::LightYellow },
			{ Facade::FullRect, Color::Green }
		}},
		Token::setting_theme_A
	},
	{
		{{
			{ Facade::FullRect, Color::Gray },
			{ Facade::FullStar, Color::Red },
			{ Facade::FullCircle, Color::LightYellow },
			{ Facade::FullRect, Color::Aqua }
		}},
		Token::setting_theme_B
	},
	{
		{{
			{ Facade::Rect, Color::Red },
			{ Facade::FullDiamond, Color::LightGreen },
			{ Facade::FullCircle, Color::LightBlue },
			{ Facade::FullRect, Color::LightRed }
		}},
		Token::setting_theme_C
	},
	{
		{{
			{ Facade::FullDiamond, Color::Gray },
			{ Facade::FullCircle, Color::Yellow },
			{ Facade::FullCircle, Color::White },
			{ Facade::Rect, Color::LightBlue }
		}},
		Token::setting_theme_D
	},
	{
		{{
			{ Facade::Circle, Color::Gray },
			{ Facade::FullStar, Color::LightWhite },
			{ Facade::FullCircle, Color::LightBlue },
			{ Facade::FullDiamond, Color::Gray }
		}},
		Token::setting_theme_E
	}
};
ENUM_CUSTOM(Theme)
{
	{}, Token::setting_custom
};

// --------------- Enum Map Resource ---------------
struct MapShapeIterator
{
	MapShapeIterator(const std::byte* data, size_t count) noexcept
		:data(data), count(count)
	{}
	Element operator*() noexcept
	{
		switch (std::to_integer<int>(*data >> count & std::byte{0b1}))
		{
			case 0b0:
				return Element::Blank;
			case 0b1:
				return Element::Barrier;
		}
		[[unlikely]] return Element::Blank;
	}
	MapShapeIterator& operator++() noexcept
	{
		if (++count == 8)
		{
			data++;
			count = 0;
		}
		return *this;
	}
	MapShapeIterator operator++(int) noexcept
	{
		MapShapeIterator tmp = *this;
		++*this;
		return tmp;
	}
	bool operator==(const MapShapeIterator&) const = default;
private:
	const std::byte* data;
	size_t count;
};
template<size_t N>
class MapShape
{
	static constexpr size_t Size = N * N;
	static constexpr size_t RemainCount = Size % 8;
	static constexpr size_t CompressedSize = Size / 8 + (RemainCount == 0 ? 0 : 1);
public:
	template<size_t Count>
	constexpr MapShape(const wchar_t(&str)[Count]) noexcept
	{
		static_assert(Count - 1 == Size + N); // - 1'\0', + N'\n'
		size_t index = 0;
		for (unsigned char count = 0, byte = 0; auto ch : str)
		{
			if (index == 0 && ch == L'\n')
				continue;
			switch (ch)
			{
				case L'□':
					byte |= 0b0 << count; break;
				case L'■':
					byte |= 0b1 << count; break;
				case L'\n':
					continue;
				case L'\0':
					if (index != CompressedSize)
						data[index] = std::byte{ byte };
					return;
			}
			if (++count == 8)
			{
				count = 0;
				data[index++] = std::byte{ byte };
				byte = 0;
			}
		}
	}
	template<std::random_access_iterator Iter>
	MapShape(Iter begin, Iter end) noexcept
	{
		assert(end - begin == Size);
		size_t index = 0;
		for (unsigned char count = 0, byte = 0; begin != end; ++begin)
		{
			Element item = *begin;
			switch (item)
			{
				case Element::Blank:
					byte |= 0b0 << count; break;
				case Element::Barrier:
					byte |= 0b1 << count; break;
			}
			if (begin == end - 1)
			{
				data[index] = std::byte{ byte };
				return;
			}
			if (++count == 8)
			{
				count = 0;
				data[index++] = std::byte{ byte };
				byte = 0;
			}
		}
	}
	MapShape() = default;
	MapShapeIterator begin() const noexcept { return MapShapeIterator(data, 0); }
	MapShapeIterator end() const noexcept
	{
		return MapShapeIterator(data + CompressedSize - (RemainCount == 0 ? 0 : 1), RemainCount);
	}
	size_t size() const noexcept { return Size; }

private:
	std::byte data[CompressedSize] = {};
};
using MapShapeSmall = MapShape<15>;
using MapShapeMiddle = MapShape<20>;
using MapShapeLarge = MapShape<24>;

struct MapCell
{
	MapShapeSmall map_small;
	MapShapeMiddle map_middle;
	MapShapeLarge map_large;
};

struct MapSetEnum {
	enum Tag {
		Square, Space,
		Mask_,
		DefaultValue = Square
	};
};
using MapSet = MultiCustomEnum<MapSetEnum, MapCell>;
ENUM_DEFINE(MapSet)
{
	{
		{
			{
				LR"(
■■■■■■■■■■■■■■■
■□□□□□□□□□□□□□■
■□□□□□□□□□□□□□■
■□□□□□□□□□□□□□■
■□□□□□□□□□□□□□■
■□□□□□□□□□□□□□■
■□□□□□□□□□□□□□■
■□□□□□□□□□□□□□■
■□□□□□□□□□□□□□■
■□□□□□□□□□□□□□■
■□□□□□□□□□□□□□■
■□□□□□□□□□□□□□■
■□□□□□□□□□□□□□■
■□□□□□□□□□□□□□■
■■■■■■■■■■■■■■■)"
			},
			{
				LR"(
■■■■■■■■■■■■■■■■■■■■
■□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□■
■■■■■■■■■■■■■■■■■■■■)"
			},
			{
				LR"(
■■■■■■■■■■■■■■■■■■■■■■■■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■□□□□□□□□□□□□□□□□□□□□□□■
■■■■■■■■■■■■■■■■■■■■■■■■)"
			}
		},
		L"Square"
	},
	{
		{
			{
				LR"(
□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□)"
			},
			{
				LR"(
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□)"
			},
			{
				LR"(
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□
□□□□□□□□□□□□□□□□□□□□□□□□)"
			}
		},
		L"Space"
	}
};

struct Map // Proxy
{
	static constexpr size_t MaxMapSetCount = 16;
	static constexpr size_t NameMaxHalfWidth = 10;
	MapSet set;
	Size size;

	void setNextValue() noexcept
	{
		if (size.setNextValue() == Size::S)
			set.setNextValue();
	}
	void visitValue(auto&& f) const
	{
		switch (size.Index())
		{
			case Size::S:
				f(set.Value().map_small); break;
			case Size::M:
				f(set.Value().map_middle); break;
			case Size::L:
				f(set.Value().map_large); break;
		}
	}
	void visitCustomValue(auto&& f)
	{
		switch (size.Index())
		{
			case Size::S:
				f(MapSet::ModifyCustomItem(set).map_small); break;
			case Size::M:
				f(MapSet::ModifyCustomItem(set).map_middle); break;
			case Size::L:
				f(MapSet::ModifyCustomItem(set).map_large); break;
		}
	}
};

#endif // SNAKE_RESOURCE_HEADER_