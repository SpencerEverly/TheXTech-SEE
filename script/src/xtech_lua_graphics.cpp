#include "../../src/graphics.h"

void xtech_lua_textPrint(const char* text, float x, float y)
{
    SuperPrint(text, 3, x, y);
}

void xtech_lua_textPrintWP(const char* text, float x, float y, float priority)
{
    SuperPrint(text, 3, x, y); //No priority exists, for some reason lol
}