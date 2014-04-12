// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 2013 Samuel Villarreal
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

#ifndef __RENDER_WORLD_H__
#define __RENDER_WORLD_H__

#include "world.h"
#include "sdNodes.h"

class kexRenderWorld {
public:
                                kexRenderWorld(void);

    void                        RenderScene(void);
    void                        BuildNodes(void);
    void                        DrawAreaNode(void);
    void                        DrawRenderNode(void);

    static void                 Init(void);
    static int                  SortSprites(const void *a, const void *b);

    bool                        bShowBBox;
    bool                        bShowGrid;
    bool                        bShowNormals;
    bool                        bShowRadius;
    bool                        bShowNodes;
    bool                        bShowOrigin;
    bool                        bWireframe;
    bool                        bShowClipMesh;
    bool                        bShowCollisionMap;
    bool                        bShowRenderNodes;

    int                         showAreaNode;
    kexSDNode<kexWorldModel>    renderNodes;

    kexVec3                     &WorldLightTransform(void) { return worldLightTransform; }

private:
    void                        DrawActors(void);
    void                        DrawStaticActors(void);
    void                        DrawViewActors(void);
    void                        DrawFX(void);
    void                        DrawWorldModel(kexWorldModel *wm);
    void                        RecursiveSDNode(int nodenum);
    void                        TraverseDrawActorNode(kexActor *actor,
                                                      const modelNode_t *node,
                                                      kexAnimState *animState);
    void                        DrawSectors(kexSector *sectors, const int count);
    void                        DrawTriangle(const kexTri &tri, const word index,
                                             byte r, byte g, byte b, byte a);

    kexWorld                    *world;
    kexVec3                     worldLightTransform;
};

extern kexRenderWorld renderWorld;

#endif
