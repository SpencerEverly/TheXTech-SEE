#include "../../src/core/msgbox.h"

#include "../../src/player.h"
#include "../../src/npc.h"
#include "../../src/globals.h"
#include "../../src/global_dirs.h"
#include "../../src/sound.h"
#include "../../src/main/cheat_code.h"
#include "../../src/game_main.h"

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