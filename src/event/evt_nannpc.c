#include "evt/evt_cmd.h"
#include "driver/dispdrv.h"
#include "driver/hitdrv.h"
#include "mario/mariost.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os.h>
#include <string.h>

extern int sprintf(char* str, const char* format, ...);
void qqsort(void* array, s32 num, s32 size, s32 (*cmp)(void*, void*));

typedef struct ExtPose {
    u32 flags;   // 0x0
    f32 rot;     // 0x4
    f32 shadowY; // 0x8
    Mtx* mtx;    // 0xC
    s32 poseId;  // 0x10
    u8 pad[0xC];
} ExtPose;

ExtPose* extGetPosePtr(void);
s32 extGetPoseNum(void);
void extPoseDraw(s32 idx);
void extLoadShadowMtx(Mtx mtx);
void extLoadShadowRenderMode(void);
void extLoadShadowVertex(void);
void extLoadShadowTev(void);
void extDrawShadow(void);
void extLoadVertex(void);
void extLoadTexture(void);
void extLoadTextureExit(void);
void extLoadRenderMode(void);
void extLoadTev(void);
void extDraw(void);
void extEntry(s32 count, void* name, void* initf, void* mainf, void* dispf);

typedef struct NanNPC {
    s8 name[0x10];    // 0x0
    ExtPose* pose;    // 0x10
    u16 flags;        // 0x14
    u16 field_16;     // 0x16
    Vec pos;          // 0x18
    Vec hosei;        // 0x24
    Vec field_30;     // 0x30
    Vec move;         // 0x3C
    f32 sizeX;        // 0x48
    f32 sizeY;        // 0x4C
    OSTime startTime; // 0x50
    s16* animtbl;     // 0x58
    s16 animnum;      // 0x5C
    s16 anim;         // 0x5E
    f32 dir;          // 0x60
    void (*func)(struct NanNPC*); // 0x64
    s32 ipos[3];      // 0x68
    u8 pad_74[0x14];  // 0x74
    Vec startPos;     // 0x88
    u8 pad_94[0x10];  // 0x94
    s32 work[4];      // 0xA4
    HitEntry* hit0;   // 0xB4
    HitEntry* hit1;   // 0xB8
    GXColor color;    // 0xBC
} NanNPC;

typedef struct NanNPCSort {
    s32 idx;
    f32 z;
} NanNPCSort;

typedef struct NanNPCWork {
    NanNPC* npc;       // 0x0
    NanNPCSort* sort;  // 0x4
    s16* tbl;          // 0x8
    s32 num;           // 0xC
    u32 flags;         // 0x10
    void (*func14)();  // 0x14
    void (*func18)();  // 0x18
    void (*func1c)();  // 0x1C
} NanNPCWork;

s32 gap_08_8041899C_sbss;
NanNPCWork* nanNPCWork;
char makestring[16];

void nannpc_ext_disp();
void nannpc_ext_shadow_disp();
void nannpc_ext_dispent();
void nannpc_ext_main();
void nannpc_ext_main_sub();
void nannpc_ext_init();

static inline NanNPC* nanNPCSearch(char* name) {
    s32 i;
    NanNPC* p;
    NanNPCWork* w = nanNPCWork;
    for (i = 0; i < w->num; i++) {
        p = &w->npc[i];
        if (!((u32)name & 0x80000000)) {
            sprintf(makestring, "%06x", name);
            name = makestring;
        }
        if (!strcmp((char*)p->name, name)) {
            return p;
        }
    }
    return NULL;
}

void nanNPCOption(void) {
    s32 clear;
    NanNPC* q;
    void* w;
    NanNPC* p;
    NanNPC* r;
    Vec diff;
    Vec v84;
    Vec v78;
    Vec v6c;
    Vec v60;
    Vec v54;
    Vec v48;
    Vec v3c;
    Vec n;
    Vec h;
    f32 mag;
    f32 dist;
    f32 d20;
    f32 nz, ny, nx, hz, hy, hx;

    w = nanNPCWork;
    for (p = ((NanNPCWork*)w)->npc; p < ((NanNPCWork*)w)->npc + ((NanNPCWork*)w)->num; p++) {
        if (p->flags & 4) {
            for (q = ((NanNPCWork*)w)->npc; q < ((NanNPCWork*)w)->npc + ((NanNPCWork*)w)->num; q++) {
                if (p != q && (q->flags & 4)) {
                    dist = PSVECDistance(&p->pos, &q->pos);
                    PSVECSubtract(&p->pos, &q->pos, &diff);
                    if (dist <= p->sizeY) {
                        mag = PSVECMag(&diff);
                        if (mag == 0.0f) {
                            diff.x = 1.0f;
                        }
                        PSVECNormalize(&diff, &diff);
                        diff.y = 0.0f;
                        PSVECScale(&diff, &diff, 0.5f);
                        PSVECAdd(&p->move, &diff, &p->move);
                        PSVECSubtract(&q->move, &diff, &q->move);
                    }
                }
            }
        }
    }
    for (p = ((NanNPCWork*)w)->npc; p < ((NanNPCWork*)w)->npc + ((NanNPCWork*)w)->num; p++) {
        if (p->flags & 4) {
            PSVECSubtract(&p->field_30, &p->pos, &diff);
            mag = PSVECMag(&diff);
            if (mag > 0.8f * p->sizeY) {
                PSVECNormalize(&diff, &diff);
                diff.y = 0.0f;
                PSVECScale(&diff, &diff, 0.5f);
                PSVECAdd(&p->move, &diff, &p->move);
            }
        }
    }
    for (r = ((NanNPCWork*)w)->npc; r < ((NanNPCWork*)w)->npc + ((NanNPCWork*)w)->num; r++) {
        if (r->flags & 4) {
            mag = PSVECMag(&r->move);
            if (mag) {
                if (mag > 10.0f) {
                    PSVECScale(&r->move, &r->move, 5.0f / mag);
                    mag = 5.0f;
                }
                v60 = r->pos;
                v60.y += 1.0f;
                PSVECNormalize(&r->move, &v6c);
                if (r->flags & 8) {
                    PSVECAdd(&r->pos, &r->move, &r->pos);
                    PSVECScale(&r->move, &r->move, 0.6f);
                } else {
                    clear = 1;
                    v84 = v6c;
                    v78 = v60;
                    r->hit1 = NULL;
                    r->hit0 = NULL;
                    d20 = mag;
                    r->hit0 = hitCheckFilter(NULL, &h.x, &h.y, &h.z, &d20, &n.x, &n.y, &n.z,
                                             v78.x, v78.y, v78.z, v84.x, v84.y, v84.z);
                    if (!r->hit0) {
                        PSVECAdd(&r->pos, &r->move, &r->pos);
                    } else {
                        v48 = r->pos;
                        PSVECAdd(&r->pos, &r->move, &r->pos);
                        if (r->move.y == 0.0f) {
                            n.y = 0.0f;
                        }
                        PSVECNormalize(&n, &n);
                        C_VECReflect(&v84, &n, &v54);
                        PSVECScale(&v54, &r->move, mag);
                        clear = 0;
                        PSVECScale(&n, &n, PSVECDotProduct(&v84, &n) * -(mag - d20) + 0.1f);
                        PSVECAdd(&r->pos, &n, &r->pos);
                        PSVECSubtract(&r->pos, &v48, &v3c);
                        d20 = PSVECMag(&v3c);
                        if ((f32)d20 > 0.0f) {
                            PSVECNormalize(&v3c, &v3c);
                            v78 = v48;
                            r->hit1 = hitCheckFilter(
                                NULL, &hx, &hy, &hz, &d20, &nx, &ny, &nz, v48.x, 1.0f + v48.y, v48.z,
                                v3c.x, v3c.y, v3c.z);
                            if (r->hit1) {
                                r->pos = v78;
                            }
                        }
                    }
                    if (clear) {
                        PSVECScale(&r->move, &r->move, 0.6f);
                    }
                }
            }
        }
    }
}


s32 nannpc_zcompare(NanNPCSort* a, NanNPCSort* b) {
    if (a->z < b->z) {
        return 1;
    }
    if (a->z > b->z) {
        return -1;
    }
    return 0;
}

static inline void nannpc_loadTev(void) {
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 15, 8, 2, 15);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 4, 1, 7);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){255, 255, 255, 255});
    GXSetNumTexGens(1);
    GXSetNumTevStages(1);
}
void nannpc_ext_disp() {
    NanNPCWork* w = nanNPCWork;
    s32 i;
    s32 idx;
    NanNPCSort* sp;
    NanNPC* np;
    if (w->flags & 1) {
        extLoadVertex();
        extLoadTexture();
        nannpc_loadTev();
        for (i = extGetPoseNum() - 1; i >= 0; i--) {
            np = w->npc + i;
            sp = w->sort + i;
            sp->idx = i;
            sp->z = np->pos.z;
        }
        qqsort(w->sort, w->num, 8, (s32(*)(void*, void*))nannpc_zcompare);
        for (i = extGetPoseNum() - 1; i >= 0; i--) {
            idx = w->sort[i].idx;
            GXSetTevColor(1, w->npc[idx].color);
            extPoseDraw(idx);
        }
        extLoadTextureExit();
    } else {
        extLoadRenderMode();
        extLoadVertex();
        extLoadTexture();
        extLoadTev();
        extDraw();
        extLoadTextureExit();
    }
}

void nannpc_ext_shadow_disp() {
    static Mtx mt = {{10.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 10.0f, 0.0f, 0.1f}, {0.0f, 0.0f, 10.0f, 0.0f}};
    extLoadShadowMtx(mt);
    extLoadShadowRenderMode();
    extLoadShadowVertex();
    extLoadShadowTev();
    extDrawShadow();
}

void nannpc_ext_init() {
    s32 i;
    ExtPose* pose = extGetPosePtr();
    for (i = extGetPoseNum() - 1; i >= 0; i--, pose++) {
        pose->flags = 3;
        pose->rot = 0.0f;
        PSMTXIdentity(*pose->mtx);
        (*pose->mtx)[0][3] = 0.0f;
        (*pose->mtx)[1][3] = -1000.0f;
        (*pose->mtx)[2][3] = 0.0f;
    }
}

void nannpc_ext_main_sub() {
    s32 i;
    NanNPCWork* w = nanNPCWork;
    for (i = extGetPoseNum() - 1; i >= 0; i--) {
        NanNPC* p = &w->npc[i];
        if (p->name[0] && p->func) {
            p->func(p);
        }
    }
}

void nannpc_ext_main_sub_fast() {
    s32 i;
    NanNPC* p;
    NanNPCWork* w = nanNPCWork;
    for (i = extGetPoseNum() - 1; i >= 0; i--) {
        p = &w->npc[i];
        if (p->name[0]) {
            if (p->func) {
                p->func(p);
            }
            if (p->animtbl) {
                p->anim = p->animtbl[(gp->retraceCount + i * i) % p->animnum];
            }
            p->pose->poseId = p->anim;
            p->pose->rot = -p->dir;
            if (p->flags & 2) {
                (*p->pose->mtx)[0][3] = 0.0f;
                (*p->pose->mtx)[1][3] = -1000.0f;
                (*p->pose->mtx)[2][3] = 0.0f;
            } else {
                (*p->pose->mtx)[0][3] = p->pos.x;
                (*p->pose->mtx)[1][3] = p->pos.y;
                (*p->pose->mtx)[2][3] = p->pos.z;
            }
        }
    }
}

void nannpc_ext_main() {
    NanNPCWork* w = nanNPCWork;
    if (w->func14) {
        w->func14();
    }
    if (w->func18) {
        w->func18();
    }
    if (w->func1c) {
        w->func1c();
    }
}

void nannpc_ext_dispent() {
    if (!(nanNPCWork->flags & 1)) {
        dispEntry(4, 0.0f, 1, nannpc_ext_disp, 0);
        dispEntry(4, 0.0f, 2, nannpc_ext_shadow_disp, 0);
    } else {
        dispEntry(4, 0.0f, 2, nannpc_ext_disp, 0);
        dispEntry(4, 0.0f, 2, nannpc_ext_shadow_disp, 0);
    }
}

USER_FUNC(evt_nannpc_init) {
    s32* args = event->args;
    NanNPCWork* w;
    s32 i;
    s32 name = evtGetValue(event, args[0]);
    s16* tbl = (s16*)evtGetValue(event, args[1]);
    void* initf = (void*)evtGetValue(event, args[2]);
    void* mainf = (void*)evtGetValue(event, args[3]);
    void* dispf = nannpc_ext_dispent;
    if (isFirstCall) {
        return 0;
    }
    nanNPCWork = _mapAlloc(0x20);
    memset(nanNPCWork, 0, 0x20);
    w = nanNPCWork;
    w->num = evtGetValue(event, args[4]);
    w->npc = _mapAlloc(w->num * 0xC0);
    memset(w->npc, 0, w->num * 0xC0);
    w->sort = _mapAlloc(w->num * 8);
    memset(w->sort, 0, w->num * 8);
    w->tbl = _mapAlloc(0x14);
    memset(w->tbl, 0, 0x14);
    w->func14 = NULL;
    nanNPCWork->func18 = nannpc_ext_main_sub;
    w->func1c = NULL;
    if (!initf) {
        initf = nannpc_ext_init;
    }
    if (!mainf) {
        mainf = nannpc_ext_main;
    }
    extEntry(w->num, (void*)name, initf, mainf, dispf);
    for (i = 0; i < w->num; i++) {
        NanNPC* p = &w->npc[i];
        p->name[0] = 0;
        p->pose = NULL;
        p->flags = 0;
        p->field_16 = 0;
        p->pos = (Vec){0.0f, -1000.0f, 0.0f};
        p->field_30 = (Vec){0.0f, -1000.0f, 0.0f};
        p->hosei = (Vec){0.0f, 0.0f, 0.0f};
        p->sizeX = 32.0f;
        p->sizeY = 32.0f;
        p->startTime = 0;
        p->animtbl = NULL;
        p->animnum = 0;
        p->anim = 0;
        p->dir = 0.0f;
        p->func = NULL;
        p->color = (GXColor){255, 255, 255, 255};
    }
    if (!tbl) {
        return 2;
    }
    for (i = 0; i < 5; i++) {
        if (tbl[i * 2] == -1) {
            break;
        }
        w->tbl[i * 2] = tbl[i * 2];
        w->tbl[i * 2 + 1] = tbl[i * 2 + 1];
    }
    return 2;
}

USER_FUNC(evt_nannpc_set_subfunc) {
    s32* args = event->args;
    s32 kind = evtGetValue(event, args[0]);
    NanNPCWork* w = nanNPCWork;
    switch (kind) {
        case 0:
            w->func14 = (void (*)())evtGetValue(event, args[1]);
            break;
        case 1:
            w->func18 = (void (*)())evtGetValue(event, args[1]);
            break;
        case 2:
            w->func1c = (void (*)())evtGetValue(event, args[1]);
            break;
    }
    return 2;
}

s32 evt_nannpc_entry(EventEntry *event, BOOL isFirstCall)
{
  NanNPC *p;
  ExtPose *pose;
  char *str;
  s32 off;
  s32 name;
  s32 i;
  s32 *args;
  NanNPCWork *w;
  args = event->args;
  pose = extGetPosePtr();
  name = (long long) evtGetValue(event, args[0]);
  w = nanNPCWork;
  for (i = 0, off = 0; i < w->num; off += sizeof(NanNPC), i++)
  {
    p = (NanNPC *) (((u8 *) w->npc) - (-off));
    str = (char *) name;
    if (!(((u32) str) & 0x80000000))
    {
 do { } while (0);
      sprintf(makestring, "%06x", name);
      str = makestring;
    }
    strcmp((char *) p->name, str);
    if (!p->pose)
    {
      strcpy((char *) p->name, str);
      p->pose = &pose[i];
      p->flags = 0;
      p->field_16 = 0;
      p->pos = (Vec){0.0f, -1000.0f, 0.0f};
      p->field_30 = (Vec){0.0f, -1000.0f, 0.0f};
      p->hosei = (Vec){0.0f, 0.0f, 0.0f};
      p->sizeX = 32.0f;
      p->sizeY = 32.0f;
      p->startTime = 0;
      p->animnum = 0;
      p->dir = 0.0f;
      p->pose->rot = -p->dir;
      p->color = (GXColor){255, 255, 255, 255};
      break;
    }
  }

  return 2;
}

USER_FUNC(evt_nannpc_set_position) {
    s32* args = event->args;
    NanNPC* npc = nanNPCSearch((char*)evtGetValue(event, args[0]));
    f32 x = evtGetFloat(event, args[1]);
    npc->pos.x = x;
    npc->field_30.x = x;
    x = evtGetFloat(event, args[2]);
    npc->pos.y = x;
    npc->field_30.y = x;
    x = evtGetFloat(event, args[3]);
    npc->pos.z = x;
    npc->field_30.z = x;
    return 2;
}

USER_FUNC(evt_nannpc_get_position) {
    s32* args = event->args;
    NanNPC* npc = nanNPCSearch((char*)evtGetValue(event, args[0]));
    evtSetFloat(event, args[1], npc->pos.x);
    evtSetFloat(event, args[2], npc->pos.y);
    evtSetFloat(event, args[3], npc->pos.z);
    return 2;
}

USER_FUNC(evt_nannpc_set_hosei_position) {
    s32* args = event->args;
    NanNPC* npc = nanNPCSearch((char*)evtGetValue(event, args[0]));
    npc->hosei.x = evtGetFloat(event, args[1]);
    npc->hosei.y = evtGetFloat(event, args[2]);
    npc->hosei.z = evtGetFloat(event, args[3]);
    return 2;
}

USER_FUNC(evt_nannpc_set_size) {
    s32* args = event->args;
    NanNPC* npc = nanNPCSearch((char*)evtGetValue(event, args[0]));
    npc->sizeX = evtGetFloat(event, args[1]);
    npc->sizeY = evtGetFloat(event, args[2]);
    return 2;
}

void evt_nannpc_set_animtbl_sub(NanNPC* npc, s16* tbl) {
    s32 n;
    npc->animtbl = tbl;
    n = 0;
    while (TRUE) {
        if (*tbl == -1) {
            break;
        }
        tbl++;
        n++;
    }
    npc->animnum = n;
    if (npc->animnum == 0) {
        npc->animtbl = NULL;
    }
}

USER_FUNC(evt_nannpc_set_animtbl) {
    s32* args = event->args;
    NanNPC* npc = nanNPCSearch((char*)evtGetValue(event, args[0]));
    s16* tbl = (s16*)evtGetValue(event, args[1]);
    s32 n;
    npc->animtbl = tbl;
    n = 0;
    while (TRUE) {
        if (*tbl == -1) {
            break;
        }
        tbl++;
        n++;
    }
    npc->animnum = n;
    if (npc->animnum == 0) {
        npc->animtbl = NULL;
    }
    return 2;
}

USER_FUNC(evt_nannpc_set_dir) {
    s32* args = event->args;
    NanNPC* npc = nanNPCSearch((char*)evtGetValue(event, args[0]));
    npc->dir = evtGetValue(event, args[1]);
    return 2;
}

USER_FUNC(evt_nannpc_set_work) {
    s32* args = event->args;
    NanNPC* npc = nanNPCSearch((char*)evtGetValue(event, args[0]));
    s32 idx = evtGetValue(event, args[1]);
    npc->work[idx] = evtGetValue(event, args[2]);
    return 2;
}

USER_FUNC(evt_nannpc_get_work) {
    s32* args = event->args;
    NanNPC* npc = nanNPCSearch((char*)evtGetValue(event, args[0]));
    evtSetValue(event, args[2], npc->work[evtGetValue(event, args[1])]);
    return 2;
}

USER_FUNC(evt_nannpc_move_position2) {
    NanNPC* npc;
    OSTime now;
    s32* args;
    f32 speed;
    s32 total;
    s32 elapsed;
    Vec target;
    Vec diff;
    Vec step;
    args = event->args;
    npc = nanNPCSearch((char*)evtGetValue(event, args[0]));
    if (gp->inBattle) {
        now = gp->renderTime;
    } else {
        now = gp->renderFieldTime;
    }
    target.x = evtGetValue(event, args[1]);
    target.y = evtGetValue(event, args[2]);
    target.z = evtGetValue(event, args[3]);
    speed = evtGetFloat(event, args[4]);
    if (isFirstCall) {
        event->savedTime = now;
        npc->startPos = npc->pos;
    }
    PSVECSubtract(&target, &npc->startPos, &diff);
    total = (s32)(1000.0f * PSVECMag(&diff) / speed);
    elapsed = (u64)(s32)((u32)now - (u32)event->savedTime) / (OS_TIMER_CLOCK / 1000);
    PSVECScale(&diff, &step, (f32)elapsed / (f32)total);
    if (elapsed < total) {
        PSVECAdd(&npc->startPos, &step, &npc->pos);
        return 0;
    }
    npc->pos = target;
    return 2;
}

USER_FUNC(evt_nannpc_jump_position) {
    s32* args = event->args;
    NanNPC* npc = nanNPCSearch((char*)evtGetValue(event, args[0]));
    f32 x = evtGetValue(event, args[1]);
    f32 y = evtGetValue(event, args[2]);
    f32 z = evtGetValue(event, args[3]);
    s32 duration = evtGetValue(event, args[4]);
    f32 height = evtGetValue(event, args[5]);
    s32 elapsed;
    s32 el;
    u32 uel;
    f32 t;
    if (isFirstCall) {
        npc->startTime = gp->renderTime;
        npc->ipos[0] = npc->pos.x;
        npc->ipos[1] = npc->pos.y;
        npc->ipos[2] = npc->pos.z;
        event->userdataf[1] = -4.0f * height;
        event->userdataf[2] = y - (f32)npc->ipos[1] - event->userdataf[1];
    }
    el = ((u32)gp->renderTime - (u32)npc->startTime) / (OS_TIMER_CLOCK / 1000);
    uel = el;
    if (el < duration) {
        elapsed = uel;
        t = (f32)elapsed / (f32)duration;
        npc->pos.x = intplGetValue(0, elapsed, duration, npc->ipos[0], x);
        npc->pos.z = intplGetValue(0, uel, duration, npc->ipos[2], z);
        npc->pos.y = t * (event->userdataf[1] * t + event->userdataf[2]) + (f32)npc->ipos[1];
        return 0;
    }
    npc->pos.x = x;
    npc->pos.y = y;
    npc->pos.z = z;
    return 2;
}

USER_FUNC(evt_nannpc_flag_onoff) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 onoff = evtGetValue(event, args[1]);
    NanNPC* npc = nanNPCSearch((char*)name);
    u16 flag = evtGetValue(event, args[2]);
    if (onoff) {
        npc->flags &= ~flag;
    } else {
        npc->flags |= flag;
    }
    return 2;
}

USER_FUNC(evt_nannpc_ppflag_onoff) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 onoff = evtGetValue(event, args[1]);
    NanNPC* npc = nanNPCSearch((char*)name);
    u16 flag = evtGetValue(event, args[2]);
    if (onoff) {
        npc->pose->flags &= ~flag;
    } else {
        npc->pose->flags |= flag;
    }
    return 2;
}

USER_FUNC(evt_nannpc_set_func) {
    s32* args = event->args;
    NanNPC* npc = nanNPCSearch((char*)evtGetValue(event, args[0]));
    npc->func = (void (*)(NanNPC*))evtGetValue(event, args[1]);
    return 2;
}

USER_FUNC(evt_nannpc_set_shadow_position) {
    s32* args = event->args;
    NanNPC* npc = nanNPCSearch((char*)evtGetValue(event, args[0]));
    npc->pose->flags |= 0x10;
    npc->pose->shadowY = evtGetFloat(event, args[1]);
    return 2;
}

USER_FUNC(evt_nannpc_set_color) {
    s32* args = event->args;
    NanNPC* npc = nanNPCSearch((char*)evtGetValue(event, args[0]));
    npc->color.r = evtGetValue(event, args[1]);
    npc->color.g = evtGetValue(event, args[2]);
    npc->color.b = evtGetValue(event, args[3]);
    npc->color.a = evtGetValue(event, args[4]);
    return 2;
}

#pragma force_active on
#pragma explicit_zero_data on
USER_FUNC(evt_nannpcwork_flag_onoff) {
    s32* args = event->args;
    s32 onoff = evtGetValue(event, args[0]);
    NanNPCWork* w = nanNPCWork;
    u16 flag = evtGetValue(event, args[1]);
    if (onoff) {
        w->flags &= ~flag;
    } else {
        w->flags |= flag;
    }
    return 2;
}
#pragma explicit_zero_data reset
#pragma force_active reset

#pragma force_active on
#pragma explicit_zero_data on
__declspec(section ".sdata") u32 gap_07_804113E8_sdata[6] = {0, 0, 0, 0, 0, 0};
#pragma explicit_zero_data reset
#pragma force_active reset
