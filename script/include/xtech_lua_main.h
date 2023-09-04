#ifndef LUA_MAIN_HHH
#define LUA_MAIN_HHH

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/detail/call_function.hpp>

#include <utility>

#include <DirManager/dirman.h>
#include <Utils/files.h>
#include <Utils/dir_list_ci.h>
#include <AppPath/app_path.h>
#include <Logger/logger.h>
#include <fmt_format_ne.h>

#define LUAHELPER_DEF_CONST(luabindObj, defName) luabindObj [ #defName ] = defName

extern lua_State* L;
extern std::string AppPath2;

extern bool xtech_lua_is_function(lua_State *luaState, const char *fname);
extern void xtech_lua_showMessageBox(std::string message);
extern std::string xtech_lua_replaceStringValue(std::string in, std::string from, std::string to);
bool xtech_lua_readFile(std::string content, std::string path, std::string errMsg);

extern bool xtech_lua_init(std::string codePath, std::string levelPath);
extern void xtech_lua_bindAll();
extern bool xtech_lua_quit();

template<typename... Args>
bool callLuaFunction(Args... args){
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
