#include "../../src/graphics.h"

void xtech_lua_textPrint(const std::string &text, float x, float y)
{
    SuperPrint(text, 3, x, y, 1.0f, 1.0f, 1.0f, 1.0f);
}

void xtech_lua_textPrintWP(const std::string &text, float x, float y, float priority)
{
    SuperPrint(text, 3, x, y, 1.0f, 1.0f, 1.0f, 1.0f); //No priority exists, for some reason lol
}
