#ifndef LUA_PLAYER_HHH
#define LUA_PLAYER_HHH

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/detail/call_function.hpp>

int xtech_lua_player_getIdx(int idx);
void xtech_lua_player_harm(int idx);
void xtech_lua_player_kill(int idx);

double xtech_lua_player_getX(int idx);
void xtech_lua_player_setX(int idx, double value);
double xtech_lua_player_getY(int idx);
void xtech_lua_player_setY(int idx, double value);
double xtech_lua_player_getSpeedX(int idx);
void xtech_lua_player_setSpeedX(int idx, double value);
double xtech_lua_player_getSpeedY(int idx);
void xtech_lua_player_setSpeedY(int idx, double value);
double xtech_lua_player_getWidth(int idx);
double xtech_lua_player_getHeight(int idx);

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

bool xtech_lua_player_getSlippy(int idx);
void xtech_lua_player_setSlippy(int idx, bool value);

bool xtech_lua_player_getFairy(int idx);
void xtech_lua_player_setFairy(int idx, bool value);

int xtech_lua_player_getFairyCD(int idx);
void xtech_lua_player_setFairyCD(int idx, int value);

int xtech_lua_player_getFairyTime(int idx);
void xtech_lua_player_setFairyTime(int idx, int value);

bool xtech_lua_player_getHasKey(int idx);
void xtech_lua_player_setHasKey(int idx, bool value);

#endif
