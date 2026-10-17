#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_disp.h"
#include "battle/battle_pad.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "pmario_sound.h"
#include "system.h"
#include <string.h>

s32 BattleACGetButtonIcon(s32 button, s32 pressed);

#pragma optimize_for_size on

typedef struct GT2Params {
    u8 field_0x0[0x3C];
    s32 totalFrames; // 0x3C
    s32 count;       // 0x40
    s32 bonus;       // 0x44
} GT2Params;

typedef struct GT2Disp {
    u8 field_0x0[0xC];
    GXColor color; // 0xC
    u8 field_0x10[0x4];
    f32 iconX; // 0x14
    f32 iconY; // 0x18
    u8 field_0x1C[0x4];
    s32 timer; // 0x20
    u8 field_0x24[0x4];
    f32 ratio; // 0x28
} GT2Disp;

typedef struct GT2Work {
    s32 frame;       // 0x0
    f32 cursor;      // 0x4
    s32 frame2;      // 0x8
    s32 totalFrames; // 0xC
    s32 hit;         // 0x10
    s32 button;      // 0x14
    f32 targets[8];  // 0x18
    s8 state[8];     // 0x38
    s8 flag[8];      // 0x40
    u32 sfx[8];      // 0x48
    u16 width;       // 0x68
    s8 miss;         // 0x6A
    u8 pad;
    s32 endTimer; // 0x6C
} GT2Work;

#define GT_PARAMS(work) ((GT2Params*)&(work)->actionCommands)
#define GT_DISP(work) ((GT2Disp*)((u8*)&(work)->actionCommands + 0x290))
#define GT_WORK(work) ((GT2Work*)((u8*)&(work)->actionCommands + 0x2BC))
#define ICON ((void (*)(Vec*, s16, s32, f32))iconDispGx)
#define TEXPLANE ((void (*)(u32, GXColor, s32, f32, f32, f32, f32, f32))btlDispTexPlane)

static void actionCommandDisp(f32 x, f32 y);

s32 battleAcMain_GaugeTiming2(BattleWork *work)
{
  const char *sfxNg;
  BOOL pressed;
  BOOL allDone;
  const char *new_var[2];
  BOOL found;
  s32 i3;
  s32 i4;
  GT2Work *gw = (GT2Work *) (((u8 *) (&work->actionCommands)) + 0x2BC);
  BOOL autoCommand = 0;
  GT2Disp *disp = (GT2Disp *) (((u8 *) (&work->actionCommands)) + 0x290);
  s32 i;
  BOOL doAuto;
  const char *sfxOk;
  if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand)
  {
    autoCommand = 1;
  }
  switch (work->actionCommands.mAcState)
  {
    case 0:
      memset(disp, 0, 0x2C);
      disp->iconX = -300.0f;
      disp->iconY = 0.0f;
      disp->timer = 20;
      disp->ratio = 0.0f;
      disp->color = (GXColor){255, 255, 255, 255};
      gw->frame = 0;
      gw->frame2 = 0;
      work->actionCommands.mAcState = 99;
      work->actionCommands.result = 1;
      gw->totalFrames = ((GT2Params *) (&work->actionCommands))->totalFrames;
      gw->hit = 0;
      gw->cursor = 0.0f;
      work->actionCommands.outputParams[0] = 0;
      gw->width = ((GT2Params *) (&work->actionCommands))->count * 60;
      for (i = 0; i < ((GT2Params *) (&work->actionCommands))->count; i++)
    {
      gw->targets[i] = (f32) ((100 / ((GT2Params *) (&work->actionCommands))->count) * (i + 1));
      gw->targets[i] -= 4.0f;
      gw->state[i] = 0;
      gw->flag[i] = 0;
      gw->sfx[i] = -1;
    }

      gw->button = 0x100;
      work->actionCommands.outputParams[1] = 0;
      break;

    case 99:
      break;

    case 100:
      if ((work->actionCommands.field_0x4 & 1) && (work->actionCommands.mAcUnit->mTokenFlags & kConfusedAction))
    {
      if (irand(100) >= 0)
      {
        work->actionCommands.field_0x8 = 30;
        gw->miss = 0;
      }
      else
      {
        gw->miss = 1;
        work->actionCommands.field_0x8 = 30;
      }
      work->actionCommands.mAcState = 1001;
      break;
    }
      work->actionCommands.mAcState = 1000;

    case 1000:
      gw->frame++;
      pressed = 0;
      if (BattlePadCheckTrigger(gw->button))
    {
      pressed = 1;
    }
      allDone = 1;
      for (i = 0; i < ((GT2Params *) (&work->actionCommands))->count; i++)
    {
      if (!gw->state[i])
      {
        allDone = 0;
        break;
      }
    }

      if ((gw->frame > gw->totalFrames) && allDone)
    {
      if (((s32) work->actionCommands.outputParams[0]) > 0)
      {
        work->actionCommands.result = 2;
        work->actionCommands.mResultCount++;
      }
      else
      {
        work->actionCommands.result = 0;
      }
      work->actionCommands.mAcState = 1002;
      break;
    }
      gw->frame2++;
      gw->cursor = (100.0f * ((f32) gw->frame2)) / ((f32) gw->totalFrames);
    {
      new_var[0] = "SFX_AC_COMMAND_NG1";
      for (i3 = 0; i3 < ((GT2Params *) (&work->actionCommands))->count; i3++)
      {
        if ((!gw->state[i3]) && (gw->cursor > (6.0f + gw->targets[i3])))
        {
          gw->state[i3] = -1;
          psndSFXOn(new_var[0]);
        }
      }

    }
      gw->hit = 0;
    {
      new_var[1] = "SFX_AC_PONE1";
      for (i = 0; i < ((GT2Params *) (&work->actionCommands))->count; i++)
      {
        if ((!gw->state[i]) && (__fabs(gw->cursor - gw->targets[i]) <= 6.0))
        {
          gw->hit = 1;
          if (gw->sfx[i] == (-1))
          {
            gw->sfx[i] = ((s32 (*)(const char *)) psndSFXOn)(new_var[1]);
          }
        }
      }

    }
      doAuto = 0;
      if (autoCommand)
    {
      BOOL hit2 = 0;
      s32 j;
      for (j = 0; j < ((GT2Params *) (&work->actionCommands))->count; j++)
      {
        if ((!gw->state[j]) && (__fabs(gw->cursor - gw->targets[j]) <= 6.0))
        {
          hit2 = 1;
          break;
        }
      }

      if (hit2)
      {
        doAuto = 1;
      }
    }
      if (pressed || doAuto)
    {
      sfxNg = "SFX_AC_COMMAND_NG1";
      sfxOk = "SFX_AC_COMMAND_OK1\0";
      found = 0;
      for (i4 = 0; i4 < ((GT2Params *) (&work->actionCommands))->count; i4++)
      {
        if ((!gw->state[i4]) && (!found))
        {
          f64 d = __fabs(gw->cursor - gw->targets[i4]);
          if (d <= 6.0)
          {
            if ((!doAuto) && gw->flag[i4])
            {
              gw->state[i4] = -1;
              psndSFXOn(sfxNg);
              found = 1;
            }
            else
            {
              gw->state[i4] = 1;
              work->actionCommands.outputParams[0]++;
              psndSFXOn(sfxOk);
              found = 1;
            }
          }
          else
            if (d < __fabs(gw->targets[1] - gw->targets[0]))
          {
            gw->flag[i4] = 1;
            if (i4 == (((GT2Params *) (&work->actionCommands))->count - 1))
            {
              gw->state[i4] = -1;
              psndSFXOn(sfxNg);
              found = 1;
            }
          }
        }
      }

    }
      break;

    case 1001:
      work->actionCommands.field_0x8--;
      if (work->actionCommands.field_0x8 < 0)
    {
      if (gw->miss)
      {
        work->actionCommands.result = 2;
        work->actionCommands.mResultCount++;
        work->actionCommands.outputParams[0] = irand(((GT2Params *) (&work->actionCommands))->count) + 1;
      }
      else
      {
        work->actionCommands.result = 0;
        work->actionCommands.outputParams[0] = 0;
      }
      work->actionCommands.field_0x30 |= 1;
      return 0;
    }
      break;

    case 1002:
      work->actionCommands.mAcState = 1003;

    case 1003:
      work->actionCommands.outputParams[1] = 1;
      work->actionCommands.field_0x30 |= 1;
      gw->endTimer = 60;
      work->actionCommands.mAcState = 1004;

    case 1004:
      gw->endTimer--;
      if (gw->endTimer <= 0)
    {
      work->actionCommands.mAcState = 1005;
    }
      break;

    case 1005:
      work->actionCommands.maincb = (void *) 0;
      work->actionCommands.dispcb = (void *) 0;
      work->actionCommands.resultcb = (void *) 0;
      work->actionCommands.deletecb = (void *) 0;
      return 0;

  }

  return 1;
}

s32 battleAcResult_GaugeTiming2(BattleWork* work) {
    return work->actionCommands.result;
}

#pragma opt_propagation off
void battleAcDisp_GaugeTiming2(CameraId cameraId, BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    GT2Disp* disp = (GT2Disp*)((u8*)ac + 0x290);
    switch (ac->mAcState) {
        case 99:
            disp->iconX = ((f32(*)(f32, f32, s32, s32, s32))intplGetValue)(-300.0f, 30.0f, 4, 20 - disp->timer, 20);
        case 100:
        case 1000:
        case 1002:
            actionCommandDisp(disp->iconX, disp->iconY);
            if (disp->timer > 0) {
                disp->timer--;
            }
            break;
        case 1003:
        case 1004:
            if (disp->timer >= 40) {
                disp->iconX = ((f32(*)(f32, f32, s32, s32, s32))intplGetValue)(30.0f, -300.0f, 4, disp->timer - 40, 20);
            } else {
                disp->iconX = 30.0f;
            }
            actionCommandDisp(disp->iconX, disp->iconY);
            if (disp->timer < 60) {
                disp->timer++;
            }
            break;
    }
}

#pragma opt_propagation reset
void battleAcDelete_GaugeTiming2(BattleWork* work) {
    if (!(work->actionCommands.field_0x30 & 1)) {
        work->actionCommands.mAcState = 1003;
    }
}

#pragma dont_inline on
#pragma opt_loop_invariants off
static void actionCommandDisp(f32 x, f32 y) {
    BattleActions* ac = &_battleWorkPointer->actionCommands;
    GT2Disp* disp = (GT2Disp*)((u8*)ac + 0x290);
    GT2Work* gw;
    CameraEntry* cam;
    f32 x0; f32 y0; f32 y1; f32 x1; f32 y2; f32 y3; f32 ratio; f32 t;
    s32 q; s32 r; s32 v; s32 i; s32 icon; s32 pressedIcon; s32 id;
    s32 cnt;
    Vec pos;
    Vec d0; Vec d1;

    camGetPtr(8);
    gw = (GT2Work*)((u8*)ac + 0x2BC);
    cam = camGetPtr(8);
    v = gw->width - 16;
    x0 = (-288.0f) + x;
    y0 = 49.0f + y;
    y1 = y0 - 8.0f;
    q = 0;
    r = 0;
    if (v > 0) {
        q = (v + 3) / 4;
        r = v - q * 4;
    }
    btlDispGXInit2DRasta();
    GXLoadPosMtxImm(cam->view, 0);
    TEXPLANE(0x43, (GXColor){255, 255, 255, 255}, 0, x0, y1, 0.0f, 1.0f, 1.0f);
    t = 2.0f + (8.0f + x0);
    for (i = 0; i < q; i++) {
        if (i != q - 1) {
            TEXPLANE(0x44, (GXColor){255, 255, 255, 255}, 0, t + (f32)(i * 4), y1, 0.0f, 1.0f, 1.0f);
        } else {
            TEXPLANE(0x44, (GXColor){255, 255, 255, 255}, 0, (f32)r + (t + (f32)(i * 4)), y1, 0.0f, 1.0f, 1.0f);
        }
    }
    TEXPLANE(0x43, (GXColor){255, 255, 255, 255}, 0, 16.0f + x0 + (f32)v, y1, 0.0f, -1.0f, 1.0f);
    ratio = gw->cursor / 100.0f;
    if (ratio > 1.0f) {
        ratio = 1.0f;
    }
    disp->ratio = ratio;

    cam = camGetPtr(8);
    y2 = y0 - 16.0f;
    x1 = x0 + (((f32)gw->width) * gw->cursor) / 100.0f;
    if (x1 >= x0) {
        btlDispGXInit2DRasta();
        GXLoadPosMtxImm(cam->view, 0);
        btlDispGXQuads2DRasta(x0, y0, x1 - 1.0f, y2, 255, 0, 0, 255);
    }

    y3 = 23.0f + y;
    cnt = *(s32*)((u8*)ac + 0x40);
    for (i = 0; i < cnt; i++) {
        f32 target = gw->targets[i];
        if (target <= gw->cursor || __fabs(gw->cursor - target) <= 6.0) {
            id = 0x95;
        } else {
            id = 0x91;
        }
        pos = (Vec){0.0f, 0.0f, 0.0f};
        d0 = (Vec){0.0f, 0.0f, 0.0f};
        d1 = (Vec){0.0f, 0.0f, 0.0f};
        pos.x = (- 288.0f + x) + ((f32)gw->width) * target / 100.0f;
        pos.y = y3;
        ICON(&pos, 0x10, id, 0.75f);
    }

    pressedIcon = BattleACGetButtonIcon(0x100, 1);
    icon = BattleACGetButtonIcon(0x100, 0);
    switch (ac->mAcState) {
        case 99:
        case 100:
        case 1002:
        case 1003:
        case 1004: {
            Vec p = {0.0f, 0.0f, 0.0f};
            Vec e0 = {0.0f, 0.0f, 0.0f};
            Vec e1 = {0.0f, 0.0f, 0.0f};
            p.x = 28.0f + (-225.0f + x);
            p.y = 55.0f + y;
            ICON(&p, 0x10, icon, 1.0f);
            break;
        }
        case 1000:
            if (gw->hit) {
                Vec p = {0.0f, 0.0f, 0.0f};
                Vec e0 = {0.0f, 0.0f, 0.0f};
                Vec e1 = {0.0f, 0.0f, 0.0f};
                p.x = 28.0f + (-225.0f + x);
                p.y = 55.0f + y;
                ICON(&p, 0x10, pressedIcon, 1.0f);
            } else {
                Vec p = {0.0f, 0.0f, 0.0f};
                p.x = 28.0f + (-225.0f + x);
                p.y = 55.0f + y;
                ICON(&p, 0x10, icon, 1.0f);
            }
            break;
    }

    y1 = 25.0f + y;
    {
    f32 y4 = y1 - 25.0f;
    for (i = 0; i < ((GT2Params*)ac)->count; i++) {
        switch (gw->state[i]) {
            case 1:
                id = 0x3E;
                break;
            case -1:
                id = 0x3F;
                break;
            default:
                id = 0x45;
                break;
        }
        if ((u16)id != 0x45) {
            TEXPLANE((u16)id, (GXColor){255, 255, 255, 255}, 0, - 288.0f + x + (((f32)gw->width) * gw->targets[i]) / 100.0f, y4, 0.0f, 1.0f, 1.0f);
        }
    }
    }

    {
        Mtx m3; Mtx m1; Mtx m2;
        s32 num = ac->outputParams[0] + ((GT2Params*)ac)->bonus;
        PSMTXTrans(m1, -200.0f + x + ((f32)gw->width) - 60.0f, y1, 0.0f);
        PSMTXScale(m2, 1.0f, 1.0f, 1.0f);
        PSMTXConcat(m1, m2, m3);
        iconNumberDispGx(m3, num, FALSE, (GXColor){255, 255, 255, 255});
    }
}
#pragma opt_loop_invariants reset
#pragma dont_inline reset
