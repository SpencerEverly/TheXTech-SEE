#ifndef LUA_SOUNDS_HHH
#define LUA_SOUNDS_HHH

void xtech_lua_MusicChange(int sectionID, int musicID, int fadeIn);
void xtech_lua_MusicChange(int sectionID, std::string musicFile, int fadeIn);
void xtech_lua_MusicChange(int sectionID, int musicID);
void xtech_lua_MusicChange(int sectionID, std::string musicFile);
void xtech_lua_playSFX(const std::string &soundFile, int loops, int volume);
void xtech_lua_playSFX(const std::string &soundFile, int loops);
void xtech_lua_playSFX(const std::string &soundFile);
void xtech_lua_playSFX(int soundID, int loops, int volume);
void xtech_lua_playSFX(int soundID, int loops);
void xtech_lua_playSFX(int soundID);
std::string xtech_lua_getCustomMusic(int sectionID);
double xtech_lua_getMusicID(int sectionID);
int xtech_lua_audio_musicVolume();
void xtech_lua_audio_musicVolume(int vlm);
void xtech_lua_audio_musicStop();
void xtech_lua_audio_stopMusicFadeOut(int ms);

#endif // LUA_SOUNDS_HHH
