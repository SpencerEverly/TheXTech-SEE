#ifndef LUA_SOUNDS_HHH
#define LUA_SOUNDS_HHH

void xtech_lua_MusicChange(int sectionID, int musicID, int fadeIn);
void xtech_lua_MusicChange(int sectionID, std::string musicFile, int fadeIn);
void xtech_lua_MusicChange(int sectionID, int musicID);
void xtech_lua_MusicChange(int sectionID, std::string musicFile);
void xtech_lua_playSFX(const std::string &soundFile, int loops, int volume);
void xtech_lua_playSFX(const std::string &soundFile, int loops);
void xtech_lua_playSFX(const std::string &soundFile);

#endif // LUA_SOUNDS_HHH
