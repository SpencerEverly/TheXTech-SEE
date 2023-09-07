#ifndef LUA_MISC_HHH
#define LUA_MISC_HHH

extern bool xtech_lua_pausedByLua;

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

extern void xtech_lua_pause();
extern void xtech_lua_pause(bool finishDrawing);
extern void xtech_lua_unpause();

extern void xtech_lua_saveGame();
extern void xtech_lua_saveGame(bool isCheater);
extern void xtech_lua_exitGame();
extern void xtech_lua_exitEngine();

extern bool xtech_lua_loadEpisode(std::string episodeName);
extern bool xtech_lua_misc_isPaused();
extern bool xtech_lua_misc_isPausedByLua();

#endif // LUA_MISC_HHH
