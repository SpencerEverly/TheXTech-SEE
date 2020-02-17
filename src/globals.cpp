/*
 * A2xTech - A platform game engine ported from old source code for VB6
 *
 * Copyright (c) 2009-2011 Andrew Spinks, original VB6 code
 * Copyright (c) 2020-2020 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "globals.h"

FrmMain frmMain;
GFX_t GFX;

bool GameIsActive = false;
std::string AppPath;

int numSavedEvents = 0;
RangeArr<std::string, 1, MaxSavedEvents> SavedEvents;
RangeArr<bool, 1, 4> BlockSwitch;
RangeArr<bool, 2, 7> PowerUpUnlock;
long myBackBuffer = 0;
long myBufferBMP = 0;
int AllCharBlock = 0;
bool StartMenu = false;
int BlockFlash = 0;
bool ScrollRelease = false;
bool TakeScreen = false;
std::string LB;
std::string EoT;
RangeArr<ConKeyboard_t, 1, 2> conKeyboard;
RangeArr<ConJoystick_t, 1, 2> conJoystick;
RangeArr<int, 1, 2> useJoystick;

int ScreenShake = 0;
std::string Checkpoint;
bool MagicHand = false;
RangeArr<Player_t, 1, 2> testPlayer;
bool ClearBuffer = false;
int numLocked = 0;
bool resChanged = false;
int inputKey = 0;
bool getNewKeyboard = false;
bool getNewJoystick = false;
int lastJoyButton = 0;
bool GamePaused = false;
std::string MessageText;
int NumSelectWorld  = 0;
RangeArr<SelectWorld_t, 1, maxSelectWorlds> SelectWorld;
bool ShowFPS = false;
double PrintFPS = 0.0;
RangeArr<vScreen_t, 0, 2> vScreen;
int ScreenType = 0;
int DScreenType = 0;
bool LevelEditor = false;
bool WorldEditor = false;
RangeArr<Location_t, 1, 2> PlayerStart;
RangeArr<bool, 0, 20> blockCharacter;
RangeArr<int, 0, maxPlayers> OwedMount;
RangeArr<int, 0, maxPlayers> OwedMountType;
RangeArr<float, 0, maxSections> AutoX;
RangeArr<float, 0, maxSections> AutoY;
int numStars = 0;
RangeArr<Water_t, 0, maxWater> Water;
int numWater = 0;
RangeArr<Star_t, 1, 1000> Star;
std::string GoToLevel;
std::string StartLevel;
bool NoMap = false;
bool RestartLevel = false;
float LevelChop[maxSections + 1];
RangeArr<int, -FLBlocks, FLBlocks> FirstBlock;
RangeArr<int, -FLBlocks, FLBlocks> LastBlock;
int MidBackground = 1;
int LastBackground = 1;
int iBlocks = 0;
RangeArr<int, 0, maxBlocks> iBlock;
int numTiles = 0;
int numScenes = 0;
RangeArr<std::string, 0, maxSections> CustomMusic;
RangeArr<Location_t, 0, maxSections> level;
RangeArr<bool, 0, maxSections> LevelWrap;
RangeArr<bool, 0, maxSections> OffScreenExit;
RangeArr<int, 0, maxSections> bgMusic;
RangeArr<int, 0, maxSections> bgMusicREAL;
RangeArr<int, 0, maxSections> Background2REAL;
RangeArr<Location_t, 0, maxSections> LevelREAL;
int curMusic = 0;
RangeArr<long, 0, maxSections> bgColor;
RangeArr<int, 0, maxSections> Background2;
RangeArr<WorldPath_t, 1, maxWorldPaths> WorldPath;
int numWorldPaths = 0;
int numWarps = 0;
RangeArr<Warp_t, 1, maxWarps> Warp;
RangeArr<Tile_t, 1, maxTiles> Tile;
RangeArr<Scene_t, 1, maxScenes> Scene;
RangeArr<CreditLine_t, 1, 200> Credit;
int numCredits = 0;
int numBlock = 0;
int numBackground = 0;
int numNPCs = 0;
int numEffects = 0;
int numPlayers = 0;
int numWorldLevels = 0;
RangeArr<WorldMusic_t, 1, maxWorldMusic> WorldMusic;
int numWorldMusic = 0;
RangeArr<WorldLevel_t, 1, maxWorldLevels> WorldLevel;
RangeArr<Background_t, 1, maxBackgrounds> Background;
RangeArr<Effect_t, 1, maxEffects> Effect;
RangeArr<NPC_t, -128, maxNPCs> NPC;
RangeArr<Block_t, 0, maxBlocks> Block;
RangeArr<Player_t, 0, maxPlayers> Player;
RangeArr<int, 0, maxPlayerFrames> MarioFrameX;
RangeArr<int, 0, maxPlayerFrames> MarioFrameY;
RangeArr<int, 0, maxPlayerFrames> LuigiFrameX;
RangeArr<int, 0, maxPlayerFrames> LuigiFrameY;
RangeArr<int, 0, maxPlayerFrames> PeachFrameX;
RangeArr<int, 0, maxPlayerFrames> PeachFrameY;
RangeArr<int, 0, maxPlayerFrames> ToadFrameX;
RangeArr<int, 0, maxPlayerFrames> ToadFrameY;
RangeArr<int, 0, maxPlayerFrames> LinkFrameX;
RangeArr<int, 0, maxPlayerFrames> LinkFrameY;
RangeArr<bool, 0, maxBackgroundType> BackgroundFence;
RangeArr<int, 0, maxNPCType> NPCFrameOffsetX;
RangeArr<int, 0, maxNPCType> NPCFrameOffsetY;
RangeArr<int, 0, maxNPCType> NPCWidth;
RangeArr<int, 0, maxNPCType> NPCHeight;
RangeArr<int, 0, maxNPCType> NPCWidthGFX;
RangeArr<int, 0, maxNPCType> NPCHeightGFX;
RangeArr<float, 0, maxNPCType> NPCSpeedvar;
RangeArr<bool, 0, maxNPCType> NPCIsAShell;
RangeArr<bool, 0, maxNPCType> NPCIsABlock;
RangeArr<bool, 0, maxNPCType> NPCIsAHit1Block;
RangeArr<bool, 0, maxNPCType> NPCIsABonus;
RangeArr<bool, 0, maxNPCType> NPCIsACoin;
RangeArr<bool, 0, maxNPCType> NPCIsAVine;
RangeArr<bool, 0, maxNPCType> NPCIsAnExit;
RangeArr<bool, 0, maxNPCType> NPCIsAParaTroopa;
RangeArr<bool, 0, maxNPCType> NPCIsCheep;
RangeArr<bool, 0, maxNPCType> NPCJumpHurt;
RangeArr<bool, 0, maxNPCType> NPCNoClipping;
RangeArr<int, 0, maxNPCType> NPCScore;
RangeArr<bool, 0, maxNPCType> NPCCanWalkOn;
RangeArr<bool, 0, maxNPCType> NPCGrabFromTop;
RangeArr<bool, 0, maxNPCType> NPCTurnsAtCliffs;
RangeArr<bool, 0, maxNPCType> NPCWontHurt;
RangeArr<bool, 0, maxNPCType> NPCMovesPlayer;
RangeArr<bool, 0, maxNPCType> NPCStandsOnPlayer;
RangeArr<bool, 0, maxNPCType> NPCIsGrabbable;
RangeArr<bool, 0, maxNPCType> NPCIsBoot;
RangeArr<bool, 0, maxNPCType> NPCIsYoshi;
RangeArr<bool, 0, maxNPCType> NPCIsToad;
RangeArr<bool, 0, maxNPCType> NPCNoYoshi;
RangeArr<bool, 0, maxNPCType> NPCForeground;
RangeArr<bool, 0, maxNPCType> NPCIsABot;
RangeArr<bool, 0, maxNPCType> NPCDefaultMovement;
RangeArr<bool, 0, maxNPCType> NPCIsVeggie;
RangeArr<bool, 0, maxNPCType> NPCNoFireBall;
RangeArr<bool, 0, maxNPCType> NPCNoIceBall;
RangeArr<bool, 0, maxNPCType> NPCNoGravity;

RangeArr<int, 0, maxNPCType> NPCFrame;
RangeArr<int, 0, maxNPCType> NPCFrameSpeed;
RangeArr<int, 0, maxNPCType> NPCFrameStyle;

NPCDefaults_t NPCDefaults;

RangeArr<bool, 0, maxBlockType> BlockIsSizable;
RangeArr<int, 0, maxBlockType> BlockSlope;
RangeArr<int, 0, maxBlockType> BlockSlope2;
RangeArr<double, 0, maxPlayers> vScreenX;
RangeArr<double, 0, maxPlayers> vScreenY;

RangeArr<double, 0, maxPlayers> qScreenX;
RangeArr<double, 0, maxPlayers> qScreenY;
bool qScreen = false;

RangeArr<int, 0, maxBlockType> BlockWidth;
RangeArr<int, 0, maxBlockType> BlockHeight;
RangeArr<int, 0, maxBlockType> BonusWidth;
RangeArr<int, 0, maxBlockType> BonusHeight;
RangeArr<int, 0, maxBlockType> EffectWidth;
RangeArr<int, 0, maxBlockType> EffectHeight;

EffectDefaults effectDefaults;
RangeArr<int, 1, maxSceneType> SceneWidth;
RangeArr<int, 1, maxSceneType> SceneHeight;
RangeArr<int, 0, 100> SpecialFrame;
RangeArr<float, 0, 100> SpecialFrameCount;
RangeArr<bool, 1, maxBackgroundType> BackgroundHasNoMask;
RangeArr<bool, 0, maxBackgroundType> Foreground;
RangeArr<int, 1, maxBackgroundType> BackgroundWidth;
RangeArr<int, 1, maxBackgroundType> BackgroundHeight;
RangeArr<int, 1, maxBackgroundType> BackgroundFrame;
RangeArr<int, 1, maxBackgroundType> BackgroundFrameCount;
RangeArr<int, 1, maxBlockType> BlockFrame;
RangeArr<int, 1, maxBlockType> BlockFrame2;
RangeArr<int, 1, 1000> sBlockArray;
int sBlockNum = 0;
RangeArr<int, 1, maxSceneType> SceneFrame;
RangeArr<int, 1, maxSceneType> SceneFrame2;
RangeArr<int, 1, maxTileType> TileWidth;
RangeArr<int, 1, maxTileType> TileHeight;
RangeArr<int, 1, maxTileType> TileFrame;
RangeArr<int, 1, maxTileType> TileFrame2;
RangeArr<int, 1, 100> LevelFrame;
RangeArr<int, 1, 100> LevelFrame2;
RangeArr<bool, 1, maxBlockType> BlockHasNoMask;
RangeArr<bool, 1, 100> LevelHasNoMask;
RangeArr<bool, 0, maxBlockType> BlockOnlyHitspot1;
RangeArr<bool, 0, maxBlockType> BlockKills;
RangeArr<bool, 0, maxBlockType> BlockKills2;
RangeArr<bool, 0, maxBlockType> BlockHurts;
RangeArr<bool, 0, maxBlockType> BlockPSwitch;
RangeArr<bool, 0, maxBlockType> BlockNoClipping;
RangeArr<int, 1, 10> CoinFrame;
RangeArr<int, 1, 10> CoinFrame2;
EditorCursor_t EditorCursor;

RangeArr<int, 1, numSounds> Sound;
RangeArr<int, 1, numSounds> SoundPause;
bool EndLevel = false;
int LevelMacro = 0;
int LevelMacroCounter = 0;
int numJoysticks = 0;
std::string FileName;
int Coins = 0;
float Lives = 0.0f;
bool EndIntro = false;
bool ExitMenu = false;
bool LevelSelect = false;
RangeArr<WorldPlayer_t, 0, 1> WorldPlayer;
int LevelBeatCode = 0;
int curWorldLevel = 0;
int curWorldMusic = 0;
RangeArr<bool, 0, maxSections> NoTurnBack;
RangeArr<bool, 0, maxSections> UnderWater;
bool TestLevel = false;
std::string FullFileName;
std::string FileNamePath;
bool GameMenu = false;
std::string WorldName;
int selWorld = 0;
int selSave = 0;
int PSwitchTime = 0;
int PSwitchStop = 0;
int PSwitchPlayer = 0;
RangeArr<int, 1, 3> SaveSlot;
RangeArr<int, 1, 3> SaveStars;
int BeltDirection = 0;
bool BeatTheGame = false;
int cycleCount = 0;
double fpsTime = 0.0;
double fpsCount = 0.0;
bool FrameSkip = false;
double GoalTime = 0.0;
double overTime = 0.0;
int worldCurs = 0;
int minShow = 0;
int maxShow = 0;

RangeArr<Layer_t, 0, maxLayers> Layer;
RangeArr<Events_t, 0, maxEvents> Events;
int ReturnWarp = 0;
int StartWarp = 0;
Physics_t Physics;
int MenuCursor = 0;
int MenuMode = 0;
bool MenuCursorCanMove = false;
bool MenuCursorCanMove2 = false;
bool NextFrame = false;
int StopHit = 0;
bool MouseRelease = false;
bool TestFullscreen = false;
bool keyDownAlt = false;
bool keyDownEnter = false;
bool BlocksSorted = false;
int SingleCoop = 0;
std::string CheatString;
bool GameOutro = false;
float CreditChop = 0.0f;
int EndCredits = 0;
int curStars = 0;
int maxStars = 0;
bool ShadowMode = false;
bool MultiHop = false;
bool SuperSpeed = false;
bool WalkAnywhere = false;
bool FlyForever = false;
bool FreezeNPCs = false;
bool CaptainN = false;
bool FlameThrower = false;
bool CoinMode = false;
bool WorldUnlock = false;
bool MaxFPS = false;
bool GodMode = false;
bool GrabAll = false;
bool Cheater = false;
RangeArr<std::string, 1, maxWorldCredits> WorldCredits;
int Score = 0;
RangeArr<int, 1, 13> Points;
int oldJumpJoy = 0;
int MaxWorldStars = 0;
bool Debugger = false;
RangeArr<Player_t, 0, 10> SavedChar;

int LoadCoins = 0;
float LoadCoinsT = 0.0f;

RangeArr<bool, 1, maxBlockType> GFXBlockCustom;
//RangeArr<long, 1, maxBlockType> GFXBlock;
RangeArr<long, 1, maxBlockType> GFXBlockMask;
RangeArr<StdPicture, 1, maxBlockType> GFXBlockBMP;
RangeArr<StdPicture, 1, maxBlockType> GFXBlockMaskBMP;
RangeArr<bool, 1, numBackground2> GFXBackground2Custom;
//RangeArr<long, 1, numBackground2> GFXBackground2;
RangeArr<StdPicture, 1, numBackground2> GFXBackground2BMP;
RangeArr<int, 1, numBackground2> GFXBackground2Height;
RangeArr<int, 1, numBackground2> GFXBackground2Width;
RangeArr<bool, 1, maxNPCType> GFXNPCCustom;
//RangeArr<long, 1, maxNPCType> GFXNPC;
RangeArr<long, 1, maxNPCType> GFXNPCMask;
RangeArr<StdPicture, 1, maxNPCType> GFXNPCBMP;
RangeArr<StdPicture, 1, maxNPCType> GFXNPCMaskBMP;
RangeArr<int, 1, maxNPCType> GFXNPCHeight;
RangeArr<int, 1, maxNPCType> GFXNPCWidth;
RangeArr<bool, 1, maxEffectType> GFXEffectCustom;
//RangeArr<long, 1, maxEffectType> GFXEffect;
RangeArr<long, 1, maxEffectType> GFXEffectMask;
RangeArr<StdPicture, 1, maxEffectType> GFXEffectBMP;
RangeArr<StdPicture, 1, maxEffectType> GFXEffectMaskBMP;
RangeArr<int, 1, maxEffectType> GFXEffectHeight;
RangeArr<int, 1, maxEffectType> GFXEffectWidth;
RangeArr<bool, 1, maxBackgroundType> GFXBackgroundCustom;
//RangeArr<long, 1, maxBackgroundType> GFXBackground;
RangeArr<long, 1, maxBackgroundType> GFXBackgroundMask;
RangeArr<StdPicture, 1, maxBackgroundType> GFXBackgroundBMP;
RangeArr<StdPicture, 1, maxBackgroundType> GFXBackgroundMaskBMP;
RangeArr<int, 1, maxBackgroundType> GFXBackgroundHeight;
RangeArr<int, 1, maxBackgroundType> GFXBackgroundWidth;
RangeArr<bool, 1, 10> GFXMarioCustom;
//RangeArr<long, 1, 10> GFXMario;
RangeArr<long, 1, 10> GFXMarioMask;
RangeArr<StdPicture, 1, 10> GFXMarioBMP;
RangeArr<StdPicture, 1, 10> GFXMarioMaskBMP;
RangeArr<int, 1, 10> GFXMarioHeight;
RangeArr<int, 1, 10> GFXMarioWidth;
RangeArr<bool, 1, 10> GFXLuigiCustom;
//RangeArr<long, 1, 10> GFXLuigi;
RangeArr<long, 1, 10> GFXLuigiMask;
RangeArr<StdPicture, 1, 10> GFXLuigiBMP;
RangeArr<StdPicture, 1, 10> GFXLuigiMaskBMP;
RangeArr<int, 1, 10> GFXLuigiHeight;
RangeArr<int, 1, 10> GFXLuigiWidth;
RangeArr<bool, 1, 10> GFXPeachCustom;
//RangeArr<long, 1, 10> GFXPeach;
RangeArr<long, 1, 10> GFXPeachMask;
RangeArr<StdPicture, 1, 10> GFXPeachBMP;
RangeArr<StdPicture, 1, 10> GFXPeachMaskBMP;
RangeArr<int, 1, 10> GFXPeachHeight;
RangeArr<int, 1, 10> GFXPeachWidth;
RangeArr<bool, 1, 10> GFXToadCustom;
//RangeArr<long, 1, 10> GFXToad;
RangeArr<long, 1, 10> GFXToadMask;
RangeArr<StdPicture, 1, 10> GFXToadBMP;
RangeArr<StdPicture, 1, 10> GFXToadMaskBMP;
RangeArr<int, 1, 10> GFXToadHeight;
RangeArr<int, 1, 10> GFXToadWidth;

RangeArr<bool, 1, 10> GFXLinkCustom;
//RangeArr<long, 1, 10> GFXLink;
RangeArr<long, 1, 10> GFXLinkMask;
RangeArr<StdPicture, 1, 10> GFXLinkBMP;
RangeArr<StdPicture, 1, 10> GFXLinkMaskBMP;
RangeArr<int, 1, 10> GFXLinkHeight;
RangeArr<int, 1, 10> GFXLinkWidth;

RangeArr<bool, 1, 10> GFXYoshiBCustom;
//RangeArr<long, 1, 10> GFXYoshiB;
RangeArr<long, 1, 10> GFXYoshiBMask;
RangeArr<StdPicture, 1, 10> GFXYoshiBBMP;
RangeArr<StdPicture, 1, 10> GFXYoshiBMaskBMP;
RangeArr<bool, 1, 10> GFXYoshiTCustom;
//RangeArr<long, 1, 10> GFXYoshiT;
RangeArr<long, 1, 10> GFXYoshiTMask;
RangeArr<StdPicture, 1, 10> GFXYoshiTBMP;
RangeArr<StdPicture, 1, 10> GFXYoshiTMaskBMP;
RangeArr<bool, 1, maxTileType> GFXTileCustom;
//RangeArr<long, 1, maxTileType> GFXTile;
RangeArr<StdPicture, 1, maxTileType> GFXTileBMP;
RangeArr<int, 1, maxTileType> GFXTileHeight;
RangeArr<int, 1, maxTileType> GFXTileWidth;
RangeArr<bool, 0, maxLevelType> GFXLevelCustom;
//RangeArr<long, 0, maxLevelType> GFXLevel;
RangeArr<long, 0, maxLevelType> GFXLevelMask;
RangeArr<StdPicture, 0, maxLevelType> GFXLevelBMP;
RangeArr<StdPicture, 0, maxLevelType> GFXLevelMaskBMP;
RangeArr<int, 0, maxLevelType> GFXLevelHeight;
RangeArr<int, 0, maxLevelType> GFXLevelWidth;
RangeArr<bool, 0, maxLevelType> GFXLevelBig;
RangeArr<bool, 1, maxSceneType> GFXSceneCustom;
//RangeArr<long, 1, maxSceneType> GFXScene;
RangeArr<long, 1, maxSceneType> GFXSceneMask;
RangeArr<StdPicture, 1, maxSceneType> GFXSceneBMP;
RangeArr<StdPicture, 1, maxSceneType> GFXSceneMaskBMP;
RangeArr<int, 1, maxSceneType> GFXSceneHeight;
RangeArr<int, 1, maxSceneType> GFXSceneWidth;
RangeArr<bool, 1, maxPathType> GFXPathCustom;
//RangeArr<long, 1, maxPathType> GFXPath;
RangeArr<long, 1, maxPathType> GFXPathMask;
RangeArr<StdPicture, 1, maxPathType> GFXPathBMP;
RangeArr<StdPicture, 1, maxPathType> GFXPathMaskBMP;
RangeArr<int, 1, maxPathType> GFXPathHeight;
RangeArr<int, 1, maxPathType> GFXPathWidth;

RangeArr<bool, 1, numCharacters> GFXPlayerCustom;
//RangeArr<long, 1, numCharacters> GFXPlayer;
RangeArr<long, 1, numCharacters> GFXPlayerMask;
RangeArr<StdPicture, 1, numCharacters> GFXPlayerBMP;
RangeArr<StdPicture, 1, numCharacters> GFXPlayerMaskBMP;
RangeArr<int, 1, numCharacters> GFXPlayerHeight;
RangeArr<int, 1, numCharacters> GFXPlayerWidth;

int PlayerCharacter = 0;
int PlayerCharacter2 = 0;
double MenuMouseX = 0.0;
double MenuMouseY = 0.0;
bool MenuMouseDown = false;
bool MenuMouseBack = false;
bool MenuMouseRelease = false;
bool MenuMouseMove = false;
bool MenuMouseClick = false;

RangeArr<std::string, 1, 100> NewEvent;
RangeArr<int, 1, 100> newEventDelay;
int newEventNum = 0;
bool ForcedControls = false;
Controls_t ForcedControl;
int SyncCount = 0;
bool noUpdate = false;
double gameTime = 0.0;
bool noSound = false;
double tempTime = 0.0;
bool BattleMode = false;
int BattleWinner = 0;
RangeArr<int, 1, maxPlayers> BattleLives;
int BattleIntro = 0;
int BattleOutro = 0;
std::string LevelName;

void DoEvents()
{
    frmMain.doEvents();
}

int ShowCursor(int show)
{
    return SDL_ShowCursor(show);
}

Uint8 getKeyState(SDL_Scancode key)
{
    return frmMain.getKeyState(key);
}

Uint8 getKeyStateI(int key)
{
    if(key < 0)
        return 0;
    return frmMain.getKeyState(static_cast<SDL_Scancode>(key));
}

const char *getKeyName(int key)
{
    if(key < 0)
        return " ... ";
    SDL_Scancode k = static_cast<SDL_Scancode>(key);
    return SDL_GetScancodeName(k);
}


void initAll()
{
    SavedEvents.fill(std::string());
    BlockSwitch.fill(false);
    PowerUpUnlock.fill(false);
    conKeyboard.fill(ConKeyboard_t());
    conJoystick.fill(ConJoystick_t());
    useJoystick.fill(0);
    testPlayer.fill(Player_t());
    SelectWorld.fill(SelectWorld_t());
    vScreen.fill(vScreen_t());
    PlayerStart.fill(Location_t());
    blockCharacter.fill(false);
    OwedMount.fill(0);
    OwedMountType.fill(0);
    AutoX.fill(0.f);
    AutoY.fill(0.f);
    Water.fill(Water_t());
    Star.fill(Star_t());
    FirstBlock.fill(0);
    LastBlock.fill(0);
    iBlock.fill(0);
    CustomMusic.fill(std::string());
    level.fill(Location_t());
    LevelWrap.fill(false);
    OffScreenExit.fill(false);
    bgMusic.fill(0);
    bgMusicREAL.fill(0);
    Background2REAL.fill(0);
    Background2.fill(0);

    Block.fill(Block_t());
    Background.fill(Background_t());
    NPC.fill(NPC_t());
}
