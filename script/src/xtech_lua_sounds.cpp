#include "../../src/sound.h"
#include "../../src/globals.h"
#include "../../src/global_dirs.h"

void xtech_lua_MusicChange(int sectionID, int musicID, int fadeIn)
{
    if((LevelSelect || WorldEditor) && !GameMenu && !GameOutro) // music on the world map
    {
        
    }
    else
    {
        bgMusic[sectionID] = musicID;
        StartMusic(sectionID, fadeIn);
    }
}

void xtech_lua_MusicChange(int sectionID, std::string musicFile, int fadeIn)
{
    if(musicFile.empty())
    {
        bgMusic[sectionID] = 0;
        StartMusic(sectionID, fadeIn);
        return;
    }
    if((LevelSelect || WorldEditor) && !GameMenu && !GameOutro) // music on the world map
    {
        
    }
    else
    {
        StopMusic();
        CustomMusic[sectionID] = musicFile;
        bgMusic[sectionID] = 24;
        StartMusic(sectionID, fadeIn);
    }
}

void xtech_lua_MusicChange(int sectionID, int musicID)
{
    xtech_lua_MusicChange(sectionID, musicID, 0);
}

void xtech_lua_MusicChange(int sectionID, std::string musicFile)
{
    xtech_lua_MusicChange(sectionID, musicFile, 0);
}

void xtech_lua_playSFX(const std::string &soundFile, int loops, int volume)
{
    PlayExtSound(soundFile, loops, volume);
}

void xtech_lua_playSFX(const std::string &soundFile, int loops)
{
    PlayExtSound(soundFile, loops);
}

void xtech_lua_playSFX(const std::string &soundFile)
{
    PlayExtSound(soundFile);
}