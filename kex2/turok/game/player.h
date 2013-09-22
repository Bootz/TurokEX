// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 2007-2012 Samuel Villarreal
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
//
//-----------------------------------------------------------------------------

#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "common.h"
#include "actor.h"

void P_RunCommand(ENetEvent *sev, ENetPacket *packet);
void P_SpawnLocalPlayer(void);
void P_LocalPlayerTick(void);

typedef struct {
    kexVec3             origin;
    kexVec3             velocity;
    kexVec3             accel;
    kexAngle            angles;
    float               moveTime;
    float               frameTime;
    float               timeStamp;
} moveState_t;

typedef struct {
    int                 ingoing;
    int                 outgoing;
    int                 acks;
} netSequence_t;

typedef struct {
    kexVec3             accelSpeed;
    kexVec3             deaccelSpeed;
    kexVec3             forwardSpeed;
    kexVec3             backwardSpeed;
} playerMove_t;

BEGIN_EXTENDED_CLASS(kexPlayerLocation, kexWorldActor);
public:
                        kexPlayerLocation(void);
                        ~kexPlayerLocation(void);

    virtual void        Parse(kexLexer *lexer);

    const int           GetID(void) const { return id; }

private:
    int                 id;
END_CLASS();

BEGIN_EXTENDED_CLASS(kexPlayer, kexWorldActor);
public:
                        kexPlayer(void);
                        ~kexPlayer(void);

    void                ResetNetSequence(void);
    void                ResetTicCommand(void);
    void                Accelerate(const playerMove_t *move, int direction, int axis);

    playerMove_t        *GroundMove(void) { return &groundMove; }
    playerMove_t        *AirMove(void) { return &airMove; }
    playerMove_t        *SwimMove(void) { return &swimMove; }
    playerMove_t        *ClimbMove(void) { return &climbMove; }
    playerMove_t        *CrawlMove(void) { return &crawlMove; }
    playerMove_t        *FlyMove(void) { return &flyMove; }
    playerMove_t        *NoClipMove(void) { return &noClipMove; }

    kexVec3             &GetAcceleration(void) { return acceleration; }
    void                SetAcceleration(const kexVec3 &accel) { acceleration = accel; }
    float               GetMaxPitch(void) { return maxPitch; }
    void                SetMaxPitch(const float p) { maxPitch = p; }
    bool                GetAllowCrawl(void) { return bAllowCrawl; }
    void                SetAllowCrawl(const bool b) { bAllowCrawl = b; }
    float               GetCrawlHeight(void) { return crawlHeight; }
    void                SetCrawlHeight(const float f) { crawlHeight = f; }
    float               GetMoveTime(void) { return moveTime; }
    void                SetMoveTime(const float t) { moveTime = t; }
    
    ticcmd_t            *Cmd(void) { return &cmd; }
    gObject_t           *GetScriptObject(void) const { return scriptObject; }
    void                SetScriptObject(gObject_t *obj) { scriptObject = obj; }
    ENetPeer            *GetPeer(void) { return peer; }
    void                SetPeer(ENetPeer *_peer) { peer = _peer; }
    int                 GetID(void) const { return id; }
    void                SetID(const int _id) { id = _id; }
    netSequence_t       *NetSeq(void) { return &netseq; }

    // TODO - REMOVE
    worldState_t        worldState;

protected:
    kexVec3             acceleration;
    playerMove_t        groundMove;
    playerMove_t        airMove;
    playerMove_t        swimMove;
    playerMove_t        climbMove;
    playerMove_t        crawlMove;
    playerMove_t        flyMove;
    playerMove_t        noClipMove;
    float               maxPitch;
    bool                bAllowCrawl;
    float               crawlHeight;
    float               moveTime;
    netSequence_t       netseq;
    ticcmd_t            cmd;
    int                 id;
    char                *name;
    ENetPeer            *peer;
    char                *jsonData;
    moveState_t         moveState;
    gObject_t           *scriptObject;
END_CLASS();

BEGIN_EXTENDED_CLASS(kexLocalPlayer, kexPlayer);
public:
                        kexLocalPlayer(void);
                        ~kexLocalPlayer(void);

    virtual void        LocalTick(void);

    bool                ProcessInput(event_t *ev);
    void                BuildCommands(void);
    int                 PlayerEvent(const char *eventName);
    void                SerializeScriptObject(void);
    void                DeSerializeScriptObject(void);

    //kexActor            *Camera(void) { return camera; }
    kexVec3             &MoveDiff(void) { return moveDiff; }

    // TODO
    gActor_t            *actor;
    gActor_t            *camera;

private:
    //kexActor            *camera;
    int                 latency[NETBACKUPS];
    kexVec3             moveDiff;
    kexVec3             oldMoves[NETBACKUPS];
    ticcmd_t            oldCmds[NETBACKUPS];
END_CLASS();

BEGIN_EXTENDED_CLASS(kexNetPlayer, kexPlayer);
public:
                        kexNetPlayer(void);
                        ~kexNetPlayer(void);

    virtual void        Tick(void);
    int                 GetState(void) const { return state; }
    void                SetState(const int s) { state = s; }

    // TODO
    gActor_t            *actor;

private:
    int                 state;
END_CLASS();

#endif
