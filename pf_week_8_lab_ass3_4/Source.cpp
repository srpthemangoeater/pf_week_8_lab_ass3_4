#include <stdio.h>
#include <windows.h>
#include <time.h>

#define scount 20
#define screen_x 80
#define screen_y 25

HANDLE rHnd;
HANDLE wHnd;
DWORD fdwMode;
COORD bufferSize = { screen_x,screen_y };
SMALL_RECT windowSize = { 0,0,screen_x - 1,screen_y };

CHAR_INFO consoleBuffer[screen_x * screen_y];
COORD characterPos = { 0,0 };
COORD star[scount];

int setConsole(int x, int y)
{
	wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
	SetConsoleScreenBufferSize(wHnd, bufferSize);

	return 0;
}

int setMode()
{
	rHnd = GetStdHandle(STD_INPUT_HANDLE);
	fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
	SetConsoleMode(rHnd, fdwMode);
	return 0;
}

void setcursor(bool visible)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO lpCursor;
	lpCursor.bVisible = visible;
	lpCursor.dwSize = 20;
	SetConsoleCursorInfo(console, &lpCursor);
}

void setcolor(int fg, int bg)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, bg * 16 + fg);
}

void gotoxy(int x, int y)
{
	COORD c = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
void draw_ship(int x, int y , int c)
{
	setcolor(c, 0);
	gotoxy(x, y); printf("<-^->");
}

void erase_ship(int x, int y)
{
	setcolor(2, 0);
	gotoxy(x, y);	printf(" ");

}

void clear_buffer()
{
	for (int i = 0; i < scount; i++)
	{
		consoleBuffer[star[i].X + screen_x * star[i].Y].Char.AsciiChar = ' ';
		consoleBuffer[star[i].X + screen_x * star[i].Y].Attributes = 7;
	}
}

void fill_buffer_to_console()
{
	WriteConsoleOutputA(wHnd, consoleBuffer, bufferSize, characterPos, &windowSize);
}

void init_star()
{
	for (int i = 0; i < scount; i++)
	{
		short randx = rand() % screen_x;
		short randy = rand() % screen_y;
		star[i] = { randx , randy };
	}
}

char cursor(int x, int y) {
	HANDLE hStd = GetStdHandle(STD_OUTPUT_HANDLE);
	char buf[2]; COORD c = { x,y }; DWORD num_read;
	if (
		!ReadConsoleOutputCharacter(hStd, (LPTSTR)buf, 1, c, (LPDWORD)&num_read))

		return '\0';
	else
		return buf[0];

}

void star_fall()
{
	for (int i = 0; i < scount; i++)
	{
		if (cursor(star[i].X, star[i].Y) == '<' || cursor(star[i].X, star[i].Y) == '-' || cursor(star[i].X, star[i].Y) == '^' || cursor(star[i].X, star[i].Y) == '>')
		{
			star[i] = { SHORT(rand() % screen_x), 1 };
		}
		else
		{
			if (star[i].Y >= screen_y -1 )
			{
				star[i] = { SHORT(rand() % screen_x), 1 };
			}
			else
			{
				short mstary = star[i].Y + 1;
				star[i] = { star[i].X, mstary };
			}
		}
	}
}

void fill_star_to_buffer()
{
	for (int i = 0; i < scount; i++)
	{
		consoleBuffer[star[i].X + screen_x * star[i].Y].Char.AsciiChar = '*';
		consoleBuffer[star[i].X + screen_x * star[i].Y].Attributes = 7;
	}
}

int main()
{
	srand(time(NULL));
	bool play = true;
	DWORD numEvents = 0;
	DWORD numEventsRead = 0;
	int color = 7;
	int pre_posx = 0;
	int pre_posy = 0;
	int HP = 10;
	setConsole(screen_x, screen_y);
	setMode();
	init_star();
	while(play)
	{
		clear_buffer();
		star_fall();
		for (int i = 0; i < scount; i++)
			if (cursor(star[i].X, star[i].Y) == '<' || cursor(star[i].X, star[i].Y) == '-' || cursor(star[i].X, star[i].Y) == '^' || cursor(star[i].X, star[i].Y) == '>')
			{
				HP--;
				//printf("star[%.2d] pos : %.2d,%.2d\n",i+1 ,star[i].X , star[i].Y);
			}
		fill_star_to_buffer();
		fill_buffer_to_console();
		draw_ship(pre_posx, pre_posy, color);
		GetNumberOfConsoleInputEvents(rHnd, &numEvents);
		INPUT_RECORD* eventBuffer = new INPUT_RECORD[numEvents];
		ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
		for (DWORD i = 0; i < numEventsRead; ++i) {

			if (eventBuffer[i].EventType == KEY_EVENT &&
					eventBuffer[i].Event.KeyEvent.bKeyDown == true) {
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
						play = false;
					}

					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'c') {
						color = 1+(rand()%15);
					}
					//printf("press : %c\n", eventBuffer[i].Event.KeyEvent.uChar.AsciiChar);
				}
			else if (eventBuffer[i].EventType == MOUSE_EVENT) {
					int posx = eventBuffer[i].Event.MouseEvent.dwMousePosition.X;
					int posy = eventBuffer[i].Event.MouseEvent.dwMousePosition.Y;
					//printf("mouse position : (%d,%d)\n", posx, posy);
					pre_posx = posx;
					pre_posy = posy;
					if (eventBuffer[i].Event.MouseEvent.dwButtonState &
						FROM_LEFT_1ST_BUTTON_PRESSED) {
						//printf("left click\n");
					}
					else if (eventBuffer[i].Event.MouseEvent.dwButtonState &
						RIGHTMOST_BUTTON_PRESSED) {
						//printf("right click\n");
					}
				}
			}
		if (HP == 0)
			play = false;
		Sleep(100);
	}
	return 0;

}