#include "../../core/msgbox.h"

#include "../../player.h"
#include "../../npc.h"
#include "../../globals.h"
#include "../../global_dirs.h"
#include "../../global_constants.h"
#include "../../global_strings.h"
#include "../../sound.h"
#include "../../main/cheat_code.h"
#include "../../game_main.h"
#include "../../main/world_file.h"
#include "../../main/menu_main.h"
#include "../../controls.h"
#include "../../main/screen_connect.h"
#include "../../main/screen_quickreconnect.h"
#include "../../graphics.h"
#include "../../main/level_file.h"
#include "../../main/speedrunner.h"
#include "../../main/game_strings.h"

#include <fmt_time_ne.h>
#include <fmt_format_ne.h>

bool xtech_lua_pausedByLua = false;

void xtech_lua_showMessageBox(std::string message)
{
    XMsgBox::simpleMsgBox(XMsgBox::MESSAGEBOX_INFORMATION, "Debug", message);
}

void xtech_lua_showMessageBoxInGame(std::string message)
{
    MessageText.clear();
    MessageText = message;
    PauseGame(PauseCode::Message, 0);
}

std::string xtech_lua_getCheatBuffer()
{
    std::string cheatThing = cheats_get();
    if(cheatThing != "")
        return cheatThing;
    else
        return "";
}

void xtech_lua_setCheatBuffer(const std::string &cheatBuffer)
{
    cheats_clearBuffer();
    cheats_setBuffer(cheatBuffer);
}

void xtech_lua_openPauseMenu(int plr)
{
    PauseGame(PauseCode::PauseScreen, plr);
}

void xtech_lua_openPauseMenu()
{
    PauseGame(PauseCode::PauseScreen, 0);
}

std::string xtech_lua_episodePath()
{
    if(FileNamePath != "")
        return FileNamePath;
    else
        return "SMBX2";
}

std::string xtech_lua_levelFolderPath()
{
    if(FileNamePath != "")
    {
        return FileNamePath + FileName + "/";
    }
    else
        return "SMBX2";
}

std::string xtech_lua_levelFilename()
{
    if(FileNameFull != "")
        return FileNameFull;
    else
        return "SMBX2";
}

std::string xtech_lua_levelName()
{
    if(FileName != "")
        return FileName;
    else
        return "SMBX2";
}

std::string xtech_lua_worldFilename()
{
    if(FileNameWorld != "")
        return FileNameWorld;
    else
        return "SMBX2";
}

void xtech_lua_pause()
{
    xtech_lua_pausedByLua = true;
    PauseGame(PauseCode::Misc, 0);
}

void xtech_lua_pause(bool finishDrawing)
{
    xtech_lua_pausedByLua = true;
    GamePaused = PauseCode::Misc;
}

void xtech_lua_unpause()
{
    xtech_lua_pausedByLua = false;
    GamePaused = PauseCode::None;
}

void xtech_lua_saveGame()
{
    SaveGame();
}

void xtech_lua_saveGame(bool isCheater)
{
    if(not isCheater)
        Cheater = false;
    SaveGame();
}

void xtech_lua_exitGame()
{
    LevelSelect = false;
    GameMenu = true;
    MenuMode = MENU_MAIN;
    MenuCursor = 0;
}

void xtech_lua_exitEngine()
{
    KillIt();
}

bool xtech_lua_misc_isPaused()
{
    if(GamePaused == PauseCode::None)
        return false;
    else
        return true;
}

bool xtech_lua_misc_isPausedByLua()
{
    if(!xtech_lua_pausedByLua)
        return false;
    else if(xtech_lua_pausedByLua)
        return true;
    
    return false;
}

int xtech_lua_misc_getWindowWidth()
{
    return ScreenW;
}

int xtech_lua_misc_getWindowHeight()
{
    return ScreenH;
}

void xtech_lua_level_exit()
{
    // in non-cheat variant, LevelMacroCounter is stuck at 0 if player never leaves section
    bool possible_softlock = (LevelMacro == LEVELMACRO_CARD_ROULETTE_EXIT && LevelMacroCounter == 0);

    if(LevelMacro != LEVELMACRO_OFF && !possible_softlock)
        return;

    LevelBeatCode = 0;
    LevelMacro = LEVELMACRO_OFF;
    LevelMacroCounter = 0;
    EndLevel = true;
}

void xtech_lua_level_exit(int winType)
{
    // in non-cheat variant, LevelMacroCounter is stuck at 0 if player never leaves section
    bool possible_softlock = (LevelMacro == LEVELMACRO_CARD_ROULETTE_EXIT && LevelMacroCounter == 0);

    if(LevelMacro != LEVELMACRO_OFF && !possible_softlock)
        return;

    LevelBeatCode = winType;
    LevelMacro = LEVELMACRO_OFF;
    LevelMacroCounter = 0;
    EndLevel = true;
}

bool xtech_lua_misc_inEditor()
{
    if((LevelEditor || WorldEditor) || TestLevel)
        return true;
    
    return false;
}

std::string xtech_lua_episodeName()
{
    if(WorldName != "")
        return WorldName;
    else
        return "SMBX2";
}

void xtech_lua_level_load(std::string levelFile, int startWarp)
{
    std::string levelFile2 = FileNamePath + levelFile;
    if(!levelFile2.empty() && levelFile2 != ".lvl" && levelFile2 != ".lvlx")
    {
        addMissingLvlSuffix(levelFile2);
        std::string levelPath = g_dirEpisode.resolveFileCaseExistsAbs(levelFile2);

        if(!levelPath.empty())
        {
            // save which characters were present at level start
            if(SwapCharAllowed())
            {
                ConnectScreen::SaveChars();
            }

            StartWarp = startWarp;
            StopMusic();
            PlaySound(SFX_LevelSelect);
            
            if(LevelSelect)
                LevelSelect = false;

            delayedMusicReset(); // Reset delayed music to prevent unexpected behaviour at loaded level

            ClearLevel();

            if(!OpenLevel(levelPath))
            {
                delayedMusicStart(); // Allow music being started
                MessageText = fmt::format_ne(g_gameStrings.errorOpenFileFailed, levelFile2);
                PauseGame(PauseCode::Message);
                ErrorQuit = true;
            }

            GameThing(1000, 3);
        }
    }
}

void xtech_lua_level_load(std::string levelFile)
{
    xtech_lua_level_load(levelFile, 0);
}