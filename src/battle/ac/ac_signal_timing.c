#pragma optimize_for_size on
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_pad.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "pmario_sound.h"
#include "system.h"
#include <string.h>

u16 BattleACGetButtonIcon(s32 button, s32 pressed);
void btlDispTexPlane(u32 id, GXColor color, s32 unk, f32 tX, f32 tY, f32 tZ, f32 sX, f32 sY);

typedef struct SignalTimingWork {
    s32 hits;       // 0x0
    s32 misses;     // 0x4
    s32 index;      // 0x8
    s32 next;       // 0xC
    s32 frame;      // 0x10
    s32 rangeStart; // 0x14
    s32 rangeEnd;   // 0x18
    s32 okStart;    // 0x1C
    s32 okEnd;      // 0x20
    s32 pressed;    // 0x24
    s32 unk28;      // 0x28
    s32 difficulty; // 0x2C
    s32 okFlag;     // 0x30
    u32 button;     // 0x34
    u32 buttons[5]; // 0x38
    s8 results[8];  // 0x4C
    s32 endTimer;   // 0x54
} SignalTimingWork;

#define W32(w, off) (*(s32*)((u8*)(w) + (off)))
#define ST_FLAGS(w) W32(w, 0x1CC4)
#define ST_COUNT(w) W32(w, 0x1CC8)
#define ST_STEP(w) W32(w, 0x1CD0)
#define ST_LEAD(w) W32(w, 0x1CE0)
#define ST_CONF(work) ((work->actionCommands.field_0x4 & 1) && (work->actionCommands.mAcUnit->mTokenFlags & kConfusedAction))

u32 _button_table_4dir[4] = {0x10000, 0x20000, 0x40000, 0x80000};
u32 _button_table_abxy[4] = {0x100, 0x200, 0x400, 0x800};
s32 ac_signaltiming_ok_frame_range[7] = {20, 16, 12, 10, 6, 4, 2};
s32 ac_signaltiming_ok_frame_range_2[7] = {30, 30, 30, 30, 25, 20, 15};

static void actionCommandDisp(f32 x, f32 y);
s32 _get_ok_frame_range(s32 difficulty);

#pragma dont_inline on
s32 _get_ok_frame_range(s32 difficulty) {
    BattleWork* work = _battleWorkPointer;
    switch (W32(work, 0x1CCC)) {
        case 1:
            return W32(work, 0x1CE4) + ac_signaltiming_ok_frame_range_2[difficulty];
    }
    return W32(work, 0x1CE4) + ac_signaltiming_ok_frame_range[difficulty];
}

#pragma dont_inline reset
s32 battleAcMain_SignalTiming(BattleWork *work)
{
  const char *sfxPi;
  const char *sfxPone;
  const char *sfxOk;
  const char *sfxNg;
  s32 j;
  s32 i;
  s32 k;
  BOOL unique;
  BattleActions *ac = &work->actionCommands;
  SignalTimingWork *w = (SignalTimingWork *) (((u8 *) ac) + 0x2BC);
  BattleACDispParams *disp = (BattleACDispParams *) (((u8 *) ac) + 0x290);
  BOOL loop;
  BOOL autoCommand = 0;
  BOOL got;
  BOOL good;
  u32 btn;
  BOOL done;
  if (ac->mAcUnit->badgesEquipped.AutoCommand)
  {
    autoCommand = 1;
  }
  sfxNg = "SFX_AC_COMMAND_NG1\0\0\0\0\0";
  sfxPi = "SFX_AC_PI1";
  sfxPone = "SFX_AC_PONE1";
  sfxOk = "SFX_AC_COMMAND_OK1";
  do
  {
    loop = 0;
    got = 0;
    switch (ac->mAcState)
    {
      case 0:
        ac->result = 1;
        memset(disp, 0, sizeof(BattleACDispParams));
        disp->iconX = -300.0f;
        disp->iconY = 0.0f;
        *((s32 *) (&disp->field_0x1C[4])) = 20;
        w->hits = 0;
        w->misses = 0;
        w->frame = 0;
        w->index = 0;
        w->next = 0;
        w->difficulty = ((s32 (*)(BattleWork *)) BattleActionCommandGetDifficulty)(work);
        w->okFlag = 0;
        *((s32 *) (((u8 *) work) + 0x1CEC)) = 0;
        for (k = 0; k < 5; k++)
      {
        w->results[k] = 0;
        if ((!w->hits) && (!w->hits))
        {
        }
      }

        switch (*((s32 *) (((u8 *) work) + 0x1CD8)))
      {
        case 1:

        case 2:
          for (i = 0; i < (*((s32 *) (((u8 *) work) + 0x1CC8))); i++)
        {
          done = 0;
          while (!done)
          {
            if ((*((s32 *) (((u8 *) work) + 0x1CD8))) == 1)
            {
              btn = _button_table_4dir[irand(4)];
            }
            else
            {
              btn = _button_table_abxy[irand(4)];
            }
            unique = 1;
            for (j = i - 1; j >= 0; j--)
            {
              if (w->buttons[j] == btn)
              {
                unique = 0;
              }
            }

            if (unique)
            {
              done = 1;
            }
          }

          w->buttons[i] = btn;
        }

          w->button = w->buttons[0];
          break;

        default:
          w->button = 0x100;
          for (i = 0; i < (*((s32 *) (((u8 *) work) + 0x1CC8))); i++)
        {
          w->buttons[i] = 0x100;
        }

          w->button = w->buttons[0];

      }

        ac->mAcState = 99;
        break;

      case 99:
        break;

      case 100:
        if ((*((s16 *) (((u8 *) work) + 0x1D18))) > 0)
      {
        (*((s16 *) (((u8 *) work) + 0x1D18)))--;
        ac->result = 2;
        loop = 1;
        ac->mAcState = 1002;
        break;
      }
        w->rangeStart = w->index * (*((s32 *) (((u8 *) work) + 0x1CD0)));
        w->rangeEnd = ((w->index + 1) * (*((s32 *) (((u8 *) work) + 0x1CD0)))) - 1;
        w->okStart = w->rangeStart;
        j = _get_ok_frame_range(w->difficulty) - 1;
        w->okEnd = w->okStart + j;
        ac->mAcState = 1000;

      case 1000:
        w->unk28 = ((w->next + 1) * (*((s32 *) (((u8 *) work) + 0x1CD0)))) - 1;
        w->next++;
        if (w->next < ((*((s32 *) (((u8 *) work) + 0x1CE0))) + 1))
      {
        if (!((work->actionCommands.field_0x4 & 1) && (work->actionCommands.mAcUnit->mTokenFlags & kConfusedAction)))
        {
          psndSFXOn(sfxPi);
        }
      }
      else
        if (!((work->actionCommands.field_0x4 & 1) && (work->actionCommands.mAcUnit->mTokenFlags & kConfusedAction)))
      {
        psndSFXOn(sfxPone);
      }
        w->pressed = 0;
        ac->mAcState = 1001;

      case 1001:
        w->frame++;
        if (w->pressed)
      {
        w->okFlag = 0;
      }
      else
        if (w->index >= (*((s32 *) (((u8 *) work) + 0x1CE0))))
      {
        if ((w->frame >= w->okStart) && (w->frame <= w->okEnd))
        {
          w->okFlag = 1;
        }
        else
        {
          w->okFlag = 0;
        }
      }
      else
      {
        w->okFlag = 0;
      }
        good = 0;
        if (BattlePadCheckTrigger(w->button))
      {
        got = 1;
      }
        if (got)
      {
        switch (*((s32 *) (((u8 *) work) + 0x1CD8)))
        {
          case 2:
            if (w->button == BattlePadGetTrigger())
          {
            good = 1;
          }
          else
          {
            good = 0;
          }
            break;

          default:
            good = 1;

        }

      }
        if (autoCommand && w->okFlag)
      {
        got = 1;
        good = 1;
      }
        if ((w->hits + w->misses) >= (*((s32 *) (((u8 *) work) + 0x1CC8))))
      {
        got = 0;
      }
        if (((w->index >= (*((s32 *) (((u8 *) work) + 0x1CE0)))) && got) && (!w->pressed))
      {
        w->pressed = 1;
        if (((w->frame >= w->okStart) && (w->frame <= w->okEnd)) && good)
        {
          w->hits++;
          *((s32 *) (((u8 *) work) + 0x1CEC)) = 1;
          if ((w->index >= (*((s32 *) (((u8 *) work) + 0x1CE0)))) && (w->index < ((*((s32 *) (((u8 *) work) + 0x1CC8))) + (*((s32 *) (((u8 *) work) + 0x1CE0))))))
          {
            w->results[w->index - (*((s32 *) (((u8 *) work) + 0x1CE0)))] = 1;
            if (!((work->actionCommands.field_0x4 & 1) && (work->actionCommands.mAcUnit->mTokenFlags & kConfusedAction)))
            {
              psndSFXOn(sfxOk);
            }
          }
          if ((*((s32 *) (((u8 *) work) + 0x1CC4))) & 0x10)
          {
            *((s32 *) (((u8 *) work) + 0x1CE8)) = w->hits;
            ac->result &= ~1;
            if (w->hits >= (*((s32 *) (((u8 *) work) + 0x1CD4))))
            {
              ac->result |= 2;
            }
            loop = 1;
            ac->mAcState = 1002;
          }
        }
        else
          if ((w->index >= (*((s32 *) (((u8 *) work) + 0x1CE0)))) && (w->index < ((*((s32 *) (((u8 *) work) + 0x1CC8))) + (*((s32 *) (((u8 *) work) + 0x1CE0))))))
        {
          w->misses++;
          if (w->frame >= w->rangeStart)
          {
            w->results[w->index - (*((s32 *) (((u8 *) work) + 0x1CE0)))] = -1;
            if (!((work->actionCommands.field_0x4 & 1) && (work->actionCommands.mAcUnit->mTokenFlags & kConfusedAction)))
            {
              psndSFXOn(sfxNg);
            }
            *((s32 *) (((u8 *) work) + 0x1CEC)) = -1;
          }
          else
          {
            w->pressed = 0;
          }
        }
      }
        if ((!w->pressed) && (w->frame > w->okEnd))
      {
        w->pressed = 1;
        *((s32 *) (((u8 *) work) + 0x1CEC)) = -1;
        if ((w->index >= (*((s32 *) (((u8 *) work) + 0x1CE0)))) && (w->index < ((*((s32 *) (((u8 *) work) + 0x1CC8))) + (*((s32 *) (((u8 *) work) + 0x1CE0))))))
        {
          w->misses++;
          w->results[w->index - (*((s32 *) (((u8 *) work) + 0x1CE0)))] = -1;
          if (!((work->actionCommands.field_0x4 & 1) && (work->actionCommands.mAcUnit->mTokenFlags & kConfusedAction)))
          {
            psndSFXOn(sfxNg);
          }
        }
      }
        if (w->pressed && (w->index < ((*((s32 *) (((u8 *) work) + 0x1CC8))) + (*((s32 *) (((u8 *) work) + 0x1CE0))))))
      {
        w->index++;
        if (w->index < ((*((s32 *) (((u8 *) work) + 0x1CC8))) + (*((s32 *) (((u8 *) work) + 0x1CE0)))))
        {
          w->rangeStart = (w->index * (*((s32 *) (((u8 *) work) + 0x1CD0)))) - 10;
          w->rangeEnd = ((w->index + 1) * (*((s32 *) (((u8 *) work) + 0x1CD0)))) - 1;
          w->okStart = w->index * (*((s32 *) (((u8 *) work) + 0x1CD0)));
          j = _get_ok_frame_range(w->difficulty) - 1;
          w->okEnd = w->okStart + j;
          w->pressed = 0;
          if ((w->index >= (*((s32 *) (((u8 *) work) + 0x1CE0)))) && (w->index < ((*((s32 *) (((u8 *) work) + 0x1CC8))) + (*((s32 *) (((u8 *) work) + 0x1CE0))))))
          {
            w->button = w->buttons[w->index - (*((s32 *) (((u8 *) work) + 0x1CE0)))];
          }
        }
      }
        if (w->frame >= (w->next * (*((s32 *) (((u8 *) work) + 0x1CD0)))))
      {
        if (w->next < ((*((s32 *) (((u8 *) work) + 0x1CC8))) + (*((s32 *) (((u8 *) work) + 0x1CE0)))))
        {
          ac->mAcState = 1000;
        }
        else
        {
          *((s32 *) (((u8 *) work) + 0x1CE8)) = w->hits;
          ac->result &= ~1;
          if (w->hits >= (*((s32 *) (((u8 *) work) + 0x1CD4))))
          {
            ac->result |= 2;
          }
          loop = 1;
          ac->mAcState = 1002;
        }
      }
        break;

      case 1002:
        ac->mAcState = 1003;
        break;

      case 1003:
        break;

      case 1004:
        *((s32 *) (((u8 *) work) + 0x1CC0)) |= 1;
        w->okFlag = 0;
        w->endTimer = 60;
        ac->mAcState = 1005;
        break;

      case 1005:
        w->endTimer--;
        if (w->endTimer <= 0)
      {
        ac->mAcState = 1006;
      }
        break;

      case 1006:
        ac->maincb = (void *) 0;
        ac->dispcb = (void *) 0;
        ac->resultcb = (void *) 0;
        ac->deletecb = (void *) 0;
        break;

    }

  }
  while (loop);
  return 1;
}

s32 battleAcResult_SignalTiming(BattleWork* work) {
    return work->actionCommands.result;
}

void battleAcDisp_SignalTiming(CameraId cameraId, BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    BattleACDispParams* disp = (BattleACDispParams*)((u8*)ac + 0x290);

    switch (ac->mAcState) {
        case 99:
        case 100:
            disp->iconX = ((f32(*)(f32, f32, s32, s32, s32))intplGetValue)(-300.0f, 30.0f, 4, 20 - *(s32*)&disp->field_0x1C[4], 20);
            actionCommandDisp(disp->iconX, disp->iconY);
            if (*(s32*)&disp->field_0x1C[4] > 0) {
                (*(s32*)&disp->field_0x1C[4])--;
            }
            break;
        case 1000:
        case 1001:
        case 1002:
        case 1003:
            actionCommandDisp(30.0f, 0.0f);
            break;
        case 1004:
        case 1005:
            if (*(s32*)&disp->field_0x1C[4] >= 40) {
                disp->iconX = ((f32(*)(f32, f32, s32, s32, s32))intplGetValue)(30.0f, -300.0f, 4, *(s32*)&disp->field_0x1C[4] - 40, 20);
            } else {
                disp->iconX = 30.0f;
            }
            actionCommandDisp(disp->iconX, disp->iconY);
            if (*(s32*)&disp->field_0x1C[4] < 60) {
                (*(s32*)&disp->field_0x1C[4])++;
            }
            break;
    }
}

void battleAcDelete_SignalTiming(BattleWork* work) {
    work->actionCommands.mAcState = 1004;
}

static void actionCommandDisp(f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
    BattleActions* ac = &work->actionCommands;
    SignalTimingWork* w;
    s32 i;
    s32 count;
    s32 next;
    s32 n;
    s32 num;
    s32 num2;
    f32 yb;
    f32 yd;
    f32 fx;
    f32 yc;
    Mtx mOut;
    Mtx mTrans;
    Mtx mScale;

    camGetPtr(8);
    w = (SignalTimingWork*)((u8*)ac + 0x2BC);
    count = ST_COUNT(work);
    next = ((SignalTimingWork*)((u8*)&work->actionCommands + 0x2BC))->next;
    if (!(ST_FLAGS(work) & 1)) {
        if (w->okFlag) {
            Vec pos = {0.0f, 0.0f, 0.0f};
            pos.x = -200.0f + x;
            pos.y = 70.0f + y;
            iconDispGx(&pos, 0x10, 0x65, 1.0f);
        } else {
            Vec pos = {0.0f, 0.0f, 0.0f};
            pos.x = -200.0f + x;
            pos.y = 70.0f + y;
            iconDispGx(&pos, 0x10, 0x64, 1.0f);
        }
    }

    switch (count) {
        case 1:
            n = ST_LEAD(work) * 2 + 5;
            break;
        case 3:
            n = ST_LEAD(work) * 2 + 11;
            break;
        case 4:
            n = ST_LEAD(work) * 2 + 13;
            break;
        case 5:
            n = ST_LEAD(work) * 2 + 15;
            break;
        default:
            n = ST_LEAD(work) * 2 + 11;
            break;
    }

    {
        Vec pos = {0.0f, 0.0f, 0.0f};
        pos.x = -288.0f + x;
        pos.y = yb = 25.0f + y;
        iconDispGx(&pos, 0x10, 0x8F, 1.0f);
    }
    for (i = 1; i < n; i++) {
        Vec pos = {0.0f, 0.0f, 0.0f};
        pos.x = x + (f32)(i * 16 - 288);
        pos.y = yb;
        iconDispGx(&pos, 0x10, 0x8E, 1.0f);
    }
    {
        Vec pos = {0.0f, 0.0f, 0.0f};
        pos.x = x + (f32)(i * 16 - 288);
        pos.y = yb;
        iconDispGx(&pos, 0x10, 0x90, 1.0f);
    }

    if (!(ST_FLAGS(work) & 4)) {
        fx = -200.0f + x;
        num = w->hits + W32(work, 0x1CDC);
PSMTXTrans(mTrans, fx + (f32)((ST_FLAGS(work) & 1) ? count * 0x30 - 26 + ST_LEAD(work) * 32 : count * 0x30 - 26 + ST_LEAD(work) * 32), yb, 0.0f);
        if (ST_FLAGS(work) & 8) {
            PSMTXScale(mScale, 1.5f, 1.5f, 1.5f);
        } else {
            PSMTXScale(mScale, 1.0f, 1.0f, 1.0f);
        }
        PSMTXConcat(mTrans, mScale, mOut);
        iconNumberDispGx(mOut, num, 0, (GXColor){255, 255, 255, 255});
    }

    for (i = 0; i < ST_LEAD(work); i++) {
        if (i < w->next) {
            if (i < ST_LEAD(work)) {
                Vec pos = {0.0f, 0.0f, 0.0f};
                pos.x = x + (f32)(i * 33 - 258);
                pos.y = yb;
                iconDispGx(&pos, 0x10, (u16)(0x92 + i), 1.0f);
            } else {
                Vec pos = {0.0f, 0.0f, 0.0f};
                pos.x = x + (f32)(i * 33 - 258);
                pos.y = yb;
                iconDispGx(&pos, 0x10, 0x94, 1.0f);
            }
        } else {
            Vec pos = {0.0f, 0.0f, 0.0f};
            pos.x = x + (f32)(i * 33 - 258);
            pos.y = yb;
            iconDispGx(&pos, 0x10, 0x91, 0.5f);
        }
    }

    if (!(ST_FLAGS(work) & 1)) {
        for (i = 0; i < count; i++) {
            if (i + ST_LEAD(work) < next) {
                Vec pos = {0.0f, 0.0f, 0.0f};
                pos.x = x + (f32)(i * 48 + (ST_LEAD(work) * 33 - 250));
                pos.y = yb;
                iconDispGx(&pos, 0x10, 0x95, 1.0f);
            } else {
                Vec pos = {0.0f, 0.0f, 0.0f};
                pos.x = x + (f32)(i * 48 + (ST_LEAD(work) * 33 - 250));
                pos.y = yb;
                iconDispGx(&pos, 0x10, 0x91, 1.0f);
            }
        }
    } else if (!(ST_FLAGS(work) & 2)) {
        yc = 15.0f + y;
        for (i = 0; i < count; i++) {
            f32 scale = 0.8f;
            if (w->next == (i + ST_LEAD(work)) && w->frame >= ST_STEP(work) * (i + ST_LEAD(work)) - 5 && w->frame < ST_STEP(work) * (i + ST_LEAD(work))) {
                Vec pos = {0.0f, 0.0f, 0.0f};
                pos.x = x + (f32)(i * 48 + (ST_LEAD(work) * 33 - 250));
                pos.y = yc;
                iconDispGx(&pos, 0x10, BattleACGetButtonIcon(w->buttons[i], 0), 0.8f);
            } else {
                s32 dy;
                if (w->next <= (i + ST_LEAD(work))) {
                    scale = 0.6f;
                    dy = -2;
                } else {
                    dy = -10;
                }
                {
                Vec pos = {0.0f, 0.0f, 0.0f};
                pos.x = x + (f32)(i * 48 + (ST_LEAD(work) * 33 - 250));
                pos.y = (f32)(dy + 25) + y;
                iconDispGx(&pos, 0x10, BattleACGetButtonIcon(w->buttons[i], 1), scale);
                }
            }
        }
    } else {
        yc = 20.800001f + yb;
        for (i = 0; i < count; i++) {
            if (w->next == (i + ST_LEAD(work)) && w->frame >= ST_STEP(work) * (i + ST_LEAD(work)) - 20 && w->frame <= ST_STEP(work) * (i + ST_LEAD(work))) {
                Vec pos = {0.0f, 0.0f, 0.0f};
                pos.x = x + (f32)(i * 48 + (ST_LEAD(work) * 33 - 250));
                pos.y = yb;
                iconDispGx(&pos, 0x10, BattleACGetButtonIcon(w->buttons[i], 0), 0.8f);
            } else if (w->next <= (i + ST_LEAD(work))) {
                if (i >= count - 1) {
                    btlDispTexPlane(0x40, (GXColor){255, 255, 255, 255}, 0, x + (f32)(i * 48 + (ST_LEAD(work) * 33 - 250)), yc, 0.0f, 0.8f, 0.8f);
                }
            } else {
                Vec pos = {0.0f, 0.0f, 0.0f};
                pos.x = x + (f32)(i * 48 + (ST_LEAD(work) * 33 - 250));
                pos.y = yb;
                iconDispGx(&pos, 0x10, BattleACGetButtonIcon(w->buttons[i], 1), 0.8f);
            }
        }
    }

    yd = yb - 25.0f;
    for (i = 0; i < count; i++) {
        if (w->results[i] == 1) {
            btlDispTexPlane(0x3E, (GXColor){255, 255, 255, 255}, 0, x + (f32)(i * 48 + (ST_LEAD(work) * 33 - 250)), yd, 0.0f, 1.0f, 1.0f);
        } else if (w->results[i] == -1) {
            btlDispTexPlane(0x3F, (GXColor){255, 255, 255, 255}, 0, x + (f32)(i * 48 + (ST_LEAD(work) * 33 - 250)), yd, 0.0f, 1.0f, 1.0f);
        }
    }
}

