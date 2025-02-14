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


#include <Logger/logger.h>
#include <Integrator/integrator.h>
#include <pge_delay.h>

#include "../globals.h"
#include "../config.h"
#include "../compat.h"
#include "../frame_timer.h"
#include "../game_main.h"
#include "../sound.h"
#include "../controls.h"
#include "../effect.h"
#include "../graphics.h"
#include "../blocks.h"
#include "../npc.h"
#include "../layers.h"
#include "../player.h"
#include "../editor.h"
#include "../core/render.h"
#include "../core/events.h"
#include "../core/window.h"
#include "game_globals.h"
#include "world_globals.h"
#include "speedrunner.h"
#include "main/record.h"
#include "menu_main.h"
#include "screen_pause.h"
#include "screen_connect.h"
#include "screen_quickreconnect.h"
#include "screen_textentry.h"
#include "screen_prompt.h"
#include "script/luna/luna.h"

#ifdef ENABLE_XTECH_LUA
#include "../script/lunalua/xtech_lua_main.h"
#include "../script/lunalua/eventproxy/xtech_lua_eventproxy.h"
#endif

#include "../pseudo_vb.h"

PauseCode GamePaused = PauseCode::None;

//! Holds the screen overlay for the level
ScreenFader g_levelScreenFader;
RangeArr<ScreenFader, 0, c_vScreenCount> g_levelVScreenFader;

void clearScreenFaders()
{
    g_levelScreenFader.clearFader();
    for(int s = 0; s < 3; ++s)
        g_levelVScreenFader[s].clearFader();
}

void updateScreenFaders()
{
    g_levelScreenFader.update();

    for(int s = 0; s < 3; ++s)
        g_levelVScreenFader[s].update();
}

void levelWaitForFade()
{
    while(!g_levelScreenFader.isComplete() && GameIsActive)
    {
        XEvents::doEvents();

        if(canProceedFrame())
        {
            computeFrameTime1();
            UpdateGraphics();
            UpdateSound();
            XEvents::doEvents();
            computeFrameTime2();
            updateScreenFaders();
        }

        if(!MaxFPS)
            PGE_Delay(1);
    }
}

void editorWaitForFade()
{
    while(!g_levelScreenFader.isComplete() && GameIsActive)
    {
        XEvents::doEvents();

        if(canProceedFrame())
        {
            computeFrameTime1();
            if(GamePaused == PauseCode::Prompt)
                PromptScreen::Render();
            else if(WorldEditor)
                UpdateGraphics2();
            else
                UpdateGraphics();
            UpdateSound();
            XEvents::doEvents();
            computeFrameTime2();
            updateScreenFaders();
        }

        if(!MaxFPS)
            PGE_Delay(1);
    }
}


void CheckActive();//in game_main.cpp

void GameLoop()
{
    g_microStats.start_task(MicroStats::Script);
    lunaLoop();
    
    if((LevelSelect || WorldEditor) && !GameMenu && !GameOutro) // music on the world map
        isOverworld = true;
    else
        isOverworld = false;

#ifdef ENABLE_XTECH_LUA
    if(GamePaused == PauseCode::None)
    {
        std::shared_ptr<Event> onTickEvent = std::make_shared<Event>("onTick", false);
        onTickEvent->setLoopable(false);
        onTickEvent->setDirectEventName("onTick");
        xtech_lua_callLuaEvent(onTickEvent);
    }
    
    std::shared_ptr<Event> onDrawEvent = std::make_shared<Event>("onDraw", false);
    onDrawEvent->setLoopable(false);
    onDrawEvent->setDirectEventName("onDraw");
    xtech_lua_callLuaEvent(onDrawEvent);
#endif

    g_microStats.start_task(MicroStats::Controls);

    if(!Controls::Update())
    {
        QuickReconnectScreen::g_active = true;

        if(!g_config.NoPauseReconnect && g_compatibility.pause_on_disconnect && !TestLevel)
            PauseGame(PauseCode::PauseScreen, 0);
    }

    if(QuickReconnectScreen::g_active)
        QuickReconnectScreen::Logic();

    Integrator::sync();

    g_microStats.start_task(MicroStats::Layers);

    if(LevelMacro > LEVELMACRO_OFF)
        UpdateMacro();

    if(BattleMode)
    {
        if(BattleOutro > 0)
        {
            BattleOutro++;

            if(g_config.EnableInterLevelFade && BattleOutro == 195)
                g_levelScreenFader.setupFader(1, 0, 65, ScreenFader::S_FADE);

            if(BattleOutro == 260)
                EndLevel = true;
        }
    }

    if(ErrorQuit)
    {
        EndLevel = true;
        ErrorQuit = false;
        LevelBeatCode = -1;
        pLogWarning("Quit level because of an error");
        XRender::clearBuffer();
    }

    if(EndLevel)
    {
#ifdef ENABLE_XTECH_LUA
        std::shared_ptr<Event> onExitEvent = std::make_shared<Event>("onExit", false);
        onExitEvent->setLoopable(false);
        onExitEvent->setDirectEventName("onExit");
        xtech_lua_callLuaEvent(onExitEvent);
        isPlayerEndCancelled = false;
#endif
        
        if(LevelBeatCode > 0)
        {
#ifdef ENABLE_XTECH_LUA
            std::shared_ptr<Event> onExitLevelEvent = std::make_shared<Event>("onExitLevel", false);
            onExitLevelEvent->setLoopable(false);
            onExitLevelEvent->setDirectEventName("onExitLevel");
            xtech_lua_callLuaEvent(onExitLevelEvent, LevelBeatCode);
#endif
            if(Checkpoint == FullFileName)
            {
                pLogDebug("Clear check-points at GameLoop()");
                Checkpoint.clear();
                CheckpointsList.clear();
            }
        }

        if(SwapCharAllowed())
        {
            pLogDebug("Save drop/add characters configuration at GameLoop()");
            ConnectScreen::SaveChars();
        }

        speedRun_triggerLeave();
        NextLevel();

        Controls::Update(false);
    }
    else if(qScreen)
    {
        g_microStats.start_task(MicroStats::Effects);
        UpdateEffects();
        if(CompatGetLevel() >= COMPAT_SMBX2)
            speedRun_tick();
        g_microStats.start_task(MicroStats::Graphics);
        UpdateGraphics();
        updateScreenFaders();
    }
    else if(BattleIntro > 0)
    {
        g_microStats.start_task(MicroStats::Graphics);
        UpdateGraphics();
        BlockFrames();
        g_microStats.start_task(MicroStats::Sound);
        UpdateSound();
        g_microStats.start_task(MicroStats::NPCs);
        For(A, 1, numNPCs)
            NPCFrames(A);
        BattleIntro--;
        if(BattleIntro == 1)
            PlaySound(SFX_Checkpoint);
        updateScreenFaders();
    }
    else
    {
        if(MagicHand)
            UpdateEditor();

        ClearTriggeredEvents();
        UpdateLayers(); // layers before/after npcs

        g_microStats.start_task(MicroStats::NPCs);
        UpdateNPCs();

        if(LevelMacro == LEVELMACRO_KEYHOLE_EXIT)
            return; // stop on key exit

        g_microStats.start_task(MicroStats::Blocks);
        UpdateBlocks();
        g_microStats.start_task(MicroStats::Effects);
        UpdateEffects();
        g_microStats.start_task(MicroStats::Player);
        UpdatePlayer();
        speedRun_tick();
        g_microStats.start_task(MicroStats::Graphics);
        if(LivingPlayers() || BattleMode)
            UpdateGraphics();
        g_microStats.start_task(MicroStats::Sound);
        UpdateSound();
        g_microStats.start_task(MicroStats::Events);
        UpdateEvents();
//        If MagicHand = True Then UpdateEditor

        updateScreenFaders();

        // Pause game and CaptainN logic
        if(LevelMacro == LEVELMACRO_OFF && CheckLiving() > 0)
        {
            // this is always able to pause the game even when CaptainN is enabled.
            if(SharedControls.Pause)
                PauseGame(PauseCode::PauseScreen, 0);
            // don't let double-pause or double-toggle happen
            else
            {
                for(int p = 1; p <= numPlayers && p <= maxLocalPlayers; p++)
                {
                    // only consider new start presses
                    if(!Player[p].UnStart || !Player[p].Controls.Start)
                        continue;

                    // use limited, buggy code for non-player 1 in compat case
                    if(p != 1 && !g_compatibility.multiplayer_pause_controls)
                    {
                        if(CaptainN || FreezeNPCs)
                        {
                            Player[p].UnStart = false;
                            FreezeNPCs = !FreezeNPCs;
                            PlaySound(SFX_Pause);
                        }

                        // don't let double-pause or double-toggle happen
                        break;
                    }

                    // the special NPC freeze toggling functionality from CaptainN
                    if((CaptainN || FreezeNPCs) && PSwitchStop == 0)
                    {
                        Player[p].UnStart = false;
                        if(FreezeNPCs)
                        {
                            FreezeNPCs = false;
                            if(PSwitchTime > 0 && !noSound)
                                ResumeMusic();
                        }
                        else
                        {
                            FreezeNPCs = true;
                            if(PSwitchTime > 0 && !noSound)
                                PauseMusic();
                        }
                        PlaySound(SFX_Pause);
                    }
                    // normally pause the game
                    else
                    {
                        PauseGame(PauseCode::PauseScreen, 0);
                    }

                    // don't let double-pause or double-toggle happen
                    break;
                }
            }
        }
    }

    g_microStats.end_frame();
}

void MessageScreen_Init()
{
    SoundPause[SFX_Message] = 0;
    PlaySound(SFX_Message);
    MenuCursorCanMove = false;
    BuildUTF8CharMap(MessageText, MessageTextMap);
}

bool MessageScreen_Logic(int plr)
{
    bool menuDoPress = SharedControls.MenuDo || SharedControls.Pause;
    bool menuBackPress = SharedControls.MenuBack;

    // there was previously code to copy all players' controls from the main player, but this is no longer necessary (and actively harmful in the SingleCoop case)

    if(!g_compatibility.multiplayer_pause_controls && plr == 0)
        plr = 1;

    if(plr == 0)
    {
        for(int i = 1; i <= numPlayers; i++)
        {
            const Controls_t& c = Player[i].Controls;

            menuDoPress |= (c.Start || c.Jump);
            menuBackPress |= c.Run;
        }
    }
    else
    {
        const Controls_t& c = Player[plr].Controls;

        menuDoPress |= (c.Start || c.Jump);
        menuBackPress |= c.Run;
    }

    if(!MenuCursorCanMove)
    {
        if(!menuDoPress && !menuBackPress)
            MenuCursorCanMove = true;

        return false;
    }

    if(MenuCursorCanMove && (menuDoPress || menuBackPress))
    {
        MessageText.clear();
        MessageTextMap.clear();
        return true;
    }

    return false;
}

int PauseGame(PauseCode code, int plr)
{
//    double fpsTime = 0;
//    int fpsCount = 0;

    int prev_cursor = XWindow::showCursor(-1);

    if(!GameMenu && !LevelEditor)
    {
        for(int A = numPlayers; A >= 1; A--)
            SavedChar[Player[A].Character] = Player[A];
    }

    if(code == PauseCode::Message)
    {
#ifdef ENABLE_XTECH_LUA
        bool isCancelled = false;
        
        std::shared_ptr<Event> onMessageBoxEvent = std::make_shared<Event>("onMessageBox", true);
        onMessageBoxEvent->setLoopable(false);
        onMessageBoxEvent->setDirectEventName("onMessageBox");
        xtech_lua_callLuaEvent(onMessageBoxEvent, MessageText);
        
        isCancelled = onMessageBoxEvent->native_cancelled();
        if(isCancelled)
        {
            GamePaused = PauseCode::None;
            return 0;
        }
#endif
        MessageScreen_Init();
    }
    else if(code == PauseCode::PauseScreen)
    {
#ifdef ENABLE_XTECH_LUA
        bool isCancelled = false;
    
        std::shared_ptr<Event> onPauseEvent = std::make_shared<Event>("onPause", true);
        onPauseEvent->setLoopable(false);
        onPauseEvent->setDirectEventName("onPause");
        xtech_lua_callLuaEvent(onPauseEvent, plr);
        
        isCancelled = onPauseEvent->native_cancelled();
        
        if(isCancelled)
        {
            GamePaused = PauseCode::None;
            return 0;
        }
#endif
        PauseScreen::Init(plr, SharedControls.LegacyPause);
    }
    else if(code == PauseCode::DropAdd)
    {
        ConnectScreen::DropAdd_Start();
        XWindow::showCursor(0);
    }
    else if(code == PauseCode::Prompt)
    {
        PromptScreen::Init();
    }
    else if(code == PauseCode::TextEntry)
    {
        // assume TextEntryScreen has already been inited through its Run function.
    }
    else if(code == PauseCode::Misc)
    {
        
    }

    PauseCode old_code = GamePaused;
    GamePaused = code;

    if(PSwitchTime > 0)
    {
        // If noSound = False Then mciSendString "pause smusic", 0, 0, 0
        if(!noSound)
            PauseMusic();
    }

    // resetFrameTimer();

    // some pause games may return a status code
    int result = 0;

    do
    {
        if(canProceedFrame())
        {
#ifdef ENABLE_XTECH_LUA
            std::shared_ptr<Event> onDrawPaintPauseEvent = std::make_shared<Event>("onDrawPaint", false);
            onDrawPaintPauseEvent->setLoopable(false);
            onDrawPaintPauseEvent->setDirectEventName("onDrawPaint");
            xtech_lua_callLuaEvent(onDrawPaintPauseEvent);
            
            std::shared_ptr<Event> onDrawPauseEvent = std::make_shared<Event>("onDraw", false);
            onDrawPauseEvent->setLoopable(false);
            onDrawPauseEvent->setDirectEventName("onDraw");
            xtech_lua_callLuaEvent(onDrawPauseEvent);
#endif
            computeFrameTime1();
            computeFrameTime2();

            g_microStats.start_task(MicroStats::Controls);

            XEvents::doEvents();
            CheckActive();

            g_microStats.start_task(MicroStats::Graphics);

            speedRun_tick();

            if(code == PauseCode::Prompt)
                PromptScreen::Render();
            else if((LevelSelect && !GameMenu) || WorldEditor)
                UpdateGraphics2();
            else
                UpdateGraphics();

            g_microStats.start_task(MicroStats::Controls);

            if(!Controls::Update())
            {
                QuickReconnectScreen::g_active = true;
            }

            if(QuickReconnectScreen::g_active)
                QuickReconnectScreen::Logic();

            g_microStats.start_task(MicroStats::Sound);

            UpdateSound();
            BlockFrames();

            g_microStats.start_task(MicroStats::Effects);

            UpdateEffects();

            if(LevelSelect)
                g_worldScreenFader.update();
            else
                updateScreenFaders();

            // reset the active player if it is no longer present
            if(plr > numPlayers)
                plr = 0;

            g_microStats.start_task(MicroStats::Script);

            // run the appropriate pause logic
            if(qScreen)
            {
                // prevent any logic or unpause from taking place
            }
            else if(GamePaused == PauseCode::PauseScreen)
            {
                if(PauseScreen::Logic(plr))
                    break;
            }
            else if(GamePaused == PauseCode::Message)
            {
                if(MessageScreen_Logic(plr))
                    break;
            }
            else if(GamePaused == PauseCode::Prompt)
            {
                if(PromptScreen::Logic())
                    break;
            }
            else if(GamePaused == PauseCode::DropAdd)
            {
                result = ConnectScreen::Logic();
                if(result)
                    break;
            }
            else if(GamePaused == PauseCode::TextEntry)
            {
                if(TextEntryScreen::Logic())
                    break;
            }
            else if(GamePaused == PauseCode::Misc)
            {
                
            }
            else if(GamePaused == PauseCode::None)
            {
                break;
            }

            g_microStats.end_frame();
        }

        if(!MaxFPS)
            PGE_Delay(1);

        if(!GameIsActive)
            break;
    } while(true);

    GamePaused = old_code;

    // prevent unexpected button presses
    for(int i = 1; i <= numPlayers; i++)
    {
        Player[i].UnStart = false;
        Player[i].CanJump = false;
    }

    MenuCursorCanMove = false;

    XWindow::showCursor(prev_cursor);

    if(PSwitchTime > 0)
    {
        // If noSound = False Then mciSendString "resume smusic", 0, 0, 0
        if(!noSound)
            ResumeMusic();
    }

    // resetFrameTimer();

    return result;
}
