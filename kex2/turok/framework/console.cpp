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
// DESCRIPTION: Console System
//
//-----------------------------------------------------------------------------

#include "common.h"
#include "system.h"
#include "array.h"
#include "kstring.h"
#include "client.h"
#include "keyInput.h"
#include "console.h"
#include "renderBackend.h"

kexCvar cvarDisplayConsole("con_alwaysShowConsole", CVF_BOOL|CVF_CONFIG, "0", "TODO");
kexCvar cvarShowFPS("con_showfps", CVF_BOOL|CVF_CONFIG, "0", "Displays current FPS");
kexCvar cvarStickyKeySpeed("con_stickySpeed", CVF_BOOL|CVF_CONFIG, "500", "TODO");

kexConsole console;

//
// clear
//

COMMAND(clear) {
    console.Clear();
}

//
// kexConsole::kexConsole
//

kexConsole::kexConsole(void) {
    this->scrollBackPos     = 0;
    this->historyTop        = 0;
    this->historyCur        = -1;
    this->typeStrPos        = 0;
    this->scrollBackLines   = 0;
    this->bShiftDown        = false;
    this->bCtrlDown         = false;
    this->state             = CON_STATE_UP;
    this->blinkTime         = 0;
    this->bKeyHeld          = false;
    this->bStickyActive     = false;
    this->lastKeyPressed    = 0;
    this->timePressed       = 0;
    this->bShowPrompt       = true;
    this->outputLength      = 0;

    ClearOutput();
}

//
// kexConsole::~kexConsole
//

kexConsole::~kexConsole(void) {
    ClearOutput();
}

//
// kexConsole::ClearOutput
//

void kexConsole::ClearOutput(void) {
    for(int i = 0; i < CON_BUFFER_SIZE; i++) {
        memset(this->scrollBackStr[i], 0, CON_LINE_LENGTH);
        lineColor[i] = COLOR_WHITE;
    }

    scrollBackLines = 0;
}

//
// kexConsole::Clear
//

void kexConsole::Clear(void) {
    ClearOutput();
    ResetInputText();
}

//
// kexConsole::OutputTextLine
//

void kexConsole::OutputTextLine(rcolor color, const char *text) {
    if(scrollBackLines >= CON_BUFFER_SIZE) {
        for(unsigned int i = 0; i < CON_BUFFER_SIZE-1; i++) {
            memset(scrollBackStr[i], 0, CON_LINE_LENGTH);
            strcpy(scrollBackStr[i], scrollBackStr[i+1]);
            lineColor[i] = lineColor[i+1];
        }

        scrollBackLines = CON_BUFFER_SIZE-1;
    }

    unsigned int len = strlen(text);
    if(len >= CON_LINE_LENGTH) {
        len = CON_LINE_LENGTH-1;
    }

    strncpy(scrollBackStr[scrollBackLines], text, len);
    scrollBackStr[scrollBackLines][len] = '\0';
    lineColor[scrollBackLines] = color;

    scrollBackLines++;
}

//
// kexConsole::AddToHistory
//

void kexConsole::AddToHistory(void) {
    strcpy(history[historyTop], typeStr);
    historyTop = (historyTop+1) % CON_MAX_HISTORY;
}

//
// kexConsole::GetHistory
//

void kexConsole::GetHistory(bool bPrev) {
    const char *hist;
    
    if(bPrev) {
        historyCur--;
        if(historyCur < 0) {
            historyCur = historyTop-1;
        }
    }
    else {
        historyCur++;
        if(historyCur >= historyTop) {
            historyCur = 0;
        }
    }
    
    ResetInputText();
    hist = history[historyCur];
    
    strcpy(typeStr, hist);
    typeStrPos = strlen(hist);
}

//
// kexConsole::Print
//

void kexConsole::Print(rcolor color, const char *text) {
    int strLength = strlen(text);
    char *curText = (char*)text;
    char tmpChar[CON_LINE_LENGTH];

    while(strLength > 0) {
        int lineLength = kexStr::IndexOf(curText, "\n");

        if(lineLength == -1) {
            lineLength = strLength;
        }

        strncpy(tmpChar, curText, lineLength);
        tmpChar[lineLength] = '\0';
        OutputTextLine(color, tmpChar);

        curText = (char*)&text[lineLength+1];
        strLength -= (lineLength+1);
    }
}

//
// kexConsole::LineScroll
//

void kexConsole::LineScroll(bool dir) {
    if(dir) {
        if(scrollBackPos < scrollBackLines) {
            scrollBackPos++;
        }
    }
    else {
        if(scrollBackPos > 0) {
            scrollBackPos--;
        }
    }
}

//
// kexConsole::BackSpace
//

void kexConsole::BackSpace(void) {
    if(strlen(typeStr) <= 0) {
        return;
    }

    char *trim = typeStr;
    int len = strlen(trim);

    typeStr[typeStrPos-1] = '\0';
    if(typeStrPos < len) {
        strncat(typeStr, &trim[typeStrPos], len-typeStrPos);
    }

    typeStrPos--;

    if(typeStrPos < 0) {
        typeStrPos = 0;
    }
}

//
// kexConsole::DeleteChar
//

void kexConsole::DeleteChar(void) {
    int tsLen = strlen(typeStr);

    if(tsLen > 0 && typeStrPos < tsLen) {
        char *trim = typeStr;
        int len = strlen(trim);

        typeStr[typeStrPos] = '\0';
        strncat(typeStr, &trim[typeStrPos+1], len-typeStrPos);
    }
}

//
// kexConsole::ShiftHeld
//

bool kexConsole::ShiftHeld(int c) const {
    return (c == KKEY_RSHIFT || c == KKEY_LSHIFT);
}

//
// kexConsole::MoveTypePos
//

void kexConsole::MoveTypePos(bool dir) {
    if(dir) {
        int len = strlen(typeStr);
        typeStrPos++;
        if(typeStrPos > len) {
            typeStrPos = len;
        }
    }
    else {
        typeStrPos--;
        if(typeStrPos < 0) {
            typeStrPos = 0;
        }
    }
}

//
// kexConsole::CheckShift
//

void kexConsole::CheckShift(const event_t *ev) {

    if(!ShiftHeld(ev->data1))
        return;

    switch(ev->type) {
        case ev_keydown:
            bShiftDown = true;
            break;
        case ev_keyup:
            bShiftDown = false;
            break;
        default:
            break;
    }
}

//
// kexConsole::CheckStickyKeys
//

void kexConsole::CheckStickyKeys(const event_t *ev) {
    if(ShiftHeld(ev->data1) || ev->data1 == KKEY_RETURN ||
        ev->data1 == KKEY_TAB) {
            return;
    }

    lastKeyPressed = ev->data1;

    switch(ev->type) {
        case ev_keydown:
            if(!bKeyHeld) {
                bKeyHeld = true;
                timePressed = sysMain.GetMS();
            }
            break;
        case ev_keyup:
            bKeyHeld = false;
            timePressed = 0;
            bStickyActive = false;
            break;
        default:
            break;
    }
}

//
// kexConsole::ParseKey
//

void kexConsole::ParseKey(int c) {
    switch(c) {
        case KKEY_BACKSPACE:
            BackSpace();
            return;
        case KKEY_DELETE:
            DeleteChar();
            return;
        case KKEY_LEFT:
            MoveTypePos(0);
            return;
        case KKEY_RIGHT:
            MoveTypePos(1);
            return;
        case KKEY_PAGEUP:
            LineScroll(1);
            return;
        case KKEY_PAGEDOWN:
            LineScroll(0);
            return;
    }

    if(c >= KKEY_SPACE && c < KKEY_z) {
        if(typeStrPos >= CON_INPUT_LENGTH) {
            return;
        }

        typeStr[typeStrPos++] = inputKey.GetAsciiKey((char)c, bShiftDown);
        typeStr[typeStrPos] = '\0';
    }
}

//
// kexConsole::StickyKeyTick
//

void kexConsole::StickyKeyTick(void) {
    if(!bStickyActive) {
        int stickyTime = cvarStickyKeySpeed.GetInt();
        if(stickyTime < 0) {
            stickyTime = 0;
        }
        
        if(bKeyHeld && ((sysMain.GetMS() - timePressed) >= stickyTime)) {
            bStickyActive = true;
        }
    }
    else {
        ParseKey(lastKeyPressed);
    }
}

//
// kexConsole::UpdateBlink
//

void kexConsole::UpdateBlink(void) {
    if(blinkTime >= client.GetTime()) {
        return;
    }

    bShowPrompt = !bShowPrompt;
    blinkTime = client.GetTime() + CON_BLINK_TIME;
}

//
// kexConsole::ParseInput
//

void kexConsole::ParseInput(void) {
    if(typeStrPos <= 0 || strlen(typeStr) <= 0)
        return;

    OutputTextLine(RGBA(192, 192, 192, 255), typeStr);
    command.Execute(typeStr);
    AddToHistory();
    ResetInputText();

    historyCur = (historyTop - 1);
}

//
// kexConsole::ProcessInput
//

bool kexConsole::ProcessInput(const event_t *ev) {
    if(ev->type == ev_mousedown || ev->type == ev_mouseup ||
        ev->type == ev_mouse) {
            return false;
    }

    if(ev->type == ev_mousewheel && state == CON_STATE_DOWN) {
        switch(ev->data1) {
            case KM_BUTTON_SCROLL_UP:
                LineScroll(1);
                break;
            case KM_BUTTON_SCROLL_DOWN:
                LineScroll(0);
                break;
        }

        return true;
    }

    CheckShift(ev);
    CheckStickyKeys(ev);

    int c = ev->data1;

    switch(state) {
        case CON_STATE_DOWN:
            if(ev->type == ev_keydown) {
                switch(c) {
                    case KKEY_BACKQUOTE:
                        state = CON_STATE_UP;
                        inputSystem->MouseCenter();
                        return true;
                    case KKEY_RETURN:
                        ParseInput();
                        return true;
                    case KKEY_UP:
                        GetHistory(false);
                        return true;
                    case KKEY_DOWN:
                        GetHistory(true);
                        return true;
                    case KKEY_TAB:
                        cvarManager.AutoComplete(typeStr);
                        command.AutoComplete(typeStr);
                        return true;
                    default:
                        ParseKey(c);
                        return true;
                }

                return false;
            }
            break;
        case CON_STATE_UP:
            if(ev->type == ev_keydown) {
                switch(c) {
                    case KKEY_BACKQUOTE:
                        state = CON_STATE_DOWN;
                        return true;
                    default:
                        break;
                }

                return false;
            }
            break;
        default:
            return false;
    }

    return false;
}

//
// kexConsole::Tick
//

void kexConsole::Tick(void) {
    if(state == CON_STATE_UP) {
        return;
    }

    StickyKeyTick();
    UpdateBlink();
}

//
// kexConsole::Draw
//

void kexConsole::Draw(void) {
    bool    bOverlay;
    float   w;
    float   h;
    rcolor  color;

    w = (float)sysMain.VideoWidth();

    if(cvarShowFPS.GetBool()) {
        color = RGBA(255, 255, 255, 255);
        renderBackend.consoleFont.DrawString(kva("fps: %i", client.fps), w - 64, 32, 1,
            false, (byte*)&color, (byte*)&color);
    }

    if(state == CON_STATE_UP && !cvarDisplayConsole.GetBool()) {
        return;
    }

    bOverlay = (state == CON_STATE_UP && cvarDisplayConsole.GetBool());

    h = (float)sysMain.VideoHeight() * 0.6875f;
    renderBackend.SetState(GLSTATE_BLEND, true);

    if(!bOverlay) {
        cpuVertList.BindDrawPointers();
        renderBackend.whiteTexture.Bind();

        // draw tint overlay
        cpuVertList.AddVertex(0, 0, 0, 0, 0, 4, 8, 16, 192);
        cpuVertList.AddVertex(w, 0, 0, 0, 0, 4, 8, 16, 192);
        cpuVertList.AddVertex(0, h, 0, 0, 0, 4, 8, 16, 192);
        cpuVertList.AddVertex(w, h, 0, 0, 0, 4, 8, 16, 192);
        cpuVertList.AddTriangle(0, 1, 2);
        cpuVertList.AddTriangle(2, 1, 3);
        cpuVertList.DrawElementsNoShader();

        // draw borders
        cpuVertList.AddVertex(0, h-17, 0, 0, 0, 0, 128, 255, 255);
        cpuVertList.AddVertex(w, h-17, 0, 0, 0, 0, 128, 255, 255);
        cpuVertList.AddVertex(0, h-16, 0, 0, 0, 0, 128, 255, 255);
        cpuVertList.AddVertex(w, h-16, 0, 0, 0, 0, 128, 255, 255);
        cpuVertList.AddVertex(0, h, 0, 0, 0, 0, 128, 255, 255);
        cpuVertList.AddVertex(w, h, 0, 0, 0, 0, 128, 255, 255);
        cpuVertList.AddVertex(0, h+1, 0, 0, 0, 0, 128, 255, 255);
        cpuVertList.AddVertex(w, h+1, 0, 0, 0, 0, 128, 255, 255);
        cpuVertList.AddTriangle(0, 1, 2);
        cpuVertList.AddTriangle(2, 1, 3);
        cpuVertList.AddTriangle(4, 5, 6);
        cpuVertList.AddTriangle(6, 5, 7);
        cpuVertList.DrawElementsNoShader();

        color = RGBA(255, 255, 255, 255);
        renderBackend.consoleFont.DrawString("> ", 0, h-15, 1, false, (byte*)&color, (byte*)&color);

        if(bShowPrompt) {
            renderBackend.consoleFont.DrawString("_", 16 +
                renderBackend.consoleFont.StringWidth(typeStr, 1.0f, typeStrPos),
                h-15, 1, false, (byte*)&color, (byte*)&color);
        }

        if(strlen(typeStr) > 0) {
            renderBackend.consoleFont.DrawString(typeStr, 16, h-15, 1, false,
                (byte*)&color, (byte*)&color);
        }
    }

    if(scrollBackLines > 0) {
        float scy = h-34;

        for(int i = scrollBackLines-(scrollBackPos)-1; i >= 0; i--) {
            if(scy < 0) {
                break;
            }

            color = lineColor[i];
            renderBackend.consoleFont.DrawString(scrollBackStr[i], 0, scy,
                1, false, (byte*)&color, (byte*)&color);
            scy -= 16;
        }
    }

    renderBackend.SetState(GLSTATE_BLEND, false);
}
