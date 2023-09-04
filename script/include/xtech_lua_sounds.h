#ifndef LUA_SOUNDS_HHH
#define LUA_SOUNDS_HHH

void xtech_lua_MusicChange(int sectionID, int musicID, int fadeIn);
void xtech_lua_MusicChange(int sectionID, std::string musicFile, int fadeIn);
void xtech_lua_MusicChange(int sectionID, int musicID);
void xtech_lua_MusicChange(int sectionID, std::string musicFile);

#endif // LUA_SOUNDS_HHH
