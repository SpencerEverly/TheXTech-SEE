#ifndef LUA_MISC_HHH
#define LUA_MISC_HHH

extern void xtech_lua_showMessageBox(std::string message);

extern void xtech_lua_showMessageBoxInGame(std::string message);

extern std::string xtech_lua_getCheatBuffer();
extern void xtech_lua_setCheatBuffer(const std::string &cheatBuffer);

extern void xtech_lua_openPauseMenu(int plr);
extern void xtech_lua_openPauseMenu();

extern std::string xtech_lua_episodePath();

extern std::string xtech_lua_levelFolderPath();
extern std::string xtech_lua_levelFilename();
extern std::string xtech_lua_levelName();

extern std::string xtech_lua_worldFilename();

#endif // LUA_MISC_HHH
