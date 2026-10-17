#pragma optimize_for_size on
#pragma dont_inline on
#include "battle/battle_menu_disp.h"
#include "battle/battle_seq_command.h"
#include "battle/battle_unit.h"
#include "system.h"
#include "battle/battle_database_common.h"
#include "data/item_data.h"
#include "driver/msgdrv.h"
#include "driver/windowdrv.h"
#include "manager/fontmgr.h"

extern BattleWork* _battleWorkPointer;
void FontDrawString(f32 x, f32 y, const char* msg);
void FontDrawScale(f32 scale);
void FontDrawColorIDX(s32 idx);
extern ItemData itemDataTable[];
extern BattleWeapon ItemWeaponData_CookingItem;
s32 BtlUnit_GetWeaponCost(BattleUnit* unit, BattleWeapon* weapon);
extern int sprintf(char* str, const char* format, ...);
double sin(double);
double cos(double);
void btlGetScreenPoint(Vec* pos, Vec* out);
void btlDispTexPlane2(Mtx mtx, u32 id, GXColor color, s32 a);
void btlDispGXInit2D(void);

typedef struct BattleSubMenu {
    BattleCursor* cursor;           // 0x0
    BattleCommandAct* acts;         // 0x4
    BattleCommandWeapon* weapons;   // 0x8
    s32 field_C;                    // 0xC
    s32 field_10;                   // 0x10
    u8 field_14;                    // 0x14
    u8 pad15[3];                    // 0x15
    s32 field_18;                   // 0x18
    s32 field_1C;                   // 0x1C
    s32 field_20[5];                // 0x20
    s32 field_34[5];                // 0x34
    s32 field_48[15];               // 0x48
    s32 field_84[5];                // 0x84
    f32 field_98;                   // 0x98
    f32 field_9C;                   // 0x9C
    f32 field_A0;                   // 0xA0
    s32 field_A4[7];                // 0xA4
    s32 field_C0[7];                // 0xC0
    s32 field_DC[7];                // 0xDC
    s32 field_F8[7];                // 0xF8
    u8 field_114[8];
    s32 field_11C; // 0x11C
    f32 field_120; // 0x120
} BattleSubMenu;

#define WP_SUBMENU(wp) (*(BattleSubMenu**)&(wp)->commandMenu.field_55C)

void DrawMultiItemWin(void);
void DrawSubMenuCommonProcess(f32* x, f32* y, void* a, void* b, void* c, void* d);
void DrawMainIcon(BattleSubMenu* menu, BattleCommandAct* acts, void* a, void* b, void* c);
void DrawSubIcon(BattleSubMenu* menu, void* a, s32* enabled, s16* icons, const char** names, s32* costs, s32* p34, void* party, s32 y);
void DrawMenuCursorAndScrollArrow(Vec* v);
void DrawMenuHelpWin(const char* help, f32 x, f32 y);
void DrawChangePartyWin(void);
void DrawOperationWin(void);
void DrawWeaponWin(void);
void DrawMainMenu(BattleWork* wp);
void DrawMenuPinchMark(void);
#include "battle/battle_audience.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "pmario_sound.h"
void DrawSubIconSub(BattleSubMenu* menu, s32 i, Mtx m, s32 enabled, s32 icon, const char* name, s32 cost, s32 p34, void* party, s32 y);
void DrawMainIconCircle(BattleSubMenu* menu, BattleCommandAct* act, Mtx m, Vec v, f32 s);
void DrawMainIconHukidasi(BattleSubMenu* menu, BattleCommandAct* act, Mtx m, f32 s);
BOOL BattleCommandCheckChangePositionEnable(BattleWork* wp);
void DrawMenuPartyChangeButton_Sub(u32 id, f32 x, f32 y, f32 angle);
void DrawMenuPartyChangeButton(void);
void DrawSubMenuCommonProcessSub1(BattleSubMenu* menu, BattleCursor* cursor, s32 prev, s32 key);
void btlDispGXQuads2D(f32 x0, f32 y0, f32 x1, f32 y1, u8 r, u8 g, u8 b, u8 a);
#include "mario/mariost.h"
#include "sdk/texPalette.h"
extern GlobalWork* gp;
void FontDrawStringCenterMtx(Mtx mtx, const char* msg);
void btlDispGXPoint2D(f32 x, f32 y, u8 r, u8 g, u8 b, u8 a, f32 s, f32 t);
void FontDrawColor(GXColor color);
void GetRingCenter(Vec* out);
void GetRingOffset(f32 angle, f32* x, f32* y);
f32 kakutable[3][5] = {{240.0f, 120.0f, 0.0f, 0.0f, 0.0f}, {270.0f, 180.0f, 90.0f, 0.0f, 0.0f}, {270.0f, 203.0f, 157.0f, 90.0f, 0.0f}};

//.sdata
GXColor seleItemCoordCol = {0xFF, 0xC0, 0xC0, 0xCF};

void SelectedItemCoordinateColorUpDate(void) {
    static s8 seq;
    switch (seq) {
        case 0:
            if (++seleItemCoordCol.g == 0xFF) {
                seq = 1;
            }
            break;
        case 1:
            if (--seleItemCoordCol.r == 0xC0) {
                seq = 2;
            }
            break;
        case 2:
            if (++seleItemCoordCol.b == 0xFF) {
                seq = 3;
            }
            break;
        case 3:
            if (--seleItemCoordCol.g == 0xC0) {
                seq = 4;
            }
            break;
        case 4:
            if (++seleItemCoordCol.r == 0xFF) {
                seq = 5;
            }
            break;
        case 5:
            if (--seleItemCoordCol.b == 0xC0) {
                seq = 0;
            }
            break;
    }
}

BOOL BattleMenuKeyOKInACT(BattleWork* wp) {
    if (!wp->commandMenu.unk540[0x18] && !WP_SUBMENU(wp)->field_14) {
        return FALSE;
    }
    return TRUE;
}

void battleMenuDispInit(void) {
    BattleWork* wp = _battleWorkPointer;
    char path[256];

    sprintf(path, "%s/battle/common/battle_common.tpl\0", getMarioStDvdRoot());
    wp->menuTex = fileAlloc(path, 4);
}

void battleMenuDispEnd(void) {
    fileFree(_battleWorkPointer->menuTex);
}

void GetRingCenter(Vec* out) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);

    btlGetScreenPoint((Vec*)((u8*)unit + 0x3C), out);
    out->x += 22.5f;
    out->y += 105.0f;
    out->z = 0.0f;
    if (out->x <= -180.0f) {
        out->x = -180.0f;
    }
}

void GetRingOffset(f32 angle, f32* x, f32* y) {
    f32 t = 4.712389f - angle;
    *x = 80.0f * (f32)cos(t);
    *y = 35.0f * (f32)sin(t);
}

void DrawMainIcon(BattleSubMenu* menu, BattleCommandAct* acts, void* a, void* b, void* c) {
    s32 order[5];
    f32 z[6];
    BattleWork* wp = _battleWorkPointer;
    BattleCursor* cursor;
    f32 tf;
    s32 ti;
    s32 i;
    s32 j;
    s32 k;

    if (BattleGetUnitPtr(wp, wp->activeUnitId)->currentKind == 0xDE) {
        cursor = &wp->commandMenu.cursor[0];
    } else {
        cursor = &wp->commandMenu.cursor[1];
    }
    for (i = 0; i < cursor->unk8; i++) {
        order[i] = i;
        z[i] = ((Vec*)b)[i].y;
    }
    for (i = 0; i < cursor->unk8 - 1; i++) {
        for (j = i + 1; j < cursor->unk8; j++) {
            if (z[i] < z[j]) {
                tf = z[i];
                ti = order[i];
                z[i] = z[j];
                order[i] = order[j];
                z[j] = tf;
                order[j] = ti;
            }
        }
    }
    for (i = 0; i < cursor->unk8; i++) {
        k = order[i];
        if (menu->field_20[k] & 1) {
            if (menu->field_20[k] & 2) {
                DrawMainIconHukidasi(menu, &acts[k], ((Mtx*)a)[k], ((f32*)c)[k]);
            } else {
                DrawMainIconCircle(menu, &acts[k], ((Mtx*)a)[k], ((Vec*)b)[k], ((f32*)c)[k]);
            }
        }
    }
}

void DrawMainIconHukidasi(BattleSubMenu* menu, BattleCommandAct* act, Mtx m, f32 s) {
    GXColor tev;
    GXColor col;
    Mtx rot;
    Mtx scl;
    Mtx trn;
    Mtx out;
    GXTexObj texobj;
    BattleWork* wp = _battleWorkPointer;
    CameraEntry* cam;
    const char* name;
    u16 icon;
    f32 rad;
    f32 ofs;

    cam = camGetPtr(8);
    btlDispGXInit2D();
    tev = (GXColor){0xE1, 0xE1, 0xE1, 0xFF};
    GXSetTevColor(GX_TEVREG0, tev);
    GXSetNumTevStages(1);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_C0, GX_CC_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    PSMTXScale(scl, 1.0f, 1.0f, 1.0f);
    PSMTXTrans(trn, 0.0f, -5.0f, 0.0f);
    rad = 0.017453292f * s;
    PSMTXRotRad(rot, 'y', rad);
    PSMTXConcat(m, rot, out);
    PSMTXConcat(out, scl, out);
    PSMTXConcat(out, trn, out);
    PSMTXConcat(cam->view, out, out);
    GXLoadPosMtxImm(out, 0);
    TEXGetGXTexObjFromPalette(*wp->menuTex->data, &texobj, 1);
    GXLoadTexObj(&texobj, GX_TEXMAP0);
    if (s >= 0.0f && s <= 180.0f) {
        ofs = 4.0f - 8.0f * s / 180.0f;
    }
    if (s >= 180.0f && s <= 360.0f) {
        ofs = -4.0f + 8.0f * (s - 180.0f) / 180.0f;
    }
    if (menu->field_1C == 0) {
        btlDispGXQuads2D(64.0f + ofs, 76.0f, -64.0f + ofs, -4.0f, 0, 0, 0, 0x80);
        btlDispGXQuads2D(64.0f, 80.0f, -64.0f, 0.0f, 0xFF, 0xFF, 0xFF, 0xFF);
    } else {
        btlDispGXQuads2D(-64.0f + ofs, 76.0f, 64.0f + ofs, -4.0f, 0, 0, 0, 0x80);
        btlDispGXQuads2D(-64.0f, 80.0f, 64.0f, 0.0f, 0xFF, 0xFF, 0xFF, 0xFF);
    }
    PSMTXTrans(trn, -67.0f, 20.0f, 0.0f);
    PSMTXScale(scl, 1.0f, 1.0f, 1.0f);
    PSMTXRotRad(rot, 'y', rad);
    PSMTXConcat(m, rot, out);
    PSMTXConcat(out, trn, out);
    PSMTXConcat(out, scl, out);
    icon = *(u16*)&act->icon;
    if (!icon) {
        icon = 0x18B;
    }
    if (act->enabled == 1) {
        ((void (*)(Mtx, s16, u16))iconDispGx2)(out, 0x10, icon);
    } else {
        ((void (*)(Mtx, s16, u16))iconDispGx2)(out, 0x30, icon);
    }
    name = act->name;
    PSMTXTrans(trn, 0.0f, 52.5f, 0.0f);
    PSMTXScale(scl, 0.9f, 0.9f, 0.9f);
    PSMTXRotRad(rot, 'y', rad);
    PSMTXConcat(m, rot, out);
    PSMTXConcat(out, trn, out);
    PSMTXConcat(out, scl, out);
    FontDrawStart();
    if (act->enabled == 1) {
        col = (GXColor){0x00, 0x00, 0x00, 0xFF};
    } else {
        col = (GXColor){0x80, 0x80, 0x80, 0xFF};
    }
    FontDrawColor(col);
    FontDrawStringCenterMtx(out, name);
}

void DrawMainIconCircle(BattleSubMenu* menu, BattleCommandAct* act, Mtx m, Vec v, f32 s) {
    GXColor tev;
    Mtx rot;
    Mtx scl;
    Mtx trn;
    Mtx out;
    GXTexObj texobj;
    BattleWork* wp = _battleWorkPointer;
    CameraEntry* cam;
    u8 c;
    u32 icon;
    f32 rad;
    f32 ofs;
    f32 sc;
    s32 i;
    s32 j;

    cam = camGetPtr(8);
    c = 0xC0 - (s32)(1.5f * v.z);
    btlDispGXInit2D();
    tev = (GXColor){0xE1, 0xE1, 0xE1, 0xFF};
    GXSetTevColor(GX_TEVREG0, tev);
    GXSetNumTevStages(1);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_C0, GX_CC_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    sc = 1.3f - 0.005f * v.z;
    PSMTXScale(scl, sc, sc, 1.0f);
    rad = 0.017453292f * s;
    PSMTXRotRad(rot, 'y', rad);
    PSMTXConcat(m, rot, out);
    PSMTXConcat(out, scl, out);
    PSMTXConcat(cam->view, out, out);
    GXLoadPosMtxImm(out, 0);
    TEXGetGXTexObjFromPalette(*wp->menuTex->data, &texobj, 0);
    GXLoadTexObj(&texobj, GX_TEXMAP0);
    if (s >= 0.0f && s <= 180.0f) {
        ofs = (4.0f - 8.0f * s / 180.0f) / 1.3f;
    }
    if (s >= 180.0f && s <= 360.0f) {
        ofs = (-4.0f + 8.0f * (s - 180.0f) / 180.0f) / 1.3f;
    }
    {
    f32 pos[4][8] = {{-20.0f, 40.0f, 0.0f, 40.0f, 0.0f, 20.0f, -20.0f, 20.0f}, {20.0f, 40.0f, 20.0f, 20.0f, 0.0f, 20.0f, 0.0f, 40.0f}, {-20.0f, 20.0f, 0.0f, 20.0f, 0.0f, 0.0f, -20.0f, 0.0f}, {0.0f, 20.0f, 20.0f, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f}};
    f32 uv[4][8] = {{0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f}};
    for (i = 0; i < 4; i++) {
        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        for (j = 0; j < 7; j += 2) {
            btlDispGXPoint2D(ofs + pos[i][j], -4.0f / 1.3f + pos[i][j + 1], 0, 0, 0, 0x80, uv[i][j], uv[i][j + 1]);
        }
    }
    for (i = 0; i < 4; i++) {
        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        for (j = 0; j < 7; j += 2) {
            btlDispGXPoint2D(pos[i][j], pos[i][j + 1], c, c, c, 0xFF, uv[i][j], uv[i][j + 1]);
        }
    }
    }
    PSMTXTrans(trn, 0.0f, 7.5f, 0.0f);
    sc = 0.66f * (1.3f - 0.005f * v.z);
    PSMTXScale(scl, sc, sc, 1.0f);
    PSMTXRotRad(rot, 'y', rad);
    PSMTXIdentity(rot);
    PSMTXConcat(m, rot, out);
    PSMTXConcat(out, scl, out);
    PSMTXConcat(out, trn, out);
    icon = *(u16*)&act->icon;
    if (!icon) {
        icon = 0x18B;
    }
    if (act->enabled == 1) {
        iconDispGxCol(out, 0x10, icon, (GXColor){c, c, c, 0xFF});




    } else {
        iconDispGxCol(out, 0x30, icon, (GXColor){c, c, c, 0xFF});




    }
}

void DrawSubIcon(BattleSubMenu* menu, void* a, s32* enabled, s16* icons, const char** names, s32* costs, s32* p34, void* party, s32 y) {
    Mtx out[7];
    Mtx m;
    BattleWork* wp = _battleWorkPointer;
    BattleCursor* cursor;
    f32 ofs;
    s32 i;

    BattleGetUnitPtr(wp, wp->activeUnitId);
    cursor = menu->cursor;
    ofs = menu->field_98 - (f32)(s32)menu->field_98; ofs = 24.0f * ofs;
    for (i = 0; i < 7; i++) {
        if ((s32)menu->field_98 + i < cursor->unk8 && (menu->field_A4[i] & 1)) {
            PSMTXTrans(m, 100.0f, 35.0f - 24.0f * i + ofs, 0.0f);
            PSMTXConcat(m, ((Mtx*)a)[i], out[i]);
            DrawSubIconSub(menu, i, out[i], enabled[i], ((u16*)icons)[i], names[i], costs[i], p34[i], party, y);
        }
    }
}

void DrawSubIconSub(BattleSubMenu* menu, s32 i, Mtx m, s32 enabled, s32 icon, const char* name, s32 cost, s32 p34, void* party, s32 y) {
    BattleWork* wp = _battleWorkPointer;
    CameraEntry* cam;
    BattleCursor* cursor = menu->cursor;
    f32 frac;
    u8 alpha;
    u8 r; u8 g; u8 b; u8 a;
    Mtx scl;
    Mtx trn;
    Mtx out;
    GXTexObj texobj;
    s32 d1[3];
    s32 o1[3];
    s32 d2[3];
    s32 o2[3];
    s32 d3[3];
    s32 o3[3];
    char buf1[2];
    char buf2[2];
    char buf3[2];
    s32 j;
    s32 n;

    cam = camGetPtr(8);
    frac = menu->field_98 - (( s32)menu->field_98) ;
    alpha = 0xFF;
    if (i == 0) {
        alpha = 255.0f * (1.0f - frac);
    }
    if (i == 6) {
        alpha = 255.0f * frac;
    }
    j = ((s32)menu->field_98 ) + i ; if (j == cursor->field_0) {
        r = seleItemCoordCol.r;
        g = seleItemCoordCol.g;
        b = seleItemCoordCol.b;
        a = alpha;
    } else {
        b = 0xFF;
        g = 0xFF;
        r = 0xFF;
        a = ( 144.0f * ( (u8)alpha )) / 255.0f;
    }
    btlDispGXInit2D();
    PSMTXConcat(cam->view, m, out);
    GXLoadPosMtxImm(out, 0);
    TEXGetGXTexObjFromPalette(*wp->menuTex->data, &texobj, 2);
    GXLoadTexObj(&texobj, GX_TEXMAP0);
    btlDispGXQuads2D(-152.0f, 24.0f, -136.0f, 0.0f, r, g, b, a);
    btlDispGXQuads2D(152.0f, 24.0f, 136.0f, 0.0f, r, g, b, a);
    TEXGetGXTexObjFromPalette(*wp->menuTex->data, &texobj, 3);
    GXLoadTexObj(&texobj, GX_TEXMAP0);
    btlDispGXQuads2D(-136.0f, 24.0f, 136.0f, 0.0f, r, g, b, a);
    PSMTXTrans(trn, -119.0f, 2.0f, 0.0f);
    PSMTXScale(scl, 0.5f, 0.5f, 1.0f);
    PSMTXConcat(m, trn, out);
    PSMTXConcat(out, scl, out);
    if (!(( u16)icon) ) {
        icon = 0x18B;
    }
    if (enabled == 1) {
        iconDispGxCol(out, 0x10, icon, (GXColor){0xFF, 0xFF, 0xFF, alpha});
    } else {
        iconDispGxCol(out, 0x30, icon, (GXColor){0xFF, 0xFF, 0xFF, alpha});
    }
    FontDrawStart();
    {
        GXColor fc;
        if (enabled == 1) {
            fc = (GXColor){0x00, 0x00, 0x00, alpha};
        } else {
            fc = (GXColor){0x40, 0x40, 0x40, alpha};
        }
        FontDrawColor(fc);
    }
    PSMTXTrans(trn, -104.0f, 22.0f, 0.0f);
    if (y == 1) {
        PSMTXScale(scl, ( FontGetMessageWidth(name) > 150 ) ? ( 120.0f / FontGetMessageWidth(name) ) : ( 0.8f) , 0.8f, 0.8f);
    } else {
        PSMTXScale(scl, 0.8f, 0.8f, 0.8f);
    }
    PSMTXConcat(m, trn, out);
    PSMTXConcat(out, scl, out);
    FontDrawStringMtx(out, name);
    if (cost != 0) {
        d1[2] = cost / 100;
        d1[1] = (cost - ( d1[2] * 100) ) / 10;
        d1[0] = (cost - ( d1[2] * 100) ) - ( d1[1] * 10) ;
        if (d1[2] > 0) {
            o1[0] = 28; o1[1] = 14; o1[2] = 0; n = 3;
        } else if (d1[1] > 0) {
            o1[0] = 21; o1[1] = 7; o1[2] = 0; n = 2;
        } else {
            o1[0] = 14; o1[1] = 0; o1[2] = 0; n = 1;
        }
        PSMTXScale(scl, 1.0f, 1.0f, 1.0f);
        for (j = 0; j < n; j++) {
            buf1[0] = d1[j] + '0';
            buf1[1] = 0;
            PSMTXTrans(trn, 4.0f + (72.0f + o1[j]), 25.0f, 0.0f);
            PSMTXConcat(m, trn, out);
            PSMTXConcat(out, scl, out);
            FontDrawStringMtx(out, buf1);
        }
        PSMTXTrans(trn, 120.0f, 22.0f, 0.0f);
        PSMTXScale(scl, 0.8f, 0.8f, 0.8f);
        PSMTXConcat(m, trn, out);
        PSMTXConcat(out, scl, out);
        FontDrawStringMtx(out, msgSearch("btl_cost_disp_FP"));
    }
    if (p34 != 0) {
        d2[2] = p34 / 100;
        d2[1] = (p34 - ( d2[2] * 100) ) / 10;
        d2[0] = (p34 - ( d2[2] * 100) ) - ( d2[1] * 10) ;
        if (d2[2] > 0) {
            o2[0] = 28; o2[1] = 14; o2[2] = 0; n = 3;
        } else if (d2[1] > 0) {
            o2[0] = 21; o2[1] = 7; o2[2] = 0; n = 2;
        } else {
            o2[0] = 14; o2[1] = 0; o2[2] = 0; n = 1;
        }
        PSMTXScale(scl, 1.0f, 1.0f, 1.0f);
        for (j = 0; j < n; j++) {
            buf2[0] = d2[j] + '0';
            buf2[1] = 0;
            PSMTXTrans(trn, 4.0f + (72.0f + o2[j]), 25.0f, 0.0f);
            PSMTXConcat(m, trn, out);
            PSMTXConcat(out, scl, out);
            FontDrawStringMtx(out, buf2);
        }
        PSMTXTrans(trn, 120.0f, 22.0f, 0.0f);
        PSMTXScale(scl, 0.8f, 0.8f, 0.8f);
        PSMTXConcat(m, trn, out);
        PSMTXConcat(out, scl, out);
        FontDrawStringMtx(out, msgSearch("btl_cost_disp_AP"));
    }
    if (party != NULL) {
        s16* p = (s16*)((( u8*)party ) + ( (i + ( (s32)menu->field_98) ) * 0x18) );
        s32 n;
        s32 hp; s32 maxhp;
        hp = p[10];
        maxhp = p[11];
        PSMTXTrans(trn, 30.0f, 22.0f, 0.0f);
        PSMTXScale(scl, 0.8f, 0.8f, 0.8f);
        PSMTXConcat(m, trn, out);
        PSMTXConcat(out, scl, out);
        FontDrawStringMtx(out, msgSearch("btl_disp_HP"));
        d3[2] = hp / 100;
        d3[1] = (hp - ( d3[2] * 100) ) / 10;
        d3[0] = (hp - ( d3[2] * 100) ) - ( d3[1] * 10) ;
        if (d3[2] > 0) {
            o3[0] = 28; o3[1] = 14; o3[2] = 0; n = 3;
        } else if (d3[1] > 0) {
            o3[0] = 21; o3[1] = 7; o3[2] = 0; n = 2;
        } else {
            o3[0] = 14; o3[1] = 0; o3[2] = 0; n = 1;
        }
        PSMTXScale(scl, 1.0f, 1.0f, 1.0f);
        for (j = 0; j < n; j++) {
            buf3[0] = d3[j] + '0';
            buf3[1] = 0;
            if ((( u32)gp->language ) == 0) {
                PSMTXTrans(trn, 26.0f + (30.0f + o3[j]), 25.0f, 0.0f);
            } else {
                PSMTXTrans(trn, 28.0f + (30.0f + o3[j]), 25.0f, 0.0f);
            }
            PSMTXConcat(m, trn, out);
            PSMTXConcat(out, scl, out);
            FontDrawStringMtx(out, buf3);
        }
        if ((( u32)gp->language ) == 0) {
            PSMTXTrans(trn, 92.0f, 22.0f, 0.0f);
        } else {
            PSMTXTrans(trn, 100.0f, 22.0f, 0.0f);
        }
        PSMTXScale(scl, 0.8f, 0.8f, 0.8f);
        PSMTXConcat(m, trn, out);
        PSMTXConcat(out, scl, out);
        FontDrawStringMtx(out, msgSearch("btl_disp_slash"));
        d3[2] = maxhp / 100;
        d3[1] = (maxhp - ( d3[2] * 100) ) / 10;
        d3[0] = (maxhp - ( d3[2] * 100) ) - ( d3[1] * 10) ;
        if (d3[2] > 0) {
            o3[0] = 28; o3[1] = 14; o3[2] = 0; n = 3;
        } else if (d3[1] > 0) {
            o3[0] = 21; o3[1] = 7; o3[2] = 0; n = 2;
        } else {
            o3[0] = 14; o3[1] = 0; o3[2] = 0; n = 1;
        }
        PSMTXScale(scl, 1.0f, 1.0f, 1.0f);
        for (j = 0; j < n; j++) {
            buf3[0] = d3[j] + '0';
            buf3[1] = 0;
            if ((( u32)gp->language ) == 0) {
                PSMTXTrans(trn, 76.0f + (30.0f + o3[j]), 25.0f, 0.0f);
            } else {
                PSMTXTrans(trn, 78.0f + (30.0f + o3[j]), 25.0f, 0.0f);
            }
            PSMTXConcat(m, trn, out);
            PSMTXConcat(out, scl, out);
            FontDrawStringMtx(out, buf3);
        }
    }
}

void DrawMenuPartyChangeButton(void) {
    Vec p;
    Vec q;
    s32 id;
    f32 ix;
    f32 iy;
    f32 iz;
    BattleUnit* party;
    BattleUnit* mario;
    BattleWork* wp;
    BattleSubMenu* menu;
    f32 x;
    f32 y;

    wp = _battleWorkPointer;
    menu = WP_SUBMENU(wp);
    camGetPtr(8);
    mario = BattleGetMarioPtr(wp);
    party = BattleGetPartyPtr(wp);
    if (!party) {
        BattleAudience_GetItemOn(&id, &ix, &iy, &iz, NULL);
        if (id != -1) {
            if (menu->field_120 > 0.0f) {
                menu->field_120 -= 5.0f;
            } else {
                menu->field_120 = 0.0f;
            }
            btlGetScreenPoint((Vec*)((u8*)mario + 0x3C), &p);
            DrawMenuPartyChangeButton_Sub(5, p.x, p.y - 20.0f, menu->field_120);
        } else {
            menu->field_120 = 90.0f;
        }
    } else {
        btlGetScreenPoint((Vec*)((u8*)mario + 0x3C), &p);
        btlGetScreenPoint((Vec*)((u8*)party + 0x3C), &q);
        x = (p.x + q.x) / 2.0f;
        y = p.y;
        BattleAudience_GetItemOn(&id, &ix, &iy, &iz, NULL);
        if (id != -1) {
            if (menu->field_120 == 90.0f) {
                psndSFXOn("SFX_BTL_SYS_CAUTION_THROW1");
            }
            if (menu->field_120 > 0.0f) {
                menu->field_120 -= 5.0f;
            } else {
                menu->field_120 = 0.0f;
            }
            DrawMenuPartyChangeButton_Sub(5, x, y - 20.0f, menu->field_120);
        } else {
            menu->field_120 = 90.0f;
            if (BattleCommandCheckChangePositionEnable(wp)) {
                DrawMenuPartyChangeButton_Sub(4, x, y - 20.0f, 0.0f);
            }
        }
    }
}

void DrawMenuPartyChangeButton_Sub(u32 id, f32 x, f32 y, f32 angle) {
    Mtx mtx;

    PSMTXRotRad(mtx, 'y', 0.017453292f * angle);
    PSMTXTransApply(mtx, mtx, x, y, 0.0f);
    btlDispTexPlane2(mtx, id, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, 0);
}

void DrawMenuPinchMark(void) {
    Mtx m;
    Mtx out;
    GXTexObj texobj;
    Vec p;
    Vec q;
    BattleWork* wp = _battleWorkPointer;
    CameraEntry* cam;
    BattleUnit* unit;
    u8 r;
    u8 g;
    u8 b;
    u8 a;
    u32 id;

    cam = camGetPtr(8);
    btlDispGXInit2D();
    unit = BattleGetMarioPtr(wp);
    if (unit->currentHp > 0 && unit->currentHp < unit->maxHp && unit->currentHp <= 5) {
        if (unit->currentHp == 1) {
            id = (gp->language ? 0 : -1) + 9;
            if (gp->retraceCount % 50 <= 24) {
                r = 0xFF;
                g = 0xE9;
                b = 0x40;
                a = 0xFF;
            } else {
                r = 0xE9;
                g = 0x52;
                b = 0x2F;
                a = 0xFF;
            }
        } else {
            id = gp->language == 0 ? 6 : 7;
            r = 0xFF;
            g = 0xE9;
            b = 0x40;
            a = 0xFF;
        }
        TEXGetGXTexObjFromPalette(*wp->menuTex->data, &texobj, id);
        GXLoadTexObj(&texobj, GX_TEXMAP0);
        btlGetScreenPoint((Vec*)((u8*)unit + 0x3C), &p);
        PSMTXTrans(m, p.x - 20.0f, 60.0f + p.y, 0.0f);
        PSMTXConcat(cam->view, m, out);
        GXLoadPosMtxImm(out, 0);
        btlDispGXQuads2D(-28.0f, 12.0f, 28.0f, -12.0f, r, g, b, a);
    }
    unit = BattleGetPartyPtr(wp);
    if (unit && unit->currentHp > 0 && unit->currentHp < unit->maxHp && unit->currentHp <= 5) {
        if (unit->currentHp == 1) {
            id = (gp->language ? 0 : -1) + 9;
            if (gp->retraceCount % 50 <= 24) {
                r = 0xFF;
                g = 0xE9;
                b = 0x40;
                a = 0xFF;
            } else {
                r = 0xE9;
                g = 0x52;
                b = 0x2F;
                a = 0xFF;
            }
        } else {
            r = 0xFF;
            g = 0xE9;
            b = 0x40;
            a = 0xFF;
            id = (gp->language ? 0 : -1) + 7;
        }
        TEXGetGXTexObjFromPalette(*wp->menuTex->data, &texobj, id);
        GXLoadTexObj(&texobj, GX_TEXMAP0);
        btlGetScreenPoint((Vec*)((u8*)unit + 0x3C), &q);
        PSMTXTrans(m, q.x - 20.0f, 60.0f + q.y, 0.0f);
        PSMTXConcat(cam->view, m, out);
        GXLoadPosMtxImm(out, 0);
        btlDispGXQuads2D(-28.0f, 12.0f, 28.0f, -12.0f, r, g, b, a);
    }
}

void DrawMenuHelpWin(const char* msg, f32 x, f32 y) {
    f32 tx;
    s8 lines[4][0x30];
    s32 ofs;
    s8 c;
    const s8* p;
    s8* dst;
    s32 i;

    windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xCF}, x, y, 400.0f, 50.0f, 10.0f);
    FontDrawStart();
    FontDrawColorIDX(0);
    FontDrawScale(0.8f);
    msg = msgSearch(msg);
    if (msg) {
        for (i = 0; i < 4; i++) {
            lines[i][0] = 0;
        }
        p = (const s8*)msg;
        dst = lines[0];
        ofs = 0;
        while (1) {
            if (*p == '<') {
                while (*++p != '>') {
                }
                p++;
            }
            c = *p;
            if (!c) {
                *dst = 0;
                break;
            }
            if (*p == '\n') {
                *dst = 0;
                ofs += 0x30;
                dst = lines[0];
                dst += ofs;
                p++;
                continue;
            }
            *dst = c;
            p++;
            dst++;
        }
        if (lines[0][0]) {
            FontDrawScale(0.8f);
        }
        tx = 20.0f + x;
        FontDrawString(tx, y - 4.0f, (char*)lines[0]);
        if (lines[1][0]) {
            FontDrawScale(0.8f);
        }
        FontDrawString(tx, y - 28.0f, (char*)lines[1]);
        if (lines[2][0]) {
            FontDrawScale(0.8f);
        }
        FontDrawString(tx, y - 52.0f, (char*)lines[2]);
    } else {
        FontDrawString(10.0f + x, y - 4.0f, "\202\271\202\302\202\337\202\242\202\315\201@\202\310\202\242\202\314\202\276\201I\201I");
    }
}

void DrawMenuCursorAndScrollArrow(Vec* v) {
    Vec pos;
    BattleWork* wp = _battleWorkPointer;
    BattleSubMenu* menu = WP_SUBMENU(wp);
    BattleCursor* cursor = menu->cursor;
    f32 bob;
    s32 n;
    f32 ax = 155.0f;
    f32 ay = 15.0f;

    bob = intplGetValue(4, menu->field_11C, 6, 0.0f, 10.0f);
    menu->field_11C++;
    if (menu->field_11C >= 12) {
        menu->field_11C = 0;
    }
    if (cursor->field_4 > 0) {
        pos.y = ay + v->y;
        pos.z = 0.0f;
        pos.x = ax + v->x;
        pos.y = pos.y + bob;

        iconDispGx(&pos, 0x10, 0x1B5, 0.75f);
    }
    if (cursor->field_4 < cursor->unk8 - 6) {
        f32 x = 155.0f + v->x;
        pos.y = 20.0f + v->y - 164.0f - bob;
        pos.x = x;
        pos.z = 0.0f;
        iconDispGx(&pos, 0x10, 0x1B6, 0.75f);
    }
    n = cursor->field_0 - cursor->field_4;
    if (n < 0) {
        n = 0;
    }
    if (n >= 6) {
        n = 5;
    }
    {
        f32 y = v->y - (f32)n * 24.0f;
        pos.x = v->x;
        pos.z = 0.0f;
        pos.y = y;
    }
    iconDispGx(&pos, 0x14, 0x1F0, 1.0f);
}

void InitSubMenuCommonProcess(BattleSubMenu** out, BattleCursor** mainCursor, BattleCursor* cursor) {
    BattleWork* wp = _battleWorkPointer;
    s32 i;

    if (WP_SUBMENU(wp)) {
        BattleFree(WP_SUBMENU(wp));
    }
    WP_SUBMENU(wp) = BattleAlloc(sizeof(BattleSubMenu));
    *out = WP_SUBMENU(wp);
    (*out)->cursor = cursor;
    (*out)->field_C = 0;
    (*out)->field_10 = 0;
    (*out)->field_14 = 0;
    (*out)->field_18 = cursor->field_0;
    if (BattleGetUnitPtr(wp, wp->activeUnitId)->currentKind == 0xDE) {
        *mainCursor = &wp->commandMenu.cursor[0];
    } else {
        *mainCursor = &wp->commandMenu.cursor[1];
    }
    (*out)->field_98 = 0.0f;
    for (i = 0; i < 7; i++) {
        (*out)->field_A4[i] = 0;
        (*out)->field_C0[i] = 1;
        (*out)->field_DC[i] = -i;
        (*out)->field_F8[i] = 0;
    }
}

void InitSubMenuCommonProcess2(BattleSubMenu* menu, BattleCursor* cursor) {
    s32 i;
    s32 idx = cursor->field_0;

    for (i = 0; i < cursor->unk8; i++) {
        if (++idx >= cursor->unk8) {
            idx = 0;
        }
        menu->field_48[idx] = 0;
        menu->field_84[idx] = i;
        menu->field_20[idx] = 0;
        menu->field_20[idx] |= 1;
        if (i == cursor->unk8 - 1) {
            menu->field_34[idx] = 0;
        } else {
            menu->field_34[idx] = 10;
        }
    }
}

void InitSubMenuCommonProcess3(BattleSubMenu* menu, BattleCursor* cursor) {
    s32 i;
    s32 idx = cursor->field_0;

    for (i = 0; i < cursor->unk8; i++) {
        if (++idx >= cursor->unk8) {
            idx = 0;
        }
        menu->field_48[idx] = 0;
        menu->field_84[idx] = i;
        menu->field_20[idx] = 0;
        menu->field_20[idx] |= 1;
        if (i == cursor->unk8 - 1) {
            menu->field_34[idx] = 1;
        } else {
            menu->field_34[idx] = 11;
        }
    }
}

void DrawSubMenuCommonProcessSub1(BattleSubMenu* menu, BattleCursor* cursor, s32 prev, s32 key) {
    s32 i;

    if (key == 6 && cursor->unk8 > 6 && prev == cursor->unk8 - 6 && cursor->field_4 == 0 && cursor->field_0 != 1) {
        menu->field_C = 30;
        menu->field_10 = 0;
        menu->field_9C = cursor->unk8 - 6;
        menu->field_A0 = 0.0f;
        for (i = 0; i < 7; i++) {
            menu->field_C0[i] = 30;
            menu->field_DC[i] = 0;
        }
    } else if (key == 6 && cursor->unk8 > 6 && prev == 0 && cursor->field_4 == cursor->unk8 - 6 && cursor->field_0 != 5) {
        menu->field_C = 40;
        menu->field_10 = 0;
        menu->field_9C = 0.0f;
        menu->field_A0 = cursor->unk8 - 6;
        for (i = 0; i < 7; i++) {
            menu->field_C0[i] = 40;
            menu->field_DC[i] = 0;
        }
    } else if (cursor->field_4 == prev + 1) {
        menu->field_C = 10;
        menu->field_10 = 0;
        menu->field_9C = prev;
        menu->field_A0 = prev + 1;
        for (i = 0; i < 7; i++) {
            menu->field_C0[i] = 10;
            menu->field_DC[i] = 0;
        }
    } else if (cursor->field_4 == prev - 1) {
        menu->field_C = 20;
        menu->field_10 = 0;
        menu->field_9C = prev;
        menu->field_A0 = prev - 1;
        for (i = 0; i < 7; i++) {
            menu->field_C0[i] = 20;
            menu->field_DC[i] = 0;
        }
    }
}

void DrawSubMenuCommonProcess(f32* x, f32* y, void* a, void* b, void* c, void* d) {
    Mtx tm;
    Mtx sm;
    f32 ox;
    f32 oy;
    BattleWork* wp = _battleWorkPointer;
    BattleSubMenu* menu = WP_SUBMENU(wp);
    BattleCursor* cursor = menu->cursor;
    BattleCursor* mainCursor;
    s32 steps;
    s32 prev;
    s32 n;
    s32 cnt;
    s32 cnt2;
    s32 i;
    f32 ang;
    f32 s;
    s32 sw;
    s32 nn;
    s32 nn2;

    if (BattleGetUnitPtr(wp, wp->activeUnitId)->currentKind == 0xDE) {
        mainCursor = &wp->commandMenu.cursor[0];
    } else {
        mainCursor = &wp->commandMenu.cursor[1];
    }
    n = 6;
    if (cursor->unk8 < 6) {
        n = cursor->unk8;
    }
    prev = cursor->field_4;
    nn2 = n;
    {
        s32 p = cursor->field_4;
        s32 m = n - 1;
        if (cursor->field_0 <= p) {
            cursor->field_4 = cursor->field_0 - 1;
        } else if (cursor->field_0 >= p + m) {
            cursor->field_4 = cursor->field_0 - n + 2;
        }
    }
    if (cursor->field_4 < 0) {
        cursor->field_4 = 0;
    }
    if (cursor->field_4 >= cursor->unk8 - n) {
        cursor->field_4 = cursor->unk8 - n;
    }
    *x = -200.0f;
    *y = -100.0f;
    switch (sw = menu->field_C) {
        case 0:
            menu->field_14 = 1;
            menu->field_98 = cursor->field_4;
            cnt = 0;
            for (i = 0; i < mainCursor->unk8; i++) {
                if (menu->field_34[i] == 1 || menu->field_34[i] == 11) {
                    cnt++;
                }
            }
            cnt2 = 0;
            for (i = 0; i < 7; i++) {
                if (menu->field_C0[i] == 2) {
                    cnt2++;
                }
            }
            if (cnt == mainCursor->unk8 && cnt2 == 7) {
                menu->field_C = 2;
            }
            DrawSubMenuCommonProcessSub1(menu, cursor, prev, nn2);
            break;
        case 2:
            menu->field_14 = 1;
            menu->field_98 = cursor->field_4;
            DrawSubMenuCommonProcessSub1(menu, cursor, prev, nn2);
            break;
        case 10:
        case 20:
        case 30:
        case 40:
            if (sw == 10 || sw == 20) {
                menu->field_14 = 1;
            }
            cnt = 0;
            for (i = 0; i < 7; i++) {
                if (menu->field_C0[i] == 2) {
                    cnt++;
                }
            }
            if (cnt == 7) {
                menu->field_C = 2;
                menu->field_98 = cursor->field_4;
            }
            DrawSubMenuCommonProcessSub1(menu, cursor, prev, nn2);
            break;
    }
    menu->field_1C = 0;
    for (i = 0; i < mainCursor->unk8; i++) {
        switch (menu->field_34[i]) {
            case 0:
                menu->field_20[i] |= 1;
                menu->field_20[i] |= 2;
                GetRingCenter(&((Vec*)b)[i]);
                {
                    f32 px = ((Vec*)b)[i].x;
                    f32 py;
                    ((Vec*)b)[i].x = ((f32 (*)(s32, f32, f32, s32, s32))intplGetValue)(4, px, -60.0f, menu->field_48[i], 12);
                    py = ((Vec*)b)[i].y;
                    ((Vec*)b)[i].y = ((f32 (*)(s32, f32, f32, s32, s32))intplGetValue)(4, py, 55.0f, menu->field_48[i], 12);
                }
                PSMTXTrans(((Mtx*)a)[i], 20.0f + ((Vec*)b)[i].x, ((Vec*)b)[i].y, ((Vec*)b)[i].z);
                ((f32*)c)[i] = ((f32 (*)(s32, f32, f32, s32, s32))intplGetValue)(0, 0.0f, 360.0f, menu->field_48[i], 12);
                if (((f32*)c)[i] > 360.0f) {
                    ((f32*)c)[i] -= 360.0f;
                }
                if (menu->field_48[i] >= 3) {
                    menu->field_1C = 1;
                }
                menu->field_48[i]++;
                if (menu->field_48[i] > 12) {
                    menu->field_34[i] = 1;
                }
                break;
            case 1:
                menu->field_20[i] |= 1;
                menu->field_20[i] |= 2;
                PSMTXTrans(((Mtx*)a)[i], -40.0f, 55.0f, 0.0f);
                ((f32*)c)[i] = 360.0f;
                menu->field_1C = 1;
                break;
            case 10:
                menu->field_20[i] |= 1;
                menu->field_20[i] &= ~2;
                ang = 3.1415927f * kakutable[mainCursor->unk8 - 3][menu->field_84[i]] / 180.0f;
                GetRingCenter(&((Vec*)b)[i]);
                GetRingOffset(ang, &ox, &oy);
                ((Vec*)b)[i].x += ox;
                ((Vec*)b)[i].y += oy;
                ((Vec*)b)[i].z = oy;
                s = 1.0f - 0.1f * menu->field_48[i];
                if (s <= 0.0f) {
                    menu->field_34[i] = 11;
                }
                PSMTXScale(sm, s, s, 1.0f);
                PSMTXTrans(tm, ((Vec*)b)[i].x, ((Vec*)b)[i].y, ((Vec*)b)[i].z);
                PSMTXConcat(tm, sm, ((Mtx*)a)[i]);
                ((f32*)c)[i] = 0.0f;
                menu->field_48[i]++;
                break;
            case 11:
                menu->field_20[i] &= ~1;
                break;
        }
    }
    for (i = 0; i < 7; i++) {
        switch (sw = menu->field_C0[i]) {
            case 0:
                menu->field_A4[i] &= ~1;
                menu->field_114[i] = 0xFF;
                break;
            case 1:
                menu->field_A4[i] &= ~1;
                menu->field_114[i] = 0xFF;
                if (menu->field_DC[i] >= 0 && i != 6) {
                    s = ((f32 (*)(s32, f32, f32, s32, s32))intplGetValue)(0, 90.0f, 0.0f, menu->field_DC[i], 12);
                    menu->field_A4[i] |= 1;
                    PSMTXRotRad(((Mtx*)d)[i], 'y', 0.017453292f * s);
                }
                menu->field_DC[i]++;
                if (menu->field_DC[i] > 12) {
                    menu->field_C0[i] = 2;
                }
                break;
            case 2:
                menu->field_114[i] = 0xFF;
                if (i != 6) {
                    menu->field_A4[i] |= 1;
                    PSMTXIdentity(((Mtx*)d)[i]);
                } else {
                    menu->field_A4[i] &= ~1;
                }
                break;
            case 10:
            case 20:
            case 30:
            case 40:
                if (sw == 10 || sw == 20) {
                    steps = 12;
                }
                if (sw == 30 || sw == 40) {
                    steps = (cursor->unk8 - 6) / 2 + 12;
                }
                menu->field_A4[i] |= 1;
                PSMTXIdentity(((Mtx*)d)[i]);
                menu->field_98 = ((f32 (*)(s32, f32, f32, s32, s32))intplGetValue)(4, menu->field_9C, menu->field_A0, menu->field_DC[i], steps);
                menu->field_DC[i]++;
                if (menu->field_DC[i] > steps) {
                    menu->field_C0[i] = 2;
                    menu->field_DC[i] = 0;
                }
                break;
        }
    }
}

void BattleMenuDisp_ActSelect_Init(BattleWork* wp, BattleCursor* cursor, BattleCommandAct* acts, BOOL flag) {
    s32 i;
    s32 idx;
    BattleSubMenu* menu;

    if (WP_SUBMENU(wp)) {
        BattleFree(WP_SUBMENU(wp));
    }
    WP_SUBMENU(wp) = BattleAlloc(sizeof(BattleSubMenu));
    if (flag & 1) {
        menu = WP_SUBMENU(wp);
        menu->cursor = cursor;
        menu->acts = acts;
        menu->field_C = 10;
        menu->field_14 = 0;
        menu->field_18 = cursor->field_0;
        idx = cursor->field_0;
        for (i = 0; i < cursor->unk8; i++) {
            if (idx == cursor->field_0) {
                menu->field_34[idx] = 10;
            } else {
                menu->field_34[idx] = 11;
            }
            menu->field_48[idx] = 0;
            menu->field_84[idx] = cursor->unk8 - 1 + i;
            if (menu->field_84[idx] >= cursor->unk8) {
                menu->field_84[idx] -= cursor->unk8;
            }
            menu->field_20[idx] = 0;
            if (++idx >= cursor->unk8) {
                idx = 0;
            }
        }
    } else {
        menu = WP_SUBMENU(wp);
        menu->cursor = cursor;
        menu->acts = acts;
        menu->field_C = 0;
        menu->field_14 = 0;
        menu->field_18 = cursor->field_0;
        idx = cursor->field_0;
        for (i = 0; i < cursor->unk8; i++) {
            if (++idx >= cursor->unk8) {
                idx = 0;
            }
            menu->field_34[idx] = 0;
            menu->field_48[idx] = (i * 10) / 2;
            menu->field_84[idx] = i;
            menu->field_20[idx] = 0;
        }
    }
}

void BattleMenuDisp_ActSelect_Main(BattleWork* wp) {
}

void BattleMenuDisp_ActSelect_Disp(BattleWork* wp) {
    DrawMainMenu(wp);
}

void BattleMenuDisp_ActSelect_End(BattleWork* wp) {
    if (WP_SUBMENU(wp)) {
        BattleFree(WP_SUBMENU(wp));
        WP_SUBMENU(wp) = NULL;
    }
}

void DrawMainMenu(BattleWork* wp_) {
    Mtx mtx[5];
    Vec pos[5];
    Mtx trans;
    Mtx rot;
    Mtx scale;
    f32 z[5];
    f32 a;
    s32 i;
    BattleWork* wp = _battleWorkPointer;
    BattleSubMenu* menu = WP_SUBMENU(wp);
    BattleCursor* cursor = menu->cursor;
    f32 c;
    f32 ox;
    f32 oy;
    f32 b;
    s32 n;
    s32 diff;
    BattleCursor* mc;

    camGetPtr(CAMERA_2D);
    switch (menu->field_C) {
    case 0:
        n = 0;
        for (i = 0; i < cursor->unk8; i++) {
            if (menu->field_34[i] == 3) {
                n++;
            }
        }
        if (n == cursor->unk8) {
            menu->field_C = 1;
        }
        break;
    case 1:
        menu->field_14 = 1;
        mc = menu->cursor;
        diff = mc->field_0 - menu->field_18;
        if (diff == 1 || -(mc->unk8 - 1) == diff) {
            menu->field_C = 2;
            menu->field_14 = 0;
            menu->field_18 = menu->cursor->field_0;
            for (i = 0; i < cursor->unk8; i++) {
                menu->field_34[i] = 4;
                menu->field_48[i] = 0;
                menu->field_48[i + 10] = menu->field_84[i];
                menu->field_84[i]--;
                if (menu->field_84[i] == -1) {
                    menu->field_84[i] = cursor->unk8 - 1;
                }
            }
        } else if (diff == -1 || mc->unk8 - 1 == diff) {

            menu->field_C = 2;
            menu->field_14 = 0;
            menu->field_18 = menu->cursor->field_0;
            for (i = 0; i < cursor->unk8; i++) {
                menu->field_34[i] = 5;
                menu->field_48[i] = 0;
                menu->field_48[i + 10] = menu->field_84[i];
                menu->field_84[i]++;
                if (menu->field_84[i] == cursor->unk8) {
                    menu->field_84[i] = 0;
                }
            }
        }
        break;
    case 2:
        n = 0;
        for (i = 0; i < cursor->unk8; i++) {
            if (menu->field_34[i] == 3) {
                n++;
            }
        }
        if (n == cursor->unk8) {
            menu->field_C = 1;
        }
        break;
    case 10:
        menu->field_C = 1;
        menu->field_14 = 1;
        break;
    }

    menu->field_1C = 0;
    for (i = 0; i < cursor->unk8; i++) {
        switch (menu->field_34[i]) {
        case 0:
            menu->field_48[i]--;
            if (menu->field_48[i] <= 0) {
                menu->field_34[i] = 1;
                menu->field_48[i] = 0;
            }
            break;
        case 1:
            menu->field_20[i] |= 1;
            c = 0.0f;
            a = c + 0.2f * menu->field_48[i];
            b = 3.1415927f * kakutable[cursor->unk8 - 3][menu->field_84[i]] / 180.0f;
            if (a >= b) {
                a = b;
            }
            GetRingCenter(&pos[i]);
            GetRingOffset(a, &ox, &oy);
            pos[i].x += ox;
            pos[i].y += oy;
            pos[i].z = oy;
            if (i == cursor->field_0) {
                c = 270.0f + 90.0f * menu->field_48[i] / 10.0f;
                if (c >= 360.0f) {
                    c = 0.0f;
                }
                PSMTXRotRad(rot, 'x', 0.017453292f * c);
                PSMTXTrans(trans, 20.0f + pos[i].x, pos[i].y, pos[i].z);
                PSMTXConcat(trans, rot, mtx[i]);
                menu->field_20[i] |= 2;
            } else {
                c = 270.0f + 90.0f * menu->field_48[i] / 10.0f;
                if (c >= 360.0f) {
                    c = 0.0f;
                }
                PSMTXRotRad(rot, 'x', 0.017453292f * c);
                PSMTXTrans(trans, pos[i].x, pos[i].y, pos[i].z);
                PSMTXConcat(trans, rot, mtx[i]);
            }
            z[i] = 0.0f;
            if (a >= b && menu->field_48[i] >= 10) {
                menu->field_34[i] = 3;
                menu->field_48[i] = -1;
            }
            menu->field_48[i]++;
            break;
        case 3:
            a = 3.1415927f * kakutable[cursor->unk8 - 3][menu->field_84[i]] / 180.0f;
            GetRingCenter(&pos[i]);
            GetRingOffset(a, &ox, &oy);
            pos[i].x += ox;
            pos[i].y += oy;
            pos[i].z = oy;
            if (i == cursor->field_0) {
                PSMTXTrans(mtx[i], 20.0f + pos[i].x, pos[i].y, pos[i].z);
                menu->field_20[i] |= 2;
            } else {
                PSMTXTrans(mtx[i], pos[i].x, pos[i].y, pos[i].z);
            }
            z[i] = 0.0f;
            break;
        case 4:
            c = kakutable[cursor->unk8 - 3][menu->field_48[i + 10]];
            b = kakutable[cursor->unk8 - 3][menu->field_84[i]];
            if (0.0f == b) {
                b = 360.0f;
            }
            a = 3.1415927f * c / 180.0f + 0.2f * menu->field_48[i];
            b = 3.1415927f * b / 180.0f;
            if (a >= b) {
                a = b;
                menu->field_34[i] = 3;
                menu->field_48[i] = -1;
            }
            GetRingCenter(&pos[i]);
            GetRingOffset(a, &ox, &oy);
            pos[i].x += ox;
            pos[i].y += oy;
            pos[i].z = oy;
            PSMTXTrans(mtx[i], pos[i].x, pos[i].y, pos[i].z);
            z[i] = 0.0f;
            menu->field_20[i] &= ~2;
            menu->field_48[i]++;
            break;
        case 5:
            c = kakutable[cursor->unk8 - 3][menu->field_48[i + 10]];
            if (0.0f == c) {
                c = 360.0f;
            }
            a = 3.1415927f * c / 180.0f - 0.2f * menu->field_48[i];
            b = 3.1415927f * kakutable[cursor->unk8 - 3][menu->field_84[i]] / 180.0f;
            if (a <= b) {
                a = b;
                menu->field_34[i] = 3;
                menu->field_48[i] = -1;
            }
            GetRingCenter(&pos[i]);
            GetRingOffset(a, &ox, &oy);
            pos[i].x += ox;
            pos[i].y += oy;
            pos[i].z = oy;
            PSMTXTrans(mtx[i], pos[i].x, pos[i].y, pos[i].z);
            z[i] = 0.0f;
            menu->field_20[i] &= ~2;
            menu->field_48[i]++;
            break;
        case 10:
            menu->field_20[i] |= 1;
            menu->field_20[i] |= 2;
            GetRingCenter(&pos[i]);
            GetRingOffset(0.0f, &ox, &oy);
            pos[i].x += ox;
            pos[i].y += oy;
            pos[i].z = oy;
            pos[i].x = ((f32 (*)(s32, f32, f32, s32, s32))intplGetValue)(4, -60.0f, pos[i].x, menu->field_48[i], 12);
            pos[i].y = ((f32 (*)(s32, f32, f32, s32, s32))intplGetValue)(4, 55.0f, pos[i].y, menu->field_48[i], 12);
            PSMTXTrans(mtx[i], 20.0f + pos[i].x, pos[i].y, pos[i].z);
            z[i] = ((f32 (*)(s32, f32, f32, s32, s32))intplGetValue)(0, 360.0f, 0.0f, menu->field_48[i], 12);
            menu->field_48[i]++;
            if (menu->field_48[i] > 12) {
                menu->field_34[i] = 3;
            }
            break;
        case 11:
            menu->field_20[i] |= 1;
            menu->field_20[i] &= ~2;
            a = 3.1415927f * kakutable[cursor->unk8 - 3][menu->field_84[i]] / 180.0f;
            GetRingCenter(&pos[i]);
            GetRingOffset(a, &ox, &oy);
            pos[i].x += ox;
            pos[i].y += oy;
            pos[i].z = oy;
            a = 0.1f * menu->field_48[i];
            if (a >= 1.0f) {
                menu->field_34[i] = 3;
            }
            PSMTXScale(scale, a, a, 1.0f);
            PSMTXTrans(trans, pos[i].x, pos[i].y, pos[i].z);
            PSMTXConcat(trans, scale, mtx[i]);
            z[i] = 0.0f;
            menu->field_48[i]++;
            break;
        }
    }
    DrawMainIcon(menu, menu->acts, mtx, pos, z);
    DrawMenuPartyChangeButton();
    DrawMenuPinchMark();
}

void BattleMenuDisp_WeaponSelect_Init(BattleWork* wp, BattleCursor* cursor, BattleCommandWeapon* weapons, BOOL flag) {
    BattleSubMenu* menu = NULL;
    BattleCursor* mainCursor = NULL;

    InitSubMenuCommonProcess(&menu, &mainCursor, cursor);
    menu->weapons = weapons;
    if (flag & 1) {
        InitSubMenuCommonProcess2(menu, mainCursor);
    } else {
        InitSubMenuCommonProcess3(menu, mainCursor);
    }
}

void BattleMenuDisp_WeaponSelect_Main(BattleWork* wp) {
}

void BattleMenuDisp_WeaponSelect_Disp(BattleWork* wp) {
    DrawWeaponWin();
}

void BattleMenuDisp_WeaponSelect_End(BattleWork* wp) {
    if (WP_SUBMENU(wp)) {
        BattleFree(WP_SUBMENU(wp));
        WP_SUBMENU(wp) = NULL;
    }
}

void DrawWeaponWin(void) {
    u8 buf1E4[0x154];
    u8 bufF4[0xF0];
    u8 bufB8[0x3C];
    u8 bufA4[0x14];
    s32 enabled[7];
    const char* names[7];
    s32 costs[7];
    s32 p34[7];
    s16 icons[8];
    Vec v;
    f32 x;
    f32 y;
    BattleWork* wp = _battleWorkPointer;
    BattleCommand* cmd;
    BattleSubMenu* menu;
    BattleCursor* cursor;
    BattleUnit* unit;
    BattleWeapon* weapon;
    const char* help;
    s32 i;
    s32 idx;

    cmd = &wp->commandMenu;
    menu = *(BattleSubMenu**)&cmd->field_55C;
    cursor = menu->cursor;
    unit = BattleGetUnitPtr(wp, wp->activeUnitId);
    weapon = menu->weapons[cursor->field_0].weapon;
    DrawSubMenuCommonProcess(&x, &y, bufF4, bufB8, bufA4, buf1E4);
    for (i = 0; i < 7; i++) {
        idx = (s32)menu->field_98 + i;
        if (idx < cursor->unk8) {
            enabled[i] = menu->weapons[idx].enabled;
            icons[i] = *(u16*)&cmd->weapon[idx].icon;
            names[i] = menu->weapons[idx].name;
            if (menu->weapons[idx].weapon) {
                costs[i] = BtlUnit_GetWeaponCost(unit, menu->weapons[idx].weapon);
            } else {
                costs[i] = 0;
            }
            if (menu->weapons[idx].weapon) {
                p34[i] = (u8)menu->weapons[idx].weapon->baseSpCost;
            } else {
                p34[i] = 0;
            }
        }
    }
    DrawMainIcon(menu, wp->commandMenu.act, bufF4, bufB8, bufA4);
    DrawSubIcon(menu, buf1E4, enabled, icons, names, costs, p34, NULL, 0);
    v = (Vec){-60.0f, 35.0f, 0.0f};
    DrawMenuCursorAndScrollArrow(&v);
    if (weapon) {
        if (weapon->itemId) {
            help = itemDataTable[weapon->itemId].description;
        } else if (weapon == &ItemWeaponData_CookingItem) {
            help = itemDataTable[menu->weapons[cursor->field_0].itemId].description;
        } else {
            help = weapon->description;
        }
    } else {
        help = itemDataTable[menu->weapons[cursor->field_0].itemId].description;
    }
    if (help) {
        DrawMenuHelpWin(help, x, y);
    }
}

void BattleMenuDisp_Operation_Init(BattleWork* wp, BOOL flag) {
    BattleSubMenu* menu;
    BattleCursor* mainCursor;
    BattleCursor* cursor;

    if (BattleGetUnitPtr(wp, wp->activeUnitId)->currentKind == 0xDE) {
        cursor = &wp->commandMenu.cursor[8];
    } else {
        cursor = &wp->commandMenu.cursor[9];
    }
    InitSubMenuCommonProcess(&menu, &mainCursor, cursor);
    if (flag & 1) {
        InitSubMenuCommonProcess2(menu, mainCursor);
    } else {
        InitSubMenuCommonProcess3(menu, mainCursor);
    }
}

void BattleMenuDisp_Operation_Main(BattleWork* wp) {
}

void BattleMenuDisp_Operation_Disp(BattleWork* wp) {
    DrawOperationWin();
}

void BattleMenuDisp_Operation_End(BattleWork* wp) {
    if (WP_SUBMENU(wp)) {
        BattleFree(WP_SUBMENU(wp));
        WP_SUBMENU(wp) = NULL;
    }
}

void DrawOperationWin(void) {
    u8 buf1E4[0x154];
    u8 bufF4[0xF0];
    u8 bufB8[0x3C];
    u8 bufA4[0x14];
    s32 enabled[7];
    const char* names[7];
    s32 costs[7];
    s32 p34[7];
    s16 icons[8];
    Vec v;
    f32 x;
    f32 y;
    BattleWork* wp = _battleWorkPointer;
    BattleSubMenu* menu = WP_SUBMENU(wp);
    BattleCursor* cursor = menu->cursor;
    BattleCommandOperation* items;
    s32 i;
    s32 idx;

    BattleGetUnitPtr(wp, wp->activeUnitId);
    items = wp->commandMenu.operation;
    DrawSubMenuCommonProcess(&x, &y, bufF4, bufB8, bufA4, buf1E4);
    for (i = 0; i < 7; i++) {
        idx = (s32)menu->field_98 + i;
        if (idx < cursor->unk8) {
            enabled[i] = items[idx].enabled;
            if (*(u16*)&items[i].icon) {
                icons[i] = *(u16*)&items[idx].icon;
            } else {
                icons[i] = 0x18B;
            }
            names[i] = items[idx].name;
            costs[i] = items[idx].cost;
            p34[i] = 0;
        }
    }
    DrawMainIcon(menu, wp->commandMenu.act, bufF4, bufB8, bufA4);
    DrawSubIcon(menu, buf1E4, enabled, icons, names, costs, p34, NULL, 0);
    v = (Vec){-60.0f, 35.0f, 0.0f};
    DrawMenuCursorAndScrollArrow(&v);
    if (items[cursor->field_0].help) {
        DrawMenuHelpWin(items[cursor->field_0].help, x, y);
    }
}

void BattleMenuDisp_ChangePartySelect_Init(BattleWork* wp, BOOL flag) {
    BattleSubMenu* menu;
    BattleCursor* mainCursor;

    InitSubMenuCommonProcess(&menu, &mainCursor, &wp->commandMenu.cursor[10]);
    if (flag & 1) {
        InitSubMenuCommonProcess3(menu, mainCursor);
    } else {
        InitSubMenuCommonProcess2(menu, mainCursor);
    }
}

void BattleMenuDisp_ChangePartySelect_Main(BattleWork* wp) {
}

void BattleMenuDisp_ChangePartySelect_Disp(BattleWork* wp) {
    DrawChangePartyWin();
}

void BattleMenuDisp_ChangePartySelect_End(BattleWork* wp) {
    if (WP_SUBMENU(wp)) {
        BattleFree(WP_SUBMENU(wp));
        WP_SUBMENU(wp) = NULL;
    }
}

void DrawChangePartyWin(void) {
    u8 buf1E4[0x154];
    u8 bufF4[0xF0];
    u8 bufB8[0x3C];
    u8 bufA4[0x14];
    s32 enabled[7];
    const char* names[7];
    s32 costs[7];
    s32 p34[7];
    s16 icons[8];
    Vec v;
    f32 x;
    f32 y;
    BattleWork* wp = _battleWorkPointer;
    BattleSubMenu* menu = WP_SUBMENU(wp);
    BattleCursor* cursor = menu->cursor;
    BattleCommandParty* items;
    s32 i;
    s32 idx;

    BattleGetUnitPtr(wp, wp->activeUnitId);
    items = wp->commandMenu.party;
    DrawSubMenuCommonProcess(&x, &y, bufF4, bufB8, bufA4, buf1E4);
    for (i = 0; i < 7; i++) {
        idx = (s32)menu->field_98 + i;
        if (idx < cursor->unk8) {
            enabled[i] = items[idx].enabled;
            if (*(u16*)&items[idx].icon) {
                icons[i] = *(u16*)&items[idx].icon;
            } else {
                icons[i] = 0x18B;
            }
            names[i] = items[idx].name;
            costs[i] = 0;
            p34[i] = 0;
        }
    }
    DrawMainIcon(menu, wp->commandMenu.act, bufF4, bufB8, bufA4);
    DrawSubIcon(menu, buf1E4, enabled, icons, names, costs, p34, items, 1);
    v = (Vec){-60.0f, 35.0f, 0.0f};
    DrawMenuCursorAndScrollArrow(&v);
    if (items[cursor->field_0].help) {
        DrawMenuHelpWin(items[cursor->field_0].help, x, y);
    }
}

void BattleMenuDisp_MultiItem_Init(BattleWork* wp, BOOL flag) {
    BattleSubMenu* menu;
    BattleCursor* mainCursor;

    InitSubMenuCommonProcess(&menu, &mainCursor, &wp->commandMenu.cursor[12]);
    if (flag & 1) {
        InitSubMenuCommonProcess2(menu, mainCursor);
    } else {
        InitSubMenuCommonProcess3(menu, mainCursor);
    }
}

void BattleMenuDisp_MultiItem_Main(BattleWork* wp) {
}

void BattleMenuDisp_MultiItem_Disp(BattleWork* wp) {
    DrawMultiItemWin();
}

void BattleMenuDisp_MultiItem_End(BattleWork* wp) {
    if (WP_SUBMENU(wp)) {
        BattleFree(WP_SUBMENU(wp));
        WP_SUBMENU(wp) = NULL;
    }
}

void DrawMultiItemWin(void) {
    u8 buf1E4[0x154];
    u8 bufF4[0xF0];
    u8 bufB8[0x3C];
    u8 bufA4[0x14];
    s32 enabled[7];
    const char* names[7];
    s32 costs[7];
    s32 p34[7];
    s16 icons[8];
    Vec v;
    f32 x;
    f32 y;
    BattleWork* wp = _battleWorkPointer;
    BattleSubMenu* menu = WP_SUBMENU(wp);
    BattleCursor* cursor = menu->cursor;
    BattleCommandMultiItem* items;
    s32 i;
    s32 idx;

    BattleGetUnitPtr(wp, wp->activeUnitId);
    items = wp->commandMenu.multiItem;
    DrawSubMenuCommonProcess(&x, &y, bufF4, bufB8, bufA4, buf1E4);
    for (i = 0; i < 7; i++) {
        idx = (s32)menu->field_98 + i;
        if (idx < cursor->unk8) {
            enabled[i] = items[idx].enabled;
            if (*(u16*)&items[idx].icon) {
                icons[i] = *(u16*)&items[idx].icon;
            } else {
                icons[i] = 0x18B;
            }
            names[i] = items[idx].name;
            costs[i] = items[idx].cost;
            p34[i] = 0;
        }
    }
    DrawMainIcon(menu, wp->commandMenu.act, bufF4, bufB8, bufA4);
    DrawSubIcon(menu, buf1E4, enabled, icons, names, costs, p34, NULL, 0);
    v = (Vec){-60.0f, 35.0f, 0.0f};
    DrawMenuCursorAndScrollArrow(&v);
    if (items[cursor->field_0].help) {
        DrawMenuHelpWin(items[cursor->field_0].help, x, y);
    }
}
