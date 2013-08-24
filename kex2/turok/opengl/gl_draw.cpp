// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 2012 Samuel Villarreal
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
// DESCRIPTION: Common drawing functions
//
//-----------------------------------------------------------------------------

#include "common.h"
#include "gl.h"
#include "kernel.h"
#include "zone.h"

static vtx_t vtxstring[1024];

static texture_t *t_confont;
static texture_t *t_bigfont;

static byte bfont_upr_r = 0xff;
static byte bfont_upr_g = 0xff;
static byte bfont_upr_b = 0xff;
static byte bfont_lwr_r = 0xff;
static byte bfont_lwr_g = 0xff;
static byte bfont_lwr_b = 0xff;

typedef struct
{
    int x;
    int y;
    int w;
    int h;
} atlas2_t;

static const atlas2_t fontmap[256] =
{
    { 0, 1, 13, 16 },
    { 14, 1, 13, 16 },
    { 28, 1, 13, 16 },
    { 42, 1, 13, 16 },
    { 56, 1, 13, 16 },
    { 70, 1, 13, 16 },
    { 84, 1, 13, 16 },
    { 98, 1, 13, 16 },
    { 112, 1, 13, 16 },
    { 126, 1, 13, 16 },
    { 140, 1, 13, 16 },
    { 154, 1, 13, 16 },
    { 168, 1, 13, 16 },
    { 182, 1, 13, 16 },
    { 196, 1, 13, 16 },
    { 210, 1, 13, 16 },
    { 224, 1, 13, 16 },
    { 238, 1, 13, 16 },
    { 0, 18, 13, 16 },
    { 14, 18, 13, 16 },
    { 28, 18, 13, 16 },
    { 42, 18, 13, 16 },
    { 56, 18, 13, 16 },
    { 70, 18, 13, 16 },
    { 84, 18, 13, 16 },
    { 98, 18, 13, 16 },
    { 112, 18, 13, 16 },
    { 126, 18, 13, 16 },
    { 140, 18, 13, 16 },
    { 154, 18, 13, 16 },
    { 168, 18, 13, 16 },
    { 182, 18, 13, 16 },
    { 196, 18, 5, 16 },
    { 202, 18, 5, 16 },
    { 208, 18, 5, 16 },
    { 214, 18, 10, 16 },
    { 225, 18, 8, 16 },
    { 234, 18, 13, 16 },
    { 0, 35, 9, 16 },
    { 10, 35, 3, 16 },
    { 14, 35, 6, 16 },
    { 21, 35, 6, 16 },
    { 28, 35, 9, 16 },
    { 38, 35, 9, 16 },
    { 48, 35, 5, 16 },
    { 54, 35, 7, 16 },
    { 62, 35, 5, 16 },
    { 68, 35, 6, 16 },
    { 75, 35, 8, 16 },
    { 84, 35, 8, 16 },
    { 93, 35, 8, 16 },
    { 102, 35, 8, 16 },
    { 111, 35, 8, 16 },
    { 120, 35, 8, 16 },
    { 129, 35, 8, 16 },
    { 138, 35, 8, 16 },
    { 147, 35, 8, 16 },
    { 156, 35, 8, 16 },
    { 165, 35, 6, 16 },
    { 172, 35, 6, 16 },
    { 179, 35, 9, 16 },
    { 189, 35, 9, 16 },
    { 199, 35, 9, 16 },
    { 209, 35, 7, 16 },
    { 217, 35, 13, 16 },
    { 231, 35, 9, 16 },
    { 241, 35, 8, 16 },
    { 0, 52, 9, 16 },
    { 10, 52, 9, 16 },
    { 20, 52, 8, 16 },
    { 29, 52, 8, 16 },
    { 38, 52, 9, 16 },
    { 48, 52, 9, 16 },
    { 58, 52, 5, 16 },
    { 64, 52, 6, 16 },
    { 71, 52, 8, 16 },
    { 80, 52, 7, 16 },
    { 88, 52, 11, 16 },
    { 100, 52, 9, 16 },
    { 110, 52, 10, 16 },
    { 121, 52, 8, 16 },
    { 130, 52, 10, 16 },
    { 141, 52, 8, 16 },
    { 150, 52, 9, 16 },
    { 160, 52, 9, 16 },
    { 170, 52, 9, 16 },
    { 180, 52, 9, 16 },
    { 190, 52, 13, 16 },
    { 204, 52, 9, 16 },
    { 214, 52, 9, 16 },
    { 224, 52, 9, 16 },
    { 234, 52, 6, 16 },
    { 241, 52, 6, 16 },
    { 248, 52, 6, 16 },
    { 0, 69, 11, 16 },
    { 12, 69, 8, 16 },
    { 21, 69, 8, 16 },
    { 30, 69, 8, 16 },
    { 39, 69, 8, 16 },
    { 48, 69, 8, 16 },
    { 57, 69, 8, 16 },
    { 66, 69, 8, 16 },
    { 75, 69, 5, 16 },
    { 81, 69, 8, 16 },
    { 90, 69, 8, 16 },
    { 99, 69, 3, 16 },
    { 103, 69, 4, 16 },
    { 108, 69, 7, 16 },
    { 116, 69, 3, 16 },
    { 120, 69, 11, 16 },
    { 132, 69, 8, 16 },
    { 141, 69, 8, 16 },
    { 150, 69, 8, 16 },
    { 159, 69, 8, 16 },
    { 168, 69, 5, 16 },
    { 174, 69, 7, 16 },
    { 182, 69, 6, 16 },
    { 189, 69, 8, 16 },
    { 198, 69, 8, 16 },
    { 207, 69, 11, 16 },
    { 219, 69, 7, 16 },
    { 227, 69, 8, 16 },
    { 236, 69, 7, 16 },
    { 244, 69, 8, 16 },
    { 0, 86, 7, 16 },
    { 8, 86, 8, 16 },
    { 17, 86, 11, 16 },
    { 29, 86, 13, 16 },
    { 43, 86, 13, 16 },
    { 57, 86, 13, 16 },
    { 71, 86, 13, 16 },
    { 85, 86, 13, 16 },
    { 99, 86, 13, 16 },
    { 113, 86, 13, 16 },
    { 127, 86, 13, 16 },
    { 141, 86, 13, 16 },
    { 155, 86, 13, 16 },
    { 169, 86, 13, 16 },
    { 183, 86, 13, 16 },
    { 197, 86, 13, 16 },
    { 211, 86, 13, 16 },
    { 225, 86, 13, 16 },
    { 239, 86, 13, 16 },
    { 0, 103, 13, 16 },
    { 14, 103, 13, 16 },
    { 28, 103, 13, 16 },
    { 42, 103, 13, 16 },
    { 56, 103, 13, 16 },
    { 70, 103, 13, 16 },
    { 84, 103, 13, 16 },
    { 98, 103, 13, 16 },
    { 112, 103, 13, 16 },
    { 126, 103, 13, 16 },
    { 140, 103, 13, 16 },
    { 154, 103, 13, 16 },
    { 168, 103, 13, 16 },
    { 182, 103, 13, 16 },
    { 196, 103, 13, 16 },
    { 210, 103, 13, 16 },
    { 224, 103, 13, 16 },
    { 238, 103, 5, 16 },
    { 244, 103, 5, 16 },
    { 0, 120, 8, 16 },
    { 9, 120, 8, 16 },
    { 18, 120, 8, 16 },
    { 27, 120, 8, 16 },
    { 36, 120, 7, 16 },
    { 44, 120, 8, 16 },
    { 53, 120, 8, 16 },
    { 62, 120, 13, 16 },
    { 76, 120, 7, 16 },
    { 84, 120, 8, 16 },
    { 93, 120, 9, 16 },
    { 103, 120, 7, 16 },
    { 111, 120, 13, 16 },
    { 125, 120, 8, 16 },
    { 134, 120, 7, 16 },
    { 142, 120, 9, 16 },
    { 152, 120, 7, 16 },
    { 160, 120, 7, 16 },
    { 168, 120, 8, 16 },
    { 177, 120, 8, 16 },
    { 186, 120, 8, 16 },
    { 195, 120, 5, 16 },
    { 201, 120, 8, 16 },
    { 210, 120, 7, 16 },
    { 218, 120, 7, 16 },
    { 226, 120, 8, 16 },
    { 235, 120, 13, 16 },
    { 0, 137, 13, 16 },
    { 14, 137, 13, 16 },
    { 28, 137, 7, 16 },
    { 36, 137, 9, 16 },
    { 46, 137, 9, 16 },
    { 56, 137, 9, 16 },
    { 66, 137, 9, 16 },
    { 76, 137, 9, 16 },
    { 86, 137, 9, 16 },
    { 96, 137, 12, 16 },
    { 109, 137, 9, 16 },
    { 119, 137, 8, 16 },
    { 128, 137, 8, 16 },
    { 137, 137, 8, 16 },
    { 146, 137, 8, 16 },
    { 155, 137, 5, 16 },
    { 161, 137, 5, 16 },
    { 167, 137, 5, 16 },
    { 173, 137, 5, 16 },
    { 179, 137, 9, 16 },
    { 189, 137, 9, 16 },
    { 199, 137, 10, 16 },
    { 210, 137, 10, 16 },
    { 221, 137, 10, 16 },
    { 232, 137, 10, 16 },
    { 243, 137, 10, 16 },
    { 0, 154, 11, 16 },
    { 12, 154, 10, 16 },
    { 23, 154, 9, 16 },
    { 33, 154, 9, 16 },
    { 43, 154, 9, 16 },
    { 53, 154, 9, 16 },
    { 63, 154, 9, 16 },
    { 73, 154, 8, 16 },
    { 82, 154, 8, 16 },
    { 91, 154, 8, 16 },
    { 100, 154, 8, 16 },
    { 109, 154, 8, 16 },
    { 118, 154, 8, 16 },
    { 127, 154, 8, 16 },
    { 136, 154, 8, 16 },
    { 145, 154, 11, 16 },
    { 157, 154, 8, 16 },
    { 166, 154, 8, 16 },
    { 175, 154, 8, 16 },
    { 184, 154, 8, 16 },
    { 193, 154, 8, 16 },
    { 202, 154, 3, 16 },
    { 206, 154, 3, 16 },
    { 210, 154, 3, 16 },
    { 214, 154, 3, 16 },
    { 218, 154, 8, 16 },
    { 227, 154, 8, 16 },
    { 236, 154, 8, 16 },
    { 245, 154, 8, 16 },
    { 0, 171, 8, 16 },
    { 9, 171, 8, 16 },
    { 18, 171, 8, 16 },
    { 27, 171, 9, 16 },
    { 37, 171, 8, 16 },
    { 46, 171, 8, 16 },
    { 55, 171, 8, 16 },
    { 64, 171, 8, 16 },
    { 73, 171, 8, 16 },
    { 82, 171, 8, 16 },
    { 91, 171, 8, 16 },
    { 100, 171, 8, 16 }
};

//
// Draw_Text
//

float Draw_Text(float x, float y, rcolor color,
                float scale, const char* string, ...)
{
    int c = 0;
    size_t i = 0;
    int vi = 0;
    float vx1 = 0.0f;
    float vy1 = 0.0f;
    float vx2 = 0.0f;
    float vy2 = 0.0f;
    float tx1 = 0.0f;
    float tx2 = 0.0f;
    float ty1 = 0.0f;
    float ty2 = 0.0f;
    char msg[1024];
    va_list	va;
    float width;
    float height;

    if(t_confont == NULL)
    {
        return 0;
    }

    va_start(va, string);
    vsprintf(msg, string, va);
    va_end(va);
    
    GL_BindTexture(t_confont);

    width = (float)t_confont->width;
    height = (float)t_confont->height;

    dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    dglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    GL_SetVertexPointer(vtxstring);
    GL_SetState(GLSTATE_BLEND, 1);

    for(i = 0, vi = 0; i < strlen(msg); i++, vi += 4)
    {
        vx1 = x;
        vy1 = y;
        
        c = msg[i];
        if(c == '\n' || c == '\t')
            continue;   // villsa: safety check
        else
        {
            vx2 = vx1 + ((float)fontmap[c].w * scale);
            vy2 = vy1 - ((float)fontmap[c].h * scale);
            
            tx1 = ((float)fontmap[c].x / width) + 0.001f;
            tx2 = (tx1 + (float)fontmap[c].w / width) - 0.002f;
            
            ty1 = ((float)fontmap[c].y / height);
            ty2 = ty1 + (((float)fontmap[c].h / height));

            vtxstring[vi + 0].x     = vx1;
            vtxstring[vi + 0].y     = vy1;
            vtxstring[vi + 0].tu    = tx1;
            vtxstring[vi + 0].tv    = ty2;
            vtxstring[vi + 1].x     = vx2;
            vtxstring[vi + 1].y     = vy1;
            vtxstring[vi + 1].tu    = tx2;
            vtxstring[vi + 1].tv    = ty2;
            vtxstring[vi + 2].x     = vx2;
            vtxstring[vi + 2].y     = vy2;
            vtxstring[vi + 2].tu    = tx2;
            vtxstring[vi + 2].tv    = ty1;
            vtxstring[vi + 3].x     = vx1;
            vtxstring[vi + 3].y     = vy2;
            vtxstring[vi + 3].tu    = tx1;
            vtxstring[vi + 3].tv    = ty1;

            dglSetVertexColor(vtxstring + vi, color, 4);

            GL_Triangle(vi + 2, vi + 1, vi + 0);
            GL_Triangle(vi + 3, vi + 2, vi + 0);
            
            x += ((float)fontmap[c].w * scale);
        }
    }

    if(vi)
    {
        GL_DrawElements(vi, vtxstring);
    }
    
    GL_SetState(GLSTATE_BLEND, 0);

    return x;
}

static const atlas2_t bigfontmap[48] =
{
    { 0, 0, 24, 32 },       // 0
    { 32, 0, 24, 32 },      // 1
    { 64, 0, 24, 32 },      // 2
    { 96, 0, 24, 32 },      // 3
    { 128, 0, 24, 32 },     // 4
    { 160, 0, 24, 32 },     // 5
    { 192, 0, 24, 32 },     // 6
    { 224, 0, 24, 32 },     // 7
    { 0, 36, 24, 32 },      // 8
    { 32, 36, 24, 32 },     // 9
    { 64, 36, 24, 32 },     // A
    { 96, 36, 24, 32 },     // B
    { 128, 36, 24, 32 },    // C
    { 160, 36, 24, 32 },    // ,
    { 192, 36, 24, 32 },    // (c)
    { 224, 36, 24, 32 },    // D
    { 0, 72, 24, 32 },      // E
    { 32, 72, 24, 32 },     // F
    { 64, 72, 24, 32 },     // G
    { 96, 72, 24, 32 },     //
    { 128, 72, 24, 32 },    //
    { 160, 72, 24, 32 },    //
    { 192, 72, 24, 32 },    // H
    { 224, 72, 24, 32 },    // I
    { 0, 108, 24, 32 },      // J
    { 32, 108, 24, 32 },     // K
    { 64, 108, 24, 32 },     // L
    { 96, 108, 24, 32 },     // '
    { 128, 108, 24, 32 },    // M
    { 160, 108, 24, 32 },    // N
    { 192, 108, 24, 32 },    // O
    { 224, 108, 24, 32 },    // P
    { 0, 144, 24, 32 },     // .
    { 32, 144, 24, 32 },    // Q
    { 64, 144, 24, 32 },    // "
    { 96, 144, 24, 32 },    // "
    { 128, 144, 24, 32 },   // R
    { 160, 144, 24, 32 },   //
    { 192, 144, 24, 32 },   // S
    { 224, 144, 24, 32 },   // T
    { 0, 180, 24, 32 },     // (r)
    { 32, 180, 24, 32 },    // U
    { 64, 180, 24, 32 },    // V
    { 96, 180, 24, 32 },    // W
    { 128, 180, 24, 32 },   // X
    { 160, 180, 24, 32 },   // Y
    { 192, 180, 24, 32 },   // Z
    { 224, 180, 24, 32 }   // space
    
};

//
// Draw_SetBigTextColor
//

void Draw_SetBigTextColor(byte r1, byte g1, byte b1, byte r2, byte g2, byte b2)
{
    bfont_upr_r = r2;
    bfont_upr_g = g2;
    bfont_upr_b = b2;
    bfont_lwr_r = r1;
    bfont_lwr_g = g1;
    bfont_lwr_b = b1;
}

//
// Draw_BigText
//

float Draw_BigText(float x, float y, byte alpha, kbool centered,
                   float scale, const char* string)
{
    int c = 0;
    size_t i = 0;
    int vi = 0;
    unsigned int len;
    float vx1 = 0.0f;
    float vy1 = 0.0f;
    float vx2 = 0.0f;
    float vy2 = 0.0f;
    float tx1 = 0.0f;
    float tx2 = 0.0f;
    float ty1 = 0.0f;
    float ty2 = 0.0f;
    float width;
    float height;
    float ratiox;
    float ratioy;

    if(t_bigfont == NULL)
    {
        return 0;
    }

    ratiox = (float)FIXED_WIDTH / video_width;
    ratioy = (float)FIXED_HEIGHT / video_height;

    len = strlen(string);

    if(centered)
    {
        x = (x - (((float)(24 * len) * scale) / 2));
    }

    x /= ratiox;
    y /= ratioy;
    
    GL_BindTexture(t_bigfont);

    width = (float)t_bigfont->width;
    height = (float)t_bigfont->height;

    GL_SetVertexPointer(vtxstring);
    GL_SetState(GLSTATE_BLEND, 1);

    for(i = 0, vi = 0; i < len; i++, vi += 8)
    {
        vx1 = x;
        vy1 = y;
        
        c = string[i];
        if(c == '\n' || c == '\t')
            continue;   // villsa: safety check
        else
        {
            int index = 0;

            if(c >= '0' && c <= '9')
            {
                index = (c - '0');
            }
            else
            {
                switch(tolower(c))
                {
                case 'a':
                    index = 10;
                    break;
                case 'b':
                    index = 11;
                    break;
                case 'c':
                    index = 12;
                    break;
                case ',':
                    index = 13;
                    break;
                case 'd':
                    index = 15;
                    break;
                case 'e':
                    index = 16;
                    break;
                case 'f':
                    index = 17;
                    break;
                case 'g':
                    index = 18;
                    break;
                case 'h':
                    index = 22;
                    break;
                case 'i':
                    index = 23;
                    break;
                case 'j':
                    index = 24;
                    break;
                case 'k':
                    index = 25;
                    break;
                case 'l':
                    index = 26;
                    break;
                case 'm':
                    index = 28;
                    break;
                case 'n':
                    index = 29;
                    break;
                case 'o':
                    index = 30;
                    break;
                case 'p':
                    index = 31;
                    break;
                case '.':
                    index = 32;
                    break;
                case 'q':
                    index = 33;
                    break;
                case 'r':
                    index = 36;
                    break;
                case 's':
                    index = 38;
                    break;
                case 't':
                    index = 39;
                    break;
                case 'u':
                    index = 41;
                    break;
                case 'v':
                    index = 42;
                    break;
                case 'w':
                    index = 43;
                    break;
                case 'x':
                    index = 44;
                    break;
                case 'y':
                    index = 45;
                    break;
                case 'z':
                    index = 46;
                    break;
                case ' ':
                    index = 47;
                    break;
                }
            }

            vx2 = vx1 + ((float)bigfontmap[index].w / ratiox * scale);
            vy2 = vy1 - ((float)bigfontmap[index].h / ratioy * scale);
            
            tx1 = ((float)bigfontmap[index].x / width) + 0.001f;
            tx2 = (tx1 + (float)bigfontmap[index].w / width) - 0.002f;
            
            ty1 = ((float)bigfontmap[index].y / height);
            ty2 = ty1 + (((float)bigfontmap[index].h / height));

            vtxstring[vi + 0].x     = vx1;
            vtxstring[vi + 0].y     = vy1;
            vtxstring[vi + 0].tu    = tx1;
            vtxstring[vi + 0].tv    = ty2;
            vtxstring[vi + 0].r     = bfont_upr_r;
            vtxstring[vi + 0].g     = bfont_upr_g;
            vtxstring[vi + 0].b     = bfont_upr_b;
            vtxstring[vi + 0].a     = alpha;
            vtxstring[vi + 1].x     = vx2;
            vtxstring[vi + 1].y     = vy1;
            vtxstring[vi + 1].tu    = tx2;
            vtxstring[vi + 1].tv    = ty2;
            vtxstring[vi + 1].r     = bfont_upr_r;
            vtxstring[vi + 1].g     = bfont_upr_g;
            vtxstring[vi + 1].b     = bfont_upr_b;
            vtxstring[vi + 1].a     = alpha;
            vtxstring[vi + 2].x     = vx2;
            vtxstring[vi + 2].y     = vy2;
            vtxstring[vi + 2].tu    = tx2;
            vtxstring[vi + 2].tv    = ty1;
            vtxstring[vi + 2].r     = bfont_lwr_r;
            vtxstring[vi + 2].g     = bfont_lwr_g;
            vtxstring[vi + 2].b     = bfont_lwr_b;
            vtxstring[vi + 2].a     = alpha;
            vtxstring[vi + 3].x     = vx1;
            vtxstring[vi + 3].y     = vy2;
            vtxstring[vi + 3].tu    = tx1;
            vtxstring[vi + 3].tv    = ty1;
            vtxstring[vi + 3].r     = bfont_lwr_r;
            vtxstring[vi + 3].g     = bfont_lwr_g;
            vtxstring[vi + 3].b     = bfont_lwr_b;
            vtxstring[vi + 3].a     = alpha;

            GL_Triangle(vi + 2, vi + 1, vi + 0);
            GL_Triangle(vi + 3, vi + 2, vi + 0);
            
            x += ((float)bigfontmap[index].w / ratiox * scale);
        }
    }

    if(vi)
    {
        GL_DrawElements(vi, vtxstring);
    }
    
    GL_SetState(GLSTATE_BLEND, 0);

    return x;
}

//
// Draw_ShadowedText
//

void Draw_ShadowedText(float x, float y, byte alpha, kbool centered,
                       float scale, const char* string)
{
    byte rgb[6];

    rgb[0] = bfont_upr_r;
    rgb[1] = bfont_upr_g;
    rgb[2] = bfont_upr_b;
    rgb[3] = bfont_lwr_r;
    rgb[4] = bfont_lwr_g;
    rgb[5] = bfont_lwr_b;

    bfont_upr_r >>= 2;
    bfont_upr_g >>= 2;
    bfont_upr_b >>= 2;
    bfont_lwr_r >>= 2;
    bfont_lwr_g >>= 2;
    bfont_lwr_b >>= 2;

    Draw_BigText(x + (6*scale), y + (6*scale), alpha, centered, scale, string);
    Draw_SetBigTextColor(rgb[3], rgb[4], rgb[5], rgb[0], rgb[1], rgb[2]);
    Draw_BigText(x, y, alpha, centered, scale, string);
}

//
// Draw_Pic
//

void Draw_Pic(const char *pic, float x, float y, byte alpha, float scale)
{
    float ratiox;
    float ratioy;
    float rx;
    float ry;
    float rw;
    float rh;
    texture_t *tex;
    vtx_t vtx[4];

    tex = Tex_CacheTextureFile(pic, DGL_CLAMP, true);

    if(tex == NULL)
    {
        return;
    }

    ratiox = (float)FIXED_WIDTH / video_width;
    ratioy = (float)FIXED_HEIGHT / video_height;
    rx = x / ratiox;
    rw = (float)tex->width / ratiox;
    ry = y / ratioy;
    rh = (float)tex->height / ratioy;

    dglEnable(GL_ALPHA_TEST);
    GL_BindTexture(tex);
    GL_SetVertexPointer(vtx);
    GL_SetState(GLSTATE_BLEND, 1);
    dglAlphaFunc(GL_GEQUAL, 0.8f * (alpha / 255.0f));

    vtx[0].x = vtx[2].x = x;
    vtx[1].x = vtx[3].x = x + rw;
    vtx[0].y = vtx[1].y = y;
    vtx[2].y = vtx[3].y = y + rh;
    vtx[0].z = vtx[1].z = 0;
    vtx[2].z = vtx[3].z = 0;
    vtx[0].tu = vtx[2].tu = 0;
    vtx[1].tu = vtx[3].tu = 1;
    vtx[0].tv = vtx[1].tv = 0;
    vtx[2].tv = vtx[3].tv = 1;

    *(rcolor*)&vtx[0].r = COLOR_WHITE_A(alpha);
    *(rcolor*)&vtx[1].r = COLOR_WHITE_A(alpha);
    *(rcolor*)&vtx[2].r = COLOR_WHITE_A(alpha);
    *(rcolor*)&vtx[3].r = COLOR_WHITE_A(alpha);

    GL_Triangle(0, 1, 2);
    GL_Triangle(2, 1, 3);
    GL_DrawElements(4, vtx);
    GL_SetState(GLSTATE_BLEND, 0);
    dglAlphaFunc(GL_GEQUAL, 0.01f);
    dglDisable(GL_ALPHA_TEST);
}

//
// Draw_Tile
// Simplier version of Draw_Pic
//

void Draw_Tile(const char *pic, float x, float y,
               float tx1, float ty1, float tx2, float ty2,
               float width, float height,
               byte r, byte g, byte b, byte alpha)
{
    texture_t *tex;
    rcolor color;
    vtx_t vtx[4];

    tex = Tex_CacheTextureFile(pic, DGL_CLAMP, true);

    if(tex == NULL)
        return;

    GL_BindTexture(tex);
    GL_SetVertexPointer(vtx);

    vtx[0].x = vtx[2].x = x;
    vtx[1].x = vtx[3].x = x + width;
    vtx[0].y = vtx[1].y = y;
    vtx[2].y = vtx[3].y = y + height;
    vtx[0].z = vtx[1].z = 0;
    vtx[2].z = vtx[3].z = 0;
    vtx[0].tu = vtx[2].tu = tx1;
    vtx[1].tu = vtx[3].tu = tx2;
    vtx[0].tv = vtx[1].tv = ty1;
    vtx[2].tv = vtx[3].tv = ty2;

    color = RGBA(r, g, b, alpha);

    *(rcolor*)&vtx[0].r = color;
    *(rcolor*)&vtx[1].r = color;
    *(rcolor*)&vtx[2].r = color;
    *(rcolor*)&vtx[3].r = color;

    GL_Triangle(0, 1, 2);
    GL_Triangle(2, 1, 3);
    GL_DrawElements(4, vtx);
}

//
// SetupFontTexture
//

static void SetupFontTexture(void)
{
    t_confont = Tex_CacheTextureFile("fonts/confont.tga", DGL_CLAMP, true);
    t_bigfont = Tex_CacheTextureFile("fonts/bigfont.tga", DGL_CLAMP, true);
}

//
// Draw_Init
//

void Draw_Init(void)
{
    SetupFontTexture();
}