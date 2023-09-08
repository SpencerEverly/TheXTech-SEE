#ifndef LUA_PLAYER_HHH
#define LUA_PLAYER_HHH

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/detail/call_function.hpp>

void xtech_lua_player_harm(int idx);
void xtech_lua_player_kill(int idx);



bool xtech_lua_player_getDoubleJump(int idx);
void xtech_lua_player_setDoubleJump(int idx, bool value);

bool xtech_lua_player_getFlySparks(int idx);
void xtech_lua_player_setFlySparks(int idx, bool value);

bool xtech_lua_player_getDriving(int idx);
void xtech_lua_player_setDriving(int idx, bool value);

int xtech_lua_player_getQuicksand(int idx);
void xtech_lua_player_setQuicksand(int idx, int value);

int xtech_lua_player_getBombs(int idx);
void xtech_lua_player_setBombs(int idx, int value);

#endif
