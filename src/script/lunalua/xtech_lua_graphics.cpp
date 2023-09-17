#include "sdl_proxy/sdl_timer.h"
#include "sdl_proxy/sdl_assert.h"
#include "sdl_proxy/sdl_stdinc.h"
#include "sdl_proxy/sdl_types.h"

#include "../../core/msgbox.h"

#include "../../graphics.h"
#include "../../gfx.h"
#include "../../load_gfx.h"

#include "../../globals.h"
#include "../../global_constants.h"
#include "../../global_dirs.h"

#include "../../core/render.h"
#include "../../core/events.h"
#include "../../std_picture.h"
#include "../../load_gfx.h"

#include <Utils/files.h>
#include <Utils/dir_list_ci.h>
#include <DirManager/dirman.h>

static inline void s_find_image(std::string& dest, DirListCI& CurDir, std::string baseName)
{
#if defined(X_IMG_EXT) && !defined(X_NO_PNG_GIF)
    int s = baseName.size();
    baseName += X_IMG_EXT;
    dest = CurDir.resolveFileCaseExistsAbs(baseName);
    if(dest.empty())
    {
        baseName.resize(s);
        baseName += ".png";
        dest = CurDir.resolveFileCaseExistsAbs(baseName);
    }
#elif defined(X_IMG_EXT)
    baseName += X_IMG_EXT;
    dest = CurDir.resolveFileCaseExistsAbs(baseName);
#else
    baseName += ".png";
    dest = CurDir.resolveFileCaseExistsAbs(baseName);
#endif
}

void xtech_lua_textPrint(const std::string &text, float x, float y)
{
    SuperPrint(text, 3, x, y, 1.0f, 1.0f, 1.0f, 1.0f);
}

void xtech_lua_textPrintWP(const std::string &text, float x, float y, float priority)
{
    SuperPrint(text, 3, x, y, 1.0f, 1.0f, 1.0f, 1.0f); //No priority exists, for some reason lol
}

void xtech_lua_graphics_loadImage(std::string directory, std::string img, int A)
{
    std::string p;
    DirListCI CurDir;
    CurDir.setCurDir(AppPath + directory);
    s_find_image(p, CurDir, img);
    if(!p.empty())
    {
        XRender::lazyLoadPicture(GFXLuaBMP[A], p);
        GFXLuaWidth[A] = GFXLuaBMP[A].w;
        GFXLuaHeight[A] = GFXLuaBMP[A].h;
    }
    else
    {
        XMsgBox::simpleMsgBox(XMsgBox::MESSAGEBOX_ERROR, "Error", "Could not load image: " + directory + img);
    }
}

void xtech_lua_graphics_drawImage(int A, double x, double y)
{
    XRender::renderTexture(x, y, GFXLuaWidth[A], GFXLuaHeight[A], GFXLuaBMP[A], 0, 0);
}

void xtech_lua_graphics_drawImage(int A, double x, double y, double width, double height)
{
    XRender::renderTexture(x, y, width, height, GFXLuaBMP[A], 0, 0);
}

void xtech_lua_graphics_drawImage(int A, double x, double y, double width, double height, int xOffset, int yOffset)
{
    XRender::renderTexture(x, y, width, height, GFXLuaBMP[A], xOffset, yOffset);
}

void xtech_lua_graphics_drawImage(int A, double x, double y, double width, double height, double opacity)
{
    XRender::renderTexture(x, y, width, height, GFXLuaBMP[A], 0, 0, 1.0f, 1.0f, 1.0f, opacity);
}

void xtech_lua_graphics_drawImage(int A, double x, double y, double width, double height, int xOffset, int yOffset, double opacity)
{
    XRender::renderTexture(x, y, width, height, GFXLuaBMP[A], xOffset, yOffset, 1.0f, 1.0f, 1.0f, opacity);
}

void xtech_lua_graphics_drawRectangle(int x, int y, int w, int h, float color1, float color2, float color3, float opacity, bool filled)
{
    XRender::renderRect(x, y, w, h, color1, color2, color3, opacity, filled);
}

void xtech_lua_graphics_drawRectangle(int x, int y, int w, int h, float color1, float color2, float color3, bool filled)
{
    XRender::renderRect(x, y, w, h, color1, color2, color3, 1.0f, filled);
}

void xtech_lua_graphics_drawScreen(float color1, float color2, float color3, float opacity, bool filled)
{
    XRender::renderRect(0, 0, ScreenW, ScreenH, color1, color2, color3, opacity, filled);
}

void xtech_lua_graphics_drawScreen(float color1, float color2, float color3, bool filled)
{
    XRender::renderRect(0, 0, ScreenW, ScreenH, color1, color2, color3, 1.0f, filled);
}