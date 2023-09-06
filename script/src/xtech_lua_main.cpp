#include "xtech_lua_main.h"
#include "xtech_lua_sounds.h"
#include "xtech_lua_graphics.h"
#include "../../src/player.h"
#include "../../src/npc.h"
#include "../../src/globals.h"
#include "../../src/global_dirs.h"
#include "../../src/sound.h"
#include "../../src/main/cheat_code.h"
#include "../../src/game_main.h"

#include <utility>
#include <string>
#include <regex>
#include <iostream>
#include <fstream>

#include "../../src/core/msgbox.h"

lua_State* L;

using namespace luabind;

bool isLuaActive = false;

std::string xtech_lua_getLuaLibsPath()
{
    std::string lapi = AppPath + "scripts/base/engine/main.lua";
    return lapi;
}

bool xtech_lua_is_function(lua_State *luaState, const char *fname)
{
    luabind::object g = luabind::globals(luaState);
    luabind::object fun = g[fname];
    if (fun.is_valid()) {
        if (luabind::type(fun)==LUA_TFUNCTION) {
            return true;
        }
    }
    return false;
}

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

std::string xtech_lua_replaceStringValue(std::string in, std::string from, std::string to)
{
    return std::regex_replace( in, std::regex(from), to );
}

bool xtech_lua_readFile(std::string content, std::string path, std::string errMsg)
{
    // Opens the script.
    std::ifstream theFile(path, std::ios::binary| std::ios::in);
    if(!theFile.is_open()){
        theFile.close();
        if(!errMsg.empty())
            XMsgBox::simpleMsgBox(XMsgBox::MESSAGEBOX_ERROR, "Error", errMsg);
        return false;
    }
    
    content = std::string((std::istreambuf_iterator<char>(theFile)), std::istreambuf_iterator<char>());
    return true;
}

std::string xtech_lua_readScriptFile()
{
    // Opens the script.
    std::ifstream theFile(xtech_lua_getLuaLibsPath(), std::ios::binary| std::ios::in);
    std::string content = std::string((std::istreambuf_iterator<char>(theFile)), std::istreambuf_iterator<char>());
    return content;
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

bool xtech_lua_init()
{
    //Just to be safe
    xtech_lua_quit();
    
    //Open up a new Lua State
    L = luaL_newstate();
    
    //Open all luabind functions
    luabind::open(L);
    
    //Open up "safe" standard lua libraries
    lua_pushcfunction(L, luaopen_base);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_math);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_string);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_table);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_debug);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_os);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_package);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_io);
    lua_call(L,0,0);
    
    //Remove unsafe apis
    {
        object _G = globals(L);
        object osTable = _G["os"];
        osTable["execute"] = object();
        osTable["exit"] = object();
        //osTable["getenv"] = object();
        //osTable["remove"] = object();
        //osTable["rename"] = object();
        osTable["setlocal"] = object();
        osTable["tmpname"] = object();
    }
    
    std::string dataFromFile;
    
    if(!xtech_lua_readFile(dataFromFile, xtech_lua_getLuaLibsPath(), "\"scripts/base/engine/main.lua\" is required.\nBe sure you installed everything correctly!"))
    {
        xtech_lua_quit();
        return false;
    }
    
    dataFromFile = xtech_lua_readScriptFile();
    xtech_lua_bindAll();
    
    bool errLapi = false;
    int lapierrcode = luaL_loadbuffer(L, dataFromFile.c_str(), dataFromFile.length(), "=main.lua") || lua_pcall(L, 0, LUA_MULTRET, 0);
    if(!(lapierrcode == 0)){
        object error_msg(from_stack(L, -1));
        XMsgBox::simpleMsgBox(XMsgBox::MESSAGEBOX_ERROR, "Error", object_cast<const char*>(error_msg));
        errLapi = true;
    }
    
    if(errLapi)
    {
        return false;
    }
    
    const char* initName = "__onInit";
    if (xtech_lua_is_function(L, initName))
    {
        xtech_lua_callLuaFunction(L, initName);
    }
    
    isLuaActive = true;
    return true;
}

void xtech_lua_bindAll()
{
    module(L)
        [
            namespace_("Native")[
                def("getSMBXPath", (std::string(*)())&AppPath)
            ],
            namespace_("Text")[
                def("print", (void(*)(const char*, float, float))&xtech_lua_textPrint),
                def("printWP", (void(*)(const char*, float, float, float))&xtech_lua_textPrintWP),
                def("windowDebug", (void(*)(std::string))&xtech_lua_showMessageBox),
                def("windowDebugSimple", (void(*)(std::string))&xtech_lua_showMessageBox),
                def("showMessageBox", (std::string(*)())&xtech_lua_showMessageBoxInGame)
            ],
            namespace_("Audio")[
                //Music
                def("MusicChange", (void(*)(int, int, int))&xtech_lua_MusicChange),
                def("MusicChange", (void(*)(int, int))&xtech_lua_MusicChange),
                def("MusicChange", (void(*)(int, std::string, int))&xtech_lua_MusicChange),
                def("MusicChange", (void(*)(int, std::string))&xtech_lua_MusicChange),
                
                def("MusicGetCustomMusic", (std::string(*)(int))&xtech_lua_getCustomMusic),
                def("MusicGetMusicID", (double(*)(int))&xtech_lua_getMusicID),
                
                //SFX
                def("SfxPlay", (void(*)(int, int, int))&xtech_lua_playSFX),
                def("SfxPlay", (void(*)(int, int))&xtech_lua_playSFX),
                def("SfxPlay", (void(*)(int))&xtech_lua_playSFX),
                def("SfxPlay", (void(*)(const std::string&, int, int))&xtech_lua_playSFX),
                def("SfxPlay", (void(*)(const std::string&, int))&xtech_lua_playSFX),
                def("SfxPlay", (void(*)(const std::string&))&xtech_lua_playSFX)
            ],
            namespace_("Misc")[
                def("cheatBuffer", (std::string(*)())&cheats_get),
                def("cheatBuffer", (void(*)(const std::string&))&xtech_lua_setCheatBuffer),
                
                def("openPauseMenu", (void(*)(int))&xtech_lua_openPauseMenu),
                def("openPauseMenu", (void(*)())&xtech_lua_openPauseMenu),
                def("saveGame", (void(*)())&SaveGame),
                
                def("episodePath", (std::string(*)())&xtech_lua_episodePath)
            ],
            namespace_("Level")[
                def("folderPath", (std::string(*)())&xtech_lua_levelFolderPath),
                def("filename", (std::string(*)())&xtech_lua_levelFilename),
                def("name", (std::string(*)())&xtech_lua_levelName)
            ],
            namespace_("World")[
                def("filename", (std::string(*)())&xtech_lua_worldFilename)
            ]
        ];
}

bool xtech_lua_quit()
{
    if(isLuaActive)
    {
        lua_close(L);
        L = NULL;
        return true;
    }
    else
    {
        return false;
    }
}
