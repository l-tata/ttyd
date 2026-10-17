#include "win/win_main.h"
#include "battle/battle_monosiri.h"
#include "data/item_data.h"
#include "data/mail_data.h"
#include "driver/camdrv.h"
#include "driver/mapdrv.h"
#include "driver/msgdrv.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/gx.h>

extern int sprintf(char* str, const char* format, ...);
extern int strcmp(const char* a, const char* b);
extern char* strncpy(char* dst, const char* src, unsigned long n);
extern ItemData itemDataTable[];
void qqsort(void* base, u32 n, u32 size, s32 (*compare)(const void*, const void*));
void unk_800d06b4(const char* msg, char* out);

typedef struct WinSortEnt {
    u8 kind; // 0x0
    u8 id;   // 0x1
} WinSortEnt;

typedef struct WinBadgeEnt {
    s32 equipped; // 0x0
    s32 idx;      // 0x4
    s32 id;       // 0x8
} WinBadgeEnt;

typedef struct WinTexRef {
    TEXPalette* tpl;
} WinTexRef;

typedef struct WinRes {
    u8 pad_0[0xA0];
    WinTexRef* tex; // 0xA0
} WinRes;

typedef struct WinTab {
    s32 id;       // 0x0
    f32 x;        // 0x4
    f32 y;        // 0x8
    u8 pad_C[0xC]; // 0xC
} WinTab;

typedef struct WinMenu {
    f32 x;      // 0x0
    f32 y;      // 0x4
    s32 state;  // 0x8
    s32 timer;  // 0xC
    s32 type;   // 0x10
} WinMenu;

typedef struct WinPauseWork {
    /* 0x000 */ u16 flags;
    /* 0x002 */ u8 pad_2[0x4 - 0x2];
    /* 0x004 */ u32 buttonTrg;
    /* 0x008 */ u32 buttonRep;
    /* 0x00C */ u32 dirTrg;
    /* 0x010 */ u32 dirRep;
    /* 0x014 */ u8 pad_14[0x24 - 0x14];
    /* 0x024 */ s32 active;
    /* 0x028 */ WinRes* res;
    /* 0x02C */ u8 pad_2C[0x40 - 0x2C];
    /* 0x040 */ s32 tabSel;
    /* 0x044 */ s32 tabNum;
    /* 0x048 */ WinTab tab[5];
    /* 0x0C0 */ u8 pad_C0[0xC4 - 0xC0];
    /* 0x0C4 */ WinMenu menu[5];
    /* 0x128 */ f32 msgOfsX;
    /* 0x12C */ f32 msgOfsY;
    /* 0x130 */ s32 msgState;
    /* 0x134 */ s32 msgTimer;
    /* 0x138 */ s32 msgX;
    /* 0x13C */ const char* msg;
    /* 0x140 */ s32 msgY;
    /* 0x144 */ const char* lastMsg;
    /* 0x148 */ s32 msgLine;
    /* 0x14C */ s32 msgLines;
    /* 0x150 */ f32 helpX;
    /* 0x154 */ f32 helpY;
    /* 0x158 */ f32 sortMsgX;
    /* 0x15C */ f32 sortMsgY;
    /* 0x160 */ u8 pad_160[0x164 - 0x160];
    /* 0x164 */ f32 sortPosX;
    /* 0x168 */ f32 sortPosY;
    /* 0x16C */ f32 sortTgtX;
    /* 0x170 */ f32 sortTgtY;
    /* 0x174 */ s32 sortSel;
    /* 0x178 */ s32 sortNum;
    /* 0x17C */ s32 sortActive;
    /* 0x180 */ s32 sortRev;
    /* 0x184 */ s32 sortType;
    /* 0x188 */ u8 pad_188[0x214 - 0x188];
    /* 0x214 */ s32 itemSel[2];
    /* 0x21C */ s32 itemPage[2];
    /* 0x224 */ u8 pad_224[0x254 - 0x224];
    /* 0x254 */ s32 mailNum;
    /* 0x258 */ s32 mailSel;
    /* 0x25C */ s32 mailPage;
    /* 0x260 */ u8 pad_260[0x26E - 0x260];
    /* 0x26E */ WinSortEnt mailList[0x80];
    /* 0x36E */ u8 pad_36E[0x3E4 - 0x36E];
    /* 0x3E4 */ s32 badgeSel[2];
    /* 0x3EC */ s32 badgePage[2];
    /* 0x3F4 */ u8 pad_3F4[0x404 - 0x3F4];
    /* 0x404 */ WinBadgeEnt badgeList[200];
    /* 0xD64 */ s32 badgeNum;
    /* 0xD68 */ u8 pad_D68[0xE94 - 0xD68];
    /* 0xE94 */ s32 recipeNum;
    /* 0xE98 */ s32 recipeSel;
    /* 0xE9C */ s32 recipePage;
    /* 0xEA0 */ u8 pad_EA0[0xEB8 - 0xEA0];
    /* 0xEB8 */ s16 recipeList[0x6C];
    /* 0xF90 */ s32 badgeLogNum;
    /* 0xF94 */ s32 badgeLogSel;
    /* 0xF98 */ s32 badgeLogPage;
    /* 0xF9C */ u8 pad_F9C[0xFB4 - 0xF9C];
    /* 0xFB4 */ s16 badgeLogList[0x42];
    /* 0x1038 */ s32 tattleNum;
    /* 0x103C */ u8 pad_103C[0x1040 - 0x103C];
    /* 0x1040 */ s32 tattleSel;
    /* 0x1044 */ s32 tattlePage;
    /* 0x1048 */ u8 pad_1048[0x1050 - 0x1048];
    /* 0x1050 */ WinSortEnt tattleList[0xE0];
} WinPauseWork;

typedef struct WinSortData {
    const char* name;
    void (*func)(WinPauseWork* wp);
} WinSortData;

void winMakeSkipList(WinPauseWork* wp);
void winTexInit_x2(TEXPalette* tpl);
void winTexSet_x2(u32 a, u32 b, Vec translate, Vec scale, GXColor color);
void winMarioDisp(s32 camId, WinPauseWork* wp, s32 n);
void winPartyDisp(s32 camId, WinPauseWork* wp, s32 n);
void winItemDisp(s32 camId, WinPauseWork* wp, s32 n);
void winBadgeDisp(s32 camId, WinPauseWork* wp, s32 n);
void winLogDisp(s32 camId, WinPauseWork* wp, s32 n);
void winMailDisp(WinPauseWork* wp);
void winMsgDisp(WinPauseWork* wp, f32 x, f32 y);
void winSortGX(WinPauseWork* wp);
void winMakeEquipList(WinPauseWork* wp);

void sort_1_1_func(WinPauseWork* wp);
void sort_1_2_func(WinPauseWork* wp);
void sort_2_1_func(WinPauseWork* wp);
void sort_2_2_func(WinPauseWork* wp);
void sort_3_1_func(WinPauseWork* wp);
void sort_3_2_func(WinPauseWork* wp);
void sort_3_3_func(WinPauseWork* wp);
void sort_4_1_func(WinPauseWork* wp);
void sort_4_2_func(WinPauseWork* wp);
void sort_4_3_func(WinPauseWork* wp);
void sort_5_1_func(WinPauseWork* wp);
void sort_5_2_func(WinPauseWork* wp);
void sort_5_3_func(WinPauseWork* wp);
void sort_6_1_func(WinPauseWork* wp);
void sort_6_2_func(WinPauseWork* wp);
void sort_7_1_func(WinPauseWork* wp);
void sort_7_2_func(WinPauseWork* wp);
void sort_7_3_func(WinPauseWork* wp);
void sort_8_1_func(WinPauseWork* wp);
void sort_8_2_func(WinPauseWork* wp);

u8 enemy_monoshiri_sort_table[] = {
    0x00, 0x01, 0x02, 0x03, 0x2F, 0x30, 0x94, 0x2A, 0x62, 0x95, 0x96, 0x12, 0x24, 0x24, 0x0A, 0x0B,
    0x32, 0x14, 0x4D, 0x54, 0x33, 0x32, 0x15, 0x63, 0x43, 0x38, 0x4F, 0x3E, 0x6B, 0x6E, 0x6F, 0x79,
    0x7A, 0x7B, 0x75, 0x99, 0x01, 0x0C, 0x0D, 0x36, 0x1B, 0x1D, 0x4E, 0x56, 0x2C, 0x2D, 0x23, 0x0E,
    0x0F, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27, 0x2B, 0x18, 0x19, 0x1A, 0x5A, 0x13, 0x50, 0x51, 0x29,
    0x66, 0x67, 0x04, 0x05, 0x06, 0x3C, 0x3D, 0x52, 0x1F, 0x20, 0x40, 0x47, 0x48, 0x68, 0x9A, 0x9B,
    0x9C, 0x9D, 0x9E, 0x9F, 0x4A, 0x4B, 0x34, 0x35, 0x39, 0x21, 0x5B, 0x5C, 0x57, 0x69, 0xA0, 0xA1,
    0xA2, 0xA3, 0xA4, 0x77, 0x6B, 0x6B, 0x6B, 0x44, 0x37, 0x22, 0x49, 0x6A, 0xA5, 0xA6, 0xA7, 0xA8,
    0x45, 0x3A, 0x53, 0x70, 0x71, 0x72, 0x73, 0x6C, 0x6D, 0x76, 0xA9, 0x41, 0x4C, 0x5D, 0x5E, 0x59,
    0x5F, 0x5F, 0x16, 0x17, 0x64, 0x79, 0x7A, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68,
    0x29, 0x28, 0x78, 0x74, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x07, 0x08, 0x09, 0x10, 0x11, 0x2E, 0x1C,
    0x1E, 0x60, 0x3B, 0x31, 0x3F, 0x55, 0x46, 0x42, 0x58, 0x61, 0x61, 0x65, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0x00,
};

#include "driver/dispdrv.h"
#include "driver/swdrv.h"
#include "mario/mario_party.h"
#include "memory.h"

void winMarioInit();
void winPartyInit();
void winItemInit();
void winBadgeInit();
void winLogInit();
void winMarioInit2();
void winPartyInit2();
void winItemInit2();
void winBadgeInit2();
void winLogInit2();
s32 winMarioMain();
s32 winPartyMain();
s32 winItemMain();
s32 winBadgeMain();
s32 winLogMain();
void winMarioMain2();
void winPartyMain2();
void winItemMain2();
void winBadgeMain2();
void winLogMain2();
void winMarioExit();
void winPartyExit();
void winItemExit();
void winBadgeExit();
void winLogExit();
void unk_8014d338();
void unk_8014d5dc();
double sin(double);
double fabs(double);

typedef struct WinRootTab {
    s32 id;          // 0x0
    f32 x;           // 0x4
    f32 y;           // 0x8
    s32 state;       // 0xC
    s32 timer;       // 0x10
    const char* msg; // 0x14
} WinRootTab;

typedef struct WinRootMenu {
    s32 id;    // 0x0
    f32 x;     // 0x4
    f32 y;     // 0x8
    s32 state; // 0xC
    s32 timer; // 0x10
} WinRootMenu;

typedef struct WinRootWork {
    /* 0x000 */ u16 flags;
    /* 0x002 */ u8 pad_2[0x4 - 0x2];
    /* 0x004 */ u32 buttonTrg;
    /* 0x008 */ u32 buttonRep;
    /* 0x00C */ u32 dirTrg;
    /* 0x010 */ u32 dirRep;
    /* 0x014 */ u8 pad_14[0x1C - 0x14];
    /* 0x01C */ s32 initTab;
    /* 0x020 */ u8 pad_20[0x24 - 0x20];
    /* 0x024 */ s32 active;
    /* 0x028 */ u8 pad_28[0x34 - 0x28];
    /* 0x034 */ smartEntry* texBuf;
    /* 0x038 */ s32 texBufValid;
    /* 0x03C */ u8 pad_3C[0x40 - 0x3C];
    /* 0x040 */ s32 tabSel;
    /* 0x044 */ s32 tabNum;
    /* 0x048 */ WinRootTab tab[5];
    /* 0x0C0 */ WinRootMenu menu[5];
    /* 0x124 */ u8 pad_124[0x128 - 0x124];
    /* 0x128 */ f32 msgOfsX;
    /* 0x12C */ f32 msgOfsY;
    /* 0x130 */ s32 msgState;
    /* 0x134 */ s32 msgTimer;
    /* 0x138 */ s32 msgX;
    /* 0x13C */ const char* msg;
    /* 0x140 */ s32 msgY;
    /* 0x144 */ const char* lastMsg;
    /* 0x148 */ s32 msgLine;
    /* 0x14C */ s32 msgLines;
    /* 0x150 */ f32 helpX;
    /* 0x154 */ f32 helpY;
    /* 0x158 */ f32 helpTgtX;
    /* 0x15C */ f32 helpTgtY;
    /* 0x160 */ u8 pad_160[0x164 - 0x160];
    /* 0x164 */ f32 sortPosX;
    /* 0x168 */ f32 sortPosY;
    /* 0x16C */ f32 sortTgtX;
    /* 0x170 */ f32 sortTgtY;
    /* 0x174 */ u8 pad_174[0x19C - 0x174];
    /* 0x19C */ s32 unk_19c;
    /* 0x1A0 */ u8 pad_1A0[0x1DC - 0x1A0];
    /* 0x1DC */ s32 unk_1dc;
    /* 0x1E0 */ s32 unk_1e0;
    /* 0x1E4 */ u8 pad_1E4[0x1FC - 0x1E4];
    /* 0x1FC */ s32 unk_1fc;
    /* 0x200 */ u8 pad_200[0x2D4 - 0x200];
    /* 0x2D4 */ s32 unk_2d4;
} WinRootWork;

#define WIN_ROOT_ADD_TAB(n)                       \
    {                                             \
        WinRootTab* t = &w->tab[w->tabNum];       \
        t->id = n;                                \
        t->state = 0;                             \
        t->x = t->y = 1000.0f;                    \
        t->msg = msg_help[n];                     \
        w->tabNum++;                              \
    }

void winMsgEntry(WinPauseWork* wp, s32 x, const char* msg, s32 y);
void winBgMain(WinPauseWork* wp);
void winMsgMain(WinPauseWork* wp);
void winSortMain(WinPauseWork* wp);

s32 winRootMain(WinRootWork* w) {
    static char* msg_help[] = {
        "msg_menu_mario", "msg_menu_nakama", "msg_menu_mochimono", "msg_menu_badge", "msg_menu_kiroku",
    };
    s32 x;
    s32 i;
    s32 ret;
    WinRootWork* t;

    switch (w->active) {
    case 0:
        psndSFXOn((const char*)0x2002A);
        w->tabNum = 0;
        WIN_ROOT_ADD_TAB(0);
        if (swByteGet(0) >= 7 || partyChkJoin(1)) {
            WIN_ROOT_ADD_TAB(1);
        }
        WIN_ROOT_ADD_TAB(2);
        if (swByteGet(0) >= 20 || pouchGetHaveBadgeCnt()) {
            WIN_ROOT_ADD_TAB(3);
        }
        WIN_ROOT_ADD_TAB(4);
        for (i = 0; i < w->tabNum; i++) {
            if (w->tab[i].id == w->initTab) {
                w->tabSel = i;
            }
        }
        for (i = 0; i < 5; i++) {
            w->menu[i].state = -1;
            w->menu[i].x = w->menu[i].y = 1000.0f;
            w->menu[i].id = -1;
        }
        w->menu[0].x = w->menu[0].y = 1000.0f;
        w->menu[0].state = 0;
        w->menu[0].id = w->tab[w->tabSel].id;
        w->msgState = 0;
        w->msgOfsX = w->msgOfsY = 1000.0f;
        w->msg = NULL;
        w->msgY = 0;
        w->lastMsg = NULL;
        winMarioInit(w);
        winPartyInit(w);
        winItemInit(w);
        winBadgeInit(w);
        winLogInit(w);
        w->helpX = w->helpTgtX = w->tabSel * 105 - 270;
        w->helpY = 300.0f;
        w->helpTgtY = 185.0f;
        w->sortPosX = w->sortTgtX = -1000.0f;
        w->sortPosY = w->sortTgtY = -1000.0f;
        winMsgEntry((WinPauseWork*)w, 0, w->tab[w->tabSel].msg, 0);
        w->active = 10;
        switch (w->initTab) {
        case 1:
            winPartyInit2(w);
            w->active = 12;
            w->flags |= 0x4000;
            break;
        case 2:
            winItemInit2(w);
            w->active = 13;
            w->flags |= 0x4000;
            break;
        case 3:
            winBadgeInit2(w);
            w->active = 14;
            w->flags |= 0x4000;
            break;
        case 4:
            winLogInit2(w);
            w->active = 15;
            w->flags |= 0x4000;
            break;
        }
        if (gp->flags & 0x1000) {
            w->tabNum = 0;
            WIN_ROOT_ADD_TAB(1);
            winPartyInit2(w);
            w->active = 12;
            w->tabSel = 0;
        }
        w->texBuf = smartAlloc(GXGetTexBufferSize(gp->fbWidth, gp->efbHeight, GX_TF_RGB565, GX_FALSE, 0), 1);
        w->texBufValid = 0;
        dispEntry(CAMERA_2D, 1000.0f, 0, (DispCallback)unk_8014d5dc, w);
        break;
    case 10:
        if (w->buttonTrg & 0x1200) {
            if (w->menu[0].state == 100) {
                w->active = 20;
            }
        } else if (w->buttonTrg & 0x100) {
            if (w->tab[w->tabSel].id == 1 && !w->unk_1e0) {
                break;
            }
            psndSFXOn((const char*)0x20012);
            w->active = w->tab[w->tabSel].id + 11;
            switch (w->tab[w->tabSel].id) {
            case 0:
                winMarioInit2(w);
                break;
            case 1:
                winPartyInit2(w);
                break;
            case 2:
                winItemInit2(w);
                break;
            case 3:
                winBadgeInit2(w);
                break;
            case 4:
                winLogInit2(w);
                break;
            }
        } else if (w->dirRep & 0x4000) {
            psndSFXOn((const char*)0x20032);
            psndSFXOn((const char*)0x20031);
            w->tabSel--;
            if (w->tabSel < 0) {
                w->tabSel = w->tabNum - 1;
            }
            for (i = 4; i > 0; i--) {
                w->menu[i] = w->menu[i - 1];
            }
            w->menu[0].x = w->menu[0].y = 1000.0f;
            w->menu[0].state = 30;
            w->menu[0].id = w->tab[w->tabSel].id;
            w->helpTgtX = w->tabSel * 105 - 270;
            winMsgEntry((WinPauseWork*)w, 0, w->tab[w->tabSel].msg, 0);
        } else if (w->dirRep & 0x8000) {
            psndSFXOn((const char*)0x20032);
            psndSFXOn((const char*)0x20031);
            w->tabSel++;
            if (w->tabSel >= w->tabNum) {
                w->tabSel = 0;
            }
            for (i = 4; i > 0; i--) {
                w->menu[i] = w->menu[i - 1];
            }
            w->menu[0].x = w->menu[0].y = 1000.0f;
            w->menu[0].state = 20;
            w->menu[0].id = w->tab[w->tabSel].id;
            w->helpTgtX = w->tabSel * 105 - 270;
            winMsgEntry((WinPauseWork*)w, 0, w->tab[w->tabSel].msg, 0);
        }
        break;
    case 100:
        if (w->menu[w->tabSel].state == 100) {
            w->active = 10;
        }
        break;
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        switch (w->active) {
        case 11:
            ret = winMarioMain(w);
            break;
        case 12:
            ret = winPartyMain(w);
            break;
        case 13:
            ret = winItemMain(w);
            break;
        case 14:
            ret = winBadgeMain(w);
            break;
        case 15:
            ret = winLogMain(w);
            break;
        }
        if ((gp->flags & 0x1000) && ret == -1) {
            ret = -2;
        }
        if (ret == -1) {
            w->helpTgtX = w->tabSel * 105 - 270;
            w->helpTgtY = 185.0f;
            w->active = 10;
            winMsgEntry((WinPauseWork*)w, 0, w->tab[w->tabSel].msg, 0);
            w->flags &= ~0x4000;
        } else if (ret == -2) {
            w->active = 20;
            w->flags &= ~0x4000;
        }
        break;
    case 20:
        psndSFXOn((const char*)0x2002B);
        for (i = 0; i < w->tabNum; i++) {
            w->tab[i].state = 10;
        }
        for (i = 0; i < 5; i++) {
            w->menu[i].state = 10;
        }
        w->msgState = 10;
        N_mapDispOn();
        camDispOn(CAMERA_3D);
        w->active++;
        break;
    default:
        if (w->texBuf) {
            smartFree(w->texBuf);
            w->texBuf = NULL;
            w->texBufValid = 0;
        }
        w->active++;
        if (w->active > 50) {
            winMarioExit(w);
            winPartyExit(w);
            winItemExit(w);
            winBadgeExit(w);
            winLogExit(w);
            if (w->unk_19c && w->unk_1dc != w->unk_1fc) {
                return -2;
            }
            if (w->unk_2d4) {
                return -3;
            }
            return -1;
        }
        break;
    }

    for (x = -205, t = w, i = 0; i < 5; t = (WinRootWork*)((u8*)t + sizeof(WinRootTab)), x += 105, i++) {
        switch (t->tab[0].state) {
        case 0:
            t->tab[0].x = x;
            t->tab[0].y = 300.0f;
            t->tab[0].timer = i << 2;
            t->tab[0].state++;
            break;
        case 1:
        case 11:
            if (--t->tab[0].timer < 0) {
                t->tab[0].timer = 0;
                t->tab[0].state++;
            }
            break;
        case 2:
            t->tab[0].y = intplGetValue(11, t->tab[0].timer++, 10, 300.0f, 185.0f);
            if (t->tab[0].timer > 10) {
                t->tab[0].state++;
            }
            break;
        case 10:
            t->tab[0].timer = (4 - i) * 4;
            t->tab[0].state++;
            break;
        case 12:
            t->tab[0].y = intplGetValue(11, t->tab[0].timer++, 10, 180.0f, 300.0f);
            if (t->tab[0].timer > 10) {
                t->tab[0].state++;
            }
            break;
        case 13:
            break;
        }
        }

    winBgMain((WinPauseWork*)w);
    winMsgMain((WinPauseWork*)w);
    winMarioMain2(w);
    winPartyMain2(w);
    winItemMain2(w);
    winBadgeMain2(w);
    winLogMain2(w);
    winSortMain((WinPauseWork*)w);

    if (w->active < 20) {
        if (w->flags & 0x4000) {
            w->helpX += (w->helpTgtX - w->helpX) / 4.0f;
            w->helpY += (w->helpTgtY - w->helpY) / 4.0f;
        } else {
            w->helpX += (w->helpTgtX - w->helpX) / 4.0f;
            w->helpY += ((w->helpTgtY + (w->tab[w->tabSel].y - 185.0f)) - w->helpY) / 4.0f;
        }
    } else if (w->helpY > 180.0f) {
        w->helpX += (w->helpTgtX - w->helpX) / 4.0f;
        w->helpY += ((w->helpTgtY + (w->tab[w->tabSel].y - 185.0f)) - w->helpY) / 4.0f;
    } else {
        w->helpX += ((w->helpTgtX + w->menu[0].x) - w->helpX) / 4.0f;
        w->helpY += (w->helpTgtY - w->helpY) / 4.0f;
    }
    w->helpX += __fabs(2.0f * (f32)sin(gp->retraceCount / 8.0f));
    w->sortPosX += (w->sortTgtX - w->sortPosX) / 4.0f;
    w->sortPosY += (w->sortTgtY - w->sortPosY) / 4.0f;
    dispEntry(CAMERA_2D, 1000.0f, 0, (DispCallback)unk_8014d338, w);
    return 0;
}

void winRootStrippedPool(f32* f) {
    f[0] = 185.0f;
    f[1] = 1000.0f;
}

static inline void winTabDisp(WinPauseWork* wp);

void winRootDisp(s32 camId, WinPauseWork* wp) {
    GXColor white = {0xFF, 0xFF, 0xFF, 0xFF};
    s32 i;
    s32 state;

    if (wp->active) {
        for (i = 4; i >= 0; i--) {
            state = wp->menu[i].state;
            if (state == -1) {
                continue;
            }
            if (i > 0 && wp->menu[i - 1].state == 100) {
                continue;
            }
            if (i > 0 && (u32)(state - 10) <= 1) {
                continue;
            }
            switch (wp->menu[i - 1].type) {
                case 0:
                    winMarioDisp(camId, wp, i);
                    break;
                case 1:
                    winPartyDisp(camId, wp, i);
                    break;
                case 2:
                    winItemDisp(camId, wp, i);
                    break;
                case 3:
                    winBadgeDisp(camId, wp, i);
                    break;
                case 4:
                    winLogDisp(camId, wp, i);
                    break;
            }
        }
        winTabDisp(wp);
        winMailDisp(wp);
        winMsgDisp(wp, wp->msgOfsX, wp->msgOfsY);
        winSortGX(wp);
        if (!(wp->flags & 0x8000)) {
            winTexInit(wp->res->tex->tpl);
            winTexSet(0, (Vec){2.0f + wp->helpX, wp->helpY - 2.0f, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, (GXColor){0x00, 0x00, 0x00, 0x80});
            winTexSet(0, (Vec){wp->helpX, wp->helpY, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, white);
        }
    }
}

static inline void winTabDisp(WinPauseWork* wp) {
    s32 i;

    for (i = 0; i < wp->tabNum; i++) {
        GXColor c;
        if (wp->tabSel == i) {
            c = (GXColor){0xFF, 0xFF, 0xFF, 0xFE};
        } else {
            c = (GXColor){0x80, 0x80, 0x80, 0xFE};
        }
        winTexInit_x2(wp->res->tex->tpl);
        winTexSet_x2(wp->tab[i].id + 1, 0xB2, (Vec){wp->tab[i].x, wp->tab[i].y, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, c);
    }
}

void winBgMain(WinPauseWork* wp) {

    s32 i;

    for (i = 0; i < 5; i++) {
        switch (wp->menu[i].state) {
            case 0:
                wp->menu[i].x = 800.0f;
                wp->menu[i].y = 0.0f;
                wp->menu[i].timer = 0;
                wp->menu[i].state++;
                break;
            case 1:
                wp->menu[i].x = intplGetValue(11, wp->menu[i].timer++, 16, 800.0f, 0.0f);
                if (wp->menu[i].timer > 16) {
                    wp->menu[i].state = 100;
                }
                break;
            case 10:
                wp->menu[i].y = 0.0f;
                wp->menu[i].timer = 0;
                wp->menu[i].state++;
                break;
            case 11:
                wp->menu[i].x = intplGetValue(11, wp->menu[i].timer++, 16, 0.0f, 800.0f);
                if (wp->menu[i].timer > 16) {
                    wp->menu[i].state = -1;
                }
                break;
            case 20:
                wp->menu[i].y = 0.0f;
                wp->menu[i].timer = 0;
                wp->menu[i].state++;
                break;
            case 21:
                wp->menu[i].x = intplGetValue(11, wp->menu[i].timer++, 24, 640.0f, 0.0f);
                if (wp->menu[i].timer > 24) {
                    wp->menu[i].state = 100;
                }
                break;
            case 30:
                wp->menu[i].y = 0.0f;
                wp->menu[i].timer = 0;
                wp->menu[i].state++;
                break;
            case 31:
                wp->menu[i].x = intplGetValue(11, wp->menu[i].timer++, 24, -640.0f, 0.0f);
                if (wp->menu[i].timer > 24) {
                    wp->menu[i].state = 100;
                }
                break;
            case 40:
                wp->menu[i].y = 0.0f;
                wp->menu[i].timer = 0;
                wp->menu[i].state++;
                break;
            case 41:
                wp->menu[i].x = intplGetValue(11, wp->menu[i].timer++, 24, 0.0f, -640.0f);
                if (wp->menu[i].timer > 24) {
                    wp->menu[i].state = -1;
                }
                break;
            case -1:
            case 100:
                break;
        }
    }
}

void winBgGX(WinPauseWork* wp, s32 type, f32 x, f32 y) {
    Mtx m;
    GXTexObj obj;
    f32 sy;

    switch (type) {
    case 0:
        GXSetTevColor(GX_TEVREG0, (GXColor){ 0xC8, 0x66, 0x60, 0xFF });
        GXSetTevColor(GX_TEVREG1, (GXColor){ 0xE1, 0xA5, 0xA1, 0xFF });
        GXSetTevColor(GX_TEVREG2, (GXColor){ 0xF0, 0xCD, 0xCB, 0xFF });
        break;
    case 1:
        GXSetTevColor(GX_TEVREG0, (GXColor){ 0xC8, 0x6B, 0x96, 0xFF });
        GXSetTevColor(GX_TEVREG1, (GXColor){ 0xDB, 0xAB, 0xC6, 0xFF });
        GXSetTevColor(GX_TEVREG2, (GXColor){ 0xEC, 0xD1, 0xE1, 0xFF });
        break;
    case 2:
        GXSetTevColor(GX_TEVREG0, (GXColor){ 0xBB, 0x9C, 0x21, 0xFF });
        GXSetTevColor(GX_TEVREG1, (GXColor){ 0xD7, 0xCD, 0x76, 0xFF });
        GXSetTevColor(GX_TEVREG2, (GXColor){ 0xEA, 0xE5, 0xAD, 0xFF });
        break;
    case 3:
        GXSetTevColor(GX_TEVREG0, (GXColor){ 0x2F, 0xA5, 0x30, 0xFF });
        GXSetTevColor(GX_TEVREG1, (GXColor){ 0xA2, 0xD2, 0x6E, 0xFF });
        GXSetTevColor(GX_TEVREG2, (GXColor){ 0xCB, 0xE7, 0xA7, 0xFF });
        break;
    case 4:
        GXSetTevColor(GX_TEVREG0, (GXColor){ 0x28, 0x7E, 0xC5, 0xFF });
        GXSetTevColor(GX_TEVREG1, (GXColor){ 0x78, 0xB3, 0xD6, 0xFF });
        GXSetTevColor(GX_TEVREG2, (GXColor){ 0xAF, 0xD6, 0xEA, 0xFF });
        break;
    }
    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
    GXSetZCompLoc(GX_FALSE);
    GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    PSMTXTrans(m, x, 18.0f + y, 0.0f);
    PSMTXConcat(camGetPtr((CameraId)8)->view, m, m);
    GXLoadPosMtxImm(m, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    TEXGetGXTexObjFromPalette(wp->res->tex->tpl, &obj, 7);
    GXInitTexObjLOD(&obj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&obj, GX_TEXMAP0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    sy = 320.0f / GXGetTexObjHeight(&obj);
    PSMTXScale(m, 570.0f / GXGetTexObjWidth(&obj), sy, 1.0f);
    GXLoadTexMtxImm(m, GX_TEXMTX0, GX_MTX2x4);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(-285.0f, 160.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(-285.0f, -160.0f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(0.0f, -160.0f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(0.0f, 160.0f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(0.0f, 160.0f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(0.0f, -160.0f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(285.0f, -160.0f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(285.0f, 160.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);

    TEXGetGXTexObjFromPalette(wp->res->tex->tpl, &obj, 6);
    GXInitTexObjLOD(&obj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&obj, GX_TEXMAP0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    sy = 288.0f / GXGetTexObjHeight(&obj);
    PSMTXScale(m, 570.0f / GXGetTexObjWidth(&obj), sy, 1.0f);
    GXLoadTexMtxImm(m, GX_TEXMTX0, GX_MTX2x4);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C2, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A2);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(-285.0f, 144.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(-285.0f, -144.0f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(285.0f, -144.0f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(285.0f, 144.0f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

void winMsgMain(WinPauseWork* wp) {
    switch (wp->msgState) {
        case 0:
            wp->msgOfsX = -800.0f;
            wp->msgOfsY = -170.0f;
            wp->msgTimer = 0;
            wp->msgState++;
            break;
        case 1:
            wp->msgOfsX = intplGetValue(11, wp->msgTimer++, 16, -800.0f, 0.0f);
            if (wp->msgTimer > 16) {
                wp->msgState++;
            }
            break;
        case 2:
            if (keyGetDirTrg(0) & 0x100) {
                wp->msgLine -= 2;
                if (wp->msgLine < 0) {
                    wp->msgLine = 0;
                } else {
                    ((void (*)(s32))psndSFXOn)(0x20006);
                }
            } else if (keyGetDirTrg(0) & 0x200) {
                if (wp->msgLine / 2 < (wp->msgLines + 1) / 2 - 1) {
                    wp->msgLine += 2;
                    ((void (*)(s32))psndSFXOn)(0x20006);
                }
            }
            break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            break;
        case 10:
            wp->msgTimer = 0;
            wp->msgState++;
            break;
        case 11:
            wp->msgOfsX = intplGetValue(11, wp->msgTimer++, 16, 0.0f, -800.0f);
            if (wp->msgTimer > 16) {
                wp->msgState++;
            }
            break;
        case 12:
            break;
    }
}

#include "evt/evt_lottery.h"
void windowDispGX_Waku_col(u16 gxTexMapID, GXColor color, f32 x, f32 y, f32 width, f32 height, f32 curve);
void winIconInit(void);
void winIconSet(u32 id, Vec translate, Vec scale, GXColor color);
void winFontInit(void);
void winFontSet(Vec translate, Vec scale, GXColor color, const char* fmt, ...);
void winFontSetEdge(Vec translate, Vec scale, GXColor color, const char* fmt, ...);
void winFontSetLabel(Vec translate, Vec scale, GXColor color, const char* msg);
void winNameGX(WinPauseWork* wp, s32 type, f32 x, f32 y, f32 w, f32 h);

void winMsgDisp(WinPauseWork* wp, f32 x, f32 y) {
    GXColor white = { 0xFF, 0xFF, 0xFF, 0xFF };
    GXColor black = { 0x00, 0x00, 0x00, 0xFF };
    GXColor waku = { 0xFF, 0xFF, 0xFF, 0xFF };
    char buf[1024];
    f32 ofs;
    f32 bx;
    f32 by;
    f32 ny;
    f32 ix;
    f32 fw;
    const char* m;
    s32 n;

    bx = (10.0f + (x - 250.0f)) - 10.0f;
    by = 40.0f + y;
    windowDispGX_Waku_col(0, waku, bx, by, 500.0f, 80.0f, 20.0f);
    if (wp->msgX != 0 && wp->msgX > 0) {
        winTexInit(wp->res->tex->tpl);
        winTexSet(0x25, (Vec){ bx, by, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, white);
        winIconInit();
        winIconSet(itemDataTable[wp->msgX].icon_id, (Vec){ bx, by, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, white);
    }
    if ((const char*)wp->msgY != NULL) {
        if (wp->msgX == -2) {
            ofs = -130.0f;
        } else {
            ofs = 0.0f;
        }
        m = msgSearch((const char*)wp->msgY);
        fw = FontGetMessageWidth(m);
        winNameGX(wp, 1, (x - 120.0f) + ofs, ny = 8.0f + (16.0f + by), 240.0f, 32.0f);
        winFontInit();
        winFontSetEdge((Vec){ ofs + ((x - 120.0f) + (240.0f - fw) / 2.0f), ny - 4.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, white, m);
    }
    if (wp->msg != NULL) {
        GXSetScissor(0, (u32)(240.0f - (by -= 14.0f)), 0x260, 0x38);
        if (strcmp(wp->msg, "msg_lottery_number\0") == 0) {
            n = *(s16*)((u8*)lotteryGetPtr() + 0x20);
            sprintf(buf, msgSearch(wp->msg), n);
            winFontInit();
            winFontSet((Vec){ (40.0f + (x - 250.0f)) - 10.0f, by + (f32)(wp->msgLine * 28), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, black, buf);
        } else {
            winFontInit();
            winFontSetLabel((Vec){ (40.0f + (x - 250.0f)) - 10.0f, by + (f32)(wp->msgLine * 28), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, black, wp->msg);
        }
        GXSetScissor(0, 0, 0x260, 0x1E0);
    }
    if (wp->msgLines > 2) {
        winIconInit();
        if (wp->msgLine == 0) {
            if ((gp->retraceCount & 0x1F) < 0x14) {
                winIconSet(0x1B6, (Vec){ (250.0f + x) - 10.0f, (40.0f + (y - 40.0f)) - 24.0f, 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
            }
        } else if (wp->msgLine / 2 == (wp->msgLines + 1) / 2 - 1) {
            if ((gp->retraceCount & 0x1F) < 0x14) {
                winIconSet(0x1B5, (Vec){ (250.0f + x) - 10.0f, 24.0f + (40.0f + (y - 40.0f)), 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
            }
        } else {
            if ((gp->retraceCount & 0x1F) < 0x14) {
                ix = (250.0f + x) - 10.0f;
                winIconSet(0x1B5, (Vec){ ix, 24.0f + (by = 40.0f + (y - 40.0f)), 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
                winIconSet(0x1B6, (Vec){ ix, by - 24.0f, 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
            }
        }
        winIconSet(0x70, (Vec){ (250.0f + x) - 10.0f, 40.0f + (y - 40.0f), 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    }
}

void winMsgEntry(WinPauseWork* wp, s32 x, const char* msg, s32 y) {
    static char _tmp[0x40];
    BOOL changed;
    u16 lines;

    changed = FALSE;
    if (msg) {
        if (wp->lastMsg != msg) {
            changed = TRUE;
        } else if (strcmp(_tmp, msg)) {
            changed = TRUE;
        }
        if (changed) {
            wp->msgX = x;
            wp->msg = msg;
            wp->msgY = y;
            FontGetMessageWidthLine(msgSearch(wp->msg), &lines);
            wp->msgLine = 0;
            wp->msgLines = lines + 1;
            wp->lastMsg = msg;
            strncpy(_tmp, msg, 60);
        }
    }
}

static inline void winRootTexQuad(WinPauseWork* wp, s32 id, f32 x, f32 y, f32 w, f32 h) {
    GXTexObj obj;
    Mtx m;
    f32 sy;
    double aw;
    double ah;

    TEXGetGXTexObjFromPalette(wp->res->tex->tpl, &obj, id);
    GXInitTexObjLOD(&obj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&obj, GX_TEXMAP0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    ah = __fabs(h);
    aw = __fabs(w);
    PSMTXScale(m, (f32)aw / GXGetTexObjWidth(&obj), (f32)ah / GXGetTexObjHeight(&obj), 1.0f);
    GXLoadTexMtxImm(m, GX_TEXMTX0, GX_MTX2x4);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(x, y, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x, y - h, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x + w, y - h, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x + w, y, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}


#pragma inline_max_total_size(100000)
void winKirinukiGX(WinPauseWork* wp, s32 type, f32 x, f32 y, f32 w, f32 h) {
    f32 w74 = 74.0f;
    f32 w66 = 66.0f;
    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
    GXSetZCompLoc(GX_FALSE);
    GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXLoadPosMtxImm(camGetPtr((CameraId)8)->view, 0);
    GXSetCurrentMtx(0);
    if (type == 0) {
        winRootTexQuad(wp, 0xB, x, y, 8, 8);
        winRootTexQuad(wp, 0x9, x + 8, y, w - 8 - 8, 8);
        winRootTexQuad(wp, 0xD, x + w - 8, y, 8, 8);
        winRootTexQuad(wp, 0xA, x, y - 8, 8, h - 8 - 8);
        winRootTexQuad(wp, 0x8, x + 8, y - 8, w - 8, h - 8 - 8);
        winRootTexQuad(wp, 0xC, x, y - (h - 8), 8, 8);
        winRootTexQuad(wp, 0x8, x + 8, y - (h - 8), w - 8 - 8, 8);
        winRootTexQuad(wp, 0xE, x + w - 8, y - (h - 8), 8, 8);
    } else {
        winRootTexQuad(wp, 0xB, x, y, 8, 8);
        winRootTexQuad(wp, 0x9, x + 8, y, w - 8 - 8, 8);
        winRootTexQuad(wp, 0xD, x + w - 8, y, 8, 8);
        winRootTexQuad(wp, 0xA, x, y - 8, 8, h - 8 - 8);
        winRootTexQuad(wp, 0x8, x + 8, y - 8, w - 8, h - 8 - 8);
        winRootTexQuad(wp, 0xC, x, y - (h - 8), 8, 8);
        winRootTexQuad(wp, 0x8, x + 8, y - (h - 8), w - 8, 8);
        winRootTexQuad(wp, 0xF, x + (w - 90), y - h, 16, 8);
        winRootTexQuad(wp, 0x8, x + (w - 90) + 16, y - h, w74, 8);
        winRootTexQuad(wp, 0xA, x + (w - 90) + 8, y - h - 8, 8, 8);
        winRootTexQuad(wp, 0x8, x + (w - 90) + 16, y - h - 8, w74, 8);
        winRootTexQuad(wp, 0xC, x + (w - 90) + 8, y - h - 16, 8, 8);
        winRootTexQuad(wp, 0x8, x + (w - 90) + 16, y - h - 16, w66, 8);
        winRootTexQuad(wp, 0xE, x + w - 8, y - h - 16, 8, 8);
    }
}

static inline double fabs_d(double x) { return __fabs(x); }
static inline float fabsf_(float x) { return (float)fabs_d((double)x); }
#define NAME_QUAD(x0, y0, x1, y1)              \
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);          \
    GXPosition3f32(x0, y0, 0.0f);              \
    GXTexCoord2f32(0.0f, 0.0f);                \
    GXPosition3f32(x0, y1, 0.0f);              \
    GXTexCoord2f32(0.0f, 1.0f);                \
    GXPosition3f32(x1, y1, 0.0f);              \
    GXTexCoord2f32(1.0f, 1.0f);                \
    GXPosition3f32(x1, y0, 0.0f);              \
    GXTexCoord2f32(1.0f, 0.0f)

#define NAME_TEX(obj, id, mtx, tw, th)                                                        \
    TEXGetGXTexObjFromPalette(wp->res->tex->tpl, &obj, id);                                   \
    GXInitTexObjLOD(&obj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1); \
    GXLoadTexObj(&obj, GX_TEXMAP0);                                                           \
    GXSetNumTexGens(1);                                                                       \
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY); \
    MTXScale(mtx, fabsf_(tw) / (f32)GXGetTexObjWidth(&obj), fabsf_(th) / (f32)GXGetTexObjHeight(&obj), 1.0f); \
    GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4)

static inline void nameQuad(f32 x0, f32 y0, f32 qw, f32 qh) {
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(x0, y0, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x0, y0 - qh, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x0 + qw, y0 - qh, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x0 + qw, y0, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

void winNameGX(WinPauseWork* wp, s32 type, f32 x, f32 y, f32 w, f32 h) {
    Mtx m0a;
    Mtx m0b;
    Mtx m1a;
    Mtx m1b;
    Mtx m2a;
    Mtx m2b;
    GXTexObj t0a;
    GXTexObj t0b;
    GXTexObj t1a;
    GXTexObj t1b;
    GXTexObj t2a;
    GXTexObj t2b;

    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
    GXSetZCompLoc(GX_FALSE);
    GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXLoadPosMtxImm(camGetPtr(8)->view, 0);
    GXSetCurrentMtx(0);

    if (type == 0) {
        NAME_TEX(t0a, 0x13, m0a, w - 16.0f, 32.0f);
        nameQuad(x, y, w - 16.0f, 32.0f);
        NAME_TEX(t0b, 0x14, m0b, 16.0f, 32.0f);
        
        nameQuad((x + w) - 16.0f, y, 16.0f, 32.0f);
    } else if (type == 1) {
        NAME_TEX(t1a, 0x15, m1a, w - 16.0f, 32.0f);
        nameQuad(x, y, w - 16.0f, 32.0f);
        NAME_TEX(t1b, 0x16, m1b, 16.0f, 32.0f);
        
        nameQuad((x + w) - 16.0f, y, 16.0f, 32.0f);
    } else if (type == 2) {
        NAME_TEX(t2a, 0x78, m2a, w - 8.0f, 24.0f);
        nameQuad(x, y, w - 8.0f, 24.0f);
        NAME_TEX(t2b, 0x79, m2b, 8.0f, 24.0f);
        
        nameQuad((x + w) - 8.0f, y, 8.0f, 24.0f);
    }
}


void winFontInit(void);
void winFontSetLabel(Vec pos, Vec scale, GXColor color, const char* msg);
void* L_msgGetWork(void);
void* animPoseGetAnimDataPtr(s32 poseId);
void animPoseSetAnim(s32 poseId, const char* animName, BOOL reset);
void animPoseSetLocalTime(s32 poseId, f32 localTimeFrames);
void animPoseMain(s32 poseId);
void animPoseDraw(s32 poseId, f32 x, f32 y, f32 z, f32 rotY, f32 scale, s32 xluMode);
extern const char** luigi_book_list[];

typedef struct WinBookWork {
    u8 pad_0[0x28];
    WinRes* res; // 0x28
    u8 pad_2C[0x2C8 - 0x2C];
    s32 bookIdx;   // 0x2C8
    s32 bookPages; // 0x2CC
    s32 bookPage;  // 0x2D0
} WinBookWork;

static inline void winBookTex(WinBookWork* w, s32 id, f32 x, f32 y, f32 width, f32 height) {
    GXTexObj obj;
    Mtx m;
    f64 aw;
    f64 ah;

    TEXGetGXTexObjFromPalette(w->res->tex->tpl, &obj, id);
    GXInitTexObjLOD(&obj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&obj, GX_TEXMAP0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    ah = __fabs(height);
    aw = __fabs(width);
    PSMTXScale(m, (f32)aw / GXGetTexObjWidth(&obj), (f32)ah / GXGetTexObjHeight(&obj), 1.0f);
    GXLoadTexMtxImm(m, GX_TEXMTX0, GX_MTX2x4);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(x, y, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x, y - height, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x + width, y - height, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x + width, y, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

void winBookGX(WinBookWork* w, s32 a, f32 x, f32 y) {
    f32 off;
    s32 i;

    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
    GXSetZCompLoc(GX_FALSE);
    GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);

    if (a == 0) {
        off = 0.0f;
    } else {
        off = 32.0f;
    }

    winBookTex(w, 0x33, 8.0f + x, y - 8.0f, 432.0f, 256.0f - off);
    winBookTex(w, 0x28, x, y, 16.0f, 8.0f);
    winBookTex(w, 0x2C, (16.0f + x), y, 184.0f, 8.0f);
    winBookTex(w, 0x26, (184.0f + (16.0f + x)), y, 48.0f, 16.0f);
    winBookTex(w, 0x2D, (48.0f + (184.0f + (16.0f + x))), y, 184.0f, 8.0f);
    winBookTex(w, 0x2A, (184.0f + (48.0f + (184.0f + (16.0f + x)))), y, 16.0f, 8.0f);
    winBookTex(w, 0x30, x, y - 8.0f, 8.0f, 256.0f - off);
    winBookTex(w, 0x32, 8.0f + (184.0f + (16.0f + x)), y - 16.0f, 16.0f, 248.0f - off);
    winBookTex(w, 0x31, 8.0f + (184.0f + (48.0f + (184.0f + (16.0f + x)))), y - 8.0f, 8.0f, 256.0f - off);
    winBookTex(w, 0x29, x, y - 8.0f - 256.0f + off, 16.0f, 16.0f);
    winBookTex(w, 0x2E, (16.0f + x), y - 8.0f - 256.0f + off, 184.0f, 16.0f);
    winBookTex(w, 0x27, (184.0f + (16.0f + x)), y - 8.0f - 256.0f + off, 48.0f, 24.0f);
    winBookTex(w, 0x2F, (48.0f + (184.0f + (16.0f + x))), y - 8.0f - 256.0f + off, 184.0f, 16.0f);
    winBookTex(w, 0x2B, (184.0f + (48.0f + (184.0f + (16.0f + x)))), y - 8.0f - 256.0f + off, 16.0f, 16.0f);

    if (a == 1) {
        for (i = 0; i < 8; i++) {
            winBookTex(w, 0x34, (16.0f + x) + (i % 2) * 216, y - 32.0f - (i / 2) * 48, 192.0f, 32.0f);
        }
    }

    if (a == 0) {
        s32 pose;
        void* anim;
        f32 lx;

        winFontInit();
        lx = 20.0f + x;
        winFontSetLabel((Vec){lx, y - 14.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, (GXColor){0x00, 0x00, 0x00, 0xFF},
                        luigi_book_list[w->bookIdx][w->bookPage * 2]);
        winFontSetLabel((Vec){220.0f + lx, y - 14.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, (GXColor){0x00, 0x00, 0x00, 0xFF},
                        luigi_book_list[w->bookIdx][w->bookPage * 2 + 1]);
        pose = *(s32*)((u8*)L_msgGetWork() + 0x20);
        anim = animPoseGetAnimDataPtr(pose);
        if (w->bookPage < w->bookPages - 1) {
            animPoseSetAnim(pose, "HOS_C_1", 0);
        } else {
            animPoseSetAnim(pose, "HOS_E_1", 0);
        }
        animPoseSetLocalTime(pose, gp->retraceCount % (s32)(*(f32**)((u8*)anim + 0x24))[2]);
        animPoseMain(pose);
        animPoseDraw(pose, 425.0f + x, y - 275.0f, 0.0f, 180.0f, 5.0f, 2);
    }
}

static inline void winHalfBookQuad(TEXPalette* tpl, s32 id, f32 x, f32 y, f32 w, f32 h) {
    GXTexObj obj;
    Mtx m;
    f64 aw;
    f64 ah;

    TEXGetGXTexObjFromPalette(tpl, &obj, id);
    GXInitTexObjLOD(&obj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&obj, GX_TEXMAP0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
    ah = __fabs(h);
    aw = __fabs(w);
    MTXScale(m, (f32)aw / (f32)GXGetTexObjWidth(&obj), (f32)ah / (f32)GXGetTexObjHeight(&obj), 1.0f);
    GXLoadTexMtxImm(m, GX_TEXMTX0, GX_MTX2x4);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(x, y, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x, y - h, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x + w, y - h, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x + w, y, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

void winHalfBookGX(TEXPalette* tpl, s32 a, f32 x, f32 y) {
    s32 i;
    Mtx m;

    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    for (i = 0; i < 2; i++) {
        if (i == 0) {
            GXSetChanMatColor(GX_COLOR0A0, (GXColor){ 0x00, 0x00, 0x00, 0x80 });
            GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
            GXSetZCompLoc(GX_TRUE);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
            MTXTrans(m, 8.0f, -8.0f, 0.0f);
            MTXConcat(camGetPtr(8)->view, m, m);
            GXLoadPosMtxImm(m, 0);
            GXSetCurrentMtx(0);
        } else {
            GXSetChanMatColor(GX_COLOR0A0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
            GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
            GXSetZCompLoc(GX_FALSE);
            GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
            GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
            GXLoadPosMtxImm(camGetPtr(8)->view, 0);
            GXSetCurrentMtx(0);
        }
        winHalfBookQuad(tpl, 0x33, x + 8.0f, y - 8.0f, 320.0f, 248.0f);
        winHalfBookQuad(tpl, 0x28, x, y, 16.0f, 8.0f);
        winHalfBookQuad(tpl, 0x35, x + 16.0f, y, 248.0f, 8.0f);
        winHalfBookQuad(tpl, 0x26, x + 16.0f + 248.0f, y, 48.0f, 16.0f);
        winHalfBookQuad(tpl, 0x3B, x + 16.0f + 248.0f + 48.0f, y, 24.0f, 16.0f);
        winHalfBookQuad(tpl, 0x30, x, y - 8.0f, 8.0f, 248.0f);
        winHalfBookQuad(tpl, 0x32, x + 16.0f + 248.0f + 8.0f, y - 16.0f, 16.0f, 240.0f);
        winHalfBookQuad(tpl, 0x36, x + 16.0f, y - 200.0f, 248.0f, 40.0f);
        winHalfBookQuad(tpl, 0x3A, x, y - 256.0f, 16.0f, 8.0f);
        winHalfBookQuad(tpl, 0x38, x + 32.0f, y - 256.0f, -16.0f, 8.0f);
        winHalfBookQuad(tpl, 0x38, x + 32.0f, y - 256.0f, 240.0f, 8.0f);
        winHalfBookQuad(tpl, 0x39, x + 272.0f, y - 256.0f, 16.0f, 8.0f);
        winHalfBookQuad(tpl, 0x38, x + 288.0f, y - 256.0f, 16.0f, 8.0f);
        winHalfBookQuad(tpl, 0x38, x + 320.0f, y - 256.0f, -16.0f, 8.0f);
        winHalfBookQuad(tpl, 0x38, x + 320.0f, y - 256.0f, 16.0f, 8.0f);
        winHalfBookQuad(tpl, 0x3C, x + 328.0f, y - 16.0f, 8.0f, 240.0f);
    }
}

void winMailGX(WinPauseWork* wp, f32 x, f32 y) {
    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
    GXSetZCompLoc(GX_FALSE);
    GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXLoadPosMtxImm(camGetPtr((CameraId)8)->view, 0);
    GXSetCurrentMtx(0);
    winRootTexQuad(wp, 0x4D, x, y, 32, 24);
    winRootTexQuad(wp, 0x63, x + 32, y, 344, 24);
    winRootTexQuad(wp, 0x64, x + 200, y, 16, 24);
    winRootTexQuad(wp, 0x55, x + 32, y - 24, 344, 8);
    winRootTexQuad(wp, 0x4E, x + 32 + 72 + 40 + 48 + 16 + 112 + 40 + 16, y, 40, 24);
    winRootTexQuad(wp, 0x51, x, y - 24, 16, 248);
    winRootTexQuad(wp, 0x55, x + 16, y - 24, 16, 248);
    winRootTexQuad(wp, 0x55, x + 16 + 16 + 72 + 8 + 248 + 16, y - 24, 16, 248);
    winRootTexQuad(wp, 0x51, x + 16 + 16 + 72 + 8 + 248 + 16 + 16, y - 24, 16, 248);
    winRootTexQuad(wp, 0x52, x + 16 + 16 + 72 + 8 + 248 + 16 + 16 + 16, y - 24, 8, 248);
    winRootTexQuad(wp, 0x5C, x + 32, y - 32, 344, 240);
    winRootTexQuad(wp, 0x55, x + 32, y - 248, 344, 24);
    winRootTexQuad(wp, 0x4F, x, y - 272, 32, 32);
    winRootTexQuad(wp, 0x53, x + 32, y - 272, 344, 32);
    winRootTexQuad(wp, 0x50, x + 32 + 344, y - 272, 40, 32);
    winRootTexQuad(wp, 0x54, x + 16, y - 24 - 8, 8, -8);
    winRootTexQuad(wp, 0x54, x + 384 + 8, y - 24 - 8, -8, -8);
    winRootTexQuad(wp, 0x54, x + 16, y - 264, 8, 8);
    winRootTexQuad(wp, 0x54, x + 384 + 8, y - 264, -8, 8);
    GXSetScissor(x + 304 + 32, 240 - y + 32 + 1, 344, 200);
}

void winIconInit(void);
void winIconSet(u32 id, Vec translate, Vec scale, GXColor color);
void winFontSet(Vec translate, Vec scale, GXColor color, const char* fmt, ...);
BOOL swGet(s32 id);

typedef struct WinHakoList {
    s32 num;     // 0x0
    s32 pad_4;   // 0x4
    s32 page;    // 0x8
    f32 scroll;  // 0xC
    u8 pad_10[0x14];
    s16 ids[108]; // 0x24
} WinHakoList;

typedef struct WinHakoWork {
    u8 pad_0[0xE94];
    WinHakoList list[2]; // 0xE94, 0xF90
} WinHakoWork;

void winHakoGX(WinHakoWork* w, s32 a, f32 x, f32 y) {
    u32 c0;
    u32 c1;
    f32 scroll;
    s32 ofs;
    s32 j;
    s32 i;
    s32 py;
    s32 k;
    s32 idx;
    s32 id;
    f32 yy;
    const char* msg;
    f32 mw;
    f32 fx;
    s32 npages;
    s32 p;
    s32 page;
    s32 num;

    if (a == 0) {
        scroll = w->list[0].scroll;
        num = w->list[0].num;
        page = w->list[0].page;
    } else {
        scroll = w->list[1].scroll;
        num = w->list[1].num;
        page = w->list[1].page;
    }

    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
    GXSetZCompLoc(GX_FALSE);
    GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXSetScissor(304.0f + x, 240.0f - y, 400, 228);

    ofs = a * 13;
    for (p = 0; p < (num + 27) / 28; p++) {
        py = p * 232;
        if (scroll + (y - j * 56) - py > 400.0f || scroll + (y - j * 56) - py < -400.0f) {
            continue;
        }
        if ((ofs != 0 && p == 2) || (ofs != 1 && p == 3)) {
            winBookTex((WinBookWork*)w, (a * 13 + 0x7B), x, y + scroll - p * 232, 56.0f, 56.0f);
            winBookTex((WinBookWork*)w, (a * 13 + 0x7D), x, y - 56.0f + scroll - p * 232, 8.0f, 8.0f);
            winBookTex((WinBookWork*)w, (a * 13 + 0x7E), 8.0f + x, y - 56.0f + scroll - p * 232, 48.0f, 8.0f);
            winBookTex((WinBookWork*)w, (a * 13 + 0x85), 56.0f + x, y + scroll - p * 232, 8.0f, 8.0f);
            winBookTex((WinBookWork*)w, (a * 13 + 0x84), 56.0f + x, y - 8.0f + scroll - p * 232, 8.0f, 48.0f);
            winBookTex((WinBookWork*)w, (a * 13 + 0x81), 56.0f + x, y - 56.0f + scroll - p * 232, 8.0f, 8.0f);
            break;
        }
        for (i = 0; i < 7; i++) {
            for (j = 0; j < 4; j++) {
                winBookTex((WinBookWork*)w, (a * 13 + 0x7B) + ((i + j * 7) & 1), x + i * 56, scroll + (y - j * 56) - py, 56.0f, 56.0f);
            }
        }
        c0 = (a * 13 + 0x7D);
        c1 = (a * 13 + 0x7E);
        for (i = 0; i < 7; i++) {
            if (i == 0) {
                id = c0;
            } else if (i & 1) {
                id = (a * 13 + 0x7F);
            } else {
                id = (a * 13 + 0x86);
            }
            winBookTex((WinBookWork*)w, id, x + i * 56, y - 224.0f + scroll - py, 8.0f, 8.0f);
            if ((i & 1) == 0) {
                id = c1;
            } else {
                id = (a * 13 + 0x80);
            }
            winBookTex((WinBookWork*)w, id, 8.0f + (x + i * 56), y - 224.0f + scroll - py, 48.0f, 8.0f);
        }
        c0 = (a * 13 + 0x85);
        c1 = (a * 13 + 0x84);
        for (i = 0; i < 4; i++) {
            if (i == 0) {
                id = c0;
            } else if (i & 1) {
                id = (a * 13 + 0x83);
            } else {
                id = (a * 13 + 0x87);
            }
            winBookTex((WinBookWork*)w, id, 392.0f + x, scroll + (y - i * 56) - py, 8.0f, 8.0f);
            if ((i & 1) == 0) {
                id = c1;
            } else {
                id = (a * 13 + 0x82);
            }
            winBookTex((WinBookWork*)w, id, 392.0f + x, scroll + (y - i * 56 - 8.0f) - py, 8.0f, 48.0f);
        }
        winBookTex((WinBookWork*)w, (a * 13 + 0x81), 392.0f + x, y - 224.0f + scroll - py, 8.0f, 8.0f);
    }

    npages = (num + 27) / 28;
    winTexInit(((WinPauseWork*)w)->res->tex->tpl);
    for (p = 0; p < npages; p++) {
        if (scroll + (y - j * 56) - p * 232 > 400.0f || scroll + (y - j * 56) - p * 232 < -400.0f) {
            continue;
        }
        for (j = 0; j < 4; j++) {
            for (i = 0; i < 7; i++) {
                s32 n = p * 28 + j * 7 + i;
                if (n >= num) {
                    continue;
                }
                yy = scroll + (y - j * 56) - p * 232 - 28.0f;
                if (ofs == 0) {
                    if (swGet(w->list[0].ids[n] + 0x80)) {
                        continue;
                    }
                } else {
                    if (swGet(w->list[1].ids[n] + 0x41)) {
                        continue;
                    }
                }
                winTexSet(0x97, (Vec){4.0f + (24.0f + (x + i * 56)), yy, 0.0f}, (Vec){1.0f, 1.0f, 1.0f},
                          (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
            }
        }
    }

    winIconInit();
    for (p = 0; p < npages; p++) {
        for (j = 0; j < 4; j++) {
            for (i = 0; i < 7; i++) {
                s32 n = p * 28 + j * 7 + i;
                s32 icon;
                if (n >= num) {
                    continue;
                }
                yy = scroll + (y - j * 56) - p * 232 - 28.0f;
                if (ofs == 0) {
                    if (!swGet(w->list[0].ids[n] + 0x80)) {
                        continue;
                    }
                    icon = itemDataTable[w->list[0].ids[n] + 0xF0].icon_id;
                } else {
                    if (!swGet(w->list[1].ids[n] + 0x41)) {
                        continue;
                    }
                    icon = itemDataTable[w->list[1].ids[n] + 0xB3].icon_id;
                }
                winIconSet(icon, (Vec){4.0f + (24.0f + (x + i * 56)), yy, 0.0f}, (Vec){0.8f, 0.8f, 0.8f},
                           (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
            }
        }
    }

    GXSetScissor(0, 0, 608, 480);
    if (page > 0) {
        winTexInit(((WinPauseWork*)w)->res->tex->tpl);
        fx = 395.0f + x;
        winTexSet(0x17, (Vec){fx, 15.0f + y, 0.0f}, (Vec){-1.0f, -1.0f, -1.0f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        winIconInit();
        winIconSet(0x7E, (Vec){fx, y, 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    }
    if (num > page * 28 + 28) {
        winTexInit(((WinPauseWork*)w)->res->tex->tpl);
        fx = 395.0f + x;
        winTexSet(0x17, (Vec){fx, y - 235.0f, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        winIconInit();
        winIconSet(0x80, (Vec){fx, y - 220.0f, 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    }

    msg = msgSearch("msg_menu_sort_narabikae");
    mw = 0.7f * FontGetMessageWidth(msg);
    winFontInit();
    fx = x + (380.0f - mw);
    winFontSet((Vec){fx, y - 228.0f, 0.0f}, (Vec){0.7f, 0.7f, 0.7f}, (GXColor){0x00, 0x00, 0x00, 0xFF}, msg);
    winIconInit();
    winIconSet(0x217, (Vec){fx - 30.0f, y - 237.0f, 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
}

static inline void winWazaQuad(WinPauseWork* wp, s32 id, f32 x, f32 y, f32 w, f32 h) {
    GXTexObj obj;
    Mtx m;
    f64 aw;
    f64 ah;

    TEXGetGXTexObjFromPalette(wp->res->tex->tpl, &obj, id);
    GXInitTexObjLOD(&obj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&obj, GX_TEXMAP0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
    ah = __fabs(h);
    aw = __fabs(w);
    MTXScale(m, (f32)aw / (f32)GXGetTexObjWidth(&obj), (f32)ah / (f32)GXGetTexObjHeight(&obj), 1.0f);
    GXLoadTexMtxImm(m, GX_TEXMTX0, GX_MTX2x4);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(x, y, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x, y - h, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x + w, y - h, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x + w, y, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

void winWazaGX(WinPauseWork* wp, s32 a, f32 x, f32 y, f32 w, f32 h) {
    f32 w1;

    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
    GXSetZCompLoc(GX_FALSE);
    GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXLoadPosMtxImm(camGetPtr(8)->view, 0);
    GXSetCurrentMtx(0);

    w1 = 0.75f * w;
    winWazaQuad(wp, 0x98, x, y, w1, 32.0f);
    winWazaQuad(wp, 0x9A, x + w1, y, w - w1 - 16.0f, 32.0f);
    winWazaQuad(wp, 0x9B, x + (w - 16.0f), y, 16.0f, 32.0f);
    winWazaQuad(wp, 0xA2, x, y - 32.0f, w1, h - 32.0f - 16.0f);
    winWazaQuad(wp, 0x9E, x + w1, y - 32.0f, w - w1 - 8.0f, h - 32.0f - 16.0f);
    winWazaQuad(wp, 0xA1, x + (w - 8.0f), y - 32.0f, 8.0f, h - 32.0f - 16.0f);
    winWazaQuad(wp, 0x9E, x + w1, y - (h - 16.0f), w - w1 - 8.0f, 8.0f);
    winWazaQuad(wp, 0xA1, x + (w - 8.0f), y - (h - 16.0f), 8.0f, 8.0f);
    winWazaQuad(wp, 0x9C, x, y - (h - 16.0f), w1, 16.0f);
    winWazaQuad(wp, 0x9F, x + w1, y - (h - 8.0f), w - w1 - 16.0f, 8.0f);
    winWazaQuad(wp, 0x9D, x + (w - 16.0f), y - (h - 16.0f), 16.0f, 16.0f);
}

WinSortData sort_1[] = {
    {"msg_menu_sort_aiueo", sort_1_1_func},
    {"msg_menu_sort_kind", sort_1_2_func},
    {NULL, NULL},
};
WinSortData sort_2[] = {
    {"msg_menu_sort_aiueo", sort_2_1_func},
    {"msg_menu_sort_kind", sort_2_2_func},
    {NULL, NULL},
};
WinSortData sort_3[] = {
    {"msg_menu_sort_aiueo", sort_3_1_func},
    {"msg_menu_sort_kind", sort_3_2_func},
    {"msg_menu_sort_bp", sort_3_3_func},
    {NULL, NULL},
};
WinSortData sort_4[] = {
    {"msg_menu_sort_aiueo", sort_4_1_func},
    {"msg_menu_sort_kind", sort_4_2_func},
    {"msg_menu_sort_bp", sort_4_3_func},
    {NULL, NULL},
};
WinSortData sort_5[] = {
    {"msg_menu_sort_aiueo", sort_5_1_func},
    {"msg_menu_sort_kind", sort_5_2_func},
    {"msg_menu_sort_bp", sort_5_3_func},
    {NULL, NULL},
};
WinSortData sort_6[] = {
    {"msg_menu_sort_aiueo", sort_6_1_func},
    {"msg_menu_sort_kind", sort_6_2_func},
    {NULL, NULL},
};
WinSortData sort_7[] = {
    {"msg_menu_sort_number", sort_7_1_func},
    {"msg_menu_sort_place", sort_7_2_func},
    {"msg_menu_sort_aiueo", sort_7_3_func},
    {NULL, NULL},
};
WinSortData sort_8[] = {
    {"msg_menu_sort_aiueo", sort_8_1_func},
    {"msg_menu_sort_number", sort_8_2_func},
    {NULL, NULL},
};
WinSortData* sort_data[] = {sort_1, sort_2, sort_3, sort_4, sort_5, sort_6, sort_7, sort_8};


void sort_1_1_func(WinPauseWork* wp) {
    s32 sel = wp->itemSel[0];
    s32 mode;

    pouchGetHaveItemCnt();
    pouchHaveItem(sel);
    mode = 7;
    if (!wp->sortRev) {
        mode = 0;
    }
    pouchSortItem(mode);
    wp->sortRev = 1 - wp->sortRev;
    winMakeSkipList(wp);
    wp->itemSel[0] = 0;
    wp->itemPage[0] = wp->itemSel[0] / 10;
}

void sort_1_2_func(WinPauseWork* wp) {
    s32 sel = wp->itemSel[0];

    pouchGetHaveItemCnt();
    pouchHaveItem(sel);
    pouchSortItem(!wp->sortRev ? 1 : 8);
    wp->sortRev = 1 - wp->sortRev;
    winMakeSkipList(wp);
    wp->itemSel[0] = 0;
    wp->itemPage[0] = wp->itemSel[0] / 10;
}

void sort_2_1_func(WinPauseWork* wp) {
    pouchSortItem(!wp->sortRev ? 2 : 9);
    wp->sortRev = 1 - wp->sortRev;
    winMakeSkipList(wp);
    wp->itemSel[1] = 0;
    wp->itemPage[1] = wp->itemSel[1] / 10;
}

void sort_2_2_func(WinPauseWork* wp) {
    pouchSortItem(!wp->sortRev ? 3 : 10);
    wp->sortRev = 1 - wp->sortRev;
    winMakeSkipList(wp);
    wp->itemSel[1] = 0;
    wp->itemPage[1] = wp->itemSel[1] / 10;
}

void sort_3_1_func(WinPauseWork* wp) {
    s32 sel = wp->badgeSel[0];

    pouchGetHaveBadgeCnt();
    pouchHaveBadge(sel);
    pouchSortItem(!wp->sortRev ? 4 : 11);
    wp->sortRev = 1 - wp->sortRev;
    winMakeEquipList(wp);
    wp->badgeSel[0] = 0;
    wp->badgePage[0] = wp->badgeSel[0] / 8;
}

void sort_3_2_func(WinPauseWork* wp) {
    s32 sel = wp->badgeSel[0];

    pouchGetHaveBadgeCnt();
    pouchHaveBadge(sel);
    pouchSortItem(!wp->sortRev ? 5 : 12);
    wp->sortRev = 1 - wp->sortRev;
    winMakeEquipList(wp);
    wp->badgeSel[0] = 0;
    wp->badgePage[0] = wp->badgeSel[0] / 8;
}

void sort_3_3_func(WinPauseWork* wp) {
    s32 sel = wp->badgeSel[0];

    pouchGetHaveBadgeCnt();
    pouchHaveBadge(sel);
    pouchSortItem(!wp->sortRev ? 6 : 13);
    pouchSortItem(!wp->sortRev ? 6 : 13);
    wp->sortRev = 1 - wp->sortRev;
    winMakeEquipList(wp);
    wp->badgeSel[0] = 0;
    wp->badgePage[0] = wp->badgeSel[0] / 8;
}

s32 N_compare_func4_1(const void* a, const void* b) {
    char buf1[128];
    char buf2[128];
    s16 ia = ((WinBadgeEnt*)a)->id;
    s16 ib = ((WinBadgeEnt*)b)->id;

    unk_800d06b4(msgSearch(itemDataTable[ia].name), buf1);
    unk_800d06b4(msgSearch(itemDataTable[ib].name), buf2);
    return strcmp(buf1, buf2);
}

s32 N_compare_func4_1_r(const void* a, const void* b) {
    char buf1[128];
    char buf2[128];
    s16 ia = ((WinBadgeEnt*)a)->id;
    s16 ib = ((WinBadgeEnt*)b)->id;

    unk_800d06b4(msgSearch(itemDataTable[ia].name), buf1);
    unk_800d06b4(msgSearch(itemDataTable[ib].name), buf2);
    return strcmp(buf2, buf1);
}

s32 N_compare_func4_2(const void* a, const void* b) {
    s16 ia = itemDataTable[(s16)((WinBadgeEnt*)a)->id].type_sort_order;
    s16 ib = itemDataTable[(s16)((WinBadgeEnt*)b)->id].type_sort_order;

    if (ia > ib) {
        return 1;
    } else if (ia < ib) {
        return -1;
    }
    return 0;
}

s32 N_compare_func4_2_r(const void* a, const void* b) {
    s16 ia = itemDataTable[(s16)((WinBadgeEnt*)a)->id].type_sort_order;
    s16 ib = itemDataTable[(s16)((WinBadgeEnt*)b)->id].type_sort_order;

    if (ia < ib) {
        return 1;
    } else if (ia > ib) {
        return -1;
    }
    return 0;
}

void sort_4_1_func(WinPauseWork* wp) {
    if (!wp->sortRev) {
        qqsort(wp->badgeList, wp->badgeNum, 12, N_compare_func4_1);
    } else {
        qqsort(wp->badgeList, wp->badgeNum, 12, N_compare_func4_1_r);
    }
    wp->sortRev = 1 - wp->sortRev;
    wp->badgeSel[1] = 0;
    wp->badgePage[1] = wp->badgeSel[1] / 8;
}

void sort_4_2_func(WinPauseWork* wp) {
    if (!wp->sortRev) {
        qqsort(wp->badgeList, wp->badgeNum, 12, N_compare_func4_2);
    } else {
        qqsort(wp->badgeList, wp->badgeNum, 12, N_compare_func4_2_r);
    }
    wp->sortRev = 1 - wp->sortRev;
    wp->badgeSel[1] = 0;
    wp->badgePage[1] = wp->badgeSel[1] / 8;
}

void sort_4_3_func(WinPauseWork* wp) {
    ItemData* db;
    ItemData* da;
    WinBadgeEnt tmp;
    s32 i;
    BOOL swap;
    s32 j;

    if (!wp->sortRev) {
        for (i = 0; i < wp->badgeNum - 1; i++) {
            for (j = i + 1; j < wp->badgeNum; j++) {
                da = &itemDataTable[(s16)wp->badgeList[i].id];
                db = &itemDataTable[(s16)wp->badgeList[j].id];
                swap = FALSE;

                if (da->bp_cost > db->bp_cost) {
                    swap = TRUE;
                } else if (da->bp_cost == db->bp_cost) {
                    if (da->type_sort_order > db->type_sort_order) {
                        swap = TRUE;
                    }
                }
                if (swap) {
                    tmp = wp->badgeList[i];
                    wp->badgeList[i] = wp->badgeList[j];
                    wp->badgeList[j] = tmp;
                }
            }
        }
    } else {
        for (i = 0; i < wp->badgeNum - 1; i++) {
            for (j = i + 1; j < wp->badgeNum; j++) {
                da = &itemDataTable[(s16)wp->badgeList[i].id];
                db = &itemDataTable[(s16)wp->badgeList[j].id];
                swap = FALSE;

                if (da->bp_cost < (&itemDataTable[(s16)wp->badgeList[j].id])->bp_cost) {
                    swap = TRUE;
                } else if (da->bp_cost == (&itemDataTable[(s16)wp->badgeList[j].id])->bp_cost) {
                    if (da->type_sort_order < (&itemDataTable[(s16)wp->badgeList[j].id])->type_sort_order) {
                        swap = TRUE;
                    }
                }
                if (swap) {
                    tmp = wp->badgeList[i];
                    wp->badgeList[i] = wp->badgeList[j];
                    wp->badgeList[j] = tmp;
                }
            }
        }
    }
    wp->sortRev = 1 - wp->sortRev;
    wp->badgeSel[1] = 0;
    wp->badgePage[1] = wp->badgeSel[1] / 8;
}

s32 compare_func5_1(const void* a, const void* b) {
    char buf1[128];
    char buf2[128];
    s16 ia = *(s16*)a + 0xF0;
    s16 ib = *(s16*)b + 0xF0;

    unk_800d06b4(msgSearch(itemDataTable[ia].name), buf1);
    unk_800d06b4(msgSearch(itemDataTable[ib].name), buf2);
    return strcmp(buf1, buf2);
}

s32 compare_func5_1_r(const void* a, const void* b) {
    char buf1[128];
    char buf2[128];
    s16 ia = *(s16*)a + 0xF0;
    s16 ib = *(s16*)b + 0xF0;

    unk_800d06b4(msgSearch(itemDataTable[ia].name), buf1);
    unk_800d06b4(msgSearch(itemDataTable[ib].name), buf2);
    return strcmp(buf2, buf1);
}

s32 compare_func5_2(const void* a, const void* b) {
    s16 ia = itemDataTable[(s16)(*(s16*)a + 0xF0)].type_sort_order;
    s16 ib = itemDataTable[(s16)(*(s16*)b + 0xF0)].type_sort_order;

    if (ia > ib) {
        return 1;
    } else if (ia < ib) {
        return -1;
    }
    return 0;
}

s32 compare_func5_2_r(const void* a, const void* b) {
    s16 ia = itemDataTable[(s16)(*(s16*)a + 0xF0)].type_sort_order;
    s16 ib = itemDataTable[(s16)(*(s16*)b + 0xF0)].type_sort_order;

    if (ia < ib) {
        return 1;
    } else if (ia > ib) {
        return -1;
    }
    return 0;
}

void sort_5_1_func(WinPauseWork* wp) {
    if (!wp->sortRev) {
        qqsort(wp->recipeList, wp->recipeNum, 2, compare_func5_1);
    } else {
        qqsort(wp->recipeList, wp->recipeNum, 2, compare_func5_1_r);
    }
    wp->sortRev = 1 - wp->sortRev;
    wp->recipeSel = 0;
    wp->recipePage = wp->recipeSel / 28;
}

void sort_5_2_func(WinPauseWork* wp) {
    if (!wp->sortRev) {
        qqsort(wp->recipeList, wp->recipeNum, 2, compare_func5_2);
    } else {
        qqsort(wp->recipeList, wp->recipeNum, 2, compare_func5_2_r);
    }
    wp->sortRev = 1 - wp->sortRev;
    wp->recipeSel = 0;
    wp->recipePage = wp->recipeSel / 28;
}

void sort_5_3_func(WinPauseWork* wp) {

    if (!wp->sortRev) {
        ItemData* db;
        ItemData* da;
        s16 a;
        s16 b;
        s32 i;
        s32 j;
        s32 swap;
        for (i = 0; i < wp->recipeNum - 1; ++i) {
            for (j = i + 1; j < wp->recipeNum; ++j) {
                a = wp->recipeList[i];
                b = wp->recipeList[j];
                da = &itemDataTable[(s16)(a + 0xF0)];
                db = &itemDataTable[(s16)(b + 0xF0)];
                swap = FALSE;

                if (da->bp_cost > db->bp_cost) {
                    swap = TRUE;
                } else if (da->bp_cost == db->bp_cost) {
                    if (da->type_sort_order > db->type_sort_order) {
                        swap = TRUE;
                    }
                }
                if (swap) {
                    wp->recipeList[i] = b;
                    wp->recipeList[j] = a;
                }
            }
        }
    } else {
        ItemData* db;
        ItemData* da;
        s16 a;
        s16 b;
        s32 i;
        s32 swap;
        s32 j;
        for (i = 0; i < wp->recipeNum - 1; ++i) {
            for (j = i + 1; j < wp->recipeNum; ++j) {
                a = wp->recipeList[i];
                b = wp->recipeList[j];
                da = &itemDataTable[(s16)(a + 0xF0)];
                db = &itemDataTable[(s16)(b + 0xF0)];
                swap = FALSE;

                if (da->bp_cost < db->bp_cost) {
                    swap = TRUE;
                } else if (da->bp_cost == db->bp_cost) {
                    if (da->type_sort_order < db->type_sort_order) {
                        swap = TRUE;
                    }
                }
                if (swap) {
                    wp->recipeList[i] = b;
                    wp->recipeList[j] = a;
                }
            }
        }
    }
    wp->sortRev = 1 - wp->sortRev;
    wp->recipeSel = 0;
    wp->recipePage = wp->recipeSel / 28;
}

s32 compare_func6_1(const void* a, const void* b) {
    char buf1[128];
    char buf2[128];
    s16 ia = *(s16*)a + 0xB3;
    s16 ib = *(s16*)b + 0xB3;

    unk_800d06b4(msgSearch(itemDataTable[ia].name), buf1);
    unk_800d06b4(msgSearch(itemDataTable[ib].name), buf2);
    return strcmp(buf1, buf2);
}

s32 compare_func6_1_r(const void* a, const void* b) {
    char buf1[128];
    char buf2[128];
    s16 ia = *(s16*)a + 0xB3;
    s16 ib = *(s16*)b + 0xB3;

    unk_800d06b4(msgSearch(itemDataTable[ia].name), buf1);
    unk_800d06b4(msgSearch(itemDataTable[ib].name), buf2);
    return strcmp(buf2, buf1);
}

s32 compare_func6_2(const void* a, const void* b) {
    s16 ia = itemDataTable[(s16)(*(s16*)a + 0xB3)].type_sort_order;
    s16 ib = itemDataTable[(s16)(*(s16*)b + 0xB3)].type_sort_order;

    if (ia > ib) {
        return 1;
    } else if (ia < ib) {
        return -1;
    }
    return 0;
}

s32 compare_func6_2_r(const void* a, const void* b) {
    s16 ia = itemDataTable[(s16)(*(s16*)a + 0xB3)].type_sort_order;
    s16 ib = itemDataTable[(s16)(*(s16*)b + 0xB3)].type_sort_order;

    if (ia < ib) {
        return 1;
    } else if (ia > ib) {
        return -1;
    }
    return 0;
}

void sort_6_1_func(WinPauseWork* wp) {
    if (!wp->sortRev) {
        qqsort(wp->badgeLogList, wp->badgeLogNum, 2, compare_func6_1);
    } else {
        qqsort(wp->badgeLogList, wp->badgeLogNum, 2, compare_func6_1_r);
    }
    wp->sortRev = 1 - wp->sortRev;
    wp->badgeLogSel = 0;
    wp->badgeLogPage = wp->badgeLogSel / 28;
}

void sort_6_2_func(WinPauseWork* wp) {
    if (!wp->sortRev) {
        qqsort(wp->badgeLogList, wp->badgeLogNum, 2, compare_func6_2);
    } else {
        qqsort(wp->badgeLogList, wp->badgeLogNum, 2, compare_func6_2_r);
    }
    wp->sortRev = 1 - wp->sortRev;
    wp->badgeLogSel = 0;
    wp->badgeLogPage = wp->badgeLogSel / 28;
}

s32 compare_func1(const void* a, const void* b) {
    if (*(u8*)a > *(u8*)b) {
        return 1;
    } else if (*(u8*)a < *(u8*)b) {
        return -1;
    }
    return 0;
}

s32 compare_func1_r(const void* a, const void* b) {
    if (*(u8*)a < *(u8*)b) {
        return 1;
    } else if (*(u8*)a > *(u8*)b) {
        return -1;
    }
    return 0;
}

void sort_7_1_func(WinPauseWork* wp) {
    if (!wp->sortRev) {
        qqsort(wp->tattleList, wp->tattleNum, 2, compare_func1);
    } else {
        qqsort(wp->tattleList, wp->tattleNum, 2, compare_func1_r);
    }
    wp->sortRev = 1 - wp->sortRev;
    wp->tattleSel = 0;
    wp->tattlePage = wp->tattleSel / 16;
}

static inline u32 getMonosiriPlace(const char* place) {
    static char* tbl[] = {
        "nara", "gor", "tik", "shiga", "hei", "nok", "stn", "gon", "win", "hou", "mri", "tou", "usu",
        "gra", "jin", "muj", "dou", "hom", "eki", "rsh", "pik", "sin", "bom", "moo", "aji", "las",
    };
    char buf[64];
    u32 i;

    for (i = 0; i < 26; i++) {
        sprintf(buf, "menu_monosiri_%s", tbl[i]);
        if (!strcmp(buf, place)) {
            break;
        }
    }
    if (i >= 26) {
        i = 0;
    }
    return i;
}

s32 compare_func2(const void* a, const void* b) {
    TattleEntry* ta = battleGetUnitMonosiriPtr(((WinSortEnt*)a)->id);
    TattleEntry* tb = battleGetUnitMonosiriPtr(((WinSortEnt*)b)->id);
    s32 ia = getMonosiriPlace(ta->locationTag);
    s32 ib = getMonosiriPlace(tb->locationTag);

    if (ia > ib) {
        return 1;
    } else if (ia < ib) {
        return -1;
    }
    return 0;
}

s32 compare_func2_r(const void* a, const void* b) {
    TattleEntry* ta = battleGetUnitMonosiriPtr(((WinSortEnt*)a)->id);
    TattleEntry* tb = battleGetUnitMonosiriPtr(((WinSortEnt*)b)->id);
    s32 ia = getMonosiriPlace(ta->locationTag);
    s32 ib = getMonosiriPlace(tb->locationTag);

    if (ia < ib) {
        return 1;
    } else if (ia > ib) {
        return -1;
    }
    return 0;
}

void sort_7_2_func(WinPauseWork* wp) {
    if (!wp->sortRev) {
        qqsort(wp->tattleList, wp->tattleNum, 2, compare_func1);
        qqsort(wp->tattleList, wp->tattleNum, 2, compare_func2);
    } else {
        qqsort(wp->tattleList, wp->tattleNum, 2, compare_func1_r);
        qqsort(wp->tattleList, wp->tattleNum, 2, compare_func2_r);
    }
    wp->sortRev = 1 - wp->sortRev;
    wp->tattleSel = 0;
    wp->tattlePage = wp->tattleSel / 16;
}

s32 compare_func3(const void* a, const void* b) {
    char buf1[128];
    char buf2[128];
    TattleEntry* ta = battleGetUnitMonosiriPtr(((WinSortEnt*)a)->id);
    TattleEntry* tb = battleGetUnitMonosiriPtr(((WinSortEnt*)b)->id);

    unk_800d06b4(msgSearch(ta->enemyTag), buf1);
    unk_800d06b4(msgSearch(tb->enemyTag), buf2);
    if (((WinSortEnt*)a)->id == 0x4D || ((WinSortEnt*)a)->id == 0x87) {
        if (evtGetValue(NULL, GSW(0)) < 210) {
            unk_800d06b4(msgSearch("btl_un_hatena"), buf1);
        } else {
            unk_800d06b4(msgSearch("btl_un_rampell"), buf1);
        }
    }
    if (((WinSortEnt*)b)->id == 0x4D || ((WinSortEnt*)b)->id == 0x87) {
        if (evtGetValue(NULL, GSW(0)) < 210) {
            unk_800d06b4(msgSearch("btl_un_hatena"), buf2);
        } else {
            unk_800d06b4(msgSearch("btl_un_rampell"), buf2);
        }
    }
    return strcmp(buf1, buf2);
}

s32 compare_func3_r(const void* a, const void* b) {
    char buf1[128];
    char buf2[128];
    TattleEntry* ta = battleGetUnitMonosiriPtr(((WinSortEnt*)a)->id);
    TattleEntry* tb = battleGetUnitMonosiriPtr(((WinSortEnt*)b)->id);

    unk_800d06b4(msgSearch(ta->enemyTag), buf1);
    unk_800d06b4(msgSearch(tb->enemyTag), buf2);
    if (((WinSortEnt*)a)->id == 0x4D || ((WinSortEnt*)a)->id == 0x87) {
        if (evtGetValue(NULL, GSW(0)) < 210) {
            unk_800d06b4(msgSearch("btl_un_hatena"), buf1);
        } else {
            unk_800d06b4(msgSearch("btl_un_rampell"), buf1);
        }
    }
    if (((WinSortEnt*)b)->id == 0x4D || ((WinSortEnt*)b)->id == 0x87) {
        if (evtGetValue(NULL, GSW(0)) < 210) {
            unk_800d06b4(msgSearch("btl_un_hatena"), buf2);
        } else {
            unk_800d06b4(msgSearch("btl_un_rampell"), buf2);
        }
    }
    return strcmp(buf2, buf1);
}

void sort_7_3_func(WinPauseWork* wp) {
    if (!wp->sortRev) {
        qqsort(wp->tattleList, wp->tattleNum, 2, compare_func1);
        qqsort(wp->tattleList, wp->tattleNum, 2, compare_func3);
    } else {
        qqsort(wp->tattleList, wp->tattleNum, 2, compare_func1_r);
        qqsort(wp->tattleList, wp->tattleNum, 2, compare_func3_r);
    }
    wp->sortRev = 1 - wp->sortRev;
    wp->tattleSel = 0;
    wp->tattlePage = wp->tattleSel / 16;
}

s32 compare_func4(const void* a, const void* b) {
    MailData* ma = mailGetPtr(((WinSortEnt*)a)->id);
    MailData* mb = mailGetPtr(((WinSortEnt*)b)->id);
    const char* sa = msgSearch(ma->name);
    const char* sb = msgSearch(mb->name);

    return strcmp(sa, sb);
}

s32 compare_func4_r(const void* a, const void* b) {
    MailData* ma = mailGetPtr(((WinSortEnt*)a)->id);
    MailData* mb = mailGetPtr(((WinSortEnt*)b)->id);
    const char* sa = msgSearch(ma->name);
    const char* sb = msgSearch(mb->name);

    return strcmp(sb, sa);
}

void sort_8_1_func(WinPauseWork* wp) {
    if (!wp->sortRev) {
        qqsort(wp->mailList, wp->mailNum, 2, compare_func4);
    } else {
        qqsort(wp->mailList, wp->mailNum, 2, compare_func4_r);
    }
    wp->sortRev = 1 - wp->sortRev;
    wp->mailSel = 0;
    wp->mailPage = wp->mailSel / 5;
}

s32 compare_func5(const void* a, const void* b) {
    if (*(u8*)a > *(u8*)b) {
        return 1;
    } else if (*(u8*)a < *(u8*)b) {
        return -1;
    }
    return 0;
}

s32 compare_func5_r(const void* a, const void* b) {
    if (*(u8*)a < *(u8*)b) {
        return 1;
    } else if (*(u8*)a > *(u8*)b) {
        return -1;
    }
    return 0;
}

void sort_8_2_func(WinPauseWork* wp) {
    if (!wp->sortRev) {
        qqsort(wp->mailList, wp->mailNum, 2, compare_func5);
    } else {
        qqsort(wp->mailList, wp->mailNum, 2, compare_func5_r);
    }
    wp->sortRev = 1 - wp->sortRev;
    wp->mailSel = 0;
    wp->mailPage = wp->mailSel / 5;
}

void winSortMain(WinPauseWork* wp) {
    u32 trg = wp->buttonTrg;
    u32 rep = wp->dirRep;
    const char* msg;

    if (!wp->sortActive) {
        trg = 0;
        rep = 0;
    }
    if (trg & 0x200) {
        ((void (*)(s32))psndSFXOn)(0x20013);
        wp->sortActive = 0;
        wp->sortTgtX = -600.0f;
    } else if (rep & 0x1000) {
        ((void (*)(s32))psndSFXOn)(0x20005);
        wp->sortSel--;
        if (wp->sortSel < 0) {
            wp->sortSel = wp->sortNum - 1;
        }
    } else if (rep & 0x2000) {
        ((void (*)(s32))psndSFXOn)(0x20005);
        wp->sortSel++;
        if (wp->sortSel >= wp->sortNum) {
            wp->sortSel = 0;
        }
    } else if (trg & 0x100) {
        ((void (*)(s32))psndSFXOn)(0x20012);
        sort_data[wp->sortType][wp->sortSel].func(wp);
    } else if (trg & 0x1000) {
        wp->sortActive = 0;
        wp->sortTgtX = -600.0f;
    }
    if (wp->sortActive) {
        wp->sortMsgX = 30.0f + wp->sortTgtX;
        wp->sortMsgY = (wp->sortTgtY - 40.0f) - (f32)(wp->sortSel * 40);
        winMsgEntry(wp, 0, sort_data[wp->sortType][wp->sortSel].name, 0);
    }
    wp->sortPosX += (wp->sortTgtX - wp->sortPosX) / 8.0f;
    wp->sortPosY += (wp->sortTgtY - wp->sortPosY) / 8.0f;
}

static inline double winSortFabs(double x) {
    return __fabs(x);
}

static inline void winSortTex(WinPauseWork* wp, s32 id, f32 x, f32 y, f32 w, f32 h) {
    GXTexObj obj;
    Mtx m;

    TEXGetGXTexObjFromPalette(wp->res->tex->tpl, &obj, id);
    GXInitTexObjLOD(&obj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&obj, GX_TEXMAP0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x1E, GX_FALSE, 0x7D);
    PSMTXScale(m, (f32)winSortFabs(w) / (f32)GXGetTexObjWidth(&obj), (f32)winSortFabs(h) / (f32)GXGetTexObjHeight(&obj),
               1.0f);
    GXLoadTexMtxImm(m, 0x1E, GX_MTX2x4);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(x, y, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x, y - h, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x + w, y - h, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x + w, y, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

void winSortGX(WinPauseWork* wp) {
    f32 x = wp->sortPosX;
    f32 y = wp->sortPosY;
    s32 i;
    f32 w = 128.0f;
    f32 w2 = 136.0f;
    f32 yy;
    u32 kind;

    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
    GXSetZCompLoc(GX_FALSE);
    GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXLoadPosMtxImm(camGetPtr((CameraId)8)->view, 0);
    GXSetCurrentMtx(0);

    if (wp->sortType == 7) {
        GXSetScissor((u32)(16.0f + (16.0f + (304.0f + *(f32*)((u8*)wp + 0x244)))),
                     (u32)(40.0f + (240.0f - *(f32*)((u8*)wp + 0x24C))), 0x200, 0xC0);
    }

    winSortTex(wp, 0xA5, x, y, w, 24.0f);
    winSortTex(wp, 0xA3, x + w, y, 24.0f, 24.0f);
    for (i = 0; i < wp->sortNum; i++) {
        winSortTex(wp, 0xA6, 8.0f + (x + 128.0f), (y - 24.0f) - (f32)(i * 40), 16.0f, 40.0f);
    }
    winSortTex(wp, 0xA4, x + w, (y - 24.0f) - (f32)(wp->sortNum * 40), 24.0f, 24.0f);
    winSortTex(wp, 0xA7, x, (y - 24.0f) - (f32)(wp->sortNum * 40), w, 24.0f);
    winSortTex(wp, 0xA8, x, y - 24.0f, w2, wp->sortNum * 40);

    for (i = 0; i < wp->sortNum; i++) {
        WinSortData* sd = sort_data[wp->sortType];
        if (strcmp(sd[i].name, "msg_menu_sort_aiueo") == 0) {
            kind = 0xA9;
        } else if (strcmp(sd[i].name, "msg_menu_sort_kind") == 0) {
            kind = 0xAC;
        } else if (strcmp(sd[i].name, "msg_menu_sort_bp") == 0) {
            kind = 0xAA;
        } else if (strcmp(sd[i].name, "msg_menu_sort_number") == 0) {
            kind = 0xAD;
        } else if (strcmp(sd[i].name, "msg_menu_sort_place") == 0) {
            kind = 0xAE;
        } else {
            kind = 0xAA;
        }
        if (i == wp->sortSel) {
            winTexInit(wp->res->tex->tpl);
            yy = (y - 20.0f) - (f32)(i * 40) - 16.0f - 8.0f;
            winTexSet(0xB6, (Vec){ 16.0f + (3.0f + (64.0f + x)), yy, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f },
                      (GXColor){ 0x00, 0x00, 0x00, 0x80 });
            winTexInit_x2(wp->res->tex->tpl);
            winTexSet_x2(kind, 0xB6, (Vec){ 4.0f + (16.0f + (64.0f + x)) - 6.0f, 6.0f + yy, 0.0f },
                         (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
        } else {
            winTexInit_x2(wp->res->tex->tpl);
            winTexSet_x2(kind, 0xB6,
                         (Vec){ 4.0f + (16.0f + (64.0f + x)), (y - 20.0f) - (f32)(i * 40) - 16.0f - 8.0f, 0.0f },
                         (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0x80, 0x80, 0x80, 0xFF });
        }
    }

    if (wp->sortType == 7) {
        GXSetScissor(0, 0, 0x260, 0x1E0);
    }
}

void winSortEntry(WinPauseWork* wp, s32 type, f32 x, f32 y) {
    WinSortData* sd = sort_data[type];
    s32 n;

    n = 0;
    while (sd->name != NULL) {
        sd++;
        n++;
    }
    wp->sortType = type;
    wp->sortNum = n;
    wp->sortSel = 0;
    wp->sortRev = 0;
    wp->sortPosX = -1000.0f;
    wp->sortTgtX = x;
    wp->sortPosY = y;
    wp->sortTgtY = y;
    wp->sortActive = 1;
}

BOOL winSortWait(WinPauseWork* wp) {
    return wp->sortActive != 0;
}

void winKageGX(WinPauseWork* wp, GXColor color, f32 x, f32 y, f32 z, f32 s) {
    Mtx out;
    Mtx trans;
    Mtx scale;
    Mtx rot;
    GXTexObj obj;
    f32 sy;

    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetZCompLoc(GX_TRUE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    TEXGetGXTexObjFromPalette(wp->res->tex->tpl, &obj, 0x7A);
    GXInitTexObjLOD(&obj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&obj, GX_TEXMAP0);
    GXSetTevColor(GX_TEVREG0, color);
    PSMTXTrans(trans, x, y, z);
    sy = s * (2.0f * GXGetTexObjHeight(&obj));
    PSMTXScale(scale, s * (2.0f * GXGetTexObjWidth(&obj)), sy, 1.0f);
    PSMTXRotRad(rot, 'x', -1.3962634f);
    PSMTXConcat(trans, rot, out);
    PSMTXConcat(out, scale, out);
    PSMTXConcat(camGetPtr((CameraId)8)->view, out, out);
    GXLoadPosMtxImm(out, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(-0.5f, 0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(-0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(0.5f, 0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

void winZClear(void) {
    Mtx mtx;

    PSMTXTrans(mtx, 0.0f, 0.0f, -1000.0f);
    PSMTXConcat(camGetCurPtr()->view, mtx, mtx);
    GXLoadPosMtxImm(mtx, 0);
    GXSetCurrentMtx(0);
    GXSetAlphaUpdate(GX_FALSE);
    GXSetColorUpdate(GX_FALSE);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_CLAMP, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetNumTexGens(0);
    GXSetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
    GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetZCompLoc(GX_TRUE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(-500.0f, 300.0f, 0.0f);
    GXPosition3f32(0.0f, 300.0f, 0.0f);
    GXPosition3f32(0.0f, -100.0f, 0.0f);
    GXPosition3f32(-500.0f, -100.0f, 0.0f);
    GXSetColorUpdate(GX_TRUE);
}

typedef struct WinMailData {
    u16 flags;        // 0x0
    u8 pad_2[2];      // 0x2
    s32 id;           // 0x4
    const char* name; // 0x8
    const char* body; // 0xC
    s32 icon;         // 0x10
} WinMailData;

#define WM_F(t, o) (*(t*)((u8*)wp + (o)))
#define WM_MODE WM_F(s32, 0x20C)
#define WM_X WM_F(f32, 0x244)
#define WM_Y WM_F(f32, 0x24C)
#define WM_SCR WM_F(f32, 0x260)
#define WM_LINE WM_F(s32, 0x268)
#define WM_LINES WM_F(u16, 0x26C)
#define WM_RES2 WM_F(WinRes*, 0x2C)

void winMailDisp(WinPauseWork* wp) {
    s32 i;
    s32 id;
    s32 kind;
    GXColor col;
    WinMailData* mail;

    if (WM_X < 320.0f && WM_Y > -240.0f) {
        ((void (*)(WinPauseWork*, f32, f32))winMailGX)(wp, WM_X, WM_Y);
        if (WM_MODE != 0xCA) {
            for (i = 0; i < wp->mailNum; i++) {
                GXColor white;

                id = wp->mailList[i].id;
                kind = wp->mailList[i].kind;
                mail = (WinMailData*)mailGetPtr(id);
                {
                GXColor black = { 0x00, 0x00, 0x00, 0xFF };
                if (mail->flags & 1) {
                    col = (GXColor){ 0xD7, 0xAC, 0xBC, 0xFF };
                } else if (mail->flags & 2) {
                    col = (GXColor){ 0xD3, 0xD7, 0xAC, 0xFF };
                } else if (mail->flags & 4) {
                    col = (GXColor){ 0xAC, 0xC2, 0xD7, 0xFF };
                } else {
                    col = (GXColor){ 0xE1, 0xE1, 0xE1, 0xFF };
                }
                winTexInit(wp->res->tex->tpl);
                winTexSet(0x77, (Vec){ 4.0f + (208.0f + WM_X), WM_SCR + ((WM_Y - 56.0f) - (f32)(i * 40)), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, col);
                white = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
                winTexSet((u32)((s32)pouchCheckMail(id) == 2 ? (void*)0x5E : (void*)0x5D), (Vec){ 72.0f + WM_X, WM_SCR + (((WM_Y - 56.0f) - (f32)(i * 40)) - 3.0f), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, white);
                winTexSet(0x65 + (kind + 1) / 10, (Vec){ 2.0f + (88.0f + WM_X), WM_SCR + (((WM_Y - 56.0f) - (f32)(i * 40)) - 3.0f), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, white);
                winTexSet(0x65 + (kind + 1) % 10, (Vec){ 2.0f + (104.0f + WM_X), WM_SCR + (((WM_Y - 56.0f) - (f32)(i * 40)) - 3.0f), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, white);
                winFontInit();
                winFontSetLabel((Vec){ 120.0f + WM_X, WM_SCR + (8.0f + ((WM_Y - 56.0f) - (f32)(i * 40))), 0.0f }, (Vec){ 0.75f, 0.75f, 0.75f }, black, mail->name);
                }
            }
            GXSetScissor(0, 0, 0x260, 0x1E0);
        } else {
            s32 kind2 = wp->mailList[wp->mailSel].kind;
            GXColor white = { 0xFF, 0xFF, 0xFF, 0xFF };
            GXColor black = { 0x00, 0x00, 0x00, 0xFF };
            GXColor col;

            mail = (WinMailData*)mailGetPtr(wp->mailList[wp->mailSel].id);
            if (mail->flags & 1) {
                col = (GXColor){ 0xD7, 0xAC, 0xBC, 0xFF };
            } else if (mail->flags & 2) {
                col = (GXColor){ 0xD3, 0xD7, 0xAC, 0xFF };
            } else if (mail->flags & 4) {
                col = (GXColor){ 0xAC, 0xC2, 0xD7, 0xFF };
            } else {
                col = (GXColor){ 0xE1, 0xE1, 0xE1, 0xFF };
            }
            winTexInit(wp->res->tex->tpl);
            winTexSet(0x77, (Vec){ 4.0f + (208.0f + WM_X), WM_Y - 56.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, col);
            winTexSet(0x5E, (Vec){ 72.0f + WM_X, WM_Y - 56.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, white);
            winTexSet(0x65 + (kind2 + 1) / 10, (Vec){ 2.0f + (88.0f + WM_X), (WM_Y - 56.0f) - 3.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, white);
            winTexSet(0x65 + (kind2 + 1) % 10, (Vec){ 2.0f + (104.0f + WM_X), (WM_Y - 56.0f) - 3.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, white);
            winFontInit();
            winFontSetLabel((Vec){ 120.0f + WM_X, 8.0f + (WM_Y - 56.0f), 0.0f }, (Vec){ 0.75f, 0.75f, 0.75f }, black, mail->name);
            GXSetScissor((u32)(32.0f + (304.0f + WM_X)), (u32)(72.0f + (240.0f - WM_Y)), 0x158, 0xB1);
            winTexInit(wp->res->tex->tpl);
            winTexSet(0x77, (Vec){ 4.0f + (208.0f + WM_X), (WM_Y - 56.0f) - 80.0f, 0.0f }, (Vec){ 1.0f, 7.5f, 1.0f }, col);
            winFontInit();
            winFontSetLabel((Vec){ 80.0f + WM_X, (WM_Y - 80.0f) + (f32)(WM_LINE * 21), 0.0f }, (Vec){ 0.75f, 0.75f, 0.75f }, (GXColor){ 0x00, 0x00, 0x00, 0xFF }, mail->body);
            if (mail->flags & 0x1000) {
                winTexInit(WM_RES2->tex->tpl);
                winTexSet(mail->icon, (Vec){ 210.0f + WM_X, 7.0f + ((WM_Y - (f32)((WM_LINES - 1) * 21)) + (f32)(WM_LINE * 21)), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
            }
            GXSetScissor(0, 0, 0x260, 0x1E0);
            if (WM_LINES > 8) {
                winIconInit();
                if (WM_LINE == 0) {
                    if ((gp->retraceCount & 0x1F) < 0x14) {
                        winIconSet(0x1B6, (Vec){ 360.0f + WM_X, (20.0f + (WM_Y - 230.0f)) - 24.0f, 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
                    }
                } else if (WM_LINE + 6 == WM_LINES) {
                    if ((gp->retraceCount & 0x1F) < 0x14) {
                        winIconSet(0x1B5, (Vec){ 360.0f + WM_X, 24.0f + (20.0f + (WM_Y - 230.0f)), 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
                    }
                } else {
                    if ((gp->retraceCount & 0x1F) < 0x14) {
                        winIconSet(0x1B5, (Vec){ 360.0f + WM_X, 24.0f + (20.0f + (WM_Y - 230.0f)), 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
                        winIconSet(0x1B6, (Vec){ 360.0f + WM_X, (20.0f + (WM_Y - 230.0f)) - 24.0f, 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
                    }
                }
                winIconSet(0x70, (Vec){ 360.0f + WM_X, 20.0f + (WM_Y - 230.0f), 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
            }
        }
    }
}

typedef struct WinBgWork {
    u8 pad_0[0x34];
    void** tex; // 0x34
    s32 copied; // 0x38
} WinBgWork;

void unk_8014d5dc(s32 unused, WinBgWork* bg) {
    GXSetTexCopySrc(0, 0, gp->fbWidth, gp->efbHeight);
    GXSetTexCopyDst(gp->fbWidth, gp->efbHeight, GX_TF_RGB565, GX_FALSE);
    GXCopyTex(*bg->tex, GX_FALSE);
    GXTexModeSync();
    bg->copied = 1;
    N_mapDispOff();
    camDispOff((CameraId)4);
}

void unk_8014d338(s32 unused, WinBgWork* bg) {
    GXTexObj obj;
    f32 w;
    f32 h;

    if (bg->copied) {
        GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
        GXSetNumChans(0);
        GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
        GXSetNumTevStages(1);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        GXSetZCompLoc(GX_TRUE);
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
        GXLoadPosMtxImm(camGetPtr((CameraId)8)->view, 0);
        GXSetCurrentMtx(0);
        GXInitTexObj(&obj, *bg->tex, gp->fbWidth, gp->efbHeight, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
        GXLoadTexObj(&obj, GX_TEXMAP0);
        GXSetCullMode(GX_CULL_NONE);
        GXClearVtxDesc();
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        w = gp->fbWidth;
        h = gp->efbHeight;
        GXPosition3f32(-w / 2.0f, -h / 2.0f, 0.0f);
        GXTexCoord2f32(0.0f, 1.0f);
        GXPosition3f32(w / 2.0f, -h / 2.0f, 0.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(w / 2.0f, h / 2.0f, 0.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(-w / 2.0f, h / 2.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
    }
}
