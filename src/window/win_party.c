#include "battle/battle_database_common.h"
#include "driver/animdrv.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mario_pouch.h"
#include "pmario_sound.h"
#include "data/item_data.h"
#include "driver/msgdrv.h"
#include "manager/fontmgr.h"
#include "mario/mariost.h"
#include "sdk/texPalette.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

extern ItemData itemDataTable[];
extern int sprintf(char* str, const char* format, ...);

typedef struct WinPartyWork {
    u16 flags;                    // 0x0
    u8 pad_2[2];                  // 0x2
    u32 buttonTrg;                // 0x4
    u32 buttonRep;                // 0x8
    u32 dirTrg;                   // 0xC
    u32 dirRep;                   // 0x10
    u8 field_0x14[0x28 - 0x14];   // 0x14
    struct { u8 pad[0xA0]; void** data; }* file; // 0x28
    u8 field_0x2C[0xC0 - 0x2C];   // 0x2C
    struct { s32 field_0x0; f32 x; f32 y; f32 w; f32 h; } pos[2]; // 0xC0
    u8 field_0xE8[0x158 - 0xE8];  // 0xE8
    f32 msgX;                     // 0x158
    f32 msgY;                     // 0x15C
    u8 field_0x160[0x19C - 0x160]; // 0x160
    s32 field_0x19C;              // 0x19C
    s32 poseIds[7];               // 0x1A0
    s32 partyIds[7];              // 0x1BC
    s32 dtIdx;                    // 0x1D8
    s32 cursor;                   // 0x1DC
    s32 count;                    // 0x1E0
    f32 angle;                    // 0x1E4
    f32 angleTarget;              // 0x1E8
    u8 field_0x1EC[0x1FC - 0x1EC]; // 0x1EC
    s32 field_0x1FC;              // 0x1FC
    s32 mode;                     // 0x200
    s32 wazaCount;                // 0x204
    s32 wazaCursor;               // 0x208
} WinPartyWork;

typedef struct WinPartyDt {
    s32 partyId;          // 0x0
    s32 nameMsgId;        // 0x4
    const char* name;     // 0x8
    const char* msg;      // 0xC
    const char* poseName; // 0x10
    const char* animName; // 0x14
    const char* poseName2; // 0x18
    const char* animName2; // 0x1C
    BattleWeapon** waza;  // 0x20
} WinPartyDt;

extern BattleWeapon partyWeapon_ChristineNormalAttack;
extern BattleWeapon partyWeapon_ChristineMonosiri;
extern BattleWeapon partyWeapon_ChristineRenzokuAttack;
extern BattleWeapon partyWeapon_ChristineKiss;
extern BattleWeapon partyWeapon_NokotarouKouraAttack;
extern BattleWeapon partyWeapon_NokotarouSyubibinKoura;
extern BattleWeapon partyWeapon_NokotarouKouraGuard;
extern BattleWeapon partyWeapon_NokotarouTsuranukiKoura;
extern BattleWeapon partyWeapon_ClaudaNormalAttack;
extern BattleWeapon partyWeapon_ClaudaBreathAttack;
extern BattleWeapon partyWeapon_ClaudaPredationAttack;
extern BattleWeapon partyWeapon_ClaudaKumogakureAttack;
extern BattleWeapon partyWeapon_YoshiNormalAttack;
extern BattleWeapon partyWeapon_YoshiNomikomi_Shot;
extern BattleWeapon partyWeapon_YoshiEggAttack_Minimini;
extern BattleWeapon partyWeapon_YoshiCallGuard;
extern BattleWeapon partyWeapon_VivianNormalAttack;
extern BattleWeapon partyWeapon_VivianShadowGuard;
extern BattleWeapon partyWeapon_VivianMagicalPowder;
extern BattleWeapon partyWeapon_VivianCharmKissAttack;
extern BattleWeapon partyWeapon_SandersNormalAttack;
extern BattleWeapon partyWeapon_SandersTimeBombSet;
extern BattleWeapon partyWeapon_SandersCounterSet;
extern BattleWeapon partyWeapon_SandersSuperBombAttack;
extern BattleWeapon partyWeapon_ChuchurinaNormalAttackLeft;
extern BattleWeapon partyWeapon_ChuchurinaItemSteal;
extern BattleWeapon partyWeapon_ChuchurinaMadowaseAttack;
extern BattleWeapon partyWeapon_ChuchurinaKiss;

void winMsgEntry(WinPartyWork* wp, s32 a, const char* msg, s32 b);
void winBgGX(WinPartyWork* wp, s32 a, f32 x, f32 y);
void winKirinukiGX(WinPartyWork* wp, s32 a, f32 x, f32 y, f32 w, f32 h);
void winNameGX(WinPartyWork* wp, s32 a, f32 x, f32 y, f32 w, f32 h);
void winWazaGX(WinPartyWork* wp, s32 a, f32 x, f32 y, f32 w, f32 h);
void winKageGX(WinPartyWork* wp, f32 x, f32 y, f32 z, f32 scale, GXColor* color);
void winZClear(void);
void winFontInit(void);
void winIconInit(void);
void winTexInit(TEXPalette* texture);
void winTexSet(u32 textureId, Vec translate, Vec scale, GXColor color);
void winIconSet(s32 iconId, Vec translate, Vec scale, GXColor color);
void winFontSet(Vec pos, Vec scale, GXColor color, const char* fmt, ...);
void winFontSetR(Vec pos, Vec scale, GXColor color, const char* fmt, ...);
void winFontSetEdge(Vec pos, Vec scale, GXColor color, const char* fmt, ...);
void winFontSetLabel(Vec pos, Vec scale, GXColor color, const char* msg);
const char* winZenkakuStr(s32 num);
void animPoseVivianMain(s32 poseId, Vec* pos);

BattleWeapon* chri_waza[4] = {
    &partyWeapon_ChristineNormalAttack,
    &partyWeapon_ChristineMonosiri,
    &partyWeapon_ChristineRenzokuAttack,
    &partyWeapon_ChristineKiss,
};
BattleWeapon* noko_waza[4] = {
    &partyWeapon_NokotarouKouraAttack,
    &partyWeapon_NokotarouSyubibinKoura,
    &partyWeapon_NokotarouKouraGuard,
    &partyWeapon_NokotarouTsuranukiKoura,
};
BattleWeapon* cloud_waza[4] = {
    &partyWeapon_ClaudaNormalAttack,
    &partyWeapon_ClaudaBreathAttack,
    &partyWeapon_ClaudaPredationAttack,
    &partyWeapon_ClaudaKumogakureAttack,
};
BattleWeapon* yoshi_waza[4] = {
    &partyWeapon_YoshiNormalAttack,
    &partyWeapon_YoshiNomikomi_Shot,
    &partyWeapon_YoshiEggAttack_Minimini,
    &partyWeapon_YoshiCallGuard,
};
BattleWeapon* vivi_waza[4] = {
    &partyWeapon_VivianNormalAttack,
    &partyWeapon_VivianShadowGuard,
    &partyWeapon_VivianMagicalPowder,
    &partyWeapon_VivianCharmKissAttack,
};
BattleWeapon* sand_waza[4] = {
    &partyWeapon_SandersNormalAttack,
    &partyWeapon_SandersTimeBombSet,
    &partyWeapon_SandersCounterSet,
    &partyWeapon_SandersSuperBombAttack,
};
BattleWeapon* chu_waza[4] = {
    &partyWeapon_ChuchurinaNormalAttackLeft,
    &partyWeapon_ChuchurinaItemSteal,
    &partyWeapon_ChuchurinaMadowaseAttack,
    &partyWeapon_ChuchurinaKiss,
};

WinPartyDt winPartyDt[7] = {
    {1, 0x155, "name_party0", "msg_menu_party_kuri", "EFF_m_kuriko", "W_1", "d_mario", "R_2", chri_waza},
    {2, 0x156, "name_party1", "msg_menu_party_noko", "EFF_m_noko", "W_1", "d_mario", "R_3", noko_waza},
    {5, 0x15F, "name_party4", "msg_menu_party_cloud", "EFF_m_cloud", "W_1", "d_mario", "R_4", cloud_waza},
    {4, 0x158, "name_party3", "msg_menu_party_yoshi", "EFF_m_yoshi", "W_1", "d_mario", "R_5", yoshi_waza},
    {6, 0x160, "name_party5", "msg_menu_party_vivi", "c_vivian", "PTR_S_1", "d_mario", "R_12", vivi_waza},
    {3, 0x157, "name_party2", "msg_menu_party_bom", "EFF_m_bomb", "W_1", "d_mario", "R_13", sand_waza},
    {7, 0x161, "name_party6", "msg_menu_party_chu", "EFF_m_chu", "W_1", "d_mario", "R_14", chu_waza},
};

void winPartyInit(WinPartyWork* wp) {
    PouchData* pouch;
    s32 i;
    s32 j;
    const char* poseName;
    const char* animName;

    pouch = pouchGetPtr();
    wp->count = 0;
    for (i = 0; i < 7; i++) {
        for (j = 0; j < 8; j++) {
            if (j == winPartyDt[i].partyId && (pouch->partyData[j].flags & 1) &&
                !(pouch->partyData[j].flags & 2)) {
                if (wp->flags & 0x2000) {
                    poseName = winPartyDt[i].poseName2;
                    switch (pouchGetPartyColor(4)) {
                        case 0:
                            animName = "R_5";
                            break;
                        case 1:
                            animName = "R_6";
                            break;
                        case 2:
                            animName = "R_7";
                            break;
                        case 3:
                            animName = "R_8";
                            break;
                        case 4:
                            animName = "R_9";
                            break;
                        case 5:
                            animName = "R_10";
                            break;
                        case 6:
                        default:
                            animName = "R_11";
                            break;
                    }
                } else if (j == 4) {
                    switch (pouchGetPartyColor(4)) {
                        case 0:
                            poseName = "EFF_m_yoshi";
                            break;
                        case 1:
                            poseName = "EFF_m_yoshi2";
                            break;
                        case 2:
                            poseName = "EFF_m_yoshi3";
                            break;
                        case 3:
                            poseName = "EFF_m_yoshi4";
                            break;
                        case 4:
                            poseName = "EFF_m_yoshi5";
                            break;
                        case 5:
                            poseName = "EFF_m_yoshi6";
                            break;
                        case 6:
                        default:
                            poseName = "EFF_m_yoshi7\0\0\0";
                            break;
                    }
                    animName = "W_1";
                } else {
                    poseName = winPartyDt[i].poseName;
                    animName = winPartyDt[i].animName;
                }
                wp->poseIds[wp->count] = animPoseEntry(poseName, 0);
                animPoseSetAnim(wp->poseIds[wp->count], animName, 1);
                animPoseSetMaterialFlagOn(wp->poseIds[wp->count], 0x1800);
                wp->partyIds[wp->count] = j;
                wp->count++;
            }
        }
    }

    wp->cursor = 0;
    for (i = 0; i < wp->count; i++) {
        s32 party;
        party = !marioBgmodeChk() ? marioGetParty() : marioGetPtr()->prevPartyMemberId[0];
        if (party == wp->partyIds[i]) {
            wp->cursor = i;
            break;
        }
    }

    for (i = 0; i < 8; i++) {
        if (wp->partyIds[wp->cursor] == winPartyDt[i].partyId) {
            wp->dtIdx = i;
            break;
        }
    }

    wp->field_0x1FC = wp->cursor;
    wp->angle = wp->angleTarget = 360.0f / wp->count * wp->cursor;
    wp->mode = 0;
    wp->wazaCount = pouchGetPartyAttackLv(winPartyDt[wp->dtIdx].partyId) + 2;
    switch (pouchGetPartyColor(4)) {
        case 0:
            winPartyDt[3].nameMsgId = 0x158;
            break;
        case 1:
            winPartyDt[3].nameMsgId = 0x159;
            break;
        case 2:
            winPartyDt[3].nameMsgId = 0x15A;
            break;
        case 3:
            winPartyDt[3].nameMsgId = 0x15B;
            break;
        case 4:
            winPartyDt[3].nameMsgId = 0x15C;
            break;
        case 5:
            winPartyDt[3].nameMsgId = 0x15D;
            break;
        case 6:
        default:
            winPartyDt[3].nameMsgId = 0x15E;
            break;
    }
}

void winPartyInit2(WinPartyWork* wp) {
    wp->angle = wp->angleTarget = 360.0f / wp->count * wp->cursor;
    wp->mode = 0;
    wp->msgX = -200.0f;
    wp->msgY = 20.0f;
    winMsgEntry(wp, 0, winPartyDt[wp->dtIdx].msg, 0);
}

void winPartyExit(WinPartyWork* wp) {
    s32 i;

    for (i = 0; i < wp->count; i++) {
        if (wp->poseIds[i] != -1) {
            animPoseRelease(wp->poseIds[i]);
        }
    }
}

s32 winPartyMain(WinPartyWork* wp) {
    s32 i;
    s32 prev;

    if (wp->count == 0) {
        return -1;
    }

    switch (wp->mode) {
        case 0:
            if (wp->buttonTrg & 0x100) {
                wp->field_0x19C = 1;
                return -2;
            }
            if (wp->buttonTrg & 0x400) {
                wp->mode = 1;
                wp->wazaCursor = 0;
                psndSFXOn((const char*)0x20012);
            } else if (wp->buttonTrg & 0x200) {
                psndSFXOn((const char*)0x20013);
                wp->field_0x19C = 0;
                return -1;
            } else if (wp->buttonTrg & 0x1000) {
                wp->field_0x19C = 0;
                return -2;
            } else if (wp->dirRep & 0x4000) {
                prev = wp->cursor;
                wp->cursor = prev - 1;
                while (wp->cursor < 0) {
                    wp->cursor += wp->count;
                }
                wp->angleTarget = 360.0f / wp->count * wp->cursor;
                while (wp->angle < wp->angleTarget) {
                    wp->angle += 360.0f;
                }
                if (prev != wp->cursor) {
                    psndSFXOn((const char*)0x209D6);
                }
                for (i = 0; i < 8; i++) {
                    if (wp->partyIds[wp->cursor] == winPartyDt[i].partyId) {
                        wp->dtIdx = i;
                        break;
                    }
                }
            } else if (wp->dirRep & 0x8000) {
                prev = wp->cursor;
                wp->cursor = prev + 1;
                while (wp->cursor > wp->count - 1) {
                    wp->cursor -= wp->count;
                }
                wp->angleTarget = 360.0f / wp->count * wp->cursor;
                while (wp->angle > wp->angleTarget) {
                    wp->angle -= 360.0f;
                }
                if (prev != wp->cursor) {
                    psndSFXOn((const char*)0x209D6);
                }
                for (i = 0; i < 8; i++) {
                    if (wp->partyIds[wp->cursor] == winPartyDt[i].partyId) {
                        wp->dtIdx = i;
                        break;
                    }
                }
            }
            wp->msgX = -200.0f;
            wp->msgY = 20.0f;
            wp->wazaCount = pouchGetPartyAttackLv(winPartyDt[wp->dtIdx].partyId) + 2;
            winMsgEntry(wp, 0, winPartyDt[wp->dtIdx].msg, 0);
            break;
        case 1:
            if (wp->dirRep & 0x1000) {
                wp->wazaCursor--;
                if (wp->wazaCursor < 0) {
                    wp->wazaCursor = wp->wazaCount - 1;
                }
                psndSFXOn((const char*)0x20005);
            } else if (wp->dirRep & 0x2000) {
                wp->wazaCursor++;
                if (wp->wazaCursor >= wp->wazaCount) {
                    wp->wazaCursor = 0;
                }
                psndSFXOn((const char*)0x20005);
            } else if (wp->buttonTrg & 0x200) {
                wp->mode = 0;
                psndSFXOn((const char*)0x20013);
            } else if (wp->buttonTrg & 0x1000) {
                wp->field_0x19C = 0;
                return -2;
            }
            wp->msgX = -10.0f;
            wp->msgY = -26.0f * wp->wazaCursor + 5.0f;
            winMsgEntry(wp, 0, winPartyDt[wp->dtIdx].waza[wp->wazaCursor]->description, 0);
            break;
    }
    return 0;
}

void winPartyMain2(WinPartyWork* wp) {
    wp->angle += (wp->angleTarget - wp->angle) / 4.0f;
}

static const GXColor winPartyWhiteC = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor winPartyBlackC = {0x00, 0x00, 0x00, 0xFF};
void winPartyDisp(s32 camId, void* param, s32 idx) {
    WinPartyWork* wp = param;
    GXColor white2;
    GXColor black2;
    GXColor red[1];
    GXColor white;
    GXColor black;
    GXColor white0;
    GXColor black0; 
    GXColor* kp;
    f32 scale;
    f32 tx;
    f32 bx;
    f32 fy;
    f32 by;
    f32 wy;
    f32 x;
    f32 y;
    f32 w1;
    f32 w2;
    f32 iy;
    s32 i;
    s32 pose;
    s32 j;
    AnimData* anim;
    const char* animName; s32 new_var2 ;
    const char* name;
    PouchPartyData* pd;
    white0 = winPartyWhiteC;
    white = winPartyWhiteC;
    white2 = winPartyWhiteC;
    black0 = winPartyBlackC;
    black = winPartyBlackC;
    black2 = winPartyBlackC;
    x = wp->pos[idx].x;
    y = wp->pos[idx].y;
    winBgGX(wp, 1, x, y);
    if (wp->count == 0) {
        return;
    }

    pd = &pouchGetPtr()->partyData[winPartyDt[wp->dtIdx].partyId];
    fy = 20.0f;
    winKirinukiGX(wp, 0, x - 10.0f, fy + (116.0f + y), 240.0f, 76.0f);

    bx = 45.0f + (x - 10.0f);
    tx = bx - 2.0f;
    by = 110.0f + y;
    for (i = 0; i < 2; i++) {
        GXColor cols[2] = {{0xCB, 0x8C, 0xA2, 0xFE}, {0x9C, 0xC3, 0x81, 0xFE}};
        red[0] = (GXColor){0xA7, 0x3C, 0x3C, 0xFF};
        {
        const char* msgs[2] = {"msg_menu_party_hp", "msg_menu_party_rank"};
        const char* msgs2[2] = {"msg_menu_mario_ruby_hp", "msg_menu_mario_ruby_hp"};

        if (i == 0) {
            scale = 1.0f;
        } else {
            scale = 0.9f;
        }
        w1 = scale * FontGetMessageWidth(msgSearch(msgs[i]));
        w2 = 0.5f * FontGetMessageWidth(msgSearch(msgs2[i]));
        winTexInit(*wp->file->data);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
        winTexSet(0xAF, (Vec){tx, (fy = by - (( f32)(i * 32) )) - 4.0f, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, cols[i]);
        winFontInit();
        winFontSet((Vec){bx - (w1 / 2.0f) , 12.0f + fy, 0.0f}, (Vec){scale, scale, scale}, black2, msgSearch(msgs[i]));
        if (i < 1) {
            winFontSet((Vec){bx - (w2 / 2.0f) , 21.0f + fy, 0.0f}, (Vec){0.5f, 0.5f, 0.5f}, red[i], msgSearch(msgs2[i]));
        }
        }
    }

    winIconInit();
    bx = 60.0f + bx;
    fy = 4.0f + by;
    winIconSet(0x19F, (Vec){bx, fy, 0.0f}, (Vec){0.82f, 0.82f, 0.82f}, white);
    for (i = 0; i < ( wp->wazaCount - 2) ; i++) {
        winIconSet(itemDataTable[0x57].icon_id, (Vec){bx + (( f32)(i * 34) ), iy = 4.0f + (by - 32.0f), 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, white2);
    }

    winTexInit(*wp->file->data);
    winTexSet(0x10, (Vec){8.0f + (170.0f + (x - 10.0f)), fy, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, white);
    winFontInit();
    winFontSetR((Vec){10.0f + (110.0f + (x - 10.0f)), fy = 4.0f + (12.0f + by), 0.0f}, (Vec){0.9f, 0.9f, 0.9f}, black, "%s", winZenkakuStr(pd->currentHP));
    winFontSet((Vec){8.0f + (180.0f + (x - 10.0f)), fy, 0.0f}, (Vec){0.9f, 0.9f, 0.9f}, black, "%s", winZenkakuStr(pd->maximumHP));
    winZClear();

    for (i = 0; i < wp->count; i++) {
        Vec v = {0.0f, 0.0f, 80.0f};
        Mtx rx;
        Mtx ry;
        PSMTXRotRad(rx, 'x', 0.5235988f);
        PSMTXRotRad(ry, 'y', 0.017453292f * ((( 360.0f / wp->count ) * i ) - wp->angle));
        PSMTXMultVec(ry, &v, &v);
        PSMTXMultVec(rx, &v, &v);
        kp = &(GXColor){0x00, 0x00, 0x00, 0x80};
        winKageGX(wp, (x - 140.0f) + v.x, 20.0f + (y + v.y), v.z, (i == wp->cursor) ? ( 0.8f ) : ( 0.6f) , kp);
    }

    for (i = 0; i < wp->count; i++) {
        Vec v = {0.0f, 0.0f, 80.0f};
        Mtx m;
        Mtx s;
        Mtx rx;
        Vec vp;
        Mtx ry;
        pose = wp->poseIds[i];
        if (pose == ( -1) ) {
            continue;
        }
        anim = animPoseGetAnimDataPtr(pose);
        PSMTXRotRad(rx, 'x', 0.5235988f);
        PSMTXRotRad(ry, 'y', 0.017453292f * ((( 360.0f / wp->count ) * i ) - wp->angle));
        PSMTXMultVec(ry, &v, &v);
        PSMTXMultVec(rx, &v, &v);
        if (i == wp->cursor) {
            PSMTXTrans(m, (x - 140.0f) + v.x, 20.0f + (y + v.y), v.z);
            PSMTXScale(s, 1.0f, 1.0f, -1.0f);
            PSMTXRotRad(ry, 'y', 3.1415927f);
            PSMTXConcat(m, s, m);
            PSMTXConcat(m, ry, m);
            animPoseSetMaterialFlagOff(pose, 0x40);
            for (j = 0; j < 7; j++) {
                if (wp->partyIds[i] == winPartyDt[j].partyId) {
                    break;
                }
            }
            if (wp->flags & 0x2000) {
                animName = winPartyDt[j].animName2;
                if (winPartyDt[j].partyId == 4) {
                    switch (pouchGetPartyColor(4)) {
                        case 0:
                            animName = "R_5";
                            break;
                        case 1:
                            animName = "R_6";
                            break;
                        case 2:
                            animName = "R_7";
                            break;
                        case 3:
                            animName = "R_8";
                            break;
                        case 4:
                            animName = "R_9";
                            break;
                        case 5:
                            animName = "R_10";
                            break;
                        case 6:
                        default:
                            animName = "R_11";
                            break;
                    }
                }
            } else {
                animName = winPartyDt[j].animName;
            }
            animPoseSetAnim(pose, animName, 0);
            animPoseSetLocalTime(pose, (f32)(gp->retraceCount % (( s32)(*(( f32*)(( *( (u8**)((( u8*)anim ) + 0x24) )) + 8)) ))));
        } else {
            PSMTXTrans(m, (x - 140.0f) + v.x, 20.0f + (y + v.y), v.z);
            PSMTXScale(s, 0.8f, 0.8f, -0.8f);
            PSMTXRotRad(ry, 'y', 3.1415927f);
            PSMTXConcat(m, s, m);
            PSMTXConcat(m, ry, m);
            animPoseSetMaterialFlagOn(pose, 0x40);
            animPoseSetMaterialEvtColor(pose, (GXColor){0x80, 0x80, 0x80, 0xFF});
            for (j = 0; j < 7; j++) {
                if (wp->partyIds[i] == winPartyDt[j].partyId) {
                    break;
                }
            }
            if (wp->flags & 0x2000) {
                animName = winPartyDt[j].animName2;
                if (winPartyDt[j].partyId == 4) {
                    switch (pouchGetPartyColor(4)) {
                        case 0:
                            animName = "Z_5";
                            break;
                        case 1:
                            animName = "Z_6";
                            break;
                        case 2:
                            animName = "Z_7";
                            break;
                        case 3:
                            animName = "Z_8";
                            break;
                        case 4:
                            animName = "Z_9";
                            break;
                        case 5:
                            animName = "Z_10";
                            break;
                        case 6:
                        default:
                            animName = "Z_11";
                            break;
                    }
                }
            } else {
                animName = winPartyDt[j].animName;
            }
            animPoseSetAnim(pose, animName, 0);
            animPoseSetLocalTime(pose, 0.0f);
        }
        animPoseMain(pose);
        if (wp->partyIds[i] == 6) {
            vp = (Vec){(x - 140.0f) + v.x, 20.0f + (y + v.y), v.z};
            animPoseVivianMain(pose, &vp);
        }
        animPoseDrawMtx(pose, m, 2, 0.0f, 5.0f);
    }

    if (wp->count != 0) {
        winNameGX(wp, 0, x - 230.0f, ( 20.0f + (y - 50.0f) ) - 10.0f, 200.0f, 32.0f);
        winFontInit();
        fy = ( 20.0f + ((y - 50.0f) - 4.0f) ) - 10.0f;
        for (i = 0; i < 7; i++) {
            new_var2 = winPartyDt [i].partyId;
            if (wp->partyIds[wp->cursor] == winPartyDt[i].partyId) {
                if (new_var2 != 4) {
                    name = msgSearch(winPartyDt[i].name);
                } else {
                    name = pouchGetYoshiName();
                }
                w2 = FontGetMessageWidth(name);
                winFontSetEdge((Vec){((200.0f - w2) / 2.0f) + (x - 230.0f), fy, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, white2, name);
            }
        }
    }

    if (wp->count > 1) {
        winFontInit();
        w1 = 0.8f * FontGetMessageWidth(msgSearch("msg_menu_party_henkou"));
        fy = ((-230.0f - w1) / 2.0f) + x;
        winFontSet((Vec){fy, y - 90.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, black, msgSearch("msg_menu_party_henkou"));
        winIconInit();
        winIconSet(100, (Vec){fy - 30.0f, y - 100.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, white);
        winTexInit(*wp->file->data);
        winTexSet(0xC0, (Vec){(x - 150.0f) - 50.0f, y - 16.0f, 0.0f}, (Vec){-0.6f, 0.6f, 0.6f}, white);
        winTexSet(0xC0, (Vec){50.0f + (x - 150.0f), y - 16.0f, 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, white);
    }

    winFontInit();
    w1 = 0.7f * FontGetMessageWidth(msgSearch("msg_menu_party_waza_kakunin"));
    fy = x + (250.0f - w1);
    winFontSet((Vec){fy, y - 100.0f, 0.0f}, (Vec){0.7f, 0.7f, 0.7f}, black, msgSearch("msg_menu_party_waza_kakunin"));
    winIconInit();
    winIconSet(0x1F5, (Vec){fy - 20.0f, y - 110.0f, 0.0f}, (Vec){0.7f, 0.7f, 0.7f}, white);
    winWazaGX(wp, 0, x - 5.0f, wy = 50.0f + y, 260.0f, 144.0f);
    winFontInit();
    w1 = 0.8f * FontGetMessageWidth(msgSearch("msg_menu_party_waza"));
    fy = 45.0f + y;
    winFontSetEdge((Vec){((200.0f - w1) / 2.0f) + x, fy, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, white, msgSearch("msg_menu_party_waza"));
    bx = 230.0f + x;
    winFontSetEdge((Vec){bx - 20.0f, fy, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, white, "\x82\x65\x82\x6F");
    for (i = 0; i < wp->wazaCount; i++) {
        char buf[0x40];
        winFontSetLabel((Vec){10.0f + x, fy = (wy - (( f32)(i * 26) )) - 30.0f, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, black2, winPartyDt[wp->dtIdx].waza[i]->name);
        sprintf(buf, "%s", winZenkakuStr((u8)winPartyDt[wp->dtIdx].waza[i]->baseFpCost));
        winFontSet((Vec){(((238.0f + x) - 20.0f) - (( f32)(FontGetMessageWidth(buf) / 2) )) - 2.0f, fy, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, black2, buf);
        winFontSet((Vec){2.0f + (2.0f + ((17.0f + bx) - 20.0f)), fy - 10.0f, 0.0f}, (Vec){0.5f, 0.5f, 0.5f}, black2, "\x82\x65\x82\x6F");
    }
}
