#include "mri_puni.h"
#include "party.h"
#include "mario/mario_party.h"
#include "driver/camdrv.h"
#include "memory.h"
#include <string.h>
#include "forcedata.h"
#include "mario/mariost.h"
#include "mario/mario.h"
#include "driver/icondrv.h"

#include "driver/swdrv.h"
#include "evt/evt_seq.h"
#include "evt/evt_npc.h"
#include "pmario_sound.h"
#include "evt/evt_bero.h"
#include "manager/evtmgr_cmd.h"
#include "driver/punidrv.h"
#include "driver/mapdrv.h"
#include <dolphin/gx.h>
extern int rand(void);
void PuniDokuritsu(PuniData* p);
void puniHagurePos(PuniData* p);
void puniMapdata_MRI_06_getidx(f32 x, f32 z, s32* bx, s32* bz);
void puniMapdata_MRI_09_getidx(f32 x, f32 z, s32* bx, s32* bz);
void puniMapdata_MRI_16_getidx(f32 x, f32 z, s32* bx, s32* bz);
void puniMapdata_MRI_09_getpos(s32 bx, s32 bz, f32* x, f32* z);
extern int abs(int);
EffectEntry* effPuniBalloonEntry(s32 type, f32 x, f32 y, f32 z, f32 scale);
void envSetYami(MapTevCallbackInfo* info);


//.data
// clang-format off
extend_data(0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f);
extend_data(1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)
// clang-format on
u16 puni_nom_patern_data[] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,    21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 0,  1,  2,  3,  4,  5,     6,  7,  8,  9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,    31, 32, 33, 34,
    35, 36, 37, 38, 39, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,    16, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 51, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 65535,
};
u16 puni_wlk_patern_data[] = {
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 65535,
};
u16 puni_run_patern_data[] = { 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 65535 };
u16 puni_dmg_patern_data[] = { 72, 65535 };
u16 puni_dwn_patern_data[] = { 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 65535 };
u16 puni_rol_patern_data[] = { 83, 65535 };
u16 puni_hag_patern_data[] = { 0, 65535 };
u16 puni_jpu_patern_data[] = { 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 65534 };
u16 puni_jpd_patern_data[] = { 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 65534 };
u16 puni_wan_patern_data[] = { 104, 105, 106, 107, 108, 109, 110, 111, 65534 };
u16* puni_patern_table[] = { puni_nom_patern_data, puni_wlk_patern_data, puni_run_patern_data, puni_dmg_patern_data,
                             puni_dwn_patern_data, puni_rol_patern_data, puni_hag_patern_data, puni_jpu_patern_data,
                             puni_jpd_patern_data, puni_wan_patern_data };

u16 toge_nom_patern_data[25] = {
    84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 65535,
};
u16 toge_wlk_patern_data[] = { 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 65535 };
u16 toge_run_patern_data[] = { 108, 110, 112, 114, 116, 118, 65535 };
u16 toge_dmg_patern_data[] = { 120, 65535 };
u16 toge_dwn_patern_data[] = { 121, 122, 123, 124, 125, 126, 127, 128, 65535 };
u16 toge_rol_patern_data[] = { 129, 65535 };
u16 toge_hag_patern_data[] = { 0, 65535 };
u16* toge_patern_table[] = { toge_nom_patern_data, toge_wlk_patern_data, toge_run_patern_data, toge_dmg_patern_data,
                             toge_dwn_patern_data, toge_rol_patern_data, toge_hag_patern_data, toge_hag_patern_data,
                             toge_hag_patern_data, toge_hag_patern_data };

//.bss
PuniMap* Punimap;
PuniTexInfo* Punitex;
PuniWork* Punigp;
PuniData* Punidat;


static inline s32 puni_bero_id_filter(s32 id) {
    return bero_id_filter(id);
}
PuniData* puniGetPunidat(void) {
    return Punidat;
}

PuniWork* puniGetPunigp(void) {
    return Punigp;
}

PuniTexInfo* puniGetTex(void) {
    return Punitex;
}

void evt_icon_disp_func(CameraId camId, void* param) {
    PuniData* p;
    s32 i;
    if (Punigp->unk328 == 9 || Punigp->unk328 == 12) {
        for (i = 1; i < 211; i++) {
            p = &Punidat[i];
            if ((p->unk4 & 0x400) && !(p->unk4 & 0x804000)) {
                Vec a;
                Vec b;
                a = p->pos;
                if (p->unk8 & 1) {
                    p->unk121++;
                    if ((s8)p->unk121 > 30) {
                        p->unk121 = 0;
                        p->unk8 &= ~1;
                    }
                }
                if (p->unk8 & 2) {
                    b = a;
                    puniSinCosRet(*(s16*)&p->unk116[0] + gp->retraceCount * 10, &a.z, &a.x, 9.0f, 9.0f);
                    a.y += 13.0f;
                    iconDispGx(&a, 0, 0x14d, 0.6f);
                    puniSinCosRet(*(s16*)&p->unk116[0] + gp->retraceCount * 10 + 180, &b.z, &b.x, 9.0f, 9.0f);
                    b.y += 15.0f;
                    iconDispGx(&b, 0, 0x14d, 0.5f);
                }
            }
        }
    }
}

PuniTexInfo* _make_texbuf(PuniTexInfo* texbuf, s32 count, const char* a3, const char* a4, f32 a5) {
    s32 i;

    for (i = 0; i < count; i++) {
        texbuf->unk0 = _mapAlloc(0xC);
        texbuf->unk4 = _mapAlloc(0xC);
        strcpy(texbuf->unk0, a3);
        strcpy(texbuf->unk4, a4);
        texbuf->unk8 = a5;
        a5 += 1.0f;
        texbuf++;
    }
    return texbuf;
}

USER_FUNC(evt_make_texbuf) {
    PuniTexInfo* texbuf = Punitex;

    texbuf = Punitex;
    texbuf = _make_texbuf(texbuf, 40, "c_puni", "S_1", 0.0f);
    texbuf = _make_texbuf(texbuf, 12, "c_puni", "S_1", 97.0f);
    texbuf = _make_texbuf(texbuf, 20, "c_puni", "W_1", 0.0f);
    texbuf = _make_texbuf(texbuf, 1, "c_puni", "D_1", 0.0f);
    texbuf = _make_texbuf(texbuf, 10, "c_puni", "D_2", 0.0f);
    texbuf = _make_texbuf(texbuf, 1, "c_puni", "F_1", 0.0f);

    if (Punigp->unk328 == 9 || Punigp->unk328 == 12) {
        texbuf = _make_texbuf(texbuf, 24, "c_toge", "S_1", 0.0f);
        texbuf = _make_texbuf(texbuf, 12, "c_toge", "W_1", 0.0f);
        texbuf = _make_texbuf(texbuf, 1, "c_toge", "D_1", 0.0f);
        texbuf = _make_texbuf(texbuf, 8, "c_toge", "D_2", 0.0f);
        texbuf = _make_texbuf(texbuf, 1, "c_toge", "F_1", 0.0f);
    } else {
        texbuf = _make_texbuf(texbuf, 10, "c_puni", "J_1A", 0.0f);
        texbuf = _make_texbuf(texbuf, 10, "c_puni", "J_1B", 0.0f);
        texbuf = _make_texbuf(texbuf, 8, "c_puni", "B_1", 0.0f);
    }
    texbuf->unk0 = 0;
    texbuf->unk4 = 0;
    texbuf->unk8 = 0.0f;
    return EVT_RETURN_DONE;
}

void puniAnimCount(s16* pattern, s16* count, s16* end) {
    s16 n = 0;
    s32 i = 0;

    while (TRUE) {
        if (pattern[i] == -1) {
            *end = pattern[i];
            break;
        }
        if (pattern[i] == -2) {
            *end = pattern[i];
            break;
        }
        n++;
        i++;
    }
    *count = n;
}

void puniAnimationInit(s16** a, s16** b) {
    PuniAnimSet* s0 = &Punigp->anim[0];
    PuniAnimSet* s1 = &Punigp->anim[1];
    s32 j;
    s32 i;
    s16* p;
    s16 v;
    s32 k;

    for (i = 0; i < 10; i++) {
        s0->table[i] = a[i];
        puniAnimCount(a[i], &s0->count[i], &s0->end[i]);
        s1->table[i] = b[i];
        k = 0; j = 0;
        for (p = b[i];; j++, k++) {
            v = p[k];
            if (v == -1) {
                s1->end[i] = v;
                break;
            }
            if (v == -2) {
                s1->end[i] = v;
                break;
            }
        }
        s1->count[i] = j;
    }
}

void puniAnimationInit2(void) {
    char (*n)[16];
    n = (char (*)[16])&Punigp->unkA4[0];
    strcpy(n[0], "S_1"); strcpy(n[1], "W_1"); strcpy(n[2], "R_1"); strcpy(n[3], "D_1"); strcpy(n[4], "D_2");
    strcpy(n[5], "F_1"); strcpy(n[6], "Z_1"); strcpy(n[7], "J_1A"); strcpy(n[8], "J_1B"); strcpy(n[9], "B_1");
    n = (char (*)[16])&Punigp->unkA4[160];
    strcpy(n[0], "S_1"); strcpy(n[1], "W_1"); strcpy(n[2], "R_1"); strcpy(n[3], "D_1"); strcpy(n[4], "D_2");
    strcpy(n[5], "F_1"); strcpy(n[6], "Z_1"); strcpy(n[7], "J_1A"); strcpy(n[8], "J_1B"); strcpy(n[9], "B_1");
    n = (char (*)[16])&Punigp->unkA4[320];
    strcpy(n[0], "S_1"); strcpy(n[1], "W_1"); strcpy(n[2], "R_1"); strcpy(n[3], "D_1"); strcpy(n[4], "D_2");
    strcpy(n[5], "F_1"); strcpy(n[6], "Z_1"); strcpy(n[7], "J_1A"); strcpy(n[8], "J_1B"); strcpy(n[9], "B_1");
    n = (char (*)[16])&Punigp->unkA4[480];
    strcpy(n[0], "S_1"); strcpy(n[1], "W_1"); strcpy(n[2], "R_1"); strcpy(n[3], "D_2"); strcpy(n[4], "D_2");
    strcpy(n[5], "F_1"); strcpy(n[6], "Z_1"); strcpy(n[7], "J_1A"); strcpy(n[8], "J_1B"); strcpy(n[9], "B_1");
}


void animPoseSetLocalTime(s32 poseId, f32 localTimeFrames);

typedef struct PuniAnimTarget {
    u8 unk0[4];
    f32 rotY;     // 0x4
    f32 rotZ;     // 0x8
    f32 rotX;     // 0xC
    u8 unk10[4];
    f32 alpha;    // 0x14
    f32* mtx;     // 0x18
    s32 frame;    // 0x1C
} PuniAnimTarget;

void puniAnimation(PuniData* p, PuniAnimTarget* t) {
    NpcEntry* npc;
    char* names;
    PuniAnimSet* set;
    s32 cnt;
    s16* tbl;
    s32 end;
    s32 show;
    s32 f;
    s32 ofs;

    if (!(p->unk4 & 0x40000)) {
    if (p->unk4 & 0x1E0) {
        if (p->unk4 & 0x20) {
            npc = npcNameToPtr("\203K\203C\203hsp");
            names = (char*)( &Punigp->unkA4[0]) ;
        } else if (p->unk4 & 0x40) {
            npc = npcNameToPtr("\203K\203C\203h\226\205sp");
            names = (char*)( &Punigp->unkA4[160]) ;
        } else if (p->unk4 & 0x80) {
            npc = npcNameToPtr("\202\277\202\321\203g\203Qsp");
            names = (char*)( &Punigp->unkA4[320]) ;
        } else if (p->unk4 & 0x100) {
            npc = npcNameToPtr("\203\211\203C\203o\203\213sp");
            names = (char*)( &Punigp->unkA4[480]) ;
        } else {
            npc = npcNameToPtr("\203\211\203C\203o\203\213sp");
            names = (char*)( &Punigp->unkA4[480]) ;
        }
        ofs = p->unk1A4;
        if (p->unk4 & 0x1000000) {
            p->unk124++;
        } else {
            if (p->unkC != 2) {
                ofs = *(( s16*)( &p->unk116)) ;
            } else {
                ofs /= 8;
                if (p->anim == 0) {
                    ofs += ( *( (s16*)( &p->unk116 ))) * 40;
                }
            }
            p->unk124 = gp->retraceCount + ofs;
        }
        if (!(p->unk4 & 0x20000)) {
            animPoseSetAnim(npc->poseId, names + ( p->anim * 16) , 1);
            animPoseSetLocalTime(npc->poseId, p->unk124);
        }
        show = 0;
        if (p->unk4 & 0x400) { show = 1;
        }
        if (p->unk4 & 0x800) { show = 0;
        }
        if (show) {
            npc->position.x = p->pos.x + ( *( (f32*)( &p->unk144))) ;
            npc->position.y = p->pos.y + p->unk148;
            npc->position.z = p->pos.z + ( *( (f32*)( &p->unk14C))) ;
        } else {
            npc->position.x = 0.0f;
            npc->position.y = -1000.0f;
            npc->position.z = 0.0f;
        }
        t->mtx[3] = 0.0f;
        t->mtx[7] = -1000.0f;
        t->mtx[11] = 0.0f;
        npc->scale.x = p->alpha;
        npc->scale.y = p->alpha;
        npc->direction = p->dir;
        npc->rotation.z = p->spin;
        npc->rotationOffset.y = p->unk188;
    } else {
        set = &Punigp->anim[0];
        if (p->unk4 & 0x8) { set = &Punigp->anim[0];
        }
        if (p->unk4 & 0x10) { set = &Punigp->anim[1];
        }
        tbl = set->table[p->anim];
        cnt = set->count[p->anim];
        end = set->end[p->anim];
        ofs = p->unk1A4;
        if (p->unk4 & 0x1000000) {
            f = p->unk124 % cnt;
            if (( end == ( -2 )) && ( cnt <= p->unk124) ) { f = cnt - 1;
            }
            t->frame = tbl[f];
            p->unk124++;
        } else {
            if (p->unkC != 2) {
                ofs = *(( s16*)( &p->unk116)) ;
            } else {
                ofs /= 8;
                if (p->anim == 0) {
                    ofs += ( *( (s16*)( &p->unk116 ))) * (cnt / 3);
                }
            }
            t->frame = tbl[(( u32)(gp->retraceCount + ofs) ) % ( (u32)set->count [p->anim])];
        }
        show = 0;
        if (p->unk4 & 0x400) { show = 1;
        }
        if (p->unk4 & 0x800) { show = 0;
        }
        if (show) {
            t->mtx[3] = p->pos.x + ( *( (f32*)( &p->unk144))) ;
            t->mtx[7] = p->pos.y + p->unk148;
            t->mtx[11] = p->pos.z + ( *( (f32*)( &p->unk14C))) ;
        } else {
            t->mtx[3] = 0.0f;
            t->mtx[7] = -1000.0f;
            t->mtx[11] = 0.0f;
        }
        t->alpha = p->alpha;
        t->rotY = -p->dir;
        t->rotZ = p->spin;
        t->rotX = p->unk188;
    }
    p->unk11E = p->anim;
    }
}

f32 _chukanhosei(f32 a, f32 b, f32 c) {
    f32 t = (20.0f + b - a) / 20.0f;
    if (t > 0.5f) {
        t = (t - 0.5f) / 0.5f;
    } else {
        t = 0.0f;
    }
    return 10.0f * t + c;
}

BOOL _areacheck(f32 x, f32 z, f32 r, f32 px, f32 pz) {
    BOOL ret = TRUE;
    if (x - r > px) {
        ret = FALSE;
    }
    if (x + r < px) {
        ret = FALSE;
    }
    if (z - r > pz) {
        ret = FALSE;
    }
    if (z + r < pz) {
        ret = FALSE;
    }
    return ret;
}

#define YSTEP(thr, base)                          \
    if (p->pos.z <= (thr)) {                      \
        f32 t = ((thr) - p->pos.z) / 20.0f;       \
        f32 u;                                    \
        if (t > 0.5f) {                           \
            u = (t - 0.5f) / 0.5f;                \
        } else {                                  \
            u = new_var;                             \
        }                                         \
        p->pos.y = 10.0f * u + (base);            \
    }

void _ymode_mri_09(PuniData* p) {
    float new_var;
    new_var = 0.0f;
    if (p->pos.x <= 400.0f && p->pos.x > 200.0f && p->pos.y <= 100.0f && p->pos.y >= new_var) {
        if (p->pos.z <= -55.0f) {
            p->pos.y = 100.0f;
        } else YSTEP(-35.0f, 90.0f)
        else YSTEP(-15.0f, 80.0f)
        else YSTEP(5.0f, 70.0f)
        else YSTEP(25.0f, 60.0f)
        else YSTEP(45.0f, 50.0f)
        else YSTEP(65.0f, 40.0f)
        else YSTEP(85.0f, 30.0f)
        else YSTEP(105.0f, 20.0f)
        else YSTEP(125.0f, 10.0f)
        else YSTEP(145.0f, new_var)
        else {
            p->pos.y = new_var;
        }
    }
    if (p->pos.x >= -170.0f && p->pos.x <= 170.0f && p->pos.z >= -80.0f && p->pos.z <= 140.0f) {
        p->pos.y = 0.0f;
    }
}

#define YSTEPX(thr, base)                             if (p->pos.x <= (thr)) {                              f32 t = ((thr) - p->pos.x) / 20.0f;               f32 u;                                            if (t > 0.5f) {                                       u = (t - 0.5f) / 0.5f;                        } else {                                              u = 0.0f;                                     }                                                 p->pos.y = 10.0f * u + (base);                }
#define YSTEPZ(thr, base)                             if (p->pos.x <= (thr)) {                              f32 t = ((thr) - p->pos.x) / 20.0f;               f32 u;                                            if (t > 0.5f) {                                       u = (t - 0.5f) / 0.5f;                        } else {                                              u = 0.0f;                                     }                                                 { f32 bz = (base); p->pos.y = 10.0f * u + bz; }                }

void _ymode_mri_16_00(PuniData* p) {
    u32 old = p->unk8;
    p->unk8 &= ~8;
    if (p->pos.y <= -200.0f && p->pos.y >= -400.0f) {
        if (p->pos.x <= 40.0f && p->pos.x >= -210.0f && p->pos.z <= -76.46314f && p->pos.z >= -200.0f) {
            if (p->pos.x <= -190.0f) {
                p->pos.y = -320.0f;
            } else YSTEPX(-170.0f, -330.0f)
            else YSTEPX(-150.0f, -340.0f)
            else YSTEPX(-130.0f, -350.0f)
            else YSTEPX(-110.0f, -360.0f)
            else YSTEPX(-90.0f, -370.0f)
            else YSTEPX(-70.0f, -380.0f)
            else YSTEPX(-50.0f, -390.0f)
            else YSTEPX(-30.0f, -400.0f)
            else {
                p->pos.y = -400.0f;
            }
            if (p->pos.y < -320.0f && p->pos.y > -400.0f) {
                p->unk8 |= 8;
            }
        }
        if (p->pos.x <= 40.0f && p->pos.x >= -200.0f && p->pos.z > -76.46314f && p->pos.z < 200.0f) {
            p->pos.y = -400.0f;
        }
    }
    if (!(p->unk8 & 4) && !(p->unk8 & 8) && (old & 8)) {
        if (p->pos.z > -76.46314f && p->unk138.z <= -76.46314f) {
            p->pos = p->unk138;
            p->pos.z -= 0.0002f;
        }
    }
}

void _ymode_mri_16_01(PuniData* p) {
    u32 old = p->unk8;
    p->unk8 &= ~8;
    if (p->pos.y <= 100.0f && p->pos.y >= 0.0f) {
        if (p->pos.x <= 40.0f && p->pos.x >= -200.0f && p->pos.z > -200.0f && p->pos.z < 200.0f) {
            p->pos.y = 0.0f;
        }
        if (p->pos.x <= 40.0f && p->pos.x >= -210.0f && p->pos.z <= 60.00006f && p->pos.z >= -10.00012f) {
            if (p->pos.x <= -190.0f) {
                p->pos.y = 80.0f;
            } else YSTEPX(-170.0f, 70.0f)
            else YSTEPX(-150.0f, 60.0f)
            else YSTEPX(-130.0f, 50.0f)
            else YSTEPX(-110.0f, 40.0f)
            else YSTEPX(-90.0f, 30.0f)
            else YSTEPX(-70.0f, 20.0f)
            else YSTEPX(-50.0f, 10.0f)
            else YSTEPZ(-30.0f, 0.0f)
            else {
                p->pos.y = 0.0f;
            }
            if (p->pos.y < 80.0f && p->pos.y > 0.0f) {
                p->unk8 |= 8;
            }
        }
    }
    if (!(p->unk8 & 4) && !(p->unk8 & 8) && (old & 8)) {
        if (p->pos.z > 60.00006f && p->unk138.z <= 60.00006f) {
            p->pos = p->unk138;
            p->pos.z -= 0.0002f;
        }
        if (p->pos.z < -10.00012f && p->unk138.z >= -10.00012f) {
            p->pos = p->unk138;
            p->pos.z += 0.0002f;
        }
    }
}

#define AC(p, x0, x1, z0, z1) (_aok = 1, _ax = (p)->pos.x, _az = (p)->pos.z, ((x0) > _ax ? _aok = 0 : 0), ((x1) < _ax ? _aok = 0 : 0), ((z0) > _az ? _aok = 0 : 0), ((z1) < _az ? _aok = 0 : 0), _aok)
extern f32 getMri02Yuka(void);
//(x0,x1,z0,z1) (ok = 1, (p->pos.x < (x0) ? ok = 0 : 0), (p->pos.x > (x1) ? ok = 0 : 0), (p->pos.z < (z0) ? ok = 0 : 0), (p->pos.z > (z1) ? ok = 0 : 0), ok)
void puniSetYPosition(PuniData* p) {
    if (p->unk4 & 1) {
        BOOL _aok;
        f32 _ax, _az;
        switch (Punigp->ymode) {
            case 0:
                break;
            case 1:
                p->pos.y = 0.0f;
                if (AC(p, -49.0f, 49.0f, -69.0f, 29.0f)) {
                    p->pos.y = getMri02Yuka();
                }
                break;
            case 2:
                _ymode_mri_09(p);
                if (!swGet(0xB23)) {
                    if (AC(p, -35.0f, 35.0f, 40.0f, 110.0f)) {
                        p->pos.y = 6.0f;
                    }
                }
                break;
            case 3:
                p->pos.y = 0.0f;
                if (!swGet(0xB2A)) {
                    if (AC(p, -80.0f, -10.0f, -99.0f, -29.0f)) {
                        p->pos.y = 6.0f;
                    }
                }
                break;
            case 4:
                _ymode_mri_16_00(p);
                break;
            case 5:
                _ymode_mri_16_01(p);
                break;
            case 6:
                p->pos.y = 0.0f;
                if (!swGet(0xB37)) {
                    if (AC(p, -35.0f, 35.0f, -70.0f, 0.0f)) {
                        p->pos.y = 6.0f;
                    }
                }
                break;
        }
        switch (Punigp->ymode) {
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                if (11.0 < __fabs(p->unk138.y - p->pos.y)) {
                    p->pos.y = p->unk138.y;
                }
                break;
        }
    }
}
u8 puniMapdata_MRI_06[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
};
u8 puniMapdata_MRI_09[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
u8 puniMapdata_MRI_09_01[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x20, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
u8 puniMapdata_MRI_16[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x20, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x20, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x20, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
};


void puniMapdata_MRI_06_getidx(f32 x, f32 z, s32* bx, s32* bz) {
    if (x <= -420.0) {
        *bx = -1;
    } else if (x <= -350.0) {
        *bx = 1;
    } else if (x <= -250.0) {
        *bx = 2;
    } else if (x <= -200.0) {
        *bx = 3;
    } else if (x <= -100.0) {
        *bx = 4;
    } else if (x <= -50.0) {
        *bx = 5;
    } else if (x <= 50.0) {
        *bx = 6;
    } else if (x <= 100.0) {
        *bx = 7;
    } else if (x <= 200.0) {
        *bx = 8;
    } else if (x <= 250.0) {
        *bx = 9;
    } else if (x <= 350.0) {
        *bx = 10;
    } else if (x <= 400.0) {
        *bx = 11;
    } else {
        *bx = -1;
    }
    if (z <= -150.0) {
        *bz = -1;
    } else if (z <= -50.0) {
        *bz = 1;
    } else if (z <= 110.0) {
        *bz = 2;
    } else if (z <= 162.0) {
        *bz = 3;
    } else {
        *bz = -1;
    }
}

void puniMapdata_MRI_06_getpos(s32 bx, s32 bz, f32* x, f32* z) {
    switch (bx) {
        case 1:
            *x = -375.0f;
            break;
        case 2:
            *x = -300.0f;
            break;
        case 3:
            *x = -225.0f;
            break;
        case 4:
            *x = -150.0f;
            break;
        case 5:
            *x = -75.0f;
            break;
        case 6:
            *x = -0.0f;
            break;
        case 7:
            *x = 75.0f;
            break;
        case 8:
            *x = 150.0f;
            break;
        case 9:
            *x = 225.0f;
            break;
        case 10:
            *x = 300.0f;
            break;
        case 11:
            *x = 375.0f;
            break;
        default:
            *x = -1000.0f;
            break;
    }
    switch (bz) {
        case 1:
            *z = -100.0f;
            break;
        case 2:
            *z = 30.0f;
            break;
        case 3:
            *z = 135.0f;
            break;
        default:
            *z = -1000.0f;
            break;
    }
}

void puniMapdata_MRI_09_getidx(f32 x, f32 z, s32* bx, s32* bz) {
    if (x <= -300.0) {
        *bx = -1;
    } else if (x <= -200.0) {
        *bx = 1;
    } else if (x <= -90.0) {
        *bx = 2;
    } else if (x <= 90.0) {
        *bx = 3;
    } else if (x <= 200.0) {
        *bx = 4;
    } else if (x <= 320.0) {
        *bx = 5;
    } else {
        *bx = -1;
    }
    if (z <= -150.0) {
        *bz = -1;
    } else if (z <= -100.0) {
        *bz = 1;
    } else if (z <= 100.0) {
        *bz = 2;
    } else if (z <= 120.0) {
        *bz = 3;
    } else if (z <= 200.0) {
        *bz = 4;
    } else {
        *bz = -1;
    }
}

void puniMapdata_MRI_09_getpos(s32 bx, s32 bz, f32* x, f32* z) {
    switch (bx) {
        case 1:
            *x = -250.0f;
            break;
        case 2:
            *x = -145.0f;
            break;
        case 3:
            *x = 0.0f;
            break;
        case 4:
            *x = 145.0f;
            break;
        case 5:
            *x = 260.0f;
            break;
        default:
            *x = -1000.0f;
            break;
    }
    switch (bz) {
        case 1:
            *z = -120.0f;
            break;
        case 2:
            *z = 0.0f;
            break;
        case 3:
            *z = 105.0f;
            break;
        case 4:
            *z = 160.0f;
            break;
        default:
            *z = -1000.0f;
            break;
    }
}

void puniMapdata_MRI_16_getidx(f32 x, f32 z, s32* bx, s32* bz) {
    if (x <= -350.0) {
        *bx = -1;
    } else if (x <= -200.0) {
        *bx = 1;
    } else if (x <= -150.0) {
        *bx = 2;
    } else if (x <= -100.0) {
        *bx = 3;
    } else if (x <= -50.0) {
        *bx = 4;
    } else if (x <= 0.0) {
        *bx = 5;
    } else if (x <= 50.0) {
        *bx = 6;
    } else if (x <= 100.0) {
        *bx = 7;
    } else if (x <= 150.0) {
        *bx = 8;
    } else if (x <= 350.0) {
        *bx = 9;
    } else {
        *bx = -1;
    }
    if (z <= -150.0) {
        *bz = -1;
    } else if (z <= -80.0) {
        *bz = 1;
    } else if (z <= -10.0) {
        *bz = 2;
    } else if (z <= 60.0) {
        *bz = 3;
    } else if (z <= 90.0) {
        *bz = 4;
    } else if (z <= 120.0) {
        *bz = 5;
    } else if (z <= 150.0) {
        *bz = 6;
    } else if (z <= 180.0) {
        *bz = 7;
    } else {
        *bz = -1;
    }
}

void puniMapdata_MRI_16_getpos(s32 bx, s32 bz, f32* x, f32* z) {
    switch (bx) {
        case 1: *x = -220.0f; break;
        case 2: *x = -175.0f; break;
        case 3: *x = -125.0f; break;
        case 4: *x = -75.0f; break;
        case 5: *x = -25.0f; break;
        case 6: *x = 25.0f; break;
        case 7: *x = 75.0f; break;
        case 8: *x = 125.0f; break;
        case 9: *x = 170.0f; break;
        default: *x = -1000.0f; break;
    }
    switch (bz) {
        case 1: *z = -120.0f; break;
        case 2: *z = -45.0f; break;
        case 3: *z = 25.0f; break;
        case 4: *z = 75.0f; break;
        case 5: *z = 105.0f; break;
        case 6: *z = 135.0f; break;
        case 7: *z = 165.0f; break;
        default: *z = -1000.0f; break;
    }
}

void puniMapPos2Blk(Vec* pos, s32* blk) {
    switch (Punigp->mapKind) {
        case 0:
            break;
        case 1:
            ((void (*)(Vec*, s32*))puniMapdata_MRI_06_getidx)(pos, blk);
            break;
        case 2:
            ((void (*)(Vec*, s32*))puniMapdata_MRI_09_getidx)(pos, blk);
            break;
        case 3:
            ((void (*)(Vec*, s32*))puniMapdata_MRI_16_getidx)(pos, blk);
            break;
    }
}

void puniMapBlk2Pos(s32 bx, s32 bz, f32* x, f32* z) {
    switch (Punigp->mapKind) {
        case 0:
            break;
        case 1:
            puniMapdata_MRI_06_getpos(bx, bz, x, z);
            break;
        case 2:
            switch (bx) {
                case 1: *x = -250.0f; break;
                case 2: *x = -145.0f; break;
                case 3: *x = 0.0f; break;
                case 4: *x = 145.0f; break;
                case 5: *x = 260.0f; break;
                default: *x = -1000.0f; break;
            }
            switch (bz) {
                case 1: *z = -120.0f; break;
                case 2: *z = 0.0f; break;
                case 3: *z = 105.0f; break;
                case 4: *z = 160.0f; break;
                default: *z = -1000.0f; break;
            }
            break;
        case 3:
            puniMapdata_MRI_16_getpos(bx, bz, x, z);
            break;
    }
}

u16 puniMapGetInfo(s32 x, s32 y) {
    u16 v = Punimap->data[x + y * Punimap->width];
    if (!(v & 0xFF)) {
        v |= 0xFFFF;
    }
    return v;
}

void puniMapInit(void) {
    Punimap = _mapAlloc(sizeof(PuniMap));
    memset(Punimap, 0, sizeof(PuniMap));
}

void puniMapDataSet(s32 w, s32 h, void* src, f32 x, f32 z) {
    Punimap->x = x;
    Punimap->z = z;
    if (src == NULL) {
        Punimap->enabled = 0;
        return;
    }
    Punimap->width = w;
    Punimap->height = h;
    Punimap->enabled = 1;
    Punimap->src = src;
    Punimap->unk1C = 0;
    Punimap->size = h * (w * 4);
    Punimap->data = _mapAlloc(Punimap->size);
    memcpy(Punimap->src, src, Punimap->size);
}

void puniMapFindbuff(Vec *pos)
{
  f32 px;
  f32 pz;
  s32 bx;
  s32 bz;
  s32 changed;
  s32 i;
  s32 n;
  s32 idx;
  if (Punimap->enabled)
  {
    if ((Punimap->x < pos->y) || (Punimap->z > pos->y))
    {
      Punimap->unk1C = 0;
    }
    else
    {
      memcpy(Punimap->data, Punimap->src, Punimap->size);
      Punimap->unk1C = 1;
      px = pos->x;
      pz = pos->z;
      switch (Punigp->mapKind)
      {
        case 0:
          break;

        case 1:
          puniMapdata_MRI_06_getidx(px, pz, &bx, &bz);
          break;

        case 2:
          puniMapdata_MRI_09_getidx(px, pz, &bx, &bz);
          break;

        case 3:
          puniMapdata_MRI_16_getidx(px, pz, &bx, &bz);
          break;

      }

      idx = bx + (Punimap->width * bz);
      if (idx < 0)
      {
        Punimap->unk1C = 0;
      }
      if (Punimap->unk1C)
      {
        Punimap->data[idx] |= 1;
        n = 1;
        while (1)
        {
          u16 l = n;
          changed = 0;
          n = Punimap->width * Punimap->height;
          for (i = 0; i < n; i++)
          {
            u16 *d = Punimap->data;
            u16 v = d[i];
            if ((v != 0xFFFF) && (l == (v & 0xFFF)))
            {
              u16 h = v & 0xF000;
              u16 *k0 = &d[i - Punimap->width];
              u16 *k1 = &d[i + Punimap->width];
              u16 *k2 = &d[i - 1];
              u16 *k3 = &d[i + 1];
              if ((((*k0) & 0xFF) == 0) && (((*k0) & 0xFF) < l))
              {
                if (__abs(((*k0) & 0xF000) - h) <= 0x1000)
                {
                  *k0 |= (u16) (l + 1);
                  changed = 1;
                }
              }
              if ((((*k1) & 0xFF) == 0) && (((*k1) & 0xFF) < l))
              {
                if (__abs(((*k1) & 0xF000) - h) <= 0x1000)
                {
                  *k1 |= (u16) (l + 1);
                  changed = 1;
                }
              }
              if ((((*k2) & 0xFF) == 0) && (((*k2) & 0xFF) < l))
              {
                if (__abs(((*k2) & 0xF000) - h) <= 0x1000)
                {
                  *k2 |= (u16) (l + 1);
                  changed = 1;
                }
              }
              if ((((*k3) & 0xFF) == 0) && (((*k3) & 0xFF) < l))
              {
                if (__abs(((*k3) & 0xF000) - h) <= 0x1000)
                {
                  *k3 |= (u16) (l + 1);
                  changed = 1;
                }
              }
            }
          }

          if (!changed)
          {
            break;
          }
          n++;
        }

      }
    }
  }
}

void puniTargetSearch(PuniData* p) {
    s32 i;
    PuniTarget* t = Punigp->targets;
    f32 d;

    p->unk1B4 = -1;
    p->unk1B8 = 1000000.0f;
    p->unk1BC = -1;
    p->unk1C0 = 1000000.0f;
    for (i = 0; i < 10; i++, t++) {
        if (!(t->flags & 1)) {
            continue;
        }
        if (32.0 < __fabs(p->pos.y - t->pos.y)) {
            continue;
        }
        if (t->flags & 4) {
            d = PSVECSquareDistance(&p->pos, &t->pos);
            if (p->unk1C0 > d) {
                p->unk1BC = i;
                p->unk1C0 = d;
            }
        }
        if (t->flags & 2) {
            d = PSVECSquareDistance(&p->pos, &t->pos);
            if (p->unk1B8 > d) {
                p->unk1B4 = i;
                p->unk1B8 = d;
            }
        }
    }
}

void puniswGetindex(s32 base, s32 idx, s32* val, Vec* pos) {
    u8 b0, b1, b2, b3, b4, b5, b6;
    s32 off;
    s32 x;
    s32 y;
    s32 z;

    off = base + idx * 7;
    b0 = _swByteGet(off + 150000000);
    b1 = _swByteGet(off + 150000001);
    b2 = _swByteGet(off + 150000002);
    b3 = _swByteGet(off + 150000003);
    b4 = _swByteGet(off + 150000004);
    b5 = _swByteGet(off + 150000005);
    b6 = _swByteGet(off + 150000006);

    *val = b0;
    x = b1 + (b2 << 8);
    if (x & 0x8000) {
        x |= 0xFFFF0000;
    }
    y = b3 + (b4 << 8);
    if (y & 0x8000) {
        y |= 0xFFFF0000;
    }
    z = b5 + (b6 << 8);
    if (z & 0x8000) {
        z |= 0xFFFF0000;
    }
    pos->x = x;
    pos->y = y;
    pos->z = z;
}

#pragma push
#pragma opt_dead_assignments off
void puniswSetindex(s32 base, s32 idx, s32* val, Vec* pos) {
    u8 yh, xh;
    s32 off;
    s32 x;
    s32 y;
    s32 z;
    off = base + idx * 7;
    x = pos->x;
    y = pos->y;
    z = pos->z;
    xh = ((x & 0xFF00) >> 8) & 0xFFu;
    yh = (y & 0xFF00) >> 8;
    _swByteSet(off + 150000000, *val);
    _swByteSet(off + 150000001, x & 0xFF);
    _swByteSet(off + 150000002, xh);
    _swByteSet(off + 150000003, y & 0xFF);
    _swByteSet(off + 150000004, yh);
    _swByteSet(off + 150000005, z & 0xFF);
    _swByteSet(off + 150000006, (z & 0xFF00) >> 8);
}
#pragma pop

void puniswTrapGetindex(s32 idx, s32* out) {
    if (idx >= 103) {
        *out = 0;
    } else {
        *out = (swByteGet(idx / 8 + 0x28C) >> (idx % 8)) & 1;
    }
}

void puniswTrapSetindex(s32 idx, s32* val) {
    if (idx < 103) {
        s32 byte = idx / 8;
        s32 bit = idx % 8;
        s32 v = swByteGet(byte + 0x28C);
        v &= (1 << bit) ^ 0xFF;
        v |= *val << bit;
        swByteSet(byte + 0x28C, (u8)v);
    }
}

static f32 randf(f32 min, f32 max) {
    f32 r = (f32)rand() / 32767.0f;
    return r * (max - min) + min;
}

s32 randi(s32 min, s32 max) {
    s32 r = rand() % (max - min + 1);
    return r + min;
}

void qrndseed(s32 seed) {
    Punigp->seed0 = seed + 0x3A3B01F9;
    Punigp->seed1 = 0x21708331;
}

s32 qrnd(s32 n) {
    s32 v;
    Punigp->seed0 += Punigp->seed1 + 0x1733AF91;
    Punigp->seed1 += Punigp->seed0 + 0x21273F9C;
    v = Punigp->seed1 < 0 ? -Punigp->seed1 : Punigp->seed1;
    return v % n;
}

void puniDeleteEffect(PuniData* p) {
    if (p->effect) {
        effDelete(p->effect);
        p->effect = NULL;
    }
}

void _daime_change(PuniData* p) {
    if (p->child1) {
        p->child1->unk11A--;
        _daime_change(p->child1);
    }
    if (p->child2) {
        p->child2->unk11A--;
        _daime_change(p->child2);
    }
}

void PuniDokuritsu(PuniData* p) {
    PuniData* c1;
    PuniData* c2;
    PuniData* par;
    p->unk118 = -1;
    p->unk4 &= ~0x80000;
    p->unk11A = 0;
    c1 = p->child1;
    par = p->parent;
    c2 = p->child2;
    p->parent = NULL;
    p->child1 = NULL;
    p->child2 = NULL;
    if (c1) {
        c1->unk11A--;
        _daime_change(c1);
        c1->parent = par;
        if (c1->parent) {
            if (par->child1 == p) {
                par->child1 = c1;
            } else if (par->child2 == p) {
                par->child2 = c1;
            }
        }
        c1->child2 = c2;
        if (c1->child2) {
            c1->child2->parent = c1;
        }
    } else if (c2) {
        c2->unk11A--;
        _daime_change(c2);
        c2->parent = par;
        if (c2->parent) {
            if (par->child1 == p) {
                par->child1 = c2;
            } else if (par->child2 == p) {
                par->child2 = c2;
            }
        }
        c2->child1 = c1;
        if (c2->child1) {
            c2->child1->parent = c2;
        }
    } else if (par) {
        if (par->child1 == p) {
            par->child1 = NULL;
        } else if (par->child2 == p) {
            par->child2 = NULL;
        }
    }
}

PuniData* PuniSakanobori(PuniData* p) {
    while (p->parent != NULL) {
        p = p->parent;
    }
    return p;
}

void PuniChildHagure(PuniData* p) {
    PuniData* e1 = p->child1;
    PuniData* c1;
    if (e1 != NULL) {
        PuniData* c;
        e1->unkC = 16;
        p->unk114 = 0;
        c = e1->child1;
        if (c != NULL) {
            PuniData* d;
            c->unkC = 16;
            e1->unk114 = 0;
            d = c->child1;
            if (d != NULL) {
                d->unkC = 16;
                c->unk114 = 0;
                PuniChildHagure(d);
                PuniDokuritsu(d);
            }
            d = c->child2;
            if (d != NULL) {
                d->unkC = 16;
                c->unk114 = 0;
                PuniChildHagure(d);
                PuniDokuritsu(d);
            }
            PuniDokuritsu(c);
        }
        c = e1->child2;
        if (c != NULL) {
            PuniData* d;
            c->unkC = 16;
            e1->unk114 = 0;
            d = c->child1;
            if (d != NULL) {
                d->unkC = 16;
                c->unk114 = 0;
                PuniChildHagure(d);
                PuniDokuritsu(d);
            }
            d = c->child2;
            if (d != NULL) {
                d->unkC = 16;
                c->unk114 = 0;
                PuniChildHagure(d);
                PuniDokuritsu(d);
            }
            PuniDokuritsu(c);
        }
        PuniDokuritsu(e1);
    }
    c1 = p->child2;
    if (c1 != NULL) {
        PuniData* c;
        c1->unkC = 16;
        p->unk114 = 0;
        c = c1->child1;
        if (c != NULL) {
            PuniData* d;
            c->unkC = 16;
            c1->unk114 = 0;
            d = c->child1;
            if (d != NULL) {
                d->unkC = 16;
                c->unk114 = 0;
                PuniChildHagure(d);
                PuniDokuritsu(d);
            }
            d = c->child2;
            if (d != NULL) {
                d->unkC = 16;
                c->unk114 = 0;
                PuniChildHagure(d);
                PuniDokuritsu(d);
            }
            PuniDokuritsu(c);
        }
        c = c1->child2;
        if (c != NULL) {
            PuniData* d;
            c->unkC = 16;
            c1->unk114 = 0;
            d = c->child1;
            if (d != NULL) {
                d->unkC = 16;
                c->unk114 = 0;
                PuniChildHagure(d);
                PuniDokuritsu(d);
            }
            d = c->child2;
            if (d != NULL) {
                d->unkC = 16;
                c->unk114 = 0;
                PuniChildHagure(d);
                PuniDokuritsu(d);
            }
            PuniDokuritsu(c);
        }
        PuniDokuritsu(c1);
    }
}

BOOL searchParent(PuniData* p) {
    s32 level = 10000;
    PuniData* found;
    PuniData* q = Punidat;
    s32 i;

    for (i = 0; i < PUNI_DATA_MAX; i++, q++) {
        if ((q->unk4 & 0x80000) && q->child2 == NULL && (q->child1 == NULL || q->unk11A < 2) && q != p &&
            (s16)level > q->unk11A) {
            found = q;
            level = q->unk11A + 1;
        }
    }
    if ((s16)level == 10000) {
        return FALSE;
    }
    p->parent = found;
    p->unk11A = level;
    p->unk118 = found->unk118;
    if (found->child1 == NULL) {
        found->child1 = p;
    } else {
        found->child2 = p;
    }
    return TRUE;
}

BOOL searchMario(PuniData* p) {
    return p->unk1A4 < 150.0f;
}
static const Vec puni_fall_dir = {0.0f, -1.0f, 0.0f};

s32 puniPositionCheck(PuniData* p) {
    f32* r = (f32*)Punigp->unk350;
    s32 in = 0;
    Vec pos;
    Vec dir;
    Vec hit;
    Vec nrm;
    f32 dist;
    f32 hi, lo;
    hi = r[0];
    lo = r[1];
    if (-1000.0f != hi && p->pos.y < hi && p->pos.y > lo) {
        in = 1;
    }
    hi = r[2];
    lo = r[3];
    if (-1000.0f != hi && p->pos.y < hi && p->pos.y > lo) {
        in = 1;
    }
    if (in == 0) {
        return 1;
    }
    pos = p->pos;
    dir = puni_fall_dir;
    pos.y += 1.0f;
    dist = 3.0f;
    if (hitCheckFilter(NULL, &hit.x, &hit.y, &hit.z, &dist, &nrm.x, &nrm.y, &nrm.z, pos.x, pos.y, pos.z, dir.x, dir.y, dir.z) == NULL) {
        return 2;
    }
    return 0;
}

void puniPositionRecover(PuniData* p) {
    p->pos = Punigp->unk344;
    p->pos.x += randf(0.0f, 2.0f) - 1.0f;
    p->pos.z += randf(0.0f, 2.0f) - 1.0f;
    p->unk138 = p->pos;
}

#define PF(p, o) (*(void**)((u8*)(p) + (o)))
#define PV(p, o) ((Vec*)((u8*)(p) + (o)))

int puniMoveAtari(PuniData* p, Vec* start, Vec* dir, int reflect, f32 dist) {
    Vec hit;
    Vec n;
    Vec diff;
    Vec old;
    Vec refl;
    f32 hx, hy, hz;
    f32 nx, ny, nz;
    f32 d;
    int ret = 1;
    PF(p, 0x214) = NULL;
    PF(p, 0x210) = NULL;
    d = dist;
    if (p->unk4 & 0x1000) {
        PF(p, 0x210) = hitCheckFilter(NULL, &hit.x, &hit.y, &hit.z, &d, &n.x, &n.y, &n.z, start->x, start->y, start->z, dir->x, dir->y, dir->z);
    }
    if (PF(p, 0x210) == NULL) {
        PSVECAdd(&p->pos, &p->vel, &p->pos);
    } else {
        old = p->pos;
        *PV(p, 0x560) = n;
        PSVECAdd(&p->pos, &p->vel, &p->pos);
        if (p->vel.y == 0.0f) {
            n.y = 0.0f;
        }
        PSVECNormalize(&n, &n);
        if (reflect) {
            C_VECReflect(dir, &n, &refl);
            PSVECScale(&refl, &p->vel, dist);
            ret = 0;
        }
        { f32 dp = PSVECDotProduct(dir, &n); f32 t = -(dist - d); PSVECScale(&n, &n, dp * t + 0.1f); }
        PSVECAdd(&p->pos, &n, &p->pos);
        PSVECSubtract(&p->pos, &old, &diff);
        d = PSVECMag(&diff);
        if (d > 0.0f) {
            PSVECNormalize(&diff, &diff);
            *(s32*)&start->y = *(s32*)&old.y;
            { f32 t = start->y + 1.0f;
            *(s32*)&start->x = *(s32*)&old.x;
            start->y = t; }
            *(s32*)&start->z = *(s32*)&old.z;
            PF(p, 0x214) = hitCheckFilter(NULL, &hx, &hy, &hz, &d, &nx, &ny, &nz, old.x, 1.0f + old.y, old.z, diff.x, diff.y, diff.z);
            if (PF(p, 0x214) != NULL) {
                p->pos = old;
            }
        }
    }
    return ret;
}

void unk_mri_000095f0(PuniData *p, Vec *start, Vec *dir, f32 dist)
{
  Vec pa;
  Vec hit;
  Vec diff;
  Vec old;
  f32 new_var;
  f32 b1;
  f32 b2;
  f32 b3;
  f32 c1;
  f32 c2;
  f32 c3;
  f32 d;
  f32 sx;
  f32 sy;
  f32 sz;
  *((void **) (((u8 *) p) + 0x214)) = (void *) 0;
  *((void **) (((u8 *) p) + 0x210)) = (void *) 0;
  sz = start->z;
  sx = start->x;
  new_var = dir->y;
  sy = start->y;
  d = dist;
  *((void **) (((u8 *) p) + 0x210)) = hitCheckFilter((void *) 0, &pa.x, &pa.y, &pa.z, &d, &hit.x, &hit.y, &hit.z, sx, sy, sz, dir->x, new_var, dir->z);
  if ((*((void **) (((u8 *) p) + 0x210))) == ((void *) 0))
  {
    PSVECAdd(&p->pos, &p->vel, &p->pos);
  }
  else
    if (Punigp->unk328 != 18)
  {
    old = p->pos;
    *((Vec *) (((u8 *) p) + 0x560)) = hit;
    PSVECAdd(&p->pos, &p->vel, &p->pos);
    if (p->vel.y == 0.0f)
    {
      hit.y = 0.0f;
    }
    PSVECNormalize(&hit, &hit);
    {
      f32 dp = PSVECDotProduct(dir, &hit);
      PSVECScale(&hit, &hit, (dp * (-(dist - d))) + 0.1f);
    }
    PSVECAdd(&p->pos, &hit, &p->pos);
    PSVECSubtract(&p->pos, &old, &diff);
    d = PSVECMag(&diff);
    if (d > 0.0f)
    {
      PSVECNormalize(&diff, &diff);
      *((void **) (((u8 *) p) + 0x214)) = hitCheckFilter((void *) 0, &b1, &b2, &b3, &d, &c1, &c2, &c3, old.x, old.y + 1.0f, old.z, diff.x, diff.y, diff.z);
      if ((*((void **) (((u8 *) p) + 0x214))) != ((void *) 0))
      {
        p->pos = old;
      }
    }
  }
}

void unk_mri_0000980c(PuniData* p, Vec* start, Vec* dir, f32 dist) {
    Vec pa;
    Vec hit;
    Vec diff;
    Vec refl;
    Vec old;
    f32 b1, b2, b3;
    f32 c1, c2, c3;
    f32 d;
    PF(p, 0x214) = NULL;
    PF(p, 0x210) = NULL;
    d = dist;
    if (p->unk4 & 0x1000) {
        PF(p, 0x210) = hitCheckFilter(NULL, &pa.x, &pa.y, &pa.z, &d, &hit.x, &hit.y, &hit.z, start->x, start->y, start->z, dir->x, dir->y, dir->z);
    }
    if (PF(p, 0x210) == NULL) {
        PSVECAdd(&p->pos, &p->vel, &p->pos);
    } else {
        old = p->pos;
        *PV(p, 0x560) = hit;
        PSVECAdd(&p->pos, &p->vel, &p->pos);
        if (p->vel.y == 0.0f) {
            hit.y = 0.0f;
        }
        PSVECNormalize(&hit, &hit);
        C_VECReflect(dir, &hit, &refl);
        PSVECScale(&refl, &p->vel, dist);
        { f32 dp = PSVECDotProduct(dir, &hit); PSVECScale(&hit, &hit, dp * -(dist - d) + 0.1f); }
        PSVECAdd(&p->pos, &hit, &p->pos);
        PSVECSubtract(&p->pos, &old, &diff);
        d = PSVECMag(&diff);
        if (d > 0.0f) {
            PSVECNormalize(&diff, &diff);
            *(s32*)&start->y = *(s32*)&old.y;
            { f32 t = start->y + 1.0f;
            *(s32*)&start->x = *(s32*)&old.x;
            start->y = t; }
            *(s32*)&start->z = *(s32*)&old.z;
            PF(p, 0x214) = hitCheckFilter(NULL, &b1, &b2, &b3, &d, &c1, &c2, &c3, old.x, old.y + 1.0f, old.z, diff.x, diff.y, diff.z);
            if (PF(p, 0x214) != NULL) {
                p->pos = old;
            }
        }
    }
}

void puniMoveDecel(PuniData* p) {
    Vec start;
    Vec dir;
    Vec s2;
    Vec d2;
    Vec old;
    Vec refl;
    Vec diff;
    Vec hit;
    Vec pa;
    f32 d;
    f32 c3, c2, c1;
    f32 b3, b2, b1;
    f32 dist = PSVECMag(&p->vel);
    if (!dist) {
        return;
    }
    if (dist > 1.0f) {
        if (p->vel.x < 0.0f) {
            p->dir = 270.0f;
        } else {
            p->dir = 90.0f;
        }
    }
    if (dist > p->unk174) {
        PSVECScale(&p->vel, &p->vel, p->unk174 / dist);
        dist = p->unk174;
    }
    start = p->pos;
    start.y += 1.0f;
    PSVECNormalize(&p->vel, &dir);
    s2 = start;
    d2 = dir;
    PF(p, 0x214) = NULL;
    PF(p, 0x210) = NULL;
    d = dist;
    if (p->unk4 & 0x1000) {
        PF(p, 0x210) = hitCheckFilter(NULL, &pa.x, &pa.y, &pa.z, &d, &hit.x, &hit.y, &hit.z, s2.x, s2.y, s2.z, d2.x, d2.y, d2.z);
    }
    if (PF(p, 0x210) == NULL) {
        PSVECAdd(&p->pos, &p->vel, &p->pos);
    } else {
        old = p->pos;
        *PV(p, 0x560) = hit;
        PSVECAdd(&p->pos, &p->vel, &p->pos);
        if (p->vel.y == 0.0f) {
            hit.y = 0.0f;
        }
        PSVECNormalize(&hit, &hit);
        C_VECReflect(&d2, &hit, &refl);
        PSVECScale(&refl, &p->vel, dist);
        { f32 dot = PSVECDotProduct(&d2, &hit); PSVECScale(&hit, &hit, -(dist - d) * dot + 0.1f); }
        PSVECAdd(&p->pos, &hit, &p->pos);
        PSVECSubtract(&p->pos, &old, &diff);
        d = PSVECMag(&diff);
        if (d > 0.0f) {
            PSVECNormalize(&diff, &diff);
            PF(p, 0x214) = hitCheckFilter(NULL, &b1, &b2, &b3, &d, &c1, &c2, &c3, old.x, old.y + 1.0f, old.z, diff.x, diff.y, diff.z);
            if (PF(p, 0x214) != NULL) {
                p->pos = old;
            }
        }
    }
    if (PF(p, 0x210) != NULL || PF(p, 0x214) != NULL) {
        p->unk4 |= 0x400000;
    } else {
        PSVECScale(&p->vel, &p->vel, 0.6f);
        p->unk4 &= ~0x400000;
    }
}

void puniMoveLimitedNormal(PuniData* p) {
    Vec start;
    Vec dir;
    Vec s2;
    Vec d2;
    Vec old;
    Vec diff;
    Vec hit;
    Vec pa;
    f32 d;
    f32 c3, c2, c1;
    f32 b3, b2, b1;
    f32 dist = PSVECMag(&p->vel);
    if (!dist) {
        return;
    }
    if (dist > p->unk174) {
        PSVECScale(&p->vel, &p->vel, p->unk174 / dist);
        dist = p->unk174;
    }
    start = p->pos;
    start.y += 1.0f;
    PSVECNormalize(&p->vel, &dir);
    d2 = dir;
    s2 = start;
    PF(p, 0x214) = NULL;
    PF(p, 0x210) = NULL;
    d = dist;
    PF(p, 0x210) = hitCheckFilter(NULL, &pa.x, &pa.y, &pa.z, &d, &hit.x, &hit.y, &hit.z, s2.x, s2.y, s2.z, d2.x, d2.y, d2.z);
    if (PF(p, 0x210) == NULL) {
        PSVECAdd(&p->pos, &p->vel, &p->pos);
    } else if (Punigp->unk328 != 0x12) {
        old = p->pos;
        *PV(p, 0x560) = hit;
        PSVECAdd(&p->pos, &p->vel, &p->pos);
        if (p->vel.y == 0.0f) {
            hit.y = 0.0f;
        }
        PSVECNormalize(&hit, &hit);
        { f32 dp = PSVECDotProduct(&d2, &hit); PSVECScale(&hit, &hit, dp * -(dist - d) + 0.1f); }
        PSVECAdd(&p->pos, &hit, &p->pos);
        PSVECSubtract(&p->pos, &old, &diff);
        d = PSVECMag(&diff);
        if (d > 0.0f) {
            PSVECNormalize(&diff, &diff);
            PF(p, 0x214) = hitCheckFilter(NULL, &b1, &b2, &b3, &d, &c1, &c2, &c3, old.x, old.y + 1.0f, old.z, diff.x, diff.y, diff.z);
            if (PF(p, 0x214) != NULL) {
                p->pos = old;
            }
        }
    }
    if (PF(p, 0x210) != NULL || PF(p, 0x214) != NULL) {
        p->unk4 |= 0x400000;
    } else {
        p->unk4 &= ~0x400000;
    }
}

void puniMoveNormal(PuniData* p) {
    Vec start;
    Vec dir;
    Vec s2;
    Vec d2;
    Vec old;
    Vec refl;
    Vec diff;
    Vec hit;
    Vec pa;
    f32 d;
    f32 c3, c2, c1;
    f32 b3, b2, b1;
    f32 dist = PSVECMag(&p->vel);
    if (!dist) {
        return;
    }
    start = p->pos;
    start.y += 1.0f;
    PSVECNormalize(&p->vel, &dir);
    s2 = start;
    d2 = dir;
    PF(p, 0x214) = NULL;
    PF(p, 0x210) = NULL;
    d = dist;
    if (p->unk4 & 0x1000) {
        PF(p, 0x210) = hitCheckFilter(NULL, &pa.x, &pa.y, &pa.z, &d, &hit.x, &hit.y, &hit.z, s2.x, s2.y, s2.z, d2.x, d2.y, d2.z);
    }
    if (PF(p, 0x210) == NULL) {
        PSVECAdd(&p->pos, &p->vel, &p->pos);
    } else {
        old = p->pos;
        *PV(p, 0x560) = hit;
        PSVECAdd(&p->pos, &p->vel, &p->pos);
        if (p->vel.y == 0.0f) {
            hit.y = 0.0f;
        }
        PSVECNormalize(&hit, &hit);
        C_VECReflect(&d2, &hit, &refl);
        PSVECScale(&refl, &p->vel, dist);
        { f32 dp = PSVECDotProduct(&d2, &hit); PSVECScale(&hit, &hit, dp * -(dist - d) + 0.1f); }
        PSVECAdd(&p->pos, &hit, &p->pos);
        PSVECSubtract(&p->pos, &old, &diff);
        d = PSVECMag(&diff);
        if (d > 0.0f) {
            PSVECNormalize(&diff, &diff);
            PF(p, 0x214) = hitCheckFilter(NULL, &b1, &b2, &b3, &d, &c1, &c2, &c3, old.x, old.y + 1.0f, old.z, diff.x, diff.y, diff.z);
            if (PF(p, 0x214) != NULL) {
                p->pos = old;
            }
        }
    }
    if (PF(p, 0x210) != NULL || PF(p, 0x214) != NULL) {
        p->unk4 |= 0x400000;
    } else {
        p->unk4 &= ~0x400000;
    }
}

void puniMoveSimple(PuniData* p) {
    if (PSVECMag(&p->vel)) {
        PSVECAdd(&p->pos, &p->vel, &p->pos);
    }
}

void puniMoveCustom(PuniData* p) {
    if (p->moveFunc) {
        p->moveFunc(p);
    }
}


void puniParaAttack(PuniData* a, PuniData* b) {
    s16 x, y, r;
    s32 c;
    if (b->unk586 == 0 && b->unkC == 9) {
        return;
    }
    if (b->unkC != 8) {
        x = a->unk588;
        y = a->unk588 + b->unk588;
    } else {
        x = a->unk58A;
        y = a->unk58A + (100 - b->unk58A);
    }
    r = qrnd(y);
    c = r < x;
    if (b->unk4 & 0x8000000) {
        c = 1;
    }
    if (c) {
        if (a->unk586 == 0) {
            b->damage = a->unk57E - b->unk580;
            {
                Vec paraVecA = { 0.0f, 0.0f, 0.0f };
                b->vel = paraVecA;
            }
            b->unk1DC = (s32)a;
            b->unk4 |= 0x2000;
            b->unk4 &= ~0x8000000;
            b->unk8 &= ~2;
            b->unkC = 9;
            b->unk114 = 0;
            b->unk4 &= ~0x2000000;
        }
    } else if (b->unk586 == 0) {
        a->damage = b->unk57E - a->unk580;
        {
            Vec paraVecB = { 0.0f, 0.0f, 0.0f };
            a->vel = paraVecB;
        }
        a->unk1DC = (s32)b;
        a->unk4 |= 0x2000;
        a->unk4 &= ~0x8000000;
        a->unk8 &= ~2;
        a->unkC = 9;
        a->unk114 = 0;
        a->unk4 &= ~0x2000000;
    }
    a->unk586 = 1;
    b->unk586 = 1;
}

void puniParaDamage(PuniData* p) {
    p->hp -= p->damage;
    if (p->hp <= 0) {
        p->hp = 0;
        p->unk4 |= 0x4000;
        p->unk8 |= 0x10;
    }
}

void puniParaRecover(PuniData* p) {
    static PuniData* debug;
    s16 old;

    debug = p;
    if (p->unk1AC > 0.001f || p->unkC == 13 || p->unkC == 10) {
        p->regenTimer = p->regenInterval;
        return;
    }
    p->regenTimer--;
    if (p->regenTimer == 0) {
        if (p->hp < p->maxHp) {
            old = p->hp;
            p->hp = p->hp + p->regen;
            if (p->hp > p->maxHp) {
                p->hp = p->maxHp;
            }
            if (p->hp > old) {
                p->unk8 |= 1;
                p->unk121 = 0;
            }
        }
        p->regenTimer = p->regenInterval;
    }
}

BOOL puniParaDangerCheck(PuniData* p) {
    return p->hp * 100 / p->maxHp <= 30;
}

BOOL puniParaSelfCheck(PuniData* p) {
    return p->hp * 100 / p->maxHp >= 80;
}

BOOL puniParaRecoverCheck(PuniData* p) {
    return p->hp >= p->maxHp;
}

PuniData* puniSearchRandom(s32 start, s32 count) {
    s32 idx = qrnd(count - 1) + start;
    s32 i = 0;
    PuniData* p;

    while (TRUE) {
        if (i == count) {
            return NULL;
        }
        if (idx == start + count) {
            idx = start;
        }
        p = &Punidat[idx];
        if ((p->unk4 & 0x400) && !(p->unk4 & 0x4000)) {
            break;
        }
        idx++;
        i++;
    }
    return p;
}

PuniData* puniSearchRandom_Mikata(void) {
    return puniSearchRandom(1, 102);
}

PuniData* puniSearchRandom_Teki(void) {
    return puniSearchRandom(110, 100);
}

PuniData* searchBattle(PuniData* self, s32 start, s32 count, f32 maxDist) {
    s32 i;
    PuniData* q;
    f32 minDist = 1000000.0f;
    PuniData* found = NULL;

    q = &Punidat[start];
    for (i = start; i < start + count; i++, q++) {
        if ((q->unk4 & 0x400) && !(q->unk4 & 0x4000) && q->unkC != 9 && q->unkC != 13) {
            f32 d = PSVECSquareDistance(&self->pos, &q->pos);
            if (d < minDist) {
                minDist = d;
                found = q;
            }
        }
    }
    if (minDist < maxDist) {
        return found;
    }
    return NULL;
}

PuniData* searchMikata(PuniData* self) {
    f32 minDist = 1000000.0f;
    PuniData* found = NULL;
    PuniData* q = &Punidat[1];
    s32 i;

    for (i = 1; i < 103; i++, q++) {
        if ((q->unk4 & 0x400) && !(q->unk4 & 0x4000) && q->unkC != 9 && q->unkC != 13) {
            f32 d = PSVECSquareDistance(&self->pos, &q->pos);
            if (d < minDist) {
                minDist = d;
                found = q;
            }
        }
    }
    if (minDist < 2500.0f) {
        return found;
    }
    return NULL;
}

PuniData* searchTeki(PuniData* self) {
    f32 minDist = 1000000.0f;
    PuniData* found = NULL;
    PuniData* q = &Punidat[110];
    s32 i;

    for (i = 110; i < 210; i++, q++) {
        if ((q->unk4 & 0x400) && !(q->unk4 & 0x4000) && q->unkC != 9 && q->unkC != 13) {
            f32 d = PSVECSquareDistance(&self->pos, &q->pos);
            if (d < minDist) {
                minDist = d;
                found = q;
            }
        }
    }
    if (minDist < 2500.0f) {
        return found;
    }
    return NULL;
}

PuniData* searchBattleMikata(PuniData* self) {
    f32 minDist = 1000000.0f;
    PuniData* found = NULL;
    PuniData* q = &Punidat[1];
    s32 i;

    for (i = 1; i < 103; i++, q++) {
        if ((q->unk4 & 0x400) && !(q->unk4 & 0x4000) && q->unkC != 9 && q->unkC != 13) {
            f32 d = PSVECSquareDistance(&self->pos, &q->pos);
            if (d < minDist) {
                minDist = d;
                found = q;
            }
        }
    }
    if (minDist < 1000000.0f) {
        return found;
    }
    return NULL;
}

static inline PuniData* searchBattleTeki_inl(PuniData* self) {
    s32 i;
    PuniData* q;
    f32 minDist = 1000000.0f;
    PuniData* found = NULL;

    q = &Punidat[110];
    for (i = 110; i < 210; i++) {
        if ((q->unk4 & 0x400) && !(q->unk4 & 0x4000) && q->unkC != 9 && q->unkC != 13) {
            f32 d = PSVECSquareDistance(&self->pos, &q->pos);
            if (d < minDist) {
                minDist = d;
                found = q;
            }
        }
        q++;
    }
    {
    PuniData* ret;
    if (minDist < 1000000.0f) {
        ret = found;
    } else {
        ret = NULL;
    }
    return ret;
    }
}

static inline PuniData* searchBattleMikata_inl(PuniData* self) {
    s32 i;
    PuniData* q;
    f32 minDist = 1000000.0f;
    PuniData* found = NULL;

    q = &Punidat[1];
    for (i = 1; i < 103; i++) {
        if ((q->unk4 & 0x400) && !(q->unk4 & 0x4000) && q->unkC != 9 && q->unkC != 13) {
            f32 d = PSVECSquareDistance(&self->pos, &q->pos);
            if (d < minDist) {
                minDist = d;
                found = q;
            }
        }
        q++;
    }
    {
    PuniData* ret;
    if (minDist < 1000000.0f) {
        ret = found;
    } else {
        ret = NULL;
    }
    return ret;
    }
}

PuniData* searchBattleTeki(PuniData* self) {
    f32 minDist = 1000000.0f;
    PuniData* found = NULL;
    PuniData* q = &Punidat[110];
    s32 i;

    for (i = 110; i < 210; i++, q++) {
        if ((q->unk4 & 0x400) && !(q->unk4 & 0x4000) && q->unkC != 9 && q->unkC != 13) {
            f32 d = PSVECSquareDistance(&self->pos, &q->pos);
            if (d < minDist) {
                minDist = d;
                found = q;
            }
        }
    }
    if (minDist < 1000000.0f) {
        return found;
    }
    return NULL;
}

void puniSearch(PuniData* p) {
    if ((s32)(gp->retraceCount & 3) == (*(s16*)((u8*)p + 0x116) & 3)) {
        if (p->unk4 & 1) {
            p->unk1DC = (s32)searchBattleTeki_inl(p);
        } else {
            p->unk1DC = (s32)searchBattleMikata_inl(p);
        }
    }
}

void puniModeChangeAttack(void) {
    s32 i;
    PuniData* p = &Punidat[1];
    for (i = 1; i < 211; i++, p++) {
        if (p->unk4 & 0x400) {
            switch (p->unkC) {
            case 0:
            case 1:
            case 2:
            case 0x1d: {
                PuniData* f;
                if (p->unk4 & 1) {
                    f = searchBattleTeki_inl(p);
                } else {
                    f = searchBattleMikata_inl(p);
                }
                if (f) {
                    p->unk1DC = (s32)f;
                    p->unkC = 7;
                    p->unk114 = 0;
                    p->unk11A = 0;
                    p->unk4 |= 0x100000;
                    PuniDokuritsu(p);
                    if (p->effect) {
                        effDelete(p->effect);
                        p->effect = NULL;
                    }
                }
                break;
            }
            }
        }
    }
}

void puniModeChangeAway(void) {
    s32 i;
    PuniData* p = &Punidat[1];

    for (i = 1; i <= 102; i++, p++) {
        if (!(p->unk4 & 0x400)) {
            continue;
        }
        if (1000000.0f == p->unk1C0) {
            continue;
        }
        switch (p->unkC) {
            case 0:
            case 1:
            case 2:
            case 4:
            case 16:
                PuniDokuritsu(p);
                p->unkE = p->unkC;
                p->unkC = 5;
                p->unk114 = 0;
                if (p->effect != NULL) {
                    effDelete(p->effect);
                    p->effect = NULL;
                }
                break;
        }
    }
}

static const Vec puni_fallchk_dir = {0.0f, -1.0f, 0.0f};

void puniModeChangeFallCheck(void) {
    Vec dir;
    Vec pos;
    Vec nrm;
    Vec hit;
    f32 dist;
    s32 i;
    PuniData* p = &Punidat[1];

    for (i = 1; i <= 102; i++, p++) {
        if (!(p->unk4 & 0x400)) {
            continue;
        }
        pos = p->pos;
        pos.y = 1.0f + pos.y;
        dir = puni_fallchk_dir;
        dist = 2.0f;
        if (hitCheckFilter(NULL, &hit.x, &hit.y, &hit.z, &dist, &nrm.x, &nrm.y, &nrm.z, pos.x, pos.y, pos.z, dir.x, dir.y, dir.z) ==
            NULL) {
            PuniDokuritsu(p);
            {
                Vec v = {0.0f, 0.0f, 0.0f};
                p->vel = v;
            }
            p->unkC = 17;
            p->unk114 = 0;
            if (p->effect != NULL) {
                effDelete(p->effect);
                p->effect = NULL;
            }
        }
    }
}

void puniFukki(PuniData* self) {
    s32 i;
    PuniData* p = &Punidat[1];

    for (i = 1; i <= 102; i++, p++) {
        if ((p->unk4 & 0x400) && p->unkC == 16) {
            if (!(32.0 < __fabs(self->pos.y - p->pos.y)) && PSVECSquareDistance(&self->pos, &p->pos) < 900.0f) {
                p->unkC = 1;
                puniFukki(p);
                puniDeleteEffect(p);
            }
        }
    }
}

void puniModeALLCheck(void) {
    switch (Punigp->unk43C) {
        case 2:
            puniModeChangeAway();
            break;
        case 3:
            puniModeChangeAttack();
            break;
        case 4:
        default:
            break;
    }
}

void puniModeALLEndCheck(void) {
    switch (Punigp->unk43C) {
        case 1:
            break;
        case 2:
            if (Punigp->unk440 == 2) {
                Punigp->unk43C = 0;
            }
            break;
        case 3:
            break;
    }
    Punigp->unk440 = Punigp->unk43C;
}

void puniModeNormal(PuniData* p) {
    if (p->unk1A0 > 10.0f) {
        p->unkE = p->unkC;
        PuniDokuritsu(p);
        p->unk1DC = Punigp->unk360;
        p->unkC = 13;
        p->unk114 = 0;
    } else if (searchMario(p)) {
        p->unkC = 1;
    }
}

void puniModeParentpre(PuniData* p) {
    Vec v;

    searchParent(p);
    p->unkC = 2;
    p->unk4 |= 0x180000;
    PSVECSubtract(&p->parent->pos, &p->pos, &v);
    p->unk1B0 = p->unk1A8 = PSVECMag(&v);
    p->unk128 = 0;
    p->unk114 = 0;
    p->anim = 1;
}

void puniModeParent(PuniData* p) {
    Vec v;
    s32 bx, bz;
    s32 flag;
    u16 info;
    switch (*(u16*)&p->unk11E) {
        case 0:
        case 1:
            if (p->unk1AC < 1.5f) {
                p->anim = 0;
            } else {
                p->anim = 1;
            }
            p->unk120 = 0;
            break;
        default:
            if (p->unk1AC < 1.5f) {
                p->anim = 0;
            } else {
                p->anim = 1;
            }
            if (*(u16*)&p->unk11E == p->anim) {
                p->unk120 = 0;
            } else if (++p->unk120 <= 8) {
                p->anim = *(u16*)&p->unk11E;
            } else {
                p->unk120 = 0;
            }
            break;
    }
    if (p->unk4 & 0x200) {
        return;
    }
    flag = 0;
    PSVECSubtract(&p->parent->pos, &p->pos, &v);
    p->unk1A8 = PSVECMag(&v);
    if (!(bero_get_BeroEXEC() & 1)) {
        if (32.0 < __fabs(p->pos.y - p->parent->pos.y)) {
            flag = 1;
        } else if (p->unk1A8 > 80.0f) {
            flag = 1;
        } else {
            p->unk128 = 0;
        }
    }
    if (0.1f + p->unk1A8 <= p->unk1B0) {
        flag = 0;
        p->unk128 = 0;
    }
    if (Punimap->unk1C != 0) {
        flag = 0;
    }
    if (flag) {
        if (++p->unk128 > 30) {
            p->unkC = 16;
            p->unk114 = 0;
            PuniChildHagure(p);
            PuniDokuritsu(p);
            return;
        }
    }
    p->unk1B0 = p->unk1A8;
    if (p->unk1A0 > 10.0f) {
        p->unkE = p->unkC;
        PuniDokuritsu(p);
        p->unk1DC = Punigp->unk360;
        p->unkC = 13;
        p->unk114 = 0;
        return;
    }
    if (p->unk1BC != -1 && p->unk1C0 < 10000.0f) {
        Punigp->unk43C = 2;
        return;
    }
    if (p->unk1B4 != -1) {
        PuniDokuritsu(p);
        p->unkC = 4;
        p->unk114 = 0;
        return;
    }
    if (Punimap->enabled != 0 && Punimap->unk1C != 0) {
        f32 z = p->pos.z;
        f32 x = p->pos.x;
        switch (Punigp->mapKind) {
            case 0:
                break;
            case 1:
                puniMapdata_MRI_06_getidx(x, z, &bx, &bz);
                break;
            case 2:
                puniMapdata_MRI_09_getidx(x, z, &bx, &bz);
                break;
            case 3:
                puniMapdata_MRI_16_getidx(x, z, &bx, &bz);
                break;
        }
        info = puniMapGetInfo(bx, bz);
        if (info != 0xFFFF && (info & 0xFFF) > 2) {
            PuniDokuritsu(p);
            p->unkC = 3;
            p->unk114 = 0;
            return;
        }
    }
    if (p->unk1A8 > p->unk190.x + p->parent->unk190.x + p->unk190.y + p->unk190.z) {
        PSVECNormalize(&v, &v);
        v.y = 0.0f;
        PSVECScale(&v, &v, 1.0f);
        PSVECAdd(&p->vel, &v, &p->vel);
        p->unk174 = 3.0f;
    }
}

#define PSEARCH_POS(a, b)                                                      { s32 _pa = (b); s32 _pb = (a); switch (Punigp->mapKind) {                                                     case 0: break; case 1: puniMapdata_MRI_06_getpos(_pa, _pb, &tgx, &tgz); break;         case 2: puniMapdata_MRI_09_getpos(_pa, _pb, &tgx, &tgz); break;         case 3: puniMapdata_MRI_16_getpos(_pa, _pb, &tgx, &tgz); break;     } }

void puniModeSearch(PuniData* p) {
    static u16 find;
    static u16 move;
    static u16 takasa;
    static u16 m0;
    static u16 m1;
    static u16 m2;
    static u16 m3;
    static u16 h0;
    static u16 h1;
    static u16 h2;
    static u16 h3;
    f32 px, pz;
    s32 bx;
    s32 bz;
    f32 tgx, tgz;
    Vec d;
    u16 c, cl, cu, cr, cd;
    s32 k;

    if (p->unk1A0 > 10.0f) {
        p->unkE = p->unkC;
        PuniDokuritsu(p);
        p->unk1DC = Punigp->unk360;
        p->unkC = 13;
        p->unk114 = 0;
        return;
    }
    if (Punimap->unk1C == 0 && p->unk1A4 == 1000.0f) {
        p->unkC = 16;
        *(s16*)((u8*)p + 0x11C) = 0;
        p->unk114 = 0;
    }
    if (Punimap->enabled == 0) {
        return;
    }
    if (Punimap->unk1C == 0) {
        return;
    }
    pz = p->pos.z;
    px = p->pos.x;
    switch (Punigp->mapKind) {
        case 0:
            break;
        case 1:
            puniMapdata_MRI_06_getidx(px, pz, &bx, &bz);
            break;
        case 2:
            puniMapdata_MRI_09_getidx(px, pz, &bx, &bz);
            break;
        case 3:
            puniMapdata_MRI_16_getidx(px, pz, &bx, &bz);
            break;
    }
    c = Punimap->data[bz * Punimap->width + bx];
    if (!(c & 0xFF)) {
        c |= 0xFFFF;
    }
    k = c & 0xFF;
    if (k <= 2) {
        p->unkC = 1;
        p->unk4 |= 0x100000;
        return;
    }
    k--;
    takasa = c & 0xF000;
    find = c & 0x0F00;
    move = k;
    k = bz * Punimap->width;
    k = bx + k;
    k--;
    cl = Punimap->data[k];
    if (!(cl & 0xFF)) { cl |= 0xFFFF; }
    m0 = cl;
    cu = Punimap->data[(bz - 1) * Punimap->width + bx];
    if (!(cu & 0xFF)) { cu |= 0xFFFF; }
    m1 = cu;
    k = bz * Punimap->width;
    k = bx + k;
    k++;
    cr = Punimap->data[k];
    if (!(cr & 0xFF)) { cr |= 0xFFFF; }
    m2 = cr;
    cd = Punimap->data[(bz + 1) * Punimap->width + bx];
    if (!(cd & 0xFF)) { cd |= 0xFFFF; }
    h0 = cl & 0xF000;
    h1 = cu & 0xF000;
    h2 = cr & 0xF000;
    h3 = cd & 0xF000;
    m0 = cl & 0xFF;
    m1 = cu & 0xFF;
    m2 = cr & 0xFF;
    m3 = cd & 0xFF;
    switch (find) {
        case 0x000:
            /* TODO: block at 0x0c19c-0x0c844: verify neighbour order/subset */
            if (m0 == move && __abs((s32)h0 - (s32)takasa) <= 0x1000) {
                PSEARCH_POS(bz, bx - 1);
            } else if (m1 == move && __abs((s32)h1 - (s32)takasa) <= 0x1000) {
                PSEARCH_POS(bz - 1, bx);
            } else if (m2 == move && __abs((s32)h2 - (s32)takasa) <= 0x1000) {
                PSEARCH_POS(bz, bx + 1);
            } else if (m3 == move && __abs((s32)h3 - (s32)takasa) <= 0x1000) {
                PSEARCH_POS(bz + 1, bx);
            } else {
                tgx = 0.0f;
                tgz = 0.0f;
            }
            break;
        case 0x100:
            /* TODO: block at 0x0c844-0x0ceec: verify neighbour order/subset */
            if (m1 == move && __abs((s32)h1 - (s32)takasa) <= 0x1000) {
                PSEARCH_POS(bz - 1, bx);
            } else if (m2 == move && __abs((s32)h2 - (s32)takasa) <= 0x1000) {
                PSEARCH_POS(bz, bx + 1);
            } else if (m3 == move && __abs((s32)h3 - (s32)takasa) <= 0x1000) {
                PSEARCH_POS(bz + 1, bx);
            } else if (m0 == move && __abs((s32)h0 - (s32)takasa) <= 0x1000) {
                PSEARCH_POS(bz, bx - 1);
            } else {
                tgx = 0.0f;
                tgz = 0.0f;
            }
            break;
        case 0x200:
            /* TODO: block at 0x0ceec-0x0d594: verify neighbour order/subset */
            if (m2 == move && __abs((s32)h2 - (s32)takasa) <= 0x1000) {
                PSEARCH_POS(bz, bx + 1);
            } else if (m3 == move && __abs((s32)h3 - (s32)takasa) <= 0x1000) {
                PSEARCH_POS(bz + 1, bx);
            } else if (m0 == move && __abs((s32)h0 - (s32)takasa) <= 0x1000) {
                PSEARCH_POS(bz, bx - 1);
            } else if (m1 == move && __abs((s32)h1 - (s32)takasa) <= 0x1000) {
                PSEARCH_POS(bz - 1, bx);
            } else {
                tgx = 0.0f;
                tgz = 0.0f;
            }
            break;
        case 0x300:
            /* TODO: block at 0x0d594-0x0dc44: verify neighbour order/subset */
            if (m3 == move && __abs((s32)h3 - (s32)takasa) <= 0x1000) {
                PSEARCH_POS(bz + 1, bx);
            } else if (m0 == move && __abs((s32)h0 - (s32)takasa) <= 0x1000) {
                PSEARCH_POS(bz, bx - 1);
            } else if (m1 == move && __abs((s32)h1 - (s32)takasa) <= 0x1000) {
                PSEARCH_POS(bz - 1, bx);
            } else if (m2 == move && __abs((s32)h2 - (s32)takasa) <= 0x1000) {
                PSEARCH_POS(bz, bx + 1);
            } else {
                tgx = 0.0f;
                tgz = 0.0f;
            }
            break;
        default:
            tgx = 0.0f;
            tgz = 0.0f;
            break;
    }
    d.x = tgx;
    d.y = p->pos.y;
    d.z = tgz;
    PSVECSubtract(&d, &p->pos, &d);
    if (d.x != 0.0f || d.y != 0.0f || d.z != 0.0f) {
        PSVECNormalize(&d, &d);
    }
    d.y = 0.0f;
    PSVECScale(&d, &d, 1.0f);
    PSVECAdd(&p->vel, &d, &p->vel);
    p->unk174 = 3.0f;
}

#define F(p, o) (*(s32*)((u8*)(p) + (o)))
#define FF(p, o) (*(f32*)((u8*)(p) + (o)))
void puniModeMass(PuniData* p) {
    Vec v;
    PuniTarget* t = &Punigp->targets[p->unk1B4];
    s32 a, b;
    if (p->unk1B4 == -1) {
        if (p->unkE == 24) {
            p->unkC = 24;
        } else {
            p->unkC = 1;
        }
        p->anim = 0;
        p->unk114 = 0;
        return;
    }
    if (p->unk1BC != -1 && p->unk1C0 < 10000.0f) {
        Punigp->unk43C = 2;
        return;
    }
    a = gp->retraceCount & 0xF;
    b = *(s16*)&p->unk116 & 0xF;
    if (Punigp->unk328 == 18) {
        a = b;
    }
    if (a == b && p->unk1A0 > 10.0f) {
        PuniDokuritsu(p);
        p->unk1DC = Punigp->unk360;
        p->unkC = 13;
        p->unk114 = 0;
        return;
    }
    if (p->unk1B8 > 100.0f) {
        if (p->unk1B8 < 3600.0f && (qrnd(3) == 0)) {
            PSVECSubtract(&p->pos, &t->pos, &v);
            PSVECNormalize(&v, &v);
            v.y = 0.0f;
            PSVECScale(&v, &v, 1.5f);
            PSVECAdd(&p->vel, &v, &p->vel);
        } else {
            PSVECSubtract(&t->pos, &p->pos, &v);
            PSVECNormalize(&v, &v);
            v.y = 0.0f;
            PSVECScale(&v, &v, 5.0f);
            PSVECAdd(&p->vel, &v, &p->vel);
        }
    } else {
        Vec zv = {0.0f, 0.0f, 0.0f};
        p->vel = zv;
    }
    switch (p->unk114) {
        case 0:
            if (p->vel.x < 0.0f) {
                p->dir = 270.0f;
            } else {
                p->dir = 90.0f;
            }
            p->unk114 = 1;
            if (Punigp->unk328 == 13) {
                p->unk114 = 10;
            }
            if (Punigp->unk328 == 18) {
                p->unk114 = 10;
            }
            p->unk1E0 = 0;
            F(p,0x1E4) = 0;
            break;
        case 1:
            break;
        case 10:
            if (p->unk1E0 == 120) {
                p->unk1E0 = 0;
                F(p,0x1E4) = 0;
                if (rand() % 2) {
                    p->unk1F8 = (Vec){0.0f, 0.0f, 2.0f};
                } else {
                    p->unk1F8 = (Vec){0.0f, 0.0f, -2.0f};
                }
                p->unk114 = 11;
            } else if (p->pos.x == p->unk138.x && p->pos.y == p->unk138.y) {
                p->unk1E0++;
            }
            break;
        case 11:
            if (F(p,0x1E4) == 30) {
                p->unk114 = 10;
            } else {
                PSVECAdd(&p->vel, &p->unk1F8, &p->vel);
                F(p,0x1E4)++;
            }
            break;
    }
}

void puniModeAwayjump(PuniData* p) {
    switch (p->unk114) {
        case 0:
            *(OSTime*)((u8*)p + 0x1C8) = gp->renderTime;
            F(p,0x1E4) = 0;
            FF(p,0x1EC) = p->pos.y;
            FF(p,0x1F0) = -72.0f;
            FF(p,0x1F4) = -FF(p,0x1F0);
            p->unk114 = 1;
            p->anim = 7;
            p->unk124 = 0;
            p->unk4 |= 0x1000000;
            if (p->unk4 & 0x80) {
                psndSFXOn_3D("SFX_STG2_TOGETTI_JUMP1", &p->pos);
            } else {
                psndSFXOn_3D("SFX_STG2_PUNI_JUMP1", &p->pos);
            }
        case 1: {
            u32 t = ((u32)gp->renderTime - F(p,0x1CC)) / (*(u32*)0x800000F8 / 4 / 1000);
            if ((s32)t > 100 && F(p,0x1E4) == 0) {
                F(p,0x1E4) = 1;
                p->anim = 8;
                p->unk124 = 0;
            }
            if ((s32)t < 200) {
                f32 f = (f32)(s32)t / 200.0f;
                p->pos.y = f * (FF(p,0x1F0) * f + FF(p,0x1F4)) + FF(p,0x1EC);
            } else {
                p->pos.y = FF(p,0x1EC);
                p->unkC = 6;
                p->anim = 2;
                p->unk4 &= ~0x1000000;
                p->unk114 = 0;
            }
            break;
        }
    }
}

#define PF(p, o) (*(void**)((u8*)(p) + (o)))
int puniMoveAtari(PuniData* p, Vec* start, Vec* dir, int reflect, f32 dist);

typedef int (*PuniMoveAtariV)(PuniData*, Vec, Vec, int, f32);
static const Vec puni_away_zero0 = {0.0f, 0.0f, 0.0f};
#pragma dont_inline on
int puniModeAway_sub(PuniData* p) {
    Vec dir;
    Vec start;
    f32 mag = PSVECMag(&p->vel);
    start = p->pos;
    start.y += 1.0f;
    if (0.0f != mag) {
        PSVECNormalize(&p->vel, &dir);
    } else {
        dir = puni_away_zero0;
    }
    ((PuniMoveAtariV)puniMoveAtari)(p, start, dir, 0, mag);
    if (PF(p, 0x210) != NULL || PF(p, 0x214) != NULL) {
        if (PF(p, 0x210) != NULL && (((s32*)PF(p, 0x210))[1] & 4)) {
            FF(p, 0x168) = -FF(p, 0x560);
            FF(p, 0x16C) = -FF(p, 0x564);
            FF(p, 0x170) = -FF(p, 0x568);
            p->unk114 = 1;
            return 1;
        } else if (PF(p, 0x214) != NULL && (((s32*)PF(p, 0x214))[1] & 4)) {
            FF(p, 0x168) = -FF(p, 0x560);
            FF(p, 0x16C) = -FF(p, 0x564);
            FF(p, 0x170) = -FF(p, 0x568);
            p->unk114 = 1;
            return 1;
        } else {
            p->unk114 = 10;
            return 2;
        }
    }
    return 0;
}
#pragma dont_inline reset

void puniModeAway(PuniData* p) {
    Vec sub;
    Vec start;
    f32 nz, ny, nx, hz, hy, hx, dist;
    PuniTarget* t;
    s32 idx = p->unk1BC;
    t = &Punigp->targets[idx];
    if (idx == -1) {
        {
            Vec v = {0.0f, 0.0f, 0.0f};
            p->vel = v;
        }
        p->unkC = 1;
        p->unk114 = 0;
        p->anim = 0;
        return;
    }
    if (p->unk1A0 > 10.0f) {
        p->unkE = p->unkC;
        p->unk1DC = Punigp->unk360;
        p->unkC = 13;
        p->unk114 = 0;
        return;
    }
    switch (p->unk114) {
        case 0:
            if (p->unk1C0 < 90000.0f) {
                PSVECSubtract(&p->pos, &t->pos, &sub);
                PSVECNormalize(&sub, &sub);
                sub.y = 0.0f;
                PSVECScale(&sub, &sub, 1.4f);
                PSVECAdd(&p->vel, &sub, &p->vel);
                p->unk174 = 4.2f;
                puniModeAway_sub(p);
            }
            break;
        case 1:
            p->unk8 |= 4;
            PSVECNormalize((Vec*)&FF(p, 0x168), &sub);
            PSVECScale(&sub, &p->vel, p->unk190.x);
            PSVECAdd(&p->pos, &p->vel, &p->pos);
            start = p->pos;
            start.y = 1.0f + start.y;
            {
                Vec d = {0.0f, -1.0f, 0.0f};
                sub = d;
            }
            dist = 2.0f;
            start.y += 0.2f;
            if (hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, start.x, start.y, start.z, sub.x, sub.y, sub.z) == NULL) {
                {
                    Vec v = {0.0f, 0.0f, 0.0f};
                    p->vel = v;
                }
                p->unkC = 17;
                p->anim = 0;
                p->unk114 = 0;
            } else {
                p->unk114 = 0;
            }
            break;
        case 10:
            break;
    }
}
#define IW(p, o) (*(s32*)((u8*)(p) + (o)))

#define SF(p, o) (*(s16*)((u8*)(p) + (o)))
void puniModeAttack(PuniData* p) {
    PuniData* best;
    Vec d;
    f32 m;
    s32 i;
    PuniData* q;
    switch (p->unk114) {
        case 0:
            p->anim = 1;
            p->unk114 = 1;
        case 1:
            if (p->unk1A0 > 10.0f) {
                p->unkE = p->unkC;
                PuniDokuritsu(p);
                p->unk1DC = Punigp->unk360;
                p->unkC = 13;
                p->unk114 = 0;
                break;
            }
            puniSearch(p);
            if ((PuniData*)p->unk1DC != NULL) {
                PSVECSubtract(&((PuniData*)p->unk1DC)->pos, &p->pos, &d);
                if (PSVECMag(&d) > FF((PuniData*)p->unk1DC, 0x19C)) {
                    PSVECNormalize(&d, &d);
                    d.y = 0.0f;
                    PSVECScale(&d, &d, 1.0f);
                    PSVECAdd(&p->vel, &d, &p->vel);
                    p->unk174 = 3.0f;
                } else {
                    puniParaAttack(p, (PuniData*)p->unk1DC);
                }
            }
            break;
    }
}

void puniModeDamage(PuniData* p) {
    Vec d;
    switch (p->unk114) {
        case 0:
            if (IW(Punigp, 0x4C0)++ < 6) psndSFXOn_3D("SFX_STG2_FIGHT_TOGE1", &p->pos);
            p->unk114 = 1;
            break;
        case 1:
            if (IW(Punigp, 0x4C4)++ < 6) psndSFXOn_3D("SFX_STG2_FIGHT_TOGE2", &p->pos);
            p->unk114 = 2;
        case 2:
            PSVECSubtract(&p->pos, &((PuniData*)p->unk1DC)->pos, &d);
            if (0.0f == PSVECMag(&d)) {
                {
                    Vec v_ = {0.0f, 0.0f, 0.0f};
                    d = v_;
                }
            } else {
                PSVECNormalize(&d, &d);
            }
            d.y = 0.0f;
            PSVECScale(&d, &d, 3.0f);
            p->vel = d;
            FF(p, 0x178) = 5.0f;
            p->anim = 3;
            p->unk114 = 3;
            break;
        case 3:
            p->pos.y += FF(p, 0x178);
            FF(p, 0x178) -= 0.5f;
            if (p->pos.y <= 0.001f) {
                p->unk4 &= ~0x2000;
                p->pos.y = 0.0f;
                FF(p, 0x178) = 0.0f;
                p->unk138 = p->pos;
                p->hp -= p->damage;
                if (p->hp <= 0) {
                    p->hp = 0;
                    p->unk4 |= 0x4000;
                    p->unk8 |= 0x10;
                }
                if ((p->hp * 100) / p->maxHp <= 30) {
                    p->unkC = 8;
                    p->anim = 2;
                    p->unk114 = 0;
                } else {
                    p->unkC = 0;
                    p->anim = 0;
                    p->unk114 = 0;
                }
            }
            break;
    }
}


void puniModePiyori(PuniData* p) {
    if (p->unk1A0 > 0.0f) {
        p->unkE = p->unkC;
        PuniDokuritsu(p);
        p->unk1DC = Punigp->unk360;
        p->unk4 &= ~0x8000000;
        p->unk8 &= ~2;
        p->unkC = 13;
        p->unk114 = 0;
        return;
    }
    switch (p->unk114) {
        case 0:
            if (p->unk4 & 2) {
                p->unk128 = 600;
                p->anim = 4;
                p->unk4 |= 0x8100000;
                p->unk8 |= 2;
                p->unk114 = 2;
            } else {
                p->unk114 = 10;
            }
            break;
        case 2:
            if (p->unk128-- == 0) {
                p->unk4 &= ~0x8000000;
                p->unk8 &= ~2;
                p->unk114 = 10;
            }
            break;
        case 10:
            if (p->unkE == 10) {
                if (p->hp * 100 / p->maxHp <= 30) {
                    p->unkC = 8;
                    p->anim = 2;
                    p->unk114 = 0;
                } else {
                    p->unkC = 0;
                    p->anim = 0;
                    p->unk114 = 0;
                }
            } else {
                p->unkC = p->unkE;
                if (p->unkE == 2) {
                    p->unkC = 1;
                }
                p->unk114 = 0;
            }
            break;
    }
}


void puniModeEscape(PuniData* p) {
    Vec d;
    f32 dist;

    switch (p->unk114) {
        case 0:
            break;
        default:
            return;
    }
    if (p->unk1A0 > 0.0f) {
        p->unkE = p->unkC;
        p->unk1DC = Punigp->unk360;
        p->unkC = 13;
        p->unk114 = 0;
        return;
    }
    if (p->hp * 100 / p->maxHp >= 80) {
        p->unkC = 0;
        p->anim = 0;
        p->unk114 = 0;
        return;
    }
    if ((s32)(gp->retraceCount & 3) == (*(s16*)((u8*)p + 0x116) & 3)) {
        if (p->unk4 & 1) {
            p->unk1DC = (s32)searchBattleTeki_inl(p);
        } else {
            p->unk1DC = (s32)searchBattleMikata_inl(p);
        }
    }
    if ((PuniData*)p->unk1DC == NULL) return;
    PSVECSubtract(&((PuniData*)p->unk1DC)->pos, &p->pos, &d);
    dist = PSVECMag(&d);
    if (dist >= 120.0f) {
        {
            Vec v_ = {0.0f, 0.0f, 0.0f};
            p->vel = v_;
        }
        p->unkC = 11;
        p->anim = 0;
        p->unk114 = 0;
        return;
    }
    if (0.0f == dist) {
        {
            Vec v_ = {0.0f, 0.0f, 0.0f};
            d = v_;
        }
    } else {
        PSVECNormalize(&d, &d);
    }
    d.y = 0.0f;
    PSVECScale(&d, &d, 0.8f);
    PSVECSubtract(&p->vel, &d, &p->vel);
    p->unk174 = 2.4f;
}


void puniModeRecover(PuniData* p) {
    switch (p->unk114) {
        case 0: {
            int r = puniPositionCheck(p);
            if (r != 0) {
                *(s32*)((u8*)p + 0x58C) = r;
                *(Vec*)((u8*)p + 0x590) = p->pos;
                puniPositionRecover(p);
                *(Vec*)((u8*)p + 0x59C) = p->pos;
            }
            {
                Vec puni_recover_vel = {0.0f, 0.0f, 0.0f};
                p->vel = puni_recover_vel;
            }
            p->unk114 = 1;
            break;
        }
        case 1: {
            if (p->hp * 100 / p->maxHp >= 80) {
                p->unkC = 0;
                p->anim = 0;
                p->unk114 = 0;
            } else if (p->unk1A0 > 10.0f) {
                p->unkE = p->unkC;
                p->unk1DC = Punigp->unk360;
                p->unkC = 13;
                p->unk114 = 0;
            } else {
                if (((s32)gp->retraceCount & 3) == (*(s16*)&p->unk116[0] & 3)) {
                    if (p->unk4 & 1) {
                        p->unk1DC = (s32)searchBattleTeki_inl(p);
                    } else {
                        p->unk1DC = (s32)searchBattleMikata_inl(p);
                    }
                }
                if ((PuniData*)p->unk1DC != NULL) {
                    Vec d;
                    PSVECSubtract(&((PuniData*)p->unk1DC)->pos, &p->pos, &d);
                    if (PSVECMag(&d) < 80.0f) {
                        p->unkC = 8;
                        p->anim = 2;
                        p->unk114 = 0;
                    }
                }
            }
            break;
        }
    }
}

void puniModeDead(PuniData* p) {
    switch (p->unk114) {
        case 0:
            psndSFXOn_3D("SFX_STG2_FIGHT_TOGE3", &p->pos);
            {
                Vec v = {0.0f, 0.0f, 0.0f};
                p->vel = v;
            }
            p->anim = 4;
            p->unk114 = 1;
            break;
        case 1:
            break;
    }
}

#define WPF(p, o) (*(void**)((u8*)(p) + (o)))
#define WFF(p, o) (*(f32*)((u8*)(p) + (o)))
int puniMoveAtari(PuniData* p, Vec* start, Vec* dir, int reflect, f32 dist);
typedef int (*PuniMoveAtariW)(PuniData*, Vec, Vec, int, f32);
int puniModeWind_sub(PuniData* p) {
    Vec dir;
    Vec start;
    f32 mag = PSVECMag(&p->vel);
    start = p->pos;
    start.y += 1.0f;
    if (0.0f != mag) {
        PSVECNormalize(&p->vel, &dir);
    } else {
        {
            Vec v_ = {0.0f, 0.0f, 0.0f};
            dir = v_;
        }
    }
    ((PuniMoveAtariW)puniMoveAtari)(p, start, dir, 0, mag);
    if (Punigp->unk43C == 3 && (WPF(p, 0x210) != NULL || WPF(p, 0x214) != NULL)) {
        p->unk114 = 10;
        return 2;
    }
    if (WPF(p, 0x210) != NULL || WPF(p, 0x214) != NULL) {
        if (WPF(p, 0x210) != NULL && (((s32*)WPF(p, 0x210))[1] & 4)) {
            WFF(p, 0x168) = -WFF(p, 0x560);
            WFF(p, 0x16C) = 0.0f;
            WFF(p, 0x170) = -WFF(p, 0x568);
            *(Vec*)&WFF(p, 0x150) = p->pos;
            p->unk114 = 3;
            return 1;
        } else if (WPF(p, 0x214) != NULL && (((s32*)WPF(p, 0x214))[1] & 4)) {
            WFF(p, 0x168) = -WFF(p, 0x560);
            WFF(p, 0x16C) = 0.0f;
            WFF(p, 0x170) = -WFF(p, 0x568);
            *(Vec*)&WFF(p, 0x150) = p->pos;
            p->unk114 = 3;
            return 1;
        } else {
            p->unk114 = 10;
            return 2;
        }
    }
    return 0;
}

void puniModeWind_kaiten(PuniData* p) {
    BOOL dir;
    if (p->dir < 180.0f) {
        dir = p->unk1E0;
    } else {
        dir = p->unk1E0 ^ 1;
    }
    if (dir) {
        p->spin += 10.0f;
        if (p->spin >= 360.0f) {
            p->spin -= 360.0f;
        }
    } else {
        p->spin -= 10.0f;
        if (p->spin <= 0.0f) {
            p->spin += 360.0f;
        }
    }
}


#define WF(p, o) (*(f32*)((u8*)(p) + (o)))

void puniModeWind(PuniData* p) {
    MarioWork* mario = marioGetPtr();
    f32 dist;
    Vec d;
    switch (p->unk114) {
    case 0:
        if (p->unk4 & 0x18) {
            p->unk188 = 8.0f;
        } else {
            p->unk148 = 8.0f;
        }
        p->unk4 &= ~0x00100000;
        PSVECSubtract(&p->pos, &mario->position, &d);
        if (d.x < 0.0f) {
            p->unk1E0 = 0;
        } else {
            p->unk1E0 = 1;
        }
        d.x *= 1.5f;
        d.z *= 0.5f;
        dist = PSVECMag(&d);
        if (0.0f != dist) {
            PSVECNormalize(&d, &d);
        } else {
            {
                Vec v_ = {0.0f, 0.0f, 0.0f};
                d = v_;
            }
        }
        d.y = 0.0f;
        WF(p, 0x1F4) = 6.0f * (p->unk1A0 / 100.0f) + 2.0f;
        p->unk1F8 = d;
        WF(p, 0x1EC) = 0.0f;
        WF(p, 0x1F0) = 0.0f;
        if (p->unk4 & 1) {
            WF(p, 0x178) = 0.06f;
        } else {
            WF(p, 0x178) = 0.6f;
        }
        p->anim = 5;
        p->unk114 = 1;
    case 1:
        PSVECScale(&p->unk1F8, &d, WF(p, 0x1EC));
        d.z = 0.0f;
        p->vel = d;
        puniModeWind_kaiten(p);
        WF(p, 0x1EC) += WF(p, 0x1F0);
        WF(p, 0x1F0) = WF(p, 0x1F0) + WF(p, 0x178);
        if (puniModeWind_sub(p) == 0 && WF(p, 0x1EC) >= WF(p, 0x1F4)) {
            if (p->unk4 & 1) {
                WF(p, 0x178) = 0.03f;
            } else {
                WF(p, 0x178) = 0.003f;
            }
            WF(p, 0x1F0) = 0.0f;
            p->unk114 = 2;
        }
        break;
    case 2:
        PSVECScale(&p->unk1F8, &d, WF(p, 0x1EC));
        d.z = 0.0f;
        p->vel = d;
        puniModeWind_kaiten(p);
        WF(p, 0x1EC) -= WF(p, 0x1F0);
        WF(p, 0x1F0) = WF(p, 0x1F0) + WF(p, 0x178);
        if (puniModeWind_sub(p) == 0 && WF(p, 0x1EC) < 0.0f) {
            p->unk114 = 10;
        }
        break;
    case 3:
        p->unk8 |= 4;
        PSVECNormalize((Vec*)((u8*)p + 0x168), &d);
        PSVECScale(&d, &p->vel, p->unk190.x);
        p->unk114 = 4;
    case 4: {
        Vec start;
        f32 hx, hy, hz, nx, ny, nz;
        PSVECAdd(&p->pos, &p->vel, &p->pos);
        start = p->pos;
        start.y = start.y + 0.2f;
        {
            Vec v_ = {0.0f, -1.0f, 0.0f};
            d = v_;
        }
        dist = 2.0f;
        if (hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, start.x, start.y, start.z, d.x, d.y,
                           d.z) == NULL) {
            {
                Vec v_ = {0.0f, 0.0f, 0.0f};
                p->vel = v_;
            }
            p->unkC = 17;
            p->anim = 0;
            p->unk188 = 0.0f;
            p->spin = 0.0f;
            p->unk148 = 0.0f;
            p->unk114 = 0;
        } else {
            p->unk114 = 10;
        }
        break;
    }
    case 10:
        puniModeWind_kaiten(p);
        if (!(p->unk1A0 > 0.0f)) {
            if (Punigp->unk43C == 3) {
                p->unkC = 10;
                p->unk188 = 0.0f;
                p->spin = 0.0f;
                p->unk148 = 0.0f;
                {
                    Vec v_ = {0.0f, 0.0f, 0.0f};
                    p->vel = v_;
                }
                p->unk114 = 0;
            } else {
                p->unk188 = 0.0f;
                p->spin = 0.0f;
                p->unk148 = 0.0f;
                {
                    Vec v_ = {0.0f, 0.0f, 0.0f};
                    p->vel = v_;
                }
                p->unkC = 1;
                p->unk114 = 0;
            }
        }
        break;
    }
}

#define FP(p, o) (*(s32**)((u8*)(p) + (o)))
void puniModeFall(PuniData* p) {
    Vec start;
    Vec dir;
    f32 mag;
    PuniTarget* t = Punigp->targets;
    s32* h0;
    s32* h1;
    s32 found;
    s32 i;

    switch (p->unk114) {
    case 0:
        p->unk4 &= ~0x00100000;
        {
            Vec v = {0.0f, -0.8f, 0.0f};
            p->vel = v;
        }
        p->unk114 = 1;
        break;
    case 1:
        p->vel.x = 0.0f;
        p->vel.y -= 0.45f;
        {
            Vec v = {0.0f, -1.0f, 0.0f};
            dir = v;
        }
        mag = PSVECMag(&p->vel);
        start = p->pos;
        ((PuniMoveAtariW)puniMoveAtari)(p, start, dir, 0, mag);
        h0 = FP(p, 0x210);
        if (h0 != NULL || FP(p, 0x214) != NULL) {
            if (h0 != NULL && (h0[1] & 4)) {
                break;
            }
            h1 = FP(p, 0x214);
            if (h1 != NULL && (h1[1] & 4)) {
                break;
            }
            if (h0 != NULL && (h0[1] & 0x100)) {
                p->unk1E0 = 0x12;
                p->anim = 0;
                p->unk114 = 2;
                break;
            }
            if (h1 != NULL && (h1[1] & 0x100)) {
                p->unk1E0 = 0x12;
                p->anim = 0;
                p->unk114 = 2;
                break;
            }
            if (h0 != NULL && (h0[1] & 0x200)) {
                p->unk1E0 = 0x16;
                p->anim = 3;
                p->unk114 = 2;
                break;
            }
            if (h1 != NULL && (h1[1] & 0x200)) {
                p->unk1E0 = 0x16;
                p->anim = 3;
                p->unk114 = 2;
                break;
            }
            found = 0;
            for (i = 0; i < 2; i++, t++) {
                if (t->hit == NULL) {
                    continue;
                }
                if ((void*)h0 == (void*)t->hit) {
                    found = 1;
                    p->unk1F8 = t->pos;
                    break;
                }
                if ((void*)h1 == (void*)t->hit) {
                    found = 1;
                    p->unk1F8 = t->pos;
                    break;
                }
            }
            if (found) {
                p->unk1E0 = 0x17;
            } else {
                p->unk1E0 = 0xF;
            }
            if (Punigp->unk328 == 0x12 && p->pos.y < -300.0f) {
                s32 zero = 0;
                puniswTrapSetindex(*(s16*)&p->unk116[0], &zero);
                p->unk4 &= ~0x00800000;
            }
            {
                Vec v = {0.0f, 0.0f, 0.0f};
                p->vel = v;
            }
            p->unk114 = 2;
        } else if (p->pos.y <= -1000.0f) {
            *(Vec*)((u8*)p + 0x590) = p->pos;
            p->pos = Punigp->unk344;
            p->pos.x += randf(0.0f, 2.0f) - 1.0f;
            p->pos.z += randf(0.0f, 2.0f) - 1.0f;
            p->unk138 = p->pos;
            *(Vec*)((u8*)p + 0x59C) = p->pos;
            p->unk1E0 = 0x10;
            {
                Vec v = {0.0f, 0.0f, 0.0f};
                p->vel = v;
            }
            p->unk114 = 2;
        }
        break;
    case 2:
        p->unkC = p->unk1E0;
        {
            Vec v = {0.0f, 0.0f, 0.0f};
            p->vel = v;
        }
        p->unk114 = 0;
        break;
    }
}

void puniModePreupdraft(PuniData* p) {
    Vec pos;

    switch (p->unk114) {
        case 0:
            pos = p->pos;
            pos.y += 15.0f;
            pos.z += 3.0f;
            p->effect = effPuniBalloonEntry(0, pos.x, pos.y, pos.z, 1.5f);
            p->unk128 = rand() % 201 + 30;
            p->unk114 = 1;
        case 1:
            if (p->unk128-- == 0) {
                p->npc->flags |= 4;
                p->unkC = 19;
                p->unk114 = 0;
            }
            break;
    }
}


void puniModeUpdraft(PuniData *p)
{
  MarioWork *mario = marioGetPtr();
  Vec d;
  Vec save;
  Vec savev;
  Vec dir;
  f32 mag;
  f32 fx;
  f32 fz;
  s32 a;
  s32 b;
  s32 t;
  if (p->unk114 == 0)
  {
    {
      Vec v_ = {0.0f, 0.94f, 0.0f};
      p->vel = v_;
    }
  }
  else
  {
    {
      Vec v_ = {0.0f, 0.66f, 0.0f};
      p->vel = v_;
    }
  }
  if ((gp->retraceCount & 0x1F) == 0)
  {
    psndSFXOn_3D("SFX_STG2_SOAPBALL_MOVE1", &p->pos);
  }
  t = (gp->retraceCount + (*((s16 *) (&p->unk116[0])))) & 0xF;
  if (t > 7)
  {
    t = 4 - (t & 7);
  }
  else
  {
    t = t - 3;
  }
  *((f32 *) (((u8 *) p) + 0x144)) = 0.5f * ((f32) t);
  switch (p->unk114)
  {
    case 0:
      if (p->unk1A0 > 10.0f)
    {
      p->unk1DC = Punigp->unk360;
      PSVECSubtract(&p->pos, &mario->position, &d);
      mag = PSVECMag(&d);
      if (0.0f != mag)
      {
        PSVECNormalize(&d, &d);
      }
      d.y = 0.0f;
      d.z = 0.0f;
      *((f32 *) (((u8 *) p) + 0x1F4)) = 1.2f + (p->unk1A0 / 100.0f);
      p->unk1F8 = d;
      *((f32 *) (((u8 *) p) + 0x1EC)) = 0.0f;
      *((f32 *) (((u8 *) p) + 0x1F0)) = 0.0f;
      p->unk114 = 1;
    }
      break;

    case 1:
      PSVECScale(&p->unk1F8, &d, *((f32 *) (((u8 *) p) + 0x1EC)));
      PSVECAdd(&p->vel, &d, &p->vel);
      *((f32 *) (((u8 *) p) + 0x1EC)) += *((f32 *) (((u8 *) p) + 0x1F0));
      *((f32 *) (((u8 *) p) + 0x1F0)) += 0.06f;
      if ((*((f32 *) (((u8 *) p) + 0x1EC))) >= (*((f32 *) (((u8 *) p) + 0x1F4))))
    {
      *((f32 *) (((u8 *) p) + 0x1EC)) = *((f32 *) (((u8 *) p) + 0x1F4));
    }
      break;

    case 2:
      PSVECScale(&p->unk1F8, &d, *((f32 *) (((u8 *) p) + 0x1EC)));
      PSVECAdd(&p->vel, &d, &p->vel);
      *((f32 *) (((u8 *) p) + 0x1EC)) -= *((f32 *) (((u8 *) p) + 0x1F0));
      *((f32 *) (((u8 *) p) + 0x1F0)) += 0.03f;
      if ((*((f32 *) (((u8 *) p) + 0x1EC))) < 0.0f)
    {
      p->unk114 = 0;
    }
      break;

  }

  mag = PSVECMag(&p->vel);
  if (0.0f != mag)
  {
    PSVECNormalize(&p->vel, &d);
  }
  else
  {
    {
      Vec v_ = {0.0f, 0.0f, 0.0f};
      d = v_;
    }
  }
  save = p->pos;
  savev = p->vel;
  ((PuniMoveAtariW) puniMoveAtari)(p, save, d, 1, mag);
  {
    Vec q = p->pos;
    q.x = q.x + (*((f32 *) (((u8 *) p) + 0x144)));
    ((f32 *) (*((s32 **) (((u8 *) p) + 0x218)))[3])[1] = q.x;
    q.y += 15.0f;
    ((f32 *) (*((s32 **) (((u8 *) p) + 0x218)))[3])[2] = q.y;
    q.z = q.z + 3.0f;
    ((f32 *) (*((s32 **) (((u8 *) p) + 0x218)))[3])[3] = q.z;
    if (((*((s32 **) (((u8 *) p) + 0x210))) != ((void *) 0)) || ((*((s32 **) (((u8 *) p) + 0x214))) != ((void *) 0)))
    {
      s32 f = 0;
      if (((*((s32 **) (((u8 *) p) + 0x210))) != ((void *) 0)) && ((*((s32 **) (((u8 *) p) + 0x210)))[1] & 4))
      {
        f = 1;
      }
      if (((*((s32 **) (((u8 *) p) + 0x214))) != ((void *) 0)) && ((*((s32 **) (((u8 *) p) + 0x214)))[1] & 4))
      {
        f = 1;
      }
      if (f == 0)
      {
        p->unkC = 20;
        p->unk114 = 0;
        *((f32 *) (((u8 *) p) + 0x144)) = 0.0f;
      }
      else
      {
        p->pos = save;
        p->vel = savev;
        PSVECAdd(&p->pos, &p->vel, &p->pos);
      }
    }
    if (p->pos.y >= 0.0f)
    {
      p->unkC = 20;
      p->unk114 = 0;
      *((f32 *) (((u8 *) p) + 0x144)) = 0.0f;
    }
  }
}

void puniModeWonder(PuniData* p) {
    Vec pos;

    switch (p->unk114) {
        case 0:
            psndSFXOn_3D("SFX_STG2_SOAPBALL_BREAK1", &p->pos);
            if (p->effect != NULL) {
                effDelete(p->effect);
                p->effect = NULL;
            }
            pos = p->pos;
            pos.y += 15.0f;
            pos.z += 3.0f;
            p->effect = effPuniBalloonEntry(1, pos.x, pos.y, pos.z, 1.5f);
            p->unk128 = 20;
            p->anim = 9;
            p->unk124 = 0;
            p->unk4 |= 0x1000000;
            p->unk114 = 1;
        case 1:
            if (p->unk128-- == 0) {
                p->npc->flags &= ~4;
                p->unkC = 21;
                p->anim = 1;
                p->unk4 &= ~0x1000000;
                p->unk114 = 0;
            }
            break;
    }
}

extern double sin(double);
void puniModeFunbari(PuniData* p) {
    switch (p->unk114) {
        case 0: {
            PuniTarget* t;
            int i;
            t = Punigp->targets;
            for (i = 0; i < 10; i++, t++) {
                if ((t->flags & 1) && (t->flags & 2)) break;
            }
            if (i >= 10) {
                p->unk114 = 10;
                break;
            }
            if (p->pos.x > t->pos.x) {
                FF(p, 0x1F4) = -1.0f;
                p->dir = 270.0f;
            } else {
                FF(p, 0x1F4) = 1.0f;
                p->dir = 90.0f;
            }
            p->vel.x = 0.5f;
            p->unk128 = 68;
            FF(p, 0x1EC) = 1.0f;
            p->npc->flags |= 4;
            p->unk114 = 1;
            break;
        }
        case 1:
        case 2:
            {
                f32 s = sin(3.1415927f * FF(p, 0x1EC) / 180.0f);
                if (FF(p, 0x1EC) <= 180.0f) {
                    FF(p, 0x1F0) = 0.5f * s;
                } else {
                    FF(p, 0x1F0) = 0.45f * s;
                }
            }
            FF(p, 0x1EC) += 7.0f;
            if (FF(p, 0x1EC) >= 360.0f) FF(p, 0x1EC) = 360.0f;
            p->pos.y += FF(p, 0x1F0);
            p->pos.x += p->vel.x * FF(p, 0x1F4);
            if (p->unk128-- == 0) p->unk114 = 10;
            break;
        case 10:
            {
                Vec v_ = {0.0f, 0.0f, 0.0f};
                p->vel = v_;
            }
            p->unkC = 17;
            p->anim = 0;
            p->npc->flags &= ~4;
            p->unk114 = 0;
            break;
    }
}

extern f32 intplGetValue(s32 mode, s32 currStep, s32 steps, f32 start, f32 end);
void puniModeWater(PuniData* p) {
    Vec d;
    switch (p->unk114) {
        case 0:
            p->unk4 |= 0x800;
            p->unk128 = 60;
            p->unk114 = 1;
        case 1:
            if (p->unk128-- == 0) {
                p->unk4 &= ~0x800;
                p->unk114 = 2;
            }
            break;
        case 2:
            *(u64*)((u8*)p + 0x1C8) = gp->renderTime;
            p->unk1F8 = p->pos;
            FF(p, 0x1EC) = -80.0f;
            FF(p, 0x1F0) = (FF(p, 0x154) - FF(p, 0x1FC)) - FF(p, 0x1EC);
            PSVECNormalize((Vec*)((u8*)p + 0x168), &d);
            PSVECScale(&d, &d, -20.0f);
            {
                f32 x = d.x + p->pos.x;
                f32 z = d.z + p->pos.z;
                FF(p, 0x150) = x;
                FF(p, 0x158) = z;
            }
            p->unk114 = 3;
        case 3: {
            u32 t = ((u32)gp->renderTime - F(p, 0x1CC)) / (*(u32*)0x800000F8 / 4 / 1000);
            if ((s32)t < 300) {
                f32 f = (f32)(s32)t / 300.0f;
                p->pos.x = intplGetValue(0, t, 300, FF(p, 0x1F8), FF(p, 0x150));
                p->pos.z = intplGetValue(0, t, 300, FF(p, 0x200), FF(p, 0x158));
                p->pos.y = f * (FF(p, 0x1EC) * f + FF(p, 0x1F0)) + FF(p, 0x1FC);
                p->unk138 = p->pos;
            } else {
                p->pos = *(Vec*)((u8*)p + 0x150);
                p->unk138 = p->pos;
                p->unk114 = 4;
            }
            break;
        }
        case 4:
            p->unkC = 16;
            p->anim = 0;
            p->unk114 = 0;
            break;
    }
}

void puniModePrehagure(PuniData* p) {
    Vec nrm;
    Vec hit;
    Vec dir;
    Vec start;
    f32 dist;
    s32 res;
    f32 b, a;
    switch (p->unk114) {
        case 0:
            res = 0;
            a = FF(Punigp, 0x350);
            b = FF(Punigp, 0x354);
            if (-1000.0f != a && p->pos.y < a && p->pos.y > b) {
                res = 1;
            }
            a = FF(Punigp, 0x358);
            b = FF(Punigp, 0x35C);
            if (-1000.0f != a && p->pos.y < a && p->pos.y > b) {
                res = 1;
            }
            if (res == 0) {
                res = 1;
            } else {
                start = p->pos;
                start.y += 1.0f;
                dir = puni_fall_dir;
                dist = 3.0f;
                if (hitCheckFilter(NULL, &hit.x, &hit.y, &hit.z, &dist, &nrm.x, &nrm.y, &nrm.z, start.x, start.y, start.z, dir.x, dir.y, dir.z) == NULL) {
                    res = 2;
                } else {
                    res = 0;
                }
            }
            if (res != 0) {
                *(s32*)&FF(p, 0x58C) = res;
                *(Vec*)&FF(p, 0x590) = p->pos;
                p->pos = Punigp->unk344;
                p->pos.x += randf(0.0f, 2.0f) - 1.0f;
                p->pos.z += randf(0.0f, 2.0f) - 1.0f;
                p->unk138 = p->pos;
                *(Vec*)&FF(p, 0x59C) = p->pos;
            }
            p->unk114 = 1;
            break;
        case 1:
            p->unkC = 16;
            p->unk114 = 0;
            break;
    }
}

extern EffectEntry* effFukidashiEntry(s32 a, s32 b, void* c, s32 d, f32 e, f32 f, f32 g, f32 h, f32 i);

void puniModeHagure(PuniData* p) {
    switch (p->unk114) {
        case 0:
            p->effect = effFukidashiEntry(4, 0, (u8*)p + 0x220, -1, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
            p->anim = 6;
            p->unk4 |= 0x100000;
            p->unk114 = 1;
        case 1:
            p->unk1E0 = rand() % 21 + 30;
            F(p, 0x1E4) = rand() % 6 + 5;
            p->unk128 = p->unk1E0;
            F(p, 0x1E8) = F(p, 0x1E4);
            p->unk114 = 10;
            FF(p, 0x2AC) = 0.0f;
            FF(p, 0x2B0) = -1000.0f;
            FF(p, 0x2B4) = 0.0f;
        case 10:
            if (p->unk128-- == 0) {
                if (F(p, 0x1E8) & 1) {
                    p->dir = 90.0f;
                } else {
                    p->dir = 270.0f;
                }
                if (F(p, 0x1E8)-- == 0) {
                    p->unk114 = 20;
                } else {
                    p->unk128 = p->unk1E0;
                }
            }
            break;
        case 20:
            p->unk128 = 30;
            p->unk114 = 21;
        case 21:
            if (p->unk128-- == 0) {
                p->unk114 = 30;
            }
            break;
        case 30:
            if (rand() % 2) {
                FF(p, 0x2AC) = p->pos.x;
                FF(p, 0x2B0) = 10.0f + p->pos.y;
                FF(p, 0x2B4) = p->pos.z;
                *(s32*)(*(u8**)((u8*)p->effect + 0xC) + 0x34) = 0;
            }
            p->unk128 = rand() % 41 + 100;
            p->unk114 = 31;
        case 31:
            if (p->unk128-- == 0) {
                p->unk114 = 1;
            }
            break;
    }
    if (p->unk1BC != -1 && p->unk1C0 < 10000.0f) {
        Punigp->unk43C = 2;
        p->unk114 = 0;
        if (p->effect != NULL) {
            effDelete(p->effect);
            p->effect = NULL;
        }
    } else if (p->unk1B4 != -1) {
        PuniDokuritsu(p);
        p->unkC = 4;
        p->unk114 = 0;
        if (p->effect != NULL) {
            effDelete(p->effect);
            p->effect = NULL;
        }
    } else if (p->unk1A0 > 10.0f) {
        PuniDokuritsu(p);
        p->unk1DC = Punigp->unk360;
        p->unkC = 13;
        p->unk114 = 0;
        if (p->effect != NULL) {
            effDelete(p->effect);
            p->effect = NULL;
        }
    } else if (p->unk1A4 < 150.0f) {
        p->unkC = 1;
        puniFukki(p);
        if (p->effect != NULL) {
            effDelete(p->effect);
            p->effect = NULL;
        }
    }
}

void puniModeDodai(PuniData* p) {
    Vec v;

    switch (p->unk114) {
        case 0:
            PSVECSubtract(&p->pos, &p->unk1F8, &v);
            if (PSVECMag(&v) < 10.0f) {
                {
                    Vec zero = {0.0f, 0.0f, 0.0f};
                    p->vel = zero;
                }
                p->unkC = 16;
                p->unk114 = 0;
            } else {
                PSVECNormalize(&v, &v);
                v.y = 0.0f;
                PSVECScale(&v, &v, 1.0f);
                PSVECSubtract(&p->vel, &v, &p->vel);
                p->unk174 = 3.0f;
            }
            break;
    }
}

void puniModeTrap_sub(PuniData* p) {
    NpcEntry* n;
    if (p->unk4 & 0x20) {
        n = npcNameToPtr("\203K\203C\203h");
        p->unk4 &= ~0x800;
        p->unk4 |= 0x100000;
        n->flags &= ~0x20;
        n->position.x = 0.0f;
        n->position.y = -1000.0f;
        n->position.z = 0.0f;
    }
    if (p->unk4 & 0x40) {
        n = npcNameToPtr("\203K\203C\203h\226\205");
        p->unk4 &= ~0x800;
        p->unk4 |= 0x100000;
        n->flags &= ~0x20;
        n->position.x = 0.0f;
        n->position.y = -1000.0f;
        n->position.z = 0.0f;
    }
    if (p->unk4 & 0x100) {
        n = npcNameToPtr("\203\211\203C\203o\203\213");
        p->unk4 &= ~0x800;
        p->unk4 |= 0x100000;
        n->flags &= ~0x20;
        n->position.x = 0.0f;
        n->position.y = -1000.0f;
        n->position.z = 0.0f;
    }
}

#define TRAP_NPC(name, setpos)                \
    {                                         \
        n = npcNameToPtr(name);               \
        p->unk4 |= 0x800;                     \
        n->position.x = p->pos.x;             \
        n->position.y = p->pos.y;             \
        n->position.z = p->pos.z;             \
        n->direction = p->dir;                   \
        n->flags |= 0x20;                     \
    }
#define TRAP_HIDE(name)                       \
    {                                         \
        n = npcNameToPtr(name);               \
        p->unk4 &= ~0x800;                    \
        p->unk4 |= 0x100000;                  \
        n->flags &= ~0x20;                    \
        n->position.x = 0.0f;                 \
        n->position.y = -1000.0f;             \
        n->position.z = 0.0f;                 \
    }


void puniModeTrap(PuniData* p) {
    NpcEntry* n;
    if (p->unk114 == 0) {
        p->unk4 |= 0x100000;
        if (p->unk4 & 0x20) TRAP_NPC("\203K\203C\203h", 0)
        if (p->unk4 & 0x40) TRAP_NPC("\203K\203C\203h\226\205", 0)
        if (p->unk4 & 0x100) TRAP_NPC("\203\211\203C\203o\203\213", 0)
        {
            Vec v_ = {0.0f, 0.0f, 0.0f};
            p->vel = v_;
        }
        p->unkE = p->unkC;
    }
    if (p->unk1A0 > 10.0f) {
        if (p->unk4 & 0x20) TRAP_HIDE("\203K\203C\203h")
        if (p->unk4 & 0x40) TRAP_HIDE("\203K\203C\203h\226\205")
        if (p->unk4 & 0x100) TRAP_HIDE("\203\211\203C\203o\203\213")
        PuniDokuritsu(p);
        p->unk1DC = Punigp->unk360;
        p->unkE = p->unkC;
        p->unkC = 13;
        p->unk114 = 0;
    } else if (p->unk1B4 != -1) {
        if (p->unk4 & 0x20) TRAP_HIDE("\203K\203C\203h")
        if (p->unk4 & 0x40) TRAP_HIDE("\203K\203C\203h\226\205")
        if (p->unk4 & 0x100) TRAP_HIDE("\203\211\203C\203o\203\213")
        PuniDokuritsu(p);
        p->unkE = p->unkC;
        p->unkC = 4;
        p->unk114 = 0;
    }
}

void puniModeEnterSubInit(PuniData* p) {
    if (Punigp->unk324 & 3) {
        p->unk1E0 = 0;
        p->alpha = 0.0f;
        p->npc->flags |= 4;
    }
}

void puniModeEnterSub(PuniData* p) {
    if (Punigp->unk324 & 3) {
        p->unk1E0++;
        if (p->unk1E0 >= 10) {
            p->alpha += 0.1f;
            if (p->alpha >= 1.0f) {
                p->alpha = 1.0f;
            }
        }
    }
}

void puniModeEnter(PuniData* p) {
    Vec d;
    switch (p->unk114) {
        case 0:
            {
                *(OSTime*)((u8*)p + 0x1C8) = gp->renderTime;
            }
            p->unk128 = 200;
            if ((Punigp->unk324 & 8) && !(Punigp->unk324 & 0x10)) {
                p->unk128 = 700;
            }
            p->unk128 += qrnd(500);
            if (Punigp->unk324 & 3) {
                p->unk1E0 = 0;
                p->alpha = 0.0f;
                p->npc->flags |= 4;
            }
            p->unk114 = 1;
        case 1: {
            u32 t = (((u32*)gp)[0x3C / 4] - F(p, 0x1CC)) / (*(u32*)0x800000F8 / 4 / 1000);
            if ((s32)t > p->unk128) {
                p->unk114 = 2;
            }
            break;
        }
        case 2:
            if (Punigp->unk324 & 3) {
                p->unk1E0++;
                if (p->unk1E0 >= 10) {
                    p->alpha += 0.1f;
                    if (p->alpha >= 1.0f) {
                        p->alpha = 1.0f;
                    }
                }
            }
            if (PSVECSquareDistance(&p->unk1F8, &p->pos) > 100.0f) {
                PSVECSubtract(&p->unk1F8, &p->pos, &d);
                PSVECNormalize(&d, &d);
                d.y = 0.0f;
                PSVECScale(&d, &d, 1.0f);
                PSVECAdd(&p->vel, &d, &p->vel);
            } else {
                p->unk4 |= 0x1000;
                {
                    Vec puni_enter_dir = {0.0f, 0.0f, 0.0f};
                    p->vel = puni_enter_dir;
                }
                p->alpha = 1.0f;
                p->npc->flags &= ~4;
                p->unk114 = 3;
            }
            break;
        case 3: {
            searchParent(p);
            p->unkC = 2;
            p->unk4 |= 0x180000;
            PSVECSubtract(&p->parent->pos, &p->pos, &d);
            p->unk1B0 = p->unk1A8 = PSVECMag(&d);
            p->unk128 = 0;
            p->unk114 = 0;
            p->anim = 1;
            break;
        }
    }
}

void puniModeExitSubInit(PuniData* p) {
    if (Punigp->unk324 & 3) {
        p->unk1E0 = 0;
        p->npc->flags |= 4;
    }
}

void puniModeExitSub(PuniData* p) {
    if (Punigp->unk324 & 3) {
        p->unk1E0++;
        if (p->unk1E0 >= 10) {
            p->alpha -= 0.3f;
            if (p->alpha <= 0.0f) {
                p->alpha = 0.0f;
            }
        }
    }
}

void puniModeExit(PuniData* p) {
    Vec d;
    switch (p->unk114) {
        case 0:
            if (PSVECSquareDistance((Vec*)((u8*)Punigp + 0x480), &p->pos) > *(f32*)((u8*)Punigp + 0x4B0)) {
                PSVECSubtract((Vec*)((u8*)Punigp + 0x480), &p->pos, &d);
                PSVECNormalize(&d, &d);
                d.y = 0.0f;
                PSVECScale(&d, &d, 1.5f);
                PSVECAdd(&p->vel, &d, &p->vel);
            } else {
                {
                    Vec puniModeExit_v0 = {0.0f, 0.0f, 0.0f};
                    p->vel = puniModeExit_v0;
                }
                p->unk4 &= ~0x100000 & ~0x1000;
                if (Punigp->unk324 & 3) {
                    p->unk1E0 = 0;
                    p->npc->flags |= 4;
                }
                p->unk114 = 1;
            }
            break;
        case 1:
            if (Punigp->unk324 & 3) {
                p->unk1E0++;
                if (p->unk1E0 >= 10) {
                    p->alpha -= 0.3f;
                    if (p->alpha <= 0.0f) {
                        p->alpha = 0.0f;
                    }
                }
            }
            if (PSVECSquareDistance((Vec*)((u8*)Punigp + 0x48C), &p->pos) > *(f32*)((u8*)Punigp + 0x4B4)) {
                PSVECSubtract((Vec*)((u8*)Punigp + 0x48C), &p->pos, &d);
                PSVECNormalize(&d, &d);
                d.y = 0.0f;
                PSVECScale(&d, &d, 2.0f);
                PSVECAdd(&p->vel, &d, &p->vel);
            } else {
                p->unkC = 30;
                p->unk4 &= ~0x100000;
                {
                    Vec puniModeExit_v1 = {0.0f, 0.0f, 0.0f};
                    p->vel = puniModeExit_v1;
                }
                {
                    Vec puniModeExit_v2 = {0.0f, -1000.0f, 0.0f};
                    p->pos = puniModeExit_v2;
                }
                p->unk138 = p->pos;
                p->unk114 = 0;
            }
            break;
        case 2:
        default:
            break;
    }
}

void puniModePoint(PuniData* p) {
    Vec d;
    switch (p->unk114) {
        case 0:
            if (PSVECSquareDistance((Vec*)((u8*)Punigp + 0x480), &p->pos) > *(f32*)((u8*)Punigp + 0x4B0)) {
                PSVECSubtract((Vec*)((u8*)Punigp + 0x480), &p->pos, &d);
                PSVECNormalize(&d, &d);
                d.y = 0.0f;
                PSVECScale(&d, &d, 1.5f);
                PSVECAdd(&p->vel, &d, &p->vel);
            } else {
                p->vel = (Vec){0.0f, 0.0f, 0.0f};
                p->unk4 &= ~0x100000 & ~0x1000;
                p->unk114 = 1;
            }
            break;
        case 1:
            if (PSVECSquareDistance((Vec*)((u8*)Punigp + 0x48C), &p->pos) > *(f32*)((u8*)Punigp + 0x4B4)) {
                PSVECSubtract((Vec*)((u8*)Punigp + 0x48C), &p->pos, &d);
                PSVECNormalize(&d, &d);
                d.y = 0.0f;
                PSVECScale(&d, &d, 2.0f);
                PSVECAdd(&p->vel, &d, &p->vel);
            } else {
                p->unkC = 30;
                p->unk4 &= ~0x100000;
                p->vel = (Vec){0.0f, 0.0f, 0.0f};
                p->pos = (Vec){0.0f, -1000.0f, 0.0f};
                p->unk138 = p->pos;
                p->unk114 = 0;
            }
            break;
        case 2:
            break;
    }
}

void puniModeDokan(PuniData* p) {
    p->unk114 = 0;
    p->unk4 |= 0x800;
    p->npc->flags |= 4;
    {
        Vec v = {0.0f, 0.0f, 0.0f};
        p->vel = v;
    }
}

void puniModeCustom(PuniData* p) {
    if (p->modeFunc) {
        p->modeFunc(p);
    }
}

void puni_puni_disp(CameraId camId, void* param) {
    MapTevCallbackInfo info;
    s32 off;
    MapTevCallbackInfo* ip;
    s32 i;
    PunidrvPose* pose;

    puniLoadRenderMode();
    puniLoadVertex();
    puniLoadTexture();
    puniLoadTev();
    if (!_swGet(3)) {
        puniDraw();
    } else {
        for (ip = &info, i = 0, off = 0; i < puniGetPoseNum(); i++, off += sizeof(PunidrvPose)) {
            pose = (PunidrvPose*)((u8*)puniGetPosePtr() + off);
            ip->tevStage = 1;
            ip->texCoord = 1;
            ip->texMap = 1;
            ip->field_0x18 = 0;
            ip->mtx = *pose->mtx;
            envSetYami(ip);
            GXSetNumTexGens(ip->texMap);
            GXSetNumTevStages(ip->tevStage);
            GXSetNumIndStages(ip->field_0x18);
            puniPoseDraw(i);
        }
    }
    puniLoadTextureExit();
}

static u8 shadow_dl[32] ATTRIBUTE_ALIGN(32) = {
    0x80, 0x00, 0x04, 0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03,
};
static f32 shadow_vtx[16] = {
    -1.2f, 0.0f, -1.2f, -1.2f, 0.0f, 1.2f, 1.2f, 0.0f, 1.2f, 1.2f, 0.0f, -1.2f,
};
static f32 shadow_coord[8] = { 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f };

void puni_puni_shadow_disp(CameraId camId, void* param) {
    static Mtx shadow_mt = { { 10.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 10.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 10.0f, 0.0f } };
    GXColor color;
    GXTexObj obj;
    Mtx mtx;
    CameraEntry* cam = camGetPtr(camId);
    PunidrvPose* pose = param;

    if (!(pose->flags & 4)) {
        static const GXColor c = { 0, 0, 0, 0xC0 };
        GXSetNumChans(1);
        GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_CLAMP, GX_AF_NONE);
        color = c;
        GXSetChanMatColor(GX_COLOR0A0, color);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
        effGetTexObjN64(0x2C, &obj);
        GXLoadTexObj(&obj, GX_TEXMAP0);
        GXSetNumTevStages(1);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
        GXClearVtxDesc();
        GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
        GXSetArray(GX_VA_POS, shadow_vtx, 12);
        GXSetArray(GX_VA_TEX0, shadow_coord, 8);
        GXSetCullMode(GX_CULL_NONE);
        PSMTXConcat(cam->view, *pose->mtx, mtx);
        PSMTXConcat(mtx, shadow_mt, mtx);
        if (pose->flags & 0x10) {
            PSMTXTransApply(mtx, mtx, 0.0f, 1.0f + (-(*pose->mtx)[1][3] + pose->shadowY), 0.0f);
        }
        GXLoadPosMtxImm(mtx, 0);
        GXSetCurrentMtx(0);
        GXCallDisplayList(shadow_dl, 0x20);
    }
}

void puni_puni_init(void) {
    s32 i;
    PuniData* p;
    MarioWork* mario;
    PunidrvPose* pose;
    f32 r;
    f32 lo = 0.0f;

    pose = puniGetPosePtr();
    mario = marioGetPtr();
    r = (f32)rand() / 32767.0f;
    Punigp->seed0 = (s32)(lo + r * (100000000.0f - lo)) + (u32)gp->renderTime + 0x3A3B01F9;
    Punigp->seed1 = 0x21708331;
    p = Punidat;
    Punigp->unk360 = (s32)p;
    p->unk4 |= 0x180404;
    p->pos = mario->position;
    p->unk138 = p->pos;
    p->unk190.x = 10.0f;
    p->unkC = 30;
    *(s16*)p->unk116 = 0;
    p->unk118 = 0;
    p = &Punidat[1];
    for (i = 1; i < 211; i++) {
        pose->flags = 1;
        pose->flags |= 2;
        pose->dir = 0.0f;
        PSMTXIdentity(*pose->mtx);
        (*pose->mtx)[0][3] = 0.0f;
        (*pose->mtx)[1][3] = -1000.0f;
        (*pose->mtx)[2][3] = 0.0f;
        pose->offsetY = 0.0f;
        p->npc = (NpcEntry*)pose;
        pose++;
        {
            Vec zero = {0.0f, 0.0f, 0.0f};
            *(Vec*)p->unk144 = zero;
        }
        p->dir = 0.0f;
        p->unk188 = 0.0f;
        p->alpha = 1.0f;
        *(f32*)p->unk19C = 10.0f;
        p->unk4 = 0;
        *(s16*)p->unk116 = i;
        p->unk118 = -1;
        p->unk174 = 3.0f;
        p++;
    }
    Punigp->showPuniCounter = 1;
    Punigp->puniCounterAlpha = 0xFF;
    Punigp->unk448 = 0;
    Punimap = _mapAlloc(0x24);
    memset(Punimap, 0, 0x24);
}

void puniModePiyori(PuniData* p);
void puniModeEscape(PuniData* p);
void puniModeHagure(PuniData* p);
void puniModeFunbari(PuniData* p);
void puniModeWater(PuniData* p);
void puniModeTrap(PuniData* p);
void puniModeEnter(PuniData* p);
void puniModeExit(PuniData* p);
void puniModeSearch(PuniData* p);
void puniModeParent(PuniData* p);
void puniModeMass(PuniData* p);
void puniModeAwayjump(PuniData* p);
void puniModeAway(PuniData* p);
void puniModeDamage(PuniData* p);
void puniModeRecover(PuniData* p);
void puniModeWind(PuniData* p);
void puniModeFall(PuniData* p);
void puniModeUpdraft(PuniData* p);


void unk_mri_00012d58(PuniData* p) {
    switch (p->unkC) {
        case 0:
            p->unk4 &= ~0x8000;
            if (p->unk1A0 > 10.0f) {
                p->unkE = p->unkC;
                PuniDokuritsu(p);
                p->unk1DC = Punigp->unk360;
                p->unkC = 13;
                p->unk114 = 0;
            } else if (p->unk1A4 < 150.0f) {
                p->unkC = 1;
            }
            break;
        case 1: {
            Vec d;
            p->unk4 &= ~0x8000;
            searchParent(p);
            p->unkC = 2;
            p->unk4 |= 0x180000;
            PSVECSubtract(&p->parent->pos, &p->pos, &d);
            {
                f32 m = PSVECMag(&d);
                p->unk1A8 = m;
                p->unk1B0 = m;
            }
            p->unk128 = 0;
            p->unk114 = 0;
            p->anim = 1;
            break;
        }
        case 2:
            p->unk4 &= ~0x8000;
            puniModeParent(p);
            break;
        case 3:
            p->unk4 &= ~0x8000;
            puniModeSearch(p);
            break;
        case 4:
            p->unk4 |= 0x8000;
            puniModeMass(p);
            break;
        case 5:
            p->unk4 |= 0x8000;
            puniModeAwayjump(p);
            break;
        case 6:
            p->unk4 |= 0x8000;
            puniModeAway(p);
            break;
        case 7:
            p->unk4 &= ~0x8000;
            puniModeAttack(p);
            break;
        case 9:
            p->unk4 &= ~0x8000;
            puniModeDamage(p);
            break;
        case 10:
            p->unk4 &= ~0x8000;
            puniModePiyori(p);
            break;
        case 8:
            p->unk4 &= ~0x8000;
            puniModeEscape(p);
            break;
        case 11:
            p->unk4 &= ~0x8000;
            puniModeRecover(p);
            break;
        case 12:
            p->unk4 &= ~0x8000;
            puniModeDead(p);
            break;
        case 13:
            p->unk4 &= ~0x8000;
            puniModeWind(p);
            break;
        case 14:
            break;
        case 15:
            p->unk4 |= 0x8000;
            switch (p->unk114) {
                case 0: {
                    s32 r = puniPositionCheck(p);
                    if (r != 0) {
                        *(s32*)((u8*)p + 0x58C) = r;
                        *(Vec*)((u8*)p + 0x590) = p->pos;
                        puniPositionRecover(p);
                        *(Vec*)((u8*)p + 0x59C) = p->pos;
                    }
                    p->unk114 = 1;
                    break;
                }
                case 1:
                    p->unkC = 16;
                    p->unk114 = 0;
                    break;
            }
            break;
        case 16:
            p->unk4 |= 0x8000;
            puniModeHagure(p);
            break;
        case 18:
            p->unk4 |= 0x8000;
            puniModePreupdraft(p);
            break;
        case 19:
            p->unk4 |= 0x8000;
            puniModeUpdraft(p);
            break;
        case 20:
            p->unk4 |= 0x8000;
            puniModeWonder(p);
            break;
        case 21:
            p->unk4 |= 0x8000;
            puniModeFunbari(p);
            break;
        case 22:
            p->unk4 |= 0x8000;
            puniModeWater(p);
            break;
        case 17:
            p->unk4 |= 0x8000;
            puniModeFall(p);
            break;
        case 23:
            p->unk4 |= 0x8000;
            puniModeDodai(p);
            break;
        case 24:
            p->unk4 &= ~0x8000;
            puniModeTrap(p);
            break;
        case 25:
            p->unk4 &= ~0x8000;
            puniModeEnter(p);
            break;
        case 26:
            p->unk4 &= ~0x8000;
            puniModeExit(p);
            break;
        case 28:
            p->unk4 &= ~0x8000;
            puniModeDokan(p);
            break;
        case 27:
            p->unk4 &= ~0x8000;
            puniModePoint(p);
            break;
        case 29:
            p->unk4 &= ~0x8000;
            if (p->modeFunc != NULL) {
                p->modeFunc(p);
            }
            break;
    }
}

#define PUNI_MOVE_13A90(chk, refl, gp) {\
    Vec s2 = start;\
    Vec d2 = dir;\
    Vec hit;\
    Vec old;\
    Vec diff;\
    Vec rv;\
    f32 d;\
    f32 nx, ny, nz;\
    *(void**)((u8*)p + 0x214) = NULL;\
    *(void**)((u8*)p + 0x210) = NULL;\
    d = mag;\
    if (!chk || (p->unk4 & 0x1000)) {\
        *(void**)((u8*)p + 0x210) = hitCheckFilter(NULL, &hit.x, &hit.y, &hit.z, &d, &nx, &ny, &nz, s2.x, s2.y, s2.z, d2.x, d2.y, d2.z);\
    }\
    if (*(void**)((u8*)p + 0x210) == NULL) {\
        PSVECAdd(&p->pos, &p->vel, &p->pos);\
    } else if (!gp || Punigp->unk328 != 18) {\
        old = p->pos;\
        *(Vec*)((u8*)p + 0x560) = hit;\
        PSVECAdd(&p->pos, &p->vel, &p->pos);\
        if (p->vel.y == 0.0f) {\
            hit.y = 0.0f;\
        }\
        PSVECNormalize(&hit, &hit);\
        if (refl) {\
            C_VECReflect(&d2, &hit, &rv);\
            PSVECScale(&rv, &p->vel, mag);\
        }\
        PSVECScale(&hit, &hit, -(mag - d) * PSVECDotProduct(&d2, &hit) + 0.1f);\
        PSVECAdd(&p->pos, &hit, &p->pos);\
        PSVECSubtract(&p->pos, &old, &diff);\
        d = PSVECMag(&diff);\
        if (d > 0.0f) {\
            PSVECNormalize(&diff, &diff);\
            *(void**)((u8*)p + 0x214) = hitCheckFilter(NULL, &hit.x, &hit.y, &hit.z, &d, &nx, &ny, &nz, old.x, old.y + 1.0f, old.z, diff.x, diff.y, diff.z);\
            if (*(void**)((u8*)p + 0x214) != NULL) {\
                p->pos = old;\
            }\
        }\
    }\
}

void fn_19_13A90(PuniData* p) {
    switch (p->unkC) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 7:
        case 8:
        case 10:
        case 14:
        case 16:
        case 23:
        case 25:
        case 26:
        case 27:
        {
            Vec start;
            Vec dir;
            Vec s2;
            Vec d2;
            Vec old;
            Vec refl;
            Vec diff;
            Vec hit;
            Vec pa;
            f32 d;
            f32 c3, c2, c1;
            f32 b3, b2, b1;
            f32 dist = PSVECMag(&p->vel);
            if (!dist) {
                goto mv_end1;
            }
            if (dist > 1.0f) {
                if (p->vel.x < 0.0f) {
                    p->dir = 270.0f;
                } else {
                    p->dir = 90.0f;
                }
            }
            if (dist > p->unk174) {
                PSVECScale(&p->vel, &p->vel, p->unk174 / dist);
                dist = p->unk174;
            }
            start = p->pos;
            start.y += 1.0f;
            PSVECNormalize(&p->vel, &dir);
            s2 = start;
            d2 = dir;
            PF(p, 0x214) = NULL;
            PF(p, 0x210) = NULL;
            d = dist;
            if (p->unk4 & 0x1000) {
                PF(p, 0x210) = hitCheckFilter(NULL, &pa.x, &pa.y, &pa.z, &d, &hit.x, &hit.y, &hit.z, s2.x, s2.y, s2.z, d2.x, d2.y, d2.z);
            }
            if (PF(p, 0x210) == NULL) {
                PSVECAdd(&p->pos, &p->vel, &p->pos);
            } else {
                old = p->pos;
                *PV(p, 0x560) = hit;
                PSVECAdd(&p->pos, &p->vel, &p->pos);
                if (p->vel.y == 0.0f) {
                    hit.y = 0.0f;
                }
                PSVECNormalize(&hit, &hit);
                C_VECReflect(&d2, &hit, &refl);
                PSVECScale(&refl, &p->vel, dist);
                { f32 dot = PSVECDotProduct(&d2, &hit); PSVECScale(&hit, &hit, -(dist - d) * dot + 0.1f); }
                PSVECAdd(&p->pos, &hit, &p->pos);
                PSVECSubtract(&p->pos, &old, &diff);
                d = PSVECMag(&diff);
                if (d > 0.0f) {
                    PSVECNormalize(&diff, &diff);
                    PF(p, 0x214) = hitCheckFilter(NULL, &b1, &b2, &b3, &d, &c1, &c2, &c3, old.x, old.y + 1.0f, old.z, diff.x, diff.y, diff.z);
                    if (PF(p, 0x214) != NULL) {
                        p->pos = old;
                    }
                }
            }
            if (PF(p, 0x210) != NULL || PF(p, 0x214) != NULL) {
                p->unk4 |= 0x400000;
            } else {
                PSVECScale(&p->vel, &p->vel, 0.6f);
                p->unk4 &= ~0x400000;
            }
        }
        mv_end1:
            puniSetYPosition(p);
            break;
        case 4:
        {
            Vec start;
            Vec dir;
            Vec s2;
            Vec d2;
            Vec old;
            Vec diff;
            Vec hit;
            Vec pa;
            f32 d;
            f32 c3, c2, c1;
            f32 b3, b2, b1;
            f32 dist = PSVECMag(&p->vel);
            if (!dist) {
                goto mv_end2;
            }
            if (dist > p->unk174) {
                PSVECScale(&p->vel, &p->vel, p->unk174 / dist);
                dist = p->unk174;
            }
            start = p->pos;
            start.y += 1.0f;
            PSVECNormalize(&p->vel, &dir);
            d2 = dir;
            s2 = start;
            PF(p, 0x214) = NULL;
            PF(p, 0x210) = NULL;
            d = dist;
            PF(p, 0x210) = hitCheckFilter(NULL, &pa.x, &pa.y, &pa.z, &d, &hit.x, &hit.y, &hit.z, s2.x, s2.y, s2.z, d2.x, d2.y, d2.z);
            if (PF(p, 0x210) == NULL) {
                PSVECAdd(&p->pos, &p->vel, &p->pos);
            } else if (Punigp->unk328 != 0x12) {
                old = p->pos;
                *PV(p, 0x560) = hit;
                PSVECAdd(&p->pos, &p->vel, &p->pos);
                if (p->vel.y == 0.0f) {
                    hit.y = 0.0f;
                }
                PSVECNormalize(&hit, &hit);
                { f32 dp = PSVECDotProduct(&d2, &hit); PSVECScale(&hit, &hit, dp * -(dist - d) + 0.1f); }
                PSVECAdd(&p->pos, &hit, &p->pos);
                PSVECSubtract(&p->pos, &old, &diff);
                d = PSVECMag(&diff);
                if (d > 0.0f) {
                    PSVECNormalize(&diff, &diff);
                    PF(p, 0x214) = hitCheckFilter(NULL, &b1, &b2, &b3, &d, &c1, &c2, &c3, old.x, old.y + 1.0f, old.z, diff.x, diff.y, diff.z);
                    if (PF(p, 0x214) != NULL) {
                        p->pos = old;
                    }
                }
            }
            if (PF(p, 0x210) != NULL || PF(p, 0x214) != NULL) {
                p->unk4 |= 0x400000;
            } else {
                p->unk4 &= ~0x400000;
            }
        }
        mv_end2:
            puniSetYPosition(p);
            break;
        case 9:
        {
            Vec start;
            Vec dir;
            Vec s2;
            Vec d2;
            Vec old;
            Vec refl;
            Vec diff;
            Vec hit;
            Vec pa;
            f32 d;
            f32 c3, c2, c1;
            f32 b3, b2, b1;
            f32 dist = PSVECMag(&p->vel);
            if (!dist) {
                goto mv_end3;
            }
            start = p->pos;
            start.y += 1.0f;
            PSVECNormalize(&p->vel, &dir);
            s2 = start;
            d2 = dir;
            PF(p, 0x214) = NULL;
            PF(p, 0x210) = NULL;
            d = dist;
            if (p->unk4 & 0x1000) {
                PF(p, 0x210) = hitCheckFilter(NULL, &pa.x, &pa.y, &pa.z, &d, &hit.x, &hit.y, &hit.z, s2.x, s2.y, s2.z, d2.x, d2.y, d2.z);
            }
            if (PF(p, 0x210) == NULL) {
                PSVECAdd(&p->pos, &p->vel, &p->pos);
            } else {
                old = p->pos;
                *PV(p, 0x560) = hit;
                PSVECAdd(&p->pos, &p->vel, &p->pos);
                if (p->vel.y == 0.0f) {
                    hit.y = 0.0f;
                }
                PSVECNormalize(&hit, &hit);
                C_VECReflect(&d2, &hit, &refl);
                PSVECScale(&refl, &p->vel, dist);
                { f32 dp = PSVECDotProduct(&d2, &hit); PSVECScale(&hit, &hit, dp * -(dist - d) + 0.1f); }
                PSVECAdd(&p->pos, &hit, &p->pos);
                PSVECSubtract(&p->pos, &old, &diff);
                d = PSVECMag(&diff);
                if (d > 0.0f) {
                    PSVECNormalize(&diff, &diff);
                    PF(p, 0x214) = hitCheckFilter(NULL, &b1, &b2, &b3, &d, &c1, &c2, &c3, old.x, old.y + 1.0f, old.z, diff.x, diff.y, diff.z);
                    if (PF(p, 0x214) != NULL) {
                        p->pos = old;
                    }
                }
            }
            if (PF(p, 0x210) != NULL || PF(p, 0x214) != NULL) {
                p->unk4 |= 0x400000;
            } else {
                p->unk4 &= ~0x400000;
            }
        }
        mv_end3:
            puniSetYPosition(p);
            break;
        case 29:
        {
            if (p->moveFunc) {
                p->moveFunc(p);
            }
        }
            puniSetYPosition(p);
            break;
        case 5:
        case 6:
        case 13:
            puniSetYPosition(p);
            break;
        case 24:
        {
            Vec start;
            Vec dir;
            Vec s2;
            Vec d2;
            Vec old;
            Vec refl;
            Vec diff;
            Vec hit;
            Vec pa;
            f32 d;
            f32 c3, c2, c1;
            f32 b3, b2, b1;
            f32 dist = PSVECMag(&p->vel);
            if (!dist) {
                goto mv_end5;
            }
            if (dist > 1.0f) {
                if (p->vel.x < 0.0f) {
                    p->dir = 270.0f;
                } else {
                    p->dir = 90.0f;
                }
            }
            if (dist > p->unk174) {
                PSVECScale(&p->vel, &p->vel, p->unk174 / dist);
                dist = p->unk174;
            }
            start = p->pos;
            start.y += 1.0f;
            PSVECNormalize(&p->vel, &dir);
            s2 = start;
            d2 = dir;
            PF(p, 0x214) = NULL;
            PF(p, 0x210) = NULL;
            d = dist;
            if (p->unk4 & 0x1000) {
                PF(p, 0x210) = hitCheckFilter(NULL, &pa.x, &pa.y, &pa.z, &d, &hit.x, &hit.y, &hit.z, s2.x, s2.y, s2.z, d2.x, d2.y, d2.z);
            }
            if (PF(p, 0x210) == NULL) {
                PSVECAdd(&p->pos, &p->vel, &p->pos);
            } else {
                old = p->pos;
                *PV(p, 0x560) = hit;
                PSVECAdd(&p->pos, &p->vel, &p->pos);
                if (p->vel.y == 0.0f) {
                    hit.y = 0.0f;
                }
                PSVECNormalize(&hit, &hit);
                C_VECReflect(&d2, &hit, &refl);
                PSVECScale(&refl, &p->vel, dist);
                { f32 dot = PSVECDotProduct(&d2, &hit); PSVECScale(&hit, &hit, -(dist - d) * dot + 0.1f); }
                PSVECAdd(&p->pos, &hit, &p->pos);
                PSVECSubtract(&p->pos, &old, &diff);
                d = PSVECMag(&diff);
                if (d > 0.0f) {
                    PSVECNormalize(&diff, &diff);
                    PF(p, 0x214) = hitCheckFilter(NULL, &b1, &b2, &b3, &d, &c1, &c2, &c3, old.x, old.y + 1.0f, old.z, diff.x, diff.y, diff.z);
                    if (PF(p, 0x214) != NULL) {
                        p->pos = old;
                    }
                }
            }
            if (PF(p, 0x210) != NULL || PF(p, 0x214) != NULL) {
                p->unk4 |= 0x400000;
            } else {
                PSVECScale(&p->vel, &p->vel, 0.6f);
                p->unk4 &= ~0x400000;
            }
        }
        mv_end5:
            puniSetYPosition(p);
            break;
    }
}

#pragma push
#pragma dont_inline on
void puniOption(void) {
    PuniData* p;
    PuniData* q;
    PuniData* start;
    PuniData* end;
    Vec diff;
    f32 dist, r2, r;

    for (p = Punidat; p < Punidat + PUNI_DATA_MAX; p++) {
        p->unk4 &= ~0x20000000;
        if (!(p->unk4 & 0x400) || !(p->unk4 & 0x100000)) {
            continue;
        }
        if (p < Punidat + 110) {
            start = Punidat;
            end = Punidat + 109;
        } else {
            start = Punidat + 110;
            end = Punidat + 209;
        }
        for (q = start; q <= end; q++) {
            if (!(q->unk4 & 0x400) || !(q->unk4 & 0x100000) || p == q) {
                continue;
            }
            dist = PSVECDistance(&p->pos, &q->pos);
            r = p->unk190.x + q->unk190.x;
            r2 = p->unk190.z + (r + p->unk190.y);
            if (q->unk8 & 0x20) {
                if (dist < r) {
                    PSVECSubtract(&p->pos, &q->pos, &diff);
                    if (0.0f == PSVECMag(&diff)) {
                        diff.x = 1.0f;
                        dist = 1.0f;
                    }
                    PSVECNormalize(&diff, &diff);
                    PSVECScale(&diff, &diff, r - dist);
                    PSVECAdd(&p->vel, &diff, &p->vel);
                }
            } else if (dist <= r2) {
                if (dist < r2 - p->unk190.y) {
                    p->unk4 |= 0x20000000;
                }
                if (!(p->unk4 & 0x10000000) && dist < p->unk190.x + q->unk190.x) {
                    p->unk4 |= 0x10000000;
                }
                if (p->unk4 & 0x10000000) {
                    PSVECSubtract(&p->pos, &q->pos, &diff);
                    if (0.0f == PSVECMag(&diff)) {
                        diff.x = 1.0f;
                        dist = 1.0f;
                    }
                    PSVECNormalize(&diff, &diff);
                    diff.y = 0.0f;
                    PSVECScale(&diff, &diff, 0.85f * (0.3f * ((r2 - dist) / r2)) + 0.044999994f);
                    PSVECAdd(&p->vel, &diff, &p->vel);
                    PSVECSubtract(&q->vel, &diff, &q->vel);
                }
            }
        }
    }
    for (p = Punidat; p < Punidat + PUNI_DATA_MAX; p++) {
        if (!(p->unk4 & 0x400) || !(p->unk4 & 0x100000)) {
            continue;
        }
        if (!(p->unk4 & 0x20000000)) {
            p->unk4 &= ~0x10000000;
        }
        if (PSVECMag(&p->vel) >= 8.0f) {
            PSVECNormalize(&p->vel, &p->vel);
            PSVECScale(&p->vel, &p->vel, 8.0f);
        }
    }
}

void unk_mri_00014ad4(void) {
    PuniData* p;
    PuniData* q;
    PuniData* start;
    PuniData* end;
    Vec diff;
    f32 dist, r2, r;

    for (p = Punidat; p <= Punidat + 109; p++) {
        p->unk4 &= ~0x20000000;
        if (!(p->unk4 & 0x400) || !(p->unk4 & 0x100000)) {
            continue;
        }
        for (q = Punidat; q <= Punidat + 109; q++) {
            if (!(q->unk4 & 0x400) || !(q->unk4 & 0x100000) || p == q) {
                continue;
            }
            dist = PSVECDistance(&p->pos, &q->pos);
            r = p->unk190.x + q->unk190.x;
            r2 = p->unk190.z + (r + p->unk190.y);
            if (q->unk8 & 0x20) {
                if (dist < r) {
                    PSVECSubtract(&p->pos, &q->pos, &diff);
                    if (0.0f == PSVECMag(&diff)) {
                        diff.x = 1.0f;
                        dist = 1.0f;
                    }
                    PSVECNormalize(&diff, &diff);
                    PSVECScale(&diff, &diff, r - dist);
                    PSVECAdd(&p->vel, &diff, &p->vel);
                }
            } else if (dist <= r2) {
                if (dist < r2 - p->unk190.y) {
                    p->unk4 |= 0x20000000;
                }
                if (!(p->unk4 & 0x10000000) && dist < p->unk190.x + q->unk190.x) {
                    p->unk4 |= 0x10000000;
                }
                if (p->unk4 & 0x10000000) {
                    PSVECSubtract(&p->pos, &q->pos, &diff);
                    if (0.0f == PSVECMag(&diff)) {
                        diff.x = 1.0f;
                        dist = 1.0f;
                    }
                    PSVECNormalize(&diff, &diff);
                    diff.y = 0.0f;
                    PSVECScale(&diff, &diff, 0.85f * (0.3f * ((r2 - dist) / r2)) + 0.044999994f);
                    PSVECAdd(&p->vel, &diff, &p->vel);
                    PSVECSubtract(&q->vel, &diff, &q->vel);
                }
            }
        }
    }
    for (p = Punidat; p <= Punidat + 109; p++) {
        if (!(p->unk4 & 0x400) || !(p->unk4 & 0x100000)) {
            continue;
        }
        if (!(p->unk4 & 0x20000000)) {
            p->unk4 &= ~0x10000000;
        }
        if (PSVECMag(&p->vel) >= 8.0f) {
            PSVECNormalize(&p->vel, &p->vel);
            PSVECScale(&p->vel, &p->vel, 8.0f);
        }
    }
}
#pragma pop


void psndSFX_pos(s32 id, Vec* pos);
#define PG_S32(o) (*(s32*)((u8*)Punigp + (o)))
void puniSound(void)
{
  PuniData *p;
  PuniData *d;
  int i;
  int n;
  int special;
  int div;
  int k;
  int m;
  Vec pos;
  k = 0;
  special = 0;
  for (i = 1; i <= 102; i++)
  {
    p = &Punidat[i];
    if ((p->unk4 & 0x400) && (((*((u16 *) (((u8 *) p) + 0x11C))) == 1) || ((*((u16 *) (((u8 *) p) + 0x11C))) == 2)))
    {
      if (i == 3)
      {
        special++;
      }
      else
      {
        k++;
      }
    }
  }

  *((s32 *) (((u8 *) Punigp) + 0x45C)) = k;
  if (k == 0)
  {
    *((s32 *) (((u8 *) Punigp) + 0x460)) = 0;
    div = 5;
  }
  else
    if (k <= 2)
  {
    *((s32 *) (((u8 *) Punigp) + 0x460)) = 1;
    div = 5;
  }
  else
    if (k <= 11)
  {
    *((s32 *) (((u8 *) Punigp) + 0x460)) = 2;
    div = 4;
  }
  else
    if (k <= 30)
  {
    *((s32 *) (((u8 *) Punigp) + 0x460)) = 3;
    div = 3;
  }
  else
    if (k <= 50)
  {
    *((s32 *) (((u8 *) Punigp) + 0x460)) = 4;
    div = 2;
  }
  else
    if (k <= 70)
  {
    *((s32 *) (((u8 *) Punigp) + 0x460)) = 5;
    div = 2;
  }
  else
  {
    *((s32 *) (((u8 *) Punigp) + 0x460)) = 6;
    div = 2;
  }
  if (((*((s32 *) (((u8 *) Punigp) + 0x460))) != 0) && ((gp->retraceCount % div) == 0))
  {
    m = *((s32 *) (((u8 *) Punigp) + 0x45C));
    n = (rand() % m) + 1;
    k = 0;
    for (i = 1; i <= 102; i++)
    {
      p = &Punidat[i];
      if ((p->unk4 & 0x400) && (((*((u16 *) (((u8 *) p) + 0x11C))) == 1) || ((*((u16 *) (((u8 *) p) + 0x11C))) == 2)))
      {
        k++;
        if (k == n)
        {
          break;
        }
      }
    }

    pos = p->pos;
    m = *((s32 *) (((u8 *) Punigp) + 0x460));
    switch ((rand() % m) + 1)
    {
      case 1:
        psndSFXOn_3D("SFX_STG2_PUNI_MOVE2_1", &pos);
        break;

      case 2:
        psndSFXOn_3D("SFX_STG2_PUNI_MOVE2_2", &pos);
        break;

      case 3:
        psndSFXOn_3D("SFX_STG2_PUNI_MOVE2_3", &pos);
        break;

      case 4:
        psndSFXOn_3D("SFX_STG2_PUNI_MOVE2_4", &pos);
        break;

      case 5:
        psndSFXOn_3D("SFX_STG2_PUNI_MOVE2_5", &pos);
        break;

      case 6:
        psndSFXOn_3D("SFX_STG2_PUNI_MOVE2_6", &pos);
        break;

    }

  }
  d = Punidat;
  if (special != 0)
  {
    if (((u32) (*((s32 *) (((u8 *) Punigp) + 0x478)))) == 0)
    {
      *((s32 *) (((u8 *) Punigp) + 0x478)) = psndSFXOn_3D("SFX_STG2_TOGETTI_MOVE1", &d[3].pos);
    }
  }
  else
    if (((u32) (*((s32 *) (((u8 *) Punigp) + 0x478)))) != 0)
  {
    psndSFXOff(*((s32 *) (((u8 *) Punigp) + 0x478)));
    *((s32 *) (((u8 *) Punigp) + 0x478)) = 0;
  }
  if (((u32) (*((s32 *) (((u8 *) Punigp) + 0x478)))) != 0)
  {
    psndSFX_pos(*((s32 *) (((u8 *) Punigp) + 0x478)), &d[3].pos);
  }
  k = 0;
  for (i = 110; i < 210; i++)
  {
    p = &Punidat[i];
    if ((p->unk4 & 0x400) && (((*((u16 *) (((u8 *) p) + 0x11C))) == 1) || ((*((u16 *) (((u8 *) p) + 0x11C))) == 2)))
    {
      k++;
    }
  }

  *((s32 *) (((u8 *) Punigp) + 0x46C)) = k;
  if (k == 0)
  {
    *((s32 *) (((u8 *) Punigp) + 0x470)) = 0;
  }
  else
    if (k <= 2)
  {
    *((s32 *) (((u8 *) Punigp) + 0x470)) = 1;
  }
  else
    if (k <= 11)
  {
    *((s32 *) (((u8 *) Punigp) + 0x470)) = 2;
  }
  else
  {
    *((s32 *) (((u8 *) Punigp) + 0x470)) = 3;
  }
  if ((*((s32 *) (((u8 *) Punigp) + 0x470))) != (*((s32 *) (((u8 *) Punigp) + 0x474))))
  {
    if (((u32) (*((s32 *) (((u8 *) Punigp) + 0x468)))) != 0)
    {
      psndSFXOff(*((s32 *) (((u8 *) Punigp) + 0x468)));
    }
    switch (*((s32 *) (((u8 *) Punigp) + 0x470)))
    {
      case 1:
        *((s32 *) (((u8 *) Punigp) + 0x468)) = ((s32 (*)(const char *)) psndSFXOn)("SFX_STG2_TOGE_MOVE1");
        break;

      case 2:
        *((s32 *) (((u8 *) Punigp) + 0x468)) = ((s32 (*)(const char *)) psndSFXOn)("SFX_STG2_TOGE_MOVE2");
        break;

      case 3:
        *((s32 *) (((u8 *) Punigp) + 0x468)) = ((s32 (*)(const char *)) psndSFXOn)("SFX_STG2_TOGE_MOVE3");
        break;

    }

  }
  *((s32 *) (((u8 *) Punigp) + 0x474)) = *((s32 *) (((u8 *) Punigp) + 0x470));
  *((s32 *) (((u8 *) Punigp) + 0x4C0)) = 0;
  *((s32 *) (((u8 *) Punigp) + 0x4C4)) = 0;
}


f32 cloudGetBreathPower(Vec pos, f32 r);
extern double fabs(double);

void puni_puni_main(void) {
    PuniTarget* tg; s32 i, k, j;
    MarioWork* mario;
    PuniData* p;
    PuniData* t;
    PunidrvPose* pose;
    Vec diff;
    Vec v;

    mario = marioGetPtr();
    t = (PuniData*)Punigp->unk360;
    t->pos = mario->position;
    v = t->pos;
    puniMapFindbuff(&v);
    puniModeALLCheck();
    if (Punigp->unk324 & 0x20) {
        
        for (i = 1, p = Punidat + 1; i < 211; i++, p++) {
            PSVECSubtract(&p->pos, &p->unk138, &diff);
            p->unk1AC = PSVECMag(&diff);
            if (!(p->unk4 & 0x400)) continue;
            p->unk4 &= ~0x6000000;
            p->unk8 &= ~4;
            p->unk586 = 0;
            puniParaRecover(p);
        }
    }
    if (Punigp->unk324 & 0x20) {
        for (k = 0; k < 2; k++) {
            
            for (i = 1, p = Punidat + 1; i < 211; i++, p++) {
                if (!(p->unk4 & 0x400)) continue;
                if (k == 0) {
                    if (p->unkC != 7) continue;
                    p->unk4 |= 0x2000000;
                } else {
                    if (p->unk4 & 0x2000000) continue;
                    p->unk4 |= 0x2000000;
                }
                if (!(p->unk4 & 0x4000000)) {

                    p->unk4 |= 0x4000000;
                    if (p->unk8 & 0x10) {
                        p->unk8 &= ~0x10;
                        p->unkC = 12;
                        p->unk114 = 0;
                    }
                    {
                        PartyEntry* party = partyGetPtr(marioGetPartyId());
                        p->unk1A0 = 0.0f;
                        if (Punigp->unk4CC == 0 && party != NULL) {
                            p->unk1A0 = cloudGetBreathPower(p->pos, 10.0f);
                        }
                    }
                    if (32.0 < __fabs(p->pos.y - mario->position.y)) {
                        p->unk1A4 = 1000.0f;
                    } else {
                        p->unk1A4 = PSVECDistance(&p->pos, &mario->position);
                    }
                    tg = Punigp->targets;
                    p->unk1B4 = -1;
                    p->unk1B8 = 1e6f;
                    p->unk1BC = -1;
                    p->unk1C0 = 1e6f;
                    for (j = 0; j < 10; j++, tg++) {
                        if (tg->flags & 1) {
                            if (!(32.0 < __fabs(p->pos.y - tg->pos.y))) {
                                if (tg->flags & 4) {
                                    f32 d = PSVECSquareDistance(&p->pos, &tg->pos);
                                    if (p->unk1C0 > d) {
                                        p->unk1BC = j;
                                        p->unk1C0 = d;
                                    }
                                }
                                if (tg->flags & 2) {
                                    f32 d = PSVECSquareDistance(&p->pos, &tg->pos);
                                    if (p->unk1B8 > d) {
                                        p->unk1B4 = j;
                                        p->unk1B8 = d;
                                    }
                                }
                            }
                        }
                    }
                }
                unk_mri_00012d58(p);
            }
        }
        puniOption();
    } else {
        
        for (i = 1, p = Punidat + 1; i <= 109; i++, p++) {
            if (p->unk4 & 0x400) {
                PartyEntry* party;
                PuniTarget* tt;
                s32 jj;
                party = partyGetPtr(marioGetPartyId());

                p->unk1A0 = 0.0f;
                if (Punigp->unk4CC == 0 && party != NULL) {
                    p->unk1A0 = cloudGetBreathPower(p->pos, 10.0f);
                }
                if (32.0 < __fabs(p->pos.y - mario->position.y)) {
                    p->unk1A4 = 1000.0f;
                } else {
                    p->unk1A4 = PSVECDistance(&p->pos, &mario->position);
                }
                tt = Punigp->targets;
                p->unk1B4 = -1;
                p->unk1B8 = 1e6f;
                p->unk1BC = -1;
                p->unk1C0 = 1e6f;
                for (jj = 0; jj < 10; jj++, tt++) {
                    if (tt->flags & 1) {
                        if (!(32.0 < __fabs(p->pos.y - tt->pos.y))) {
                            if (tt->flags & 4) {
                                f32 d = PSVECSquareDistance(&p->pos, &tt->pos);
                                if (p->unk1C0 > d) {
                                    p->unk1BC = jj;
                                    p->unk1C0 = d;
                                }
                            }
                            if (tt->flags & 2) {
                                f32 d = PSVECSquareDistance(&p->pos, &tt->pos);
                                if (p->unk1B8 > d) {
                                    p->unk1B4 = jj;
                                    p->unk1B8 = d;
                                }
                            }
                        }
                    }
                }
                unk_mri_00012d58(p);
            }
        }
        unk_mri_00014ad4();
    }
    if (Punigp->unk324 & 0x20) {
        s32 ii; PuniData* q; PunidrvPose* ps; 
        ps = puniGetPosePtr();
        
        for (ii = 1, q = Punidat + 1; ii < 211; ii++, ps++, q++) {
            if (q->unk4 & 0x400) {
                if (!(q->unk4 & 0x200)) {
                    q->unk138 = q->pos;
                    fn_19_13A90(q);
                }
                puniAnimation(q, (PuniAnimTarget*)ps);
            }
        }
    } else {
        s32 ii; PuniData* q; PunidrvPose* ps; 
        ps = puniGetPosePtr();
        
        for (ii = 1, q = Punidat + 1; ii <= 109; ii++, ps++, q++) {
            if (q->unk4 & 0x400) {
                if (!(q->unk4 & 0x200)) {
                    q->unk138 = q->pos;
                    fn_19_13A90(q);
                }
                puniAnimation(q, (PuniAnimTarget*)ps);
            }
        }
    }
    puniSound();
    switch (Punigp->unk43C) {
        case 2:
            if (Punigp->unk440 == 2) Punigp->unk43C = 0;
            break;
        case 1: break;
        case 3: break;
    }
    Punigp->unk440 = Punigp->unk43C;
}

void puni_puni_dispent(void) {
    PunidrvPose* pose;
    s32 i;
    Vec v;
    Vec tmp;

    dispEntry(CAMERA_3D, 0.0f, 1, puni_puni_disp, NULL);
    pose = puniGetPosePtr();
    for (i = 0; i < 210; i++) {
        {
            Vec zero = {0.0f, 0.0f, 0.0f};
            tmp = zero;
        }
        tmp.x = (*pose->mtx)[0][3];
        tmp.y = (*pose->mtx)[1][3];
        tmp.z = (*pose->mtx)[2][3];
        v = tmp;
        dispEntry(CAMERA_3D, dispCalcZ(&v), 2, puni_puni_shadow_disp, pose);
        pose++;
    }
    dispEntry(CAMERA_3D, 0.0f, 1, evt_icon_disp_func, NULL);
}

USER_FUNC(evt_puni_mem_init) {
    Punigp = _mapAlloc(sizeof(PuniWork));
    memset(Punigp, 0, sizeof(PuniWork));
    Punidat = _mapAlloc(sizeof(PuniData) * PUNI_DATA_MAX);
    memset(Punidat, 0, sizeof(PuniData) * PUNI_DATA_MAX);
    Punitex = _mapAlloc(sizeof(PuniTexInfo) * PUNI_TEX_INFO_MAX);
    memset(Punitex, 0, sizeof(PuniTexInfo) * PUNI_TEX_INFO_MAX);
    return EVT_RETURN_DONE;
}

void puniAnimationInit2(void);
s32 evt_puni_init(EventEntry *event, BOOL isFirstCall)
{
  s32 *args = event->args;
  s32 a = evtGetValue(event, args[0]);
  PuniTexInfo *tex = Punitex;
  s32 b = evtGetValue(event, args[1]);
  s32 c = evtGetValue(event, args[2]);
  s32 d = evtGetValue(event, args[3]);
  s32 i;
  s16 *t;
  PuniAnimSet *a0;
  PuniAnimSet *a1;
  if (isFirstCall)
  {
    puniInit();
    return 0;
  }
  a0 = &Punigp->anim[0];
  a1 = &Punigp->anim[1];
  for (i = 0; i < 10; i++)
  {
    s32 n;
    a0->table[i] = (s16 *) puni_patern_table[i];
    t = (s16 *) puni_patern_table[i];
    for (n = 0;; n++)
    {
      s16 *e = &a0->end[i];
      s16 v = t[n];
      if (v == (-1))
      {
        *e = v;
        break;
      }
      if (v == (-2))
      {
        *e = v;
        break;
      }
    }

    a0->count[i] = n;
    a1->table[i] = (s16 *) toge_patern_table[i];
    t = (s16 *) toge_patern_table[i];
    for (n = 0;; n++)
    {
      s16 *e = &a1->end[i];
      s16 v = t[n];
      if (v == (-1))
      {
        *e = v;
        break;
      }
      if (v == (-2))
      {
        *e = v;
        break;
      }
    }

    a1->count[i] = n;
  }

  puniAnimationInit2();
  puniEntry(a, (PunidrvTexInfo *) tex, (void *) b, (void *) c, (void *) d);
  return 2;
}

USER_FUNC(evt_puni_set_yuka) {
    s32* args = event->args;
    Punigp->unk328 = evtGetValue(event, args[0]) + 1;
    Punigp->posY[0] = evtGetValue(event, args[1]);
    Punigp->posY[1] = evtGetValue(event, args[2]);
    Punigp->posY[2] = evtGetValue(event, args[3]);
    Punigp->posY[3] = evtGetValue(event, args[4]);
    Punigp->ymode = evtGetValue(event, args[5]);
    Punigp->mapKind = evtGetValue(event, args[6]);
    swByteSet(0x299, Punigp->unk328);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_set_hamari) {
    s32* args = event->args;
    f32* v = (f32*)((u8*)Punigp + 0x344);
    Punigp->unk344.x = evtGetValue(event, args[0]);
    Punigp->unk344.y = evtGetValue(event, args[1]);
    Punigp->unk344.z = evtGetValue(event, args[2]);
    *(f32*)((u8*)Punigp + 0x350) = evtGetValue(event, args[3]);
    *(f32*)((u8*)Punigp + 0x354) = evtGetValue(event, args[4]);
    *(f32*)((u8*)Punigp + 0x358) = evtGetValue(event, args[5]);
    *(f32*)((u8*)Punigp + 0x35C) = evtGetValue(event, args[6]);
    return EVT_RETURN_DONE;
}

#define puniMapDataSetI(w_, h_, src_, x_, z_)     do {         s32 w = (w_);         s32 h = (h_);                  Punimap->x = (x_);         Punimap->z = (z_);         if ((src_) == NULL) {             Punimap->enabled = 0;         } else {             Punimap->width = w;             Punimap->height = h;             Punimap->enabled = 1;             Punimap->src = (src_);             Punimap->unk1C = 0;             Punimap->size = h * (w * 4);             Punimap->data = _mapAlloc(Punimap->size);             memcpy(Punimap->src, (src_), Punimap->size);         }     } while (0)
extern u8 puniMapdata_MRI_06[];
extern u8 puniMapdata_MRI_09[];
extern u8 puniMapdata_MRI_09_01[];
extern u8 puniMapdata_MRI_16[];

USER_FUNC(evt_puni_set_maze) {
    switch (Punigp->mapKind) {
        case 0:
            break;
        case 1:
            puniMapDataSetI(13, 5, puniMapdata_MRI_06, 50.0f, -1.0f);
            break;
        case 2:
            if (swGet(0xB22)) {
                puniMapDataSetI(7, 6, puniMapdata_MRI_09_01, 150.0f, -1.0f);
            } else {
                puniMapDataSetI(7, 6, puniMapdata_MRI_09, 150.0f, -1.0f);
            }
            break;
        case 3:
            if (swGet(0xB33)) {
                puniMapDataSetI(11, 9, puniMapdata_MRI_16, -269.0f, -320.0f);
            }
            break;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_main) {
    puniMain();
    return EVT_RETURN_BLOCK;
}

f32 puni_init_posy(f32 y) {
    f32 best = Punigp->posY[0];
    f32 d = __fabs(y - best);
    f32 d2;

    d2 = __fabs(y - Punigp->posY[1]);
    if (d > d2) {
        d = d2;
        best = Punigp->posY[1];
    }
    d2 = __fabs(y - Punigp->posY[2]);
    if (d > d2) {
        d = d2;
        best = Punigp->posY[2];
    }
    d2 = __fabs(y - Punigp->posY[3]);
    if (d > d2) {
        best = Punigp->posY[3];
    }
    return best;
}

void puni_init_para(PuniData* p, f32 x, f32 y, f32 z) {
    p->unk114 = 0;
    p->anim = 0;
    p->unk11E = 1000;
    p->pos.x = x;
    p->pos.y = y;
    p->pos.z = z;
    p->unk138 = p->pos;
    p->unk190.x = 6.0f;
    p->unk190.y = 2.0f;
    p->unk190.z = 3.0f;
    p->hp = 60;
    p->maxHp = 60;
    p->regen = 10;
    p->regenTimer = 60;
    p->regenInterval = 60;
    p->unk57E = 20;
    p->unk580 = 10;
    p->unk582 = 10;
    p->damage = 0;
    p->unk586 = 0;
    p->unk588 = 50;
    p->unk58A = 70;
    p->parent = NULL;
    p->child1 = NULL;
    p->child2 = NULL;
    PuniDokuritsu(p);
    p->unk4 |= 0x1401;
}

void toge_init_para(PuniData* p, f32 x, f32 y, f32 z) {
    p->anim = 0;
    p->unk114 = 0;
    p->unk11E = 1000;
    p->pos.x = x;
    p->pos.y = y;
    p->pos.z = z;
    p->unk138 = p->pos;
    p->unk190.x = 6.0f;
    p->unk190.y = 2.0f;
    p->unk190.z = 3.0f;
    p->hp = 60;
    p->maxHp = 60;
    p->regen = 10;
    p->regenTimer = 60;
    p->regenInterval = 60;
    p->unk57E = 20;
    p->unk580 = 10;
    p->unk582 = 10;
    p->damage = 0;
    p->unk586 = 0;
    p->unk588 = 50;
    p->unk58A = 70;
    p->unk4 |= 0x101402;
}

USER_FUNC(evt_puni_rnd_pos) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 px = args[1];
    s32 py = args[2];
    s32 pz = args[3];
    f32 x = evtGetFloat(event, px);
    f32 y = evtGetFloat(event, py);
    f32 ry, rx, rz, oy;
    f32 d, d2;
    f32 z;
    ry = Punigp->posY[0];
    d = __fabs(y - ry);
    d2 = __fabs(y - Punigp->posY[1]);
    if (d > d2) {
        d = d2;
        ry = Punigp->posY[1];
    }
    d2 = __fabs(y - Punigp->posY[2]);
    if (d > d2) {
        d = d2;
        ry = Punigp->posY[2];
    }
    d2 = __fabs(y - Punigp->posY[3]);
    if (d > d2) {
        ry = Punigp->posY[3];
    }
    z = evtGetFloat(event, pz);
    switch (type) {
    case 0:
        rx = randf(x - 10.0f, x + 10.0f);
        oy = ry;
        rz = randf(z - 10.0f, z + 10.0f);
        break;
    case 1:
        rx = randf(x - 50.0f, x + 50.0f);
        oy = ry;
        rz = randf(z - 50.0f, z + 50.0f);
        break;
    }
    evtSetFloat(event, px, rx);
    evtSetFloat(event, py, oy);
    evtSetFloat(event, pz, rz);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_get_systemmode) {
    evtSetValue(event, event->args[0], Punigp->systemMode);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_set_systemmode) {
    Punigp->systemMode = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_wait_systemmode) {
    s32 mode = evtGetValue(event, event->args[0]);
    return Punigp->systemMode != mode ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

extern int sprintf(char*, const char*, ...);
static inline void puni_swget_idx(s32 idx, s32* val, Vec* pos) {
    s32 off;
    u8 b6, b5, b4, b3, b2, b1, b0;
    s32 x;
    s32 y;
    s32 z;

    off = (idx - 1) * 7;
    b0 = _swByteGet(off);
    b1 = _swByteGet(off + 1);
    b2 = _swByteGet(off + 2);
    b3 = _swByteGet(off + 3);
    b4 = _swByteGet(off + 4);
    b5 = _swByteGet(off + 5);
    b6 = _swByteGet(off + 6);

    *val = b0;
    x = b1 + (b2 << 8);
    if (x & 0x8000) {
        x |= 0xFFFF0000;
    }
    y = b3 + (b4 << 8);
    if (y & 0x8000) {
        y |= 0xFFFF0000;
    }
    z = b5 + (b6 << 8);
    if (z & 0x8000) {
        z |= 0xFFFF0000;
    }
    pos->x = x;
    pos->y = y;
    pos->z = z;
}

s32 evt_puni_debug_info(EventEntry *event, BOOL isFirstCall)
{
  char buf[256];
  s32 i;
  for (i = 1; i <= 102; i++)
  {
    s32 b0;
    Vec pos;
    s32 a;
    s32 trap;
    f32 fx;
    f32 fy;
    f32 fz;
    puni_swget_idx(i, &b0, &pos);
    fx = pos.x;
    fy = pos.y;
    fz = pos.z;
    if (i >= 103)
    {
      a = 0;
    }
    else
    {
      a = 8;
      trap = swByteGet((i / 8) + 0x28C);
      a = (trap >> (i % a)) & 1;
    }
    trap = 0;
    if (i == 1)
    {
      if (!swGet(0xB3C))
      {
        trap = 1;
      }
    }
    else
      if (i == 2)
    {
      if (!swGet(0xB3D))
      {
        trap = 1;
      }
    }
    else
      if (i == 4)
    {
      if (!swGet(0xB3F))
      {
        trap = 1;
      }
    }
    else
      if (i == 3)
    {
      if (!swGet(0xB3E))
      {
        trap = 1;
      }
    }
    else
      if ((swByteGet(0x28B) + 5) <= i)
    {
      trap = 1;
    }
    if (a == 0)
    {
      if (trap == 0)
      {
        if (b0 == 0)
        {
          sprintf(buf, "No%03d,���ė��Ă��܂�\n", i);
        }
        else
        {
          sprintf(buf, "No%03d,�͂���Ă��܂� area = mri_%02d, x = %04d, y = %04d, z = %04d,\n", i, b0 - 1, (s32) fx, (s32) fy, (s32) fz);
        }
      }
      else
      {
        sprintf(buf, "No%03d,���܂����ԂɂȂ��Ă��܂���\n", i);
      }
    }
    else
    {
      sprintf(buf, "No%03d,���g���b�v�ɂ������Ă��܂���\n", i);
    }
    if ((i == 1) && swGet(0xB14))
    {
      sprintf(buf, "No%03d,��mri_03�̘S���ɑ؍݂��Ă��܂���\n", i);
    }
  }

  return 2;
}

extern double fabs(double);
USER_FUNC(evt_puni_setup) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    s32 a0 = evtGetValue(event, args[0]);
    s32 a1;
    s32 i;
    PuniData* p;
    BeroEntry** bero;
    s32 n;
    evtGetValue(event, args[1]);
    a1 = evtGetValue(event, args[2]);

    Punigp->unk43C = 0;
    if (a0 == 2) {
        Punigp->unk324 &= ~0xF;
        switch (a1) {
            case 6:
                Punigp->unk324 |= 1;
                break;
            case 2:
                Punigp->unk324 |= 2;
                break;
            case 8:
                Punigp->unk324 |= 4;
                break;
            case 9:
                Punigp->unk324 |= 8;
                break;
        }
    }
    if (Punigp->unk328 == 9 || Punigp->unk328 == 12) {
        Punigp->unk324 |= 0x20;
    }
    if (swByteGet(0x299) == 1) {
        return EVT_RETURN_DONE;
    }
    p = &Punidat[1];
    for (i = 1; i <= 109; i++, p++) {
        s32 b0;
        Vec pos;
        f32 fx, fy, fz;
        s32 a;
        f32 sx, sy, sz;
        if (p->unk4 & 0x40000000) {
            p->unk4 &= ~0x40000000;
            continue;
        }
        if (i >= 103) {
            p->unk8 |= 0x20;
            continue;
        }
        puni_swget_idx(i, &b0, &pos);
        fx = pos.x;
        fy = pos.y;
        fz = pos.z;
        if (b0 != 0 && b0 != Punigp->unk328) continue;
        puniswTrapGetindex(i, &a);
        if (a != 0) continue;
        if (i == 1) {
            if (!swGet(0xB3C)) continue;
            if (swGet(0xB14)) continue;
            *(PuniData**)((u8*)Punigp + 0x364) = p;
            p->unk4 |= 0x20;
        } else if (i == 2) {
            if (!swGet(0xB3D)) continue;
            *(PuniData**)((u8*)Punigp + 0x368) = p;
            p->unk4 |= 0x40;
        } else if (i == 4) {
            if (!swGet(0xB3F)) continue;
            *(PuniData**)((u8*)Punigp + 0x370) = p;
            p->unk4 |= 0x100;
        } else if (i == 3) {
            if (!swGet(0xB3E)) continue;
            *(PuniData**)((u8*)Punigp + 0x36C) = p;
            p->unk4 |= 0x80;
        } else {
            if (swByteGet(0x28B) + 5 <= i) continue;
            p->unk4 |= 8;
        }
        if (b0 == 0) {
            if (gp->flags & 1) {
                sx = randf(mario->position.x - 10.0f, mario->position.x + 10.0f);
                sy = mario->position.y;
                sz = randf(mario->position.z - 10.0f, mario->position.z + 10.0f);
                p->unkC = 1;
            } else {
                f32 ex, ey, ez;
                f32 d, e;
                sx = bero_get_BeroSX();
                sy = puni_init_posy(bero_get_BeroSY());
                sz = bero_get_BeroSZ();
                ex = bero_get_BeroEX();
                ey = puni_init_posy(bero_get_BeroEY());
                ez = bero_get_BeroEZ();
                if (!(Punigp->unk324 & 8)) {
                    sx = randf(sx - 10.0f, sx + 10.0f);
                    sz = randf(sz - 10.0f, sz + 10.0f);
                } else {
                    ez += 60.0f;
                }
                p->unk1F8.x = randf(ex - 10.0f, ex + 10.0f);
                p->unk1F8.y = ey;
                p->unk1F8.z = randf(ez - 10.0f, ez + 10.0f);
                p->unkC = 25;
                p->unk4 &= ~0x1000;
            }
        } else {
            sx = fx;
            sy = fy;
            sz = fz;
            p->unkC = 15;
            p->unk4 |= 0x8000;
        }
        p->unk114 = 0;
        p->anim = 0;
        p->unk11E = 1000;
        p->pos.x = sx;
        p->pos.y = sy;
        p->pos.z = sz;
        p->unk138 = p->pos;
        p->unk190.x = 6.0f;
        p->unk190.y = 2.0f;
        p->unk190.z = 3.0f;
        p->hp = 60;
        p->maxHp = 60;
        p->regen = 10;
        p->regenTimer = 60;
        p->regenInterval = 60;
        p->unk57E = 20;
        p->unk580 = 10;
        p->unk582 = 10;
        p->damage = 0;
        p->unk586 = 0;
        p->unk588 = 50;
        p->unk58A = 70;
        p->parent = NULL;
        p->child1 = NULL;
        p->child2 = NULL;
        PuniDokuritsu(p);
        p->unk4 |= 0x1401;
    }
    bero = bero_get_ptr();
    n = bero_get_BeroNUM();
    {
    s32 j;
    for (j = 0; j < n; j++) {
        if (bero[j]->kinddirArg3 == 9) {
            PuniData* q = Punidat + j + 103;
            q->unk4 |= 0x100400;
            q->unkE = 28;
            q->unkC = 28;
            q->unk190.x = 25.0f;
            hitObjGetPos(bero[j]->hitName, &q->pos);
            q->pos.y = puni_init_posy(q->pos.y);
        }
    }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_toge_setup) {
    s32* args = event->args;
    s32 end = 1;
    s32 kind = evtGetValue(event, args[0]);
    f32 x = evtGetValue(event, args[1]);
    f32 y = evtGetValue(event, args[2]);
    f32 z = evtGetValue(event, args[3]);
    s32 i;
    PuniData* p;

    switch (kind) {
        case 0:
            return EVT_RETURN_DONE;
        case 1:
            end = 210;
            break;
        case 2:
            end = 120;
            break;
    }
    for (i = 110; i < end; i++) {
        f32 rx, rz;
        p = &Punidat[i];
        rx = randf(x - 50.0f, x + 50.0f);
        rz = randf(z - 120.0f, z + 120.0f);
        p->unk4 |= 0x10;
        p->unkC = 0;
        p->pos.x = rx;
        p->pos.y = y;
        p->pos.z = rz;
        *(s16*)&p->unk116[0] = i;
        p->anim = 0;
        p->unk114 = 0;
        p->unk11E = 1000;
        p->unk138 = p->pos;
        p->unk190.x = 6.0f;
        p->unk190.y = 2.0f;
        p->unk190.z = 3.0f;
        p->hp = 60;
        p->maxHp = 60;
        p->regen = 10;
        p->regenTimer = 60;
        p->regenInterval = 60;
        p->unk57E = 20;
        p->unk580 = 10;
        p->unk582 = 10;
        p->damage = 0;
        p->unk586 = 0;
        p->unk588 = 50;
        p->unk58A = 70;
        p->unk4 |= 0x101402;
    }
    return EVT_RETURN_DONE;
}

BOOL _puniVAtari(PuniData* p, Vec* from, Vec* to) {
    Vec start;
    Vec dir;
    f32 hz;
    f32 hy;
    f32 hx;
    f32 nz;
    f32 ny;
    f32 nx;
    f32 dist;

    PSVECSubtract(to, from, &dir);
    if (0.0f == dir.x && 0.0f == dir.z) {
        p->pos = *from;
        return TRUE;
    }
    start = *from;
    PSVECNormalize(&dir, &dir);
    dist = PSVECMag(&dir);
    if (hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, start.x, start.y, start.z, dir.x, dir.y, dir.z) ==
        NULL) {
        p->pos = *to;
        return TRUE;
    }
    return FALSE;
}

#define HAGURE_FLOOR(dst, v) do { (dst) = (s32)(v); if ((v) < 0.0f) { (dst)--; } } while (0)
void puniHagurePos(PuniData* p) {
    Vec cur;
    Vec a;
    Vec c;
    Vec b;
    Vec d;
    s32 ix;
    s32 iz;
    s32 ix1;
    s32 iz1;

    switch (p->unkC) {
        case 17:
        case 18:
        case 19:
        case 21:
        case 22:
            {
                Vec v_ = {0.0f, -1000.0f, 0.0f};
                p->pos = v_;
            }
            return;
        default:
            break;
    }
    cur = p->pos;
    HAGURE_FLOOR(ix, cur.x);
    ix1 = ix + 1;
    HAGURE_FLOOR(iz, cur.z);
    iz1 = iz + 1;
    a.x = ix;
    a.y = cur.y;
    a.z = iz;
    b.x = ix1;
    b.y = cur.y;
    b.z = iz1;
    c.x = ix1;
    c.y = cur.y;
    c.z = iz;
    d.x = ix;
    d.y = cur.y;
    d.z = iz1;
    if (_puniVAtari(p, &cur, &a)) return;
    if (_puniVAtari(p, &cur, &c)) return;
    if (_puniVAtari(p, &cur, &b)) return;
    if (_puniVAtari(p, &cur, &d)) return;
}

static const Vec exit_zero0 = {0.0f, 0.0f, 0.0f};
static const Vec exit_zero1 = {0.0f, 0.0f, 0.0f};
static inline void puni_exit_swset(s32 idx, s32 val, Vec* pos) {
    u8 yh, xh;
    s32 off;
    s32 x;
    s32 y;
    s32 z;
    off = (idx - 1) * 7;
    x = pos->x;
    y = pos->y;
    z = pos->z;
    xh = ((x & 0xFF00) >> 8) & 0xFFu;
    yh = (y & 0xFF00) >> 8;
    _swByteSet(off, val);
    _swByteSet(off + 1, x & 0xFF);
    _swByteSet(off + 2, xh);
    _swByteSet(off + 3, y & 0xFF);
    _swByteSet(off + 4, yh);
    _swByteSet(off + 5, z & 0xFF);
    _swByteSet(off + 6, (z & 0xFF00) >> 8);
}
#define EXIT_SWSET(idx, v, pv) (pos = (pv), puni_exit_swset((idx), (v), &pos))

#pragma push
#pragma opt_dead_assignments off
#pragma opt_lifetimes off
s32 evt_puni_exit(EventEntry *event, BOOL isFirstCall)
{
  s32 *args = event->args;
  s32 a0 = evtGetValue(event, args[0]);
  s32 a1;
  const char *name;
  f32 sx;
  f32 sy;
  f32 sz;
  f32 ex;
  f32 ey;
  f32 ez;
  s32 notMri00;
  s32 i;
  PuniData *p;
  Vec pos;
  Vec pos2;
  evtGetValue(event, args[1]);
  a1 = evtGetValue(event, args[2]);
  name = (const char *) evtGetValue(event, args[3]);
  Punigp->unk43C = 5;
  if (a0 == 2)
  {
    Punigp->unk324 &= ~0xF;
    switch (a1)
    {
      case 6:
        Punigp->unk324 |= 1;
        break;

      case 2:
        Punigp->unk324 |= 2;
        break;

      case 8:
        Punigp->unk324 |= 4;
        break;

      case 9:
        Punigp->unk324 |= 8;
        break;

    }

  }
  if (name == ((void *) 0))
  {
    Punigp->unk324 |= 0x10;
  }
  else
  {
    Punigp->unk324 &= ~0x10;
  }
  if (!(Punigp->unk324 & 8))
  {
    sx = bero_get_BeroSX();
    sy = puni_init_posy(bero_get_BeroSY());
    sz = bero_get_BeroSZ();
    ex = bero_get_BeroEX();
    ey = puni_init_posy(bero_get_BeroEY());
    ez = bero_get_BeroEZ();
  }
  else
  {
    ex = (sx = bero_get_BeroSX());
    sy = puni_init_posy(bero_get_BeroSY());
    sz = bero_get_BeroSZ();
    ey = sy;
    ez = sz;
    sz += 60.0f;
  }
  *((s32 *) (((u8 *) Punigp) + 0x47C)) = 2;
  *((f32 *) (((u8 *) Punigp) + 0x480)) = sx;
  *((f32 *) (((u8 *) Punigp) + 0x484)) = sy;
  *((f32 *) (((u8 *) Punigp) + 0x488)) = sz;
  *((f32 *) (((u8 *) Punigp) + 0x4B0)) = 100.0f;
  *((f32 *) (((u8 *) Punigp) + 0x48C)) = ex;
  *((f32 *) (((u8 *) Punigp) + 0x490)) = ey;
  *((f32 *) (((u8 *) Punigp) + 0x494)) = ez;
  *((f32 *) (((u8 *) Punigp) + 0x4B4)) = 100.0f;
  if (name != ((void *) 0))
  {
    notMri00 = strcmp(name, "mri_00");
  }
  else
  {
    notMri00 = 1;
  }
  p = &Punidat[1];
  for (i = 1; i <= 102; i++, p++)
  {
    if (!(p->unk4 & 0x400))
    {
      continue;
    }
    if (p->unk4 & 0x800000)
    {
      continue;
    }
    if (p->unk4 & 0x8000)
    {
      if (name == ((void *) 0))
      {
        p->unk4 |= 0x40000000;
      }
      else
      {
        puniHagurePos(p);
        pos = p->pos, puni_exit_swset(*((s16 *) (((u8 *) p) + 0x116)), (u8) Punigp->unk328, &pos);
      }
    }
    else
    {
      pos = exit_zero0, puni_exit_swset(*((s16 *) (((u8 *) p) + 0x116)), 0, &pos);
      PuniDokuritsu(p);
      if (notMri00 == 0)
      {
        p->unkC = 14;
        p->unk114 = 0;
        p->anim = 0;
      }
      else
      {
        p->unkC = 26;
        p->unk114 = 0;
        p->anim = 2;
      }
    }
  }

  if ((name != ((void *) 0)) && (strcmp(name, "mri_00") == 0))
  {
    for (i = 1; i <= 102; i++)
    {
      pos2 = *((volatile Vec *) (&exit_zero1));
      puni_exit_swset(i, 0, &pos2);
    }

  }
  return 2;
}
#pragma pop

USER_FUNC(evt_puni_init_puni) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 type = evtGetValue(event, args[1]);
    PuniData* p = &Punidat[idx];
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);
    s32 chk = evtGetValue(event, args[5]);

    if (idx == 1) {
        if (chk != 0 && swGet(0xB3C) == 0) {
            return EVT_RETURN_DONE;
        }
        if (chk != 0 && swGet(0xB14) != 0) {
            return EVT_RETURN_DONE;
        }
        *(PuniData**)((u8*)Punigp + 0x364) = p;
        p->unk4 |= 0x20;
    } else if (idx == 2) {
        if (chk != 0 && swGet(0xB3D) == 0) {
            return EVT_RETURN_DONE;
        }
        *(PuniData**)((u8*)Punigp + 0x368) = p;
        p->unk4 |= 0x40;
    } else if (idx == 4) {
        if (chk != 0 && swGet(0xB3F) == 0) {
            return EVT_RETURN_DONE;
        }
        *(PuniData**)((u8*)Punigp + 0x370) = p;
        p->unk4 |= 0x100;
    } else if (idx == 3) {
        if (chk != 0 && swGet(0xB3E) == 0) {
            return EVT_RETURN_DONE;
        }
        *(PuniData**)((u8*)Punigp + 0x36C) = p;
        p->unk4 |= 0x80;
    } else {
        if (chk != 0 && swByteGet(0x28B) + 5 <= idx) {
            return EVT_RETURN_DONE;
        }
        p->unk4 |= 8;
    }
    p->unkC = type;
    p->unk114 = 0;
    p->anim = 0;
    p->unk11E = 1000;
    p->pos.x = x;
    p->pos.y = y;
    p->pos.z = z;
    p->unk138 = p->pos;
    p->unk190.x = 6.0f;
    p->unk190.y = 2.0f;
    p->unk190.z = 3.0f;
    p->hp = 60;
    p->maxHp = 60;
    p->regen = 10;
    p->regenTimer = 60;
    p->regenInterval = 60;
    p->unk57E = 20;
    p->unk580 = 10;
    p->unk582 = 10;
    p->damage = 0;
    p->unk586 = 0;
    p->unk588 = 50;
    p->unk58A = 70;
    p->parent = NULL;
    p->child1 = NULL;
    p->child2 = NULL;
    PuniDokuritsu(p);
    p->unk4 |= 0x1401;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_init_teki) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 type = evtGetValue(event, args[1]);
    PuniData* p = &Punidat[idx];
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);

    p->unk4 |= 0x10;
    p->unkC = type;
    p->anim = 0;
    p->unk114 = 0;
    p->unk11E = 1000;
    p->pos.x = x;
    p->pos.y = y;
    p->pos.z = z;
    p->unk138 = p->pos;
    p->unk190.x = 6.0f;
    p->unk190.y = 2.0f;
    p->unk190.z = 3.0f;
    p->hp = 60;
    p->maxHp = 60;
    p->regen = 10;
    p->regenTimer = 60;
    p->regenInterval = 60;
    p->unk57E = 20;
    p->unk580 = 10;
    p->unk582 = 10;
    p->damage = 0;
    p->unk586 = 0;
    p->unk588 = 50;
    p->unk58A = 70;
    p->unk4 |= 0x101402;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_get_position) {
    s32* args = event->args;
    PuniData* p = &Punidat[evtGetValue(event, args[0])];
    evtSetValue(event, args[1], (s32)p->pos.x);
    evtSetValue(event, args[2], (s32)p->pos.y);
    evtSetValue(event, args[3], (s32)p->pos.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_set_position) {
    s32* args = event->args;
    PuniData* p = &Punidat[evtGetValue(event, args[0])];
    p->pos.x = evtGetValue(event, args[1]);
    p->pos.y = evtGetValue(event, args[2]);
    p->pos.z = evtGetValue(event, args[3]);
    p->unk138 = p->pos;
    return EVT_RETURN_DONE;
}



USER_FUNC(evt_puni_get_dir) {
    s32* args = event->args;
    evtSetValue(event, args[1], (s32)Punidat[evtGetValue(event, args[0])].dir);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_set_dir) {
    s32* args = event->args;
    PuniData* p = &Punidat[evtGetValue(event, args[0])];
    p->dir = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_get_anim) {
    s32* args = event->args;
    evtSetValue(event, args[1], Punidat[evtGetValue(event, args[0])].anim);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_set_anim) {
    s32* args = event->args;
    PuniData* p = &Punidat[evtGetValue(event, args[0])];
    p->anim = evtGetValue(event, args[1]);
    p->unk11E = 1000;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_korokoro) {
    s32* args = event->args;
    PuniData* p = &Punidat[evtGetValue(event, args[0])];

    switch (p->anim) {
        case 5:
            if (isFirstCall) {
                if (p->unk4 & 8) {
                    p->unk188 = 8.0f;
                } else {
                    p->unk148 = 8.0f;
                }
            }
            p->spin += evtGetValue(event, args[1]);
            return EVT_RETURN_BLOCK;
        default:
            if (p->unk4 & 8) {
                p->unk188 = 0.0f;
            } else {
                p->unk148 = 0.0f;
            }
            p->spin = evtGetValue(event, args[2]);
            return EVT_RETURN_DONE;
    }
}

typedef struct PuniMoveView {
    u8 pad[0x1F8];
    Vec a;
    Vec b;
} PuniMoveView;
extern f32 intplGetValue(s32 mode, s32 currStep, s32 steps, f32 start, f32 end);
extern f32 sysFrame2SecFloat(f32 frame);
extern f32 sysMsec2FrameFloat(f32 msec);
USER_FUNC(evt_puni_move_position) {
    s32* args = event->args;
    PuniData* p = &Punidat[evtGetValue(event, args[0])];
    f32 x = evtGetValue(event, args[1]);
    f32 z = evtGetValue(event, args[2]);
    s32 frames = evtGetValue(event, args[3]);
    u32 t;

    if (isFirstCall) {
        event->savedTime = gp->renderTime;
        p->unk1F8 = p->pos;
        *(f32*)((u8*)p + 0x204) = x;
        *(f32*)((u8*)p + 0x208) = p->pos.y;
        *(f32*)((u8*)p + 0x20C) = z;
    }
    if ((s32)(t = (((u32)gp->renderTime - (u32)event->savedTime) / (*(u32*)0x800000F8 / 4 / 1000))) < frames) {
        p->pos.x = intplGetValue(0, t, frames, p->unk1F8.x, *(f32*)((u8*)p + 0x204));
        p->pos.z = intplGetValue(0, t, frames, p->unk1F8.z, *(f32*)((u8*)p + 0x20C));
        p->unk138 = p->pos;
        return EVT_RETURN_BLOCK;
    }
    p->pos = *(Vec*)((u8*)p + 0x204);
    p->unk138 = p->pos;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_move_position2) {
    s32* args = event->args;
    PuniData* p = &Punidat[evtGetValue(event, args[0])];
    PuniMoveView* v = (PuniMoveView*)p;
    f32 x = evtGetValue(event, args[1]);
    f32 z = evtGetValue(event, args[2]);
    f32 speed = evtGetFloat(event, args[3]);
    f32 dist;
    s32 t;
    Vec dir;

    if (isFirstCall) {
        event->savedTime = gp->renderTime;
        v->a = p->pos;
        v->b.x = x;
        v->b.y = p->pos.y;
        v->b.z = z;
    }
    speed = sysFrame2SecFloat(speed);
    dist = PSVECDistance(&v->a, &v->b);
    t = (s32)(0.5f + sysMsec2FrameFloat((s32)(((u32)gp->renderTime - (u32)event->savedTime) / (*(u32*)0x800000F8 / 4 / 1000))));
    PSVECSubtract(&v->b, &v->a, &dir);
    PSVECNormalize(&dir, &dir);
    if (t < (s32)(dist / speed)) {
        PSVECScale(&dir, &dir, speed * t);
        PSVECAdd(&v->a, &dir, &p->pos);
        return EVT_RETURN_BLOCK;
    }
    p->pos = v->b;
    p->unk138 = p->pos;
    return EVT_RETURN_DONE;
}

typedef struct PuniJumpView {
    u8 pad[0x1C8];
    u64 time;
    u8 pad2[0x1EC - 0x1D0];
    f32 grav;
    f32 c;
    u8 pad3[4];
    Vec a;
} PuniJumpView;

USER_FUNC(evt_puni_jump_position) {
    s32* args = event->args;
    PuniData* p = &Punidat[evtGetValue(event, args[0])];
    PuniJumpView* v = (PuniJumpView*)p;
    f32 x = evtGetValue(event, args[1]);
    f32 y = evtGetValue(event, args[2]);
    f32 z = evtGetValue(event, args[3]);
    s32 frames = evtGetValue(event, args[4]);
    f32 h = evtGetValue(event, args[5]);
    s32 t;
    f32 r;

    if (isFirstCall) {
        v->time = gp->renderTime;
        v->a = p->pos;
        v->grav = -4.0f * h;
        v->c = (y - v->a.y) - v->grav;
        psndSFXOn_3D("SFX_STG2_PUNI_JUMP1", &p->pos);
    }
    t = ((u32)gp->renderTime - (u32)v->time) / ((*(u32*)0x800000F8 / 4) / 1000);
    if (t < frames) {
        r = (f32)t / (f32)frames;
        r = (f32)t / (f32)frames;
        p->pos.x = intplGetValue(0, (s32)t, frames, v->a.x, x);
        p->pos.z = intplGetValue(0, (s32)t, frames, v->a.z, z);
        p->pos.y = r * (v->grav * r + v->c) + v->a.y;
        return EVT_RETURN_BLOCK;
    }
    p->pos.x = x;
    p->pos.y = y;
    p->pos.z = z;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_get_mode) {
    s32* args = event->args;
    evtSetValue(event, args[1], Punidat[evtGetValue(event, args[0])].unkC);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_set_mode) {
    s32* args = event->args;
    PuniData* p = &Punidat[evtGetValue(event, args[0])];
    s32 mode = evtGetValue(event, args[1]);
    p->unkC = mode;
    p->unkE = mode;
    p->unk114 = 0;
    p->unk11E = 1000;
    PuniDokuritsu(p);
    puniDeleteEffect(p);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_setsudan) {
    PuniData* p = &Punidat[evtGetValue(event, event->args[0])];
    if (p->unk4 & 0x400) {
        PuniDokuritsu(p);
        puniDeleteEffect(p);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_get_flag) {
    s32* args = event->args;
    evtSetValue(event, args[1], Punidat[evtGetValue(event, args[0])].unk4);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_flag_onoff) {
    s32* args = event->args;
    PuniData* p = &Punidat[evtGetValue(event, args[0])];
    s32 on = evtGetValue(event, args[1]);
    s32 mask = evtGetValue(event, args[2]);
    if (on) {
        p->unk4 &= ~mask;
    } else {
        p->unk4 |= mask;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_dokan_onoff) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 on = evtGetValue(event, args[1]);
    PuniData* p = &Punidat[puni_bero_id_filter(id)] + 103;
    if (on == 0) {
        p->unk4 |= 0x100000;
    } else {
        p->unk4 &= ~0x100000;
    }
    return EVT_RETURN_DONE;
}

static inline void puniswGetindex_inl(s32 base, s32 idx, s32* val, Vec* pos) {
    u8 b6, b5, b4, b3, b2, b1, b0;
    s32 off;
    s32 x;
    s32 y;
    s32 z;

    off = base + idx * 7;
    b0 = _swByteGet(off + 150000000);
    b1 = _swByteGet(off + 150000001);
    b2 = _swByteGet(off + 150000002);
    b3 = _swByteGet(off + 150000003);
    b4 = _swByteGet(off + 150000004);
    b5 = _swByteGet(off + 150000005);
    b6 = _swByteGet(off + 150000006);

    *val = b0;
    x = b1 + (b2 << 8);
    if (x & 0x8000) {
        x |= 0xFFFF0000;
    }
    y = b3 + (b4 << 8);
    if (y & 0x8000) {
        y |= 0xFFFF0000;
    }
    z = b5 + (b6 << 8);
    if (z & 0x8000) {
        z |= 0xFFFF0000;
    }
    pos->x = x;
    pos->y = y;
    pos->z = z;
}

USER_FUNC(evt_puni_get_lsw_index) {
    s32* args = event->args;
    s32 base = args[0];
    s32 idx = evtGetValue(event, args[1]);
    s32 val;
    Vec pos;
    base += idx * 7;
    puniswGetindex_inl(base, 0, &val, &pos);
    evtSetValue(event, args[2], val);
    evtSetValue(event, args[3], (s32)pos.x);
    evtSetValue(event, args[4], (s32)pos.y);
    evtSetValue(event, args[5], (s32)pos.z);
    return EVT_RETURN_DONE;
}

#pragma push
#pragma opt_dead_assignments off
USER_FUNC(evt_puni_set_lsw_index) {
    s32* args = event->args;
    s32 base = args[0];
    s32 idx = evtGetValue(event, args[1]);
    s32 val = evtGetValue(event, args[2]);
    Vec pos;
    pos.x = evtGetValue(event, args[3]);
    pos.y = evtGetValue(event, args[4]);
    pos.z = evtGetValue(event, args[5]);
    puniswSetindex(base, idx, &val, &pos);
    return EVT_RETURN_DONE;
}
#pragma pop

USER_FUNC(evt_puni_get_trap_index) {
    s32* args = event->args;
    s32 v;
    puniswTrapGetindex(evtGetValue(event, args[0]), &v);
    evtSetValue(event, args[1], v);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_set_trap_index) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 val = evtGetValue(event, args[1]);
    puniswTrapSetindex(idx, &val);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_set_ymode) {
    Punigp->ymode = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_all_fall_check) {
    puniModeChangeFallCheck();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_all_movestop) {
    s32 stop = evtGetValue(event, event->args[0]);
    s32 i;
    for (i = 1; i < 103; i++) {
        PuniData* p = &Punidat[i];
        if (p->unk4 & 0x400) {
            if (stop == 0) {
                p->unk4 &= ~0x200;
            } else {
                p->unk4 |= 0x200;
            }
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_count) {
    s32* args = event->args;
    s32 start, end, i;
    s32 count;

    if (evtGetValue(event, args[0]) == 0) {
        start = 1;
        end = 103;
    } else {
        start = 110;
        end = 210;
    }
    count = 0;
    for (i = start; i < end; i++) {
        if ((Punidat[i].unk4 & 0x400) && !(Punidat[i].unk4 & 0x8000)) {
            count++;
        }
    }
    evtSetValue(event, args[1], count);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_count_area) {
    s32* args = event->args;
    f32 x = evtGetValue(event, args[0]);
    f32 y = evtGetValue(event, args[1]);
    f32 z = evtGetValue(event, args[2]);
    f32 r = evtGetValue(event, args[3]);
    s32 count;
    s32 i;
    for (i = 1, count = 0; i < 103; i++) {
        PuniData* p = &Punidat[i];
        if ((p->unk4 & 0x400) && p->unkC != 16) {
            if (!(10.0 < __fabs(y - p->pos.y)) && !(r < __fabs(x - p->pos.x)) && !(r < __fabs(z - p->pos.z))) {
                count++;
            }
        }
    }
    evtSetValue(event, args[4], count);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_zenmetsu_check) {
    s32* args = event->args;
    s32 start;
    s32 end;
    s32 i;
    s32 count;

    if (evtGetValue(event, args[0]) == 0) {
        start = 1;
        end = 103;
    } else {
        start = 110;
        end = 210;
    }
    count = 0;
    for (i = start; i < end; i++) {
        if ((Punidat[i].unk4 & 0x400) && !(Punidat[i].unk4 & 0x4000)) {
            count++;
        }
    }
    if (count == 0) {
        evtSetValue(event, args[1], 1);
    } else {
        evtSetValue(event, args[1], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_landon_check) {
    s32* args = event->args;
    s32 kind = evtGetValue(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    s32 start;
    s32 end;
    s32 i;
    s32 count;
    PuniData* p;

    if (kind == 0) {
        start = 1;
        end = 103;
    } else {
        start = 110;
        end = 210;
    }
    count = 0;
    for (i = start; i < end; i++) {
        p = &Punidat[i];
        if ((p->unk4 & 0x400) && p->pos.y > y) {
            count++;
        }
    }
    if (count == 0) {
        evtSetValue(event, args[2], 1);
    } else {
        evtSetValue(event, args[2], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_pera_onoff) {
    s32* args = event->args;
    PuniData* p = &Punidat[evtGetValue(event, args[0])];
    if (evtGetValue(event, args[1])) {
        p->npc->flags &= ~2;
    } else {
        p->npc->flags |= 2;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_fade_onoff) {
    puniFadeOff(evtGetValue(event, event->args[0]));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_attack_on) {
    Punigp->unk43C = 3;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_puninum_onoff) {
    Punigp->showPuniCounter = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_puninum_alpha) {
    Punigp->puniCounterAlpha = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_winddisable_onoff) {
    Punigp->unk4CC = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_force_se_off) {
    if (Punigp->sfx458) {
        psndSFXOff(Punigp->sfx458);
    }
    if (Punigp->sfx468) {
        psndSFXOff(Punigp->sfx468);
    }
    return EVT_RETURN_DONE;
}

static const Vec puni_save_zero = { 0.0f, 0.0f, 0.0f };
#pragma push
#pragma opt_dead_assignments off
USER_FUNC(evt_puni_all_save) {
    PuniData* p = Punidat + 1;
    Vec pos;
    s32 i;
    for (i = 1; i <= 102; i++, p++) {
        if ((p->unk4 & 0x400) && !(p->unk4 & 0x800000)) {
            if (p->unk4 & 0x8000) {
                puniHagurePos(p);
                pos = p->pos;
                puni_exit_swset(*(s16*)((u8*)p + 0x116), (u8)Punigp->unk328, &pos);
            } else {
                pos = puni_save_zero;
                puni_exit_swset(*(s16*)((u8*)p + 0x116), 0, &pos);
            }
        }
    }
    return EVT_RETURN_DONE;
}
#pragma pop

USER_FUNC(evt_target_flag_onoff) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 on = evtGetValue(event, args[1]);
    s32 mask = evtGetValue(event, args[2]);
    PuniTarget* t = &Punigp->targets[idx];
    if (on) {
        t->flags &= ~mask;
    } else {
        t->flags |= mask;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_target_set_position) {
    s32* args = event->args;
    s32 i = evtGetValue(event, args[0]);
    PuniTarget* t = &Punigp->targets[i];
    t->pos.x = evtGetValue(event, args[1]);
    t->pos.y = evtGetValue(event, args[2]);
    t->pos.z = evtGetValue(event, args[3]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_target_set_soko) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    PuniTarget* t = &Punigp->targets[idx];
    t->hit = hitNameToPtr((const char*)evtGetValue(event, args[1]));
    return EVT_RETURN_DONE;
}

const char* npcGetTribeName(const char** name) {
    return *name;
}

#define PIDERS_Y(n)                                           if (50.0 >= __fabs(v.y - Punigp->posY[n])) {                  found = TRUE;                                              v.y = Punigp->posY[n];                                 }

s32 puni_piders_check_func(EventEntry *event, BOOL isFirstCall)
{
  NpcWork *work = npcGetWorkPtr();
  s32 new_var3[2];
  int new_var;
  NpcEntry *npc;
  s32 count;
  s32 i;
  count = work->total;
  npc = work->entries;
  for (new_var3[1] = 0x28, i = 0; i < 8; i++, new_var3[1] += 0x14)
  {
    *((u32 *) ((((u8 *) Punigp) + 0x374) + new_var3[1])) &= ~5;
  }

  new_var3[1] = 0x28;
  for (i = 0; i < count; i++, npc++)
  {
    if ((((npc->flags & 1) && (npc->flags & 2)) && (npc->tribe != ((void *) 0))) && (strcmp(npc->tribe->nameJp, "�p�C�_�[�X") == 0))
    {
      Vec v;
      BOOL found = 0;
      v = npc->position;
      new_var = 1;
      new_var3[0] = 50.0;
      if (50.0 >= __fabs(v.y - Punigp->posY[0, 0]))
      {
        found = 1;
        v.y = Punigp->posY[0];
      }
      if (new_var3[0] >= __fabs(v.y - Punigp->posY[1]))
      {
        found = 1;
        v.y = Punigp->posY[new_var];
      }
      if (50.0 >= __fabs(v.y - Punigp->posY[2]))
      {
        found = new_var;
        v.y = Punigp->posY[2];
      }
      if (50.0 >= __fabs(v.y - Punigp->posY[3]))
      {
        found = new_var;
        v.y = Punigp->posY[3];
      }
      if (found)
      {
        PuniTarget *t = (PuniTarget *) ((((u8 *) Punigp) + 0x374) + new_var3[1]);
        t->pos = v;
        t->flags |= 5;
      }
      new_var3[1] += 0x14;
    }
  }

  return 2;
}

USER_FUNC(puni_disp_info_kassencheck) {
    s32* args = event->args;
    BOOL ret = FALSE;
    if (Punigp->unk328 == 9 || Punigp->unk328 == 12) {
        ret = TRUE;
    }
    evtSetValue(event, args[0], ret);
    return EVT_RETURN_DONE;
}

extern BOOL evtShopIsActive(void);
void puniCountDisp(s32 unused, s32 packed) {
    Mtx m;
    GXColor c1 = {0xFF, 0xFF, 0xFF, 0x99};
    GXColor c2 = {0, 0, 0, 0xFF};
    GXColor c3 = {0xFF, 0xFF, 0xFF, 0xFF};
    s32 num2 = packed >> 16;
    s32 num1 = packed & 0xFFFF;
    if (evtShopIsActive()) {
        return;
    }
    c1.a = c1.a * Punigp->puniCounterAlpha / 255;
    c2.a = c2.a * Punigp->puniCounterAlpha / 255;
    c3.a = c3.a * Punigp->puniCounterAlpha / 255;
    PSMTXTrans(m, -160.0f, -180.0f, 0.0f);
    iconDispGxCol(m, 0x10, 0x203, c1);
    PSMTXTrans(m, -232.0f, -178.0f, 0.0f);
    iconDispGxCol(m, 0x10, 0x204, c2);
    PSMTXTrans(m, -160.0f, -178.0f, 0.0f);
    if (Punigp->unk448) {
        if (gp->retraceCount & 0x10) {
            iconNumberDispGx(m, num2, FALSE, c3);
        }
    } else {
        iconNumberDispGx(m, num2, FALSE, c3);
    }
    PSMTXTrans(m, -140.0f, -178.0f, 0.0f);
    iconDispGxCol(m, 0x10, 0x1D8, c3);
    PSMTXTrans(m, -90.0f, -178.0f, 0.0f);
    iconNumberDispGx(m, num1, TRUE, c3);
}

USER_FUNC(puni_disp_info_sub) {
    s32* args = event->args;
    s32 count = 0;
    s32 total;
    BOOL done;
    s32 i;
    s32 packed;
    for (i = 1; i < 103; i++) {
        if ((Punidat[i].unk4 & 0x400) && !(Punidat[i].unk4 & 0x804000) && !(Punidat[i].unk4 & 0x80)) {
            if (Punidat[i].unkC != 16 && Punidat[i].unkC != 15) {
                count++;
            }
        }
    }
    total = swGet(0xB3C);
    total += swGet(0xB3D);
    total += swGet(0xB3F);
    total += swByteGet(0x28B);
    done = FALSE;
    if (Punigp->unk328 == 9 && !swGet(0xB21)) {
        done = TRUE;
    }
    if (Punigp->unk328 == 12 && !swGet(0xB29)) {
        done = TRUE;
    }
    if (done) {
        evtSetValue(event, args[0], count);
        evtSetValue(event, args[1], 0);
        Punigp->unk448 = 0;
    } else {
        evtSetValue(event, args[0], count);
        evtSetValue(event, args[1], total - count);
        if (count == total) {
            Punigp->unk448 = 0;
        } else {
            Punigp->unk448 = 1;
        }
    }
    if (!(Punigp->systemMode & 2)) {
        count = swByteGet(0x29B);
        total = swByteGet(0x29C);
        Punigp->unk448 = 0;
    } else {
        swByteSet(0x29B, (u8)count);
        swByteSet(0x29C, (u8)total);
    }
    packed = (count << 16) | total;
    if (Punigp->showPuniCounter && total >= 10) {
        dispEntry((CameraId)8, 1000.0f, 0, (void (*)(CameraId, void*))puniCountDisp, (void*)packed);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_make_texbuf);
USER_FUNC(evt_npc_set_tribe);
USER_FUNC(evt_sub_get_mapname);
USER_FUNC(evt_sub_intpl_msec_get_value);
USER_FUNC(evt_sub_intpl_msec_init);
extern s32 mri_puni_init_dvd[];
extern s32 mri_puni_main[];
extern s32 mri_puni_position_init[];
extern s32 mri_puninum_fadein[];
extern s32 mri_puninum_fadeout[];

s32 puni_piders_check[] = {
    0x0002005B, (s32)evt_seq_wait, 0x00000002, 0x00010005,
    0x00000000, 0x0001005B, (s32)puni_piders_check_func, 0x00010009,
    0x00000001, 0x00000006, 0x00000002, 0x00000001,
};

s32 puni_disp_info[] = {
    0x0002001D, 0xF5DE0180, 0x00000076, 0x00000002,
    0x00000021, 0x00010005, 0x00000000, 0x0003005B,
    (s32)puni_disp_info_sub, 0xFE363C80, 0xFE363C81, 0x00010009,
    0x00000001, 0x00000006, 0x00000002, 0x00000001,
};

s32 mri_puninum_fadeout[] = {
    0x0001000A, 0x000004B0, 0x0005005B, (s32)evt_sub_intpl_msec_init,
    0x0000000B, 0x000000FF, 0x00000000, 0x000001F4,
    0x00010005, 0x00000000, 0x0001005B, (s32)evt_sub_intpl_msec_get_value,
    0x0002005B, (s32)evt_puni_puninum_alpha, 0xFE363C80, 0x00010009,
    0x00000001, 0x00020018, 0xFE363C81, 0x00000000,
    0x00000007, 0x00000021, 0x00000006, 0x00000002,
    0x00000001,
};

s32 mri_puninum_fadein[] = {
    0x0005005B, (s32)evt_sub_intpl_msec_init, 0x0000000B, 0x00000000,
    0x000000FF, 0x000001F4, 0x00010005, 0x00000000,
    0x0001005B, (s32)evt_sub_intpl_msec_get_value, 0x0002005B, (s32)evt_puni_puninum_alpha,
    0xFE363C80, 0x00010009, 0x00000001, 0x00020018,
    0xFE363C81, 0x00000000, 0x00000007, 0x00000021,
    0x00000006, 0x00000002, 0x00000001,
};

s32 mri_puni_position_init[] = {
    0x0002005B, (s32)evt_puni_set_systemmode, 0x00000000, 0x0000006B,
    0x0002005B, (s32)evt_puni_puninum_alpha, 0x00000000, 0x00010009,
    0x00000002, 0x0002005B, (s32)evt_bero_get_now_number, 0xFE363C8A,
    0x0005005B, (s32)evt_bero_get_info_kinddir, 0xFE363C8A, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0004005B, (s32)evt_bero_get_info_nextarea,
    0xFE363C8A, 0xFE363C83, 0xFE363C84, 0x00020032,
    0xFBD3E280, 0x00000000, 0x00020018, 0xFE363C80,
    0x00000002, 0x00020019, 0xFE363C83, 0x00000000,
    0x00020032, 0xFBD3E280, 0x00000001, 0x00000021,
    0x00000021, 0x00020018, 0xFBD3E280, 0x00000000,
    0x0002005B, (s32)evt_puni_puninum_alpha, 0x000000FF, 0x00000021,
    0x0000006D, 0x00010071, (s32)"\203v\203j\212\326\214W\201F\217\211\212\372\202\314\216\300\215s\222\206\221\322\202\277", 0x0002005B,
    (s32)evt_bero_exec_wait, 0x00000001, 0x00010005, 0x00000000,
    0x0002005B, (s32)evt_puni_set_systemmode, 0x00000001, 0x00010071,
    (s32)"\203v\203j\212\326\214W\201F\215\300\225W\203Z\203b\203g\221\322\202\277", 0x0002005B, (s32)evt_bero_exec_wait, 0x00000002,
    0x00010071, (s32)"\203v\203j\212\326\214W\201F\217\254\223\256\225\250\212\326\214W\217\211\212\372\211\273", 0x00010073, 0xF5DE0419,
    0x00010073, 0xF5DE041A, 0x0002005B, (s32)evt_bero_get_now_number,
    0xFE363C8A, 0x0005005B, (s32)evt_bero_get_info_kinddir, 0xFE363C8A,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0004005B,
    (s32)evt_bero_get_info_nextarea, 0xFE363C8A, 0xFE363C83, 0xFE363C84,
    0x00020018, 0xFE363C80, 0x00000002, 0x00020019,
    0xFE363C83, 0x00000000, 0x0001005C, (s32)mri_puninum_fadein,
    0x00000021, 0x00000021, 0x0004005B, (s32)evt_puni_setup,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0005005B,
    (s32)evt_toge_setup, 0xFE363C86, 0xFE363C87, 0xFE363C88,
    0xFE363C89, 0x0002005B, (s32)evt_puni_set_systemmode, 0x00000002,
    0x00010071, (s32)"\203v\203j\212\326\214W\201F\217I\227\271\221\322\202\277", 0x0002005B, (s32)evt_bero_exec_wait,
    0x00010000, 0x0002005B, (s32)evt_puni_set_systemmode, 0x00000003,
    0x00010071, (s32)"\203v\203j\212\326\214W\201F\216\237\202\314\216\300\215s\222\206\221\322\202\277", 0x0002005B, (s32)evt_bero_exec_wait,
    0x00000001, 0x00020032, 0xF5DE041A, 0xF5DE0419,
    0x0002005B, (s32)evt_bero_get_now_number, 0xFE363C8A, 0x0005005B,
    (s32)evt_bero_get_info_kinddir, 0xFE363C8A, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0004005B, (s32)evt_bero_get_info_nextarea, 0xFE363C8A,
    0xFE363C83, 0xFE363C84, 0x00020018, 0xFE363C80,
    0x00000001, 0x00010071, (s32)"\203v\203j\212\326\214W\201F\224\340\221\322\202\277", 0x0002005B,
    (s32)evt_bero_exec_wait, 0x10000000, 0x00010071, (s32)"\203v\203j\212\326\214W\201F\224\340\221\322\202\277\217I\202\355\202\350",
    0x0003005B, (s32)evt_bero_exec_onoff, 0x00000001, 0x10000000,
    0x00000021, 0x00020018, 0xFE363C80, 0x00000002,
    0x00020019, 0xFE363C83, 0x00000000, 0x0001005C,
    (s32)mri_puninum_fadeout, 0x00000021, 0x00000021, 0x0000006B,
    0x00010071, (s32)"\203v\203j\212\326\214W\201F\203G\203\212\203A\203`\203F\203\223\203W\222\274\221O\221\322\202\277", 0x0002005B, (s32)evt_bero_exec_wait,
    0x00000008, 0x00010071, (s32)"\203v\203j\212\326\214W\201F\203v\203j\221\260\203g\203Q\221\260SE\213\255\220\247\203I\203t", 0x0001005B,
    (s32)evt_puni_force_se_off, 0x0000006D, 0x0005005B, (s32)evt_puni_exit,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0xFE363C83,
    0x00000006, 0x00000002, 0x00000001,
};

s32 mri_puni_main[] = {
    0x00010063, 0x00000000, 0x0001005B, (s32)evt_puni_main,
    0x00000002, 0x00000001,
};

s32 mri_puni_init_dvd[] = {
    0x0003005B, (s32)evt_npc_entry, (s32)"\203v\203jsp", (s32)"c_puni",
    0x0003005B, (s32)evt_npc_set_tribe, (s32)"\203v\203jsp", (s32)"\203v\203j\221\260",
    0x0003005B, (s32)evt_npc_entry, (s32)"\203g\203Qsp", (s32)"c_toge",
    0x0003005B, (s32)evt_npc_set_tribe, (s32)"\203g\203Qsp", (s32)"\203g\203Q\221\260",
    0x0003005B, (s32)evt_npc_entry, (s32)"\203K\203C\203hsp", (s32)"c_pguide",
    0x0003005B, (s32)evt_npc_set_tribe, (s32)"\203K\203C\203hsp", (s32)"\203K\203C\203h",
    0x0003005B, (s32)evt_npc_entry, (s32)"\203K\203C\203h\226\205sp", (s32)"c_psister",
    0x0003005B, (s32)evt_npc_set_tribe, (s32)"\203K\203C\203h\226\205sp", (s32)"\203K\203C\203h\226\205",
    0x0003005B, (s32)evt_npc_entry, (s32)"\202\277\202\321\203g\203Qsp", (s32)"c_tchibi",
    0x0003005B, (s32)evt_npc_set_tribe, (s32)"\202\277\202\321\203g\203Qsp", (s32)"\202\277\202\321\203g\203Q",
    0x0003005B, (s32)evt_npc_entry, (s32)"\203\211\203C\203o\203\213sp", (s32)"c_prival",
    0x0003005B, (s32)evt_npc_set_tribe, (s32)"\203\211\203C\203o\203\213sp", (s32)"\203\211\203C\203o\203\213",
    0x0005005B, (s32)evt_npc_set_position, (s32)"\203v\203jsp", 0x00000000,
    0xFFFFFC18, 0x00000000, 0x0005005B, (s32)evt_npc_set_position,
    (s32)"\203g\203Qsp", 0x00000000, 0xFFFFFC18, 0x00000000,
    0x0005005B, (s32)evt_npc_set_position, (s32)"\203K\203C\203hsp", 0x00000000,
    0xFFFFFC18, 0x00000000, 0x0005005B, (s32)evt_npc_set_position,
    (s32)"\203K\203C\203h\226\205sp", 0x00000000, 0xFFFFFC18, 0x00000000,
    0x0005005B, (s32)evt_npc_set_position, (s32)"\202\277\202\321\203g\203Qsp", 0x00000000,
    0xFFFFFC18, 0x00000000, 0x0005005B, (s32)evt_npc_set_position,
    (s32)"\203\211\203C\203o\203\213sp", 0x00000000, 0xFFFFFC18, 0x00000000,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"\203v\203jsp",
    0x40080020, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    (s32)"\203g\203Qsp", 0x40080020, 0x0004005B, (s32)evt_npc_flag_onoff,
    0x00000001, (s32)"\203K\203C\203hsp", 0x40080020, 0x0004005B,
    (s32)evt_npc_flag_onoff, 0x00000001, (s32)"\203K\203C\203h\226\205sp", 0x40080020,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"\202\277\202\321\203g\203Qsp",
    0x40080020, 0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001,
    (s32)"\203\211\203C\203o\203\213sp", 0x40080020, 0x00000002, 0x00000001,
};

s32 mri_puni_init[] = {
    0x0001005E, (s32)mri_puni_init_dvd, 0x00010022, 0xF5DE0180,
    0x00010026, 0x00000058, 0x00000002, 0x00010029,
    0x00000193, 0x00000002, 0x00010024, 0x00000190,
    0x0002005B, (s32)evt_sub_get_mapname, 0xFE363C80, 0x0002000D,
    0xFE363C80, (s32)"mri_00", 0x00000002, 0x00000021,
    0x00020032, 0xFBD3E280, 0x00000000, 0x0002005B,
    (s32)evt_bero_get_entername, 0xFE363C80, 0x0002000C, 0xFE363C80,
    (s32)"sekai_yami1", 0x00020032, 0xFBD3E280, 0x00000001,
    0x00000021, 0x0002000C, 0xFE363C80, (s32)"minnnanokoe",
    0x00020032, 0xFBD3E280, 0x00000001, 0x00000021,
    0x00020018, 0xFBD3E280, 0x00000000, 0x00000002,
    0x00000021, 0x0002002F, 0x00000076, 0x0000018F,
    0x0002005B, (s32)evt_sub_get_mapname, 0xFE363C80, 0x0002000D,
    0xFE363C80, (s32)"mri_00", 0x00000002, 0x00000021,
    0x00020018, 0xF84066C4, 0x00000001, 0x00000002,
    0x00000021, 0x0000002A, 0x00000031, 0x0001005B,
    (s32)evt_make_texbuf, 0x0005005B, (s32)evt_puni_init, 0x000000D2,
    (s32)puni_puni_init, (s32)puni_puni_main, (s32)puni_puni_dispent, 0x0001005B,
    (s32)evt_puni_set_maze, 0x0001005C, (s32)mri_puni_position_init, 0x0001005C,
    (s32)mri_puni_main, 0x00000002, 0x00000001,
};

s32 mri_puni_init_none[] = {
    0x0001005E, (s32)mri_puni_init_dvd, 0x0001005B, (s32)evt_make_texbuf,
    0x0005005B, (s32)evt_puni_init, 0x000000D2, (s32)puni_puni_init,
    (s32)puni_puni_main, (s32)puni_puni_dispent, 0x0001005B, (s32)evt_puni_set_maze,
    0x00000002, 0x00000001,
};
