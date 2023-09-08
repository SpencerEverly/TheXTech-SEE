#include "xtech_lua_player.h"

#include "../../src/player.h"
#include "../../src/npc.h"
#include "../../src/globals.h"
#include "../../src/global_dirs.h"
#include "../../src/sound.h"
#include "../../src/main/cheat_code.h"
#include "../../src/game_main.h"



void xtech_lua_player_harm(int idx)
{
    PlayerHurt(idx);
}

void xtech_lua_player_kill(int idx)
{
    PlayerDead(idx);
}



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
