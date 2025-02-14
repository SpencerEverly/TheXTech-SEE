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

#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <string>
#include "location.h"
#include "globals.h"

enum
{
    SHAKE_RANDOM = 0,
    SHAKE_SEQUENTIAL
};

void doShakeScreen(int force, int type);
void doShakeScreen(int forceX, int forceY, int type, int duration, double decay);
void doShakeScreenClear();

// Public Sub UpdateGraphics2() 'draws GFX to screen when on the world map/world map editor
// draws GFX to screen when on the world map/world map editor
void UpdateGraphics2(bool skipRepaint = false);
// Unpack all visible lazily-loaded graphics
void GraphicsLazyPreLoad();
// Public Sub UpdateGraphics() 'This draws the graphic to the screen when in a level/game menu/outro/level editor
// This draws the graphic to the screen when in a level/game menu/outro/level editor
void UpdateGraphics(bool skipRepaint = false);
// Public Sub GetvScreen(A As Integer) ' Get the screen position
//  Get the screen position
void GetvScreen(vScreen_t& vscreen);
// Public Sub GetvScreenAverage() ' Get the average screen position for all players
//  Get the average screen position for all players
void GetvScreenAverage(vScreen_t& vscreen);
// Public Sub GetvScreenAverage2() ' Get the average screen position for all players with no level edge detection
//  Get the average screen position for all players with no level edge detection
void GetvScreenAverage2(vScreen_t& vscreen);
// Public Sub SetupGraphics()
//! DUMMY AND USELESS
void SetupGraphics();
// Public Sub SetupEditorGraphics()
//! DUMMY AND USELESS
void SetupEditorGraphics();
// Public Sub SetupScreens()
void SetupScreens(bool reset = true);
// Public Sub DynamicScreen() 'for the split screen stuff
// for the split screen stuff
void DynamicScreen(Screen_t& screen, bool mute = false);

// NEW: moves qScreen towards vScreen, now including the screen size
bool Update_qScreen(int Z, int camRate = 2, int resizeRate = 2);

// Public Sub SuperPrint(SuperWords As String, Font As Integer, X As Single, Y As Single) 'prints text to the screen
// prints text to the screen
int SuperTextPixLen(int SuperN, const char* SuperChars, int Font);
void SuperPrint(int SuperN, const char* SuperChars, int Font, float X, float Y, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);
void SuperPrintRightAlign(int SuperN, const char* SuperChars, int Font, float X, float Y, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);
void SuperPrintCenter(int SuperN, const char* SuperChars, int Font, float X, float Y, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);
void SuperPrintScreenCenter(int SuperN, const char* SuperChars, int Font, float Y, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);

int SuperTextPixLen(const char* SuperChars, int Font);
void SuperPrint(const char* SuperChars, int Font, float X, float Y, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);
void SuperPrintRightAlign(const char* SuperChars, int Font, float X, float Y, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);
void SuperPrintCenter(const char* SuperChars, int Font, float X, float Y, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);
void SuperPrintScreenCenter(const char* SuperChars, int Font, float Y, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);

int SuperTextPixLen(const std::string &SuperWords, int Font);
void SuperPrint(const std::string &SuperWords, int Font, float X, float Y, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);
void SuperPrintRightAlign(const std::string &SuperWords, int Font, float X, float Y, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);
void SuperPrintCenter(const std::string &SuperWords, int Font, float X, float Y, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);
void SuperPrintScreenCenter(const std::string &SuperWords, int Font, float Y, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);

/*!
 * \brief Builds a UTF8 character positions map to simplify the text processing.
 * \param SuperText Input text line
 * \param outMap Output character positions map
 */
void BuildUTF8CharMap(const std::string& SuperText, UTF8CharMap_t &outMap);
/*!
 * \brief Print the message box with a multi-line text inside using a plain string itself
 * \param SuperText The multi-line string
 */
void DrawMessage(const std::string& SuperText);
/*!
 * \brief Print the mssage box with a multi-line text inside using a pre-built character positions map
 * \param SuperTextMap The UTF8 character positions map of a multi-line string
 */
void DrawMessage(const UTF8CharMap_t& SuperTextMap);

// Public Sub SetRes()
void SetRes();
// Public Function CheckKey(newStrizzle As String) As String
//std::string CheckKey(std::string newStrizzle); // USELESS

// moved to gfx_special_frames.h
// Private Sub SpecialFrames() 'update frames for special things such as coins and kuribo's shoe
// extern void SpecialFrames();//PRIVATE
// update frames for special things such as coins and kuribo's shoe

// Public Sub DrawBackground(S As Integer, Z As Integer) 'draws the background to the screen
// draws the background to the screen
void DrawBackground(int S, int Z);
// Public Sub PlayerWarpGFX(A As Integer, tempLocation As Location, X2 As Single, Y2 As Single)
void PlayerWarpGFX(int A, Location_t &tempLocation, float &X2, float &Y2);
// Public Sub NPCWarpGFX(A As Integer, tempLocation As Location, X2 As Single, Y2 As Single)
void NPCWarpGFX(int A, Location_t &tempLocation, float &X2, float &Y2);
// Public Sub ChangeScreen() 'change from fullscreen to windowed mode
// change from fullscreen to windowed mode
void ChangeScreen();
// Public Sub GetvScreenCredits() ' Get the average screen position for all players for the games outro
//  Get the average screen position for all players for the games outro
void GetvScreenCredits(vScreen_t& vscreen);
// Public Sub DoCredits() 'print credits
// print credits
void DoCredits();
void DrawCredits();
// Public Sub DrawInterface(Z As Integer, numScreens) 'draws the games interface
// draws the games interface
void DrawInterface(int Z, int numScreens);
// NEW: draws the level editor interface on vScreen Z
void DrawEditorLevel(int Z);
// NEW: draws the level editor UI
void DrawEditorLevel_UI();
// NEW: draws the world editor interface
void DrawEditorWorld();

#if 0
// old, 100x100-based functions

// Public Function pfrX(plrFrame As Integer) As Integer
//! Get X offset at the player sprite (old call, required to add 100 into source value)
int pfrXo(int plrFrame);
// Public Function pfrY(plrFrame As Integer) As Integer
//! Get Y offset at the player sprite (old call, required to add 100 into source value)
int pfrYo(int plrFrame);
//! Get X offset at the player sprite
int pfrX(int plrFrame);
//! Get Y offset at the player sprite
int pfrY(int plrFrame);
#endif

// NEW: defined in gfx_draw_player.cpp

//! Get left pixel at the player sprite
int pfrX(const StdPicture& tx, const Player_t& p);
//! Get top pixel at the player sprite
int pfrY(const StdPicture& tx, const Player_t& p);
//! Get width at the player sprite
int pfrW(const StdPicture& tx, const Player_t& p);
//! Get height at the player sprite
int pfrH(const StdPicture& tx, const Player_t& p);
//! Get x offset that should be ADDED to the player position to draw the sprite
int pfrOffX(const StdPicture& tx, const Player_t& p);
//! Get y offset that should be ADDED to the player position to draw the sprite
int pfrOffY(const StdPicture& tx, const Player_t& p);

// Public Sub GameThing()
/*!
 * \brief Draw the level enter scene
 * \param noSetup Avoid player settings re-setup
 */
void GameThing(int waitms = 0, int fadeSpeed = 0);
// Public Sub DrawPlayer(A As Integer, Z As Integer)
void DrawPlayer(const int A, const int Z);
void DrawPlayer(Player_t &p, const int Z);
// Public Sub ScreenShot()
void ScreenShot();
// Public Sub DrawFrozenNPC(Z As Integer, A As Integer)
void DrawFrozenNPC(int Z, int A);

// NEW: draws device battery status in top-right corner of screen
void DrawDeviceBattery();


#endif // GRAPHICS_H
