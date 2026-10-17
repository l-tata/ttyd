#include "mario/mario.h"
#include "mario/mario_party.h"
#include "driver/itemdrv.h"
#include "party.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/mtx.h>

extern void unk_800c92c4(PartyEntry* party);
extern void partyRecordFootmark(PartyEntry* party);
extern void partyWalkInit(PartyEntry* party, s32 mode);
extern void party_move_mode_stay(PartyEntry* party);
extern void party_move_mode_walk(PartyEntry* party);
extern void party_move_mode_behind_mario(PartyEntry* party);
extern void party_move_mode_beside_mario(PartyEntry* party);
extern void partyGoodbyeInit(PartyEntry* party);
extern s32 partyGoodbyeMain(PartyEntry* party);
extern void partyChgRunMode(PartyEntry* party, s32 mode);

#define PB(p, o) (*(u8*)((u8*)(p) + (o)))
#define PH(p, o) (*(s16*)((u8*)(p) + (o)))
#define PW(p, o) (*(u32*)((u8*)(p) + (o)))

static s32 nokoSe;
double sin(double);
double cos(double);
#include "driver/hitdrv.h"
#include "mario/mario_sbr.h"

extern void itemNokoForceGet(ItemEntry* entry);
#include "eff/eff_kemu_test.h"
#include "eff/n64/eff_small_star_n64.h"
#include "mario/mariost.h"
extern ItemEntry* itemNearDistCheck(f32 x, f32 y, f32 z, f32 range);
extern void itemPickUp(ItemEntry* entry);
extern void* mobjHitObjPtrToPtr(HitEntry* hit);
extern void marioChkWallAround(Vec* pos, s32 a1, f32 a2, f32 angle, f32 radius, f32 height);
extern void partyGetMoveDirSpd(PartyEntry* party, f32* dir, f32* spd);
extern void partyGetReturnPos(Vec* pos);
extern void partyMove(PartyEntry* party);
extern void partyMoveNoHosei(PartyEntry* party);
extern void partyClearJumpPara(PartyEntry* party);
extern void partyChgPose(PartyEntry* party, char* pose);
extern void marioChgSmallJumpMotion(void);
extern s32 marioKeyOffChk(void);
extern BOOL marioChkKey(void);
extern void marioAdjustMoveDir(void);
extern BOOL marioChkInScreen(s32 x, s32 y);
extern f32 angleABf(f32 x1, f32 y1, f32 x2, f32 y2);
extern f32 distABf(f32 x1, f32 y1, f32 x2, f32 y2);
extern void movePos(float* ret_sin, float* ret_cos, f32 a3, f32 degree);
extern f32 toMovedirSimple(f32 a);
extern f32 toMovedir2(f32 a, f32 b);
extern s32 sysMsec2Frame(s32 msec);
double sqrt(double);
void nokonoko_finish(PartyEntry* party);
HitEntry* ccwall(PartyEntry* party, s32 mode);
void lookupSafetyPosSub(Vec pos, f32 height, f32 angle, Vec* out);
void lookupSafetyPosSub(Vec pos, f32 height, f32 angle, Vec* out);
void lookupSafetyPosSub(Vec pos, f32 height, f32 angle, Vec* out);
void lookupSafetyPosSub(Vec pos, f32 height, f32 angle, Vec* out);
void lookupSafetyPosSub(Vec pos, f32 height, f32 angle, Vec* out);
void lookupSafetyPosSub(Vec pos, f32 height, f32 angle, Vec* out);
void lookupSafetyPosSub(Vec pos, f32 height, f32 angle, Vec* out);
void lookupSafetyPosSub(Vec pos, f32 height, f32 angle, Vec* out);
void lookupSafetyPos3(Vec pos, f32 height, Vec* out);
void lookupSafetyPos2(Vec pos, f32 height, Vec* out);
u32 lookupSafetyPos(Vec pos, f32 height, Vec* out);

s32 nokonokoGetStatus(PartyEntry* party) {
    u32 m;
    s32 status;
    if (party == NULL) {
        return 0;
    }
    if (party->currentMemberId != 2 || !(party->flags & 0x100)) {
        return 0;
    }
    status = 7;
    m = party->useMotionId;
    if (m <= 2) {
        return 1;
    }
    if (m >= 10 && m <= 13) {
        return 2;
    }
    if (m >= 20 && m <= 21) {
        return 3;
    }
    if (m >= 50 && m <= 71) {
        return 4;
    }
    if (m >= 80 && m <= 100) {
        return 5;
    }
    if (m >= 110 && m <= 114) {
        status = 6;
    }
    return status;
}

BOOL nokonoko_holdItem(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (party == NULL) {
        return FALSE;
    }
    if (party->currentMemberId != 2 || !(party->flags & 0x100)) {
        return FALSE;
    }
    return PW(party, 0x174) != 0;
}

void nokonoko_bye(PartyEntry* party) {
    if (party->field_8 & 8) {
        party->field_8 &= ~8;
        partyGoodbyeInit(party);
    }
    if (partyGoodbyeMain(party)) {
        partyChgRunMode(party, 14);
    }
}

void nokonoko_init(PartyEntry* party) {
    party->field_32 [1] = 2;
    party->field_32 [0] = 0;
    party->field_F0 = 37.0f;
    party->width = 24.0f;
    nokoSe = -1;
}

void nokonoko_move(PartyEntry* party) {
    marioGetPtr();
    if (party->field_8 & 8) {
        party->field_8 &= ~8;
        partyWalkInit(party, 1);
    }
    unk_800c92c4(party);
    partyRecordFootmark(party);
    switch (PB(party, 0x3b)) {
    case 0:
        party_move_mode_stay(party);
        break;
    case 1:
        party_move_mode_walk(party);
        break;
    case 2:
        party_move_mode_behind_mario(party);
        break;
    case 3:
        party_move_mode_beside_mario(party);
        break;
    case 4:
        break;
    }
}

void unk_800cd9a0(void) {
    PartyEntry* party;
    marioGetPtr();
    party = partyGetPtr(marioGetPartyId());
    if (nokonokoGetStatus(party) != 0) {
        if (party->flags & 0x100) {
            marioGetPtr();
            if ((u32)nokoSe != 0xFFFFFFFFU) {
                psndSFXOff(nokoSe);
                nokoSe = -1;
            }
            party->flags &= ~0x100;
            party->flags &= ~0x04000000;
            party->flags &= ~0x01000000;
            party->flags &= ~0x02000000;
            party->flags2 &= ~0x20;
            party->flags2 &= ~0x10;
            party->field_F0 = 37.0f;
            PW(party, 0x178) = 0;
            PW(party, 0x17c) = 0;
            PW(party, 0x180) = 0;
        }
        if (party->field_34 != 8 && party->field_34 != 0xd) {
            partyChgRunMode(party, 2);
            PB(party, 0x3b) = 0;
        }
    }
}

void nokonoko_use_post(PartyEntry* party) {
    if (PW(party, 0x174) != 0) {
        itemNokoForceGet((ItemEntry*)PW(party, 0x174));
        PW(party, 0x174) = 0;
    }
    marioGetPtr();
    if ((u32)nokoSe != 0xFFFFFFFFU) {
        psndSFXOff(nokoSe);
        nokoSe = -1;
    }
    party->flags &= ~0x100;
    party->flags &= ~0x04000000;
    party->flags &= ~0x01000000;
    party->flags &= ~0x02000000;
    party->flags2 &= ~0x20;
    party->flags2 &= ~0x10;
    party->field_F0 = 37.0f;
    PW(party, 0x178) = 0;
    PW(party, 0x17c) = 0;
    PW(party, 0x180) = 0;
}

#define PF(p, o) (*(f32*)((u8*)(p) + (o)))
#define PS32(p, o) (*(s32*)((u8*)(p) + (o)))
typedef struct NokoPad94 {
    u8 pad[0x94];
    Vec v;
} NokoPad94;
#define PV(p, o) (((NokoPad94*)(p))->v)
#define MH(p, o) (*(u16*)((u8*)(p) + (o)))

static inline void noko_se_off(void) {
    marioGetPtr();
    if ((u32)nokoSe != 0xFFFFFFFFU) {
        psndSFXOff(nokoSe);
        nokoSe = -1;
    }
}

static inline void noko_spin(PartyEntry* party) {
    f32 dir;
    if (revise360(party->field_100 - PF(party, 0xf8)) > 180.0f) {
        dir = 1.0f;
    } else {
        dir = -1.0f;
    }
    party->field_10C = revise360(50.0f * dir + party->field_10C);
}

static inline void noko_star(PartyEntry* party, s32 n) {
    f32 c = (f32)cos(6.2831855f * party->field_100 / 360.0f);
    f32 s = (f32)sin(6.2831855f * party->field_100 / 360.0f);
    effSmallStarN64Entry(3, n, party->position.x, party->position.y + party->field_F0, party->position.z, s, -1.0f, -c);
}

#define NOKO_ITEM(party, body)                                                                              if (PW(party, 0x174) == 0) {                                                                                  PW(party, 0x174) = (u32)itemNearDistCheck(party->position.x, party->position.y, party->position.z,                                                   0.5f * party->width + 12.0f);                                    if (PW(party, 0x174) != 0) {                                                                                  itemPickUp((ItemEntry*)PW(party, 0x174));                                                                 body                                                                                                  }                                                                                                     }
void nokonoko_use(PartyEntry* party) {
    Vec out;
    Vec out2;
    Vec rp;
    f32 sx;
    f32 sy;
    f32 sz;
    f32 mx;
    f32 mz;
    f32 dir;
    f32 spd;
    f32 s2x;
    u8* new_var;
    f32 s2y;
    f32 s2z;
    f32 s3x;
    f32 s3y;
    f32 s3z;
    u32 flags;
    HitEntry* hit;
    BOOL b;
    f32 dy;
    f32 dx;
    f32 qx;
    f32 qy;
    f32 qz;
    f32 dz;
    MarioWork* mario = party->playerPtr;

    if (!(party->flags & 0x80000000) && marioKeyOffChk()) {
        marioGetScreenPos(party->position, &sx, &sy, &sz);
        if (!marioChkInScreen((s32)sx, (s32)sy)) {
            mx = mario->position.x;
            mz = mario->position.z;
            movePos(&mx, &mz, 160.0f, angleABf(mario->position.x, mario->position.z, party->position.x, party->position.z));
            party->position.x = mx;
            party->position.z = mz;
            nokonoko_finish(party);
            return;
        }
    }
    if (party->field_8 & 8) {
        party->field_8 &= ~8;
        PW(party, 0x174) = 0;
        PW(party, 0x178) = 0;
        PW(party, 0x17c) = 0;
        PW(party, 0x180) = 0;
        if (party->flags & 0x80000000) {
            if (party->unk16C > 0) {
                PH(party, 0x17e) |= 1;
            }
        } else {
            PH(party, 0x17e) &= ~1;
        }
        party->useMotionId = 0;
    }

    switch (party->useMotionId) {
    case 0:
        if (mario->currMotionId == 0x20 || mario->currMotionId == 0x1f) {
            party->useMotionId = 70;
            break;
        }
        partyClearJumpPara(party);
        party->flags |= 0x04000000;
        party->flags2 |= 0x10;
        partyChgPose(party, "PNK_K_2");
        party->field_F0 = 18.5f;
        party->useMotionId = 1;
        PS32(party, 0x24) = 100;
        PW(party, 0x174) = 0;
        PH(party, 0x17c) = 0;
        marioAdjustMoveDir();
        mario->baseSpeed = 0.0f;
        PV(party, 0x94) = mario->position;
        movePos(&PF(party, 0x94), &PF(party, 0x9c), 2.0f, toMovedirSimple(mario->unk1AC));
        PS32(party, 0x24) = 4;
        PF(party, 0x114) = 0.0f;
        party->field_100 = angleABf(party->position.x, party->position.z, PV(party, 0x94).x, PV(party, 0x94).z);
        party->field_104 = distABf(party->position.x, party->position.z, PV(party, 0x94).x, PV(party, 0x94).z) /
                           (f32)PS32(party, 0x24);
        party->field_10C = revise360(45.0f + party->field_10C);
        party->useMotionId = 1;
        marioChgSmallJumpMotion();
        mario->trigFlags |= 0x20000;
        mario->baseSpeed = 0.0f;
        break;
    case 1:
        if (mario->currMotionId == 0x20 || mario->currMotionId == 0x1f) {
            party->useMotionId = 70;
            break;
        }
        party->field_10C = revise360(45.0f + party->field_10C);
        party->position.x += (PV(party, 0x94).x - party->position.x) / (f32)PS32(party, 0x24);
        party->position.y += (PV(party, 0x94).y - party->position.y) / (f32)PS32(party, 0x24);
        party->position.z += (PV(party, 0x94).z - party->position.z) / (f32)PS32(party, 0x24);
        if (--PS32(party, 0x24) > 0) {
            break;
        }
        party->position = PV(party, 0x94);
        party->field_104 = 0.0f;
        party->field_110 = mario->unk1AC;
        party->field_10C = mario->unk1AC;
        party->useMotionId = 2;
        partyChgPose(party, "PNK_A_1");
    case 2:
        if (mario->currMotionId == 0x20 || mario->currMotionId == 0x1f) {
            party->useMotionId = 70;
            break;
        }
        if (mario->hitobjStandOn == NULL) {
            break;
        }
        marioChgMot(3);
        mario->flags |= 0x80;
        mario->baseSpeed = 0.0f;
        party->useMotionId = 10;
        PS32(party, 0x24) = 15;
        party->field_104 = 0.0f;
        PF(party, 0x128) = 0.0f;
        party->field_100 = toMovedir(mario->unk1AC);
        psndSFXOn_3D((const char*)0x92d, &party->position);
        break;
    case 10:
        psndSFXOn_3D((const char*)0x92a, &party->position);
        noko_star(party, 3);
        party->flags |= 0x04000000;
        party->flags |= 0x01000000;
        party->useMotionId = 11;
    case 11:
        party->field_104 += 2.0f;
        if (party->field_104 > 10.0f) {
            party->field_104 = 10.0f;
        }
    case 12:
        noko_spin(party);
        if (gp->retraceCount & 1) {
            effKemuTestDrawCam(effKemuTestEntry(0, party->position.x, party->field_F0 / 4.0f + party->position.y,
                                                party->position.z, 0.3f),
                               party->camId);
        }
        PF(party, 0x128) += party->field_104;
        NOKO_ITEM(party, party->useMotionId = 50; psndSFXOn_3D((const char*)0x92c, &party->position); noko_star(party, 1); break;)
        if (ccwall(party, 0) != NULL) {
            party->useMotionId = 50;
            psndSFXOn_3D((const char*)0x92c, &party->position);
            noko_star(party, 1);
            break;
        }
        if (PF(party, 0x128) >= 120.0f) {
            party->useMotionId = 12;
            party->field_104 += 0.15f * (-1.0f - party->field_104);
            if (party->field_104 <= 0.0f) {
                party->field_104 = 0.0f;
                if (!(party->flags & 0x80000000)) {
                    if (MH(mario, 0x246) & 0x400) {
                        party->useMotionId = 13;
                        PS32(party, 0x24) = 0;
                    } else {
                        party->useMotionId = 50;
                    }
                } else if (PH(party, 0x17e) & 1) {
                    PS32(party, 0x24) = 5;
                    party->useMotionId = 21;
                    PH(party, 0x17c) = 1;
                    if ((u32)nokoSe == 0xFFFFFFFFU) {
                        nokoSe = psndSFXOn_3D((const char*)0x92e, &party->position);
                    }
                } else {
                    party->useMotionId = 50;
                }
            }
        }
        break;
    case 13:
        if (++PS32(party, 0x24) >= 5) {
            PS32(party, 0x24) = 5;
            party->useMotionId = 20;
            PH(party, 0x17c) = 1;
            if ((u32)nokoSe == 0xFFFFFFFFU) {
                nokoSe = psndSFXOn_3D((const char*)0x92e, &party->position);
            }
        }
    case 20:
        noko_spin(party);
        if (gp->retraceCount % 30 == 0) {
            effKemuTestDrawCam(effKemuTestEntry(5, party->position.x, party->position.y, party->position.z, 1.0f),
                               party->camId);
        }
        if (ccwall(party, 0) != NULL) {
            party->useMotionId = 50;
            psndSFXOn_3D((const char*)0x92c, &party->position);
            noko_star(party, 1);
            noko_se_off();
            break;
        }
        NOKO_ITEM(party, party->useMotionId = 50; psndSFXOn_3D((const char*)0x92c, &party->position); noko_star(party, 1); noko_se_off(); break;)
        if (!(party->flags & 0x80000000)) {
            if (mario->currMotionId == 0x19 || mario->currMotionId == 0x14 || mario->currMotionId == 0x18 || mario->currMotionId == 0x16 || (u16)(mario->currMotionId - 0x1f) <= 1 ||
                (mario->flags & 0x20) || !marioChkKey()) {
                nokonoko_finish(party);
                noko_se_off();
                break;
            }
            if (MH(mario, 0x246) & 0x400) {
                break;
            }
            party->useMotionId = PH(party, 0x17c) != 0 ? 80 : 50;
            noko_se_off();
            break;
        }
        if (PH(party, 0x17e) & 1) {
            party->useMotionId = 80;
        } else {
            party->useMotionId = 50;
        }
        noko_se_off();
        break;
    case 21:
        noko_spin(party);
        if (gp->retraceCount % 30 == 0) {
            effKemuTestDrawCam(effKemuTestEntry(5, party->position.x, party->position.y, party->position.z, 1.0f),
                               party->camId);
        }
        break;
    }

    new_var = ((u8*)party) + 0x128;
    switch (party->useMotionId) {
    case 50:
        partyClearJumpPara(party);
        party->useMotionId = 51;
        party->field_104 = 0.0f;
        party->field_100 = angleABf(party->position.x, party->position.z, PV(party, 0x94).x, PV(party, 0x94).z);
        psndSFXOn_3D((const char*)0x92b, &party->position);
        break;
    case 51: {
        f32 lim = PH(party, 0x17c) != 0 ? 20.0f : 10.0f;
        party->field_104 += 0.3f * (lim - party->field_104);
        if (party->field_104 >= lim) {
            party->field_104 = lim;
            if ((*(f32*)new_var) < 10.0f) {
                party->useMotionId = 52;
            }
        }
    }
    case 52:
        if (ccwall(party, 1) != NULL) {
            partyGetMoveDirSpd(party, &dir, &spd);
            marioChkWallAround(&party->position, 1, spd, dir, party->width, party->field_F0);
            party->useMotionId = 70;
            psndSFXOn_3D((const char*)0x92c, &party->position);
            noko_star(party, 1);
            return;
        }
        noko_spin(party);
        party->field_104 += 0.1f * (-1.0f - party->field_104);
        (*(f32*)new_var) -= party->field_104;
        if ((*(f32*)new_var) <= 0.0f) {
            (*(f32*)new_var) = 0.0f;
            party->useMotionId = 53;
            party->field_104 += 0.1f * (-1.0f - party->field_104);
            PF(party, 0x178) = 50.0f;
        }
        break;
    case 53:
        if (ccwall(party, 1) != NULL) {
            party->useMotionId = 70;
            return;
        }
        PF(party, 0x178) -= 2.2f;
        if (PF(party, 0x178) <= 0.0f) {
            PF(party, 0x178) = 0.0f;
        }
        noko_spin(party);
        party->field_104 += (-2.0f - party->field_104) * (PH(party, 0x17c) != 0 ? 0.2f : 0.1f);
        (*(f32*)new_var) -= party->field_104;
        if (party->field_104 <= 0.0f) {
            party->field_104 = 0.0f;
            flags = lookupSafetyPos(PV(party, 0x94), party->field_F0, &out);
            if (party->field_138 != NULL && ((u32)(flags - 1) <= 1 || flags == 4 || flags == 8)) {
                party->useMotionId = 70;
                break;
            }
            switch (flags) {
            case 3:
                lookupSafetyPosSub(PV(party, 0x94), party->field_F0, 135.0f, &out);
                break;
            case 5:
                lookupSafetyPosSub(PV(party, 0x94), party->field_F0, 315.0f, &out);
                break;
            case 10:
                lookupSafetyPosSub(PV(party, 0x94), party->field_F0, 225.0f, &out);
                break;
            case 12:
                lookupSafetyPosSub(PV(party, 0x94), party->field_F0, 315.0f, &out);
                break;
            case 7:
            case 14:
            case 15:
                party->useMotionId = 70;
                break;
            case 1:
                lookupSafetyPosSub(PV(party, 0x94), party->field_F0, 270.0f, &out);
                break;
            case 8:
                lookupSafetyPos3(PV(party, 0x94), party->field_F0, &out);
                break;
            default:
                lookupSafetyPos2(PV(party, 0x94), party->field_F0, &out);
                break;
            }
            party->useMotionId = 54;
            PV(party, 0x94) = out;
            (*(f32*)new_var) = distABf(party->position.x, party->position.z, PV(party, 0x94).x, PV(party, 0x94).z);
            party->field_100 = angleABf(party->position.x, party->position.z, PV(party, 0x94).x, PV(party, 0x94).z);
        }
        break;
    case 54:
        noko_spin(party);
        party->field_104 += 0.5f;
        if (party->field_104 >= 4.0f) {
            party->field_104 = 4.0f;
        }
        (*(f32*)new_var) -= party->field_104;
        if ((*(f32*)new_var) <= 10.0f) {
            party->useMotionId = 70;
            party->field_104 = 0.0f;
            return;
        }
        break;
    case 55:
        if (ccwall(party, 1) != NULL) {
            party->useMotionId = 70;
            return;
        }
        noko_spin(party);
        PF(party, 0x178) -= 2.2f;
        if (PF(party, 0x178) <= 0.0f) {
            PF(party, 0x178) = 0.0f;
        }
        party->field_104 += (-2.0f - party->field_104) * (PH(party, 0x17c) != 0 ? 0.2f : 0.1f);
        if (party->field_104 <= 0.0f) {
            party->useMotionId = 70;
            party->field_104 = 0.0f;
            return;
        }
        break;
    case 70:
        dx = party->position.x - mario->position.x;
        dy = party->position.y - mario->position.y;
        dz = party->position.z - mario->position.z;
        qx = dx * dx;
        qy = dy * dy;
        qz = dz * dz;
        if ((f32)sqrt(qz + (qx + qy)) < 5.0f) {
            f32 a;
            a = mario->directionView + (mario->directionView >= 180.0 ? -90.0f : 90.0f);
            movePos(&party->position.x, &party->position.z, 0.5f * mario->wPlayerCollisionBox.x, a);
            ccwall(party, 1);
        }
        party->field_104 = 0.0f;
        party->flags2 &= ~0x10;
        party->flags2 |= 0x20;
        party->useMotionId = 71;
        break;
    case 71:
        if (PW(party, 0x174) != 0) {
            itemNokoForceGet((ItemEntry*)PW(party, 0x174));
            PW(party, 0x174) = 0;
        }
        nokonoko_finish(party);
        return;
    }

    switch (party->useMotionId) {
    case 80:
        party->useMotionId = 81;
        party->field_104 = 0.0f;
        party->field_100 = angleABf(party->position.x, party->position.z, PV(party, 0x94).x, PV(party, 0x94).z);
        (*(f32*)new_var) += 75.0f;
        psndSFXOn_3D((const char*)0x92b, &party->position);
    case 81:
        noko_spin(party);
        party->field_104 += 3.0f;
        if (party->field_104 >= 18.0f) {
            party->field_104 = 18.0f;
        }
        NOKO_ITEM(party, psndSFXOn_3D((const char*)0x92c, &party->position); noko_star(party, 1);)
        hit = ccwall(party, 1);
        if (hit != NULL) {
            if (!(hitGetAttr(hit) & 0x80000000)) {
                party->useMotionId = 100;
                return;
            }
            mobjHitObjPtrToPtr(hit);
            party->useMotionId = 110;
            return;
        }
        (*(f32*)new_var) -= party->field_104;
        if ((*(f32*)new_var) <= 20.0f) {
            party->useMotionId = 82;
        }
        break;
    case 82:
        noko_spin(party);
        party->field_104 += 0.35f * (-1.0f - party->field_104);
        NOKO_ITEM(party, psndSFXOn_3D((const char*)0x92c, &party->position); noko_star(party, 1);)
        if (ccwall(party, 1) != NULL) {
            party->useMotionId = 100;
            break;
        }
        (*(f32*)new_var) -= party->field_104;
        if ((*(f32*)new_var) <= 0.0f) {
            (*(f32*)new_var) = distABf(PV(party, 0x94).x, PV(party, 0x94).z, party->position.x, party->position.z);
            party->useMotionId = 84;
            PF(party, 0x178) = 50.0f;
            party->field_100 = angleABf(party->position.x, party->position.z, PV(party, 0x94).x, PV(party, 0x94).z);
        }
        break;
    case 84:
        noko_spin(party);
        if (ccwall(party, 1) != NULL) {
            party->useMotionId = 100;
            break;
        }
        (*(f32*)new_var) -= party->field_104;
        if ((*(f32*)new_var) <= 30.0f) {
            party->useMotionId = 86;
            party->field_104 = 0.0f;
            PS32(party, 0x24) = sysMsec2Frame(500);
        }
        break;
    case 86:
        noko_spin(party);
        if (--PS32(party, 0x24) <= 0) {
            party->useMotionId = 100;
        }
        if (ccwall(party, 1) != NULL) {
            party->useMotionId = 100;
        }
        break;
    case 100:
        ccwall(party, 1);
        dy = party->position.x - mario->position.x;
        dx = party->position.y - mario->position.y;
        dz = party->position.z - mario->position.z;
        qx = dy * dy;
        qy = dx * dx;
        qz = dz * dz;
        if ((f32)sqrt(qz + (qx + qy)) < 5.0f) {
            f32 a;
            a = mario->directionView + (mario->directionView >= 180.0 ? 90.0f : -90.0f);
            movePos(&party->position.x, &party->position.z, 0.5f * mario->wPlayerCollisionBox.x, a);
            ccwall(party, 1);
        }
        party->field_104 = 0.0f;
        party->flags2 &= ~0x10;
        if (PW(party, 0x174) != 0) {
            itemNokoForceGet((ItemEntry*)PW(party, 0x174));
            PW(party, 0x174) = 0;
        }
        if (party->field_138 == NULL) {
            party->useMotionId = 88;
            break;
        }
        if (PW(party, 0x174) != 0) {
            itemNokoForceGet((ItemEntry*)PW(party, 0x174));
            PW(party, 0x174) = 0;
        }
        nokonoko_finish(party);
        return;
    case 88:
        party->field_104 = 0.0f;
        flags = lookupSafetyPos(PV(party, 0x94), party->field_F0, &out2);
        if (party->field_138 != NULL && ((u32)(flags - 1) <= 1 || flags == 4 || flags == 8)) {
            party->useMotionId = 70;
            break;
        }
        switch (flags) {
        case 3:
            lookupSafetyPosSub(PV(party, 0x94), party->field_F0, 135.0f, &out2);
            break;
        case 5:
            lookupSafetyPosSub(PV(party, 0x94), party->field_F0, 315.0f, &out2);
            break;
        case 10:
            lookupSafetyPosSub(PV(party, 0x94), party->field_F0, 225.0f, &out2);
            break;
        case 12:
            lookupSafetyPosSub(PV(party, 0x94), party->field_F0, 315.0f, &out2);
            break;
        case 7:
        case 14:
        case 15:
            party->useMotionId = 70;
            break;
        case 1:
            lookupSafetyPosSub(PV(party, 0x94), party->field_F0, 270.0f, &out2);
            break;
        case 8:
            lookupSafetyPos3(PV(party, 0x94), party->field_F0, &out2);
            break;
        default:
            lookupSafetyPos2(PV(party, 0x94), party->field_F0, &out2);
            break;
        }
        party->useMotionId = 90;
        PV(party, 0x94) = out2;
        (*(f32*)new_var) = distABf(party->position.x, party->position.z, PV(party, 0x94).x, PV(party, 0x94).z);
        party->field_100 = angleABf(party->position.x, party->position.z, PV(party, 0x94).x, PV(party, 0x94).z);
        PF(party, 0x178) = 50.0f;
        break;
    case 90:
        noko_spin(party);
        party->field_104 += 0.5f;
        if (party->field_104 >= 4.0f) {
            party->field_104 = 4.0f;
        }
        (*(f32*)new_var) -= party->field_104;
        if ((*(f32*)new_var) <= 10.0f) {
            party->useMotionId = 70;
            party->field_104 = 0.0f;
        }
        break;
    case 110:
        party->field_104 = 0.0f;
        PF(party, 0x114) = 15.0f;
        PF(party, 0x118) = 0.0f;
        PS32(party, 0x24) = sysMsec2Frame(1000);
        party->useMotionId = 111;
    case 111:
        party->position.y += PF(party, 0x114);
        marioGetScreenPos(party->position, &s2x, &s2y, &s2z);
        if (s2y <= -300.0f) {
            PS32(party, 0x24) = sysMsec2Frame(800);
            party->useMotionId = 112;
        }
        break;
    case 112:
        if (--PS32(party, 0x24) > 0) {
            break;
        }
        if (!(party->flags & 0x80000000)) {
            partyGetReturnPos(&rp);
        } else {
            rp = mario->position;
            b = FALSE;
            if (mario->unk1AC >= 90.0f && mario->unk1AC <= 270.0f) {
                b = TRUE;
            }
            movePos(&rp.x, &rp.z, 30.0f, toMovedir2(b ? 0.0f : 180.0f, -30.0f));
        }
        party->position.x = rp.x;
        party->position.y = 1000.0f + rp.y;
        party->position.z = rp.z;
        PF(party, 0x114) = 0.0f;
        PF(party, 0x118) = -1.0f;
        party->field_104 = 0.0f;
        party->flags |= 0x20;
        party->flags |= 0x02000000;
        party->flags &= ~0x04000000;
        party->flags &= ~0x01000000;
        party->flags2 &= ~0x20;
        party->flags2 &= ~0x10;
        party->field_F0 = 37.0f;
        party->useMotionId = 113;
        break;
    case 113:
        marioGetScreenPos(party->position, &s3x, &s3y, &s3z);
        if (s3y > 240.0f) {
            party->flags &= ~0x02000000;
        }
        partyMove(party);
        if (PF(party, 0x114) <= -6.0f) {
            PF(party, 0x118) = 0.0f;
            PF(party, 0x114) = -6.0f;
        }
        if (party->position.y < -2000.0f) {
            party->position = mario->position;
            party->position.y += 200.0f;
        }
        if (party->field_138 != NULL) {
            party->field_110 = mario->unk1AC;
            party->field_10C = party->field_110;
            if (!((hitGetAttr(party->field_138) >> 9) & 1)) {
                party->useMotionId = 114;
                PS32(party, 0x24) = 30;
            }
        }
        break;
    case 114:
        if (--PS32(party, 0x24) > 0) {
            break;
        }
        nokonoko_finish(party);
        return;
    }
    if (party->useMotionId != 100) {
        partyMoveNoHosei(party);
    }
}

void nokonoko_finish(PartyEntry* party) {
    if (party != NULL && party->currentMemberId == 2 && (party->flags & 0x100)) {
        nokonoko_use_post(party);
        if (party->field_34 != 9) {
            if (!(party->flags & 0x80000000)) {
                partyChgRunMode(party, 2);
                PB(party, 0x3b) = 0;
            } else {
                party->flags &= 0x7FFFFFFF;
                partyChgRunMode(party, 0);
            }
        }
    }
}

void nokotaro_hold_cancel(PartyEntry* party) {
    party->useMotionId = 20;
}

HitEntry *ccwall(PartyEntry *party, s32 mode)
{
  f32 s;
  f32 c;
  f32 dist;
  f32 hx;
  f32 hy;
  f32 hz;
  f32 nx;
  f32 ny;
  f32 nz;
  f32 s1;
  f32 c1;
  f32 s2;
  f32 c2;
  f32 s3;
  f32 c3;
  HitEntry *hit;
  f32 py;
  f32 vx;
  f32 vz;
  f32 d;
  f32 y0;
  f32 y;
  f32 z;
  f32 x;
  f32 yofs;
  f32 w;
  yofs = 10.0f;
  y0 = party->position.y + party->field_F0;
  x = party->position.x;
  dist = party->field_F0 + 30.0f;
  y0 = yofs + y0;
  hit = hitCheckFilter((void *) 0, &hx, &hy, &hz, &dist, &nx, &ny, &nz, x, y0, party->position.z, 0.0f, -1.0f, 0.0f);
  if (((hitGetAttr(hit) >> 9) & 1) == 1)
  {
    hit = (void *) 0;
  }
  party->field_138 = hit;
  if (hit != ((void *) 0))
  {
    hy = ((f32) ((s32) ((100.0f * hy) + 0.5f))) / 100.0f;
    py = ((f32) ((s32) ((100.0f * party->position.y) + 0.5f))) / 100.0f;
    if (__fabsf(py - hy) <= 11.0f)
    {
      if (py <= hy)
      {
        if (mode == 0)
        {
          party->position.y = hy;
          if (__fabsf(py - hy) >= 1.0f)
          {
            *((s16 *) (((u8 *) party) + 0x17e)) |= 2;
          }
        }
      }
      else
      {
        if (((*((s16 *) (((u8 *) party) + 0x17e))) & 2) && (mode == 1))
        {
          party->position.y = hy;
        }
        if (__fabsf(py - hy) <= 6.0f)
        {
          party->position.y = hy;
        }
      }
    }
  }
  *((u32 *) (((u8 *) party) + 0x130)) = 0;
  *((u32 *) (((u8 *) party) + 0x12c)) = 0;
  yofs += 1.0f;
  x = party->position.x;
  z = party->position.z;
  y = party->position.y + yofs;
  dist = (0.5f * party->width) + party->field_104;
  sincosf(&s, &c, party->field_100);
  hit = hitCheckFilter((void *) 0, &hx, &hy, &hz, &dist, &nx, &ny, &nz, x, y, z, s, 0.0f, c);
  if ((hit != ((void *) 0)) && (!(hitGetAttr(hit) & 5)))
  {
    sincosf(&s1, &c1, 180.0f + party->field_100);
    party->position.x = hx + (0.5f * (s1 * party->width));
    party->position.z = hz + (0.5f * (c1 * party->width));
    vx = s * party->field_104;
    vz = c * party->field_104;
    d = (vx * nx) + (vz * nz);
    party->position.x += 0.5f * (vx - (d * nx));
    party->position.z += 0.5f * (vz - (d * nz));
    if (((*((s16 *) (((u8 *) party) + 0x17c))) != 0) || (mode == 0))
    {
      *((u32 *) (((u8 *) party) + 0x130)) = (u32) hit;
    }
    *((u32 *) (((u8 *) party) + 0x12c)) = (u32) hit;
    return hit;
  }
  w = 0.4f * party->width;
  z = (w * ((f32) cos((3.1415927f * (party->field_100 - 90.0f)) / 180.0f))) + party->position.z;
  x = (w * ((f32) sin((3.1415927f * (party->field_100 - 90.0f)) / 180.0f))) + party->position.x;
  y = party->position.y + yofs;
  dist = (0.4f * party->width) + party->field_104;
  sincosf(&s, &c, party->field_100);
  hit = hitCheckFilter((void *) 0, &hx, &hy, &hz, &dist, &nx, &ny, &nz, x, y, z, s, 0.0f, c);
  if ((hit != ((void *) 0)) && (!(hitGetAttr(hit) & 5)))
  {
    sincosf(&s2, &c2, 180.0f + party->field_100);
    party->position.x = hx + (0.5f * (s2 * party->width));
    party->position.z = hz + (0.5f * (c2 * party->width));
    party->position.z -= w * ((f32) cos((3.1415927f * (party->field_100 - 90.0f)) / 180.0f));
    party->position.x -= w * ((f32) sin((3.1415927f * (party->field_100 - 90.0f)) / 180.0f));
    vx = s * party->field_104;
    vz = c * party->field_104;
    d = (vx * nx) + (vz * nz);
    party->position.x += 0.5f * (vx - (d * nx));
    party->position.z += 0.5f * (vz - (d * nz));
    if (((*((s16 *) (((u8 *) party) + 0x17c))) != 0) || (mode == 0))
    {
      *((u32 *) (((u8 *) party) + 0x130)) = (u32) hit;
    }
    *((u32 *) (((u8 *) party) + 0x12c)) = (u32) hit;
    party->position.x = hx;
    return hit;
  }
  w = 0.4f * party->width;
  z = (w * ((f32) cos((3.1415927f * (90.0f + party->field_100)) / 180.0f))) + party->position.z;
  x = (w * ((f32) sin((3.1415927f * (90.0f + party->field_100)) / 180.0f))) + party->position.x;
  y = party->position.y + yofs;
  dist = (0.4f * party->width) + party->field_104;
  dist = (0.4f * party->width) + party->field_104;
  sincosf(&s, &c, party->field_100);
  hit = hitCheckFilter((void *) 0, &hx, &hy, &hz, &dist, &nx, &ny, &nz, x, y, z, s, 0.0f, c);
  if ((hit != ((void *) 0)) && (!(hitGetAttr(hit) & 5)))
  {
    sincosf(&s3, &c3, 180.0f + party->field_100);
    party->position.x = hx + (0.5f * (s3 * party->width));
    party->position.z = hz + (0.5f * (c3 * party->width));
    party->position.z -= w * ((f32) cos((3.1415927f * (90.0f + party->field_100)) / 180.0f));
    party->position.x -= w * ((f32) sin((3.1415927f * (90.0f + party->field_100)) / 180.0f));
    vx = s * party->field_104;
    vz = c * party->field_104;
    d = (vx * nx) + (vz * nz);
    party->position.x += 0.5f * (vx - (d * nx));
    party->position.z += 0.5f * (vz - (d * nz));
    if (((*((s16 *) (((u8 *) party) + 0x17c))) != 0) || (mode == 0))
    {
      *((u32 *) (((u8 *) party) + 0x130)) = (u32) hit;
    }
    *((u32 *) (((u8 *) party) + 0x12c)) = (u32) hit;
    party->position.x = hx;
    return hit;
  }
  return (void *) 0;
}

void nokoStrippedPool(f32* p) { p[0] = 10.0f; p[1] = 30.0f; p[2] = 100.0f; p[3] = 11.0f; p[4] = 6.0f; }
u32 lookupSafetyPos(Vec pos, f32 height, Vec* out) {
    Vec p[4];
    f32 dist;
    f32 hx; f32 hy; f32 hz; f32 nx; f32 ny; f32 nz;
    f32 rad;
    f32 x; f32 y; f32 z; f32 c;
    f32 w;
    HitEntry* h0;
    HitEntry* h1;
    HitEntry* h2;
    HitEntry* h3;
    u32 flags;
    MarioWork* player = marioGetPtr();
    w = player->wPlayerCollisionBox.x;
    rad = (3.1415927f * revise360(toMovedir(player->unk1AC) - 180.0f)) / 180.0f;
    x = w * (f32)sin(rad) + pos.x;
    p[3].x = x;
    c = (f32)cos(rad);
    y = pos.y;
    p[3].y = y;
    z = w * -c + pos.z;
    p[3].z = z;
    dist = 0.1f + height;
    h0 = hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, x, y + height, z, 0.0f, -1.0f, 0.0f);
    rad = (3.1415927f * revise360(toMovedir(player->unk1AC) - 90.0f)) / 180.0f;
    x = w * (f32)sin(rad) + pos.x;
    p[2].x = x;
    c = (f32)cos(rad);
    y = pos.y;
    p[2].y = y;
    z = w * -c + pos.z;
    p[2].z = z;
    dist = 0.1f + height;
    h1 = hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, x, y + height, z, 0.0f, -1.0f, 0.0f);
    rad = (3.1415927f * revise360(90.0f + toMovedir(player->unk1AC))) / 180.0f;
    x = w * (f32)sin(rad) + pos.x;
    p[1].x = x;
    c = (f32)cos(rad);
    y = pos.y;
    p[1].y = y;
    z = w * -c + pos.z;
    p[1].z = z;
    dist = 0.1f + height;
    h2 = hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, x, y + height, z, 0.0f, -1.0f, 0.0f);
    rad = (3.1415927f * revise360(toMovedir(player->unk1AC))) / 180.0f;
    x = w * (f32)sin(rad) + pos.x;
    p[0].x = x;
    c = (f32)cos(rad);
    y = pos.y;
    p[0].y = y;
    z = w * -c + pos.z;
    p[0].z = z;
    dist = 0.1f + height;
    h3 = hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, x, y + height, z, 0.0f, -1.0f, 0.0f);
    flags = 0;
    if (h2) flags |= 4;
    if (h3) flags |= 8;
    if (h0) flags |= 1;
    if (h1) flags |= 2;
    if (h0) {
        if (h1 && h2 && h3) {
        out->x = 0.5f * (p[3].x + p[1].x); out->z = 0.5f * (p[3].z + p[1].z); out->y = p[3].y; return flags;
        }
        if (h1) {
        out->x = 0.5f * (p[3].x + p[2].x); out->z = 0.5f * (p[3].z + p[2].z); out->y = pos.y; return flags;
        }
        if (h2) {
        out->x = 0.5f * (p[3].x + p[1].x); out->z = 0.5f * (p[3].z + p[1].z); out->y = p[3].y; return flags;
        }
        if (h3) {
        out->x = 0.5f * (p[3].x + p[0].x); out->z = 0.5f * (p[3].z + p[0].z); out->y = p[3].y; return flags;
        }
        *out = p[3];
    } else if (h3) {
        if (h1) {
        out->x = 0.5f * (p[0].x + p[2].x); out->z = 0.5f * (p[0].z + p[2].z); out->y = p[0].y; return flags;
        }
        if (h2) {
        out->x = 0.5f * (p[1].x + p[2].x); out->z = 0.5f * (p[1].z + p[2].z); out->y = p[1].y; return flags;
        }
        *out = p[0];
    } else {
        if (h1) {
            *out = p[2];
            return flags;
        }
        if (h2) {
            *out = p[1];
            return flags;
        }
    }
    *out = pos;
    return flags;
}

void lookupSafetyPos2(Vec pos, f32 height, Vec* out) {
    f32 s; f32 c; f32 rad; f32 dist; f32 w; f32 x; f32 z; f32 y; f32 hx; f32 hy; f32 hz; f32 nx; f32 ny; f32 nz;
    MarioWork* player = marioGetPtr();
    w = player->wPlayerCollisionBox.x;
    rad = 1.5707964f;
    s = (f32)sin(rad);
    c = (f32)cos(rad);
    c = -c; s = w * s; c = w * c; x = s + pos.x; y = pos.y; z = pos.z + c;
    dist = 0.1f + height;
    if (hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, x, y + height, z, 0.0f, -1.0f, 0.0f) != NULL) {
        out->x = x; out->z = z; out->y = y; return;
    }
    rad = 4.712389f;
    s = (f32)sin(rad);
    c = (f32)cos(rad);
    c = -c; s = w * s; c = w * c; x = pos.x + s; y = pos.y; z = pos.z + c;
    dist = 0.1f + height;
    if (hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, x, y + height, z, 0.0f, -1.0f, 0.0f) != NULL) {
        out->x = x; out->z = z; out->y = y; return;
    }
    rad = 3.1415927f;
    s = (f32)sin(rad);
    c = (f32)cos(rad);
    x = pos.x; y = pos.y; z = pos.z; y = pos.y;
    dist = 0.1f + height;
    if (hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, x, y + height, z, 0.0f, -1.0f, 0.0f) != NULL) {
        out->x = x; out->z = z; out->y = y; return;
    }
    rad = 0.0f;
    s = (f32)sin(rad);
    c = (f32)cos(rad);
    c = -c; s = w * s; c = w * c; x = pos.x + s; y = pos.y; z = pos.z + c;
    dist = 0.1f + height;
    if (hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, x, y + height, z, 0.0f, -1.0f, 0.0f) != NULL) {
        out->x = x; out->z = z; out->y = y; return;
    }
    *out = pos; }

void lookupSafetyPos3(Vec pos, f32 height, Vec* out) {
    f32 y; f32 s; f32 dist; f32 z; f32 c; f32 rad; f32 hx; f32 hy; f32 hz; f32 nx; f32 ny; f32 nz;
    MarioWork* player = marioGetPtr();
    f32 w = player->wPlayerCollisionBox.x;
    rad = 1.5707964f;
    s = (f32)sin(rad);
    c = (f32)cos(rad);
    c = -c; s = w * s; c = w * c; s = s + pos.x; z = pos.z + c; y = pos.y;
    dist = 0.1f + height;
    if (hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, s, y + height, z, 0.0f, -1.0f, 0.0f) != NULL) {
        out->x = s; out->z = z; out->y = y; return;
    }
    rad = 4.712389f;
    s = (f32)sin(rad);
    c = (f32)cos(rad);
    c = -c; s = w * s; c = w * c; s = s + pos.x; z = pos.z + c; y = pos.y;
    dist = 0.1f + height;
    if (hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, s, y + height, z, 0.0f, -1.0f, 0.0f) != NULL) {
        out->x = s; out->z = z; out->y = y; return;
    }
    *out = pos; }

void lookupSafetyPosSub(Vec pos, f32 height, f32 angle, Vec* out) {
    f32 rad;
    f32 x;
    f32 y;
    f32 z;
    f32 dist;
    f32 hx;
    f32 hy;
    f32 hz;
    f32 nx;
    f32 ny;
    f32 nz;
    Vec* p;
    MarioWork* player = marioGetPtr();
    rad = (3.1415927f * angle) / 180.0f;
    angle = player->wPlayerCollisionBox.x;
    x = angle * (f32)sin(rad) + pos.x;
    z = angle * -(f32)cos(rad) + pos.z;
    y = pos.y + height;
    dist = 0.1f + height;
    if (hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, x, y, z, 0.0f, -1.0f, 0.0f) != NULL) {
        Vec v = {0.0f, 0.0f, 0.0f};
        v.x = x;
        v.y = y;
        v.z = z;
        p = &v;
    } else {
        p = &pos;
    }
    *out = *p;
}

