/*
 * TheXTech - A platform game engine ported from old source code for VB6
 *
 * Copyright (c) 2009-2011 Andrew Spinks, original VB6 code
 * Copyright (c) 2020-2023 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <SDL2/SDL_version.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_hints.h>

#include <FreeImageLite.h>
#include <Logger/logger.h>
#include <Utils/maths.h>

#include "render_sdl.h"
#include "video.h"
#include "core/window.h"
#include "core/render.h"

#include "main/cheat_code.h"

#include "sdl_proxy/sdl_stdinc.h"
#include <fmt_format_ne.h>

#include "graphics.h"
#include "controls.h"
#include "sound.h"

#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

// Workaround for older SDL versions that lacks the floating-point based rects and points
#if SDL_COMPILEDVERSION < SDL_VERSIONNUM(2, 0, 10)
#define XTECH_SDL_NO_RECTF_SUPPORT
#define SDL_RenderCopyF SDL_RenderCopy
#define SDL_RenderCopyExF SDL_RenderCopyEx
#endif



RenderSDL::RenderSDL() :
    AbstractRender_t()
{}

RenderSDL::~RenderSDL()
{
    if(m_window)
        RenderSDL::close();
}

unsigned int RenderSDL::SDL_InitFlags()
{
    return 0;
}

bool RenderSDL::isWorking()
{
    return m_gRenderer && (m_tBuffer || m_tBufferDisabled);
}

bool RenderSDL::initRender(const CmdLineSetup_t &setup, SDL_Window *window)
{
    pLogDebug("Init renderer settings...");

    if(!AbstractRender_t::init())
        return false;

    m_window = window;

    Uint32 renderFlags = 0;

    switch(setup.renderType)
    {
    case RENDER_ACCELERATED_SDL:
    default:
        if(setup.vSync)
        {
            renderFlags = SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC;
            g_videoSettings.renderModeObtained = RENDER_ACCELERATED_SDL;
            pLogDebug("Using accelerated rendering with a vertical synchronization");
            m_gRenderer = SDL_CreateRenderer(window, -1, renderFlags | SDL_RENDERER_TARGETTEXTURE); // Try to make renderer
            if(m_gRenderer)
                break; // All okay
            pLogWarning("Failed to initialize V-Synced renderer, trying to create accelerated renderer...");
        }

        // continue

        renderFlags = SDL_RENDERER_ACCELERATED;
        g_videoSettings.renderModeObtained = RENDER_ACCELERATED_SDL;
        pLogDebug("Using accelerated rendering");
        m_gRenderer = SDL_CreateRenderer(window, -1, renderFlags | SDL_RENDERER_TARGETTEXTURE); // Try to make renderer
        if(m_gRenderer)
            break; // All okay
        pLogWarning("Failed to initialize accelerated renderer, trying to create a software renderer...");

        // fallthrough
    case RENDER_SOFTWARE:
        renderFlags = SDL_RENDERER_SOFTWARE;
        g_videoSettings.renderModeObtained = RENDER_SOFTWARE;
        pLogDebug("Using software rendering");
        m_gRenderer = SDL_CreateRenderer(window, -1, renderFlags | SDL_RENDERER_TARGETTEXTURE); // Try to make renderer
        if(m_gRenderer)
            break; // All okay

        pLogCritical("Unable to create renderer!");
        return false;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

    SDL_RendererInfo ri;
    SDL_GetRendererInfo(m_gRenderer, &ri);
    m_maxTextureWidth = ri.max_texture_width;
    m_maxTextureHeight = ri.max_texture_height;

    m_tBuffer = SDL_CreateTexture(m_gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ScaleWidth, ScaleHeight);
    if(!m_tBuffer)
    {
        pLogWarning("Unable to create texture render buffer: %s", SDL_GetError());
        pLogDebug("Continue without of render to texture. The ability to resize the window will be disabled.");
        SDL_SetWindowResizable(window, SDL_FALSE);
        m_tBufferDisabled = true;
    }

    // Clean-up from a possible start-up junk
    clearBuffer();

    setTargetTexture();
    SDL_SetRenderDrawBlendMode(m_gRenderer, SDL_BLENDMODE_BLEND);

    updateViewport();

    // Clean-up the texture buffer from the same start-up junk
    clearBuffer();

    setTargetScreen();

    repaint();

    return true;
}

void RenderSDL::close()
{
    RenderSDL::clearAllTextures();
    AbstractRender_t::close();

    if(m_tBuffer)
        SDL_DestroyTexture(m_tBuffer);
    m_tBuffer = nullptr;

    if(m_gRenderer)
        SDL_DestroyRenderer(m_gRenderer);
    m_gRenderer = nullptr;
}

void RenderSDL::repaint()
{
#ifdef USE_RENDER_BLOCKING
    if(m_blockRender)
        return;
#endif

    if(XRender::g_BitmaskTexturePresent)
        SuperPrintScreenCenter("Bitmasks using GIFs2PNG in SDL", 5, 2, 1.0f, 0.7f, 0.5f);
    else if(g_ForceBitmaskMerge)
        SuperPrintScreenCenter("GIFs2PNG always simulated in SDL", 5, 2, 1.0f, 0.7f, 0.5f);

    int w, h, off_x, off_y, wDst, hDst;

    #ifdef USE_SCREENSHOTS_AND_RECS
        if(TakeScreen)
        {
            makeShot();
            PlaySoundMenu(SFX_GotItem);
            TakeScreen = false;
        }
    #endif

    setTargetScreen();

#ifdef USE_SCREENSHOTS_AND_RECS
    processRecorder();
#endif

    // Get the size of surface where to draw the scene
    SDL_GetRendererOutputSize(m_gRenderer, &w, &h);

    if(m_tBufferDisabled) /* Render-to-texture is not supported, draw the scene on the screen */
    {
        Controls::RenderTouchControls();
        SDL_RenderPresent(m_gRenderer);
        return;
    }

    // Calculate the size difference factor
    wDst = int(m_scale_x * ScaleWidth);
    hDst = int(m_scale_y * ScaleHeight);

    // Align the rendering scene to the center of screen
    off_x = (w - wDst) / 2;
    off_y = (h - hDst) / 2;

    SDL_SetRenderDrawColor(m_gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(m_gRenderer);

    SDL_Rect destRect = {off_x, off_y, wDst, hDst};
    SDL_Rect sourceRect = {0, 0, ScaleWidth, ScaleHeight};

    SDL_SetTextureColorMod(m_tBuffer, 255, 255, 255);
    SDL_SetTextureAlphaMod(m_tBuffer, 255);
    SDL_RenderCopyEx(m_gRenderer, m_tBuffer, &sourceRect, &destRect, 0.0, nullptr, SDL_FLIP_NONE);

    Controls::RenderTouchControls();

    SDL_RenderPresent(m_gRenderer);
}

void RenderSDL::updateViewport()
{
    int   render_w, render_h;

    getRenderSize(&render_w, &render_h);

    D_pLogDebug("Updated render size: %d x %d", render_w, render_h);

    // quickly update the HiDPI scaling factor
    int window_w, window_h;
    XWindow::getWindowSize(&window_w, &window_h);
    m_hidpi_x = (float)render_w / (float)window_w;
    m_hidpi_y = (float)render_h / (float)window_h;

    float scale_x = (float)render_w / ScreenW;
    float scale_y = (float)render_h / ScreenH;

    float scale = SDL_min(scale_x, scale_y);

    if(g_videoSettings.scaleMode == SCALE_FIXED_05X && scale > 0.5f)
        scale = 0.5f;
    if(g_videoSettings.scaleMode == SCALE_DYNAMIC_INTEGER && scale > 1.f)
        scale = std::floor(scale);
    if(g_videoSettings.scaleMode == SCALE_FIXED_1X && scale > 1.f)
        scale = 1.f;
    if(g_videoSettings.scaleMode == SCALE_FIXED_2X && scale > 2.f)
        scale = 2.f;

    int game_w = scale * ScreenW;
    int game_h = scale * ScreenH;

    m_scale_x = scale;
    m_scale_y = scale;
    m_viewport_scale_x = scale;
    m_viewport_scale_y = scale;

    m_viewport_offset_x = 0;
    m_viewport_offset_y = 0;
    m_viewport_offset_x_cur = 0;
    m_viewport_offset_y_cur = 0;
    m_viewport_offset_ignore = false;

    m_offset_x = (render_w - game_w) / 2;
    m_offset_y = (render_h - game_h) / 2;

    m_viewport_x = 0;
    m_viewport_y = 0;
    m_viewport_w = ScreenW;
    m_viewport_h = ScreenH;

    // update render targets
    if(m_current_scale_mode != g_videoSettings.scaleMode)
    {
#ifdef USE_SCREENSHOTS_AND_RECS
        // invalidates GIF recorder handle
        if(recordInProcess())
            toggleGifRecorder();
#endif

        // update video settings
        if(g_videoSettings.scaleMode == SCALE_DYNAMIC_LINEAR || scale < 0.5f)
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
        else
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

        SDL_DestroyTexture(m_tBuffer);
        m_tBuffer = SDL_CreateTexture(m_gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ScreenW, ScreenH);
        SDL_SetRenderTarget(m_gRenderer, m_tBuffer);

        // reset scaling setting for images loaded later
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

        m_current_scale_mode = g_videoSettings.scaleMode;
    }
}

void RenderSDL::resetViewport()
{
    // FIXME: Clarify the version of SDL2 with the buggy viewport
//#if SDL_COMPILEDVERSION < SDL_VERSIONNUM(2, 0, 22)
    // set to an alt viewport as a workaround for SDL bug (doesn't allow resizing viewport without changing position)
    SDL_Rect altViewport = {m_viewport_x + 1, m_viewport_y + 1, 1, 1};
    SDL_RenderSetViewport(m_gRenderer, &altViewport);
//#endif

    SDL_RenderSetViewport(m_gRenderer, nullptr);
}

void RenderSDL::setViewport(int x, int y, int w, int h)
{
    SDL_Rect topLeftViewport;

    // FIXME: Clarify the version of SDL2 with the buggy viewport
//#if SDL_COMPILEDVERSION < SDL_VERSIONNUM(2, 0, 22)
    // set to an alt viewport as a workaround for SDL bug (doesn't allow resizing viewport without changing position)
    topLeftViewport = {m_viewport_x + 1, m_viewport_y + 1, 1, 1};
    SDL_RenderSetViewport(m_gRenderer, &topLeftViewport);
//#endif

    topLeftViewport = {x, y, w, h};
    SDL_RenderSetViewport(m_gRenderer, &topLeftViewport);

    m_viewport_x = x;
    m_viewport_y = y;
    m_viewport_w = w;
    m_viewport_h = h;
}

void RenderSDL::offsetViewport(int x, int y)
{
    if(m_viewport_offset_x != x || m_viewport_offset_y != y)
    {
        m_viewport_offset_x_cur = x;
        m_viewport_offset_y_cur = y;
        m_viewport_offset_x = m_viewport_offset_ignore ? 0 : m_viewport_offset_x_cur;
        m_viewport_offset_y = m_viewport_offset_ignore ? 0 : m_viewport_offset_y_cur;
    }
}

void RenderSDL::offsetViewportIgnore(bool en)
{
    if(m_viewport_offset_ignore != en)
    {
        m_viewport_offset_x = en ? 0 : m_viewport_offset_x_cur;
        m_viewport_offset_y = en ? 0 : m_viewport_offset_y_cur;
    }
    m_viewport_offset_ignore = en;
}

void RenderSDL::getRenderSize(int* w, int* h)
{
    // make sure we're operating on default target (same as SDL)
    SDL_Texture* saved_target = SDL_GetRenderTarget(m_gRenderer);
    if(saved_target)
        SDL_SetRenderTarget(m_gRenderer, NULL);

    SDL_GetRendererOutputSize(m_gRenderer, w, h);

    if (saved_target)
        SDL_SetRenderTarget(m_gRenderer, saved_target);
}

void RenderSDL::mapToScreen(int x, int y, int *dx, int *dy)
{
    *dx = static_cast<int>((static_cast<float>(x) * m_hidpi_x - m_offset_x) / m_viewport_scale_x);
    *dy = static_cast<int>((static_cast<float>(y) * m_hidpi_y - m_offset_y) / m_viewport_scale_y);
}

void RenderSDL::mapFromScreen(int scr_x, int scr_y, int *window_x, int *window_y)
{
    *window_x = ((float)scr_x * m_viewport_scale_x + m_offset_x) / m_hidpi_x;
    *window_y = ((float)scr_y * m_viewport_scale_y + m_offset_y) / m_hidpi_y;
}

void RenderSDL::setTargetTexture()
{
    if(m_tBufferDisabled || m_recentTarget == m_tBuffer)
        return;
    SDL_SetRenderTarget(m_gRenderer, m_tBuffer);
    m_recentTarget = m_tBuffer;
}

void RenderSDL::setTargetScreen()
{
    if(m_tBufferDisabled || m_recentTarget == nullptr)
        return;
    SDL_SetRenderTarget(m_gRenderer, nullptr);
    m_recentTarget = nullptr;
}

void RenderSDL::loadTextureInternal(StdPicture &target, uint32_t width, uint32_t height, uint8_t *RGBApixels, uint32_t pitch)
{
    SDL_Surface *surface;
    SDL_Texture *texture = nullptr;

    surface = SDL_CreateRGBSurfaceFrom(RGBApixels,
                                       static_cast<int>(width),
                                       static_cast<int>(height),
                                       32,
                                       static_cast<int>(pitch),
                                       FI_RGBA_RED_MASK,
                                       FI_RGBA_GREEN_MASK,
                                       FI_RGBA_BLUE_MASK,
                                       FI_RGBA_ALPHA_MASK);
    if(surface)
        texture = SDL_CreateTextureFromSurface(m_gRenderer, surface);

    SDL_FreeSurface(surface);

    if(!texture)
    {
        pLogWarning("Render SDL: Failed to load texture! (%s)", SDL_GetError());
        target.inited = false;
        return;
    }

    target.d.texture = texture;

    target.d.nOfColors = GL_RGBA;
    target.d.format = GL_BGRA;

    target.d.w_scale = static_cast<float>(width) / target.w;
    target.d.h_scale = static_cast<float>(height) / target.h;

    m_loadedPictures.insert(&target);
    D_pLogDebug("RenderSDL: loading texture at %p, new texture count %d...", &target, (int)m_loadedPictures.size());

    target.inited = true;

#if defined(__APPLE__) && defined(USE_APPLE_X11)
    SDL_GL_UnbindTexture(texture); // Unbind texture after it got been loaded (otherwise a white screen will happen)
#endif
}

void RenderSDL::unloadTexture(StdPicture &tx)
{
    auto corpseIt = m_loadedPictures.find(&tx);
    if(corpseIt != m_loadedPictures.end())
        m_loadedPictures.erase(corpseIt);

    D_pLogDebug("RenderSDL: unloading texture at %p, new texture count %d...", &tx, (int)m_loadedPictures.size());

    if(tx.d.hasTexture())
        SDL_DestroyTexture(tx.d.texture);

    tx.d = StdPictureData();

    if(!tx.l.canLoad())
        static_cast<StdPicture_Sub&>(tx) = StdPicture_Sub();

    return;
}

void RenderSDL::clearAllTextures()
{
    for(StdPicture *tx : m_loadedPictures)
    {
        D_pLogDebug("RenderSDL: unloading texture at %p on clearAllTextures()", tx);

        if(tx->d.hasTexture())
            SDL_DestroyTexture(tx->d.texture);

        tx->d = StdPictureData();

        if(!tx->l.canLoad())
            static_cast<StdPicture_Sub&>(*tx) = StdPicture_Sub();
    }

    m_loadedPictures.clear();
}

void RenderSDL::clearBuffer()
{
#ifdef USE_RENDER_BLOCKING
    SDL_assert(!m_blockRender);
#endif
    SDL_SetRenderDrawColor(m_gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(m_gRenderer);
}

void RenderSDL::renderRect(int x, int y, int w, int h, float red, float green, float blue, float alpha, bool filled)
{
#ifdef USE_RENDER_BLOCKING
    SDL_assert(!m_blockRender);
#endif
    SDL_Rect aRect = {x + m_viewport_offset_x,
                      y + m_viewport_offset_y,
                      w, h};
    SDL_SetRenderDrawColor(m_gRenderer,
                           static_cast<unsigned char>(255.f * red),
                           static_cast<unsigned char>(255.f * green),
                           static_cast<unsigned char>(255.f * blue),
                           static_cast<unsigned char>(255.f * alpha)
                          );

    if(filled)
        SDL_RenderFillRect(m_gRenderer, &aRect);
    else
        SDL_RenderDrawRect(m_gRenderer, &aRect);
}

void RenderSDL::renderRectBR(int _left, int _top, int _right, int _bottom, float red, float green, float blue, float alpha)
{
#ifdef USE_RENDER_BLOCKING
    SDL_assert(!m_blockRender);
#endif
    SDL_Rect aRect = {_left + m_viewport_offset_x,
                      _top + m_viewport_offset_y,
                      _right - _left, _bottom - _top};
    SDL_SetRenderDrawColor(m_gRenderer,
                           static_cast<unsigned char>(255.f * red),
                           static_cast<unsigned char>(255.f * green),
                           static_cast<unsigned char>(255.f * blue),
                           static_cast<unsigned char>(255.f * alpha)
                          );
    SDL_RenderFillRect(m_gRenderer, &aRect);
}

void RenderSDL::renderCircle(int cx, int cy, int radius, float red, float green, float blue, float alpha, bool filled)
{
#ifdef USE_RENDER_BLOCKING
    SDL_assert(!m_blockRender);
#endif
    UNUSED(filled);

    if(radius <= 0)
        return; // Nothing to draw

    SDL_SetRenderDrawColor(m_gRenderer,
                               static_cast<unsigned char>(255.f * red),
                               static_cast<unsigned char>(255.f * green),
                               static_cast<unsigned char>(255.f * blue),
                               static_cast<unsigned char>(255.f * alpha)
                          );

    cx += m_viewport_offset_x;
    cy += m_viewport_offset_y;

    double dy = 1;
    do //for(double dy = 1; dy <= radius; dy += 1.0)
    {
        double dx = std::floor(std::sqrt((2.0 * radius * dy) - (dy * dy)));
        SDL_RenderDrawLine(m_gRenderer,
                           int(cx - dx),
                           int(cy + dy - radius),
                           int(cx + dx),
                           int(cy + dy - radius));

        if(dy < radius) // Don't cross lines
        {
            SDL_RenderDrawLine(m_gRenderer,
                               int(cx - dx),
                               int(cy - dy + radius),
                               int(cx + dx),
                               int(cy - dy + radius));
        }

        dy += 1.0;
    } while(dy <= radius);
}

void RenderSDL::renderCircleHole(int cx, int cy, int radius, float red, float green, float blue, float alpha)
{
#ifdef USE_RENDER_BLOCKING
    SDL_assert(!m_blockRender);
#endif

    if(radius <= 0)
        return; // Nothing to draw

    SDL_SetRenderDrawColor(m_gRenderer,
                               static_cast<unsigned char>(255.f * red),
                               static_cast<unsigned char>(255.f * green),
                               static_cast<unsigned char>(255.f * blue),
                               static_cast<unsigned char>(255.f * alpha)
                          );

    cx += m_viewport_offset_x;
    cy += m_viewport_offset_y;

    double dy = 1;
    do //for(double dy = 1; dy <= radius; dy += 1.0)
    {
        double dx = std::floor(std::sqrt((2.0 * radius * dy) - (dy * dy)));

        SDL_RenderDrawLine(m_gRenderer,
                           int(cx - radius),
                           int(cy + dy - radius),
                           int(cx - dx),
                           int(cy + dy - radius));

        SDL_RenderDrawLine(m_gRenderer,
                           int(cx + dx),
                           int(cy + dy - radius),
                           int(cx + radius),
                           int(cy + dy - radius));


        if(dy < radius) // Don't cross lines
        {
            SDL_RenderDrawLine(m_gRenderer,
                               int(cx - radius),
                               int(cy - dy + radius),
                               int(cx - dx),
                               int(cy - dy + radius));

            SDL_RenderDrawLine(m_gRenderer,
                               int(cx + dx),
                               int(cy - dy + radius),
                               int(cx + radius),
                               int(cy - dy + radius));
        }

        dy += 1.0;
    } while(dy <= radius);
}



void RenderSDL::txColorMod(StdPictureData &tx, float red, float green, float blue, float alpha)
{
    uint8_t modColor[4] = {static_cast<unsigned char>(255.f * red),
                           static_cast<unsigned char>(255.f * green),
                           static_cast<unsigned char>(255.f * blue),
                           static_cast<unsigned char>(255.f * alpha)};

    if(SDL_memcmp(tx.modColor, modColor, 3) != 0)
    {
        SDL_SetTextureColorMod(tx.texture, modColor[0], modColor[1], modColor[2]);
        tx.modColor[0] = modColor[0];
        tx.modColor[1] = modColor[1];
        tx.modColor[2] = modColor[2];
    }

    if(tx.modColor[3] != modColor[3])
    {
        SDL_SetTextureAlphaMod(tx.texture, modColor[3]);
        tx.modColor[3] = modColor[3];
    }
}

void RenderSDL::renderTextureScaleEx(double xDstD, double yDstD, double wDstD, double hDstD,
                                       StdPicture &tx,
                                       int xSrc, int ySrc,
                                       int wSrc, int hSrc,
                                       double rotateAngle, FPoint_t *center, unsigned int flip,
                                       float red, float green, float blue, float alpha)
{
#ifdef USE_RENDER_BLOCKING
    SDL_assert(!m_blockRender);
#endif
    if(!tx.inited)
        return;

    if(!tx.d.texture && tx.l.lazyLoaded)
        lazyLoad(tx);

    if(!tx.d.texture)
    {
        D_pLogWarningNA("Attempt to render an empty texture!");
        return;
    }

    SDL_assert_release(tx.d.texture);

    int xDst = Maths::iRound(xDstD);
    int yDst = Maths::iRound(yDstD);
    int wDst = Maths::iRound(wDstD);
    int hDst = Maths::iRound(hDstD);

    // Don't go more than size of texture
    if(xSrc + wSrc > tx.w)
    {
        wSrc = tx.w - xSrc;
        if(wSrc < 0)
            wSrc = 0;
    }
    if(ySrc + hSrc > tx.h)
    {
        hSrc = tx.h - ySrc;
        if(hSrc < 0)
            hSrc = 0;
    }

#ifndef XTECH_SDL_NO_RECTF_SUPPORT
    SDL_FRect destRect = {(float)xDst + m_viewport_offset_x,
                          (float)yDst + m_viewport_offset_y,
                          (float)wDst,
                          (float)hDst};
    SDL_FPoint *centerD = (SDL_FPoint*)center;
#else
    SDL_Rect destRect = {(int)xDst + m_viewport_offset_x,
                         (int)yDst + m_viewport_offset_y,
                         (int)wDst,
                         (int)hDst};
    SDL_Point centerI = {center ? Maths::iRound(center->x) : 0,
                         center ? Maths::iRound(center->y) : 0};
    SDL_Point *centerD = center ? &centerI : nullptr;
#endif

    SDL_Rect sourceRect;
    sourceRect = {int(tx.d.w_scale * xSrc), int(tx.d.h_scale * ySrc),
                  int(tx.d.w_scale * wSrc), int(tx.d.h_scale * hSrc)};

    txColorMod(tx.d, red, green, blue, alpha);
    SDL_RenderCopyExF(m_gRenderer, tx.d.texture, &sourceRect, &destRect,
                      rotateAngle, centerD, static_cast<SDL_RendererFlip>(flip));
}

void RenderSDL::renderTextureScale(double xDst, double yDst, double wDst, double hDst,
                                     StdPicture &tx,
                                     float red, float green, float blue, float alpha)
{
#ifdef USE_RENDER_BLOCKING
    SDL_assert(!m_blockRender);
#endif
    const unsigned int flip = SDL_FLIP_NONE;

    if(!tx.inited)
        return;

    if(!tx.d.texture && tx.l.lazyLoaded)
        lazyLoad(tx);

    if(!tx.d.texture)
    {
        D_pLogWarningNA("Attempt to render an empty texture!");
        return;
    }

#ifndef XTECH_SDL_NO_RECTF_SUPPORT
    SDL_FRect destRect = {Maths::fRound(xDst) + m_viewport_offset_x,
                          Maths::fRound(yDst) + m_viewport_offset_y,
                          (float)wDst,
                          (float)hDst};
#else
    SDL_Rect destRect = {Maths::iRound(xDst) + m_viewport_offset_x,
                         Maths::iRound(yDst) + m_viewport_offset_y,
                         (int)wDst,
                         (int)hDst};
#endif

    SDL_Rect sourceRect;
    sourceRect = {0, 0, tx.w, tx.h};

    txColorMod(tx.d, red, green, blue, alpha);
    SDL_RenderCopyExF(m_gRenderer, tx.d.texture, &sourceRect, &destRect,
                      0.0, nullptr, static_cast<SDL_RendererFlip>(flip));
}

void RenderSDL::renderTexture(double xDstD, double yDstD, double wDstD, double hDstD,
                                StdPicture &tx,
                                int xSrc, int ySrc,
                                float red, float green, float blue, float alpha)
{
#ifdef USE_RENDER_BLOCKING
    SDL_assert(!m_blockRender);
#endif
    if(!tx.inited)
        return;

    if(!tx.d.texture && tx.l.lazyLoaded)
        lazyLoad(tx);

    if(!tx.d.texture)
    {
        D_pLogWarningNA("Attempt to render an empty texture!");
        return;
    }

    SDL_assert_release(tx.d.texture);

    int xDst = Maths::iRound(xDstD);
    int yDst = Maths::iRound(yDstD);
    int wDst = Maths::iRound(wDstD);
    int hDst = Maths::iRound(hDstD);

    // Don't go more than size of texture
    if(xSrc + wDst > tx.w)
    {
        wDst = tx.w - xSrc;
        if(wDst < 0)
            wDst = 0;
    }

    if(ySrc + hDst > tx.h)
    {
        hDst = tx.h - ySrc;
        if(hDst < 0)
            hDst = 0;
    }

#ifndef XTECH_SDL_NO_RECTF_SUPPORT
    SDL_FRect destRect = {(float)xDst + m_viewport_offset_x,
                          (float)yDst + m_viewport_offset_y,
                          (float)wDst,
                          (float)hDst};
#else
    SDL_Rect destRect = {(int)xDst + m_viewport_offset_x,
                         (int)yDst + m_viewport_offset_y,
                         (int)wDst,
                         (int)hDst};
#endif

    SDL_Rect sourceRect;

    sourceRect = {int(tx.d.w_scale * xSrc), int(tx.d.h_scale * ySrc),
                  int(tx.d.w_scale * wDst), int(tx.d.h_scale * hDst)};

    txColorMod(tx.d, red, green, blue, alpha);
    SDL_RenderCopyF(m_gRenderer, tx.d.texture, &sourceRect, &destRect);
}

void RenderSDL::renderTextureFL(double xDstD, double yDstD, double wDstD, double hDstD,
                                  StdPicture &tx,
                                  int xSrc, int ySrc,
                                  double rotateAngle, FPoint_t *center, unsigned int flip,
                                  float red, float green, float blue, float alpha)
{
#ifdef USE_RENDER_BLOCKING
    SDL_assert(!m_blockRender);
#endif
    if(!tx.inited)
        return;

    if(!tx.d.texture && tx.l.lazyLoaded)
        lazyLoad(tx);

    if(!tx.d.texture)
    {
        D_pLogWarningNA("Attempt to render an empty texture!");
        return;
    }

    SDL_assert_release(tx.d.texture);

    int xDst = Maths::iRound(xDstD);
    int yDst = Maths::iRound(yDstD);
    int wDst = Maths::iRound(wDstD);
    int hDst = Maths::iRound(hDstD);

    // Don't go more than size of texture
    if(xSrc + wDst > tx.w)
    {
        wDst = tx.w - xSrc;
        if(wDst < 0)
            wDst = 0;
    }

    if(ySrc + hDst > tx.h)
    {
        hDst = tx.h - ySrc;
        if(hDst < 0)
            hDst = 0;
    }

#ifndef XTECH_SDL_NO_RECTF_SUPPORT
    SDL_FRect destRect = {(float)xDst + m_viewport_offset_x,
                          (float)yDst + m_viewport_offset_y,
                          (float)wDst,
                          (float)hDst};
    SDL_FPoint *centerD = (SDL_FPoint*)center;
#else
    SDL_Rect destRect = {(int)xDst + m_viewport_offset_x,
                         (int)yDst + m_viewport_offset_y,
                         (int)wDst,
                         (int)hDst};
    SDL_Point centerI = {center ? Maths::iRound(center->x) : 0,
                         center ? Maths::iRound(center->y) : 0};
    SDL_Point *centerD = center ? &centerI : nullptr;
#endif

    SDL_Rect sourceRect;

    sourceRect = {int(tx.d.w_scale * xSrc), int(tx.d.h_scale * ySrc),
                  int(tx.d.w_scale * wDst), int(tx.d.h_scale * hDst)};

    txColorMod(tx.d, red, green, blue, alpha);
    SDL_RenderCopyExF(m_gRenderer, tx.d.texture, &sourceRect, &destRect,
                      rotateAngle, centerD, static_cast<SDL_RendererFlip>(flip));
}

void RenderSDL::renderTexture(float xDst, float yDst,
                                StdPicture &tx,
                                float red, float green, float blue, float alpha)
{
#ifdef USE_RENDER_BLOCKING
    SDL_assert(!m_blockRender);
#endif
    const unsigned int flip = SDL_FLIP_NONE;

    if(!tx.inited)
        return;

    if(!tx.d.texture && tx.l.lazyLoaded)
        lazyLoad(tx);

    if(!tx.d.texture)
    {
        D_pLogWarningNA("Attempt to render an empty texture!");
        return;
    }

#ifndef XTECH_SDL_NO_RECTF_SUPPORT
    SDL_FRect destRect = {Maths::fRound(xDst), Maths::fRound(yDst), (float)tx.w, (float)tx.h};
#else
    SDL_Rect destRect = {Maths::iRound(xDst), Maths::iRound(yDst), tx.w, tx.h};
#endif

    SDL_Rect sourceRect;
    sourceRect = {0, 0, tx.w, tx.h};

    txColorMod(tx.d, red, green, blue, alpha);
    SDL_RenderCopyExF(m_gRenderer, tx.d.texture, &sourceRect, &destRect,
                      0.0, nullptr, static_cast<SDL_RendererFlip>(flip));
}

void RenderSDL::getScreenPixels(int x, int y, int w, int h, unsigned char *pixels)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

#ifndef XTECH_SDL_NO_RECTF_SUPPORT
    SDL_RenderFlush(m_gRenderer);
#endif
    SDL_RenderReadPixels(m_gRenderer,
                         &rect,
                         SDL_PIXELFORMAT_BGR24,
                         pixels,
                         w * 3 + (w % 4));
}

void RenderSDL::getScreenPixelsRGBA(int x, int y, int w, int h, unsigned char *pixels)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

#ifndef XTECH_SDL_NO_RECTF_SUPPORT
    SDL_RenderFlush(m_gRenderer);
#endif
    SDL_RenderReadPixels(m_gRenderer,
                         &rect,
                         SDL_PIXELFORMAT_ABGR8888,
                         pixels,
                         w * 4);
}

int RenderSDL::getPixelDataSize(const StdPicture &tx)
{
    if(!tx.d.texture)
        return 0;
    return (tx.w * tx.h * 4);
}

void RenderSDL::getPixelData(const StdPicture &tx, unsigned char *pixelData)
{
    int pitch, w, h, a;
    void *pixels;

    if(!tx.d.texture)
        return;

    SDL_SetTextureBlendMode(tx.d.texture, SDL_BLENDMODE_BLEND);
    SDL_QueryTexture(tx.d.texture, nullptr, &a, &w, &h);
    SDL_LockTexture(tx.d.texture, nullptr, &pixels, &pitch);
    std::memcpy(pixelData, pixels, static_cast<size_t>(pitch) * h);
    SDL_UnlockTexture(tx.d.texture);
}
