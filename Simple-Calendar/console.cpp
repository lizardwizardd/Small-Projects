#include <string>
#include <windows.h>
#include <stdio.h>
#include <iostream>

enum TColor {BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHTGRAY,
             DARKGRAY, LIGHTBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED,
             LIGHTMAGENTA, YELLOW, WHITE};

void gotoXY(int x, int y)
{
    HANDLE Console = GetStdHandle(STD_OUTPUT_HANDLE);
      if (!Console)
          return;

        COORD pos;
        pos.X = x;
        pos.Y = y;

        SetConsoleCursorPosition(Console, pos);
}

COORD whereXY(void)
{
    HANDLE Console = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {-1, -1};
    if (!Console)
       return pos;

    CONSOLE_SCREEN_BUFFER_INFO buf;

    GetConsoleScreenBufferInfo(Console, &buf);
    return buf.dwCursorPosition;
}

void textColor(int color)
{
    HANDLE Console = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!Console)
        return;

    CONSOLE_SCREEN_BUFFER_INFO buf;
    GetConsoleScreenBufferInfo(Console, &buf);

    WORD Attr;
    DWORD Count;
    COORD pos = buf.dwCursorPosition;
    ReadConsoleOutputAttribute(Console, &Attr, 1, pos, &Count);

    int bk_col = Attr / 16;
    int col = color % 16;
    col = col + bk_col * 16;

    SetConsoleTextAttribute(Console, col);

    char ch;
    pos = whereXY();
    ReadConsoleOutputCharacter(Console, &ch, 1, pos, &Count);
    printf("%c", ch);
    gotoXY(pos.X, pos.Y);
}

void printTextInColor(std::string text, TColor color)
{
    textColor(color);
    std::cout << text;
    textColor(LIGHTGRAY);
}
