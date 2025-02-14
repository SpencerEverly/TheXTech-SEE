/*
 * TheXTech - A platform game engine ported from old source code for VB6
 *
 * Copyright (c) 2009-2011 Andrew Spinks, original VB6 code
 * Copyright (c) 2020-2023 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../globals.h"
#include "../npc.h"
#include "../npc_id.h"
#include "../eff_id.h"
#include "../sound.h"
#include "../collision.h"
#include "../effect.h"
#include "../graphics.h"
#include "../player.h"
#include "../game_main.h"
#include "../core/events.h"
#include "../compat.h"
#include "../config.h"
#include "../layers.h"

#ifdef ENABLE_XTECH_LUA
#include "../script/lunalua/xtech_lua_main.h"
#include "../script/lunalua/eventproxy/xtech_lua_eventproxy.h"
#endif

#include "npc/npc_queues.h"

#include "../controls.h"

#include <Utils/maths.h>
#include <Logger/logger.h>
#ifdef THEXTECH_INTERPROC_SUPPORTED
#   include <InterProcess/intproc.h>
#endif


static bool doPlayGrowWithGotItem()
{
    /*switch(g_compatibility.sfx_player_grow_with_got_item)
    {
    default:
    case Compatibility_t::SPGWGI_UNSPECIFIED:
        return g_config.SoundPlayerGrowWithGetItem;
    case Compatibility_t::SPGWGI_ENABLE:
        return true;
    case Compatibility_t::SPGWGI_DISABLE:
        return false;
    }*/
    return true;
}

static void s_PowerupScore(NPCRef_t n)
{
    if(g_compatibility.custom_powerup_collect_score)
        MoreScore(NPCScore[n->Type], n->Location);
    else
        MoreScore(6, n->Location);
}


inline void RumbleForPowerup(int A)
{
    Controls::Rumble(A, 200, 0.25);
}

void DropBonus(int A)
{
    int B = 0;
    //    if(nPlay.Online == true && nPlay.MySlot + 1 != A)
    //        return;
    if(A == 1 || numPlayers == 2 /*|| nPlay.Online == true*/)
    {
        if(Player[A].Character == 3 || Player[A].Character == 4)
        {
            Player[A].HeldBonus = 0;
            return;
        }

        if(Player[A].HeldBonus > 0)
        {
            PlaySound(SFX_DropItem);
            numNPCs++;
            NPC[numNPCs] = NPC_t();
            NPC[numNPCs].Type = Player[A].HeldBonus;
            NPC[numNPCs].Location.Width = NPCWidth[Player[A].HeldBonus];
            NPC[numNPCs].Location.Height = 32;

            const Screen_t& screen = ScreenByPlayer(A);

            if(screen.Type == 5 && !screen.vScreen(2).Visible /*&& nPlay.Online == false*/)
            {
                vScreen_t& vscreen = screen.vScreen(1);

                if(A == 1)
                    B = -40;
                if(A == 2)
                    B = 40;

                GetvScreenAverage(vscreen);

                double ScreenTop = -vscreen.Y;

                if(vscreen.Height > 600)
                    ScreenTop += vscreen.Height / 2 - 300;

                double CenterX = -vscreen.X + vscreen.Width / 2;

                NPC[numNPCs].Location.X = CenterX - NPC[numNPCs].Location.Width / 2.0 + B;
                NPC[numNPCs].Location.Y = ScreenTop + 16 + 12;
            }
                //            else if(nPlay.Online == true)
                //            {
                //                GetvScreen 1;
                //                NPC[numNPCs].Location.X = -vScreen[1].X + vScreen[1].Width / 2.0 - NPC[numNPCs].Location.Width / 2.0;
                //                NPC[numNPCs].Location.Y = -vScreen[1].Y + 16 + 12;
                //            }
            else
            {
                vScreen_t& vscreen = vScreenByPlayer(A);

                GetvScreen(vscreen);

                double ScreenTop = -vscreen.Y;

                if(vscreen.Height > 600)
                    ScreenTop += vscreen.Height / 2 - 300;

                double CenterX = -vscreen.X + vscreen.Width / 2;

                NPC[numNPCs].Location.X = CenterX - NPC[numNPCs].Location.Width / 2.0;
                NPC[numNPCs].Location.Y = ScreenTop + 16 + 12;
            }

            NPC[numNPCs].Location.SpeedX = 0;
            NPC[numNPCs].Location.SpeedY = 0;
            NPC[numNPCs].Effect = 2;
            NPC[numNPCs].Effect2 = 1;
            NPC[numNPCs].Active = true;
            NPC[numNPCs].TimeLeft = 200;
            syncLayers_NPC(numNPCs);
            CheckSectionNPC(numNPCs);
            Player[A].HeldBonus = 0;
            //            if(nPlay.Online == true)
            //                Netplay::sendData "M" + NPC[numNPCs].Type + "|" + NPC[numNPCs].Location.X + "|" + NPC[numNPCs].Location.Y + LB;
        }
    }
    else
        Player[A].HeldBonus = 0;
}

void CheckAfterStarTake(bool many)
{
    int allBGOs = numBackground + numLocked;
    for(int c = 1; c <= numWarps; c++)
    {
        auto &w = Warp[c];
        if((!many && (w.Stars == numStars)) || (many && (w.Stars <= numStars)))
        {
            for(int d = numBackground + 1; d <= allBGOs; d++)
            {
                auto &b = Background[d];
                if(b.Type == 160 &&
                    (CheckCollision(w.Entrance, b.Location) ||
                     (w.twoWay && CheckCollision(w.Exit, b.Location)))
                )
                {
                    // this makes the background permanently disappear
                    b.Layer = LAYER_NONE;
                    b.Hidden = true;
                    syncLayers_BGO(d);
                }
            }
        }
    }
}

void TouchBonus(int A, int B)
{
    int C = 0;
    // int D = 0;
    int toadBool = 0;
    bool tempBool = false;
    Location_t tempLocation;

    if(NPC[B].CantHurtPlayer != A || (NPCIsACoin[NPC[B].Type] && Player[A].HoldingNPC != B && NPC[B].Killed == 0))
    {
        //        if(nPlay.Online == true && nPlay.MySlot + 1 == A)
        //            Netplay::sendData "1k" + std::to_string(A) + "|" + std::to_string(B) + "|" + NPC[B].Type + LB;
        if(Player[A].Character == 3 || Player[A].Character == 4 || Player[A].Character == 5) // for link give hearts
        {
            if(NPC[B].Type == NPCID_LEAF_POWER || NPC[B].Type == NPCID_STATUE_POWER || NPC[B].Type == NPCID_HEAVY_POWER)
            {
                Player[A].Hearts += 1;
                if(Player[A].Hearts > 3)
                    Player[A].Hearts = 3;
            }
        }

        // If .Character = 3 Or .Character = 4 Then  'for peach and toad turn leaf and suits into a mushroom
        // If NPC(B).Type = 34 Or NPC(B).Type = 169 Or NPC(B).Type = 170 Then NPC(B).Type = 9
        // End If

        if(NPC[B].Type == NPCID_SWAP_POWER && numPlayers > 1) // ? mushroom
        {
            // TODO: generalize this for >2P (find another living player besides A and swap A with it)

            if(!Player[1].Dead && Player[1].TimeToLive == 0 && !Player[2].Dead &&
                Player[2].TimeToLive == 0 && Player[1].Immune == 0 && Player[2].Immune == 0)
            {
                tempLocation = Player[1].Location;
                Player[1].Location.X = Player[2].Location.X + Player[2].Location.Width / 2.0 - Player[1].Location.Width / 2.0;
                Player[1].Location.Y = Player[2].Location.Y + Player[2].Location.Height - Player[1].Location.Height;
                Player[2].Location.X = tempLocation.X + Player[1].Location.Width / 2.0 - Player[2].Location.Width / 2.0;
                Player[2].Location.Y = tempLocation.Y + Player[1].Location.Height - Player[2].Location.Height;
                C = Player[1].Direction;
                Player[1].Direction = Player[2].Direction;
                Player[2].Direction = C;
                C = Player[1].Slope;
                Player[1].Slope = Player[2].Slope;
                Player[2].Slope = C;
                C = Player[1].StandingOnNPC;
                Player[1].StandingOnNPC = Player[2].StandingOnNPC;
                Player[2].StandingOnNPC = C;
                if(Player[1].Immune < 10)
                    Player[1].Immune = 10;
                if(Player[2].Immune < 10)
                    Player[2].Immune = 10;
                NPC[B].Killed = 9;
                NPCQueues::Killed.push_back(B);
                PlaySound(SFX_BossBeat);
                return;
            }
        }

        if(NPC[B].Type == NPCID_FLY_POWER) // Player is a fairy
        {
            if(Player[A].Mount == 2)
                return;
            else
            {
                if(!Player[A].Fairy)
                {
                    Player[A].Immune = 30;
                    Player[A].Effect = 8;
                    Player[A].Effect2 = 4;
                    Player[A].Fairy = true;
                    SizeCheck(Player[A]);
                    NewEffect(EFFID_SMOKE_S5, Player[A].Location);
                }
                PlaySound(SFX_HeroFairy);
                Player[A].FairyTime = -1;
                NPC[B].Killed = 9;
                NPCQueues::Killed.push_back(B);
            }
        }
        if(NPC[B].Type == NPCID_LIFE_S3 || NPC[B].Type == NPCID_LIFE_S4 || NPC[B].Type == NPCID_LIFE_S1) // player touched a 1up mushroom
        {
            NPC[B].Killed = 9;
            NPCQueues::Killed.push_back(B);
            MoreScore(10, NPC[B].Location);
            return;
        }
        if(NPC[B].Type == NPCID_TIMER_S3 && NPC[B].Effect != 2 && (Player[A].Character == 1 || Player[A].Character == 2)) // send the clock to the item container
        {
            Player[A].HeldBonus = 248;
            NPC[B].Killed = 9;
            NPCQueues::Killed.push_back(B);
            PlaySound(SFX_GotItem);
            return;
        }
        if(NPC[B].Type == NPCID_TIMER_S2 || NPC[B].Type == NPCID_TIMER_S3) // player touched the clock
        {
            PSwitchStop = Physics.NPCPSwitch;
            FreezeNPCs = true;
            PSwitchPlayer = A;
            NPC[B].Killed = 9;
            NPCQueues::Killed.push_back(B);
            return;
        }
        if(NPC[B].Type == NPCID_CHECKPOINT) // player touched the chekpoint
        {
            RumbleForPowerup(A);
            if(Player[A].State == 1)
                Player[A].State = 2;
            if(Player[A].Hearts == 1)
                Player[A].Hearts = 2;
            SizeCheck(Player[A]);
            NPC[B].Killed = 9;
            NPCQueues::Killed.push_back(B);
            PlaySound(SFX_Checkpoint);
            Checkpoint = FullFileName;
            Checkpoint_t cp;
            cp.id = Maths::iRound(NPC[B].Special);
            CheckpointsList.push_back(cp);
            pLogDebug("Added checkpoint ID %d", cp.id);

#ifdef ENABLE_XTECH_LUA
            std::shared_ptr<Event> checkpointEvent = std::make_shared<Event>("onCheckpoint", false);
            checkpointEvent->setLoopable(false);
            checkpointEvent->setDirectEventName("onCheckpoint");
            xtech_lua_callLuaEvent(checkpointEvent, cp.id, A);
#endif
            return;
        }
        if(NPC[B].Type == NPCID_3_LIFE) // player touched the 3up moon
        {
            NPC[B].Killed = 9;
            NPCQueues::Killed.push_back(B);
            MoreScore(12, NPC[B].Location);
            return;
        }
        if(NPC[B].Type == NPCID_AXE)
        {
            NPC[B].Killed = 9;
            NPCQueues::Killed.push_back(B);
            return;
        }
        if(NPCIsToad[NPC[B].Type])
        {
            toadBool = NPC[B].Type;
            NPC[B].Type = NPCID_POWER_S3;
        }
        if(NPC[B].Type == NPCID_POISON) // Bonus is a POISON mushroom
            PlayerHurt(A);
        else if(NPC[B].Type == NPCID_POWER_S3 || NPC[B].Type == NPCID_POWER_S1 || NPC[B].Type == NPCID_POWER_S4 || NPC[B].Type == NPCID_POWER_S2 || NPC[B].Type == NPCID_POWER_S5) // Bonus is a mushroom
        {
            if(Player[A].Character == 5 && Player[A].State == 1)
                Player[A].State = 2;
            if(Player[A].Character == 3 || Player[A].Character == 4 || Player[A].Character == 5)
            {
                Player[A].Hearts += 1;
                if(Player[A].Hearts > 3)
                    Player[A].Hearts = 3;
            }

            UpdatePlayerBonus(A, NPC[B].Type);
            if(Player[A].State == 1 && Player[A].Character != 5)
            {
                RumbleForPowerup(A);
                if(Player[A].Duck)
                    UnDuck(Player[A]);
                Player[A].StateNPC = NPC[B].Type;
                Player[A].Frame = 1;
                Player[A].Effect = 1;
                if(Player[A].Mount > 0)
                    UnDuck(Player[A]);
                PlaySound(SFX_PlayerGrow);
            }
            else if(NPC[B].Type == NPCID_POWER_S5)
                PlaySound(SFX_HeroHeart);
            else
            {
                if(doPlayGrowWithGotItem())
                    PlaySound(SFX_PlayerGrow);
                PlaySound(SFX_GotItem);
            }
            if(NPC[B].Effect != 2)
                s_PowerupScore(B);
        }
        else if(NPC[B].Type == NPCID_FIRE_POWER_S3 || NPC[B].Type == NPCID_FIRE_POWER_S1 || NPC[B].Type == NPCID_FIRE_POWER_S4) // Bonus is a fire flower
        {
            if(Player[A].Character == 3 || Player[A].Character == 4 || Player[A].Character == 5)
            {
                Player[A].Hearts += 1;
                if(Player[A].Hearts > 3)
                    Player[A].Hearts = 3;
            }
            UpdatePlayerBonus(A, NPC[B].Type);
            Player[A].StateNPC = NPC[B].Type;
            if(Player[A].State != 3)
            {
                RumbleForPowerup(A);
                Player[A].Frame = 1;
                Player[A].Effect = 4;
                if(Player[A].Mount > 0)
                    UnDuck(Player[A]);
                if(Player[A].Character == 5)
                    PlaySound(SFX_HeroItem);
                else
                    PlaySound(SFX_PlayerGrow);
            }
            else
            {
                if(Player[A].Character == 5)
                    PlaySound(SFX_HeroHeart);
                else
                {
                    if(doPlayGrowWithGotItem())
                        PlaySound(SFX_PlayerGrow);
                    PlaySound(SFX_GotItem);
                }
            }
            if(NPC[B].Effect != 2)
                s_PowerupScore(B);
        }
        else if(NPC[B].Type == NPCID_ICE_POWER_S3 || NPC[B].Type == NPCID_ICE_POWER_S4) // Bonus is an ice flower
        {
            if(Player[A].Character == 3 || Player[A].Character == 4 || Player[A].Character == 5)
            {
                Player[A].Hearts += 1;
                if(Player[A].Hearts > 3)
                    Player[A].Hearts = 3;
            }

            UpdatePlayerBonus(A, NPC[B].Type);
            Player[A].StateNPC = NPC[B].Type;
            if(Player[A].State != 7)
            {
                RumbleForPowerup(A);
                Player[A].Frame = 1;
                Player[A].Effect = 41;
                if(Player[A].Mount > 0)
                    UnDuck(Player[A]);
                if(Player[A].Character == 5)
                    PlaySound(SFX_HeroItem);
                else
                    PlaySound(SFX_PlayerGrow);
            }
            else
            {
                if(Player[A].Character == 5)
                    PlaySound(SFX_HeroHeart);
                else
                {
                    if(doPlayGrowWithGotItem())
                        PlaySound(SFX_PlayerGrow);
                    PlaySound(SFX_GotItem);
                }
            }
            if(NPC[B].Effect != 2)
                s_PowerupScore(B);
        }
        else if(NPC[B].Type == NPCID_BUBBLE_POWER_S3) // Bonus is a bubble flower
        {
            if(Player[A].Character == 3 || Player[A].Character == 4 || Player[A].Character == 5)
            {
                Player[A].Hearts += 1;
                if(Player[A].Hearts > 3)
                    Player[A].Hearts = 3;
            }

            UpdatePlayerBonus(A, NPC[B].Type);
            Player[A].StateNPC = NPC[B].Type;
            if(Player[A].State != 8)
            {
                RumbleForPowerup(A);
                Player[A].Frame = 1;
                Player[A].Effect = 42;
                if(Player[A].Mount > 0)
                    UnDuck(Player[A]);
                if(Player[A].Character == 5)
                    PlaySound(SFX_HeroItem);
                else
                    PlaySound(SFX_PlayerGrow);
            }
            else
            {
                if(Player[A].Character == 5)
                    PlaySound(SFX_HeroHeart);
                else
                {
                    if(doPlayGrowWithGotItem())
                        PlaySound(SFX_PlayerGrow);
                    PlaySound(SFX_GotItem);
                }
            }
            if(NPC[B].Effect != 2)
                s_PowerupScore(B);
        }
        else if(NPC[B].Type == NPCID_LEAF_POWER) // Bonus is a leaf
        {
            UpdatePlayerBonus(A, NPC[B].Type);
            Player[A].StateNPC = NPC[B].Type;
            if(Player[A].State != 4)
            {
                RumbleForPowerup(A);
                Player[A].Frame = 1;
                Player[A].Effect = 5;
                Player[A].Effect2 = 0;
                if(Player[A].Mount > 0)
                    UnDuck(Player[A]);
                if(Player[A].Character == 5)
                    PlaySound(SFX_HeroItem);
                else
                    PlaySound(SFX_Transform);
            }
            else
            {
                if(Player[A].Character == 5)
                    PlaySound(SFX_HeroHeart);
                else
                {
                    if(doPlayGrowWithGotItem())
                        PlaySound(SFX_PlayerGrow);
                    PlaySound(SFX_GotItem);
                }
            }
            if(NPC[B].Effect != 2)
                s_PowerupScore(B);
        }
        else if(NPC[B].Type == NPCID_STATUE_POWER) // Bonus is a Tanooki Suit
        {
            UpdatePlayerBonus(A, NPC[B].Type);
            Player[A].StateNPC = NPC[B].Type;
            if(Player[A].State != 5)
            {
                RumbleForPowerup(A);
                Player[A].Frame = 1;
                Player[A].Effect = 11;
                Player[A].Effect2 = 0;
                if(Player[A].Mount > 0)
                    UnDuck(Player[A]);
                if(Player[A].Character == 5)
                    PlaySound(SFX_HeroItem);
                else
                    PlaySound(SFX_Transform);
            }
            else
            {
                if(Player[A].Character == 5)
                    PlaySound(SFX_HeroHeart);
                else
                {
                    if(doPlayGrowWithGotItem())
                        PlaySound(SFX_PlayerGrow);
                    PlaySound(SFX_GotItem);
                }
            }
            if(NPC[B].Effect != 2)
                s_PowerupScore(B);
        }
        else if(NPC[B].Type == NPCID_HEAVY_POWER) // Bonus is a Hammer Suit
        {
            UpdatePlayerBonus(A, NPC[B].Type);
            Player[A].StateNPC = NPC[B].Type;
            if(Player[A].State != 6)
            {
                RumbleForPowerup(A);
                Player[A].Frame = 1;
                Player[A].Effect = 12;
                Player[A].Effect2 = 0;
                if(Player[A].Mount > 0)
                    UnDuck(Player[A]);
                if(Player[A].Character == 5)
                    PlaySound(SFX_HeroItem);
                else
                    PlaySound(SFX_Transform);
            }
            else
            {
                if(Player[A].Character == 5)
                    PlaySound(SFX_HeroHeart);
                else
                {
                    if(doPlayGrowWithGotItem())
                        PlaySound(SFX_PlayerGrow);
                    PlaySound(SFX_GotItem);
                }
            }
            if(NPC[B].Effect != 2)
                s_PowerupScore(B);
        }
        else if(NPCIsACoin[NPC[B].Type]) // Bonus is a coin
        {
            if(NPC[B].Type == NPCID_RING)
                PlaySound(SFX_RingGet);
            else if(NPC[B].Type == NPCID_GEM_1 || NPC[B].Type == NPCID_GEM_5 || NPC[B].Type == NPCID_GEM_20)
                PlaySound(SFX_HeroRupee);
            else if(NPC[B].Type != NPCID_MEDAL)
                PlaySound(SFX_Coin);
            if(NPC[B].Type == NPCID_GEM_5 || NPC[B].Type == NPCID_COIN_5)
                Coins += 5;
            else if(NPC[B].Type == NPCID_GEM_20)
                Coins += 20;
            else
                Coins += 1;
            if(Coins >= 100)
            {
                if(Lives < maxLives)
                {
                    Lives += 1;
                    PlaySound(SFX_1up);
                    Coins -= 100;
                }
                else
                    Coins = 99;
            }
            if(NPC[B].Type == NPCID_MEDAL)
            {
                PlaySound(SFX_MedalGet);
                MoreScore(NPCScore[NPC[B].Type], NPC[B].Location);
                NPCScore[274] += 1;
                if(NPCScore[274] > 14)
                    NPCScore[274] = 14;
            }
            else
                MoreScore(1, NPC[B].Location);
            NewEffect(EFFID_COIN_COLLECT, NPC[B].Location);
        }
        else if(NPC[B].Type == NPCID_STARMAN_SMW || NPC[B].Type == NPCID_STARMAN_SMB3 || NPC[B].Type == NPCID_STARMAN_SMB2 || NPC[B].Type == NPCID_STARMAN_SMB1) // Bonus is a Starman
        {
            Player[A].StateNPC = NPC[B].Type;
            if(Player[A].Character == 5)
                PlaySound(SFX_HeroHeart);
            else
            {
                PlaySound(SFX_PlayerGrow);
            }
            if(NPC[B].Type == NPCID_STARMAN_SMW)
                chosenStarman = true;
            else
                chosenStarman = false;
            Player[A].Starman = true;
            Player[A].StarmanLength = maxStarmanTime;
            if(NPC[B].Effect != 2)
                s_PowerupScore(B);
        }
        else if(NPCIsAnExit[NPC[B].Type] && LevelMacro == LEVELMACRO_OFF) // Level exit
        {
            if(NPC[B].Type != NPCID_STAR_COLLECT)
            {
                TurnNPCsIntoCoins();
                FreezeNPCs = false;
                if(g_ClonedPlayerMode)
                    Player[1] = Player[A];
            }
            if(NPC[B].Type == NPCID_ITEMGOAL)
            {
                if(NPC[B].Frame == 0)
                    MoreScore(10, Player[A].Location);
                if(NPC[B].Frame == 1)
                    MoreScore(6, Player[A].Location);
                if(NPC[B].Frame == 2)
                    MoreScore(8, Player[A].Location);
                LevelMacro = LEVELMACRO_CARD_ROULETTE_EXIT;
                for(C = 1; C <= numPlayers; C++)
                {
                    if(A != C) // And DScreenType <> 5 Then
                    {
                        Player[C].Section = Player[A].Section;
                        Player[C].Location.Y = Player[A].Location.Y + Player[A].Location.Height - Player[C].Location.Height;
                        Player[C].Location.X = Player[A].Location.X + Player[A].Location.Width / 2.0 - Player[C].Location.Width / 2.0;
                        Player[C].Location.SpeedX = 0;
                        Player[C].Location.SpeedY = 0;
                        Player[C].Effect = 8;
                        Player[C].Effect2 = -A;
                    }
                }
                StopMusic();
                XEvents::doEvents();
                PlaySound(SFX_CardRouletteClear);
            }
            else if(NPC[B].Type == NPCID_GOALORB_S3)
            {
                LevelMacro = LEVELMACRO_QUESTION_SPHERE_EXIT;
                for(C = 1; C <= numPlayers; C++)
                {
                    if(A != C) // And DScreenType <> 5 Then
                    {
                        Player[C].Section = Player[A].Section;
                        Player[C].Location.Y = Player[A].Location.Y + Player[A].Location.Height - Player[C].Location.Height;
                        Player[C].Location.X = Player[A].Location.X + Player[A].Location.Width / 2.0 - Player[C].Location.Width / 2.0;
                        Player[C].Location.SpeedX = 0;
                        Player[C].Location.SpeedY = 0;
                        Player[C].Effect = 8;
                        Player[C].Effect2 = -A;
                    }
                }
                StopMusic();
                PlaySound(SFX_DungeonClear);
            }
            else if(NPC[B].Type == NPCID_GOALORB_S2)
            {
                LevelMacro = LEVELMACRO_CRYSTAL_BALL_EXIT;
                for(C = 1; C <= numPlayers; C++)
                {
                    if(A != C) // And DScreenType <> 5 Then
                    {
                        Player[C].Section = Player[A].Section;
                        Player[C].Location.Y = Player[A].Location.Y + Player[A].Location.Height - Player[C].Location.Height;
                        Player[C].Location.X = Player[A].Location.X + Player[A].Location.Width / 2.0 - Player[C].Location.Width / 2.0;
                        Player[C].Location.SpeedX = 0;
                        Player[C].Location.SpeedY = 0;
                        Player[C].Effect = 8;
                        Player[C].Effect2 = -A;
                    }
                }
                StopMusic();
                PlaySound(SFX_CrystalBallExit);
            }
            else if(NPC[B].Type == NPCID_STAR_EXIT || NPC[B].Type == NPCID_STAR_COLLECT)
            {
                for(const auto& star : Star)
                {
                    bool bySection = NPC[B].Variant == 0 && (star.Section == NPC[B].Section || star.Section == -1);
                    bool byId = NPC[B].Variant > 0 && -(star.Section + 100) == int(NPC[B].Variant);
                    if(star.level == FileNameFull && (bySection || byId))
                        tempBool = true;
                }

                if(!tempBool)
                {
                    Star_t star;
                    star.level = FileNameFull;
                    // Positive - section number, Negative - UID of each star per level
                    int special = (int)NPC[B].Variant;
                    star.Section = special <= 0 ? NPC[B].Section : -special - 100;
                    if(special > 0)
                        pLogDebug("Got a star with UID=%d", special);
                    Star.push_back(std::move(star));
                    numStars = (int)Star.size();
#ifdef THEXTECH_INTERPROC_SUPPORTED
                    IntProc::sendStarsNumber(numStars);
#endif
                    CheckAfterStarTake(false);
                }

                if(NPC[B].Type == NPCID_STAR_EXIT)
                {
                    LevelMacro = LEVELMACRO_STAR_EXIT;
                    for(C = 1; C <= numPlayers; C++)
                    {
                        if(A != C) // And DScreenType <> 5 Then
                        {
                            Player[C].Section = Player[A].Section;
                            Player[C].Location.Y = Player[A].Location.Y + Player[A].Location.Height - Player[C].Location.Height;
                            Player[C].Location.X = Player[A].Location.X + Player[A].Location.Width / 2.0 - Player[C].Location.Width / 2.0;
                            Player[C].Location.SpeedX = 0;
                            Player[C].Location.SpeedY = 0;
                            Player[C].Effect = 8;
                            Player[C].Effect2 = -A;
                        }
                    }
                    StopMusic();
                    PlaySound(SFX_GotStar);
                }
                else
                    PlaySound(SFX_MedalGet);
            }
        }
        if(toadBool > 0)
            NPC[B].Type = toadBool;
        NPC[B].Killed = 9;
        NPCQueues::Killed.push_back(B);
        //        if(nPlay.Online == true && A == nPlay.MySlot + 1)
        //            Netplay::sendData Netplay::PutPlayerControls(nPlay.MySlot) + "1c" + std::to_string(A) + "|" + Player[A].Effect + "|" + Player[A].Effect2 + "1h" + std::to_string(A) + "|" + Player[A].State + LB;
    }
}
