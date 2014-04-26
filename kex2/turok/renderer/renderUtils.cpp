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
//
// DESCRIPTION: Drawing utilities
//
//-----------------------------------------------------------------------------

#include "common.h"
#include "renderBackend.h"
#include "renderMain.h"
#include "material.h"
#include "renderUtils.h"

float kexRenderUtils::debugLineNum = 0;

//
// kexRenderUtils::DrawBoundingBox
//

void kexRenderUtils::DrawBoundingBox(const kexBBox &bbox, const byte r, const byte g, const byte b) {
    renderBackend.SetState(GLSTATE_TEXTURE0, false);
    renderBackend.SetState(GLSTATE_CULL, false);
    renderBackend.SetState(GLSTATE_BLEND, true);

    renderBackend.DisableShaders();

#define ADD_LINE(ba1, ba2, ba3, bb1, bb2, bb3)                      \
    renderer.AddLine(bbox[ba1][0], bbox[ba2][1], bbox[ba3][2],      \
                         bbox[bb1][0], bbox[bb2][1], bbox[bb3][2],  \
                         r, g, b, 255)
    
    renderer.BindDrawPointers();
    ADD_LINE(0, 0, 0, 1, 0, 0); // 0 1
    ADD_LINE(1, 0, 1, 0, 0, 1); // 2 3
    ADD_LINE(0, 0, 0, 0, 1, 0); // 0 4
    ADD_LINE(0, 1, 1, 0, 0, 1); // 5 3
    ADD_LINE(0, 0, 1, 0, 0, 0); // 3 0
    ADD_LINE(0, 1, 0, 1, 1, 0); // 4 6
    ADD_LINE(1, 1, 1, 0, 1, 1); // 7 5
    ADD_LINE(0, 1, 1, 0, 1, 0); // 5 4
    ADD_LINE(1, 0, 0, 1, 1, 0); // 1 6
    ADD_LINE(1, 1, 0, 1, 1, 1); // 6 7
    ADD_LINE(1, 1, 1, 1, 0, 1); // 7 2
    ADD_LINE(1, 0, 1, 1, 0, 0); // 2 1
    renderer.DrawLineElements();
    
#undef ADD_LINE

    renderBackend.SetState(GLSTATE_TEXTURE0, true);
}

//
// kexRenderUtils::DrawFilledBoundingBox
//

void kexRenderUtils::DrawFilledBoundingBox(const kexBBox &bbox,
                                           const byte r, const byte g, const byte b) {
    
    word indices[36];
    float points[24];
    byte colors[96];
    
    renderBackend.SetState(GLSTATE_TEXTURE0, false);
    renderBackend.SetState(GLSTATE_CULL, false);
    renderBackend.SetState(GLSTATE_BLEND, false);
    
    indices[ 0] = 0; indices[ 1] = 1; indices[ 2] = 3;
    indices[ 3] = 4; indices[ 4] = 7; indices[ 5] = 5;
    indices[ 6] = 0; indices[ 7] = 4; indices[ 8] = 1;
    indices[ 9] = 1; indices[10] = 5; indices[11] = 6;
    indices[12] = 2; indices[13] = 6; indices[14] = 7;
    indices[15] = 4; indices[16] = 0; indices[17] = 3;
    indices[18] = 1; indices[19] = 2; indices[20] = 3;
    indices[21] = 7; indices[22] = 6; indices[23] = 5;
    indices[24] = 2; indices[25] = 1; indices[26] = 6;
    indices[27] = 3; indices[28] = 2; indices[29] = 7;
    indices[30] = 7; indices[31] = 4; indices[32] = 3;
    indices[33] = 4; indices[34] = 5; indices[35] = 1;
    
    bbox.ToPoints(points);
    
    for(int i = 0; i < 96 / 4; i++) {
        colors[i * 4 + 0] = r;
        colors[i * 4 + 1] = g;
        colors[i * 4 + 2] = b;
        colors[i * 4 + 3] = 255;
    }
    
    dglDisableClientState(GL_NORMAL_ARRAY);
    dglDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
    dglVertexPointer(3, GL_FLOAT, sizeof(float)*3, points);
    dglColorPointer(4, GL_UNSIGNED_BYTE, sizeof(byte)*4, colors);
    
    renderBackend.DisableShaders();
    dglDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, indices);
    
    dglEnableClientState(GL_NORMAL_ARRAY);
    dglEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    renderBackend.SetState(GLSTATE_TEXTURE0, true);
}

//
// kexRenderUtils::DrawRadius
//

void kexRenderUtils::DrawRadius(float x, float y, float z,
                             float radius, float height,
                             const byte r, const byte g, const byte b) {
    float an;
    int i;

    renderBackend.SetState(GLSTATE_TEXTURE0, false);
    renderBackend.SetState(GLSTATE_CULL, false);
    renderBackend.SetState(GLSTATE_BLEND, true);

    renderBackend.DisableShaders();
    renderer.BindDrawPointers();

    an = DEG2RAD(360 / 32);

    for(i = 0; i < 32; i++) {
        float s1 = kexMath::Sin(an * i);
        float c1 = kexMath::Cos(an * i);
        float s2 = kexMath::Sin(an * ((i+1)%31));
        float c2 = kexMath::Cos(an * ((i+1)%31));
        float x1 = x + (radius * s1);
        float x2 = x + (radius * s2);
        float y1 = y;
        float y2 = y + height;
        float z1 = z + (radius * c1);
        float z2 = z + (radius * c2);
        
        renderer.AddLine(x1, y1, z1, x1, y2, z1, r, g, b, 255);
        renderer.AddLine(x1, y1, z1, x2, y1, z2, r, g, b, 255);
        renderer.AddLine(x1, y2, z1, x2, y2, z2, r, g, b, 255);
    }

    renderer.DrawLineElements();
    renderBackend.SetState(GLSTATE_TEXTURE0, true);
}

//
// kexRenderUtils::DrawOrigin
//

void kexRenderUtils::DrawOrigin(float x, float y, float z, float size) {
    renderBackend.SetState(GLSTATE_TEXTURE0, false);

    dglDepthRange(0.0f, 0.0f);
    dglLineWidth(2.0f);

    renderBackend.DisableShaders();
    
    renderer.BindDrawPointers();
    renderer.AddLine(x, y, z, x + size, y, z, 255, 0, 0, 255); // x
    renderer.AddLine(x, y, z, x, y + size, z, 0, 255, 0, 255); // y
    renderer.AddLine(x, y, z, x, y, z + size, 0, 0, 255, 255); // z
    renderer.DrawLineElements();
    
    renderBackend.SetState(GLSTATE_TEXTURE0, true);
    
    dglLineWidth(1.0f);
    dglDepthRange(0.0f, 1.0f);
}

//
// kexRenderUtils::DrawSphere
//

void kexRenderUtils::DrawSphere(float x, float y, float z, float radius,
                                const byte r, const byte g, const byte b) {
    float points[72];
    int count;
    int i;
    int j;
    int k;
    float s;
    float c;
    float v1[3];
    float v2[3];

    renderBackend.SetState(GLSTATE_TEXTURE0, false);
    renderBackend.SetState(GLSTATE_CULL, false);

    renderBackend.DisableShaders();
    renderer.BindDrawPointers();

    count = (360 / 15);
    points[0 * 3 + 0] = x;
    points[0 * 3 + 1] = y;
    points[0 * 3 + 2] = z + radius;

    for(i = 1; i < count; i++) {
        points[i * 3 + 0] = points[0 * 3 + 0];
        points[i * 3 + 1] = points[0 * 3 + 1];
        points[i * 3 + 2] = points[0 * 3 + 2];
    }

    for(i = 15; i <= 360; i += 15) {
        s = kexMath::Sin(DEG2RAD(i));
        c = kexMath::Cos(DEG2RAD(i));

        v1[0] = x;
        v1[1] = y + radius * s;
        v1[2] = z + radius * c;

        for(k = 0, j = 15; j <= 360; j += 15, k++) {
            v2[0] = x + kexMath::Sin(DEG2RAD(j)) * radius * s;
            v2[1] = y + kexMath::Cos(DEG2RAD(j)) * radius * s;
            v2[2] = v1[2];

            renderer.AddLine(v1[0], v1[1], v2[2], v2[0], v2[1], v2[2], r, g, b, 255);
            renderer.AddLine(v1[0], v1[1], v2[2],
                points[k * 3 + 0],
                points[k * 3 + 1],
                points[k * 3 + 2],
                r, g, b, 255);

            points[k * 3 + 0] = v1[0];
            points[k * 3 + 1] = v1[1];
            points[k * 3 + 2] = v1[2];

            v1[0] = v2[0];
            v1[1] = v2[1];
            v1[2] = v2[2];
        }
    }

    renderer.DrawLineElements();
    renderBackend.SetState(GLSTATE_TEXTURE0, true);
}

//
// kexRenderUtils::DrawLine
//

void kexRenderUtils::DrawLine(const kexVec3 &p1, const kexVec3 &p2,
                              const byte r, const byte g, const byte b) {
    
    renderBackend.SetState(GLSTATE_TEXTURE0, false);
    renderBackend.SetState(GLSTATE_CULL, false);
    renderBackend.DisableShaders();
    renderer.BindDrawPointers();
    renderer.AddLine(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, r, g, b, 255);
    renderer.DrawLineElements();
    renderBackend.SetState(GLSTATE_TEXTURE0, true);
}

//
// kexRenderUtils::PrintStatsText
//

void kexRenderUtils::PrintStatsText(const char *title, const char *s, ...) {
    va_list v;
    static char vastr[1024];
    unsigned int c;
    byte *cb;
	
    cb = (byte*)&c;
    
    if(title != NULL) {
        c = RGBA(0, 255, 0, 255);
        renderBackend.consoleFont.DrawString(title, 32, debugLineNum, 1, false, cb, cb);
    }
    
    if(s != NULL) {
        va_start(v, s);
        vsprintf(vastr, s, v);
        va_end(v);
        
        c = RGBA(255, 255, 0, 255);
        renderBackend.consoleFont.DrawString(vastr, 192, debugLineNum, 1, false, cb, cb);
    }
    
    debugLineNum += 16.0f;
}

