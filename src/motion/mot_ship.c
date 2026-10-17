#include "dolphin/types.h"
#include "evt/evt_cmd.h"
#include "evt/evt_snd.h"
#include "driver/hitdrv.h"
#include "mario/mario.h"
#include "mario/mario_sbr.h"
#include "memory.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include "mario/mario_cam.h"
#include "mario/mario_motion.h"
#include "manager/evtmgr.h"
#include "pmario_sound.h"
#include "system.h"
#include <string.h>
#include "mario/mariost.h"

typedef struct ShipWork {
	u32 flags; // 0x0
	u8 pad4[0x28 - 4];
	f32 swirlY; // 0x28
	f32 swirlX; // 0x2C
	s32 swirlTime; // 0x30
} ShipWork;

typedef struct ShipDir {
	u32 flags;
	u8 pad4[0x14 - 4];
	f32 dispX; // 0x14
	f32 dispY; // 0x18
} ShipDir;

static EVT_BEGIN(sound_evt)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_SHIP1_1"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_SHIP1_1"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_SHIP1_1"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_SHIP1_1"), LW(0), LW(1), LW(2), 0)
RETURN
EVT_END()

EVT_BEGIN(sound_evt2)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_SHIP2"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_SHIP2"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_SHIP2"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_SHIP2"), LW(0), LW(1), LW(2), 0)
RETURN
EVT_END()

f32 shipData[6] = {-0.05f, -0.7f, 0.02f, 0.02f, 2.0f, 0.01f};
const char* paper_ship[4] = {"p_hune", "p_hune_g", "p_hune_y", "p_hune_v"};

void sincosf(f32* s, f32* c, f32 a);

static inline s32 harborKind(HitEntry* hit) {
	s32 k = 0;
	s32 attr = hitGetAttr(hit);
	if (attr & 0x4000) {
		k = 1;
	} else if (attr & 0x8000) {
		k = 2;
	} else if (attr & 0x10000) {
		k = 3;
	} else if (attr & 0x20000) {
		k = 4;
	}
	return k;
}

#define HARBOR_PROBE(ANG, POS)  sincosf(&sn, &cs, toMovedir(ANG)); tx = 40.0f * sn + m->position.x; tz = 40.0f * cs + m->position.z; ty = 20.0f + m->position.y; dist = 40.0f; hit = hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, tx, ty, tz, 0.0f, -1.0f, 0.0f); if (hit != NULL && harborKind(hit) != 0) { hitObjGetPos(hitGetName(hit), &POS); ix = (s32)((100.0f * POS.x + 0.5f) / 100.0f); iy = (s32)((100.0f * POS.y + 0.5f) / 100.0f); iz = (s32)((100.0f * POS.z + 0.5f) / 100.0f); out->x = ix; out->y = iy; out->z = iz; return TRUE; }

BOOL checkHarbor(Vec* out);

#define MSF(m, o) (*(f32*)((u8*)(m) + (o)))
#define MSI(m, o) (*(s32*)((u8*)(m) + (o)))
#define MSP(m, o) (*(void**)((u8*)(m) + (o)))
#define MSO(o) (*(s32*)((u8*)marioGetPtr()->motStruct + (o)))
#define MSOF(o) (*(f32*)((u8*)marioGetPtr()->motStruct + (o)))

void allPartyRideOff(void);
void allPartyForceRideOn(void);
void* effFunemizuEntry(s32, f32, f32, f32, f32, f32);

void marioPreJump(void);
void allPartyRideShip(void);
void marioSetFallPara(void);
void marioClearJumpPara(void);
void N_marioReloadMapOnBottomlessOff(void);

f32 distABf(f32 x1, f32 y1, f32 x2, f32 y2);
f32 angleABf(f32 x1, f32 y1, f32 x2, f32 y2);
void movePos(float* ret_sin, float* ret_cos, f32 a3, f32 degree);
double __fabs(double);
void marioMakeJumpPara();
void N_marioSetBottomlessRespawnPosOnBeroEntry();
s32 marioChkLandon(f32, f32*);

#define MS_ARM() (*(f32*)((u8*)marioGetPtr()->motStruct + 0x1c))
#define SHIP_DIR (MSI(marioGetPtr()->motStruct, 0) & 1)

void marioCheckWallShip(f32, f32);
void marioPaperOff(void);
double sin(double);
double cos(double);
void shipMove(void);
#define EFFP() (*(void**)((u8*)*(void**)((u8*)marioGetPtr()->motStruct + 0x24) + 0xC))
#define SHIP_EFF()                                                                                                 \
	{                                                                                                              \
		f30 = MSF(m, 0x180);                                                                                       \
		{                                                                                                          \
			MarioWork* mm = marioGetPtr();                                                                         \
			f32 zz = 0.0f;                                                                                         \
			if (0.0f == f30) {                                                                                     \
				s32 c;                                                                                             \
				marioGetPtr();                                                                                     \
				c = (MSOF(8) > 180.0f) ? 0 : 1;                                                                    \
				f30 = c ? 0.3f : 0.6f;                                                                             \
				{                                                                                                  \
					f32* ms = (f32*)marioGetPtr()->motStruct;                                                      \
					f32 old = ms[4];                                                                               \
					ms[4] = old + 0.1f * (f30 - MSOF(0x10));                                                       \
				}                                                                                                  \
				f30 = MSOF(0x10);                                                                                  \
			}                                                                                                      \
			f31 = mm->position.x;                                                                                  \
			MSF(EFFP(), 4) = f31;                                                                                  \
			f31 = mm->position.y + zz;                                                                           \
			MSF(EFFP(), 8) = f31;                                                                                  \
			f31 = mm->position.z;                                                                                  \
			MSF(EFFP(), 0xc) = f31;                                                                                \
			MSF(EFFP(), 0x10) = f30;                                                                               \
			f31 = MSF(mm, 0x1a8);                                                                                  \
			MSF(EFFP(), 0x14) = f31;                                                                               \
		}                                                                                                          \
	}

s32 N_marioShipGetDir(void) {
	marioGetPtr();
	if (marioGetPtr()->motStruct != NULL) {
		return (((ShipDir*)marioGetPtr()->motStruct)->flags >> 1) & 1;
	}
	return 0;
}

s32 marioShipChanging(void) {
	MarioWork* m = marioGetPtr();
	if (m->currMotionId != MARIO_MOTION_SHIP) {
		return 0;
	}
	if ((s32)m->currSubMotionId == 10) {
		return 2;
	}
	if ((s32)m->currSubMotionId >= 0 && (s32)m->currSubMotionId <= 12 || (s32)m->currSubMotionId >= 30 && (s32)m->currSubMotionId <= 48) {
		return 1;
	}
	if ((s32)m->currSubMotionId == 50 || (s32)m->currSubMotionId == 52) {
		return 3;
	}
	return 0;
}

void marioSetSwirlPower(s32 msec, f32 a, f32 b) {
	s32 t;
	marioGetPtr();
	t = sysMsec2Frame(msec);
	((ShipWork*)marioGetPtr()->motStruct)->swirlTime = t;
	((ShipWork*)marioGetPtr()->motStruct)->swirlX = a;
	((ShipWork*)marioGetPtr()->motStruct)->swirlY = b;
}

BOOL marioChkSwirlPower(void) {
	marioGetPtr();
	return ((ShipWork*)marioGetPtr()->motStruct)->swirlTime != 0;
}

void marioShipForceStop(void) {
	f32 a = 0.0f;
	f32 b;
	marioGetPtr();
	b = 0.0f;
	((ShipDir*)marioGetPtr()->motStruct)->dispX = a;
	((ShipDir*)marioGetPtr()->motStruct)->dispY = b;
}

void marioShipSetDispDir(void) {
	f32 a = 0.0f;
	f32 b;
	marioGetPtr();
	b = 0.0f;
	((ShipDir*)marioGetPtr()->motStruct)->dispX = a;
	((ShipDir*)marioGetPtr()->motStruct)->dispY = b;
}

BOOL marioChkShipPlace(HitEntry* hit) {
	return (hitGetAttr(hit) & 0x3C000) != 0;
}

void marioForceShipAnime(void) {
	MarioWork* m = marioGetPtr();
	marioPaperOff();
	marioChgPaper(NULL);
	marioPaperOn(paper_ship[marioGetColor()]);
	marioChgPaper("PM_H_1B");
	marioChgPose("M_Z_1");
	m->dispFlags |= 8;
}

void marioReInit_ship(void) {
	MarioWork* m = marioGetPtr();
	void* p;
	s32 t, u;
	f32 a, b;
	m->trigFlags &= ~1;
	p = __memAlloc(HEAP_DEFAULT, 0x48);
	marioGetPtr()->motStruct = p;
	memset(marioGetPtr()->motStruct, 0, 0x48);
	*(s16*)((u8*)m + 0x2E) = 0x19;
	MSF(m, 0x9C) = -15.0f;
	MSI(m, 0x44) = 0x32;
	MSF(m, 0x1B4) = 40.0f;
	MSF(m, 0x1B8) = 40.0f;
	marioPaperOn(paper_ship[marioGetColor()]);
	marioChgPaper("PM_H_1B");
	marioChgPose("M_Z_1");
	m->dispFlags |= 8;
	MSI(m, 0x2A4) = 0x58;
	marioSetPaperAnimeLocalTime(MSI(m, 0x2A4));
	t = -1;
	MSO(0x38) = t;
	u = 0;
	MSO(0) = u;
	a = 0.0f;
	MSOF(4) = a;
	b = 0.0f;
	MSOF(0x14) = b;
	a = 0.0f;
	MSOF(0x18) = a;
	b = 0.0f;
	MSOF(0x1C) = b;
	a = 0.0f;
	MSOF(0x20) = a;
	b = 0.0f;
	MSOF(8) = b;
	a = 4.25f;
	MSOF(0xC) = a;
	t = (s32)effFunemizuEntry(0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	MSO(0x24) = t;
	u = MSI(m, 0x134);
	*(s32*)((u8*)*(void**)((u8*)*(void**)((u8*)marioGetPtr()->motStruct + 0x24) + 0xC) + 0x48) = u;
	allPartyForceRideOn();
}

void mot_ship_post(void) {
	MarioWork* m = marioGetPtr();
	s32 t;
	allPartyRideOff();
	if (m->dispFlags & 0x40000000) {
		marioPaperOff();
	}
	MSF(m, 0x1B4) = 20.0f;
	MSF(m, 0x1B8) = 37.0f;
	marioGetPtr();
	if (marioGetPtr()->motStruct != NULL) {
		if ((u32)MSO(0x38) != 0xFFFFFFFFU) {
			psndSFXOff(MSO(0x38));
			t = -1;
			MSO(0x38) = t;
		}
	}
	m->dispFlags &= ~4;
	m->dispFlags &= ~0x100;
	m->dispFlags &= ~8;
	marioOfsRotReset();
	if (marioGetPtr()->motStruct != NULL) {
		__memFree(HEAP_DEFAULT, marioGetPtr()->motStruct);
		if ((!m) && (!m)) {
		}
		t = 0;
		marioGetPtr()->motStruct = (void*)t;
	}
}

void mot_ship(void) {
	MarioWork* m = marioGetPtr();
	MarioWork* m2;
	s32 r29, r27;
	f32 f30, f31;
	Vec pos;
	Vec p1, p2;

	if (m->trigFlags & 1) {
		m->trigFlags &= ~1;
		MSI(m, 0) &= ~0xF0000;
		m2 = (MarioWork*)__memAlloc(0, 0x48);
		marioGetPtr()->motStruct = m2;
		memset(marioGetPtr()->motStruct, 0, 0x48);
		m2 = (MarioWork*)-1;
		MSO(0x38) = (s32)m2;
		MSOF(0x20) = 0.0f;
		MSOF(8) = 0.0f;
		MSOF(0xC) = 4.25f;
		m->currSubMotionId = 0;
		if (!(m->trigFlags & 4)) {
			camGetPtr(8)->flags |= 0x200;
		}
	}
	switch (m->currSubMotionId) {
	case 0:
		psndSFXOn_3D((const char*)0x18d, &m->position);
		psndSFXOn_3D((const char*)0xaf, &m->position);
		marioCamZoomUp();
		marioPreJump();
		marioChgPose("M_J_1B");
		camFollowYOff();
		m->dispFlags |= 0x100;
		MSI(m, 0x48) = 0;
		m->currSubMotionId = 1;
		*(s16*)((u8*)m + 0x50) = 0;
		*(s16*)((u8*)m + 0x52) = 0;
		MSF(m, 0x180) = 0.0f;
		MSOF(0x1c) = 0.0f;
		N_marioSetBottomlessRespawnPosOnBeroEntry(m->position.x, m->position.y, m->position.z);
		{
			HitEntry* hit = (HitEntry*)MSP(m, 0x1E8);
			s32 attr;
			m2 = marioGetPtr();
			attr = hitGetAttr(hit);
			r29 = 0;
			if (attr & 0x4000) {
				hitObjGetPos(hitGetName((HitEntry*)MSP(m2, 0x1E8)), &p1);
				f30 = revise360(angleABf(m2->position.x, m2->position.z, p1.x, p1.z) - MSF(m2, 0x198));
				r29 = (f30 >= 180.0f) ? 2 : 1;
			} else if (attr & 0x8000) {
				hitObjGetPos(hitGetName((HitEntry*)MSP(m2, 0x1E8)), &p2);
				f30 = revise360(angleABf(m2->position.x, m2->position.z, p2.x, p2.z) - MSF(m2, 0x198));
				r29 = (f30 >= 180.0f) ? 2 : 1;
			} else if (attr & 0x10000) {
				r29 = 2;
			} else if (attr & 0x20000) {
				r29 = 1;
			}
			MSO(0) = r29;
		}
		/* fallthrough */
	case 1:
		marioMakeJumpPara();
		f31 = SHIP_DIR ? 30.0f : -30.0f;
		{
			f32* ms = (f32*)marioGetPtr()->motStruct;
			f32 old = ms[7];
			ms[7] = old + 0.125f * (f31 - MS_ARM());
		}
		if (SHIP_DIR) {
			f30 = 30.0f;
			if (MS_ARM() >= f30) {
				MS_ARM() = f30;
			}
		} else {
			f30 = -30.0f;
			if (MS_ARM() <= f30) {
				MS_ARM() = f30;
			}
		}
		MSF(m, 0x1A8) += MS_ARM();
		r27 = 0;
		if (SHIP_DIR) {
			if (MSF(m, 0x1A8) >= 270.0f) {
				r27 = 1;
			}
		} else {
			if (MSF(m, 0x1A8) <= -270.0f) {
				r27 = 1;
			}
		}
		if (r27) {
			m->currSubMotionId = 2;
			MSI(m, 0x48) = 20;
			marioPaperOn(paper_ship[marioGetColor()]);
			marioChgPaper("PM_H_1A");
			marioChgPose("M_Z_1");
			f30 = 15.0f;
			m->dispFlags |= 8;
			MSOF(0x20) = f30;
			{
				void* e = evtEntry(sound_evt, 0, 0);
				MSI(e, 0x9c) = (s32)(1024.0f * m->position.x) - 230000000;
				MSI(e, 0xa0) = (s32)(1024.0f * m->position.y) - 230000000;
				MSI(e, 0xa4) = (s32)(1024.0f * m->position.z) - 230000000;
			}
		}
		if (MSF(m, 0x7c) > 0.0f) {
			m->position.y += MSF(m, 0x7c);
			MSI(m, 0) &= ~0xF0000;
		}
		break;
	case 2:
		f30 = SHIP_DIR ? 1.0f : -1.0f;
		f31 = MSOF(0x20) * f30;
		{
			f32* ms = (f32*)marioGetPtr()->motStruct;
			f32 old = ms[7];
			ms[7] = old + 0.125f * (f31 - MS_ARM());
		}
		MSF(m, 0x1A8) += MS_ARM();
		r27 = 0;
		if (SHIP_DIR) {
			if (MSF(m, 0x1A8) >= 1260.0f) {
				r27 = 1;
			}
		} else {
			if (MSF(m, 0x1A8) <= -1080.0f) {
				r27 = 1;
			}
		}
		if (r27) {
			MSI(m, 0x48) = 40;
			m->currSubMotionId = 3;
		}
		break;
	case 3:
		if (MSI(m, 0x48) == 14) {
			allPartyRideShip();
		}
		if (MSI(m, 0x48) == 20) {
			marioCamZoomOffReq2(900);
			camFollowYOn();
		}
		if (--MSI(m, 0x48) <= 0) {
			m->currSubMotionId = 4;
		}
		if (MSF(m, 0x9c) > -15.0f) {
			MSF(m, 0x9c) -= 1.0f;
			if (MSF(m, 0x9c) <= -15.0f) {
				MSF(m, 0x9c) = -15.0f;
			}
		}
	case 4:
		f30 = SHIP_DIR ? 1.0f : -1.0f;
		f31 = MSOF(0x20) * f30;
		{
			f32* ms = (f32*)marioGetPtr()->motStruct;
			f32 old = ms[7];
			ms[7] = old + 0.125f * (f31 - MS_ARM());
		}
		MSF(m, 0x1A8) += MS_ARM();
		MSOF(0x20) -= 0.1f;
		f30 = 1.0f;
		if (MSOF(0x20) <= f30) {
			MSOF(0x20) = f30;
		}
		r27 = 0;
		if (SHIP_DIR) {
			if (MSF(m, 0x1A8) >= 1620.0f) {
				MSF(m, 0x1A8) = 1620.0f;
				r27 = 1;
			}
		} else {
			if (MSF(m, 0x1A8) <= -1440.0f) {
				MSF(m, 0x1A8) = -1440.0f;
				r27 = 1;
			}
		}
		if (r27) {
			if (MSI(m, 0x44) == 4) {
				m->currSubMotionId = 10;
			}
		}
		break;
	case 10: {
		MarioWork* mm;
		void* ms;
		f32 fz, fx;
		s32 ix, iz;
		HitEntry* hit;
		s32 attr;
		m->dispFlags &= ~0x100;
		MSF(m, 0x1A8) = revise360(MSF(m, 0x1A8));
		MSF(m, 0x1B4) = 40.0f;
		MSF(m, 0x1B8) = 40.0f;
		ms = marioGetPtr()->motStruct;
		hit = (HitEntry*)MSP(m, 0x1E8);
		mm = marioGetPtr();
		hitObjGetPos(hitGetName(hit), &pos);
		ix = (s32)((100.0f * pos.x + 0.5f) / 100.0f);
		iz = (s32)((100.0f * pos.z + 0.5f) / 100.0f);
		MSF(ms, 0x40) = MSF(m, 0x90) - 15.0f;
		fx = ix;
		fz = iz;
		r27 = harborKind((HitEntry*)MSP(mm, 0x1E8));
		if (r27 == 1) {
			MSF(ms, 0x3c) = fx;
			MSF(ms, 0x44) = fz - 50.0f - 30.30303f;
		} else if (r27 == 2) {
			MSF(ms, 0x3c) = fx;
			MSF(ms, 0x44) = 30.30303f + (50.0f + fz);
		} else if (r27 == 3) {
			MSF(ms, 0x3c) = fx - 50.0f - 30.30303f;
			MSF(ms, 0x44) = fz;
		} else if (r27 == 4) {
			MSF(ms, 0x3c) = 30.30303f + (50.0f + fx);
			MSF(ms, 0x44) = fz;
		} else {
			MSF(ms, 0x3c) = fx;
			MSF(ms, 0x44) = fz;
		}
		f30 = 0.1f * distABf(MSOF(0x3c), MSOF(0x44), m->position.x, m->position.z);
		switch (r27) {
		case 1: MSOF(0x44) -= f30; break;
		case 2: MSOF(0x44) += f30; break;
		case 3: MSOF(0x3c) -= f30; break;
		case 4: MSOF(0x3c) += f30; break;
		}
		MSF(m, 0x180) = distABf(m->position.x, m->position.z, MSOF(0x3c), MSOF(0x44)) / 36.0f;
		MSF(m, 0x1a0) = angleABf(m->position.x, m->position.z, MSOF(0x3c), MSOF(0x44));
		MSF(m, 0x19c) = MSF(m, 0x1a0);
		MSOF(4) = MSOF(0x40);
		{
			f32 dd = MSOF(0x40) - m->position.y;
			MSF(m, 0x7c) = 0.0f;
			MSF(m, 0x80) = (dd + dd) / 306.0f;
		}
		MSF(m, 0x84) = 0.0f;
		MSF(m, 0x88) = 0.0f;
		MSI(m, 0x48) = 36;
		m->currSubMotionId = 11;
		break;
	}
	case 11: {
		s32 ok;
		u32 land;
		f32 ly;
		MarioWork* mm;
		movePos(&m->position.x, &m->position.z, MSF(m, 0x180), MSF(m, 0x1a0));
		m->position.y += MSF(m, 0x7c);
		marioMakeJumpPara();
		f30 = MSF(m, 0x7c);
		mm = marioGetPtr();
		MSF(mm, 0x1b4) = 20.0f;
		land = marioChkLandon(f30, &ly);
		MSF(mm, 0x1b4) = 40.0f;
		{
			f32 d;
			if (land && ((d = mm->position.y - ly) <= 0.0f || d + f30 <= 0.0f)) {
				MSI(mm, 0x1e4) = land;
				mm->position.y = ly;
				MSI(mm, 0) &= ~0xF0000;
				MSF(mm, 0x128) = __fabsf(distABf(mm->position.x, mm->position.z, MSF(mm, 0x11c), MSF(mm, 0x124)));
				MSI(mm, 0) &= ~0x80;
				*(s16*)((u8*)mm + 0x50) = 0;
				*(s16*)((u8*)mm + 0x52) = 0;
				camFollowYOn();
				ok = 1;
			} else {
				ok = 0;
			}
		}
		if (ok) {
			f30 = 0.0f;
			MSF(m, 0x7c) = f30;
			MSF(m, 0x80) = f30;
			MSF(m, 0x84) = f30;
			MSF(m, 0x88) = f30;
			MSF(m, 0x180) = f30;
			MSOF(0x1c) = f30;
			MSI(m, 0) &= ~0xF0000;
			MSF(m, 0x1a8) = revise360(MSF(m, 0x1a8));
			MSF(m, 0x1ac) = MSF(m, 0x1a8);
			MSF(m, 0x1a0) = toMovedir(MSF(m, 0x1a8));
			MSF(m, 0x19c) = MSF(m, 0x1a0);
			m->dispFlags &= ~0x100;
			m->dispFlags |= 0x1000;
			psndSFXOn_3D((const char*)0x18f, &m->position);
			MSO(0x24) = (s32)effFunemizuEntry(0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
			MSI(*(void**)((u8*)*(void**)((u8*)marioGetPtr()->motStruct + 0x24) + 0xC), 0x48) = MSI(m, 0x134);
			MSOF(8) = 0.0f;
			MSOF(0xC) = 4.25f;
			m->currSubMotionId = 50;
			if (!(m->trigFlags & 4)) {
				camGetPtr(8)->flags &= ~0x200;
			}
		}
		break;
	}
	case 30:
		if (!(m->trigFlags & 4)) {
			camGetPtr(8)->flags |= 0x200;
		}
		m->currSubMotionId = 31;
		/* fallthrough */
	case 31: {
		MSF(m, 0x9c) += -0.4f;
		if (MSF(m, 0x9c) <= -18.0f) {
			MSI(m, 0x48) = 4;
			m->currSubMotionId = 32;
		}
		f30 = 1.6f;
		{
			MarioWork* mm = marioGetPtr();
			f32 zz = 0.0f;
			f31 = mm->position.x;
			MSF(EFFP(), 4) = f31;
			f31 = mm->position.y + zz;
			MSF(EFFP(), 8) = f31;
			f31 = mm->position.z;
			MSF(EFFP(), 0xc) = f31;
			MSF(EFFP(), 0x10) = f30;
			f31 = MSF(mm, 0x1a8);
			MSF(EFFP(), 0x14) = f31;
		}
		break;
	}
	case 32:
		if (--MSI(m, 0x48) > 0) {
			break;
		}
		m->currSubMotionId = 40;
		/* fallthrough */
	case 40: {
		f32 tt, qq, dd;
		s32 n;
		void* ms2;
		marioGetPtr();
		if (marioGetPtr()->motStruct != NULL) {
			if ((u32)MSO(0x38) != 0xFFFFFFFFU) {
				psndSFXOff(MSO(0x38));
				r29 = -1;
				MSO(0x38) = r29;
			}
		}
		camFollowYOff();
		m->dispFlags &= ~0x1000;
		f31 = MSOF(0x40);
		MSOF(4) = f31;
		MSF(m, 0x180) = 2.8f;
		ms2 = marioGetPtr()->motStruct;
		dd = distABf(m->position.x, m->position.z, MSOF(0x3c), MSF(ms2, 0x44));
		n = (s32)((10.0f * (2.0f * dd / MSF(m, 0x180)) + 0.5f) / 10.0f);
		ms2 = marioGetPtr()->motStruct;
		MSF(m, 0x1a0) = angleABf(m->position.x, m->position.z, MSOF(0x3c), MSF(ms2, 0x44));
		MSF(m, 0x19c) = MSF(m, 0x1a0);
		r29 = ((m->trigFlags >> 9) & 1) + 1;
		MSO(0) = r29;
		tt = 0.5f * (f32)n;
		qq = 80.0f / (tt * tt - tt);
		f30 = 0.0f;
		MSF(m, 0x7c) = qq * tt;
		MSF(m, 0x80) = -qq;
		MSF(m, 0x84) = f30;
		MSF(m, 0x88) = f30;
		MSI(m, 0x48) = (s16)(s32)(0.5f + tt);
		m->currSubMotionId = 0x29;
		MSF(m, 0x2b0) = __fabsf(MSF(m, 0x9c) / (f32)(MSI(m, 0x48) + 0x58));
		effDelete((EffectEntry*)MSO(0x24));
		break;
	}
	case 41:
		movePos(&m->position.x, &m->position.z, MSF(m, 0x180), MSF(m, 0x1a0));
		m->position.y += MSF(m, 0x7c);
		marioMakeJumpPara();
		MSF(m, 0x9c) += MSF(m, 0x2b0);
		f30 = 0.0f;
		if (MSF(m, 0x9c) > f30) {
			MSF(m, 0x9c) = f30;
		}
		if (--MSI(m, 0x48) > 0) {
			break;
		}
		marioChgPaper("PM_H_1A");
		marioChgPose("M_Z_1");
		MSI(m, 0x2a4) = 0x58;
		marioSetPaperAnimeLocalTime(MSI(m, 0x2a4));
		f31 = 0.0f;
		MSOF(0x1c) = f31;
		m->dispFlags |= 0x100;
		marioAdjustMoveDir();
		m->currSubMotionId = 0x2b;
		psndSFXOn_3D((const char*)0x192, &m->position);
		{
			void* e = evtEntry(sound_evt2, 0, 0);
			MSI(e, 0x9c) = (s32)(1024.0f * m->position.x) - 230000000;
			MSI(e, 0xa0) = (s32)(1024.0f * m->position.y) - 230000000;
			MSI(e, 0xa4) = (s32)(1024.0f * m->position.z) - 230000000;
		}
		break;
	case 43:
		MSF(m, 0x9c) += MSF(m, 0x2b0);
		f30 = 0.0f;
		if (MSF(m, 0x9c) > f30) {
			MSF(m, 0x9c) = f30;
		}
		marioSetPaperAnimeLocalTime(MSI(m, 0x2a4));
		if ((MSI(m, 0x2a4) = MSI(m, 0x2a4) - 1) < 0x2c) {
			break;
		}
		f30 = SHIP_DIR ? 20.0f : -20.0f;
		{
			f32* ms = (f32*)marioGetPtr()->motStruct;
			f32 old = ms[7];
			ms[7] = old + 0.125f * (f30 - MS_ARM());
		}
		if (SHIP_DIR) {
			f31 = 20.0f;
			if (MS_ARM() >= f31) {
				MS_ARM() = f31;
			}
		} else {
			f31 = -20.0f;
			if (MS_ARM() <= f31) {
				MS_ARM() = f31;
			}
		}
		MSF(m, 0x1A8) += MS_ARM();
		if (MSI(m, 0x2a4) == 0x2c) {
			m->currSubMotionId = 0x2c;
		}
		break;
	case 44:
		MSF(m, 0x9c) += MSF(m, 0x2b0);
		f30 = 0.0f;
		if (MSF(m, 0x9c) > f30) {
			MSF(m, 0x9c) = f30;
		}
		marioSetPaperAnimeLocalTime(MSI(m, 0x2a4));
		if ((MSI(m, 0x2a4) = MSI(m, 0x2a4) - 1) <= 0) {
			MSI(m, 0x2a4) = 0;
			m->currSubMotionId = 0x2d;
			MSI(m, 0x48) = 6;
			MSF(m, 0x1A8) = revise360(MSF(m, 0x1A8));
			if (MSI(marioGetPtr()->motStruct, 0) & 2) {
				MSF(m, 0x1A8) = MSF(m, 0x1A8) - 360.0f;
			}
		}
		MSF(m, 0x1A8) += MS_ARM();
		break;
	case 45:
		marioSetPaperAnimeLocalTime(MSI(m, 0x2a4));
		f30 = SHIP_DIR ? 1.0f : -1.0f;
		f30 = MSOF(0x20) * f30;
		{
			f32* ms = (f32*)marioGetPtr()->motStruct;
			f32 old = ms[7];
			ms[7] = old + 0.05f * (f30 - MS_ARM());
		}
		MSF(m, 0x1A8) += MS_ARM();
		MSOF(0x20) += -0.005f;
		f31 = 10.0f;
		if (MSOF(0x20) <= f31) {
			MSOF(0x20) = f31;
		}
		if (--MSI(m, 0x48) > 0) {
			break;
		}
	case 46:
		f30 = SHIP_DIR ? 1.0f : -1.0f;
		f30 = MSOF(0x20) * f30;
		{
			f32* ms = (f32*)marioGetPtr()->motStruct;
			f32 old = ms[7];
			ms[7] = old + 0.05f * (f30 - MS_ARM());
		}
		MSF(m, 0x1A8) += MS_ARM();
		MSOF(0x20) += -0.005f;
		f31 = 10.0f;
		if (MSOF(0x20) <= f31) {
			MSOF(0x20) = f31;
		}
		r27 = 0;
		if (SHIP_DIR) {
			if (MSF(m, 0x1A8) >= 360.0f) {
				MSF(m, 0x1A8) = revise360(MSF(m, 0x1A8));
				r27 = 1;
			}
		} else {
			if (MSF(m, 0x1A8) <= -360.0f) {
				MSF(m, 0x1A8) = revise360(MSF(m, 0x1A8));
				r27 = 1;
			}
		}
		if (!r27) {
			break;
		}
		MSF(m, 0x1A8) = revise360(MSF(m, 0x1A8));
		if (MSI(marioGetPtr()->motStruct, 0) & 2) {
			MSF(m, 0x1A8) = MSF(m, 0x1A8) - 360.0f;
		}
		marioPaperOff();
		m->dispFlags &= ~8;
		marioChgPose("M_J_1B");
		m->currSubMotionId = 0x2f;
		f31 = 10.0f;
		MSOF(0x20) = f31;
	case 47:
		r27 = 0;
		marioGetPtr();
		marioGetPtr();
		f30 = SHIP_DIR ? 4.0f : -4.0f;
		{
			f32* ms = (f32*)marioGetPtr()->motStruct;
			f32 old = ms[7];
			ms[7] = old + 0.05f * (f30 - MS_ARM());
		}
		MSF(m, 0x1A8) += MS_ARM();
		if (SHIP_DIR) {
			if (MSF(m, 0x1A8) >= 540.0f) {
				MSF(m, 0x1A8) = 540.0f;
				r27 = 1;
			}
		} else {
			if (MSF(m, 0x1A8) <= -360.0f) {
				MSF(m, 0x1A8) = -360.0f;
				r27 = 1;
			}
		}
		if (MSI(m, 0x44) != 0x2f || !r27) {
			break;
		}
		MSF(m, 0x1A8) = revise360(MSF(m, 0x1A8));
		MSF(m, 0x1AC) = MSF(m, 0x1A8);
		MSF(m, 0x19c) = toMovedir(MSF(m, 0x1A8));
		MSF(m, 0x1a0) = MSF(m, 0x19c);
		m->dispFlags &= ~0x100;
		MSF(m, 0x1B4) = 20.0f;
		MSF(m, 0x1B8) = 37.0f;
		f30 = 0.0f;
		MSF(m, 0x7c) = f30;
		MSF(m, 0x80) = f30;
		MSF(m, 0x84) = f30;
		MSF(m, 0x88) = f30;
		MSF(m, 0x180) = f30;
		MSI(m, 0) &= ~0xF0000;
		MSF(m, 0x9c) = f30;
		marioChgMot(10);
		marioSetFallPara();
		allPartyRideOff();
		N_marioSetBottomlessRespawnPosOnBeroEntry(m->position.x, m->position.y, m->position.z);
		N_marioReloadMapOnBottomlessOff();
		if (marioGetPtr()->motStruct != NULL) {
			__memFree(HEAP_DEFAULT, marioGetPtr()->motStruct);
			r29 = 0;
			marioGetPtr()->motStruct = (void*)r29;
		}
		if (!(m->trigFlags & 4)) {
			camGetPtr(8)->flags &= ~0x200;
		}
		break;
	case 50:
		m->currSubMotionId = 0x34;
	case 52:
		if ((u32)MSI(m, 0x1e4) != 0) {
			f31 = MSOF(0xC);
			f31 = revise360(MSOF(8) - f31);
			MSOF(8) = f31;
			MSF(m, 0x9c) = 3.0f * (f32)sin(3.1415927f * MSOF(8) / 180.0f) + -13.0f;
		}
		if (MSI(m, 0) & 0x20) {
			SHIP_EFF()
			break;
		}
		if ((u32)MSI(m, 0x1e4) == 0) {
			*(u8*)((u8*)m + 0x24e) = 0;
			*(u8*)((u8*)m + 0x24f) = 0;
		}
		shipMove();
		if (MSI(m, 0) & 0x20000) {
			MarioWork* mm;
			s32 ok; u32 land;
			f32 ly;
			marioMakeJumpPara();
			if (MSF(m, 0x7c) <= -7.0f) {
				MSF(m, 0x7c) = -7.0f;
			}
			f30 = MSF(m, 0x7c);
			mm = marioGetPtr();
			MSF(mm, 0x1b4) = 20.0f;
			land = marioChkLandon(f30, &ly);
			MSF(mm, 0x1b4) = 40.0f;
			{
				f32 d;
				if (land && ((d = mm->position.y - ly) <= 0.0f || d + f30 <= 0.0f)) {
					MSI(mm, 0x1e4) = land;
					mm->position.y = ly;
					MSI(mm, 0) &= ~0xF0000;
					MSF(mm, 0x128) = __fabsf(distABf(mm->position.x, mm->position.z, MSF(mm, 0x11c), MSF(mm, 0x124)));
					MSI(mm, 0) &= ~0x80;
					*(s16*)((u8*)mm + 0x50) = 0;
					*(s16*)((u8*)mm + 0x52) = 0;
					camFollowYOn();
					ok = 1;
				} else {
					ok = 0;
				}
			}
			if (!ok) {
				m->position.y += MSF(m, 0x7c);
			}
		} else {
			marioClearJumpPara();
		}
		if (strcmp((char*)gp + 0x12c, "dou_03") == 0) {
			if (m->position.y <= 260.0f && m->position.y >= 50.0f) {
				f32 ang = MSF(m, 0x214);
				if (0.0f != (f32)(s32)(10.0f * ang + 0.5f) / 10.0f) {
					m->position.y = 2.0f * (f32)sin(3.1415927f * ang / 180.0f) + m->position.y;
					m->position.z = 2.0f * -(f32)cos(3.1415927f * MSF(m, 0x214) / 180.0f) + m->position.z;
				}
			}
		}
		MSI(m, 0x1e0) = 0;
		MSI(m, 0x1dc) = 0;
		MSI(m, 0x1ec) = 0;
		MSI(m, 0x1f0) = 0;
		marioCheckWallShip(MSF(m, 0x180), MSF(m, 0x1a0));
		if (MSF(m, 0x180) < 0.5f) {
			MSF(m, 0x180) = 0.0f;
			m->currSubMotionId = 0x32;
		}
		SHIP_EFF()
		if (*(u16*)((u8*)m + 0x248) & 0xa00) {
			if (checkHarbor((Vec*)((u8*)marioGetPtr()->motStruct + 0x3c))) {
				m->currSubMotionId = 0x1e;
			}
		}
		break;
	}
}

void marioShipChgFall(void) {
	MarioWork* m = marioGetPtr();
	if (!(m->flags & 0x20000)) {
		m->flags |= 0x20000;
		*(f32*)((u8*)m + 0x7C) = -0.1f;
		*(f32*)((u8*)m + 0x80) = -0.2f;
		*(f32*)((u8*)m + 0x84) = 0.024f;
		*(f32*)((u8*)m + 0x88) = -0.004f;
	}
}

BOOL marioChkShipMoveMode(void) {
	MarioWork* m = marioGetPtr();
	if (m->currMotionId != MARIO_MOTION_SHIP) {
		return FALSE;
	}
	if ((s32)m->currSubMotionId >= 50 && (s32)m->currSubMotionId <= 52) {
		return TRUE;
	}
	return FALSE;
}

double sin(double);
double cos(double);

void shipMove(void)
{
  MarioWork *m = marioGetPtr();
  f32 len;
  unsigned long long new_var;
  f32 vz;
  f32 spd;
  f32 tz;
  f32 vx;
  f32 tx;
  f32 max;
  f32 dx;
  f32 dz;
  f32 fr;
  s32 snd;
  spd = *((f32 *) (((u8 *) m) + 0x190));
  dx = 0.0f;
  dx = 0.0f;
  if (((ShipWork *) marioGetPtr()->motStruct)->swirlTime != 0)
  {
    ((ShipWork *) marioGetPtr()->motStruct)->swirlTime--;
    movePos(&dx, &dz, ((ShipWork *) marioGetPtr()->motStruct)->swirlX, ((ShipWork *) marioGetPtr()->motStruct)->swirlY);
    ((ShipDir *) marioGetPtr()->motStruct)->dispX += dx;
    ((ShipDir *) marioGetPtr()->motStruct)->dispY += dz;
    vz = shipData[4];
    if (((ShipDir *) marioGetPtr()->motStruct)->dispX > vz)
    {
      vz = shipData[4];
      ((ShipDir *) marioGetPtr()->motStruct)->dispX = vz;
    }
    if (((ShipDir *) marioGetPtr()->motStruct)->dispX < (-shipData[4]))
    {
      ((ShipDir *) marioGetPtr()->motStruct)->dispX = -shipData[4];
    }
    vz = shipData[4];
    if (((ShipDir *) marioGetPtr()->motStruct)->dispY > vz)
    {
      vz = shipData[4];
      ((ShipDir *) marioGetPtr()->motStruct)->dispY = vz;
    }
    if (((ShipDir *) marioGetPtr()->motStruct)->dispY < (-shipData[4]))
    {
      ((ShipDir *) marioGetPtr()->motStruct)->dispY = -shipData[4];
    }
  }
  vx = ((ShipDir *) marioGetPtr()->motStruct)->dispX;
  vz = ((ShipDir *) marioGetPtr()->motStruct)->dispY;
  new_var = 3;
  if (*((f32 *) (((u8 *) m) + 0x190)))
  {
    tx = spd * ((f32) sin((3.1415927f * (*((f32 *) (((u8 *) m) + 0x194)))) / 180.0f));
    tz = spd * (-((f32) cos((3.1415927f * (*((f32 *) (((u8 *) m) + 0x194)))) / 180.0f)));
  }
  else
  {
    tx = vx;
    tz = vz;
  }
  max = shipData[4];
  if (spd || (((ShipWork *) marioGetPtr()->motStruct)->swirlTime != 0))
  {
    if (tx > vx)
    {
      vx += ((1.0f + tx) - vx) * shipData[new_var];
      if (vx >= max)
      {
        vx = max;
      }
    }
    else
    {
      vx += ((tx - 1.0f) - vx) * shipData[new_var];
      if (vx <= (-max))
      {
        vx = -max;
      }
    }
    if (tz > vz)
    {
      vz += ((1.0f + tz) - vz) * shipData[new_var];
      if (max <= vz)
      {
        vz = max;
      }
    }
    else
    {
      vz += ((tz - 1.0f) - vz) * shipData[new_var];
      if (vz <= (-max))
      {
        vz = -max;
      }
    }
  }
  if (!spd)
  {
    fr = shipData[0];
    if (((u32) (*((s32 *) (((u8 *) m) + 0x1F0)))) != 0)
    {
      fr = shipData[1];
    }
    fr = __fabsf(fr);
    vx += (-vx) * fr;
    vz += (-vz) * fr;
    if (__fabsf(vx) < 0.01f)
    {
      vx = 0.0f;
    }
    if (__fabsf(vz) < 0.01f)
    {
      vz = 0.0f;
    }
  }
  ((ShipDir *) marioGetPtr()->motStruct)->dispX = vx;
  ((ShipDir *) marioGetPtr()->motStruct)->dispY = vz;
  len = distABf(0.0f, 0.0f, vx, vz);
  if (spd)
  {
    if (len >= shipData[4])
    {
      len = shipData[4];
    }
  }
  else
    if (len < shipData[5])
  {
    len = 0.0f;
    ((ShipDir *) marioGetPtr()->motStruct)->dispX = 0.0f;
    ((ShipDir *) marioGetPtr()->motStruct)->dispY = 0.0f;
  }
  *((f32 *) (((u8 *) m) + 0x180)) = len;
  if (len)
  {
    *((f32 *) (((u8 *) m) + 0x1A0)) = angleABf(0.0f, 0.0f, ((ShipDir *) marioGetPtr()->motStruct)->dispX, ((ShipDir *) marioGetPtr()->motStruct)->dispY);
  }
  if (len >= 1.0f)
  {
    MarioWork *mm = marioGetPtr();
    if ((marioGetPtr()->motStruct != ((void *) 0)) && (((u32) (*((s32 *) (((u8 *) marioGetPtr()->motStruct) + 0x38)))) == 0xFFFFFFFFU))
    {
      snd = psndSFXOn_3D((const char *) 0x190, (Vec *) (((u8 *) mm) + 0x8C));
      *((s32 *) (((u8 *) marioGetPtr()->motStruct) + 0x38)) = snd;
    }
  }
  else
    if (len <= 0.2f)
  {
    marioGetPtr();
    if ((marioGetPtr()->motStruct != ((void *) 0)) && (((u32) (*((s32 *) (((u8 *) marioGetPtr()->motStruct) + 0x38)))) != 0xFFFFFFFFU))
    {
      psndSFXOff(*((s32 *) (((u8 *) marioGetPtr()->motStruct) + 0x38)));
      snd = -1;
      *((s32 *) (((u8 *) marioGetPtr()->motStruct) + 0x38)) = snd;
    }
  }
  if (((u32) (*((s32 *) (((u8 *) marioGetPtr()->motStruct) + 0x38)))) != 0xFFFFFFFFU)
  {
    psndSFX_pos(*((s32 *) (((u8 *) marioGetPtr()->motStruct) + 0x38)), (Vec *) (((u8 *) m) + 0x8C));
  }
  *((f32 *) (((u8 *) m) + 0x18C)) = len;
}
void shipStrippedPool(f32* p) { p[0] = 15.0f; p[1] = 50.0f; p[2] = 30.30303f; p[3] = 180.0f; p[4] = 3.1415927f; }


BOOL checkHarbor(Vec* out) {
	f32 ix, iy, iz;
	f32 tx, ty, tz, sn, cs, hx, hy, hz, dist, nx, ny, nz;
	Vec p1, p2, p3;
	HitEntry* hit;
	MarioWork* m = marioGetPtr();
	HARBOR_PROBE(m->unk1A8, p1)
	HARBOR_PROBE(90.0f + m->unk1A8, p2)
	HARBOR_PROBE(m->unk1A8 - 90.0f, p3)
	return FALSE;
}