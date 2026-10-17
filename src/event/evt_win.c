#include "evt/evt_cmd.h"
#include "driver/icondrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/hitdrv.h"
#include "driver/mapdrv.h"
#include "driver/msgdrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "manager/winmgr.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "data/item_data.h"
#include "driver/windowdrv.h"
#include "evt/evt_badgeshop.h"
#include "evt/evt_johoya.h"
#include "evt/evt_lottery.h"
#include "pmario_sound.h"
#include "memory.h"
#include "party.h"
#include "system.h"
#include <dolphin/mtx.h>
#include <stdio.h>
#include <string.h>

extern ItemData itemDataTable[];
extern GlobalWork* gp;

void mapObjGetPos(const char* name, Vec* pos);
void mapObjFlagOn(const char* name, u32 flags);
void mapObjFlagOff(const char* name, u32 flags);
void FontDrawString(f32 x, f32 y, const char* str);
void FontDrawEdge(void);
void FontDrawEdgeOff(void);
BOOL winGhostDiaryChk(void);
BOOL N_itemPickUpFromFieldCheck(void);
void statusWinForceUpdateCoin(void);
void FontDrawScale(f32 scale);
s16 johoya_get(u8* table, s16 id);
extern BadgeShopWork* bdsw;
extern s32 badge_bottakuru100_table[];
extern InformantWork _jdt;
s32 marioKeyOn(void);

typedef struct UnitWinEntry {
	const char* mapObj; // 0x0
	const char* hitName; // 0x4
	void* evt;          // 0x8
} UnitWinEntry;

typedef struct UnitWinWork {
	u16 flags;             // 0x0
	u16 pad2;              // 0x2
	UnitWinEntry* entries; // 0x4
	s32* goods;            // 0x8
	u16 itemFlags[6];      // 0xC
	s32 goodsMax;          // 0x18
	EventEntry* evt;       // 0x1C
	EventEntry* childEvt;  // 0x20
	s32 goodsIdx;          // 0x24
	s32 timer;             // 0x28
	s32 coin;              // 0x2C
	s32 coinDisp;          // 0x30
	f32 cursor;            // 0x34
	u8 pad38[4];           // 0x38
	f32 cursorX;           // 0x3C
	f32 cursorY;           // 0x40
	f32 scroll;            // 0x44
	u8 pad48[4];           // 0x48
	s32 field_0x4C;        // 0x4C
	s32 field_0x50;        // 0x50
	u16 flags2;            // 0x54
	u8 pad56[2];           // 0x56
	s32 count;             // 0x58
	s32 field_0x5C;        // 0x5C
	s32 winId[3];          // 0x60
	u8 pad6C[0xE0 - 0x6C]; // 0x6C
	s32 selected;          // 0xE0
	s32 bought;            // 0xE4
} UnitWinWork;

#pragma force_active on
__declspec(section ".sdata") u32 gap_08_80418A0C_sbss[1];
#pragma force_active reset
static s16* mario_status_point_table[3];
s32 unk_804189fc;
static UnitWinWork* _wp;

USER_FUNC(evt_unitwin_main_func);
USER_FUNC(unk_80174e98);
void evt_unitwin_disp_func(CameraId cameraId, void* param);

void unk_8017597c(WinMgrEntry* entry);
void unk_801758fc(WinMgrEntry* entry);
void unk_80175710(WinMgrEntry* entry);
void unk_8017599c(WinMgrEntry* entry);

WinMgrDesc unk_80375478[3] = {
	{0, 0, 8, -150, 120, 300, 50, {0x2B, 0xA4, 0xFF, 0xFF}, NULL, unk_8017599c},
	{0, 0, 8, -240, -130, 500, 80, {0xF0, 0xF0, 0xF0, 0xFF}, unk_8017597c, unk_801758fc},
	{2, 0, 8, -250, -130, 150, 45, {0xF0, 0xF0, 0xF0, 0xFF}, NULL, unk_80175710},
};

static s32 party_id_table[8] = {1, 2, 5, 4, 6, 3, 7, 0};
static s32 party_icon_table[7] = {0x155, 0x156, 0x15F, 0x158, 0x160, 0x157, 0x161};
static const char* party_labelname_table[7] = {"_kur", "_nok", "_win", "_yos", "_viv", "_bom", "_chu"};
static s32 mario_status_icon_table[3] = {0x19F, 0x1A0, 0x1A1};
static const char* mario_status_labelname_table[3] = {"_hp", "_fp", "_bp"};
static const char* mario_status_name_table[3] = {"\x82\x67\x82\x6F", "\x82\x65\x82\x6F", "\x82\x61\x82\x6F"};
static s32 mario_status_henka_table[3] = {5, 5, 3};

static EVT_BEGIN(main_evt)
	CALL(evt_unitwin_main_func)
	RETURN
EVT_END()

EVT_BEGIN(select_evt)
	CALL(unk_80174e98)
	RETURN
EVT_END()

void unk_8017599c(WinMgrEntry* entry) {
	UnitWinWork* wp = _wp;
	const char* msg = msgSearch(itemDataTable[wp->goods[wp->goodsIdx]].name);
	s32 i;
	if (!winMgrAction(wp->winId[0])) {
		FontDrawStart();
		FontDrawEdge();
		FontDrawColor((GXColor){0xFF, 0xFF, 0xFF, 0xFF});
		FontDrawString(-FontGetMessageWidth(msg) / 2, 108.0f, msg);
		FontDrawEdgeOff();
	}
	for (i = 0; i < wp->goodsMax; i++) {
		if (!(wp->itemFlags[i] & 1)) {
			Mtx mtx;
			char buf[64];
			Vec pos;
			mapObjGetPos(wp->entries[i].mapObj, &pos);
			pos.y += 30.0f;
			PSMTXMultVec(camGetPtr(4)->view, &pos, &pos);
			PSMTX44MultVec(camGetPtr(4)->projection, &pos, &pos);
			pos.x *= gp->fbWidth / 2.0f;
			pos.y *= gp->efbHeight / 2.0f;
			sprintf(buf, "%d", itemDataTable[wp->goods[i]].discount_price);
			PSMTXTrans(mtx, 12.0f * (strlen(buf) - 1) + pos.x, pos.y, pos.z);
			iconNumberDispGx(mtx, itemDataTable[wp->goods[i]].discount_price, 0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
		}
	}
}


#define UNITWIN_UNUSED_INLINE(fn) static void fn(char* buf, const char* name, s32 n) { sprintf(buf, "%s_%04d", name, n); }
UNITWIN_UNUSED_INLINE(unitwin_fmt)

void unk_8017597c(WinMgrEntry* entry) {
	winMgrHelpMain(entry);
}

void unk_801758fc(WinMgrEntry* entry) {
	UnitWinWork* wp = _wp;
	s32 idx = wp->goodsIdx * 4;
	s32* goods = wp->goods;
	if (!winMgrAction(wp->winId[1])) {
		entry->msg = msgSearch(itemDataTable[*(s32*)((u8*)goods + idx)].description);
		winMgrHelpDraw(entry);
	}
}

void unk_80175710(WinMgrEntry* entry) {
	UnitWinWork* wp = _wp;
	PouchData* pouch = pouchGetPtr();
	s32 mode;
	Mtx trans;
	Mtx scale;
	s32 d;
	if (!(entry->flags & 4)) {
		PSMTXTrans(trans, 10.0f + (30.0f + entry->x), entry->y - 36.0f, 0.0f);
		if ((u32)gp->language == 0) {
			PSMTXScale(scale, 1.1f, 1.1f, 1.1f);
			PSMTXConcat(trans, scale, trans);
			iconDispGx2(trans, 0x10, 0x1F1);
		} else {
			PSMTXScale(scale, 0.8f, 1.1f, 1.1f);
			PSMTXConcat(trans, scale, trans);
			iconDispGx2(trans, 0x10, 0x20A);
		}
		PSMTXTrans(trans, 124.0f + entry->x, entry->y - 38.0f, 0.0f);
		{
			GXColor c = {0xFF, 0xFF, 0xFF, 0xFF};
			iconNumberDispGx(trans, wp->coinDisp, 0, c);
		}
		d = *(s16*)&pouch->mShopPoints - wp->coinDisp;
		if (d) {
			d /= 10;
			if (!d) {
				d = *(s16*)&pouch->mShopPoints > wp->coinDisp ? 1 : -1;
			}
			wp->coinDisp += d;
		}
	}
}

USER_FUNC(evt_unitwin_alloc) {
	s32 arg = evtGetValue(event, event->args[0]);
	UnitWinWork* wp = _mapAlloc(sizeof(UnitWinWork));
	memset(wp, 0, sizeof(UnitWinWork));
	wp->field_0x50 = 0;
	_wp = wp;
	if (arg) {
		wp->flags |= 0x80;
	} else {
		wp->flags &= ~0x80;
	}
	iconEntry2D("yubicursor_2", 0x1F0);
	iconFlagOn("yubicursor_2", 2);
	iconFlagOn("yubicursor_2", 4);
	if (!(wp->flags & 0x80)) {
		wp->evt = evtEntry(main_evt, 0, 0);
		wp->evt->lwData[0] = (s32)wp;
	}
	mario_status_point_table[0] = &pouchGetPtr()->baseMaxHP;
	mario_status_point_table[1] = &pouchGetPtr()->baseMaxFP;
	mario_status_point_table[2] = &pouchGetPtr()->maximumBP;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_unitwin_free) {
	UnitWinWork* wp = _wp;
	s32 i;
	char name[32];
	if (wp == NULL) {
		return EVT_RETURN_DONE;
	}
	if (isFirstCall) {
		wp->flags2 |= 1;
	}
	if (!(wp->flags2 & 2)) {
		return EVT_RETURN_BLOCK;
	}
	for (i = 0; i < wp->goodsMax; i++) {
		mapObjFlagOff(wp->entries[i].mapObj, 0x4000);
		sprintf(name, "uwn%d", i);
		iconDelete(name);
	}
	if (wp->flags & 0x80) {
		for (i = 0; i < 3; i++) {
			winMgrDelete(wp->winId[i]);
		}
	}
	iconDelete("yubicursor_2");
	_mapFree(_wp);
	_wp = NULL;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_unitwin_selltable_setup) {
	s32 i;
	s32* args = event->args;
	UnitWinEntry* tbl = (UnitWinEntry*)evtGetValue(event, args[0]);
	s32* goods = (s32*)evtGetValue(event, args[1]);
	UnitWinWork* wp = _wp;
	char name[32];
	Vec pos;
	wp->entries = tbl;
	wp->goods = goods;
	wp->goodsMax = 0;
	for (i = 0; wp->entries[i].mapObj != NULL; i++) {
		wp->goodsMax++;
		mapObjGetPos(wp->entries[i].mapObj, &pos);
		mapObjFlagOn(wp->entries[i].mapObj, 0x4000);
		if (wp->goods[i] != 0) {
			sprintf(name, "uwn%d", i);
			((void (*)(const char*, u16))iconEntry)(name, itemDataTable[wp->goods[i]].icon_id);
			iconSetPos(name, pos.x, pos.y + 6.0f, pos.z);
			iconFlagOn(name, 0x40);
		} else {
			wp->itemFlags[i] |= 1;
		}
	}
	wp->evt = evtEntryType(select_evt, 0, 0x20, 0);
	wp->evt->lwData[0] = (s32)wp;
	for (i = 0; i < 3; i++) {
		wp->winId[i] = winMgrEntry(&unk_80375478[i]);
	}
	wp->selected = -1;
	return EVT_RETURN_DONE;
}

USER_FUNC(unitwincoin_disp_onoff) {
	s32 on = evtGetValue(event, event->args[0]);
	UnitWinWork* wp;
	pouchGetPtr();
	wp = _wp;
	wp->coin = pouchGetCoin();
	wp->field_0x5C = 0;
	if (on) {
		wp->flags |= 0x1000;
	} else {
		wp->flags &= ~0x1000;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(unitwincoin_wait) {
	UnitWinWork* wp = _wp;
	pouchGetPtr();
	if (wp->coin != pouchGetCoin()) {
		return EVT_RETURN_BLOCK;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(unk_8017524c) {
	s32 on = evtGetValue(event, event->args[0]);
	UnitWinWork* wp = _wp;
	if (on) {
		wp->flags |= 0x100;
	} else {
		wp->flags &= ~0x100;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(unk_801751f8) {
	s32* args = event->args;
	UnitWinWork* wp = (UnitWinWork*)evtGetValue(event, args[0]);
	wp->bought = evtGetValue(event, args[1]);
	return EVT_RETURN_DONE;
}

#define UNITWIN_CLOSE(wp)                \
	if ((wp)->selected != -1) {          \
		(wp)->selected = -1;             \
		winMgrClose((wp)->winId[0]);     \
		winMgrClose((wp)->winId[1]);     \
	}

USER_FUNC(unk_80174e98) {
	UnitWinWork* wp = (UnitWinWork*)event->lwData[0];
	s32 found = FALSE;
	MarioWork* mario = marioGetPtr();
	PartyEntry* party = partyGetPtr(0);
	HitEntry* hit = mario->hitobjStandOn;
	u32 btn;
	s32 i;
	char name[32];
	MapObject* obj;
	if (wp->flags2 & 1) {
		wp->flags2 |= 2;
		return EVT_RETURN_DONE;
	}
	wp->flags &= ~1;
	btn = keyGetButtonTrg(0);
	if ((marioChkCtrl() ? FALSE : TRUE) || (marioChkKey() ? FALSE : TRUE)) {
		btn = 0;
	}
	if (party != NULL && (party->flags & 0x100)) {
		UNITWIN_CLOSE(wp);
		return EVT_RETURN_BLOCK;
	}
	if (winGhostDiaryChk() || N_itemPickUpFromFieldCheck()) {
		UNITWIN_CLOSE(wp);
		return EVT_RETURN_BLOCK;
	}
	if (!(wp->flags & 2)) {
		if (hit != NULL) {
			for (i = 0; i < wp->goodsMax; i++) {
				const char* hitName = hitGetName(hit);
				if (!strcmp(wp->entries[i].hitName, hitName)) {
					break;
				}
			}
			if (!(wp->itemFlags[i] & 1) && i < wp->goodsMax) {
				if (wp->selected != i) {
					if (wp->selected == -1) {
						winMgrOpen(wp->winId[0]);
						winMgrOpen(wp->winId[1]);
					}
					wp->selected = i;
					wp->goodsIdx = i;
					winMgrHelpInit(winMgrGetPtr(wp->winId[1]));
				}
				found = TRUE;
			}
		}
		if (!found) {
			UNITWIN_CLOSE(wp);
		}
		if (wp->selected != -1) {
			wp->flags |= 1;
			if (btn & 0x100) {
				marioKeyOff();
				wp->childEvt = evtEntry(wp->entries[wp->goodsIdx].evt, 0, 0);
				wp->childEvt->lwData[0] = (s32)wp;
				wp->flags |= 2;
				winMgrClose(wp->winId[0]);
				winMgrClose(wp->winId[1]);
				wp->selected = -1;
				wp->bought = 0;
			}
		}
	} else if (!evtCheckID(wp->childEvt->eventId)) {
		wp->flags &= ~2;
		marioKeyOn();
		if (wp->bought) {
			wp->itemFlags[wp->goodsIdx] |= 1;
		}
	}
	for (i = 0; i < wp->goodsMax; i++) {
		obj = mapGetMapObj(wp->entries[i].mapObj);
		if (obj != NULL) {
			sprintf(name, "uwn%d", i);
			if (wp->itemFlags[i] & 1) {
				iconFlagOn(name, 2);
			} else if (obj->flags & 1) {
				iconFlagOn(name, 2);
			} else {
				iconFlagOff(name, 2);
			}
		}
	}
	return EVT_RETURN_BLOCK;
}

USER_FUNC(evt_unitwin_main_func) {
	UnitWinWork* wp = (UnitWinWork*)event->lwData[0];
	if (wp->flags2 & 1) {
		wp->flags2 |= 2;
		return EVT_RETURN_DONE;
	}
	if (wp->timer > 0) {
		wp->timer--;
	} else {
		wp->flags &= ~1;
	}
	dispEntry((CameraId)8, 1000.0f, 0, evt_unitwin_disp_func, wp);
	return EVT_RETURN_BLOCK;
}

static inline s32 unitwin_party_icon(s32 icon) {
	if (icon == 0x158) {
		switch (pouchGetPartyColor(4)) {
			case 0:
				icon = 0x158;
				break;
			case 1:
				icon = 0x159;
				break;
			case 2:
				icon = 0x15A;
				break;
			case 3:
				icon = 0x15B;
				break;
			case 4:
				icon = 0x15C;
				break;
			case 5:
				icon = 0x15D;
				break;
			case 6:
			default:
				icon = 0x15E;
				break;
		}
	}
	return icon;
}

#define ICONGX ((void (*)(Vec*, u16, u16, f32))iconDispGx)
#define ICONGX2 ((void (*)(Vec*, u16, f32, u16))iconDispGx)

void evt_unitwin_disp_func(CameraId cameraId, void* param) {
	PouchData* pouch = pouchGetPtr();
	UnitWinWork* wp = (UnitWinWork*)param;
	s32 mode;
	s32 row;
	s32 i;
	char fbuf[64];

	if (0) { sprintf(fbuf, "%s_%04d", "", 0); }
	if (wp->flags & 1) {
		const char* desc = itemDataTable[wp->goods[wp->goodsIdx]].description;
		const char* name = itemDataTable[wp->goods[wp->goodsIdx]].id;

		windowDispGX_Waku_col(0, (GXColor){0x2B, 0xA4, 0xFF, 0xFF}, -150.0f, 120.0f, 300.0f, 50.0f, 20.0f);
		FontDrawStart();
		FontDrawEdge();
		FontDrawColor((GXColor){0xFF, 0xFF, 0xFF, 0xFF});
		FontDrawString((s32)(strlen(name) * 24 / 4) * -1, 108.0f, name);
		FontDrawEdgeOff();
		windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -240.0f, -130.0f, 480.0f, 70.0f, 20.0f);
		FontDrawStart();
		FontDrawMessage(-220, -138, msgSearch(desc));

		for (i = 0; i < wp->goodsMax; i++) {
			if (wp->goods[i] != 0) {
				Vec pos;
				f32 proj[7];
				f32 vp[6];
				f32 sx, sy, sz;
				Mtx mtx;
				CameraEntry* cam;
				s32 id;
				u8 price;

				mapObjGetPos(wp->entries[i].mapObj, &pos);
				cam = camGetPtr(4);
				GXSetProjection(cam->projection, cam->projType);
				GXGetProjectionv(proj);
				GXGetViewportv(vp);
				GXProject(pos.x, pos.y, pos.z, cam->view, proj, vp, &sx, &sy, &sz);
				cam = camGetPtr(cameraId);
				GXSetProjection(cam->projection, cam->projType);
				id = wp->goods[i];
				if (id >= 0xF0 && id < 0x153) {
					price = itemDataTable[id].discount_price;
				} else {
					price = itemDataTable[id].buy_price;
				}
				PSMTXTrans(mtx, 8.0f + (-304.0f + sx), 240.0f - (sy - 60.0f), 0.0f);
				iconNumberDispGx(mtx, price, 1, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
			}
		}
	}

	if (wp->flags & 0xD000) {
		Mtx mtx;
		Mtx scale;
		s32 coin;
		s32 delta;

		windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -250.0f, -75.0f, 150.0f, 45.0f, 20.0f);
		PSMTXTrans(mtx, -220.0f, -113.0f, 0.0f);
		PSMTXScale(scale, 0.7f, 0.7f, 0.7f);
		PSMTXConcat(mtx, scale, mtx);
		if (wp->flags & 0x1000) {
			iconDispGx2(mtx, 0x10, 0x18B);
			coin = pouchGetCoin();
		} else if (wp->flags & 0x4000) {
			iconDispGx2(mtx, 0x10, 0x102);
			coin = ((s32 (*)(void))pouchGetSuperCoin)();
		} else {
			iconDispGx2(mtx, 0x10, 0x18D);
			coin = ((s32 (*)(void))pouchGetStarPiece)();
		}
		PSMTXTrans(mtx, -192.0f, -113.0f, 0.0f);
		iconDispGx2(mtx, 0x10, 0x1D6);
		PSMTXTrans(mtx, -126.0f, -113.0f, 0.0f);
		iconNumberDispGx(mtx, wp->coin, 0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
		if ((delta = coin - wp->coin)) {
			delta /= 10;
			if (!delta) {
				delta = coin > wp->coin ? 1 : -1;
			}
			if (wp->field_0x5C & 1) {
				wp->coin += delta;
				statusWinForceUpdateCoin();
			}
			wp->field_0x5C++;
			wp->field_0x5C %= 1200;
			if (!unk_804189fc) {
				psndSFXOn("SFX_COUNT_COIN1");
				unk_804189fc = 7;
			}
		}
		if (unk_804189fc > 0) {
			unk_804189fc--;
		}
	}

	if (wp->flags & 0x2000) {
		Mtx mtx;
		Mtx scale;
		s32 delta;

		windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -250.0f, -130.0f, 150.0f, 45.0f, 20.0f);
		PSMTXTrans(mtx, -210.0f, -166.0f, 0.0f);
		PSMTXScale(scale, 1.1f, 1.1f, 1.1f);
		PSMTXConcat(mtx, scale, mtx);
		iconDispGx2(mtx, 0x10, 0x1F1);
		PSMTXTrans(mtx, -126.0f, -168.0f, 0.0f);
		iconNumberDispGx(mtx, wp->coinDisp, 0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
		if (!(gp->retraceCount & 3)) {
			if ((delta = pouch->mShineSpriteCount - wp->coinDisp)) {
				delta /= 10;
				if (!delta) {
					delta = pouch->mShineSpriteCount > wp->coinDisp ? 1 : -1;
				}
				wp->coinDisp += delta;
			}
		}
	}

	if ((wp->flags & 0x100) && (lotteryGetPtr()->mFlags & 2)) {
		Mtx mtx;
		char buf[32];

		windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -250.0f, -130.0f, 150.0f, 45.0f, 20.0f);
		PSMTXTrans(mtx, -220.0f, -172.0f, 0.0f);
		iconDispGx2(mtx, 0x10, 0x140);
		sprintf(buf, "%04d", *(s16*)((u8*)lotteryGetPtr() + 0x20));
		for (i = 0; i < 4; i++) {
			PSMTXTrans(mtx, 20.0f * i + -126.0f - 60.0f, -168.0f, 0.0f);
			iconNumberDispGx(mtx, ((s8*)buf)[i] - '0', 0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
		}
	}

	switch (mode = wp->field_0x50) {
		case 0:
		case 1:
			break;
		case 4: {
			const char* desc;
			s32 h;
			u32 sl, st, sw, sh;

			row = 0;
			desc = NULL;

			h = (8 - pouchGetKeepItemCnt() > 0 ? 8 - pouchGetKeepItemCnt() : 0) * 24;
			windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -24.0f, 100.0f, 300.0f, 220 - h, 20.0f);
			windowDispGX_Waku_col(0, (GXColor){0x2B, 0xA4, 0xFF, 0xFF}, 51.0f, 114.0f, 150.0f, 28.0f, 8.0f);
			FontDrawStart();
			FontDrawEdge();
			FontDrawColor((GXColor){0xFF, 0xFF, 0xFF, 0xFF});
			FontDrawString(81.0f, 111.0f, msgSearch("msg_window_title_1"));
			FontDrawEdgeOff();
			GXGetScissor(&sl, &st, &sw, &sh);
			GXSetScissor(0x120, 0xA2, 0x11C, 0xBE - h);
			for (i = 0; i < 32; i++) {
				s32 id = pouchKeepItem(i);
				ItemData* item = &itemDataTable[id];
				if (id) {
					ICONGX2(&(Vec){11.0f, wp->scroll + (56 - row * 24), 0.0f}, 0x10, 0.5f, item->icon_id);
					FontDrawStart();
					FontDrawString(36.0f, wp->scroll + (80 - row * 24), item->id);
					if (row == wp->cursor) {
						desc = item->description;
					}
					row++;
				}
			}
			GXSetScissor(sl, st, sw, sh);
			if (desc) {
				windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -240.0f, -130.0f, 480.0f, 70.0f, 20.0f);
				FontDrawStart();
				FontDrawMessage(-220, -138, msgSearch(desc));
			}
			break;
		}
		case 2:
		case 3:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16: {
			s32 h;
			s32* tbl;
			const char* title;
			s32 titleX;
			s32 i;
			const char* desc;
			s32 max;
			s32 flag;
			u32 sl, st, sw, sh;

			desc = NULL;
			if (mode == 2 || mode == 3) {
				h = (8 - pouchGetHaveItemCnt() > 0 ? 8 - pouchGetHaveItemCnt() : 0) * 24;
				max = 20;
				title = msgSearch("msg_window_title_1");
				titleX = 30;
			} else if (mode == 12) {
				h = (8 - pouchGetHaveBadgeCnt() > 0 ? 8 - pouchGetHaveBadgeCnt() : 0) * 24;
				max = 200;
				title = msgSearch("msg_window_title_2");
				titleX = 40;
			} else if (mode == 14) {
				h = 8 - wp->count;
			h = (h > 0 ? h : 0) * 24;
				max = 0x152;
				title = msgSearch("msg_window_title_3");
				titleX = 20;
			} else if (mode == 15) {
				h = 8 - wp->count;
			h = (h > 0 ? h : 0) * 24;
				max = wp->count;
				title = msgSearch("msg_window_title_3");
				titleX = 20;
			} else if (mode == 16) {
				h = 8 - wp->count;
			h = (h > 0 ? h : 0) * 24;
				max = 0x152;
				title = msgSearch("msg_window_title_4");
				titleX = 30;
			} else {
				h = 8 - wp->count;
			h = (h > 0 ? h : 0) * 24;
				max = 99;
				title = msgSearch("msg_window_title_2");
				titleX = 40;
			}
			windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -24.0f, 100.0f, 300.0f, 220 - h, 20.0f);
			windowDispGX_Waku_col(0, (GXColor){0x2B, 0xA4, 0xFF, 0xFF}, 51.0f, 114.0f, 150.0f, 28.0f, 8.0f);
			FontDrawStart();
			FontDrawEdge();
			FontDrawColor((GXColor){0xFF, 0xFF, 0xFF, 0xFF});
			FontDrawString(titleX + 51, 111.0f, title);
			FontDrawEdgeOff();
			if (wp->field_0x50 != 3) {
				Mtx mtx;
				Mtx scale;
				s32 icon;
				PSMTXTrans(mtx, 244.0f, 82.0f, 0.0f);
				PSMTXScale(scale, 0.6f, 0.6f, 0.6f);
				PSMTXConcat(mtx, scale, mtx);
				iconDispGxCol(mtx, 0x10, 0x211, (GXColor){0xF0, 0xF0, 0xF0, 0xFF});
				if (wp->field_0x50 == 13) {
					icon = 0x18D;
				} else if (wp->field_0x50 == 16) {
					icon = 0x13F;
				} else {
					icon = 0x18B;
				}
				{
					iconDispGx(&(Vec){244.0f, 86.0f, 0.0f}, 0x10, icon, 0.6f);
				}
			}
			GXGetScissor(&sl, &st, &sw, &sh);
			GXSetScissor(0x120, 0xA2, 0x11C, 0xBE - h);
			for (tbl = badge_bottakuru100_table, i = 0, row = i; i < max; i++) {
				
				GXColor color = {0x00, 0x00, 0x00, 0xFF};
				s32 id;
				ItemData* item;
				flag = 0;
				switch (wp->field_0x50) {
					case 2:
					case 3:
						id = pouchHaveItem(i);
						break;
					case 12:
						id = pouchHaveBadge(i);
						break;
					case 11:
						id = (s16)badgeShop_get(bdsw->bargain_table, i + 0xF0);
						if (id - badgeShop_ThrowCheck(i + 0xF0) > 0) {
							id = i + 0xF0;
						} else {
							id = 0;
						}
						break;
					case 13:
						if ((s16)badgeShop_get(bdsw->starmaniac_table, i + 0xF0) > 0) {
							id = i + 0xF0;
						} else {
							id = 0;
						}
						break;
					case 14:
						if ((s16)badgeShop_get(bdsw->bottakuru_table, i + 1) > 0) {
							id = i + 1;
						} else {
							id = 0;
						}
						break;
					case 15:
						id = tbl[i];
						break;
					case 16:
						if ((s16)badgeShop_get(bdsw->bteresa_table, i + 1) > 0) {
							id = i + 1;
						} else {
							id = 0;
						}
						break;
				}
				item = &itemDataTable[id];
				if (id) {
					s32 ty;
					if (wp->field_0x50 == 13 && item->star_piece_price > ((s32 (*)(void))pouchGetStarPiece)()) {
						color = (GXColor){0xA0, 0xA0, 0xA0, 0xFF};
						flag = 0x20;
					}
					{
						ICONGX2(&(Vec){11.0f, wp->scroll + (56 - row * 24), 0.0f}, flag | 0x10, 0.5f, item->icon_id);
					}
					FontDrawStart();
					FontDrawScale(0.8f);
					FontDrawColor(color);
					ty = 80 - row * 24;
					FontDrawString(36.0f, wp->scroll + ty - 2.0f, item->id);
					if (wp->field_0x50 != 3) {
						char buf[32];
						f32 sc;
						s32 xoff;
						switch (wp->field_0x50) {
							case 14:
								sprintf(buf, "%2d", item->buy_price * 12 / 10);
								sc = 1.0f;
								xoff = 0;
								break;
							case 15:
								sprintf(buf, "%3d", item->buy_price * (evtGetValue(NULL, 0xF5DE06A9) + 11) / 10);
								sc = 1.0f;
								xoff = -14;
								break;
							case 16:
								sprintf(buf, "%4d", (item->buy_price * 2 + 2) / 3);
								sc = 0.8f;
								xoff = -20;
								break;
							case 11:
								sprintf(buf, "%2d", item->buy_price);
								sc = 1.0f;
								xoff = 0;
								break;
							case 13:
								sprintf(buf, "%2d", item->star_piece_price);
								sc = 1.0f;
								xoff = 0;
								break;
							case 12:
							default:
								sprintf(buf, "%2d", item->sell_price);
								xoff = 0;
								sc = 1.0f;
								break;
						}
						FontDrawStart();
						FontDrawScale(sc);
						FontDrawColor(color);
						FontDrawString(xoff + 226, wp->scroll + ty, buf);
					}
					if (row == wp->cursor) {
						desc = item->description;
					}
					row++;
				}
			}
			GXSetScissor(sl, st, sw, sh);
			if (desc) {
				windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -240.0f, -130.0f, 480.0f, 70.0f, 20.0f);
				FontDrawStart();
				FontDrawMessage(-220, -138, msgSearch(desc));
			}
			break;
		}
		case 17:
		case 18: {
			InformantWork* jdt = &_jdt;
			s32 h;
			u8* tbl;
			const char* title;
			u32 sl, st, sw, sh;
			InformantInfo* info;


			h = 8 - wp->count;
			h = (h > 0 ? h : 0) * 24;
			if (mode == 17) {
				title = msgSearch("msg_window_title_5");
				tbl = jdt->unk14.unkD;
			} else {
				title = msgSearch("msg_window_title_6");
				tbl = jdt->unk14.unk1A;
			}
			windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -24.0f, 100.0f, 300.0f, 220 - h, 20.0f);
			windowDispGX_Waku_col(0, (GXColor){0x2B, 0xA4, 0xFF, 0xFF}, 51.0f, 114.0f, 150.0f, 28.0f, 8.0f);
			FontDrawStart();
			FontDrawEdge();
			FontDrawColor((GXColor){0xFF, 0xFF, 0xFF, 0xFF});
			FontDrawString(71.0f, 111.0f, title);
			FontDrawEdgeOff();
			if (wp->field_0x50 == 17) {
				Mtx mtx;
				Mtx scale;
				PSMTXTrans(mtx, 244.0f, 82.0f, 0.0f);
				PSMTXScale(scale, 0.6f, 0.6f, 0.6f);
				PSMTXConcat(mtx, scale, mtx);
				iconDispGxCol(mtx, 0x10, 0x211, (GXColor){0xF0, 0xF0, 0xF0, 0xFF});
				{
					iconDispGx(&(Vec){244.0f, 86.0f, 0.0f}, 0x10, 0x18B, 0.6f);
				}
			}
			GXGetScissor(&sl, &st, &sw, &sh);
			GXSetScissor(0x120, 0xA2, 0x11C, 0xBE - h);
			for (i = 0, info = jdt->info; i < wp->count; i++, info++) {
				char buf[32];
				GXColor color;
				char c;
				s32 ty;
				if (johoya_get(tbl, info->storyId)) {
					sprintf(buf, "");
					color = (GXColor){0xA0, 0xA0, 0xA0, 0xFF};
				} else {
					sprintf(buf, "%3d", info->unkA);
					color = (GXColor){0x00, 0x00, 0x00, 0xFF};
				}
				FontDrawStart();
				c = ((char*)info->initEndPos)[info->tagEndPos];
				((char*)info->initEndPos)[info->tagEndPos] = 0;
				FontDrawColor(color);
				FontDrawScale(0.8f);
				ty = 80 - i * 24;
				FontDrawString(1.0f, wp->scroll + ty, (char*)info->initEndPos);
				((char*)info->initEndPos)[info->tagEndPos] = c;
				if (wp->field_0x50 == 17) {
					FontDrawStart();
					FontDrawColor(color);
					FontDrawString(221.0f, wp->scroll + ty, buf);
				}
			}
			GXSetScissor(sl, st, sw, sh);
			break;
		}
		case 5:
		case 6:
		case 7: {
			char msgName[64] = {0};
			s32 row = 0;
			s32 h;
			u32 sl, st, sw, sh;
			
			s32* pid;
			s32 lv;
			s32 flag;
			GXColor color;
			const char* name;

			h = 8 - wp->count;
			h = (h > 0 ? h : 0) * 24;
			windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -24.0f, 100.0f, 300.0f, 220 - h, 20.0f);
			windowDispGX_Waku_col(0, (GXColor){0x2B, 0xA4, 0xFF, 0xFF}, 16.0f, 114.0f, 120.0f, 28.0f, 8.0f);
			FontDrawStart();
			FontDrawEdge();
			FontDrawColor((GXColor){0xFF, 0xFF, 0xFF, 0xFF});
			FontDrawString(41.0f, 111.0f, msgSearch("msg_window_title_7"));
			FontDrawEdgeOff();
			windowDispGX_Waku_col(0, (GXColor){0x2B, 0xA4, 0xFF, 0xFF}, 167.0f, 114.0f, 85.0f, 28.0f, 8.0f);
			FontDrawStart();
			FontDrawEdge();
			FontDrawColor((GXColor){0xFF, 0xFF, 0xFF, 0xFF});
			FontDrawString(177.0f, 111.0f, msgSearch("msg_window_title_8"));
			FontDrawEdgeOff();
			GXGetScissor(&sl, &st, &sw, &sh);
			GXSetScissor(0x120, 0xA2, 0x11C, 0xBE - h);
			for (i = 0, pid = party_id_table; *pid; i++, pid++) {
				s32 icon;
				s32 ty;
				char buf[32];
				char help[16];
				char dir[16];
				if (wp->field_0x50 == 5 || wp->field_0x50 == 6) {
					if (!partyChkJoin(*pid)) {
						continue;
					}
					if (evtGetValue(NULL, 0xF8406022) == 0) {
						if (pouch->partyData[*pid].techLevel < 1) {
							color = (GXColor){0x00, 0x00, 0x00, 0xFF};
							lv = 0;
							flag = 0;
						} else {
							color = (GXColor){0xA0, 0xA0, 0xA0, 0xFF};
							lv = 1;
							flag = 0x20;
						}
					} else {
						if (pouch->partyData[*pid].techLevel < 2) {
							color = (GXColor){0x00, 0x00, 0x00, 0xFF};
							lv = 0;
							flag = 0;
						} else {
							color = (GXColor){0xA0, 0xA0, 0xA0, 0xFF};
							lv = 2;
							flag = 0x20;
						}
					}
				} else {
					if (!partyChkJoin(*pid)) {
						continue;
					}
					if (wp->field_0x4C == *pid) {
						continue;
					}
					if (pouch->partyData[*pid].techLevel > 0) {
						color = (GXColor){0x00, 0x00, 0x00, 0xFF};
						lv = 0;
						flag = 0;
					} else {
						color = (GXColor){0xA0, 0xA0, 0xA0, 0xFF};
						lv = 1;
						flag = 0x20;
					}
				}
				ICONGX2(&(Vec){11.0f, wp->scroll + (56 - row * 24), 0.0f}, flag | 0x10, 0.5f, unitwin_party_icon(party_icon_table[i]));
				FontDrawStart();
				FontDrawColor(color);
				switch (i) {
					case 0:
						name = msgSearch("name_party0");
						break;
					case 1:
						name = msgSearch("name_party1");
						break;
					case 2:
						name = msgSearch("name_party4");
						break;
					case 3:
						name = pouchGetYoshiName();
						break;
					case 4:
						name = msgSearch("name_party5");
						break;
					case 5:
						name = msgSearch("name_party2");
						break;
					case 6:
						name = msgSearch("name_party6");
						break;
				}
				ty = 80 - row * 24;
				FontDrawString(36.0f, wp->scroll + ty, name);
				sprintf(buf, "%d", pouch->partyData[*pid].techLevel);
				FontDrawStart();
				FontDrawString(236.0f, wp->scroll + ty, buf);
				if (row == wp->cursor) {
					if (lv == 0) {
						sprintf(help, "help");
					} else {
						sprintf(help, "helperr");
					}
					if (wp->field_0x50 == 5 || wp->field_0x50 == 6) {
						sprintf(dir, "lankup");
					} else {
						sprintf(dir, "lankdown");
					}
					sprintf(msgName, "%s_%s_lank%d%s", help, dir, pouch->partyData[*pid].techLevel + 1, party_labelname_table[i]);
				}
				row++;
			}
			GXSetScissor(sl, st, sw, sh);
			if (*(s8*)msgName) {
				windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -240.0f, -130.0f, 480.0f, 70.0f, 20.0f);
				FontDrawStart();
				FontDrawMessage(-220, -138, msgSearch(msgName));
			}
			break;
		}
		case 8: {
			char msgName[64] = {0};
			s32 row = 0;
			s32 h;
			u32 sl, st, sw, sh;
			
			s32 j;
			s32 flag;
			s32 lv;
			s32 v;
			s32 icon;
			s32 bob;
			s32 iy;
			s32 iflag;
			s32 ty;
			GXColor color;

			h = 8 - wp->count;
			h = (h > 0 ? h : 0) * 24;
			windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -24.0f, 100.0f, 300.0f, 220 - h, 20.0f);
			windowDispGX_Waku_col(0, (GXColor){0x2B, 0xA4, 0xFF, 0xFF}, 16.0f, 114.0f, 120.0f, 28.0f, 8.0f);
			FontDrawStart();
			FontDrawEdge();
			FontDrawColor((GXColor){0xFF, 0xFF, 0xFF, 0xFF});
			FontDrawString(21.0f, 111.0f, msgSearch("msg_window_title_9"));
			FontDrawEdgeOff();
			windowDispGX_Waku_col(0, (GXColor){0x2B, 0xA4, 0xFF, 0xFF}, 167.0f, 114.0f, 85.0f, 28.0f, 8.0f);
			FontDrawStart();
			FontDrawEdge();
			FontDrawColor((GXColor){0xFF, 0xFF, 0xFF, 0xFF});
			FontDrawString(177.0f, 111.0f, msgSearch("msg_window_title_10"));
			FontDrawEdgeOff();
			GXGetScissor(&sl, &st, &sw, &sh);
			GXSetScissor(0x120, 0xA2, 0x11C, 0xBE - h);
			for (i = 0; i < wp->count; i++, row++) {
				char buf[32];
				char help[16];
				flag = 0;
				for (j = 0; j < wp->count; j++) {
					if (j != wp->cursor && *mario_status_point_table[j] - mario_status_henka_table[j] <= 0) {
						flag = 1;
					}
				}
				if (!flag) {
					color = (GXColor){0x00, 0x00, 0x00, 0xFF};
					lv = 0;
					iflag = 0;
				} else {
					color = (GXColor){0xA0, 0xA0, 0xA0, 0xFF};
					lv = 1;
					iflag = 0x20;
				}
				iy = 56 - i * 24;
				iflag |= 0x10;
				{
					ICONGX2(&(Vec){11.0f, wp->scroll + iy, 0.0f}, iflag, 0.5f, mario_status_icon_table[i]);
				}
				FontDrawStart();
				FontDrawColor(color);
				sprintf(buf, "%s%3d", mario_status_name_table[i], *mario_status_point_table[i]);
				ty = 80 - i * 24;
				FontDrawString(36.0f, wp->scroll + ty, buf);
				bob = (gp->retraceCount & 0xF) > 7 ? 0 : 2;
				if (row == wp->cursor) {
					if (lv == 0) {
						sprintf(help, "help");
					} else {
						sprintf(help, "helperr");
					}
					sprintf(msgName, "%s_levelup%s", help, mario_status_labelname_table[i]);
					v = *mario_status_point_table[i] + mario_status_henka_table[i] * 2;
					if (v > 0) {
						icon = 0x1B5;
					} else {
						icon = 0x162;
						bob = 0;
					}
					{
						ICONGX2(&(Vec){166.0f, bob + (wp->scroll + iy), 0.0f}, iflag, 0.5f, icon);
					}
					{
						ICONGX2(&(Vec){186.0f, bob + (wp->scroll + iy), 0.0f}, iflag, 0.5f, icon);
					}
				} else {
					v = *mario_status_point_table[i] - mario_status_henka_table[i];
					if (v > 0) {
						icon = 0x1B6;
					} else {
						icon = 0x162;
						bob = 0;
					}
					{
						ICONGX2(&(Vec){176.0f, (wp->scroll + iy) - bob, 0.0f}, iflag, 0.5f, icon);
					}
				}
				if (v > 0) {
					sprintf(buf, "%3d", v);
				} else {
					sprintf(buf, "  \x81\x48", v);
				}
				FontDrawStart();
				FontDrawString(206.0f, wp->scroll + ty, buf);
			}
			GXSetScissor(sl, st, sw, sh);
			if (*(s8*)msgName) {
				windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -240.0f, -130.0f, 480.0f, 70.0f, 20.0f);
				FontDrawStart();
				FontDrawMessage(-220, -138, msgSearch(msgName));
			}
			break;
		}
		case 9: {
			char msgName[64] = {0};
			s32 row = 0;
			s32 h;
			u32 sl, st, sw, sh;
			
			s32 j;
			s32 cnt;
			s32 lv;
			s32 v;
			s32 ty;
			GXColor color;

			h = 8 - wp->count;
			h = (h > 0 ? h : 0) * 24;
			windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -24.0f, 100.0f, 300.0f, 220 - h, 20.0f);
			windowDispGX_Waku_col(0, (GXColor){0x2B, 0xA4, 0xFF, 0xFF}, 16.0f, 114.0f, 120.0f, 28.0f, 8.0f);
			FontDrawStart();
			FontDrawEdge();
			FontDrawColor((GXColor){0xFF, 0xFF, 0xFF, 0xFF});
			FontDrawString(21.0f, 111.0f, msgSearch("msg_window_title_9"));
			FontDrawEdgeOff();
			windowDispGX_Waku_col(0, (GXColor){0x2B, 0xA4, 0xFF, 0xFF}, 167.0f, 114.0f, 85.0f, 28.0f, 8.0f);
			FontDrawStart();
			FontDrawEdge();
			FontDrawColor((GXColor){0xFF, 0xFF, 0xFF, 0xFF});
			FontDrawString(177.0f, 111.0f, msgSearch("msg_window_title_10"));
			FontDrawEdgeOff();
			GXGetScissor(&sl, &st, &sw, &sh);
			GXSetScissor(0x120, 0xA2, 0x11C, 0xBE - h);
			for (i = 0; i < wp->count; i++, row++) {
				char buf[32];
				char help[16];
				cnt = 0;
				for (j = 0; j < wp->count; j++) {
					if (j != wp->cursor && *mario_status_point_table[j] - mario_status_henka_table[j] <= 0) {
						cnt++;
					}
				}
				if (cnt < 2) {
					color = (GXColor){0x00, 0x00, 0x00, 0xFF};
					lv = 0;
					j = 0;
				} else {
					color = (GXColor){0xA0, 0xA0, 0xA0, 0xFF};
					lv = 1;
					j = 0x20;
				}
				{
					ICONGX2(&(Vec){11.0f, wp->scroll + (56 - i * 24), 0.0f}, j | 0x10, 0.5f, mario_status_icon_table[i]);
				}
				FontDrawStart();
				FontDrawColor(color);
				sprintf(buf, "%s%3d", mario_status_name_table[i], *mario_status_point_table[i]);
				ty = 80 - i * 24;
				FontDrawString(36.0f, wp->scroll + ty, buf);
				if (row == wp->cursor) {
					if (lv == 0) {
						sprintf(help, "help");
					} else {
						sprintf(help, "helperr");
					}
					sprintf(msgName, "%s_levelup%s", help, mario_status_labelname_table[i]);
					v = *mario_status_point_table[i] + mario_status_henka_table[i];
					if (v > 0) {
						sprintf(buf, "%3d", v);
					} else {
						sprintf(buf, "  \x81\x48", v);
					}
					FontDrawStart();
					FontDrawString(206.0f, wp->scroll + ty, buf);
				}
			}
			GXSetScissor(sl, st, sw, sh);
			if (*(s8*)msgName) {
				windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -240.0f, -130.0f, 480.0f, 70.0f, 20.0f);
				FontDrawStart();
				FontDrawMessage(-220, -138, msgSearch(msgName));
			}
			break;
		}
		case 10: {
			char msgName[64] = {0};
			s32 row = 0;
			s32 h;
			u32 sl, st, sw, sh;
			
			s32 lv;
			s32 v;
			s32 ty;
			s32 flag;
			GXColor color;

			h = 8 - wp->count;
			h = (h > 0 ? h : 0) * 24;
			windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -24.0f, 100.0f, 300.0f, 220 - h, 20.0f);
			windowDispGX_Waku_col(0, (GXColor){0x2B, 0xA4, 0xFF, 0xFF}, 16.0f, 114.0f, 120.0f, 28.0f, 8.0f);
			FontDrawStart();
			FontDrawEdge();
			FontDrawColor((GXColor){0xFF, 0xFF, 0xFF, 0xFF});
			FontDrawString(21.0f, 111.0f, msgSearch("msg_window_title_9"));
			FontDrawEdgeOff();
			windowDispGX_Waku_col(0, (GXColor){0x2B, 0xA4, 0xFF, 0xFF}, 167.0f, 114.0f, 85.0f, 28.0f, 8.0f);
			FontDrawStart();
			FontDrawEdge();
			FontDrawColor((GXColor){0xFF, 0xFF, 0xFF, 0xFF});
			FontDrawString(177.0f, 111.0f, msgSearch("msg_window_title_10"));
			FontDrawEdgeOff();
			GXGetScissor(&sl, &st, &sw, &sh);
			GXSetScissor(0x120, 0xA2, 0x11C, 0xBE - h);
			for (i = 0; i < 3; i++) {
				char buf[32];
				char help[16];
				if (i == wp->field_0x4C) {
					continue;
				}
				if (*mario_status_point_table[i] - mario_status_henka_table[i] > 0) {
					color = (GXColor){0x00, 0x00, 0x00, 0xFF};
					lv = 0;
					flag = 0;
				} else {
					color = (GXColor){0xA0, 0xA0, 0xA0, 0xFF};
					lv = 1;
					flag = 0x20;
				}
				{
					ICONGX2(&(Vec){11.0f, wp->scroll + (56 - row * 24), 0.0f}, flag | 0x10, 0.5f, mario_status_icon_table[i]);
				}
				FontDrawStart();
				FontDrawColor(color);
				sprintf(buf, "%s%3d", mario_status_name_table[i], *mario_status_point_table[i]);
				ty = 80 - row * 24;
				FontDrawString(36.0f, wp->scroll + ty, buf);
				if (row == wp->cursor) {
					if (lv == 0) {
						sprintf(help, "help");
					} else {
						sprintf(help, "helperr");
					}
					sprintf(msgName, "%s_leveldown%s", help, mario_status_labelname_table[i]);
					v = *mario_status_point_table[i] - mario_status_henka_table[i];
					if (v > 0) {
						sprintf(buf, "%3d", v);
					} else {
						sprintf(buf, "  \x81\x48", v);
					}
					FontDrawStart();
					FontDrawString(206.0f, wp->scroll + ty, buf);
				}
				row++;
			}
			GXSetScissor(sl, st, sw, sh);
			if (*(s8*)msgName) {
				windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -240.0f, -130.0f, 480.0f, 70.0f, 20.0f);
				FontDrawStart();
				FontDrawMessage(-220, -138, msgSearch(msgName));
			}
			break;
		}
	}

	if (wp->field_0x50 != 0 && wp->field_0x50 != 1) {
		char text[136];
		char* line1;
		char* line2 = NULL;
		s32 lines;
		s32 k;
		s8* p;
		s32 w2;

		switch (wp->field_0x50) {
			case 0:
			case 1:
				break;
			case 11:
			case 14:
			case 15:
				strcpy(text, msgSearch("msg_window_select_6"));
				break;
			case 17:
				strcpy(text, msgSearch("msg_window_select_8"));
				break;
			case 18:
				strcpy(text, msgSearch("msg_window_select_7"));
				break;
			case 13:
			case 16:
				strcpy(text, msgSearch("msg_window_select_10"));
				break;
			case 2:
			case 12:
				strcpy(text, msgSearch("msg_window_select_2"));
				break;
			case 3:
				strcpy(text, msgSearch("msg_window_select_3"));
				break;
			case 4:
				strcpy(text, msgSearch("msg_window_select_4"));
				break;
			case 5:
				strcpy(text, msgSearch("msg_window_select_11"));
				break;
			case 6:
				strcpy(text, msgSearch("msg_window_select_11"));
				break;
			case 7:
				strcpy(text, msgSearch("msg_window_select_12"));
				break;
			case 8:
			case 9:
				strcpy(text, msgSearch("msg_window_select_9"));
				break;
			case 10:
				strcpy(text, msgSearch("msg_window_select_9_1"));
				break;
		}
		line1 = text;
		lines = 1;
		for (p = (s8*)line1, k = 0; *p; p++, k++) {
			if (*p == '\n') {
				text[k] = 0;
				lines = 2;
				line2 = &text[k] + 1;
				break;
			}
		}
		windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -280.0f, 100.0f, 240.0f, (lines - 1) * 24 + 45, 20.0f);
		FontDrawStart();
		k = line1 ? FontGetMessageWidth(line1) : 0;
		if (line2) {
			w2 = FontGetMessageWidth(line2);
		} else {
			w2 = 0;
		}
		if (w2 > k) {
			k = w2;
		}
		if (k <= 228) {
			k = (228 - k) / 2;
		} else {
			FontDrawScale(228.0f / k);
			k = 0;
		}
		FontDrawString(k - 274, 90.0f, line1);
		if (line2) {
			FontDrawString(k - 274, 66.0f, line2);
		}
		iconFlagOff("yubicursor_2", 2);
		iconSetPos("yubicursor_2", wp->cursorX, wp->cursorY, 0.0f);
	} else {
		iconFlagOn("yubicursor_2", 2);
	}
}


USER_FUNC(set_new_goods_list) {
	s32* args = event->args;
	UnitWinWork* wp = (UnitWinWork*)evtGetValue(event, args[0]);
	s32 item = evtGetValue(event, args[1]);
	char name[32];
	wp->goods[wp->goodsIdx] = item;
	if (wp->goodsIdx < wp->goodsMax) {
		sprintf(name, "uwn%d", wp->goodsIdx);
		if (item != 0) {
			((void (*)(char*, u16))iconChange)(name, itemDataTable[item].icon_id);
		} else {
			iconDelete(name);
		}
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(unitwin_get_work_ptr) {
	evtSetValue(event, event->args[0], (s32)_wp);
	return EVT_RETURN_DONE;
}

USER_FUNC(unitwin_getpartylank) {
	s32* args = event->args;
	PouchData* pouch = pouchGetPtr();
	s32 idx = evtGetValue(event, args[0]);
	evtSetValue(event, args[1], pouch->partyData[idx].techLevel);
	return EVT_RETURN_DONE;
}

#define PP ((PouchPartyData*)((u8*)pouch + idx))
USER_FUNC(unitwin_setpartylank) {
	PouchPartyData* new_var2;
	unsigned int idx;
	s16 v;
	s32 new_var;
	s32* args = event->args;
	PouchData* pouch = pouchGetPtr();
	idx = evtGetValue(event, args[0]) * 14;
	v = evtGetValue(event, args[1]);
	new_var = idx;
	new_var2 = (PouchPartyData*)((u8*)pouch + new_var);
	new_var2->HPLevel = ((PouchPartyData*)((u8*)pouch + new_var))->attackLevel = ((PouchPartyData*)((u8*)pouch + new_var))->techLevel = v;
	pouchRevisePartyParam();
	PP->currentHP = PP->maximumHP;
	return EVT_RETURN_DONE;
}
#undef PP

USER_FUNC(unitwin_setmariolevel) {
	s32* args = event->args;
	PouchData* pouch = pouchGetPtr();
	s32 i;
	BOOL r;
	pouch->baseMaxHP = evtGetValue(event, args[0]);
	pouch->baseMaxFP = evtGetValue(event, args[1]);
	pouch->maximumBP = evtGetValue(event, args[2]);
	pouchReviseMarioParam();
	pouch->currentHP = pouch->maximumHP;
	pouch->currentFP = pouch->maximumFP;
	if (pouch->availableBP < 0) {
		for (i = 0; i < 200; i++) {
			if (pouchEquipCheckBadgeIndex(i)) {
				pouchUnEquipBadgeIndex(i);
			}
		}
		pouchReviseMarioParam();
		r = TRUE;
	} else {
		r = FALSE;
	}
	evtSetValue(event, args[3], r);
	return EVT_RETURN_DONE;
}
