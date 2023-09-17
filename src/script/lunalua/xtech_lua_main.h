#ifndef LUA_MAIN_HHH
#define LUA_MAIN_HHH

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/detail/call_function.hpp>

#include "eventproxy/xtech_lua_eventproxy.h"

#include <utility>
#include <string>

#include <DirManager/dirman.h>
#include <Utils/files.h>
#include <Utils/dir_list_ci.h>
#include <AppPath/app_path.h>
#include <Logger/logger.h>
#include <fmt_format_ne.h>

#define LUAHELPER_DEF_CONST(luabindObj, defName) luabindObj [ #defName ] = defName

extern lua_State* L;
extern bool isLuaActive;

extern bool xtech_lua_is_function(lua_State *luaState, const char *fname);

extern std::string xtech_lua_replaceStringValue(std::string in, std::string from, std::string to);
bool xtech_lua_readFile(std::string content, std::string path, std::string errMsg);
extern std::string xtech_lua_readScriptFile();

extern bool xtech_lua_init();
extern void xtech_lua_bindAll();
extern bool xtech_lua_quit();

template<typename... Args>
extern void xtech_lua_callSimpleLuaEvent(std::string e, Args... args)
{
    if(isLuaActive)
    {
        xtech_lua_callLuaFunction(L, "__callEvent", e, args...);
    }
}

template<typename... Args>
extern void xtech_lua_callLuaEvent(const std::shared_ptr<Event>& e, Args... args)
{
    if(isLuaActive)
    {
        xtech_lua_callLuaFunction(L, "__callEvent", e, args...);
    }
}

template<typename... Args>
extern bool xtech_lua_callLuaFunction(Args... args){
    bool err = false;
    try
    {
        luabind::call_function<void>(args...);
    }
    catch (luabind::error& /*e*/)
    {
        err = true;
    }
    err = err || luabind::object_cast<bool>(luabind::globals(L)["__isLuaError"]);

    // If there was an error, shut down Lua
    if (err)
    {
        xtech_lua_quit();
    }

    return err;
}

#endif // LUA_MAIN_HHH
