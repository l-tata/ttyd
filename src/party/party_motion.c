#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "pmario_sound.h"
#include "driver/hitdrv.h"
#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "mario/mariost.h"
#include <string.h>

// file-local full-layout view of PartyEntry (party.h is not included)
typedef struct PartyMarioView {
    u32 flags0;             // 0x0
    u8 unk4[0xC - 0x4];     // 0x4
    u32 flags;              // 0xC
    u8 unk10[0x2E - 0x10];  // 0x10
    u16 motionId;           // 0x2E
    u8 unk30[0x3C - 0x30];  // 0x30
    u8 unk3C;               // 0x3C
    u8 unk3D[0x50 - 0x3D];  // 0x3D
    s16 unk50;              // 0x50
    u8 unk52[0x8C - 0x52];  // 0x52
    Vec position;           // 0x8C
    u8 unk98[0x180 - 0x98]; // 0x98
    f32 unk180;             // 0x180
    u8 unk184[0x190 - 0x184]; // 0x184
    f32 unk190;             // 0x190
    u8 unk194[0x198 - 0x194]; // 0x194
    f32 unk198;             // 0x198
    u8 unk19C[0x1A8 - 0x19C]; // 0x19C
    f32 unk1A8;             // 0x1A8
    f32 unk1AC;             // 0x1AC
    u8 unk1B0[0x1B4 - 0x1B0]; // 0x1B0
    f32 unk1B4;             // 0x1B4
    u8 unk1B8[0x1E4 - 0x1B8]; // 0x1B8
    HitEntry* unk1E4;       // 0x1E4
    u8 unk1E8[0x224 - 0x1E8]; // 0x1E8
    f32 unk224;             // 0x224
} PartyMarioView;

typedef struct PartyEntry {
    u32 flags;                  // 0x0
    u32 flags2;                 // 0x4
    u32 flags3;                 // 0x8
    u8 unkC[0x24 - 0xC];        // 0xC
    s32 unk24;                  // 0x24
    s16 unk28;                  // 0x28
    u8 unk2A[0x2F - 0x2A];      // 0x2A
    s8 unk2F;                   // 0x2F
    u8 unk30;                   // 0x30
    s8 currentMemberId;         // 0x31
    u8 unk32;                   // 0x32
    u8 unk33;                   // 0x33
    u8 unk34;                   // 0x34
    u8 unk35[0x37 - 0x35];      // 0x35
    u8 unk37;                   // 0x37
    u8 unk38;                   // 0x38
    u8 unk39;                   // 0x39
    u8 unk3A;                   // 0x3A
    u8 unk3B;                   // 0x3B
    u8 unk3C[0x40 - 0x3C];      // 0x3C
    f32 unk40;                  // 0x40
    u8 unk44[0x4C - 0x44];      // 0x44
    s8 unk4C;                   // 0x4C
    s8 unk4D;                   // 0x4D
    s8 unk4E;                   // 0x4E
    u8 unk4F[0x54 - 0x4F];      // 0x4F
    f32 unk54;                  // 0x54
    Vec position;               // 0x58
    f32 unk64;                  // 0x64
    u8 unk68[0x6C - 0x68];      // 0x68
    f32 unk6C;                  // 0x6C
    u8 unk70[0x7C - 0x70];      // 0x70
    Vec unk7C;                  // 0x7C
    Vec unk88;                  // 0x88
    Vec unk94;                  // 0x94
    f32 unkA0;                  // 0xA0
    f32 unkA4;                  // 0xA4
    f32 unkA8;                  // 0xA8
    u8 unkAC[0xE0 - 0xAC];      // 0xAC
    f32 unkE0;                  // 0xE0
    f32 unkE4;                  // 0xE4
    u8 unkE8[0xF0 - 0xE8];      // 0xE8
    f32 unkF0;                  // 0xF0
    f32 unkF4;                  // 0xF4
    f32 unkF8;                  // 0xF8
    f32 unkFC;                  // 0xFC
    f32 unk100;                 // 0x100
    f32 unk104;                 // 0x104
    u8 unk108[0x10C - 0x108];   // 0x108
    f32 unk10C;                 // 0x10C
    f32 unk110;                 // 0x110
    f32 unk114;                 // 0x114
    f32 unk118;                 // 0x118
    f32 unk11C;                 // 0x11C
    f32 unk120;                 // 0x120
    u8 unk124[0x128 - 0x124];   // 0x124
    f32 unk128;                 // 0x128
    HitEntry* unk12C;           // 0x12C
    u8 unk130[0x134 - 0x130];   // 0x130
    HitEntry* unk134;           // 0x134
    HitEntry* unk138;           // 0x138
    u8 unk13C[0x140 - 0x13C];   // 0x13C
    void* unk140;               // 0x140
    u8 unk144[0x14C - 0x144];   // 0x144
    HitEntry* unk14C;           // 0x14C
    u8 unk150[0x160 - 0x150];   // 0x150
    PartyMarioView* playerPtr;  // 0x160
    u8 unk164[0x168 - 0x164];   // 0x164
    s32 unk168;                 // 0x168
} PartyEntry;

typedef struct PartyCamView {
    u8 unk0[0x114];
    f32 unk114;
} PartyCamView;

PartyEntry* partyGetPtr(s32 index);
s32 marioGetPartyId(void);
s32 marioGetExtraPartyId(void);
f32 revise360(f32 angle);
f32 distABf(f32 x1, f32 y1, f32 x2, f32 y2);
void partyChgMot(PartyEntry* party, s32 mot);
void partyPaperOff(PartyEntry* party);
void partyChgPoseId(PartyEntry* party, s32 pose);
void partyChgMotSlitFloor(PartyEntry* party);
void sincosf(f32* ret_sin, f32* ret_cos, f32 degree);

static s32 unk_804189f4;
static f32 touch_wall_ct;
static s32 unk_804189ec;
static f32 unk_804189e8;

void L_partyForceSlitOff(PartyEntry* party);
BOOL party_slit(PartyEntry* party);
BOOL party_roll(PartyEntry* party);
void unk_8014b824(PartyEntry* party);
void unk_8014ba3c(PartyEntry* party);
void* partySearchHead(PartyEntry* party, Vec* pos, f32* height, f32 dir);
HitEntry* partySearchGround(PartyEntry* party, f32 up, f32 down);
s32 unk_800c8dd0(HitEntry* hit);
void partyGetAppearPos5(PartyEntry* party, HitEntry* hit, Vec* out);
void partyClearFootmark(void);
void partyClearFootmark2(void);
void unk_800c9044(PartyEntry* party);
void partyChkWall(PartyEntry* party);
void partyChkGnd(PartyEntry* party);
void partyChgRunMode(PartyEntry* party, s32 mode);
void partyMoveBehindMario(PartyEntry* party, s32 a);
s32 marioKeyOffChk(void);
s32 marioChkDeepSleep(void);
s32 chuchu_searchObject(void);
void partyChgPose(PartyEntry* party, const char* pose);
void marioGetScreenPos(Vec pos, f32* x, f32* y, f32* z);
void partyGetReturnPos(Vec* pos);
void unk_800c9398(PartyEntry* party, f32 a);
BOOL partyPaperOn(PartyEntry* party, const char* name);
void partyChgPaper(PartyEntry* party, const char* name);
void partyPaperLightOff(PartyEntry* party);
void marioClearSlitFloor(void);
f32 partyToMovedir(PartyEntry* party, f32 dir);
void movePos(f32* x, f32* z, f32 dist, f32 dir);
void partyMoveCheckWall(PartyEntry* party);
void unk_800c8f24(PartyEntry* party, f32 speed);
void partyChgMoveMode(PartyEntry* party, s32 mode);
void partySetDir(PartyEntry* party, f32 x, f32 z);
void partyMove(PartyEntry* party);
s32 partyGetFootmarkId();
void partyNextReadId(s32 id);
f32 angleABf(f32 x1, f32 y1, f32 x2, f32 y2);
f32 toMovedir(f32 angle);
void partyGetAppearPos4(PartyEntry* party, Vec* pos);
HitEntry* partySearchFrontWall(PartyEntry* party, Vec* pos, f32 dist, f32 dir);
void N_partyPlaySfxPartyLanding1(PartyEntry* party);

// Note: the real evt_snd user functions live in this split (see evt_snd.c).
// Functions are defined in reverse order (-inline deferred).

void psndSetReverb(s32 mode);
BOOL psndENVOn(s32 a, s32 b);
BOOL psndENVOn_f_d(s32 a, s32 b, s32 c, s32 d);
BOOL psndENVOff_f_d(s32 a, s32 b, s32 c);
void psndENV_LPF(s32 a, u16 b);
void psndSFX_dist(s32 id, u16 dist);
void psndSFX_pos(s32 id, Vec* pos);
void psndSFX_vol(s32 id, u8 vol);
void psndSFX_pit(s32 id, s16 pit);
s32 psndSFXChk(s32 id);
s32 psndSFXOnEx_3D(const char* name, Vec* pos, u8 a, u8 b, u16 c, u16 d);
s32 psndSFXOn_(const char* name);
void psndBGMScope(s32 a, s32 b);
void psndBGM_rate(s32 a, u16 b);
BOOL psndBGMOn_f_d(s32 a, s32 b, s32 c, s32 d, s32 e);
BOOL psndBGMOff(s32 a);
void unk_800d7270(s32 a, s32 b, s32 c, s32 d);

USER_FUNC(evt_snd_bgmon) {
    s32* args = event->args;
    psndBGMOn(args[0], (char*)args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_bgmon_f) {
    s32* args = event->args;
    s32 a = args[0];
    s32 b = args[1];
    s32 c = evtGetValue(event, args[2]);
    unk_800d7270(a, b, c, 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(unk_8014cc24) {
    s32* args = event->args;
    s32 a = args[0];
    s32 b = args[1];
    s32 c = evtGetValue(event, args[2]);
    evtGetValue(event, args[3]);
    psndBGMOn_f_d(a, b, c, c, 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_bgmoff) {
    psndBGMOff(event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_bgmoff_f) {
    s32* args = event->args;
    s32 a = args[0];
    s32 t = evtGetValue(event, args[1]);
    ((BOOL (*)(s32, s32, s32))psndBGMOff_f_d)(a, t, 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_bgm_freq) {
    s32* args = event->args;
    s32 a = args[0];
    s32 t = evtGetValue(event, args[1]);
    psndBGM_rate(a, t);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_bgm_scope) {
    s32* args = event->args;
    s32 a = args[0];
    s32 t = evtGetValue(event, args[1]);
    psndBGMScope(a, t);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfxon) {
    s32* args = event->args;
    const char* name = (const char*)args[0];
    s32 out = args[1];
    if (!out) {
        psndSFXOn(name);
    } else {
        evtSetValue(event, out, ((s32 (*)(const char*))psndSFXOn)(name));
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfxon__) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    s32 out = args[1];
    if (!out) {
        psndSFXOn(name);
    } else {
        evtSetValue(event, out, ((s32 (*)(const char*))psndSFXOn)(name));
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfxon_) {
    s32* args = event->args;
    const char* name = (const char*)args[0];
    s32 out = args[1];
    if (!out) {
        psndSFXOn_(name);
    } else {
        evtSetValue(event, out, psndSFXOn_(name));
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfxon_3d) {
    s32* args = event->args;
    const char* name = (const char*)args[0];
    Vec pos;
    s32 out;
    pos.x = evtGetFloat(event, args[1]);
    pos.y = evtGetFloat(event, args[2]);
    pos.z = evtGetFloat(event, args[3]);
    out = args[4];
    if (!out || out == -250000000) {
        psndSFXOn_3D(name, &pos);
    } else {
        evtSetValue(event, out, psndSFXOn_3D(name, &pos));
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfxon_3d_ex) {
    s32* args = event->args;
    const char* name = (const char*)args[0];
    Vec pos;
    u16 a, b, c, d;
    s32 out;
    pos.x = evtGetFloat(event, args[1]);
    pos.y = evtGetFloat(event, args[2]);
    pos.z = evtGetFloat(event, args[3]);
    a = evtGetValue(event, args[4]);
    b = evtGetValue(event, args[5]);
    c = evtGetValue(event, args[6]);
    d = evtGetValue(event, args[7]);
    out = args[8];
    if (!out) {
        psndSFXOnEx_3D(name, &pos, a, b, c, d);
    } else {
        evtSetValue(event, out, psndSFXOnEx_3D(name, &pos, a, b, c, d));
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfxoff) {
    psndSFXOff(evtGetValue(event, event->args[0]));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfxchk) {
    s32* args = event->args;
    evtSetValue(event, args[1], psndSFXChk(evtGetValue(event, args[0])));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfx_pit) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 t = evtGetValue(event, args[1]);
    psndSFX_pit(id, t);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfx_vol) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 t = evtGetValue(event, args[1]);
    psndSFX_vol(id, t);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfx_pos) {
    s32* args = event->args;
    Vec pos;
    s32 id = evtGetValue(event, args[0]);
    pos.x = evtGetFloat(event, args[1]);
    pos.y = evtGetFloat(event, args[2]);
    pos.z = evtGetFloat(event, args[3]);
    psndSFX_pos(id, &pos);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfx_dist) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    psndSFX_dist(id, evtGetValue(event, args[1]));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_envon) {
    s32* args = event->args;
    psndENVOn(args[0], args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_envon_f) {
    s32* args = event->args;
    s32 a = args[0];
    s32 b = args[1];
    s32 c = evtGetValue(event, args[2]);
    psndENVOn_f_d(a, b, c, 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_envoff) {
    psndENVOff(event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_envoff_f) {
    s32* args = event->args;
    s32 a = args[0];
    s32 t = evtGetValue(event, args[1]);
    psndENVOff_f_d(a, t, 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_env_lpf) {
    s32* args = event->args;
    s32 a = args[0];
    psndENV_LPF(a, evtGetValue(event, args[1]));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_set_rev_mode) {
    psndSetReverb(event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfx_all_stop) {
    psndSFXAllOff();
    return EVT_RETURN_DONE;
}

void unk_8014c47c(void) {
    touch_wall_ct = 60.0f;
}

void N_partyFollowCloseOn(PartyEntry* party) {
    party->flags3 |= 0x20000;
}

void N_partyFollowCloseOff(PartyEntry* party) {
    party->flags3 &= ~0x20000;
}

void unk_800c8e74(PartyEntry* party);
void unk_800c93d0(PartyEntry* party, f32 spd);
HitEntry* partyHitCheck(PartyEntry* party, Vec* pos, Vec* dir, Vec* outPos, Vec* outNrm, f32* dist);
void partyGetAppearPos3(PartyEntry* party, Vec* pos);
s32 marioCtrlOffChk(void);
extern int __float_nan[];
static inline HitEntry* partyBehindGround(PartyEntry* party) {
    HitEntry* hit = partySearchGround(party, 10.0f, 0.0f);
    if (hit && (hitGetAttr(hit) & 0x200)) {
        hit = NULL;
        if (!party->unk4C) {
            party->flags3 |= 0x1000000;
            party->flags |= 0x2000000;
        }
    }
    return hit;
}

s32 partyGetFootmarkWriteId(s32 id);
s32 partyGetFootmarkReadId(s32 id);
f32 partyGetTargetDist(s32 id, f32 x, f32 z);
f32 partyGetTargetDir(s32 id, f32 x, f32 z);
f32 partyGetTargetDistY(s32 id);
void partyGetReadId(s32 id, f32* x, f32* y, f32* z);
static f32 partyStayDist(PartyEntry* party);
static u32 partyGetFrontAttr(PartyEntry* party, f32 dir);
static f32 partyHomingOff(PartyEntry* party);
static f32 partyGetGndY(f32 x, f32 y, f32 z);
static f32 partyGetDirGndY(Vec u, f32 md);
static u32 partyGetFrontAttrFar(PartyEntry* party);
static void partyGorFix(PartyEntry* party);
static void partyPoseA2(PartyEntry* party);
s32 getFrontFloor(PartyEntry* party, Vec* out, f32 dir);










f32 partyGetTargetDist3D(s32 id, Vec* p);
f32 partyGetTargetDist(s32 id, f32 x, f32 z);
s32 nokonokoGetStatus(PartyEntry* party);
HitEntry* marioChkWallAround(Vec* pos, s32 a, f32 spd, f32 dir, f32 w, f32 h);
s32 marioChkSlitThrouh(void);
void partyGetMoveDirSpd(PartyEntry* party, f32* dir, f32* spd);
PartyEntry* anotherPartyGetPtr(s32 id);
HitEntry* partySearchFrontWall(PartyEntry* party, Vec* pos, f32 dist, f32 dir);


void party_force_reset_outofscreen(PartyEntry* party);

void party_motion_stay(PartyEntry* party) {
    PartyMarioView* mario = party->playerPtr;
    s32 id;
    PartyEntry* other;
    HitEntry* hit;
    f32 lim;
    f32 x, z;
    f32 dir, spd;
    Vec v;
    Vec p;

    id = partyGetFootmarkId(party);
    if (party->flags3 & 2) {
        party->flags3 &= ~2;
        partyChgPoseId(party, 1);
        if (!(party->flags3 & 0x800000)) {
            if (!(party->flags & 8)) {
                partyClearFootmark();
            } else {
                partyClearFootmark2();
            }
        }
        party->flags3 &= ~0x800000;
        party->unk168 = -1;
        unk_800c9398(party, 0.0f);
    }
    if (mario->motionId != 0xE && (party->flags & 0x80000)) {
        party->flags &= ~0x80000;
    }
    lim = party->unk40;
    if (!mario->unk190 && mario->unk180 < 1.0f) {
        lim = 100.0f;
    }
    if ((s8)mario->unk3C == 2) {
        lim = 70.0f;
    }
    if (!strcmp(gp->currentMapName, "aji_02") || !strcmp(gp->currentMapName, "aji_05") ||
        !strcmp(gp->currentMapName, "aji_12"))
    {
        if (!marioKeyOffChk()) {
            {
                f32 sd;
                if (party->unk32 == 0) {
                    sd = 18.0f;
                } else if (party->currentMemberId == 5) {
                    if (party->flags3 & 0x200000) {
                        sd = 8.0f;
                    } else {
                        sd = 30.0f;
                    }
                } else if (party->currentMemberId == 6) {
                    if (party->flags3 & 0x200000) {
                        sd = 8.0f;
                    } else {
                        sd = 30.0f;
                    }
                }
                lim = sd;
            }
            party->flags3 |= 0x20000;
        }
    }
    if ((party->flags3 >> 17) & 1) {
        {
            f32 sd;
            if (party->unk32 == 0) {
                sd = 18.0f;
            } else if (party->currentMemberId == 5) {
                if (party->flags3 & 0x200000) {
                    sd = 8.0f;
                } else {
                    sd = 30.0f;
                }
            } else if (party->currentMemberId == 6) {
                if (party->flags3 & 0x200000) {
                    sd = 8.0f;
                } else {
                    sd = 30.0f;
                }
            }
            lim = sd;
        }
    }
    party_slit(party);
    party_roll(party);
    unk_800c9044(party);
    if (mario->motionId == 0xE && !(party->flags & 0x80000)) {
        partyMoveBehindMario(party, 0);
        return;
    }
    if (party->unk34 != 0) {
        if (!party->unk2F || !partyGetPtr(marioGetPartyId())) {
            if (partyChkMarioFollowY(party->playerPtr)) {
                f32 d;
                if (party->unk32 == 0) {
                    p = party->position;
                    d = partyGetTargetDist3D(id, &p);
                    partyGetTargetDist(id, party->position.x, party->position.z);
                } else {
                    x = party->position.x;
                    z = party->position.z;
                    movePos(&x, &z, 0.5f * party->unkF4,
                            angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z));
                    d = distABf(x, z, mario->position.x, mario->position.z);
                }
                if (marioChkSlitThrouh() || d >= lim) {
                    if (!marioChkDeepSleep()) {
                        partyChgMot(party, 1);
                    }
                }
            }
        } else {
            other = anotherPartyGetPtr(party->unk2F);
            if (other) {
                if (nokonokoGetStatus(other) == 3 || PSVECDistance(&mario->position, &other->position) >= 150.0f) {
                    if (partyGetTargetDist(id, party->position.x, party->position.z) >= lim) {
                        partyChgMot(party, 1);
                        partyClearFootmark2();
                    }
                } else if (other->currentMemberId == 3 && other->unk34 == 3) {
                    if (mario->motionId != 0x1F && mario->motionId != 0x10 && mario->motionId != 0x11 &&
                        distABf(party->position.x, party->position.z, mario->position.x, mario->position.z) >= lim)
                    {
                        partyChgMot(party, 1);
                    }
                } else if (!(other->flags & 0x100) || other->currentMemberId == 4) {
                    if (distABf(party->position.x, party->position.z, other->position.x, other->position.z) >= lim) {
                        partyChgMot(party, 1);
                    }
                }
                if (marioChkSlitThrouh()) {
                    partyChgMot(party, 1);
                }
            }
        }
    }
    v = party->position;
    partyGetMoveDirSpd(party, &dir, &spd);
    hit = partySearchFrontWall(party, &v, spd, dir);
    if (hit) {
        party->unk12C = hit;
        if (party->unk104) {
            party->unk134 = hit;
        }
    }
    party->position.x = v.x;
    party->position.z = v.z;
    partyGetMoveDirSpd(party, &dir, &spd);
    marioChkWallAround(&v, 1, spd, dir, party->unkF4, party->unkF0);
    party->position.x = v.x;
    party->position.z = v.z;
    if (party->unk32 == 1) {
        unk_8014b824(party);
        return;
    }
    unk_8014ba3c(party);
    party_force_reset_outofscreen(party);
    if (party->currentMemberId == 7 && !(party->flags2 & 0x8000) && !marioKeyOffChk() && marioChkDeepSleep() != 1 &&
        chuchu_searchObject())
    {
        switch (party->unk39) {
        case 1:
            if (--party->unk24 > 0) {
                break;
            }
            partyChgPoseId(party, 1);
            party->unk24 = 30;
            party->unk39 = 2;
            break;
        case 2:
            if (--party->unk24 > 0) {
                break;
            }
            party->unk39 = 0;
            break;
        case 0:
        default:
            partyPoseA2(party);
            break;
        }
    }
}

void unk_8014bbc8(PartyEntry* party) {
    PartyMarioView* mario = party->playerPtr;
    if (party->flags3 & 2) {
        party->flags3 &= ~2;
        partyChgPoseId(party, 1);
        if (!(party->flags & 8)) {
            partyClearFootmark();
        } else {
            partyClearFootmark2();
        }
        party->unk168 = -1;
    }
    party_slit(party);
    party_roll(party);
    unk_800c9044(party);
    if (!(party->flags & 0x1000000)) {
        partyChkWall(party);
    }
    partyChkGnd(party);
    if (distABf(party->position.x, party->position.z, mario->position.x, mario->position.z) > 31.0f) {
        partyChgRunMode(party, 2);
    }
    if (mario->motionId == 0xE && !(party->flags & 0x80000)) {
        partyMoveBehindMario(party, 0);
    } else if (party->unk32 == 1) {
        unk_8014b824(party);
    } else {
        unk_8014ba3c(party);
        if (party->currentMemberId == 7 && !(party->flags2 & 0x8000) && !marioKeyOffChk() &&
            marioChkDeepSleep() != 1 && chuchu_searchObject())
        {
            switch (party->unk39) {
                case 1:
                    if (--party->unk24 <= 0) {
                        partyChgPoseId(party, 1);
                        party->unk24 = 30;
                        party->unk39 = 2;
                    }
                    break;
                case 2:
                    if (--party->unk24 <= 0) {
                        party->unk39 = 0;
                    }
                    break;
                case 0:
                default:
                    partyPoseA2(party);
                    break;
            }
        }
    }
}

void unk_8014ba3c(PartyEntry* party) {
    Vec pos;
    f32 height;
    void* head;
    HitEntry* hit;
    s32 attr;

    pos = party->position;
    height = party->unkF0;
    head = partySearchHead(party, &pos, &height, party->unk10C);
    if (head) {
        party->unk140 = head;
    }
    if (party->flags & 0x4000000) {
        return;
    }
    party->flags &= ~0x80;
    hit = partySearchGround(party, 15.0f, -15.0f);
    if (hit) {
        attr = hitGetAttr(hit);
        if (attr & 0x200) {
            hit = NULL;
            if (!party->unk4C) {
                party->flags3 |= 0x1000000;
                party->flags |= 0x2000000;
            }
        }
    }
    party->unk138 = hit;
    if (!hit) {
        party->flags &= ~2;
        partyChgMot(party, 3);
        party->unk11C = -1.5f;
        return;
    }
    party->position.y = party->unkE4;
    party->flags |= 2;
    if (attr & 0x100) {
        party->flags |= 0x80;
    } else {
        party->flags &= ~0x80;
    }
    if (attr & 0x40000000) {
        party->unk14C = hit;
    }
    if (unk_800c8dd0(hit)) {
        partyGetAppearPos5(party, hit, &party->unk94);
        party->flags3 |= 0x400000;
        party->unk94.y = party->position.y;
        partyChgMot(party, 2);
    }
}

static inline BOOL partyChkMarioFollowY(PartyMarioView* mario) {
    if (mario->unk50 != 0 && mario->unk180 == 0.0f) {
        return FALSE;
    }
    if (mario->motionId == 0x1F || mario->motionId == 0x20 || mario->motionId == 0x10 ||
        mario->motionId == 0x11)
    {
        return FALSE;
    }
    return TRUE;
}

void unk_8014b824(PartyEntry* party) {
    PartyMarioView* mario = party->playerPtr;
    HitEntry* hit;
    s32 attr;
    f32 target;
    f32 rate;
    f32 diff;
    f32 add;

    if (party->flags & 0x4000000) {
        return;
    }
    hit = partySearchGround(party, 10.0f, 0.0f);
    if (hit) {
        attr = hitGetAttr(hit);
        if (attr & 0x800) {
            partyChgRunMode(party, 0xD);
            return;
        }
        if (attr & 0x100) {
            party->flags |= 0x80;
        } else {
            party->flags &= ~0x80;
        }
    }
    if (partyChkMarioFollowY(party->playerPtr)) {
        rate = 0.05f;
        if (hit) {
            if (mario->position.y > party->unkE4) {
                target = mario->position.y;
                rate = 0.1f;
            } else {
                target = party->unkE4;
                rate = 0.1f;
            }
            if (party->position.y < party->unkE4) {
                target = 18.5f + party->unkE4;
                rate = 0.1f;
            }
        } else {
            target = mario->position.y;
        }
        add = 5.0f;
        if (party->flags2 & 0x8000) {
            add = 1.0f;
        }
        target += add;
        diff = target - party->position.y;
        if (diff > 5.0f || 0.0f > diff) {
            party->position.y += diff * rate;
        }
    } else {
        if (hit) {
            if (mario->position.y > party->unkE4) {
                target = mario->position.y;
            } else {
                target = party->unkE4;
            }
            if (party->position.y < party->unkE4) {
                target = 18.5f + party->unkE4;
            }
        } else {
            target = party->position.y;
        }
        add = 5.0f;
        if (party->flags2 & 0x8000) {
            add = 1.0f;
        }
        target += add;
        diff = target - party->position.y;
        if (diff > 5.0f || 0.0f > diff) {
            rate = 0.05f;
            party->position.y += diff * rate;
        }
    }
}

typedef struct PartyFootmark {
    u8 unk0;
    u8 unk1[3];
    Vec pos;
} PartyFootmark;

f32 partyGetTargetMovespd2(PartyEntry* party);
PartyFootmark* partyGetFootmarkPtr(s32 id);
s32 marioGetSlitFloorPos(Vec* pos);
f32 unk_800c9f48(s32 id, f32 y);

PartyMarioView* marioGetPtr(void);
void partyGetReadId2(s32 id, Vec* pos);
void L_getTargetPos(s32 id, Vec* pos);
void partyChkFrontStep(PartyEntry* party, f32 dist, f32* y, f32* a, f32* b);

void party_motion_homing_walk(PartyEntry* party) {
    HitEntry* hit = NULL;
    s32 id;
    PartyMarioView* mario;
    PartyEntry* other;
    s32 i;
    PartyFootmark* fm;
    PartyMarioView* m3;
    u32 attr;
    f32 lim;
    f32 lim2;
    f32 spd;
    f32 px, py, pz;
    f32 d3;
    f32 a;
    f32 gy;
    f32 md;
    Vec v;
    Vec t;
    Vec tp;
    Vec hp;
    Vec p2;
    Vec p3;
    Vec u;
    f32 sp;
    f32 dir;
    f32 s4;
    f32 c4;
    f32 rx, ry, rz;
    f32 fsy, fsa, fsb;
    f32 c, s;
    f32 s2, c2;
    f32 c3, s3;
    f32 hh;
    HitCheckQuery q;
    HitCheckQuery q2;
    HitCheckQuery q3;

    id = partyGetFootmarkId(party);
    mario = party->playerPtr;
    if (party->flags3 & 2) {
        party->flags3 &= ~2;
        party->flags &= ~0x80000;
    }
    if (party->unk28 != 0) {
        lim = 28.0f;
    } else {
        lim = 37.5f;
    }
    if (!strcmp(gp->currentMapName, "aji_02") || !strcmp(gp->currentMapName, "aji_05") ||
        !strcmp(gp->currentMapName, "aji_12"))
    {
        if (!marioKeyOffChk()) {
            lim = partyStayDist(party);
            party->flags3 |= 0x20000;
        }
    }
    if (partyGetTargetDistY(id)) {
        lim2 = 50.0f;
    } else {
        lim2 = 35.0f;
    }
    if ((s32)mario->unk3C == 2) {
        lim2 = 70.0f;
    }
    if ((party->flags3 >> 17) & 1) {
        lim2 = partyStayDist(party);
    }
    if (!party_slit(party)) {
        return;
    }
    if (!party_roll(party)) {
        return;
    }
    if (mario->motionId == 0xE && !(party->flags & 0x80000)) {
        partyMoveBehindMario(party, 0);
        return;
    }
    if (!partyChkMarioFollowY(party->playerPtr)) {
        partyChgMot(party, 0);
        return;
    }
    spd = partyGetTargetMovespd2(party);
    if (!(party->flags & 8)) {
        d3 = PSVECDistance(&mario->position, &party->position);
        if (d3 < 35.0f) {
            spd = 1.0f;
        } else if (d3 >= 50.0f) {
            spd = 3.0f;
        }
        if (party->playerPtr->flags0 & 0x8000) {
            spd = 1.0f;
            lim = 100.0f;
        }
        unk_800c93d0(party, spd);
    } else if ((other = partyGetPtr(marioGetPartyId()))) {
        v = other->position;
        if (nokonokoGetStatus(other) == 3 || PSVECDistance(&mario->position, &other->position) >= 150.0f) {
            d3 = PSVECDistance(&mario->position, &party->position);
            if (d3 < 35.0f) {
                spd = 1.0f;
            } else if (d3 >= 50.0f) {
                spd = 3.0f;
            }
            if (party->playerPtr->flags0 & 0x8000) {
                spd = 1.0f;
            }
            unk_800c93d0(party, spd);
        } else {
            d3 = PSVECDistance(&other->position, &party->position);
            if (d3 < 35.0f) {
                spd = 1.0f;
            } else if (d3 >= 50.0f) {
                spd = 3.0f;
            }
            if (party->playerPtr->flags0 & 0x8000) {
                spd = 1.0f;
            }
            unk_800c93d0(party, spd);
        }
    }
    px = party->position.x;
    i = 0;
    py = party->position.y;
    pz = party->position.z;
    do {
        fm = partyGetFootmarkPtr(id);
        t = fm->pos;
        dir = angleABf(px, pz, t.x, t.z);
        attr = partyGetFrontAttr(party, dir);
        if (!(attr & 0x800)) {
            goto found;
        }
        partyNextReadId(id);
        partyGetReadId2(id, &t);
    } while (++i <= 10);
    t = marioGetPtr()->position;
found:
    p2 = party->position;
    if (partyGetTargetDist3D(id, &p2) >= lim) {
        unk_800c93d0(party, spd);
    }
    if (lim2 < 20.0f) {
        unk_800c93d0(party, spd);
    }
    while (TRUE) {
        d3 = distABf(px, pz, t.x, t.z);
        dir = angleABf(px, pz, t.x, t.z);
        if (mario->flags0 & 0x8000) {
            a = party->unkF4 + mario->unk1B4;
        } else {
            a = 0.5f * party->unkF4 + party->unk104;
        }
        if (d3 > a) {
            p3 = (Vec){px, py, pz};
            d3 = partyGetTargetDist3D(id, &p3);
            if (d3 >= lim) {
                if (party->flags3 & 0x20000) {
                    a = 0.1f * (d3 - lim2);
                    if (a > 4.0f) {
                        a = 4.0f;
                    }
                    if (a < 0.5f) {
                        a = 0.0f;
                    }
                    unk_800c9398(party, mario->unk180 + a);
                }
                break;
            }
            if (marioGetSlitFloorPos(&v)) {
                unk_800c93d0(party, 1.5f);
                partyNextReadId(id);
                if (marioGetSlitFloorPos(&v)) {
                    t = v;
                    py = distABf(px, pz, t.x, t.z);
                    dir = angleABf(px, pz, t.x, t.z);
                    if (py <= 5.0f) {
                        ((void (*)(PartyEntry*))partyChgMotSlitFloor)(party);
                    }
                }
                break;
            }
            if (d3 > lim2) {
                if (party->flags3 & 0x20000) {
                    a = 0.1f * (d3 - lim2);
                    if (a > 4.0f) {
                        a = 4.0f;
                    }
                    if (a < 0.5f) {
                        a = 0.0f;
                    }
                    unk_800c9398(party, mario->unk180 + a);
                } else {
                    a = d3 - lim2;
                    if (a >= 1.0f) {
                        a = 1.0f;
                        if (lim2 < 20.0f) {
                            a = spd;
                        }
                    }
                    unk_800c93d0(party, a);
                }
                break;
            } else if (party->unkA0 == 0.0f && party->unkA8 == 0.0f) {
                unk_800c93d0(party, party->playerPtr->unk180);
            } else {
                unk_800c9398(party, 0.0f);
                party->unk100 = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
                partyChgPoseId(party, 1);
            }
        }
        if (partyGetFootmarkReadId(id) == (i = partyGetFootmarkWriteId(id))) {
            unk_800c93d0(party, party->playerPtr->unk180);
            if (party->playerPtr->unk180 == 0.0f || party->unkA0 != 0.0f || party->unkA8 != 0.0f) {
                dir = party->unk100;
                unk_800c93d0(party, 0.0f);
                party->unk11C = 0.0f;
                if (!(party->flags & 0x200)) {
                    partyChgPoseId(party, 1);
                }
                partyChgMot(party, 0);
            }
            break;
        }
        partyNextReadId(id);
        partyGetReadId2(id, &t);
        if (!(party->flags & 0x4000000)) {
            party->flags &= ~0x80;
            hit = partySearchGround(party, 10.0f, 0.0f);
            if (hit && (hitGetAttr(hit) & 0x200)) {
                hit = NULL;
                if (!party->unk4C) {
                    party->flags3 |= 0x1000000;
                    party->flags |= 0x2000000;
                }
            }
            party->unk138 = hit;
            if (!hit) {
                partyChgMot(party, 3);
                party->unk11C = -1.0f;
            } else {
                attr = hitGetAttr(hit);
                if (attr & 0x100) {
                    party->flags |= 0x80;
                } else {
                    party->flags &= ~0x80;
                }
                if (attr & 0x800) {
                    partyChgMot(party, 0);
                    break;
                }
            }
        }
        if ((party->flags & 2) && (fm->unk0 & 1)) {
            md = toMovedir(party->unk110);
            partyGetReadId(id, &rx, &ry, &rz);
            L_getTargetPos(id, &tp);
            if (tp.y < ry) {
                ry = tp.y;
            }
            party->unk94.x = rx;
            party->unk94.z = rz;
            party->unk94.y = ry;
            gy = partyGetDirGndY(party->unk94, md);
            if (gy > party->position.y) {
                dir = partyToMovedir(party, party->unk110);
                sincosf(&s4, &c4, dir);
                { f32 k = 40.0f;
                party->unk94.x = s4 * k;
                party->unk94.z = c4 * k; }
                party->unk94.y = gy;
            }
            partyChgMot(party, 2);
            break;
        }
        if (unk_800c8dd0(hit)) {
            partyGetAppearPos5(party, hit, &party->unk94);
            party->flags3 |= 0x400000;
            partyChgMot(party, 2);
            break;
        }
        if ((party->flags & 2) && (fm->unk0 & 4)) {
            if (distABf(px, pz, t.x, t.z) <= 5.0f) {
                ((void (*)(PartyEntry*))partyChgMotSlitFloor)(party);
            }
            break;
        }
    }
    if (party->unk104) {
        unk_800c8f24(party, party->unk104);
    } else if (!(fm->unk0 & 4)) {
        partyChgMot(party, 0);
    }
    party->unk100 = dir;
    v = party->position;
    partyGetMoveDirSpd(party, &dir, &sp);
    hit = partySearchFrontWall(party, &v, sp, dir);
    if (party->flags & 0x1000000) {
        hit = NULL;
    }
    if (!hit) {
        party->unk4D = 0;
        if (!(party->flags3 & 0x200000)) {
            getFrontFloor(party, &t, party->unk100);
        } else {
            party->unk54 = revise360(270.0f + mario->unk198);
            partyGetAppearPos4(party, &t);
        }
        if (t.y > party->position.y) {
            a = t.y - party->position.y;
            if (a > 12.0 && a < 60.0f) {
                party->unk94 = t;
                partyChgMot(party, 2);
                return;
            }
            if (t.y > party->position.y) {
                party->position.y = t.y;
            }
        }
        if (party->unk104) {
            attr = partyGetFrontAttrFar(party);
            if (!(attr & 0x200)) {
                v.x = party->position.x;
                v.z = party->position.z;
                movePos(&v.x, &v.z, party->unk104, party->unk100);
            }
        }
    } else {
        party->unk12C = hit;
        if (party->unk104) {
            party->unk134 = hit;
            if (hitGetAttr(hit) & 0x2000) {
                if (party->unk37 != 1) {
                    if (!(party->playerPtr->flags & 0x80000000)) {
                        party->flags3 |= 1;
                        party->unk37 = 1;
                    }
                } else if (party->unk104) {
                    v.x = party->position.x;
                    v.z = party->position.z;
                    movePos(&v.x, &v.z, party->unk104, party->unk100);
                }
            } else {
                party->unk4D++;
                if (party->unk4D > 8) {
                    party->unk4D = 0;
                    if (unk_800c9f48(id, party->position.y) >= 1.0f) {
                        getFrontFloor(party, &t, party->unk100);
                        if (t.y - party->position.y > 5.0f) {
                            party->unk94 = t;
                            partyChgMot(party, 2);
                            return;
                        }
                        if (!(party->flags & 0x30)) {
                            partyChkFrontStep(party, 11.0f, &fsy, &fsa, &fsb);
                            if (fsy - party->position.y >= 5.0f && party->unkE0 <= 30.0f) {
                                party->unk120 = fsy;
                                if (party->unk104) {
                                    partyChgMot(party, 6);
                                    return;
                                }
                            }
                        }
                    } else {
                        dir = partyGetTargetDir(id, party->position.x, party->position.z);
                        getFrontFloor(party, &t, dir);
                        a = t.y - party->position.y;
                        if (a > 100.0f) {
                            party->position = t;
                            partyChgMot(party, 1);
                        } else if (a >= 0.0f) {
                            party->unk94 = t;
                            partyChgMot(party, 2);
                        } else {
                            partyChgMot(party, 0);
                        }
                    }
                }
            }
        }
    }
    party->position.x = v.x;
    party->position.z = v.z;
    hh = party->unkF0;
    hp = party->position;
    hit = partySearchHead(party, &hp, &hh, party->unk10C);
    if (hit) {
        party->unk140 = hit;
    }
    party->flags &= ~0x80;
    hit = partySearchGround(party, 10.0f, 10.0f);
    v.y = party->unkE4;
    if (hit) {
        attr = hitGetAttr(hit);
        if (attr & 0x200) {
            hit = NULL;
            if (!party->unk4C) {
                party->flags3 |= 0x1000000;
                party->flags |= 0x2000000;
            }
        } else if (attr & 0x800) {
            partyChgRunMode(party, 13);
            return;
        }
    }
    party->unk138 = hit;
    if (!hit) {
        partyChgMot(party, 3);
        party->unk11C = -1.0f;
    } else {
        if (hitGetAttr(hit) & 0x100) {
            party->flags |= 0x80;
        } else {
            party->flags &= ~0x80;
        }
        if (hitGetAttr(hit) & 0x40000000) {
            party->unk14C = hit;
        }
    }
    party->position.y = v.y;
    party_force_reset_outofscreen(party);
    partyGorFix(party);
}

void party_motion_homing_fly(PartyEntry* party) {
    PartyMarioView* mario = party->playerPtr;
    s32 id;
    s32 near;
    s32 w;
    HitEntry* hit;
    PartyMarioView* m2;
    PartyFootmark* fm;
    u32 attr;
    f32 lim;
    f32 lim2;
    f32 spd;
    f32 px, py, pz;
    f32 d3;
    f32 dd;
    f32 a;
    f32 off;
    f32 gy;
    Vec v;
    Vec s2;
    Vec ap;
    Vec ff;
    Vec p2;
    Vec p3;
    f32 dir, sp;
    f32 tx, ty, tz;
    f32 x, z;

    id = partyGetFootmarkId(party);
    if (party->flags3 & 2) {
        party->flags3 &= ~2;
        party->flags &= ~0x80000;
        party->unk4E = 0;
    }
    if (party->unk28 != 0) {
        lim = 28.0f;
    } else {
        lim = 32.0f;
    }
    if (!strcmp(gp->currentMapName, "aji_02") || !strcmp(gp->currentMapName, "aji_05") ||
        !strcmp(gp->currentMapName, "aji_12"))
    {
        if (!marioKeyOffChk()) {
            lim = partyStayDist(party);
            party->flags3 |= 0x20000;
        }
    }
    if (partyGetTargetDistY(id)) {
        lim2 = 50.0f;
    } else {
        lim2 = 35.0f;
    }
    if ((party->flags3 >> 17) & 1) {
        lim2 = partyStayDist(party);
    }
    if (!party_slit(party)) {
        return;
    }
    if (!party_roll(party)) {
        return;
    }
    if (mario->motionId == 0xE && !(party->flags & 0x80000)) {
        partyMoveBehindMario(party, 0);
        return;
    }
    if (!partyChkMarioFollowY(party->playerPtr)) {
        partyChgMot(party, 0);
        return;
    }
    spd = partyGetTargetMovespd2(party);
    d3 = PSVECDistance(&mario->position, &party->position);
    if (d3 < 35.0f) {
        spd = 1.0f;
    } else if (d3 >= 50.0f) {
        spd = 3.0f;
    }
    if (party->playerPtr->flags0 & 0x8000) {
        spd = 1.0f;
        lim = 100.0f;
    }
    unk_800c93d0(party, spd);
    px = party->position.x;
    py = party->position.y;
    pz = party->position.z;
    fm = partyGetFootmarkPtr(id);
    tx = fm->pos.x;
    off = partyHomingOff(party);
    ty = fm->pos.y + off;
    tz = fm->pos.z;
    p2 = party->position;
    if (partyGetTargetDist3D(id, &p2) >= lim) {
        unk_800c93d0(party, spd);
    }
    if (lim2 < 20.0f) {
        unk_800c93d0(party, spd);
    }
    near = 0;
    while (TRUE) {
        dir = angleABf(px, pz, tx, tz);
        dd = distABf(px, pz, tx, tz);
        if (mario->flags0 & 0x8000) {
            a = party->unkF4 + mario->unk1B4;
        } else {
            a = 0.5f * party->unkF4 + party->unk104;
        }
        if (dd > a) {
            p3 = (Vec){px, py, pz};
            d3 = partyGetTargetDist3D(id, &p3);
            if (d3 >= lim) {
                if (party->flags3 & 0x20000) {
                    a = 0.1f * (d3 - lim2);
                    if (a > 4.0f) {
                        a = 4.0f;
                    }
                    if (a < 0.5f) {
                        a = 0.0f;
                    }
                    unk_800c9398(party, mario->unk180 + a);
                }
                near = 1;
                break;
            }
            if (marioGetSlitFloorPos(&v)) {
                unk_800c93d0(party, 1.5f);
                partyNextReadId(id);
                if (marioGetSlitFloorPos(&v)) {
                    s2 = v;
                    py = distABf(px, pz, s2.x, s2.z);
                    dir = angleABf(px, pz, s2.x, s2.z);
                    if (py <= 5.0f) {
                        ((void (*)(PartyEntry*))partyChgMotSlitFloor)(party);
                    }
                }
                break;
            }
            if (d3 > lim2) {
                if (party->flags3 & 0x20000) {
                    a = 0.1f * (d3 - lim2);
                    if (a > 4.0f) {
                        a = 4.0f;
                    }
                    if (a < 0.5f) {
                        a = 0.0f;
                    }
                    unk_800c9398(party, mario->unk180 + a);
                } else {
                    a = d3 - lim2;
                    if (a >= 1.0f) {
                        a = 1.0f;
                        if (lim2 < 20.0f) {
                            a = spd;
                        }
                    }
                    unk_800c93d0(party, a);
                    break;
                }
            } else if (party->unkA0 == 0.0f && party->unkA8 == 0.0f) {
                unk_800c93d0(party, party->playerPtr->unk180);
            } else {
                unk_800c9398(party, 0.0f);
                party->unk100 = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
                partyChgPoseId(party, 1);
            }
        }
        if (partyGetFootmarkReadId(id) == (w = partyGetFootmarkWriteId(id))) {
            unk_800c93d0(party, party->playerPtr->unk180);
            if (party->playerPtr->unk180 == 0.0f || party->unkA0 != 0.0f || party->unkA8 != 0.0f) {
                dir = party->unk100;
                unk_800c93d0(party, 0.0f);
                party->unk11C = 0.0f;
                if (!(party->flags & 0x200)) {
                    partyChgPoseId(party, 1);
                }
                partyChgMot(party, 0);
            }
            break;
        }
        partyNextReadId(id);
        partyGetReadId(id, &tx, &ty, &tz);
        if (party->flags & 0x4000000) {
            continue;
        }
        party->flags &= ~0x80;
        hit = partySearchGround(party, 100.0f, 100.0f);
        if (hit && (hitGetAttr(hit) & 0x200)) {
            hit = NULL;
        }
        party->unk138 = hit;
        if (!hit) {
            partyChgMot(party, 3);
            party->unk11C = -1.0f;
            continue;
        }
        attr = hitGetAttr(hit);
        if (attr & 0x100) {
            party->flags |= 0x80;
        } else {
            party->flags &= ~0x80;
        }
        if (attr & 0x800) {
            partyChgMot(party, 0);
            break;
        }
    }
    if (party->unk104) {
        unk_800c8f24(party, party->unk104);
    } else {
        partyChgMot(party, 0);
    }
    if (party->unk4E > 10) {
        dir = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
    }
    if (near) {
        m2 = party->playerPtr;
        partyGetAppearPos3(party, &ap);
        dir = angleABf(party->position.x, party->position.z, ap.x, ap.z);
        hit = partySearchGround(party, 10.0f, 0.0f);
        ap.y = party->unkE4;
        if (hit && (hitGetAttr(hit) & 0x200)) {
            hit = NULL;
        }
        if (hit) {
            party->position.y = ap.y;
        } else {
            off = partyHomingOff(party);
            party->position.y += 0.05f * ((m2->position.y + off) - party->position.y);
        }
    }
    party->unk100 = dir;
    v = party->position;
    partyGetMoveDirSpd(party, &dir, &sp);
    hit = partySearchFrontWall(party, &v, sp, dir);
    if (party->flags & 0x1000000) {
        hit = NULL;
    }
    if (!hit) {
        if (!(party->flags3 & 0x200000)) {
            getFrontFloor(party, &ff, party->unk100);
        } else {
            party->unk54 = revise360(270.0f + mario->unk198);
            partyGetAppearPos4(party, &ff);
        }
        if (ff.y >= mario->position.y) {
            off = partyHomingOff(party);
            party->position.y += 0.05f * ((ff.y + off) - party->position.y);
        }
        party->unk4E = 0;
        if (party->unk104) {
            v.x = party->position.x;
            v.z = party->position.z;
            movePos(&v.x, &v.z, party->unk104, party->unk100);
        }
    } else {
        party->unk12C = hit;
        if (party->unk104) {
            party->unk134 = hit;
            if (hitGetAttr(hit) & 0x2000) {
                if (party->unk37 != 1) {
                    if (!(party->playerPtr->flags & 0x80000000)) {
                        party->flags3 |= 1;
                        party->unk37 = 1;
                    }
                } else if (party->unk104) {
                    v.x = party->position.x;
                    v.z = party->position.z;
                    movePos(&v.x, &v.z, party->unk104, party->unk100);
                }
            } else {
                x = party->position.x;
                gy = party->position.y;
                z = party->position.z;
                movePos(&x, &z, party->unkF4, dir);
                gy = partyGetGndY(x, gy, z);
                if (gy > -2000.0f) {
                    party->position.y += 0.05f * ((gy + party->unkF0) - party->position.y);
                } else {
                    party->unk4E++;
                    if (unk_800c9f48(id, party->position.y) >= 1.0f &&
                        __fabsf(mario->position.y - party->position.y) >= 10.0f)
                    {
                        party->position.y = party->position.y + 0.1f * ((10.0f + mario->position.y) - party->position.y);
                        party->position.x = v.x;
                        party->position.z = v.z;
                        return;
                    }
                }
            }
        }
    }
    party->position.x = v.x;
    party->position.z = v.z;
    party->flags &= ~0x80;
    hit = partySearchGround(party, 10.0f, 10.0f);
    if (hit) {
        attr = hitGetAttr(hit);
        if (attr & 0x800) {
            partyChgRunMode(party, 13);
            return;
        }
        if (attr & 0x100) {
            party->flags |= 0x80;
        } else {
            party->flags &= ~0x80;
        }
    }
    party_force_reset_outofscreen(party);
}

void party_set_behind_dir(PartyEntry* party, f32 dir) {
    party->unk54 = revise360(dir + ((PartyCamView*)camGetPtr((CameraId)4))->unk114);
    party->flags3 |= 0x100000;
}

static inline f32 partyGetFollowDir(PartyEntry* party) {
    PartyMarioView* mario = party->playerPtr;
    PartyEntry* other;
    if (!(party->flags & 8)) {
        return toMovedir(mario->unk1A8);
    }
    other = partyGetPtr(marioGetPartyId());
    if (other) {
        return partyToMovedir(party, other->unk10C);
    }
    return toMovedir(mario->unk1A8);
}

void partyGetMoveDirSpd(PartyEntry* party, f32* dir, f32* spd);
PartyEntry* anotherPartyGetPtr(s32 id);

void party_motion_behind_mario(PartyEntry* party) {
    PartyMarioView* mario = party->playerPtr;
    HitEntry* hit;
    f32 ang;
    f32 gy;
    f32 dist;
    f32 thr;
    f32 ty;
    f32 x, z;
    Vec target;

    if (party->flags3 & 2) {
        party->flags3 &= ~2;
        party->flags3 |= 0x40000;
        if (!(party->flags3 & 0x100000)) {
            {
            f32 t;
            if (!(party->flags3 & 0x10000)) {
                t = partyGetFollowDir(party); t += 180.0f; t = revise360(t);
            } else {
                party->flags3 &= ~0x10000;
                t = revise360(180.0f + angleABf(mario->position.x, mario->position.z, party->unk94.x, party->unk94.z));
            }
            party->unk54 = t;
            }
        }
        partyGetAppearPos4(party, &target);
        party->unk24 = distABf(party->position.x, party->position.z, target.x, target.z);
        if (party->unk24 <= 0) {
            party->unk24 = 1;
        }
    }
    if (!(party->flags3 & 0x100000) && marioKeyOffChk()) {
        { f32 t2 = partyGetFollowDir(party); t2 += 180.0f; party->unk54 = revise360(t2); }
    }
    partyGetAppearPos4(party, &target);
    x = target.x;
    z = target.z;
    ty = target.y;
    ang = angleABf(party->position.x, party->position.z, x, z);
    {
        f32 py = ty;
        f32 nz, ny, nx, dd, hz, hy, hx;
        py += touch_wall_ct;
        dd = 1000.0f;
        hit = hitCheckFilter(NULL, &hx, &hy, &hz, &dd, &nx, &ny, &nz, x, py, z, 0.0f, -1.0f, 0.0f);
        if (!hit) {
            gy = -2000.0f;
        } else {
            gy = (s32)(100.0f * hy + 0.5f) / 100.0f;
            hitGetAttr(hit);
        }
    }
    if (gy <= -2000.0f) {
        gy = 1.5f * mario->unk1B4;
        movePos(&x, &z, gy, toMovedir(mario->unk1A8));
    }
    thr = 2.0f;
    if (mario->unk180 > 1.0f) {
        thr = 10.0f;
    }
    dist = distABf(party->position.x, party->position.z, x, z);
    party->unk24--;
    if (dist >= thr && party->unk24 > 0) {
        party->unk100 = angleABf(party->position.x, party->position.z, x, z);
        distABf(party->position.x, party->position.z, x, z);
        unk_800c9398(party, (party->flags3 & 0x100000) ? 3.6f : 2.0f);
        partyMoveCheckWall(party);
        unk_800c8f24(party, party->unk104);
        if (party->unk12C) {
            x = party->position.x;
            z = party->position.z;
            movePos(&x, &z, dist, ang);
            angleABf(party->position.x, party->position.z, x, z);
            {
                f32 py = ty;
                f32 GG;
                f32 nz, ny, nx, dd, hz, hy, hx;
                py += touch_wall_ct;
                dd = 1000.0f;
                hit = hitCheckFilter(NULL, &hx, &hy, &hz, &dd, &nx, &ny, &nz, x, py, z, 0.0f, -1.0f, 0.0f);
                if (!hit) {
                    GG = -2000.0f;
                } else {
                    GG = (s32)(100.0f * hy + 0.5f) / 100.0f;
                    hitGetAttr(hit);
                }
                thr = GG;
            }
            if (__fabsf(party->position.y - thr) > 1.0f) {
                party->unk94.x = x;
                party->unk94.y = thr;
                party->unk94.z = z;
                partyChgMot(party, 2);
                return;
            }
        } else if (ty > party->position.y) {
            x = party->position.x;
            z = party->position.z;
            movePos(&x, &z, dist, ang);
            angleABf(party->position.x, party->position.z, x, z);
            {
                f32 py = ty;
                f32 GG;
                f32 nz, ny, nx, dd, hz, hy, hx;
                py += touch_wall_ct;
                dd = 1000.0f;
                hit = hitCheckFilter(NULL, &hx, &hy, &hz, &dd, &nx, &ny, &nz, x, py, z, 0.0f, -1.0f, 0.0f);
                if (!hit) {
                    GG = -2000.0f;
                } else {
                    GG = (s32)(100.0f * hy + 0.5f) / 100.0f;
                    hitGetAttr(hit);
                }
                thr = GG;
            }
            if (__fabsf(party->position.y - thr) > 1.0f) {
                party->unk94.x = x;
                party->unk94.y = thr;
                party->unk94.z = z;
                partyChgMot(party, 2);
                return;
            }
        }
        hit = partyBehindGround(party);
        party->unk138 = hit;
        if (!hit) {
            partyChgMot(party, 3);
            party->unk11C = -1.0f;
        }
        party->position.y = party->unkE4;
    } else {
        party->unk24 = 0;
        if (party->flags3 & 0x200000) {
            party->flags3 &= ~0x20000;
        }
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
        partySetDir(party, mario->position.x, mario->position.z);
        party->unk40 = 50.0f;
        party->flags2 &= ~0x10;
        party->flags |= 0x80000;
        party->flags3 &= ~0x100000;
        party->flags3 &= ~0x40000;
    }
    if (party->unk134) {
        party->flags |= 0x80000;
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
        partySetDir(party, x, z);
        party->unk40 = 50.0f;
        party->flags2 &= ~0x10;
        party->flags3 &= ~0x40000;
    }
}

void party_motion_behind_mario_continue(PartyEntry* party) {
    PartyMarioView* mario = party->playerPtr;
    PartyEntry* other;
    HitEntry* hit;
    f32 y;
    f32 gy;
    f32 sp;
    f32 thr;
    f32 x, z;

    { f32 t = partyGetFollowDir(party); t += 180.0f; party->unk54 = revise360(t); }
    if (party->flags3 & 2) {
        party->flags3 &= ~2;
        sp = 1.5f * mario->unk1B4;
        x = mario->position.x;
        z = mario->position.z;
        movePos(&x, &z, sp, party->unk54);
        party->unk24 = distABf(party->position.x, party->position.z, x, z);
        if (party->unk24 <= 0) {
            party->unk24 = 1;
        }
    }
    if (marioKeyOffChk()) {
        { f32 t = partyGetFollowDir(party); t += 180.0f; party->unk54 = revise360(t); }
    }
    if (!(party->flags & 8)) {
        sp = 1.5f * mario->unk1B4;
        x = mario->position.x;
        z = mario->position.z;
        y = mario->position.y;
    } else {
        sp = 1.5f * mario->unk1B4;
        other = anotherPartyGetPtr(party->unk2F);
        if (other) {
            x = other->position.x;
            z = other->position.z;
            y = other->position.y;
        } else {
            x = mario->position.x;
            z = mario->position.z;
            y = mario->position.y;
        }
    }
    movePos(&x, &z, sp, party->unk54);
    angleABf(party->position.x, party->position.z, x, z);
    {
        f32 py = y;
        f32 nz, ny, nx, dist, hz, hy, hx;
        py += touch_wall_ct;
        dist = 1000.0f;
        hit = hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, x, py, z, 0.0f, -1.0f, 0.0f);
        if (!hit) {
            gy = -2000.0f;
        } else {
            gy = (s32)(100.0f * hy + 0.5f) / 100.0f;
            hitGetAttr(hit);
        }
    }
    if (gy <= -2000.0f) {
        sp = 1.5f * mario->unk1B4;
        movePos(&x, &z, sp, toMovedir(mario->unk1A8));
    }
    thr = 2.0f;
    if (mario->unk180 > 1.0f) {
        thr = 10.0f;
    }
    sp = distABf(party->position.x, party->position.z, x, z);
    party->unk24--;
    if (sp >= thr) {
        party->unk100 = angleABf(party->position.x, party->position.z, x, z);
        sp = distABf(party->position.x, party->position.z, x, z) / 10.0f;
        if (sp > 2.0f) {
            sp = 2.0f;
        }
        if (sp < 1.0f) {
            sp = 1.0f;
        }
        unk_800c9398(party, sp);
        partyMoveCheckWall(party);
        unk_800c8f24(party, party->unk104);
        hit = partyBehindGround(party);
        party->unk138 = hit;
        if (!hit) {
            partyChgMot(party, 3);
            party->unk11C = -1.0f;
        }
        party->position.y = party->unkE4;
    } else {
        party->unk24 = 0;
        if (mario->unk180 == 0.0f) {
            party->flags |= 0x80000;
            partyChgMoveMode(party, 1);
            partyChgMot(party, 0);
            partySetDir(party, mario->position.x, mario->position.z);
            party->unk40 = 50.0f;
            party->flags2 &= ~0x10;
        }
    }
    if (party->unk134) {
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
        partySetDir(party, x, z);
        party->unk40 = 50.0f;
        party->flags2 &= ~0x10;
    }
}

void party_motion_behind_mario_fly(PartyEntry* party) {
    PartyMarioView* mario = party->playerPtr;
    HitEntry* hit;
    f32 thr;
    f32 spd;
    f32 dist;
    Vec pos;
    f32 sp, dir;

    if (party->flags3 & 2) {
        party->flags3 &= ~2;
        party->flags3 |= 0x40000;
        if (!(party->flags3 & 0x100000)) {
            {
            f32 t;
            if (!(party->flags3 & 0x10000)) {
                t = partyGetFollowDir(party); t += 180.0f; t = revise360(t);
            } else {
                party->flags3 &= ~0x10000;
                t = revise360(angleABf(mario->position.x, mario->position.z, party->unk94.x, party->unk94.z) + 180.0f);
            }
            party->unk54 = t;
            }
        }
        partyGetAppearPos4(party, &party->unk94);
        party->unk24 = distABf(party->position.x, party->position.z, party->unk94.x, party->unk94.z);
        if (party->unk24 <= 0) {
            party->unk24 = 1;
        }
    }
    if (!(party->flags3 & 0x100000) && marioKeyOffChk()) {
        f32 t2 = partyGetFollowDir(party); t2 += 180.0f; party->unk54 = revise360(t2);
    }
    angleABf(party->position.x, party->position.z, party->unk94.x, party->unk94.z);
    if (mario->unk180 > 1.0f) {
        thr = 6.0f;
    } else {
        thr = 2.0f;
    }
    dist = distABf(party->position.x, party->position.z, party->unk94.x, party->unk94.z);
    party->unk24--;
    if ((dist >= thr || __fabsf(party->unk94.y - party->position.y) > 2.0f) && party->unk24 > 0) {
        party->unk100 = angleABf(party->position.x, party->position.z, party->unk94.x, party->unk94.z);
        distABf(party->position.x, party->position.z, party->unk94.x, party->unk94.z);
        if (party->flags3 & 0x100000) {
            spd = 3.6f;
        } else {
            spd = 2.0f;
        }
        if (dist < thr) {
            spd = 0.0f;
            unk_800c9398(party, spd);
        }
        unk_800c93d0(party, spd);
        pos = party->position;
        partyGetMoveDirSpd(party, &dir, &sp);
        hit = partySearchFrontWall(party, &pos, sp, dir);
        if (hit) {
            party->unk12C = hit;
            if (party->unk104) {
                party->unk134 = hit;
            }
        }
        party->position.x = pos.x;
        party->position.z = pos.z;
        party->position.y += 0.05f * (party->unk94.y - party->position.y);
        unk_800c8f24(party, party->unk104);
        return;
    }
    party->unk24 = 0;
    if (party->flags3 & 0x200000) {
        party->flags3 &= ~0x20000;
        if (party->unk32 == 1) {
            party->flags2 &= ~0x10;
        }
    }
    partyChgMoveMode(party, 1);
    partyChgMot(party, 0);
    partySetDir(party, mario->position.x, mario->position.z);
    party->unk40 = 50.0f;
    party->flags2 &= ~0x10;
    unk_800c9398(party, 0.0f);
    party->flags |= 0x80000;
    party->flags3 &= ~0x100000;
    party->flags3 &= ~0x40000;
    unk_800c8e74(party);
    if (party->unk134) {
        party->flags |= 0x80000;
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
        partySetDir(party, mario->position.x, mario->position.z);
        party->unk40 = 50.0f;
        party->flags2 &= ~0x10;
        party->flags3 &= ~0x40000;
    }
    hit = partySearchGround(party, 100.0f, 100.0f);
    if (hit) {
        if (hitGetAttr(hit) & 0x200) {
            hit = NULL;
            if (!party->unk4C) {
                party->flags3 |= 0x1000000;
                party->flags |= 0x2000000;
            }
        }
        if (dist > party->unkF0) {
            hit = NULL;
        }
        party->position.y += 0.05f * (party->unkE4 - party->position.y);
    }
    party->unk138 = hit;
}

void party_motion_beside_mario(PartyEntry* party) {
    PartyMarioView* mario = party->playerPtr;
    HitEntry* hit;
    f32 x, z;
    f32 y;
    f32 nz, ny, nx, dist, hz, hy, hx;
    f32 thr;
    f32 sp;
    f32 t;

    if (party->flags3 & 2) {
        party->flags3 &= ~2;
        t = partyGetFollowDir(party); t += 90.0f; party->unk54 = revise360(t);
        sp = 1.5f * mario->unk1B4;
        x = mario->position.x;
        z = mario->position.z;
        movePos(&x, &z, sp, party->unk54);
        party->unk24 = distABf(party->position.x, party->position.z, x, z);
        if (party->unk24 <= 0) {
            party->unk24 = 1;
        }
    }
    if (marioKeyOffChk()) {
        t = partyGetFollowDir(party); t += 90.0f; party->unk54 = revise360(t);
    }
    sp = 1.5f * mario->unk1B4;
    x = mario->position.x;
    z = mario->position.z;
    y = mario->position.y;
    movePos(&x, &z, sp, party->unk54);
    angleABf(party->position.x, party->position.z, x, z);
    y = partyGetGndY(x, y, z);
    if (y <= -2000.0f) {
        sp = 1.5f * mario->unk1B4;
        x = mario->position.x;
        z = mario->position.z;
        movePos(&x, &z, sp, toMovedir(mario->unk1A8));
    }
    thr = 2.0f;
    if (mario->unk180 > 1.0f) {
        thr = 10.0f;
    }
    sp = distABf(party->position.x, party->position.z, x, z);
    party->unk24--;
    if (sp >= thr && party->unk24 > 0) {
        party->unk100 = angleABf(party->position.x, party->position.z, x, z);
        sp = distABf(party->position.x, party->position.z, x, z) / 10.0f;
        if (sp > 2.0f) {
            sp = 2.0f;
        }
        if (sp < 1.0f) {
            sp = 1.0f;
        }
        unk_800c9398(party, sp);
        partyMoveCheckWall(party);
        unk_800c8f24(party, party->unk104);
        hit = partySearchGround(party, 10.0f, 0.0f);
        if (hit && (hitGetAttr(hit) & 0x200)) {
            hit = NULL;
            if (!party->unk4C) {
                party->flags3 |= 0x1000000;
                party->flags |= 0x2000000;
            }
        }
        party->unk138 = hit;
        if (!hit) {
            partyChgMot(party, 3);
            party->unk11C = -1.0f;
        }
        party->position.y = party->unkE4;
    } else {
        party->unk24 = 0;
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
        partySetDir(party, mario->position.x, mario->position.z);
        party->unk40 = 50.0f;
        party->flags2 &= ~0x10;
        party->flags |= 0x80000;
    }
    if (party->unk134) {
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
        partySetDir(party, mario->position.x, mario->position.z);
        party->unk40 = 50.0f;
        party->flags2 &= ~0x10;
    }
}

static inline void partyJumpWallMove(PartyEntry* party) {
    Vec v;
    HitEntry* hit;

    if (party->flags & 0x1000000) {
        movePos(&v.x, &v.z, party->unk104, party->unk100);
        return;
    }
    v = party->position;
    hit = partySearchFrontWall(party, &v, party->unk104, party->unk100);
    if (hit) {
        party->unk12C = hit;
        if (party->unk104) {
            party->unk134 = hit;
        }
    }
    party->position.x = v.x;
    party->position.z = v.z;
}

void party_motion_jump(PartyEntry* party) {
    PartyMarioView* mario = party->playerPtr;
    s32 id;
    s32 w;
    HitEntry* hit;
    f32 dir;
    f32 dist;
    f32 lim;
    f32 t, dy, g;
    Vec fl;
    f32 s, c;
    f32 x, y, z;

    id = partyGetFootmarkId(party);
    if (party->flags3 & 2) {
        party->flags3 &= ~2;
        party->unk38 = 0;
    }
    switch (party->unk38) {
    case 0:
        x = party->unk94.x;
        y = party->unk94.y;
        z = party->unk94.z;
        if (party->unk28 != 0 || party->unk34 == 9) {
            x = party->position.x;
            y = party->position.y;
            z = party->position.z;
            movePos(&x, &z, party->unk104, party->unk100);
            party->unk94 = (Vec){x, y, z};
        } else if (!(party->flags3 & 0x400000)) {
            if (partyGetFootmarkReadId(id) == partyGetFootmarkWriteId(id)) {
                if (partyGetTargetDist(id, party->position.x, party->position.z) > 2.0f * mario->unk1B4) {
                    dir = partyGetTargetDir(id, party->position.x, party->position.z);
                    getFrontFloor(party, &fl, dir);
                    fl.y += 10.0f;
                    y = fl.y;
                    x = party->position.x;
                    z = party->position.z;
                    movePos(&x, &z, 0.6f * party->unkF4, dir);
                }
                party->unk94 = (Vec){x, y, z};
            } else {
                partyGetReadId(id, &x, &y, &z);
                lim = 50.0f;
                dist = distABf(party->position.x, party->position.z, x, z);
                if (50.0f > 1.4f * (y - party->position.y)) {
                    lim = 1.4f * (y - party->position.y);
                }
                sincosf(&s, &c, angleABf(party->position.x, party->position.z, x, z));
                if (dist > lim) {
                    dist = lim;
                    if (partyGetTargetDistY(id) == 0.0f) {
                        dist = 0.0f;
                    }
                    x = party->position.x;
                    z = party->position.z;
                    x = x + dist * s;
                    z = z + dist * c;
                    y += 10.0f;
                    party->unk94 = (Vec){x, y, z};
                } else {
                    if (partyGetTargetDistY(id) == 0.0f) {
                        dist = 0.0f;
                    }
                    x = x + dist * s;
                    z = z + dist * c;
                    party->unk94 = (Vec){x, y, z};
                }
                if (dist < lim) {
                    partyNextReadId(id);
                }
            }
        }
        party->unk128 = distABf(party->position.x, party->position.z, party->unk94.x, party->unk94.z);
        if (party->unk28 == 0) {
            party->unk100 = angleABf(party->position.x, party->position.z, party->unk94.x, party->unk94.z);
            party->unk104 = 2.25f * party->playerPtr->unk224;
        }
        party->unk24 = (10.0f * (0.9f * (party->unk128 / party->unk104)) + 0.5f) / 10.0f;
        if (party->unk24 < 12) {
            party->unk24 = 12;
        }
        if (party->unk24 > 40) {
            party->unk24 = 40;
        }
        dy = party->unk94.y - party->position.y;
        t = 0.5f * party->unk24;
        if (__fabsf(dy) < 1.0f) {
            dy = 25.0f;
        } else if (dy >= 0.0f) {
            x = x;
        } else {
            dy = 10.0f;
        }
        g = (dy + dy) / (t * t - t);
        party->unk114 = mario->unk224 * (g * t);
        if (party->unk114 > 13.0f) {
            party->unk114 = 13.0f;
        }
        if (party->unk114 < 3.0f) {
            party->unk114 = 3.0f;
        }
        party->unk11C = -g * mario->unk224;
        party->flags |= 0x10;
        party->flags &= ~2;
        partyChgPoseId(party, 4);
        psndSFXOn_3D((const char*)0x927, &party->position);
        party->unk38 = 10;
        if (!(party->flags & 8)) {
            partyNextReadId(partyGetFootmarkId(party));
        }
        break;
    case 10:
        if (!(party->flags & 8)) {
            partyNextReadId(partyGetFootmarkId(party));
        }
        party->unk114 += party->unk11C;
        party->position.y += party->unk114;
        if (party->position.y < -2000.0f) {
            party->flags &= ~0x10;
            party->unk114 = 0.0f;
            party->unk100 = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
            party->position = mario->position;
            partyChgMot(party, 0);
            break;
        }
        if (party->unk114 <= 0.0f) {
            party->flags &= ~0x10;
            party->flags |= 0x20;
            hit = partySearchGround(party, __fabsf(party->unk114), 0.0f);
            if (hit && (hitGetAttr(hit) & 0x200)) {
                if (!party->unk4C) {
                    party->flags3 |= 0x1000000;
                    party->flags |= 0x2000000;
                }
                party->unk138 = NULL;
                partyChgMot(party, 3);
                party->unk11C = -1.0f;
                break;
            }
            party->unk138 = hit;
            party->position.y = party->unkE4;
            if (hit) {
                party->flags |= 2;
                party->flags &= ~0x70;
                N_partyPlaySfxPartyLanding1(party);
                party->unk114 = 0.0f;
                partyChgMot(party, 0);
                party->flags3 |= 0x800000;
                break;
            }
        }
        partyJumpWallMove(party);
        break;
    }
}

void party_motion_jump2(PartyEntry* party) {
    PartyMarioView* mario = party->playerPtr;
    HitEntry* hit;
    Vec pos;
    f32 dist;
    f32 dy;
    f32 fr;
    s32 frames;

    if (party->flags3 & 2) {
        party->flags3 &= ~2;
        party->unk24 = 0;
        party->unk11C = -1.0f * mario->unk224;
        party->unk104 = 2.25f * mario->unk224;
        party->unk100 = angleABf(party->position.x, party->position.z, party->unk94.x, party->unk94.z);
        dist = distABf(party->position.x, party->position.z, party->unk94.x, party->unk94.z);
        if (dist > 50.0f) {
            dist = 50.0f;
        }
        party->unk128 = dist;
        dy = party->unk94.y - party->position.y;
        frames = (10.0f * (dist / party->unk104) + 0.5f) / 10.0f;
        if (frames < 10) {
            frames = 10;
        }
        fr = frames;
        party->unk24 = frames;
        party->unk104 = mario->unk224 * (party->unk128 / fr);
        party->unk114 = (0.5f * (fr * (fr * __fabsf(party->unk11C))) + dy) / fr;
        party->unk114 *= mario->unk224;
        if (party->unk114 > 20.0f) {
            party->unk114 = 20.0f;
        }
        if (party->unk114 < 0.0f) {
            party->unk114 = 0.0f;
        }
        partyChgPoseId(party, 4);
        party->flags |= 0x10;
        party->flags &= ~2;
        psndSFXOn_3D((const char*)0x927, &party->position);
        party->unk38 = 0;
    }
    switch (party->unk38) {
        case 0:
            party->unk38 = 1;
            break;
        case 1:
            party->unk114 += party->unk11C;
            party->position.y += party->unk114;
            pos = party->position;
            partySearchFrontWall(party, &pos, party->unk104, party->unk100);
            party->position.x = pos.x;
            party->position.z = pos.z;
            if (party->unk114 <= 0.0f) {
                hit = partySearchGround(party, __fabsf(party->unk114), 0.0f);
                pos.y = party->unkE4;
                if (hit && (hitGetAttr(hit) & 0x200)) {
                    if (!party->unk4C) {
                        party->flags3 |= 0x1000000;
                        party->flags |= 0x2000000;
                    }
                    party->unk138 = NULL;
                    partyChgMot(party, 3);
                    party->unk11C = -1.0f;
                    return;
                }
                party->position.y = pos.y;
                party->unk138 = hit;
                if (hit) {
                    N_partyPlaySfxPartyLanding1(party);
                    party->flags &= ~0x70;
                    party->flags |= 2;
                    party->unk114 = 0.0f;
                    partyChgMot(party, distABf(party->position.x, party->position.z, party->unk94.x, party->unk94.z) < 5.0f ? 0 : 1);
                }
            }
            break;
    }
    if (party->position.y < -2000.0f) {
        party->flags &= ~0x10;
        party->unk114 = 0.0f;
        party->unk100 = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
        party->position = mario->position;
        partyChgMot(party, 0);
    }
}

s32 party_motion_fall(PartyEntry* party) {
    PartyMarioView* mario = party->playerPtr;
    HitEntry* hit;
    Vec pos;

    if (!(party->flags & 8)) {
        partyNextReadId(partyGetFootmarkId());
    }
    if (party->flags3 & 2) {
        party->flags3 &= ~2;
        partyChgPoseId(party, 4);
        party->flags &= ~0x80000;
    }
    hit = NULL;
    if (!(party->flags & 0x2000000)) {
        hit = partySearchGround(party, 10.0f, __fabsf(party->unk114));
    }
    if (hit && (hitGetAttr(hit) & 0x200)) {
        hit = NULL;
        if (party->unk32 == 0 && !party->unk4C) {
            party->flags3 |= 0x1000000;
            party->flags |= 0x2000000;
        }
    }
    party->unk138 = hit;
    if (!hit) {
        party->flags &= ~2;
        if (!(party->flags & 0x20)) {
            party->flags |= 0x20;
            if (party->unk11C == 0.0f) {
                party->unk11C = -1.4f;
            }
        }
        party->unk114 += party->unk11C;
        if (party->unk114 < -16.0f) {
            party->unk114 = -16.0f;
        }
        party->position.y += party->unk114;
        if (party->position.y < -2000.0f) {
            party->position.y = -2001.0f;
            if (mario->motionId == 0x1F || mario->motionId == 0x20) {
                return 2;
            }
            party->flags &= ~0x70;
            party->unk114 = 0.0f;
            party->unk11C = 0.0f;
            party->unk100 = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
            party->unk54 = revise360(180.0f + toMovedir(mario->unk1AC));
            partyGetAppearPos4(party, &party->position);
            party->position.y += 37.0f;
            partyChgMot(party, 0);
            party->flags &= ~0x2000000;
        }
        if (party->unk104) {
            pos = party->position;
            partySearchFrontWall(party, &pos, 1.3f * party->unk104, party->unk100);
            party->position.x = pos.x;
            party->position.z = pos.z;
        }
        return 2;
    }
    if (party->flags & 0x20) {
        party->flags &= ~0x70;
        party->flags |= 2;
        party->unk114 = 0.0f;
        party->position.y = party->unkE4;
        if (party->unk104 == 0.0f) {
            partyChgMot(party, 0);
        } else {
            partyChgMot(party, 1);
        }
        N_partyPlaySfxPartyLanding1(party);
        return 3;
    }
    party->flags &= ~0x2000;
    party->flags &= ~0x70;
    party->flags |= 2;
    party->unk114 = 0.0f;
    party->position.y = party->unkE4;
    if (party->unk104 == 0.0f) {
        partyChgMot(party, 0);
    } else {
        partyChgMot(party, 1);
    }
    N_partyPlaySfxPartyLanding1(party);
    if (!(party->flags & 8)) {
        partyClearFootmark();
    } else {
        partyClearFootmark2();
    }
    return 3;
}

void party_motion_upstairs(PartyEntry* party) {
    if (party->flags3 & 2) {
        party->flags3 &= ~2;
        party->flags |= 0x40;
        unk_804189ec = 3;
        unk_804189e8 = (party->unk120 - party->position.y) / 3.0f;
    }
    party->position.y += unk_804189e8;
    if (--unk_804189ec <= 0) {
        party->flags &= ~0x40;
        partyChgMot(party, 1);
    }
}

static inline BOOL partyChkHitAttr200(HitEntry* hit) {
    if (!hit) {
        return FALSE;
    }
    if (hitGetAttr(hit) & 0x200) {
        return TRUE;
    }
    return FALSE;
}

void party_motion_damage(PartyEntry* party) {
    PartyMarioView* mario = party->playerPtr;
    f32 x, y, z;
    Vec ret;

    if (party->flags3 & 2) {
        party->flags3 &= ~2;
        party->unk38 = 0;
    }
    switch (party->unk38) {
        case 0:
            partyChgPoseId(party, 8);
            party->unk114 = 15.0f;
            party->unk118 = 0.0f;
            party->unk24 = 60;
            party->unk38 = 1;
        case 1:
            party->position.y += party->unk114;
            marioGetScreenPos(party->position, &x, &y, &z);
            if (y <= -300.0f) {
                party->unk24 = 20;
                party->unk38 = 10;
            }
            break;
        case 10:
            if (mario->motionId == 0x1F || mario->motionId == 0x20) {
                break;
            }
            if (--party->unk24 <= 0) {
                partyGetReturnPos(&ret);
                party->position.x = ret.x;
                party->position.y = 200.0f + ret.y;
                party->position.z = ret.z;
                party->unk114 = 0.0f;
                party->unk118 = -1.0f;
                unk_800c9398(party, 0.0f);
                party->flags |= 0x20;
                party->flags |= 0x2000000;
                party->unk38 = 11;
            }
            break;
        case 11:
            marioGetScreenPos(party->position, &x, &y, &z);
            if (y > 240.0f) {
                party->flags &= ~0x2000000;
            }
            partyMove(party);
            if (party->unk114 <= -6.0f) {
                party->unk118 = 0.0f;
                party->unk114 = -6.0f;
            }
            if (party->position.y < -2000.0f) {
                party->position = mario->position;
                party->position.y += 200.0f;
                party->flags &= ~0x70;
            }
            if (party->unk138 && !partyChkHitAttr200(party->unk138)) {
                party->unk38 = 12;
                party->unk24 = 30;
            }
            break;
        case 12:
            if (--party->unk24 <= 0) {
                partyChgRunMode(party, 2);
                party->unk3B = 0;
                party->flags2 &= ~0x10;
            }
            break;
    }
}

void allPartySlitOn(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (party) {
        if (!(party->playerPtr->flags & 0x80000000)) {
            party->flags3 |= 1;
            party->unk37 = 1;
        }
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party) {
        if (!(party->playerPtr->flags & 0x80000000)) {
            party->flags3 |= 1;
            party->unk37 = 1;
        }
    }
}

void allPartySlitOff(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (party && (party->flags & 0x1000) && party->unk3A < 0x28) {
        party->unk3A = 0x28;
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party && (party->flags & 0x1000) && party->unk3A < 0x28) {
        party->unk3A = 0x28;
    }
}

void allPartyForceSlitOff(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (party) {
        L_partyForceSlitOff(party);
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party) {
        L_partyForceSlitOff(party);
    }
}

void L_partyForceSlitOff(PartyEntry* party) {
    if ((party->flags & 0x1000) || (party->flags2 & 0x100)) {
        party->unk10C = revise360(party->unk100 - party->unkF8) >= 180.0f ? 0.0f : 180.0f;
        party->unk110 = party->unk10C;
        party->unkFC = revise360(270.0f - party->unk10C);
        party->unk100 = party->unkFC;
        party->flags2 &= ~0x3C0;
        partyPaperOff(party);
        party->flags &= ~0x3000;
        party->unk37 = 0;
        partyChgPoseId(party, 1);
    }
}

void partyChgMotSlitFloor(PartyEntry* party) {
    if (party->flags & 0x2000) {
        return;
    }
    if (!(party->playerPtr->flags & 0x80000000)) {
        party->flags3 |= 1;
        party->unk37 = 1;
    }
    party->flags |= 0x2000;
}

BOOL party_slit(PartyEntry* party) {
    BOOL ret = TRUE;
    PartyMarioView* mario;
    f32 a;

    mario = party->playerPtr;
    if (party->unk37 != 1) {
        return TRUE;
    }
    if (party->flags3 & 1) {
        party->flags3 &= ~1;
        party->flags2 |= 0x100;
        partyPaperOn(party, "p_slit");
        partyChgPaper(party, "PM_S_1A");
        party->unk24 = 30;
        unk_800c9398(party, 0.0f);
        if (revise360(party->unk100 - mario->unk198) <= 180.0f) {
            party->flags2 &= ~0x200;
            party->unk3A = 0;
        } else {
            party->flags2 |= 0x200;
            party->unk3A = 2;
        }
    }
    switch (party->unk3A) {
        case 0:
            if (--party->unk24 < 0) {
                party->flags |= 0x1000;
                party->unk10C = 270.0f;
                party->unk110 = party->unk10C;
                partyChgPaper(party, "PM_S_1B");
                if (!(party->flags & 0x2000)) {
                    party->unk3A = 10;
                    ret = FALSE;
                } else {
                    party->unk3A = 20;
                }
            }
            break;
        case 2:
            if (--party->unk24 < 0) {
                party->flags |= 0x1000;
                party->unk10C = 270.0f;
                party->unk110 = party->unk10C;
                partyChgPaper(party, "PM_S_1B");
                if (!(party->flags & 0x2000)) {
                    party->unk3A = 10;
                    ret = FALSE;
                } else {
                    party->unk3A = 20;
                }
            }
            break;
        case 10:
            if (!party->unk104) {
                partyChgPaper(party, "PM_S_1B");
                party->unk10C = 270.0f;
            } else {
                if (revise360(party->unk100 - mario->unk198) >= 180.0) {
                    party->flags2 |= 0x200;
                } else {
                    party->flags2 &= ~0x200;
                }
                party->unk10C = (party->flags2 & 0x200) ? 280.0f : 260.0f;
                a = revise360(party->unk100 - mario->unk198);
                if (a >= 315.0f || a <= 45.0f || (a >= 135.0f && a <= 225.0f)) {
                    partyChgPaper(party, "PM_S_1E");
                } else {
                    partyChgPaper(party, "PM_S_1D");
                }
            }
            if (!(mario->flags0 & 0x100000) && !(hitGetAttr(party->unk12C) & 0x2000) &&
                !(hitGetAttr(party->unk134) & 0x2000))
            {
                party->unk3A = 40;
            }
            break;
    }
    switch (party->unk3A) {
        case 20:
            partyChgPaper(party, "PM_S_1H");
            party->flags |= 0x2000000;
            party->flags |= 0x20;
            party->unk24 = 20;
            unk_800c9398(party, 0.0f);
            party->unk3A = 21;
            break;
        case 21:
            party->unk10C = revise360(party->unk100 - party->unkF8) <= 180.0f ? 260.0f : 280.0f;
            party->position.y += -3.0f;
            if (--party->unk24 <= 0) {
                party->unk3A = 22;
            }
            break;
        case 22:
            party->unk3A = 10;
            party->flags &= ~0x2000000;
            marioClearSlitFloor();
            break;
    }
    switch (party->unk3A) {
        case 40:
            partyChgPaper(party, "PM_S_1C");
            party->unk24 = 30;
            party->unk3A = 41;
            party->unk10C = revise360(party->unk100 - mario->unk198) >= 180.0f ? 0.0f : 180.0f;
            party->unk110 = party->unk10C;
        case 41:
            if (--party->unk24 <= 0) {
                party->unk3A = 42;
            }
            break;
        case 42:
            party->flags2 &= ~0x3C0;
            partyPaperOff(party);
            party->flags &= ~0x3000;
            party->unk37 = 0;
            break;
    }
    return ret;
}

BOOL party_roll(PartyEntry* party) {
    BOOL ret = TRUE;
    f32 d;
    f32 x;
    f32 t;

    if (party->unk37 != 2) {
        return TRUE;
    }
    if (party->flags3 & 1) {
        party->flags3 &= ~1;
        partyPaperOn(party, "p_roll");
        partyChgPaper(party, "PM_R_1A");
        partyChgPoseId(party, 0);
        party->unk88.x = -6.0f;
        party->unk88.y = 6.0f;
        party->unk7C.x = 6.0f;
        party->unk7C.y = 2.0f;
        party->unk24 = 0x41;
        party->unk3A = 0;
        unk_800c9398(party, 0.0f);
    }
    switch (party->unk3A) {
        case 0:
            party->unk24--;
            if (party->unk24 <= 0) {
                partyChgPaper(party, "PM_R_1B");
                party->unk3A = 10;
            }
            break;
        case 10:
            party->unk10C = revise360(270.0f - party->unk100);
            if (revise360(party->unkFC + party->unk10C) > 180.0f) {
                party->unk6C = revise360(7.0f * party->unk104 + party->unk6C);
            } else {
                party->unk6C = revise360(party->unk6C - 7.0f * party->unk104);
            }
            break;
    }
    switch (party->unk3A) {
        case 100:
            party->unk104 = 0.0f;
            party->unk64 = 0.0f;
            party->unk6C = 0.0f;
            party->unk88.x = 0.0f;
            party->unk88.y = 0.0f;
            party->unk7C.x = party->unk100 < 180.0f ? 3.0f : -3.0f;
            party->unk7C.y = 0.0f;
            party->unk24 = 0x41;
            party->unk3A = 101;
            partyChgPaper(party, "PM_R_1C");
            partyPaperLightOff(party);
        case 101:
            ret = FALSE;
            party->unk24--;
            if (party->unk24 > 0) {
                if (party->unk100 < 180.0f) {
                    x = 0.1f * (90.0f - party->unk10C);
                    x += party->unk10C;
                    if (__fabsf(90.0f - x < 1.0f)) {
                        x = 90.0f;
                    }
                    party->unk10C = x;
                    party->unk110 = x;
                } else {
                    x = 0.1f * (270.0f - party->unk10C);
                    x += party->unk10C;
                    if (__fabsf(270.0f - x < 1.0f)) {
                        x = 270.0f;
                    }
                    party->unk10C = x;
                    party->unk110 = x;
                }
            } else {
                ret = TRUE;
                if (party->unk100 >= 180.0f) {
                    party->unk10C = 0.0f;
                    party->unk110 = 0.0f;
                    party->unkFC = 270.0f;
                    party->unk100 = 270.0f;
                } else {
                    party->unk10C = 180.0f;
                    party->unk110 = 180.0f;
                    party->unkFC = 90.0f;
                    party->unk100 = 90.0f;
                }
                party->unk88 = (Vec){0.0f, 0.0f, 0.0f};
                party->unk7C = (Vec){0.0f, 0.0f, 0.0f};
                party->unk37 = 0;
                party->flags &= ~0x1000;
                party->flags2 &= ~0x100;
                party->flags &= ~0x200;
                party->flags2 &= ~0x10;
                party->flags2 &= ~0x8;
                party->unk3A = 100;
                partyPaperOff(party);
            }
            break;
    }
    return ret;
}



static inline s32 pm_fpclassifyf(f32 x) { switch (*(s32*)&x & 0x7F800000) { case 0x7F800000: if (*(s32*)&x & 0x007FFFFF) { return 1; } else { return 2; } break; case 0: if (*(s32*)&x & 0x007FFFFF) { return 5; } else { return 3; } break; default: return 4; } }
static inline f32 pm_sqrtf(f32 x) { if (x > 0.0f) { f64 guess = __frsqrte(x); guess = 0.5 * guess * (3.0 - guess * guess * x); guess = 0.5 * guess * (3.0 - guess * guess * x); guess = 0.5 * guess * (3.0 - guess * guess * x); return x * guess; } else if (x < 0.0) { return *(f32*)__float_nan; } else if (pm_fpclassifyf(x) == 1) { return *(f32*)__float_nan; } return x; }







































static f32 partyStayDist(PartyEntry* party) {
    f32 d;
    if (party->unk32 == 0) {
        d = 18.0f;
    } else if (party->currentMemberId == 5) {
        if (party->flags3 & 0x200000) {
            d = 8.0f;
        } else {
            d = 30.0f;
        }
    } else if (party->currentMemberId == 6) {
        if (party->flags3 & 0x200000) {
            d = 8.0f;
        } else {
            d = 30.0f;
        }
    }
    return d;
}

static f32 partyHomingOff(PartyEntry* party) {
    f32 off = 5.0f;
    if (party->flags2 & 0x8000) {
        off = 1.0f;
    }
    return off;
}

s32 getFrontFloor(PartyEntry* party, Vec* out, f32 dir) {
    f32 s2, c2;
    HitCheckQuery q2;
    HitCheckQuery q;
    f32 c, s;
    f32 r;
    f32 wallct;
    f32 y;
    f32 rf;
    s32 i;
    HitEntry* hit;

    r = 2.0f * party->unkF4 / 3.0f;
    q.targetDir = (Vec){0.0f, 1.0f, 0.0f};
    wallct = touch_wall_ct;
    sincosf(&s, &c, dir);
    for (i = 0; i < 3; i++) {
        q.targetPos.x = party->position.x + (f32)i * (s * r);
        q.targetPos.z = party->position.z + (f32)i * (c * r);
        q.targetPos.y = party->position.y + party->unkF0;
        q.targetDistance = 100.0f;
        if (hitCheckVecFilter(&q, NULL)) {
            if (pm_sqrtf(q.hitNormal.x * q.hitNormal.x + q.hitNormal.z * q.hitNormal.z) < 45.0f) {
                y = q.hitPos.y;
                goto found;
            }
        }
    }
    y = -2000.0f;
found:
    if (y > -2000.0f) {
        f32 t = party->position.y + wallct;
        f32 u = y - 2.0f;
        if (t < u) {
            q2.targetPos.y = t;
        } else {
            q2.targetPos.y = u;
        }
    } else {
        q2.targetPos.y = party->position.y + wallct;
    }
    r = 0.75f * party->unkF4;
    sincosf(&s2, &c2, dir);
    q2.targetPos.x = s2 * r + party->position.x;
    q2.targetPos.z = c2 * r + party->position.z;
    q2.targetDistance = 300.0f;
    q2.targetDir = (Vec){0.0f, -1.0f, 0.0f};
    hit = hitCheckVecFilter(&q2, NULL);
    if (!hit) {
        out->x = party->position.x;
        out->y = party->position.y;
        out->z = party->position.z;
        return 0;
    }
    rf = q2.hitPos.x >= 0.0f ? 0.5f : -0.5f;
    out->x = (s32)((1000.0f * q2.hitPos.x) + rf) / 1000.0f;
    rf = q2.hitPos.y >= 0.0f ? 0.5f : -0.5f;
    out->y = (s32)((1000.0f * q2.hitPos.y) + rf) / 1000.0f;
    rf = q2.hitPos.z >= 0.0f ? 0.5f : -0.5f;
    out->z = (s32)((1000.0f * q2.hitPos.z) + rf) / 1000.0f;
    return hitGetAttr(hit);
}



















static inline void partyDownDirX(HitCheckQuery* q) {
    q->targetDir = (Vec){0.0f, -1.0f, 0.0f};
}

static f32 partyGetDirGndY(Vec u, f32 md) {
    f32 k = 40.0f;
    HitCheckQuery q2;
    HitEntry* hit;
    f32 s2, c2;
    f32 gy;
    f32 a;
    gy = touch_wall_ct;
    sincosf(&s2, &c2, md);
    q2.targetPos = u;
    q2.targetPos.x = s2 * k + q2.targetPos.x;
    q2.targetPos.z = c2 * k + q2.targetPos.z;
    q2.targetPos.y = q2.targetPos.y + gy;
    q2.targetDistance = 1000.0f;
    q2.targetDir = (Vec){0.0f, -1.0f, 0.0f};
    hit = hitCheckVecFilter(&q2, NULL);
    if (hit) {
        a = q2.hitPos.y >= 0.0f ? 0.5f : -0.5f;
        gy = (s32)((1000.0f * q2.hitPos.y) + a) / 1000.0f;
        hitGetAttr(hit);
    } else {
        gy = -2000.0f;
    }
    return gy;
}

static f32 partyGetGndY(f32 x, f32 y, f32 z) {
    f32 r;
    f32 hx, hy, hz, dist, nx, ny, nz;
    HitEntry* hit;
    y += touch_wall_ct;
    dist = 1000.0f;
    hit = hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, x, y, z, 0.0f, -1.0f, 0.0f);
    if (!hit) {
        r = -2000.0f;
    } else {
        r = (s32)(100.0f * hy + 0.5f) / 100.0f;
        hitGetAttr(hit);
    }
    return r;
}

static u32 partyGetFrontAttr(PartyEntry* party, f32 dir) {
    HitCheckQuery q;
    HitEntry* hit;
    f32 s, c;
    sincosf(&s, &c, dir);
    q.targetDistance = 40.0f;
    q.targetPos.x = 20.0f * s + party->position.x;
    q.targetPos.z = 20.0f * c + party->position.z;
    q.targetPos.y = 20.0f + party->position.y;
    q.targetDir = (Vec){0.0f, -1.0f, 0.0f};
    hit = hitCheckVecFilter(&q, NULL);
    return hit ? hitGetAttr(hit) : 0;
}























































static inline u32 partyGetFrontAttrFar(PartyEntry* party) {
    HitCheckQuery q;
    HitEntry* hit;
    f32 s, c;
    f32 k = 40.0f;
    sincosf(&s, &c, party->unk100);
    q.targetDistance = 100.0f;
    q.targetPos.x = s * k + party->position.x;
    q.targetPos.z = c * k + party->position.z;
    q.targetPos.y = party->position.y + k;
    q.targetDir = (Vec){0.0f, -1.0f, 0.0f};
    hit = hitCheckVecFilter(&q, NULL);
    return hit ? hitGetAttr(hit) : 0;
}

static inline void partyDownDirY(HitCheckQuery* q) {
    q->targetDir = (Vec){0.0f, -1.0f, 0.0f};
}

void party_force_reset_outofscreen(PartyEntry* party) {
    PartyMarioView* m2;
    s32 i;
    s32 margin;
    s32 j;
    PartyMarioView* mario = party->playerPtr;
    f32 speed;
    f32 y;
    f32 up;
    HitEntry* hit;
    f32 dist;
    f32 sx, sy, sz;
    Vec pos;
    Vec start;
    Vec dir;
    Vec hitPos;
    Vec nrm;

    if (marioCtrlOffChk()) {
        return;
    }
    if (marioKeyOffChk()) {
        return;
    }
    marioGetScreenPos(party->position, &sx, &sy, &sz);
    if (distABf(mario->position.x, mario->position.z, party->position.x, party->position.z) < 20.0f &&
        sy > 800.0f)
    {
        partyGetAppearPos3(party, &pos);
        party->position = pos;
        return;
    }
    margin = 100;
    if (-100.0f <= sx && sx <= 700.0f && sy <= 650.0f) {
        return;
    }
    if (!(party->flags & 8)) {
        partyClearFootmark();
    } else {
        partyClearFootmark();
    }
    m2 = party->playerPtr;
    i = 0;
    pos = party->position;
    if (party->unk32 == 0) {
        speed = party->unkF4;
    } else if (party->currentMemberId == 5) {
        speed = 10.0f;
        margin = 70;
    } else {
        speed = 10.0f;
        margin = 50;
    }
    {
        Vec d = {0.0f, -1.0f, 0.0f};
        dir = d;
    }
    while (TRUE) {
        marioGetScreenPos(pos, &sx, &sy, &sz);
        if ((f32)-margin <= sx && sx <= (f32)(margin + 600)) {
            party->position = pos;
            return;
        }
        if (i > 100) {
            return;
        }
        {
            f32 a = angleABf(pos.x, pos.z, m2->position.x, m2->position.z);
            movePos(&pos.x, &pos.z, speed, a);
        }
        up = 200.0f;
        j = 0;
        y = pos.y;
        while (TRUE) {
            dist = 300.0f;
            start = pos;
            start.y += up;
            hit = partyHitCheck(party, &start, &dir, &hitPos, &nrm, &dist);
            if (hit) {
                if (!partyChkHitAttr200(hit)) {
                    y = hitPos.y;
                }
            } else {
                y = m2->position.y;
            }
            start.y = y;
            marioGetScreenPos(start, &sx, &sy, &sz);
            if (sy > 100.0f) {
                dist = 100.0f;
                start = pos;
                if (pos.y + 74.0f > y) {
                    start.y += 74.0f;
                    hit = partyHitCheck(party, &start, &dir, &hitPos, &nrm, &dist);
                    if (hit && !partyChkHitAttr200(hit)) {
                        y = hitPos.y;
                    }
                }
                break;
            }
            if (++j > 4) {
                break;
            }
            up -= 50.0f;
            if (up < 0.0f) {
                up = 0.0f;
            }
        }
        pos.y = y;
        i++;
    }
}























static void partyGorFix(PartyEntry* party) {
    PartyMarioView* m3 = party->playerPtr;
    if (!strcmp(gp->currentMapName, "gor_01") && m3->unk1E4 && party->unk138 &&
        !strcmp(hitGetName(m3->unk1E4), "A_ie_03_yuka_f1_C") && !strcmp(hitGetName(party->unk138), "A_ie_03_yuka_f2_C"))
    {
        Vec fix = {122.0f, 130.0f, -313.0f};
        party->position = fix;
    }
}

static void partyPoseA2(PartyEntry* party) {
    partyChgPose(party, "PCH_A2_1");
    party->unk39 = 1;
    party->unk24 = 30;
}
