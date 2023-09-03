#include "xtech_lua_main.h"
#include "xtech_lua_sounds.h"
#include "../../src/player.h"
#include "../../src/npc.h"
#include "../../src/globals.h"
#include "../../src/global_dirs.h"
#include "../../src/sound.h"

#include <utility>
#include <string>
#include <regex>
#include <iostream>
#include <fstream>

#include "../../src/core/msgbox.h"

lua_State* L;

using namespace luabind;

std::string AppPath2 = xtech_lua_replaceStringValue(AppPath, "/", "\\");

std::string xtech_lua_getLuaLibsPath()
{
    std::string lapi = AppPath2 + "scripts\\base\\engine\\main.lua";
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

std::string xtech_lua_replaceStringValue(std::string in, std::string from, std::string to)
{
    return std::regex_replace( in, std::regex(from), to );
}

bool xtech_lua_init(std::string codePath, std::string levelPath)
{
    L = luaL_newstate();
    luaL_openlibs(L);
    luabind::open(L);
    
    std::string scriptToLoad = xtech_lua_getLuaLibsPath();
    std::string dataFromFile;
    
    DirListCI LuaDir;
    
    LuaDir.setCurDir(AppPath2 + "scripts\\base\\engine\\");
    dataFromFile = LuaDir.resolveFileCaseExistsAbs("main.lua");
    if(dataFromFile.empty())
    {
        XMsgBox::simpleMsgBox(XMsgBox::MESSAGEBOX_ERROR, "Error", "\"scripts\\base\\engine\\main.lua\" is required.\nBe sure you installed everything correctly!");
        xtech_lua_quit();
        return false;
    }
    
    // Opens 'foo.txt'.
    std::ifstream is(scriptToLoad);
    std::string finalLuaCode;

    // Sets position to the end of the file.
    is.seekg(0, std::ios::end);

    // Reserves memory for the file.
    finalLuaCode.reserve(is.tellg());

    // Sets position to the start of the file.
    is.seekg(0, std::ios::beg);

    // Sets contents of 'finalLuaCode' to all characters in the file.
    finalLuaCode.assign(std::istreambuf_iterator<char>(is),
      std::istreambuf_iterator<char>());
    
    xtech_lua_bindAll();
    
    bool errLapi = false;
    int lapierrcode = luaL_loadbuffer(L, finalLuaCode.c_str(), finalLuaCode.length(), "=main.lua") || lua_pcall(L, 0, LUA_MULTRET, 0);
    if(!(lapierrcode == 0)){
        object error_msg(from_stack(L, -1));
        XMsgBox::simpleMsgBox(XMsgBox::MESSAGEBOX_ERROR, "Error", object_cast<const char*>(error_msg));
        errLapi = true;
    }
    
    if(errLapi)
    {
        xtech_lua_quit();
        return false;
    }
    
    const char* initName = "__onInit";
    if (xtech_lua_is_function(L, initName))
    {
        callLuaFunction(L, initName, codePath, levelPath);
    }
    
    return true;
}

void xtech_lua_bindAll()
{
    module(L)
        [
            namespace_("Native")[
                def("getSMBXPath", (std::string(*)())&AppPath2)
            ],
            namespace_("Text")[
                def("windowDebug", (void(*)(std::string))&xtech_lua_showMessageBox),
                def("windowDebugSimple", (void(*)(std::string))&xtech_lua_showMessageBox)
            ],
            namespace_("Audio")[
                //Music
                def("MusicChange", (void(*)(int, int, int))&xtech_lua_MusicChange),
                def("MusicChange", (void(*)(int, std::string, int))&xtech_lua_MusicChange),
                def("MusicChange", (void(*)(int, int))&xtech_lua_MusicChange),
                def("MusicChange", (void(*)(int, std::string))&xtech_lua_MusicChange),
                //SFX
                def("SfxPlay", (void(*)(int, int, int))&PlaySound),
                def("SfxPlay", (void(*)(std::string, int, int))&PlayExtSoundNoMenu),
                def("SfxPlayMenu", (void(*)(int, int))&PlaySoundMenu),
                def("SfxPlayMenu", (void(*)(std::string, int, int))&PlayExtSound)
            ]
        ];
}

bool xtech_lua_quit()
{
    lua_close(L);
    L = NULL;
    return true;
}
