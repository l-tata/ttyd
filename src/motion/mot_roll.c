#pragma defer_codegen on
#include "dolphin/types.h"
#include "driver/hitdrv.h"
#include "mario/mario.h"
#include "mario/mario_cam.h"
#include "mario/mario_motion.h"
#include "mario/mariost.h"
#include "memory.h"
#include "pmario_sound.h"
#include <string.h>

f32 revise360(f32);
f32 distABf(f32, f32, f32, f32);
f32 angleABf(f32, f32, f32, f32);
f32 toMovedir(f32);
void sincosf(f32* s, f32* c, f32 deg);
double sin(double);
double cos(double);
BOOL marioChkBero(HitEntry* hit);
HitEntry* marioHitCheckVec(Vec* pos, Vec* dir, Vec* outPos, Vec* outNrm, f32* dist);
HitEntry* marioChkLandon(f32* outY, f32 y);
f32 marioGetFallSpd(void);
void marioLandOn(void);
void marioClearJumpPara(void);
void marioChgMotSub(s32 a, s32 b);
void marioGetRubInit(void);
void allPartyRideShip(void);
void allPartyRideOff(void);
void allPartyForceRideOn(void);
s32 marioSlitChkWallAround(void);
void marioUpdateCamCollision(Vec* pos);
void camFollowYOff(void);
void camFollowYOn(void);
void marioOfsRotReset(void);
void marioSetSpec(void);
void marioAdjustMoveDir(void);
extern int __float_nan[];

#define PF(p, o) (*(f32*)((u8*)(p) + (o)))
#define PI(p, o) (*(s32*)((u8*)(p) + (o)))
#define MF(o) (*(f32*)((u8*)mario + (o)))
#define MI(o) (*(s32*)((u8*)mario + (o)))
#define MH(o) (*(s16*)((u8*)mario + (o)))
#define MV(o) (*(Vec*)((u8*)mario + (o)))
#define MB(o) (*(s8*)((u8*)mario + (o)))
#define MU(o) (*(u16*)((u8*)mario + (o)))
#define CAMF() (*(u16*)camGetPtr(8))

typedef struct RollWork {
    f32 x;
    f32 z;
    s32 state;
    s32 scaleState;
    s32 pad;
} RollWork;

#define RW() ((RollWork*)marioGetPtr()->motStruct)

static const Vec rollOfs = {0.0f, 40.0f, 0.0f};


f32 roll_grx;
f32 roll_grz;
s32 roll_flyout_flag2;

f32 rollData[] = {3.0f, -0.05f, -0.5f, 0.5f, 7.0f, 180.0f, 140.0f, 10.0f, 1.2f, 2.0f, 0.8f};

static inline s32 roll_fpclassifyf(f32 x) {
    switch (*(s32*)&x & 0x7F800000) {
    case 0x7F800000:
        if (*(s32*)&x & 0x007FFFFF) {
            return 1;
        } else {
            return 2;
        }
        break;
    case 0:
        if (*(s32*)&x & 0x007FFFFF) {
            return 5;
        } else {
            return 3;
        }
        break;
    default:
        return 4;
    }
}

static inline f32 roll_sqrtf(f32 x) {
    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return *(f32*)__float_nan;
    } else if (roll_fpclassifyf(x) == 1) {
        return *(f32*)__float_nan;
    }
    return x;
}

static inline f32 trunc10(f32 x) {
    s32 i = (s32)(10.0f * x);
    f32 f = (f32)i;
    return f / 10.0f;
}

static inline s32 angOutT(f32 center, f32 half, f32 x) {
    s32 w = 0;
    s32 r = w;
    f32 c = trunc10(center);
    f32 lo;
    f32 hi;
    lo = c - half;
    if (lo < 0.0f) {
        w = 1;
        lo += 360.0f;
    }
    hi = c + half;
    if (hi > 360.0f) {
        hi -= 360.0f;
        w = 2;
    }
    if (w == 1) {
        if (hi < x && x < lo) {
            r = 1;
        }
    } else if (w == 2) {
        if (hi < x && x < lo) {
            r = 1;
        }
    } else if (w == 0) {
        if (x < lo || hi < x) {
            r = 1;
        }
    }
    return r;
}

static inline s32 angOut(f32 center, f32 half, f32 x) {
    return angOutT(center, half, x);
}

/* repurposed unused inline: angOut on the mario stick angle, with everything read inside */
static inline s32 makeVec(f32 half) {
    s32 w = 0;
    s32 r = w;
    MarioWork* m = marioGetPtr();
    f32 c = trunc10(m->unk19C);
    f32 x = angleABf(0.0f, 0.0f, (f32)(s32)*((s8*)m + 0x24e), (f32)(s32)*((s8*)m + 0x24f));
    f32 lo;
    f32 hi;
    lo = c - half;
    if (lo < 0.0f) {
        w = 1;
        lo += 360.0f;
    }
    hi = c + half;
    if (hi > 360.0f) {
        hi -= 360.0f;
        w = 2;
    }
    if (w == 1) {
        if (hi < x && x < lo) {
            r = 1;
        }
    } else if (w == 2) {
        if (hi < x && x < lo) {
            r = 1;
        }
    } else if (w == 0) {
        if (x < lo || hi < x) {
            r = 1;
        }
    }
    return r;
}

#define DISPDIR_BODY(a, b, m2)                                                                    \
    if (angOut(b, 135.0f, a) == 1) {                                                              \
        b = revise360(180.0f + b);                                                                \
    }                                                                                             \
    {                                                                                             \
        f32 ty_ = trunc10((m2)->targetYaw); f32 dd = revise360(-(ty_ - trunc10(b))); \
        s32 sgn = (dd < 180.0f) ? 0 : 1;                                                          \
        if (sgn == 0) {                                                                           \
            if (__fabsf(b - a) > 90.0f) {                                                            \
                if (a > b) {                                                                      \
                    b += 360.0f;                                                                  \
                }                                                                                 \
            }                                                                                     \
        } else {                                                                                  \
            if (__fabsf(b - a) > 90.0f) {                                                            \
                if (a < b) {                                                                      \
                    a += 360.0f;                                                                  \
                }                                                                                 \
            }                                                                                     \
        }                                                                                         \
    }                                                                                             \
    if (angOut(b, 90.0f, a) == 0) {                                                               \
        if (__fabsf(b - a) > 180.0f) {                                                               \
            if (b > a) {                                                                          \
                b -= 360.0f;                                                                      \
            } else {                                                                              \
                a -= 360.0f;                                                                      \
            }                                                                                     \
        }                                                                                         \
    }                                                                                             \
    b = 1.2f * (a - b) + b;                                                                       \
    b = revise360(b);                                                                             \
    {                                                                                             \
        f32 md = toMovedir(MF(0x1a8));                                                            \
        if (MB(0x43)) {                                                                           \
            md = revise360(180.0f + md);                                                          \
        }                                                                                         \
        if (angOut(b, 135.0f, md) == 1) {                                                         \
            MB(0x43) = (MB(0x43) + 1) & 1;                                                        \
        }                                                                                         \
    }                                                                                             \
    { f32 t_ = 270.0f + MF(0x198) - b; MF(0x1a8) = revise360(t_ + (MB(0x43) ? -180.0f : 0.0f)); } \
    if (MF(0x1a8) >= 200.0f) {                                                                    \
        MF(0x1a8) = revise360(270.0f + MF(0x198) - b + (MB(0x43) ? -180.0f : 0.0f));              \
    }                                                                                             \
    if (MF(0x1a8) < 170.0f) {                                                                     \
        MF(0x1a8) = revise360(270.0f + MF(0x198) - b + (MB(0x43) ? -180.0f : 0.0f));              \
    }

void roll_upstairs(void);
static inline void marioRollCan(void);


#pragma inline_max_size(20000)
#pragma inline_max_total_size(200000)


void marioRollFixOn(void) {
    MarioWork* mario = marioGetPtr();
    if (mario->currMotionId == 0x16) {
        MI(0x2a4) |= 0x200;
    }
}

s32 unk_8009fcbc(void) {
    MarioWork* mario = marioGetPtr();
    if (MI(0x2a4) & 0x200) {
        return 1;
    }
    if ((mario->flags & 0x8000000) != 0) {
        return 1;
    }
    return !mario->hitobjStandOn ? 1 : (marioSlitChkWallAround() != 0);
}

s32 marioRollKeyDisable(void) {
    MarioWork* mario = marioGetPtr();
    if (mario->currMotionId != 0x16) {
        return 1;
    }
    {
        s32 sub = MI(0x44);
        if (sub >= 0 && sub <= 1) {
            return 0;
        }
        if (sub >= 0x14 && sub <= 0x16) {
            return 0;
        }
    }
    return 1;
}

void setRollEvtFlag(void) {
    MarioWork* mario = marioGetPtr();
    MI(0x2a4) |= 0x100;
}

void clrRollEvtFlag(void) {
    MarioWork* mario = marioGetPtr();
    MI(0x2a4) &= ~0x100;
}

s32 getRollEvtFlag(void) {
    s32 r = 0;
    MarioWork* mario = marioGetPtr();
    if (mario->flags & 0x1000000) {
        if (MI(0x2a4) & 0x100) {
            r = 1;
        }
    }
    return r;
}

void marioResetRollSpd(void) {
    MarioWork* mario = marioGetPtr();
    if (mario->currMotionId == 0x16) {
        if (marioGetPtr()->motStruct) {
            f32 a = 0.0f;
            f32 b;
            RW()->x = a;
            b = 0.0f;
            RW()->z = b;
        }
        MF(0x180) = 0.0f;
    }
}

void marioForceRollAnime(void) {
    MarioWork* mario = marioGetPtr();
    marioPaperOff();
    if (mario) {
    }
    marioChgPaper(0);
    marioPaperOn("p_roll");
    marioChgPose("M_I_U");
    marioChgPaper("PM_R_1B");
    mario->dispFlags |= 8;
    mario->dispFlags |= 0x100;
}

s32 unk_8009f9c8(void) {
    s32 ret = 0;
    MarioWork* mario = marioGetPtr();
    if (!(mario->flags & 0x1000000)) {
        return 0;
    }
    if (MI(0x2a4) & 0x200) {
        return 0;
    }
    {
        s32 r;
        MarioWork* m = marioGetPtr();
        if (*(s32*)((u8*)m + 0x2a4) & 0x200) {
            r = 1;
        } else if (m->flags & 0x8000000) {
            r = 1;
        } else if (!m->hitobjStandOn) {
            r = 1;
        } else if (marioSlitChkWallAround()) {
            r = 1;
        } else {
            r = 0;
        }
        if (!r) {
            ret = 1;
            mario->currSubMotionId = 0x14;
            CAMF() |= 0x200;
        }
    }
    return ret;
}


s32 marioRollCancel(void) {
    MarioWork* mario = marioGetPtr();
    if (!(mario->flags & 0x1000000)) {
        return 0;
    }
    if (MI(0x2a4) & 0x200) {
        return 0;
    }
    mario->currSubMotionId = 0x14;
    CAMF() |= 0x200;
    return 1;
}

void mot_roll_post(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* m = marioGetPtr();
    if (*(u32*)((u8*)m + 0x2a8) != (u32)-1) {
        psndSFXOff(*(s32*)((u8*)m + 0x2a8));
        *(s32*)((u8*)m + 0x2a8) = -1;
    }
    m = marioGetPtr();
    m->unk148 = 0.0f;
    m->unk150.z = 0.004f;
    {
        f32 d = revise360(mario->directionView - mario->targetYaw);
        do { if (d >= 180.0f) { mario->unk1A8 = 0.0f; mario->unk1AC = 0.0f; mario->unk19C = 270.0f; mario->directionView = 270.0f; } else { mario->unk1A8 = 180.0f; mario->unk1AC = 180.0f; mario->unk19C = 90.0f; mario->directionView = 90.0f; } } while (0);
    }
    marioSetSpec();
    mario->field_0x8 = 0;
    mario->rotOfs = (Vec){0.0f, 0.0f, 0.0f};
    mario->field_0x98 = (Vec){0.0f, 0.0f, 0.0f};
    mario->rot = (Vec){0.0f, 0.0f, 0.0f};
    mario->dispFlags &= ~0x100;
    mario->dispFlags &= ~8;
    mario->flags &= ~0x1000000;
    marioPaperOff();
    marioAdjustMoveDir();
    marioChgPose("M_S_1");
    if (marioGetPtr()->motStruct) {
        __memFree(0, marioGetPtr()->motStruct);
        mario = 0;
        marioGetPtr()->motStruct = (void*)mario;
    }
    allPartyRideOff();
}

void marioRollForceCancel(void) {
    MarioWork* mario = marioGetPtr();
    if (mario->currMotionId == 0x16) {
        MarioWork* m = marioGetPtr();
        if (*(u32*)((u8*)m + 0x2a8) != (u32)-1) {
            psndSFXOff(*(s32*)((u8*)m + 0x2a8));
            *(s32*)((u8*)m + 0x2a8) = -1;
        }
        m = marioGetPtr();
        m->unk148 = 0.0f;
        m->unk150.z = 0.004f;
        {
            f32 d = revise360(mario->directionView - mario->targetYaw);
            if (d >= 180.0f) {
                mario->unk1A8 = 0.0f;
                mario->unk1AC = 0.0f;
                mario->unk19C = 270.0f;
                do { mario->directionView = 270.0f; } while (0);
            } else {
                mario->unk1A8 = 180.0f;
                mario->unk1AC = 180.0f;
                mario->unk19C = 90.0f;
                mario->directionView = 90.0f;
            }
        }
        marioSetSpec();
        mario->field_0x8 = 0;
        mario->rotOfs = (Vec){0.0f, 0.0f, 0.0f};
        mario->field_0x98 = (Vec){0.0f, 0.0f, 0.0f};
        mario->rot = (Vec){0.0f, 0.0f, 0.0f};
        mario->dispFlags &= ~0x100;
        mario->dispFlags &= ~8;
        mario->flags &= ~0x1000000;
        marioPaperOff();
        marioAdjustMoveDir();
        marioChgPose("M_S_1");
        if (marioGetPtr()->motStruct) {
            __memFree(0, marioGetPtr()->motStruct);
            mario = 0;
            marioGetPtr()->motStruct = (void*)mario;
        }
        allPartyRideOff();
        marioChgMotSub(0, 0);
    }
}

void marioSetRollDispDir(void) {
    MarioWork* mario = marioGetPtr();
    f32 b, a;
    MarioWork* m2;
    a = mario->directionView;
    b = mario->unk19C;
    m2 = marioGetPtr();
    DISPDIR_BODY(a, b, m2)
}

void rollEvtJumpSetup(void) {
    MarioWork* mario = marioGetPtr();
    camFollowYOff();
    mario->flags |= 0x10000;
    mario->airTimer = 1;
}

void marioReInit_roll(void) {
    void* p;
    MarioWork* mario = marioGetPtr();
    f32* new_var;
    f32 z0;
    f32 z1;
    p = __memAlloc(0, 0x14);
    marioGetPtr()->motStruct = p;
    memset(marioGetPtr()->motStruct, 0, 0x14);
    {
        MarioWork* t = marioGetPtr();
        PI(t, 0x2a8) = -1;
    }
    mario->trigFlags &= ~1;
    mario->currMotionId = 0x16;
    mario->flags |= 0x1000000;
    mario->field_0x8 = 0x10;
    marioPaperOn("p_roll");
    marioChgPose("M_I_U");
    marioChgPaper("PM_R_1B");
    mario->dispFlags |= 8;
    mario->dispFlags |= 0x100;
    mario->currSubMotionId = 0x1e;
    mario = marioGetPtr();
    z0 = 0.0f;
    MF(0x1b4) = 14.8f;
    MF(0x1b8) = 14.8f;
    MF(0x188) = rollData[0];
    RW()->x = z0;
    z1 = 0.0f;
    RW()->z = z1;
    *(f32*)(0xb0 + (u8*)mario) = -6.0f;
    MF(0xb4) = 6.0f;
    new_var = (f32*)((u8*)mario + 0x98);
    *new_var = 6.0f;
    MF(0x9c) = 0.0f;
    MF(0xc8) = 1.0f;
    MF(0xcc) = 1.0f;
    MF(0xd0) = 1.0f;
    MF(0x130) = 1.0f;
    allPartyForceRideOn();
}




static inline s32 rollChk(void) {
    s32 r;
    MarioWork* m = marioGetPtr();
    if (PI(m, 0x2a4) & 0x200) {
        r = 1;
    } else if (m->flags & 0x8000000) {
        r = 1;
    } else if (!m->hitobjStandOn) {
        r = 1;
    } else if (marioSlitChkWallAround()) {
        r = 1;
    } else {
        r = 0;
    }
    return r;
}



void mot_roll(void) {
    MarioWork* mario = marioGetPtr();
    f32 v31, v30, v29, v28, v27, v26;
    f32 tcA;
    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0xF0000;
        MI(0x2a4) = 0;
        MI(0x2a8) = 0;
        MI(0x2ac) = 0;
        MI(0x2b0) = 0;
        MI(0x2b4) = 0;
        {
            void* p = __memAlloc(0, 0x14);
            marioGetPtr()->motStruct = p;
        }
        memset(marioGetPtr()->motStruct, 0, 0x14);
        {
            MarioWork* t = marioGetPtr();
            PI(t, 0x2a8) = -1;
        }
        MI(0x44) = 0;
    }
    switch (MI(0x44)) {
    case 0: {
        MarioWork* m;
        s32 z0 = 0;
        s32 z1;
        CAMF() |= 0x200;
        marioAdjustMoveDir();
        v31 = MF(0x1a8);
        if (v31 < 90.0f || v31 > 270.0f) {
            MB(0x43) = 1;
            MF(0x1a0) = revise360(270.0f - MF(0x198));
            MF(0x19c) = 270.0f;
        } else {
            MB(0x43) = 0;
            MF(0x1a0) = revise360(90.0f - MF(0x198));
            MF(0x19c) = 90.0f;
        }
        marioGetRubInit();
        mario->flags |= 0x1000000;
        mario->field_0x8 = 0x10;
        mario->dispFlags |= 8;
        mario->dispFlags |= 0x100;
        marioPaperOn("p_roll");
        marioChgPose("M_I_U");
        marioChgPaper("PM_R_1A");
        MI(0x48) = 0x41;
        m = marioGetPtr();
        v28 = 0.0f;
        PF(m, 0x1b4) = 14.8f;
        PF(m, 0x1b8) = 14.8f;
        PF(m, 0x188) = rollData[0];
        RW()->x = v28;
        v27 = 0.0f;
        RW()->z = v27;
        PF(m, 0xb0) = -6.0f;
        PF(m, 0xb4) = 6.0f;
        PF(m, 0x98) = 6.0f;
        PF(m, 0x9c) = 0.0f;
        PF(m, 0xc8) = 1.0f;
        PF(m, 0xcc) = 1.0f;
        PF(m, 0xd0) = 1.0f;
        PF(m, 0x130) = 1.0f;
        mario->airTimer = 0;
        if (MH(0x4e) == 0) {
            MF(0x180) = 0.0f;
        }
        m = marioGetPtr();
        PF(m, 0x148) = 0.4f;
        PF(m, 0x158) = 0.01f;
        PF(m, 0x68) = 2.0f;
        ((MarioWork*)m)->field_6C = rollOfs;
        RW()->state = z0;
        z1 = 0;
        RW()->scaleState = z1;
        psndSFXOn_3D((char*)0x179, &mario->position);
        psndSFXOn_3D((char*)0x9f, &mario->position);
        MI(0x44) = 1;
        break;
    }
    case 1:
        if (MI(0x48) == 0x36) {
            allPartyRideShip();
        }
        MF(0x9c) += 0.1f;
        if (MF(0x9c) > 1.0f) {
            MF(0x9c) = 1.0f;
        }
        if (--MI(0x48) <= 0) {
            MI(0x44) = 0x1e;
            marioChgPose("M_I_U");
            marioChgPaper("PM_R_1B");
            CAMF() &= ~0x200;
        }
        break;
    case 0x14:
        MF(0x180) = 0.0f;
        MF(0xbc) = 0.0f;
        MF(0xc4) = 0.0f;
        MF(0xb0) = 0.0f;
        MF(0xb4) = 0.0f;
        v31 = revise360(MF(0x1a0) - MF(0x198));
        MF(0x98) = (v31 < 180.0f) ? 3.0f : -3.0f;
        MF(0x9c) = 0.0f;
        MI(0x48) = 0x41;
        MI(0x44) = 0x15;
        MF(0x1a8) = 0.0f;
        {
            MarioWork* m = marioGetPtr();
            if ((u32)PI(m, 0x2a8) != (u32)-1) {
                psndSFXOff(PI(m, 0x2a8));
                PI(m, 0x2a8) = -1;
            }
        }
        {
            MarioWork* m = marioGetPtr();
            m->unk148 = 0.0f;
            m->unk150.z = 0.004f;
        }
        marioChgPaper("PM_R_1C");
        marioPaperLightOff();
        psndSFXOn_3D((char*)0x17a, &mario->position);
    case 0x15:
        if (--MI(0x48) > 0) {
            v31 = revise360(MF(0x1a0) - MF(0x198));
            if (v31 < 180.0f) {
                f32 t = 90.0f;
                f32 diff = t - MF(0x1a8);
                f32 n = 0.1f * diff;
                n += MF(0x1a8);
                if (__fabsf((f32)((t - n) < 1.0f))) {
                    n = t;
                }
                MF(0x1a8) = n;
                MF(0x1ac) = n;
            } else {
                f32 t = 270.0f;
                f32 diff = t - MF(0x1a8);
                f32 n = 0.1f * diff;
                n += MF(0x1a8);
                if (__fabsf((f32)((t - n) < 1.0f))) {
                    n = t;
                }
                MF(0x1a8) = n;
                MF(0x1ac) = n;
            }
        } else {
            v31 = revise360(MF(0x1a0) - MF(0x198));
            if (v31 >= 180.0f) {
                MF(0x1a8) = 0.0f;
                MF(0x1ac) = 0.0f;
                MF(0x19c) = 270.0f;
                MF(0x1a0) = 270.0f;
            } else {
                MF(0x1a8) = 180.0f;
                MF(0x1ac) = 180.0f;
                MF(0x19c) = 90.0f;
                MF(0x1a0) = 90.0f;
            }
            marioSetSpec();
            mario->field_0x8 = 0;
            marioOfsRotReset();
            mario->dispFlags &= ~0x100;
            mario->dispFlags &= ~8;
            mario->flags &= ~0x1000000;
            marioPaperOff();
            marioAdjustMoveDir();
            marioChgPose("M_S_1");
            if (marioGetPtr()->motStruct) {
                void* z;
                __memFree(0, marioGetPtr()->motStruct);
                z = 0;
                marioGetPtr()->motStruct = z;
            }
            allPartyRideOff();
            CAMF() &= ~0x200;
            MI(0x44) = 0x16;
        }
        break;
    case 0x16:
        marioChgMotSub(0, 0);
        break;
    }
    if (MI(0x44) < 0x1e) {
        return;
    }
    if (mario->flags & 0x40000) {
        roll_upstairs();
        return;
    }
    if (MU(0x248) & 0x200) {
        if (!rollChk()) {
            MI(0x44) = 0x14;
            CAMF() |= 0x200;
        } else {
            MU(0x248) |= 0x100;
        }
    }
    v26 = MF(0x130);
    if (RW()->state) {
        s32 z = 0;
        s32 one;
        RW()->state = z;
        one = 1;
        RW()->scaleState = one;
        v26 = 1.0f;
    }
    switch (RW()->scaleState) {
    case 1:
        if (v26 < 1.4f) {
            { f32 k = 1.0f; v26 = k * (1.4499999f - v26) + v26; }
            if (v26 >= 1.4f) {
                v26 = 1.4f;
            }
        }
        if (mario->airTimer == 0) {
            s32 t = 2;
            RW()->scaleState = t;
        }
        break;
    case 2:
        if (v26 > 0.4f) {
            v26 = 0.4f * (0.35f - v26) + v26;
            if (v26 <= 0.4f) {
                s32 t = 3;
                v26 = 0.4f;
                RW()->scaleState = t;
            }
        }
        break;
    case 3:
        if (v26 < 1.0f) {
            v26 = 0.5f * (1.05f - v26) + v26;
            if (v26 >= 1.0f) {
                s32 t = 0;
                RW()->scaleState = t;
                v26 = 1.0f;
            }
        }
        break;
    }
    MF(0x130) = v26;
    if (!mario->hitobjStandOn && mario->field_0x1E0 && !marioChkBero(mario->field_0x1E0)) {
        v26 = RW()->x;
        v27 = RW()->z;
        v26 += 0.2f * -v26;
        v27 += 0.2f * -v27;
        if (__fabsf(v26) < 1.0f) {
            v26 = 0.0f;
        }
        if (__fabsf(v27) < 1.0f) {
            v27 = 0.0f;
        }
        RW()->x = v26;
        RW()->z = v27;
    }
    if (mario->hitobjStandOn) {
        if (!(hitGetAttr(mario->hitobjStandOn) & 0xa00) && mario->airTimer == 0 && (MU(0x248) & 0x100)) {
            if (!(strcmp(gp->currentMapName, "eki_04") == 0 && __fabsf(MF(0x214)) >= 40.0f)) {
                MarioWork* m = marioGetPtr();
                camFollowYOff();
                PF(m, 0x7c) = 2.85f;
                PF(m, 0x80) = -0.14f;
                PF(m, 0x84) = 0.08f;
                PF(m, 0x88) = -0.02f;
                m->flags |= 0x10000;
                m->airTimer = 1;
                psndSFXOn_3D((char*)0x17c, &marioGetPtr()->position);
                mario->hitobjJumpFrom = mario->hitobjStandOn;
                mario->hitobjStandOn = 0;
                MV(0xd4) = mario->position;
                MV(0x11c) = mario->position;
                camFollowYOff();
            }
        }
        {
            Vec pv = mario->position;
            marioUpdateCamCollision(&pv);
        }
        mario->airTimer = 0;
    }
    if (mario->flags & 0x30000) {
        if (mario->field_0x1F4) {
            if (MF(0x7c) >= 0.0f) {
                MF(0x7c) = -0.03f;
                MF(0x80) = -0.02f;
                MF(0x84) = -0.02f;
                MF(0x88) = -0.002f;
            }
            mario->flags |= 0x20000;
        }
        if (mario->airTimer == 0 && MF(0x7c) < 0.0f) {
            MV(0xd4) = mario->position;
            camFollowYOn();
        }
        if (mario->position.y < MF(0x120)) {
            camFollowYOn();
        }
        mario->airTimer++;
    }
    v29 = RW()->x;
    v28 = RW()->z;
    v27 = 0.0f;
    v30 = v27;
    if (!(mario->flags & 0x30000)) {
        f32 a26, a30;
        a26 = (f32)(s32)(s8)mario->wStickDir1;
        a30 = (f32)(s32)(s8)mario->wStickDir2;
        v27 = distABf(v30, v30, a26, a30);
        v31 = angleABf(0.0f, 0.0f, a26, a30);
        v31 = revise360(MF(0x198) + v31);
        v26 = 3.1415927f * v31 / 180.0f;
        v30 = v27 * (f32)sin(v26);
        v27 = v27 * -(f32)cos(v26);
        v31 = angleABf(0.0f, 0.0f, v30, v27);
        tcA = MF(0x1a0);
        if (angOut(tcA, 135.0f, v31) == 1) {
            v27 = distABf(0.0f, 0.0f, v30, v27);
            v30 = -v27 * (f32)sin(3.1415927f * MF(0x1a0) / 180.0f);
            v27 = v27 * (f32)cos(3.1415927f * MF(0x1a0) / 180.0f);
        }
    }
    if (!(mario->flags & 0x30000) && !(MI(0x2a4) & 0x20)) {
        v31 = angleABf(0.0f, 0.0f, v29, -v28);
        if (MF(0x220) > 0.05f) {
            v26 = (v31 > 180.0f) ? rollData[5] : rollData[6];
            {
                f32 t = (f32)sin(3.1415927f * MF(0x220) / 180.0f) * v26;
                v30 = v30 - t;
            }
            if (v30 <= -100.0f) {
                v30 = -100.0f;
            }
        } else if (MF(0x220) < -0.05f) {
            v26 = (v31 < 180.0f) ? rollData[5] : rollData[6];
            {
                f32 t = (f32)sin(3.1415927f * MF(0x220) / 180.0f) * v26;
                v30 = v30 - t;
            }
            if (v30 >= 100.0f) {
                v30 = 100.0f;
            }
        }
        if (MF(0x218) > 0.05f) {
            v26 = (v31 > 180.0f) ? rollData[5] : rollData[6];
            {
                f32 t = (f32)sin(3.1415927f * MF(0x218) / 180.0f) * v26;
                v27 = v27 - t;
            }
            if (v27 <= -100.0f) {
                v27 = -100.0f;
            }
        } else if (MF(0x218) < -0.05f) {
            v26 = (v31 < 180.0f) ? rollData[5] : rollData[6];
            {
                f32 t = (f32)sin(3.1415927f * MF(0x218) / 180.0f) * v26;
                v27 = v27 - t;
            }
            if (v27 >= 100.0f) {
                v27 = 100.0f;
            }
        }
        v31 = angleABf(0.0f, 0.0f, v30, v27);
        tcA = MF(0x1a0);
        if (angOut(tcA, 135.0f, v31) == 1) {
            v27 = distABf(0.0f, 0.0f, v30, -v27);
            v30 = -v27 * (f32)sin(3.1415927f * MF(0x1a0) / 180.0f);
            v27 = v27 * (f32)cos(3.1415927f * MF(0x1a0) / 180.0f);
        }
    }
    if (*(void**)((u8*)mario + 0x1d8)) {
        if (!marioChkBero(mario->field_0x1E0)) {
            v29 = 0.05f * -v29 + v29;
            v28 = 0.05f * -v28 + v28;
            if (__fabsf(v29) < 0.1f) {
                v29 = 0.0f;
            }
            if (__fabsf(v28) < 0.1f) {
                v28 = 0.0f;
            }
        }
    }
    if (mario->field_0x1E0 && !MB(0x24e) && !MB(0x24f) && !marioChkBero(mario->field_0x1E0)) {
        v29 = 0.6f * -v29 + v29;
        v28 = 0.6f * -v28 + v28;
        if (__fabsf(v29) < 0.1f) {
            v29 = 0.0f;
        }
        if (__fabsf(v28) < 0.1f) {
            v28 = 0.0f;
        }
    }
    if (MH(0x4e) == 0 && !v30 && !v27) {
        if (mario->field_0x1F0) {
            if (!marioChkBero(mario->field_0x1F0)) {
                f32 k = rollData[2];
                v29 = v29 * k + v29;
                v28 = v28 * k + v28;
            }
        } else if (mario->hitobjStandOn && mario->airTimer == 0) {
            if (__fabsf((f32)(MF(0x214) <= 1.0f))) {
                f32 k = rollData[1];
                v29 = v29 * k + v29;
                v28 = v28 * k + v28;
            } else {
                f32 k = rollData[1];
                f32 a = v29 * k;
                f32 b = v28 * k;
                v29 = 6.4f * a + v29;
                v28 = 6.4f * b + v28;
            }
        }
        if (__fabsf(v29) < rollData[3]) {
            v29 = 0.0f;
        }
        if (__fabsf(v28) < rollData[3]) {
            v28 = 0.0f;
        }
    } else if ((MI(0x2a4) & 0x10) && MF(0x220) == 0.0f) {
        if (makeVec(135.0f) == 1) {
            v29 = 0.2f * -v29 + v29;
            v28 = 0.2f * -v28 + v28;
            if (__fabsf(v29) < 1.0f) {
                v29 = 0.0f;
            }
            if (__fabsf(v28) < 1.0f) {
                v28 = 0.0f;
            }
            if (v29 == 0.0f && v28 == 0.0f) {
                MF(0x19c) = revise360(180.0f + MF(0x19c));
                MF(0x1a0) = MF(0x19c);
            }
        }
    }
    v31 = rollData[0] / 0.05f;
    v26 = 1.0f;
    if (MH(0x4e) == 0 && __fabsf(MF(0x214)) >= v26) {
        f32 ang = angleABf(0.0f, 0.0f, v29, -v28);
        f32 sp = MF(0x220);
        if (sp > 0.0f) {
            if (ang > 180.0f) {
                v26 = rollData[9];
            } else {
                v26 = 1.0f;
            }
        } else if (sp < 0.0f) {
            if (ang < 180.0f) {
                v26 = rollData[9];
            } else {
                v26 = 1.0f;
            }
        }
    }
    v31 = v31 * v26;
    v26 = v28 * v28;
    if (v29 * v29 + v26 <= v31 * v31) {
        v29 = v30 * 0.025f + v29;
    } else {
        f32 n = -v31;
        v29 = v30 * 0.025f + v29;
        if (v29 < n) {
            v29 = n;
        }
        if (v29 > v31) {
            v29 = v31;
        }
    }
    v31 = rollData[0] / 0.05f;
    v30 = 1.0f;
    if (MH(0x4e) == 0 && __fabsf(MF(0x214)) >= v30) {
        f32 ang = angleABf(0.0f, 0.0f, v29, -v28);
        f32 sp = MF(0x218);
        if (sp > 0.0f) {
            if (ang > 180.0f) {
                v30 = rollData[9];
            } else {
                v30 = 1.0f;
            }
        } else if (sp < 0.0f) {
            if (ang < 180.0f) {
                v30 = rollData[9];
            } else {
                v30 = 1.0f;
            }
        }
    }
    v31 = v31 * v30;
    if (v29 * v29 + v26 <= v31 * v31) {
        v28 = v28 - v27 * 0.025f;
    } else {
        f32 n = -v31;
        v28 = v28 - v27 * 0.025f;
        if (v28 < n) {
            v28 = n;
        }
        if (v28 > v31) {
            v28 = v31;
        }
    }
    RW()->x = v29;
    RW()->z = v28;
    v31 = -v28;
    v30 = angleABf(0.0f, 0.0f, v29, v31);
    v27 = 3.1415927f * v30 / 180.0f;
    v26 = (f32)cos(v27);
    v27 = distABf(0.0f, 0.0f, v29 * (f32)sin(v27), v28 * v26);
    v27 = 0.05f * v27;
    if (!getRollEvtFlag() && MH(0x4e) == 0) {
        v30 = angleABf(0.0f, 0.0f, v29, v31);
        if (!(MI(0x2a4) & 0x10000)) {
            v27 = v27 * MF(0x224);
            if (v27 < 0.05f) {
                v27 = 0.0f;
            }
            MF(0x180) = v27;
            if (v27) {
                MF(0x1a0) = v30;
            }
        }
    }
    if (MF(0x180)) {
        MarioWork* m = marioGetPtr();
        if ((u32)PI(m, 0x2a8) == (u32)-1) {
            PI(m, 0x2a8) = (s32)psndSFXOn_3D((char*)0x17b, &m->position);
            PI(m, 0x2a4) |= 0x1000;
        }
    } else {
        MarioWork* m = marioGetPtr();
        if ((u32)PI(m, 0x2a8) != (u32)-1) {
            psndSFXOff(PI(m, 0x2a8));
            PI(m, 0x2a8) = -1;
        }
    }
    v28 = rollData[4];
    {
        s32 flip;
        f32 sgn;
        f32 mv = toMovedir(MF(0x1a8));
        f32 pre;
        tcA = MF(0x1a0);
        flip = angOut(tcA, 90.0f, mv);
        pre = MF(0x180) * v28;
        sgn = (flip == 1) ? -1.0f : 1.0f;
        MF(0xc4) = revise360(pre * sgn + MF(0xc4));
    }
    if (MH(0x4e) == 0 && !getRollEvtFlag() && v27) {
        v31 = MF(0x19c);
        {
            MarioWork* m2 = marioGetPtr();
            DISPDIR_BODY(v30, v31, m2)
        }
        MF(0x1a0) = v31;
        MF(0x19c) = v31;
    }
    if (MF(0x220) != 0.0f) {
        MF(0xbc) = revise360(-MF(0x220) * (f32)sin(3.1415927f * MF(0x1a8) / 180.0f));
    } else {
        MF(0xbc) = 0.0f;
    }
    if (MB(0x24e) || MB(0x24f)) {
        MI(0x2a4) |= 0x4000;
    }
    if (MF(0x180)) {
        MarioWork* m = marioGetPtr();
        m->unk148 = 0.0f;
        m->unk150.z = 0.004f;
    }
    marioRollCan();
}

void roll_upstairs_on(void) {
    MarioWork* mario = marioGetPtr();
    mario->flags |= 0x40000;
    mario->flags |= 0x80;
    marioClearJumpPara();
    mario->multiTimer = 3;
    MF(0x7c) = (MF(0x1c4) - mario->position.y) / (f32)(s32)mario->multiTimer;
    MF(0x7c) = 4.0f;
    MF(0x80) = -0.42f;
    MF(0x84) = 0.04f;
    MF(0x88) = -0.01f;
}

#pragma dont_inline on
void roll_upstairs(void) {
    MarioWork* mario = marioGetPtr();
    mario->position.y = mario->position.y + MF(0x7c);
    if ((s32)--mario->multiTimer <= 0) {
        mario->flags &= ~0x80;
        mario->flags &= ~0x40;
        mario->flags &= ~0x40000;
    }
}

#pragma dont_inline reset

void mot_fall_roll(void) {
    MarioWork* mario = marioGetPtr();
    f32 spd = marioGetFallSpd();
    f32 y;
    HitEntry* h;
    if (spd < -10.0f) {
        spd = -10.0f;
    }
    h = marioChkLandon(&y, spd);
    if (h) {
        mario->position.y = y;
        mario->hitobjStandOn = h;
        mario->hitobjJumpFrom = 0;
    } else {
        mario->position.y += spd;
    }
    if (1) {
        if (mario->hitobjStandOn) {
            marioLandOn();
            roll_flyout_flag2 = 0;
        }
    }
}

static inline void marioRollCan(void) {
    MarioWork* mario = marioGetPtr();
    f32 v26;
    {
        f32 s;
        f32 c;
        f32 dist;
        Vec pos;
        Vec dir;
        Vec out;
        Vec nrm;
        Vec d2;
        dist = 100.0f;
        pos = mario->position;
        pos.y = pos.y + MF(0x1b8);
        dir = (Vec){0.0f, -1.0f, 0.0f};
        if (marioHitCheckVec(&pos, &dir, &out, &nrm, &dist)) {
            f32 sq = roll_sqrtf(nrm.x * nrm.x + nrm.z * nrm.z);
            f32 ang = angleABf(0.0f, 0.0f, sq, -nrm.y);
            f32 r = (f32)(s32)(0.5f + ang);
            if (__fabsf(ang) < 45.0) {
                v26 = 3.1415927f * r / 180.0f;
                s = (f32)sin(v26);
                c = (f32)cos(v26);
                {
                    f32 t1 = 5.0f * s;
                    f32 t2 = 5.0f * c;
                    MF(0x98) = 6.0f + t1;
                    MF(0x9c) = -4.0f + t2;
                }
            } else {
                sincosf(&s, &c, 90.0f);
                dist = 100.0f;
                pos = mario->position;
                d2 = (Vec){0.0f, 0.0f, 0.0f};
                d2.x = s;
                d2.z = c;
                dir = d2;
                marioHitCheckVec(&pos, &dir, &out, &nrm, &dist);
            }
        }
    }
}

void marioRollStrippedPool(void) {
    MarioWork* mario = marioGetPtr();
    MF(0x98) = 100.0f;
    MF(0x9c) = 0.0f;
    MF(0xa0) = 0.5f;
    MF(0xa4) = 3.1415927f;
    MF(0xa8) = 180.0f;
    MF(0xac) = 6.0f;
    MF(0xb0) = 5.0f;
    MF(0xb4) = -4.0f;
    MF(0xb8) = 90.0f;
    MF(0xbc) = 270.0f;
    MF(0xc0) = 360.0f;
    MF(0xc4) = 10.0f;
    MF(0xc8) = 135.0f;
    MF(0xcc) = 1.2f;
}
