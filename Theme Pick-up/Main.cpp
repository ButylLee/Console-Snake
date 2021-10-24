#include <cwchar>
#include <clocale>
#include <Windows.h>
#include <conio.h>

enum Color :WORD
{
	Gray = 8,
	Blue = 0x01,
	LightBlue = 0x09,
	Green = 0x02,
	LightGreen = 10,
	Aqua = 0x03,
	LightAqua = 0x0B,
	Red = 0x04,
	LightRed = 0x0C,
	LightPurple = 0x0D,
	Purple = 5,
	Yellow = 6,
	LightYellow = 0x0E,
	LightWhite = 0x0F,
	White = 0x07
};

Color color_arr[] = { Gray, Blue ,LightBlue ,Green ,LightGreen,Aqua ,LightAqua ,Red ,LightRed,Purple,LightPurple,Yellow ,LightYellow,White ,LightWhite };
const char* color_name[] = { "Gray", "Blue" ,"LightBlue","Green" ,"LightGreen","Aqua" ,"LightAqua" ,"Red" ,"LightRed","Purple","LightPurple","Yellow","LightYellow","White" ,"LightWhite" };
wchar_t facade_arr[] = { L'□' ,L'★' , L'●' ,L'■' ,L'◆',L'◇',L'○' };

size_t blank_color_shift = 1;
size_t food_color_shift = 7;
size_t snake_color_shift = 12;
size_t barrier_color_shift = 3;

size_t blank_facade_shift = 0;
size_t food_facade_shift = 1;
size_t snake_facade_shift = 2;
size_t barrier_facade_shift = 3;

void setColor(Color color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(color));
}
void setCursor(short x, short y)
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { x,y });
}
void setCursorVisible(bool isVisible)
{
	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);
	cci.bVisible = static_cast<BOOL>(isVisible);
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);
}

size_t next_color(size_t shift)
{
	return shift == sizeof color_arr / 2 - 1 ? 0 : shift + 1;
}

size_t next_facade(size_t shift)
{
	return shift == sizeof facade_arr / 2 - 1 ? 0 : shift + 1;
}

void paintBackground()
{
	setCursor(0, 0);
	for (int row = 0; row < 16; row++)
	{
		for (int col = 0; col < 16; col++)
		{
			if (row == 0 || col == 0 || row == 15 || col == 15)
			{
				setColor(color_arr[barrier_color_shift]);
				putwchar(facade_arr[barrier_facade_shift]);
			}
			else
			{
				setColor(color_arr[blank_color_shift]);
				putwchar(facade_arr[blank_facade_shift]);
			}
		}
		putwchar(L'\n');
	}
	setCursor(4, 5);
	setColor(color_arr[snake_color_shift]);
	putwchar(facade_arr[snake_facade_shift]);
	putwchar(facade_arr[snake_facade_shift]);
	putwchar(facade_arr[snake_facade_shift]);

	setCursor(20, 12);
	setColor(color_arr[food_color_shift]);
	putwchar(facade_arr[food_facade_shift]);
}

void printArg()
{
	setColor(White);
	setCursor(0, 18);
	printf("         color           facade\n");

	printf("blank    1: %-12s q: %lc  \n", color_name[blank_color_shift], facade_arr[blank_facade_shift]);
	printf("food     2: %-12s w: %lc  \n", color_name[food_color_shift], facade_arr[food_facade_shift]);
	printf("snake    3: %-12s e: %lc  \n", color_name[snake_color_shift], facade_arr[snake_facade_shift]);
	printf("barrier  4: %-12s r: %lc  \n", color_name[barrier_color_shift], facade_arr[barrier_facade_shift]);
}

int main()
{
	setlocale(LC_ALL, "");
	setCursorVisible(false);
	while (true)
	{
		paintBackground();
		printArg();
		switch (_getch())
		{
			case '1':
				blank_color_shift = next_color(blank_color_shift);
				break;
			case '2':
				food_color_shift = next_color(food_color_shift);
				break;
			case '3':
				snake_color_shift = next_color(snake_color_shift);
				break;
			case '4':
				barrier_color_shift = next_color(barrier_color_shift);
				break;
			case 'q':
			case 'Q':
				blank_facade_shift = next_facade(blank_facade_shift);
				break;
			case 'w':
			case 'W':
				food_facade_shift = next_facade(food_facade_shift);
				break;
			case 'e':
			case 'E':
				snake_facade_shift = next_facade(snake_facade_shift);
				break;
			case 'r':
			case 'R':
				barrier_facade_shift = next_facade(barrier_facade_shift);
				break;

			case 27:
				return 0;
		}
		Sleep(10);
	}
	return 0;
}