#include "xtech_lua_player.h"

#include "../../src/player.h"
#include "../../src/npc.h"
#include "../../src/globals.h"
#include "../../src/global_dirs.h"
#include "../../src/sound.h"
#include "../../src/main/cheat_code.h"
#include "../../src/game_main.h"
#include "../../src/location.h"



int xtech_lua_player_getIdx(int idx)
{
    return idx;
}

void xtech_lua_player_harm(int idx)
{
    PlayerHurt(idx);
}

void xtech_lua_player_kill(int idx)
{
    PlayerDead(idx);
}

double xtech_lua_player_getX(int idx)
{
    return Player[idx].Location.X;
}

void xtech_lua_player_setX(int idx, double value)
{
    Player[idx].Location.X = value;
}

double xtech_lua_player_getY(int idx)
{
    return Player[idx].Location.Y;
}

void xtech_lua_player_setY(int idx, double value)
{
    Player[idx].Location.Y = value;
}

double xtech_lua_player_getSpeedX(int idx)
{
    return Player[idx].Location.SpeedX;
}

void xtech_lua_player_setSpeedX(int idx, double value)
{
    Player[idx].Location.SpeedX = value;
}

double xtech_lua_player_getSpeedY(int idx)
{
    return Player[idx].Location.SpeedY;
}

void xtech_lua_player_setSpeedY(int idx, double value)
{
    Player[idx].Location.SpeedY = value;
}

double xtech_lua_player_getWidth(int idx)
{
    return Player[idx].Location.Width;
}

double xtech_lua_player_getHeight(int idx)
{
    return Player[idx].Location.Height;
}



//Global memory values

bool xtech_lua_player_getDoubleJump(int idx)
{
    return Player[idx].DoubleJump;
}

void xtech_lua_player_setDoubleJump(int idx, bool value)
{
    Player[idx].DoubleJump = value;
}


bool xtech_lua_player_getFlySparks(int idx)
{
    return Player[idx].FlySparks;
}

void xtech_lua_player_setFlySparks(int idx, bool value)
{
    Player[idx].FlySparks = value;
}


bool xtech_lua_player_getDriving(int idx)
{
    return Player[idx].Driving;
}

void xtech_lua_player_setDriving(int idx, bool value)
{
    Player[idx].Driving = value;
}


int xtech_lua_player_getQuicksand(int idx)
{
    return Player[idx].Quicksand;
}

void xtech_lua_player_setQuicksand(int idx, int value)
{
    Player[idx].Quicksand = value;
}


int xtech_lua_player_getBombs(int idx)
{
    return Player[idx].Bombs;
}

void xtech_lua_player_setBombs(int idx, int value)
{
    Player[idx].Bombs = value;
}


bool xtech_lua_player_getSlippy(int idx)
{
    return Player[idx].Slippy;
}

void xtech_lua_player_setSlippy(int idx, bool value)
{
    Player[idx].Slippy = value;
}


bool xtech_lua_player_getFairy(int idx)
{
    return Player[idx].Fairy;
}

void xtech_lua_player_setFairy(int idx, bool value)
{
    Player[idx].Fairy = value;
}


int xtech_lua_player_getFairyCD(int idx)
{
    return Player[idx].FairyCD;
}

void xtech_lua_player_setFairyCD(int idx, int value)
{
    Player[idx].FairyCD = value;
}


int xtech_lua_player_getFairyTime(int idx)
{
    return Player[idx].FairyTime;
}

void xtech_lua_player_setFairyTime(int idx, int value)
{
    Player[idx].FairyTime = value;
}


bool xtech_lua_player_getHasKey(int idx)
{
    return Player[idx].HasKey;
}

void xtech_lua_player_setHasKey(int idx, bool value)
{
    Player[idx].HasKey = value;
}
