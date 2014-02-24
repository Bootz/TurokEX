// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 2014 Samuel Villarreal
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

#ifndef _AI_H_
#define _AI_H_

#include "common.h"
#include "actor.h"

typedef enum {
    AIF_TURNING         = 1,
    AIF_DORMANT         = 2,
    AIF_SEETARGET       = 4,
    AIF_HASTARGET       = 8,
    AIF_FINDTARGET      = 16,
    AIF_AVOIDWALLS      = 32,
    AIF_AVOIDACTORS     = 64,
    AIF_DISABLED        = 128,
    AIF_LOOKATTARGET    = 256
} aiFlags_t;

typedef enum {
    AIS_NONE            = 0,
    AIS_IDLE,
    AIS_CALM,
    AIS_ALERT,
    AIS_ATTACK_MELEE,
    AIS_ATTACK_RANGE,
    AIS_DEATH,
    AIS_SPAWNING,
    AIS_TELEPORT_OUT,
    AIS_TELEPORT_IN
} aiState_t;

BEGIN_EXTENDED_CLASS(kexAI, kexActor);
public:
                                kexAI(void);
                                ~kexAI(void);

    virtual void                Tick(void);

    void                        Spawn(void);
    void                        Save(kexBinFile *saveFile);
    void                        Load(kexBinFile *loadFile);

    float                       GetTargetDistance(void);
    float                       GetYawToTarget(void);
    void                        TracePosition(traceInfo_t *trace, const kexVec3 &position,
                                              const float radius, const float yaw);
    bool                        CheckPosition(const kexVec3 &position,
                                              const float radius, const float yaw);
    float                       GetBestYawToTarget(const float extendedRadius);
    void                        FireProjectile(const char *fxName, const kexVec3 &org,
                                               const float maxAngle, bool bLocalToActor = false);
    void                        FireProjectile(const kexStr &fxName, const kexVec3 &org,
                                               const float maxAngle, bool bLocalToactor = false);
    void                        SetIdealYaw(const float yaw, const float speed);
    void                        Turn(void);
    bool                        CanSeeTarget(kexWorldObject *object);
    void                        FindTargets(void);
    void                        ClearTargets(void);
    void                        ChangeState(const aiState_t aiState);
    void                        SeekTarget(void);
    void                        TurnYaw(const float yaw);

    static void                 InitObject(void);

protected:
    float                       idealYaw;
    float                       turnSpeed;
    float                       activeDistance;
    unsigned int                aiFlags;
    aiState_t                   aiState;
    kexVec3                     goalOrigin;
    float                       thinkTime;
    float                       nextThinkTime;
    unsigned int                nodeHead;
    float                       headYaw;
    float                       headPitch;
    float                       headTurnSpeed;
    float                       maxHeadAngle;
    kexVec3                     headYawAxis;
    kexVec3                     headPitchAxis;
    bool                        bCanMelee;
    bool                        bCanRangeAttack;
    bool                        bCanTeleport;
    bool                        bAttacking;
    bool                        bTurning;
    float                       attackThreshold;
    float                       sightThreshold;
    float                       sightRange;
    float                       checkRadius;
    float                       meleeRange;
    float                       alertRange;
    float                       rangeDistance;

END_CLASS();

#endif