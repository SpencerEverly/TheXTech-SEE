#include "xtech_lua_main.h"
#include "xtech_lua_sounds.h"
#include "xtech_lua_graphics.h"
#include "xtech_lua_misc.h"
#include "xtech_lua_mem.h"
#include "xtech_lua_player.h"

#include "eventproxy/xtech_lua_eventproxy.h"

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
#include <filesystem>

#include "../../src/core/msgbox.h"

lua_State* L;

using namespace luabind;

bool isLuaActive = false;

std::string xtech_lua_getLuaLibsPath()
{
    std::string lapi = AppPath + "lua/scripts/base/engine/main.lua";
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
                def("getSMBXPath", (std::string(*)())&AppPath),
                def("isOverworld", (bool(*)())&isOverworld)
            ],
            namespace_("Text")[
                def("print", (void(*)(const std::string&, float, float))&xtech_lua_textPrint),
                def("printWP", (void(*)(const std::string&, float, float, float))&xtech_lua_textPrintWP),
                def("windowDebug", (void(*)(std::string))&xtech_lua_showMessageBox),
                def("windowDebugSimple", (void(*)(std::string))&xtech_lua_showMessageBox),
                def("showMessageBox", (void(*)(std::string))&xtech_lua_showMessageBoxInGame)
            ],
            namespace_("Graphics")[
                def("loadImage", (void(*)(std::string, std::string, int))&xtech_lua_graphics_loadImage),
                def("drawImage", (void(*)(int, double, double))&xtech_lua_graphics_drawImage),
                def("drawImage", (void(*)(int, double, double, double, double))&xtech_lua_graphics_drawImage),
                def("drawImage", (void(*)(int, double, double, double, double, int, int))&xtech_lua_graphics_drawImage),
                def("drawImage", (void(*)(int, double, double, double, double, double))&xtech_lua_graphics_drawImage),
                def("drawImage", (void(*)(int, double, double, double, double, int, int, double))&xtech_lua_graphics_drawImage),
                def("drawRectangle", (void(*)(int, int, int, int, float, float, float, float, bool))&xtech_lua_graphics_drawRectangle),
                def("drawRectangle", (void(*)(int, int, int, int, float, float, float, bool))&xtech_lua_graphics_drawRectangle),
                def("drawScreen", (void(*)(float, float, float, float, bool))&xtech_lua_graphics_drawScreen),
                def("drawScreen", (void(*)(float, float, float, bool))&xtech_lua_graphics_drawScreen)
            ],
            namespace_("Audio")[
                //Music
                def("MusicChange", (void(*)(int, int, int))&xtech_lua_MusicChange),
                def("MusicChange", (void(*)(int, int))&xtech_lua_MusicChange),
                def("MusicChange", (void(*)(int, std::string, int))&xtech_lua_MusicChange),
                def("MusicChange", (void(*)(int, std::string))&xtech_lua_MusicChange),
                def("MusicStop", (void(*)())&xtech_lua_audio_musicStop),
                def("MusicVolume", (int(*)())&xtech_lua_audio_musicVolume),
                def("MusicVolume", (void(*)(int))&xtech_lua_audio_musicVolume),
                def("MusicFadeOut", (void(*)(int))&xtech_lua_audio_stopMusicFadeOut),
                
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
            
            class_<Event>("Event")
            .property("eventName", &Event::eventName)
            .property("cancellable", &Event::isCancellable)
            .property("cancelled", &Event::cancelled, &Event::setCancelled)
            .property("loopable", &Event::getLoopable, &Event::setLoopable)
            .property("directEventName", &Event::getDirectEventName, &Event::setDirectEventName),
            
            namespace_("Player")[
                def("harm", (void(*)(int))&xtech_lua_player_harm),
                def("kill", (void(*)(int))&xtech_lua_player_kill),
                def("idx", (int(*)(int))&xtech_lua_player_getIdx),
                def("count", (int(*)())&xtech_lua_player_getCount),
                
                def("x", (double(*)(int))&xtech_lua_player_getX),
                def("x", (void(*)(int, double))&xtech_lua_player_setX),
                def("y", (double(*)(int))&xtech_lua_player_getY),
                def("y", (void(*)(int, double))&xtech_lua_player_setY),
                def("speedX", (double(*)(int))&xtech_lua_player_getSpeedX),
                def("speedX", (void(*)(int, double))&xtech_lua_player_setSpeedX),
                def("speedY", (double(*)(int))&xtech_lua_player_getSpeedY),
                def("speedY", (void(*)(int, double))&xtech_lua_player_setSpeedY),
                def("width", (double(*)(int))&xtech_lua_player_getWidth),
                def("width", (void(*)(int, double))&xtech_lua_player_setWidth),
                def("height", (double(*)(int))&xtech_lua_player_getHeight),
                def("height", (void(*)(int, double))&xtech_lua_player_setHeight),
                
                def("doubleJump", (bool(*)(int))&xtech_lua_player_getDoubleJump),
                def("doubleJump", (void(*)(int, bool))&xtech_lua_player_setDoubleJump),
                
                def("flySparks", (bool(*)(int))&xtech_lua_player_getFlySparks),
                def("flySparks", (void(*)(int, bool))&xtech_lua_player_setFlySparks),
                
                def("driving", (bool(*)(int))&xtech_lua_player_getDriving),
                def("driving", (void(*)(int, bool))&xtech_lua_player_setDriving),
                
                def("quicksand", (int(*)(int))&xtech_lua_player_getQuicksand),
                def("quicksand", (void(*)(int, int))&xtech_lua_player_setQuicksand),
                
                def("bombs", (int(*)(int))&xtech_lua_player_getBombs),
                def("bombs", (void(*)(int, int))&xtech_lua_player_setBombs),
                
                def("slippy", (bool(*)(int))&xtech_lua_player_getSlippy),
                def("slippy", (void(*)(int, bool))&xtech_lua_player_setSlippy),
                
                def("fairy", (bool(*)(int))&xtech_lua_player_getFairy),
                def("fairy", (void(*)(int, bool))&xtech_lua_player_setFairy),
                
                def("fairyCD", (int(*)(int))&xtech_lua_player_getFairyCD),
                def("fairyCD", (void(*)(int, int))&xtech_lua_player_setFairyCD),
                
                def("fairyTime", (int(*)(int))&xtech_lua_player_getFairyTime),
                def("fairyTime", (void(*)(int, int))&xtech_lua_player_setFairyTime),
                
                def("hasKey", (bool(*)(int))&xtech_lua_player_getHasKey),
                def("hasKey", (void(*)(int, bool))&xtech_lua_player_setHasKey),
                
                def("swordPoke", (int(*)(int))&xtech_lua_player_getSwordPoke),
                def("swordPoke", (void(*)(int, int))&xtech_lua_player_setSwordPoke),
                
                def("hearts", (int(*)(int))&xtech_lua_player_getHearts),
                def("hearts", (void(*)(int, int))&xtech_lua_player_setHearts),
                
                def("canFloat", (bool(*)(int))&xtech_lua_player_getCanFloat),
                def("canFloat", (void(*)(int, bool))&xtech_lua_player_setCanFloat),
                
                def("floatRelease", (bool(*)(int))&xtech_lua_player_getFloatRelease),
                def("floatRelease", (void(*)(int, bool))&xtech_lua_player_setFloatRelease),
                
                def("floatTime", (int(*)(int))&xtech_lua_player_getFloatTime),
                def("floatTime", (void(*)(int, int))&xtech_lua_player_setFloatTime)
            ],
            
            namespace_("Misc")[
                def("cheatBuffer", (std::string(*)())&xtech_lua_getCheatBuffer),
                def("cheatBuffer", (void(*)(const std::string&))&xtech_lua_setCheatBuffer),
                
                def("openPauseMenu", (void(*)(int))&xtech_lua_openPauseMenu),
                def("openPauseMenu", (void(*)())&xtech_lua_openPauseMenu),
                def("saveGame", (void(*)())&xtech_lua_saveGame),
                def("saveGame", (void(*)(bool))&xtech_lua_saveGame),
                def("exitGame", (void(*)())&xtech_lua_exitGame),
                def("exitEngine", (void(*)())&xtech_lua_exitEngine),
                
                def("pause", (void(*)())&xtech_lua_pause),
                def("pause", (void(*)(bool))&xtech_lua_pause),
                def("unpause", (void(*)())&xtech_lua_unpause),
                def("isPaused", (bool(*)())&xtech_lua_misc_isPaused),
                def("isPausedByLua", (bool(*)())&xtech_lua_misc_isPausedByLua),
                
                def("episodePath", (std::string(*)())&xtech_lua_episodePath),
                
                def("getScreenWidth", (int(*)())&xtech_lua_misc_getWindowWidth),
                def("getScreenHeight", (int(*)())&xtech_lua_misc_getWindowHeight)
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
