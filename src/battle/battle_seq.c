#pragma optimize_for_size on
#include "battle/battle_seq.h"
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "battle/battle_unit_event.h"
#include "driver/animdrv.h"
#include "driver/mapdrv.h"
#include "eff/eff_nice.h"
#include "battle/battle_database_common.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "mario/mario_party.h"
#include "party.h"
#include "pmario_sound.h"
#include "system.h"
#include "driver/dispdrv.h"
#include "battle/battle_status_effect.h"
#include "driver/msgdrv.h"
#include "driver/windowdrv.h"
#include "manager/fontmgr.h"
#include "driver/camdrv.h"
#include <stdio.h>
#include <string.h>

extern BattleWork* _battleWorkPointer;

#define FIELD(p, type, off) (*(type*)((u8*)(p) + (off)))

// subset of BattleStageObjectWork (battle_stage_object.c), lives in BattleWork at 0x180C0
typedef struct BattleSeqStageWork {
	u8 field_0x0[0x20];             // 0x0
	s32 bgDestroyEvtId;             // 0x20
	u8 field_0x24[0x1D8 - 0x24];    // 0x24
	s32 nozzleWorkEvtId;            // 0x1D8
	u8 field_0x1DC[0x1E4 - 0x1DC];  // 0x1DC
	s32 closeWallEvtId;             // 0x1E4
	s32 fallFrameEvtId;             // 0x1E8
	s32 objectFallEvtId;            // 0x1EC
} BattleSeqStageWork;

#define STAGE_WORK(wp) ((BattleSeqStageWork*)(wp)->field_180C0)

// external prototypes
f32 intpl_sub(s32 type, f32 start, f32 end, s32 cur, s32 max);
void mapObjScale(const char* name, f32 x, f32 y, f32 z);
BOOL BattleAfterReactionMain(void);
void BtlActRec_JudgeTurnRuleKeep(void);
void BtlActRec_AddCount(u8* count);
void BattleMajinaiCheck(void);
void BattleFogEndCheck(void);
void BattleStatusWindowEventOn(void);
void BattleStatusWindowSystemOn(void);
void BattleStatusWindowSystemOff(void);
void BattleStatusWindowEventOff(void);
void BattleCommandInit(BattleWork* wp);
BOOL BattleCommandInput(BattleWork* wp);
f32 battleCameraGetPosMoveSpeed(void);
void BattleStatusChangeMsgAdjust(BattleWork* wp);
BOOL BattleStatusChangeMsgMain(BattleWork* wp);
BOOL BattleStatusChangeAnnouceMain(BattleWork* wp);
void BattleConsumeReserveItem(void);
BOOL BattleBreakSlot_CheckReaction(void);
extern s32 btldefaultevt_CantMoveZeroGravity[];
extern s32 btldefaultevt_BiribiriMove[];
void BattleMajinaiDone(void);
void BattleMajinaiEndCheck(void);
extern s32 marioAttackEvent_MajinaiDefenceUp[];
void BtlUnit_SetTotalHitDamage(BattleUnit* unit, s32 damage);
BOOL BtlUnit_CheckShadowGuard(BattleUnit* unit);
s32 BattleDamageDirect(s32 unitIdx, BattleUnit* unit, BattleUnitPart* part, s32 damage, s32 fpDamage, u32 stopFlags, u32 damageEffect, u32 pattern);
void effRecoveryEntry(s32 type, s32 amount, f32 x, f32 y, f32 z);
void effFreezeN64Entry(s32 a, s32 b, f32 x, f32 y, f32 z, f32 scale);
void BattleAudience_Case_TurnEnd(void);
extern s32 subsetevt_status_recovery[];
extern s32 subsetevt_regeneration[];
extern s32 subsetevt_happy_heart[];
extern s32 subsetevt_happy_flower[];
void BattleSetStatusDamageFromWeapon(BattleUnit* attacker, BattleUnit* target, BattleUnitPart* part, BattleWeapon* weapon, u32 flags);
extern s32 marioDefaultWeapon_Jump[];
extern s32 marioWeapon_KururinJump[];
extern s32 marioDefaultWeapon_Hammer[];
extern s32 marioDefaultWeapon_FirstAttackKaitenHammer[];
extern s32 marioDefaultWeapon_FirstAttackUltraHammer[];
extern s32 partyWeapon_NokotarouFirstAttack[];
extern s32 partyWeapon_SandersFirstAttack[];

//local prototypes
USER_FUNC(_set_haikei_entry_scale);
void _mapobj_data_touch_scale(s32 layer);
BOOL BattleCheckConcluded(BattleWork* wp);
BOOL BattleWaitAllActiveEvtEnd_NoBgSetEndWait(BattleWork* wp);
BOOL BattleWaitAllActiveEvtEnd(BattleWork* wp);
void battleMakePhaseEvtTable(BattleWork* wp);
void battleSortPhaseMoveTable(BattleWork* wp, s32 count);
void btlseqPhaseFirstProcess(BattleWork* wp);
void btlseqTurnFirstProcess(BattleWork* wp);
void btlseqAct(BattleWork* wp);
BOOL BattlePhaseEndCheck(void);
void btlseqMove(BattleWork* wp);
void btlseqPhase(BattleWork* wp);
void _rule_disp(CameraId cameraId, void* param);
USER_FUNC(_set_effect_luck);

extern s32 btlevtcmd_WaitEventEnd(EventEntry* event, BOOL isFirstCall);
s32 _set_haikei_entry_scale(EventEntry* event, BOOL isFirstCall);
__declspec(section ".sdata") s32 nanika_luck_event[] = {
	0x1006f, (s32)0xfe363c8f, 0x10009, 0x3c, 0x70, 0x1005b, (s32)_set_effect_luck, 0x2005b,
	(s32)btlevtcmd_WaitEventEnd, (s32)0xfe363c8f, 2, 1,
};
__declspec(section ".sdata") s32 _default_haikei_set_entry_event[] = {
	0x1005b, (s32)_set_haikei_entry_scale, 2, 1,
};

BattleWeapon* _GetFirstAttackWeapon(s32 type);
void BattleCheckAllPinchStatus(BattleWork* wp, BOOL a2);
void btlseqInit(BattleWork* wp);
void btlseqFirstAct(BattleWork* wp);
void btlseqTurn(BattleWork* wp);
void btlseqEnd(BattleWork* wp);

// static data from an inlined header function (see unit_party_sanders.c)
__declspec(section ".data") s32 lbl_80360FE0[] = {
	-1082130432, 1065353216,
};
static __declspec(section ".data") s32 seq_negone_one_374[] = {
	-1082130432, 1065353216,
};
__declspec(section ".data") s32 lbl_80360FF0[] = {
	1065353216, -1082130432,
};
static __declspec(section ".data") s32 seq_one_negone_373[] = {
	1065353216, -1082130432,
};
__declspec(section ".data") s32 lbl_80361000[] = {
	-1082130432, 1065353216,
};
static __declspec(section ".data") s32 seq_negone_one_360[] = {
	-1082130432, 1065353216,
};
__declspec(section ".data") s32 lbl_80361010[] = {
	1065353216, -1082130432,
};
static __declspec(section ".data") s32 seq_one_negone_359[] = {
	1065353216, -1082130432,
};

const s32 N_weapon_lucky_start[48] = {
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x64000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x01004020, 0x10000000, 0x00000302, 0x00000000, 0x00020108, 0x000007FF, 0x80002004,
	0x00000000, 0x00000000, 0x00000064, 0x03640300, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000302, 0x03020000, 0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

static const GXColor c = {0, 0, 0, 0xFF};
static const GXColor color = {0xFF, 0xFF, 0xFF, 0xFF};

const char* _rule_msg_table_2bu[] = {
	"a", "stg3_tou_cp_001", "a", "stg3_tou_cp_002", "a", "stg3_tou_cp_004", "stg3_tou_cp_008", "a",
	"a", "a", "a", "a", "a", "a", "a", "a",
	"a", "a", "stg3_tou_cp_000", "a", "a", "a", "a", "a",
	"a", "a", "stg3_tou_cp_005", "a", "a", "a", "a", "a",
	"stg3_tou_cp_006", "a", "a", "a", "a", "a", "a", "a",
	"a", "a", "a", "stg3_tou_cp_009", "a", "a", "a", "a",
	"stg3_tou_cp_003", "a", "a", "a", "a", "a", "a", "a",
	"a", "a", "a", "stg3_tou_cp_007", "a", "a", "a", "a",
	"a", "a", "a", "a",
};

const char* _rule_msg_table_1bu[] = {
	"test", "stg3_tou_cp_011", "a", "stg3_tou_cp_012", "a", "stg3_tou_cp_014", "stg3_tou_cp_019", "a",
	"a", "a", "a", "stg3_tou_cp_020", "a", "a", "a", "a",
	"a", "a", "stg3_tou_cp_010", "a", "stg3_tou_cp_018", "a", "a", "a",
	"a", "a", "stg3_tou_cp_015", "a", "a", "a", "a", "a",
	"stg3_tou_cp_016", "a", "a", "a", "a", "a", "a", "a",
	"a", "a", "a", "stg3_tou_cp_021", "a", "a", "a", "a",
	"stg3_tou_cp_013", "a", "a", "a", "a", "a", "a", "a",
	"a", "a", "a", "stg3_tou_cp_017", "a", "a", "a", "stg3_tou_cp_024",
	"stg3_tou_cp_023", "stg3_tou_cp_022", "a", "a",
};

const char* _rule_msg_table_after[] = {
	"a", "stg3_tou_cp_041", "a", "stg3_tou_cp_042", "a", "stg3_tou_cp_044", "stg3_tou_cp_049", "a",
	"a", "a", "a", "stg3_tou_cp_050", "a", "a", "a", "a",
	"a", "a", "stg3_tou_cp_040", "a", "stg3_tou_cp_048", "a", "a", "a",
	"a", "a", "stg3_tou_cp_045", "a", "a", "a", "a", "a",
	"stg3_tou_cp_046", "a", "a", "a", "a", "a", "a", "a",
	"a", "a", "a", "stg3_tou_cp_051", "a", "a", "a", "a",
	"stg3_tou_cp_043", "a", "a", "a", "a", "a", "a", "a",
	"a", "a", "a", "stg3_tou_cp_047", "a", "a", "a", "stg3_tou_cp_054",
	"stg3_tou_cp_053", "stg3_tou_cp_052", "a", "a",
};

void BattleSequenceManager(void) { //1:1
	BattleWork* wp = _battleWorkPointer;
	switch (BattleGetSeq(wp, SEQ_UNKNOWN)) {
		case 0:
			btlseqInit(wp);
			BattleCheckAllPinchStatus(wp, TRUE);
			break;
		case 1:
			BattleCheckAllPinchStatus(wp, FALSE);
			btlseqFirstAct(wp);
			break;
		case 2:
			BattleCheckAllPinchStatus(wp, FALSE);
			btlseqTurn(wp);
			break;
		case 3:
			btlseqEnd(wp);
			break;
		case 4:
			break;
	}
}

void BattleCheckAllPinchStatus(BattleWork* wp, BOOL a2) { //1:1, needs stmw forced
	BattleUnit* unit;
	BOOL v6;
	int i;

	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(wp, i);
		if (unit) {
			BtlUnit_CheckPinchStatus(unit, a2);
		}
	}

	v6 = FALSE;
	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(wp, i);
		if (unit) {
			if ((unit->mStatusFlags & 0x20000000) &&
				!BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) &&
				!BtlUnit_GetBelong(unit))
			{
				if (unit->currentKind == UNIT_MARIO ||
					unit->currentKind >= TYPE_PARTNER_MIN &&
					unit->currentKind < TYPE_PARTNER_MAX) {
					if (unit->currentHp > 0) {
						v6 = TRUE;
					}
				}
				break;
			}
		}
	}

	if (v6) {
		wp->alertTick--;
		if (wp->alertTick <= 0) {
			wp->alertTick = 60;
			if (wp->commandMenu.field_4 & 1) {
				psndSFXOn("SFX_BTL_DANGER1");
			}
		}
	}
	else {
		v6 = FALSE;
		for (i = 0; i < 64; i++) {
			unit = BattleGetUnitPtr(wp, i);
			if (unit) {
				if ((unit->mStatusFlags & 0x10000000) &&
					!BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) &&
					!BtlUnit_GetBelong(unit))
				{
					if (unit->currentKind == UNIT_MARIO ||
						unit->currentKind >= TYPE_PARTNER_MIN &&
						unit->currentKind < TYPE_PARTNER_MAX) {
						if (unit->currentHp > 0) {
							v6 = TRUE;
						}
					}
					break;
				}
			}
		}
		if (v6) {
			wp->alertTick--;
			if (wp->alertTick <= 0) {
				wp->alertTick = 60;
				if (wp->commandMenu.field_4 & 1) {
					psndSFXOn("SFX_BTL_PINCH1\0\0\0\0\0");
				}
			}
		}
		else {
			unit = BattleGetMarioPtr(wp);
			if (unit) {
				unit->work[8] = 0;
			}
			wp->alertTick = 0;
		}
	}
}

void btlseqInit(BattleWork* wp) { //1:1, needs stmw
	MarioWork* mario;
	PartyEntry* party;

	switch (BattleGetSeq(wp, SEQ_INIT)) {
		case 0x1000000: //continues through to 0x1000001
			BattleSetSeq(wp, SEQ_INIT, 0x1000001);

		case 0x1000001:
			mario = marioGetPtr();
			mario->dispFlags |= 1;
			party = partyGetPtr(marioGetPartyId());
			if (party) {
				party->flags2 |= 1;
			}
			Btl_UnitSetup(wp);
			BattleSetSeq(wp, SEQ_INIT, 0x1000002);
			break;

		case 0x1000002:
			if (BattleCheckEndUnitInitEvent(wp)) {
				BattleSetSeq(wp, SEQ_INIT, 0x1000003u);
			}
			break;

		case 0x1000003:
			if (animEffectAsync("p_b_st", 1) &&
				animEffectAsync("p_jyabara", 1) &&
				animEffectAsync("p_kaiten_h", 1) &&
				effNiceAsync(1))
			{
				BattleSetSeq(wp, SEQ_INIT, 0x1000004);
			}
			break;

		case 0x1000004:
			BattleSetSeq(wp, SEQ_INIT, 0x1000005);
			break;

		case 0x1000005:
			BattleStatusWindowEventOn();
			BattleStatusWindowSystemOn();
			BattleSetSeq(wp, SEQ_UNKNOWN, 1);
			BattleSetSeq(wp, SEQ_FIRST_ACT, 0x2000000);
			break;
	}
}

BattleWeapon* _GetFirstAttackWeapon(s32 type) {
	switch (type) {
		case 1:
			return (BattleWeapon*)marioDefaultWeapon_Jump;
		case 2:
			return (BattleWeapon*)marioWeapon_KururinJump;
		case 3:
			return (BattleWeapon*)marioDefaultWeapon_Jump;
		case 4:
			return (BattleWeapon*)marioDefaultWeapon_Hammer;
		case 5:
			return (BattleWeapon*)marioDefaultWeapon_FirstAttackKaitenHammer;
		case 6:
			return (BattleWeapon*)marioDefaultWeapon_FirstAttackUltraHammer;
		case 7:
			return (BattleWeapon*)partyWeapon_NokotarouFirstAttack;
		case 8:
			return (BattleWeapon*)partyWeapon_SandersFirstAttack;
		case 0:
		default:
			return NULL;
	}
}


BattleUnit* BattleGetMarioPtr(BattleWork* wp);
void BattleAudience_Case_FirstAttack_Good(void);
void BattleAudience_Case_FirstAttack_Bad(void);
void BattleStage_WallCloseCheck(void);
void BattleStage_NozzleWorkCheck(s32 a);
void BattleGetFirstAttackUnit(BattleWork* wp, BattleUnit** a, BattleUnit** b);
BOOL fadeIsFinish(void);

#define U_EVT(u) FIELD(u, s32*, 0x2B4)
#define U_EVTID(u) FIELD(u, s32, 0x2B8)

void btlseqFirstAct(BattleWork* wp) {
	s32 type;
	int i;
	BattleUnit* unit;
	BattleUnit* a;
	BattleUnit* b;
	EventEntry* evt;
	int found;
	s32 flag;
	int k;
	u8* fb = (u8*)wp->fbatInfo;
	u8* info;
	type = FIELD(fb, s32, 8);

	switch (BattleGetSeq(wp, SEQ_FIRST_ACT)) {
		case 0x2000000:
			switch (type) {
			case 0:
			{
				wp->ruleDispTimer = 0;
				evtEntry(_default_haikei_set_entry_event, 10, 0);
				info = FIELD(wp->fbatInfo, u8*, 0xC);
				for (i = 0; i < 64; i++) {
					unit = BattleGetUnitPtr(wp, i);
					if (unit && U_EVT(unit)) {
						if ((FIELD(info, u32, 4) & 0x100000) && (unit->currentKind == 0xDE || (unit->currentKind >= 0xE0 && unit->currentKind < 0xE7))) {
							continue;
						}
						if (FIELD(marioGetPtr(), u16, 0x2E) == 0x1A && unit->currentKind == 0xDE) {
							evt = evtEntry((s32*)BtlUnit_GetData(unit, 0x41), 10, 0);
							U_EVTID(unit) = evt->eventId;
							evt->unitId = unit->unitId;
						}
						else {
							evt = evtEntry(U_EVT(unit), 10, 0);
							U_EVTID(unit) = evt->eventId;
							evt->unitId = unit->unitId;
						}
					}
				}
				BattleSetSeq(wp, SEQ_FIRST_ACT, 0x2000001);
				goto done_fa;
			}
			}
			switch (type) {
			case 1: case 2: case 3:
				wp->ruleDispTimer = 10;
				break;
			case 4: case 5: case 6: case 7: case 8:
				wp->ruleDispTimer = 0;
				break;
			default:
				wp->ruleDispTimer = 0;
				break;
			}
			wp->flags |= 2;
			BattleGetFirstAttackUnit(wp, &a, &b);
			wp->activeUnitId = a->unitId;
			FIELD(a, u32, 0x278) |= 0x1000;
			evt = evtEntry(FIELD(b, s32*, 0x2AC), 10, 0x20);
			if (evt) {
				evt->unitId = b->unitId;
				FIELD(b, s32, 0x274) = 0x39;
				FIELD(b, s32, 0x2B0) = evt->eventId;
			}
			FIELD(a, u32, 0x1C) |= 0x4000000;
			if (type < 9) {
				FIELD(a, s32, 0x29C) = FIELD(_GetFirstAttackWeapon(type), s32, 0xB0);
				FIELD(a, s32, 0x2A8) = 0;
				BattleStatusWindowEventOff();
			}
			if (BtlUnit_GetBelong(a) == 0) {
				BattleAudience_Case_FirstAttack_Good();
			}
			else {
				BattleAudience_Case_FirstAttack_Bad();
			}
			evt = evtEntry(FIELD(a, s32*, 0x29C), 10, 0);
			if (evt) {
				evt->unitId = a->unitId;
				FIELD(a, s32, 0x2A4) = evt->eventId;
			}
			switch (type) {
			case 1: case 2: case 3: case 4: case 5: case 6:
				unit = BattleGetPartyPtr(wp);
				if (unit) {
					evt = evtEntry(U_EVT(unit), 10, 0);
					U_EVTID(unit) = evt->eventId;
					evt->unitId = unit->unitId;
				}
				break;
			case 7: case 8:
				unit = BattleGetMarioPtr(wp);
				if (unit) {
					evt = evtEntry(U_EVT(unit), 10, 0);
					U_EVTID(unit) = evt->eventId;
					evt->unitId = unit->unitId;
				}
			}
			BattleSetSeq(wp, SEQ_FIRST_ACT, 0x2000001);
		done_fa:
			break;

		case 0x2000001:
			if (--wp->ruleDispTimer > 0) {
				break;
			}
			{
				fadeEntry(0x17, 200, c);
			}
			wp->field_19060 = ((s32 (*)(const char*))psndSFXOn)("SFX_OFF_BTL_START_CURTAIN_UP1");
			BattleSetSeq(wp, SEQ_FIRST_ACT, 0x2000002);

		case 0x2000002:
			if (fadeIsFinish() && (u32)wp->field_19060 != 0xFFFFFFFFU) {
				psndSFXOff(wp->field_19060);
				wp->field_19060 = -1;
			}
			if (BattleWaitAllActiveEvtEnd(wp)) {
				BattleStatusWindowEventOn();
			}
			if (BattleWaitAllActiveEvtEnd(wp) && fadeIsFinish()) {
				BattleSetSeq(wp, SEQ_FIRST_ACT, 0x2000003);
				unit = BattleGetUnitPtr(wp, wp->activeUnitId);
				if (unit) {
					FIELD(unit, u32, 0x1C) &= ~0x4000000;
				}
			}
			break;

		case 0x2000003:
			if (BattleCheckConcluded(wp)) {
				BattleAudience_PerAct();
				BattleSetSeq(wp, SEQ_UNKNOWN, 3);
				BattleSetSeq(wp, 7, 0x7000000);
				break;
			}
			BattleSetSeq(wp, SEQ_FIRST_ACT, 0x2000004);
			BattleStatusChangeMsgAdjust(wp);

		case 0x2000004:
			if (!BattleWaitAllActiveEvtEnd(wp)) {
				break;
			}
			if (BattleStatusChangeAnnouceMain(wp) | BattleStatusChangeMsgMain(wp)) {
				break;
			}
			BattleIncSeq(wp, SEQ_FIRST_ACT);

		case 0x2000005:
			if (!(wp->flags & 0x30)) {
				BattleStatusWindowSystemOn();
			}
			BattleConsumeReserveItem();
			wp->flags &= ~4;
			BattleAudience_PerAct();
			BattleIncSeq(wp, SEQ_FIRST_ACT);

		case 0x2000006:
			if ((u8)BattleAudience_CheckReaction()) {
				break;
			}
			if (!BattleCheckConcluded(wp) && (u8)BattleBreakSlot_CheckReaction()) {
				break;
			}
			BattleIncSeq(wp, SEQ_FIRST_ACT);

		case 0x2000007:
			if (BattleStatusChangeAnnouceMain(wp) | BattleStatusChangeMsgMain(wp)) {
				break;
			}
			BattleIncSeq(wp, SEQ_FIRST_ACT);

		case 0x2000008:
			if (!BattleWaitAllActiveEvtEnd(wp)) {
				break;
			}
			if (FIELD(FIELD(wp->fbatInfo, u8*, 0xC), u32, 4) & 0x10) {
				BattleSetSeq(wp, SEQ_FIRST_ACT, 0x2000009);
				BattleStage_WallCloseCheck();
			}
			else {
				BattleSetSeq(wp, SEQ_FIRST_ACT, 0x200000A);
			}
			break;

		case 0x2000009:
		{
			s32* id = &FIELD(wp, s32, 0x182A4);
			if (*id != 0 && !evtCheckID(*id)) {
				*id = 0;
			}
			if (*id == 0) {
				BattleSetSeq(wp, SEQ_FIRST_ACT, 0x200000A);
			}
		}
			break;

		case 0x200000A:
			wp->luckEventId = 0;
			flag = 0;
			k = 0;
			do {
				unit = BattleGetUnitPtr(wp, k);
				if (unit && FIELD(unit, u8, 0x300) && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
					flag = 1;
				}
			} while (++k < 64);
			if (flag) {
				evt = evtEntry(nanika_luck_event, 10, 0);
				evt->unitId = -5;
				wp->luckEventId = evt->eventId;
				FIELD(wp, s64, 0x18FD8) |= -1;
				BattleStatusChangeMsgAdjust(wp);
				BattleSetSeq(wp, SEQ_FIRST_ACT, 0x200000B);
				wp->luckTimer = 0;
			}
			else {
				BattleSetSeq(wp, SEQ_FIRST_ACT, 0x200000C);
			}
			break;

		case 0x200000B:
			found = 1;
			if (wp->luckTimer < 300) {
				wp->luckTimer++;
			}
			if (wp->luckEventId != 0) {
				if (evtCheckID(wp->luckEventId)) {
					found = 0;
				}
				else {
					wp->luckEventId = 0;
				}
			}
			if (!BattleWaitAllActiveEvtEnd(wp)) {
				found = 0;
			}
			if (BattleStatusChangeAnnouceMain(wp)) {
				found = 0;
			}
			if (BattleStatusChangeMsgMain(wp)) {
				found = 0;
			}
			if (found) {
				BattleSetSeq(wp, SEQ_FIRST_ACT, 0x200000C);
			}
			break;

		case 0x200000C:
			if (wp->flags & 2) {
				BattleSetSeq(wp, SEQ_FIRST_ACT, 0x200000E);
				break;
			}
			BattleStage_NozzleWorkCheck(0);
			BattleSetSeq(wp, SEQ_FIRST_ACT, 0x200000D);

		case 0x200000D:
			if (!BattleWaitAllActiveEvtEnd(wp)) {
				break;
			}
			BattleSetSeq(wp, SEQ_FIRST_ACT, 0x200000E);

		case 0x200000E:
			wp->flags &= ~2;
			BattleSetSeq(wp, SEQ_UNKNOWN, 2);
			BattleSetSeq(wp, 3, 0x3000000);
			break;
	}
}


//==== written below (reverse order) ====

USER_FUNC(_set_effect_luck) {
	BattleWork* wp = _battleWorkPointer;
	BattleWeapon weapon = *(BattleWeapon*)N_weapon_lucky_start;
	f32 z, y, x;
	const char* sfx;
	f32* pz;
	f32* py;
	BattleWeapon* pw;
	BattleUnit* unit;
	BattleUnitPart* part;
	NiceEffectWork* effect;
	int i;

	sfx = (0, "SFX_SYSTEM_LUCKY1");
	pw = &weapon;
	pz = &z;
	py = &y;
	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(wp, i);
		if (unit && unit->badgesEquipped.LuckyStart && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
			switch (irand(4)) {
				case 0:
					weapon.dodgyChance = 0;
					weapon.dodgyTime = 0;
					weapon.hpRegenTime = 0;
					weapon.hpRegenStrength = 0;
					weapon.fpRegenTime = 0;
					weapon.fpRegenStrength = 0;
					break;
				case 1:
					weapon.electricChance = 0;
					weapon.electricTime = 0;
					weapon.hpRegenTime = 0;
					weapon.hpRegenStrength = 0;
					weapon.fpRegenTime = 0;
					weapon.fpRegenStrength = 0;
					break;
				case 2:
					weapon.electricChance = 0;
					weapon.electricTime = 0;
					weapon.dodgyChance = 0;
					weapon.dodgyTime = 0;
					weapon.fpRegenTime = 0;
					weapon.fpRegenStrength = 0;
					break;
				default:
					weapon.electricChance = 0;
					weapon.electricTime = 0;
					weapon.dodgyChance = 0;
					weapon.dodgyTime = 0;
					weapon.hpRegenTime = 0;
					weapon.hpRegenStrength = 0;
					break;
			}
			part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
			BattleSetStatusDamageFromWeapon(BattleGetSystemPtr(wp), unit, part, pw, 0x100);
			BtlUnit_GetPos(unit, &x, py, pz);
			psndSFXOn(sfx);
			{
				EffectEntry* e = effNiceEntry(6, x - 45.0f, 60.0f + y, z);
				effect = e->userdata;
				effect->field_0x1C = 0.75f;
			}
		}
	}
	return EVT_RETURN_DONE;
}


#define TURN_NEXT(wp) \
	BattleIncSeq(wp, SEQ_TURN); \
	btlseqTurn(wp)

#define TURN_SKIP(wp) \
	BattleIncSeq(wp, SEQ_TURN); \
	BattleIncSeq(wp, SEQ_TURN); \
	btlseqTurn(wp)

#define TURN_WAIT_CASE(wp) \
	if (!BattleWaitAllActiveEvtEnd(wp)) { \
		break; \
	} \
	if (BattleCheckConcluded(wp)) { \
		BattleSetSeq(wp, SEQ_UNKNOWN, 3); \
		BattleSetSeq(wp, SEQ_END, 0x7000000); \
	} \
	else { \
		BattleIncSeq(wp, SEQ_TURN); \
	} \
	break

#define TURN_RECOVERY_CASE(wp, status) \
	for (i = 0; i < 64; i++) { \
		unit = BattleGetUnitPtr(wp, i); \
		ret = BtlUnit_CheckRecoveryStatus(unit, status); \
		sfx |= ret; \
		if (ret) { \
			evt = evtEntry(subsetevt_status_recovery, 10, 0); \
			if (evt) { \
				unit->attackEventId = evt->eventId; \
				evt->unitId = unit->unitId; \
			} \
			found = TRUE; \
		} \
	} \
	if (sfx) { \
		psndSFXOn("SFX_REST_CONDITION1"); \
	} \
	if (!found) { \
		TURN_SKIP(wp); \
		break; \
	} \
	TURN_NEXT(wp); \
	break

#define TURN_RECOVERY_CASE2(wp, status) \
	for (i = 0; i < 64; i++) { \
		unit = BattleGetUnitPtr(wp, i); \
		if (unit) { \
			ret = BtlUnit_CheckRecoveryStatus(unit, status); \
			sfx |= ret; \
			if (ret) { \
				evt = evtEntry(subsetevt_status_recovery, 10, 0); \
				if (evt) { \
					unit->attackEventId = evt->eventId; \
					evt->unitId = unit->unitId; \
				} \
				found = TRUE; \
			} \
		} \
	} \
	if (sfx) { \
		psndSFXOn("SFX_REST_CONDITION1"); \
	} \
	if (!found) { \
		TURN_SKIP(wp); \
		break; \
	} \
	TURN_NEXT(wp); \
	break

#define TURN_COMMAND_DISABLED_CASE(wp, n) \
	if (wp->commandDisabled[n]) { \
		wp->commandDisabled[n]--; \
	} \
	TURN_SKIP(wp); \
	break

void btlseqTurn(BattleWork* wp) {
	Vec pos;
	f32 hpX, hpY, hpZ;
	f32 fpX, fpY, fpZ;
	f32 heartX, heartY, heartZ;
	f32 flowerX, flowerY, flowerZ;
	s8 turns;
	s8 strength;
	BattleUnit* unit;
	EventEntry* evt;
	BOOL found;
	BOOL sfx;
	BOOL ret;
	BOOL done;
	s32 count;
	s32 n;
	s32 rate;
	int i;

	found = FALSE;
	sfx = FALSE;
	switch (BattleGetSeq(wp, SEQ_TURN)) {
		case 0x3000000:
			BattleIncSeq(wp, SEQ_TURN);
			break;
		case 0x3000001:
			btlseqTurnFirstProcess(wp);
			TURN_NEXT(wp);
			break;
		case 0x3000003:
			if (wp->turnCount != 1) {
				TURN_SKIP(wp);
				break;
			}
			if (wp->fbatInfo->field_0x18 == 0) {
				TURN_SKIP(wp);
				break;
			}
			wp->ruleDispTimer = 120;
			TURN_NEXT(wp);
			break;
		case 0x3000004:
			if (wp->ruleDispTimer) {
				wp->ruleDispTimer--;
			}
			if (wp->ruleDispTimer <= 0 || BattlePadCheckTrigger(0x300)) {
				BattleIncSeq(wp, SEQ_TURN);
				break;
			}
			dispEntry(CAMERA_2D, 900.0f, 1, _rule_disp, NULL);
			break;
		case 0x3000002:
			TURN_NEXT(wp);
			break;
		case 0x3000005:
			TURN_NEXT(wp);
			break;
		case 0x3000007:
		case 0x300000B:
		case 0x300000D:
		case 0x300000F:
		case 0x3000011:
		case 0x3000013:
		case 0x3000015:
		case 0x3000017:
		case 0x3000019:
		case 0x300001B:
		case 0x300001D:
		case 0x300001F:
		case 0x3000021:
		case 0x3000023:
		case 0x3000025:
		case 0x3000027:
		case 0x3000029:
		case 0x300002B:
		case 0x300002D:
		case 0x300002F:
		case 0x3000031:
		case 0x3000033:
		case 0x3000035:
		case 0x3000037:
		case 0x3000039:
		case 0x300003B:
		case 0x300003D:
		case 0x300003F:
		case 0x3000041:
		case 0x3000043:
		case 0x3000045:
		case 0x3000047:
		case 0x3000049:
			TURN_WAIT_CASE(wp);

		case 0x3000012:
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				ret = BtlUnit_CheckRecoveryStatus(unit, 1);
				sfx |= ret;
				if (ret) {
					evt = evtEntry(subsetevt_status_recovery, 10, 0);
					if (evt) {
						unit->attackEventId = evt->eventId;
						evt->unitId = unit->unitId;
					}
					found = TRUE;
				}
			}
			if (sfx) {
				psndSFXOn("SFX_REST_CONDITION1");
			}
			if (!found) {
				TURN_SKIP(wp);
				break;
			}
			BattleIncSeq(wp, SEQ_TURN);
			break;
		case 0x3000014:
			TURN_RECOVERY_CASE(wp, 2);
		case 0x3000016:
			TURN_RECOVERY_CASE(wp, 3);
		case 0x3000018:
			TURN_RECOVERY_CASE(wp, 5);
		case 0x300001A:
			TURN_RECOVERY_CASE(wp, 6);
		case 0x300001C:
			TURN_RECOVERY_CASE(wp, 7);
		case 0x300001E:
			TURN_RECOVERY_CASE(wp, 8);
		case 0x3000020: {
			BattleUnit* unit;
			void* sfxName = (0, "SFX_CONDITION_BREAK_ICE1");
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (BtlUnit_CheckRecoveryStatus(unit, 9)) {
					if (!BtlUnit_CheckShadowGuard(unit)) {
						BattleDamageDirect(-5, unit, NULL, 1, 0, 0x11A, 0, 1);
					}
					BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
					pos.y += BtlUnit_GetHeight(unit);
					effFreezeN64Entry(0, 1, pos.x, pos.y, pos.z, 1.0f);
					psndSFXOn_3D(sfxName, &pos);
					found = TRUE;
				}
			}
			if (!found) {
				TURN_SKIP(wp);
				break;
			}
			TURN_NEXT(wp);
			break;
		}
		case 0x3000022:
			TURN_RECOVERY_CASE(wp, 10);
		case 0x3000024:
			TURN_RECOVERY_CASE(wp, 11);
		case 0x3000026:
			TURN_RECOVERY_CASE(wp, 4);
		case 0x3000028:
			TURN_RECOVERY_CASE(wp, 12);
		case 0x300002A:
			TURN_RECOVERY_CASE(wp, 13);
		case 0x300002C:
			TURN_RECOVERY_CASE(wp, 14);
		case 0x300002E:
			TURN_RECOVERY_CASE(wp, 15);
		case 0x3000030:
			TURN_RECOVERY_CASE(wp, 0);
		case 0x3000032:
			TURN_RECOVERY_CASE2(wp, 0x17);
		case 0x3000034:
			TURN_RECOVERY_CASE2(wp, 0x18);
		case 0x3000036:
			TURN_RECOVERY_CASE2(wp, 0x12);
		case 0x3000038:
			TURN_RECOVERY_CASE2(wp, 0x13);
		case 0x300003A:
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (unit) {
					ret = BtlUnit_CheckRecoveryStatus(unit, 0x14);
					sfx |= ret;
					if (ret) {
						BtlUnit_ResetMoveStatus(unit);
						evt = evtEntry(subsetevt_status_recovery, 10, 0);
						if (evt) {
							unit->attackEventId = evt->eventId;
							evt->unitId = unit->unitId;
						}
						found = TRUE;
					}
				}
			}
			if (sfx) {
				psndSFXOn("SFX_REST_CONDITION1");
			}
			if (!found) {
				TURN_SKIP(wp);
				break;
			}
			TURN_NEXT(wp);
			break;
		case 0x300003C:
			TURN_RECOVERY_CASE2(wp, 0x15);
		case 0x300003E:
			TURN_RECOVERY_CASE2(wp, 0x16);
		case 0x3000006:
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (unit && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) && unit->currentHp > 0) {
					BtlUnit_GetStatus(unit, 4, &turns, &strength);
					if (strength > 0 && !BtlUnit_CheckShadowGuard(unit)) {
						found = TRUE;
						BattleDamageDirect(-5, unit, NULL, strength, 0, 0x117, 0, 1);
					}
				}
			}
			if (!found) {
				TURN_SKIP(wp);
				break;
			}
			TURN_NEXT(wp);
			break;
		case 0x3000008:
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (unit && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) && unit->currentHp > 0) {
					BtlUnit_GetStatus(unit, 8, &turns, &strength);
					if (strength > 0) {
						found = TRUE;
						break;
					}
				}
			}
			if (!found) {
				TURN_SKIP(wp);
				break;
			}
			wp->turnSubSeq = 0;
			TURN_NEXT(wp);
			break;
		case 0x3000009:
			switch (wp->turnSubSeq) {
				case 0:
					wp->turnSubSeq++;
					for (i = 0; i < 64; i++) {
						unit = BattleGetUnitPtr(wp, i);
						if (unit && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) && unit->currentHp > 0) {
							BtlUnit_GetStatus(unit, 8, &turns, &strength);
							if (strength > 0) {
								BSE_TurnFirstProcessEffectEntry(unit, 1);
							}
						}
					}
				case 1:
					done = TRUE;
					for (i = 0; i < 64; i++) {
						unit = BattleGetUnitPtr(wp, i);
						if (unit && BSE_TurnFirstProcessEffectMain(unit)) {
							done = FALSE;
						}
					}
					if (done) {
						wp->turnSubSeq++;
					}
					break;
				default:
					TURN_WAIT_CASE(wp);
			}
			break;
		case 0x300000A:
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (unit && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) && unit->currentHp > 0) {
					BtlUnit_GetStatus(unit, 0x17, &turns, &strength);
					if (strength > 0) {
						if (!(unit->mAttributeFlags & 0x40000000)) {
							unit->hpGauge.field_0xC = unit->currentHp;
							BtlUnit_RecoverHp(unit, strength);
							unit->hpGauge.field_0x10 = unit->currentHp;
							unit->hpGauge.field_0x4 = unit->hpGauge.field_0x6;
							unit->hpGauge.field_0x8 = unit->hpGauge.field_0xA;
							unit->hpGauge.field_0x2 |= 1;
						}
						else {
							BtlUnit_RecoverHp(unit, strength);
						}
						BtlUnit_GetPartsWorldPos(BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit)), &hpX, &hpY, &hpZ);
						hpY += unit->mSizeMultiplier * (BtlUnit_GetHeight(unit) + 5);
						effRecoveryEntry(0, strength, hpX, hpY, hpZ);
						evt = evtEntry(subsetevt_regeneration, 10, 0);
						if (evt) {
							unit->attackEventId = evt->eventId;
							evt->unitId = unit->unitId;
						}
						found = TRUE;
					}
				}
			}
			if (!found) {
				TURN_SKIP(wp);
				break;
			}
			TURN_NEXT(wp);
			break;
		case 0x300000C:
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (unit && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) && unit->currentHp > 0) {
					BtlUnit_GetStatus(unit, 0x18, &turns, &strength);
					if (strength > 0) {
						BtlUnit_RecoverFp(unit, strength);
						BtlUnit_GetPartsWorldPos(BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit)), &fpX, &fpY, &fpZ);
						fpY += unit->mSizeMultiplier * (BtlUnit_GetHeight(unit) + 5);
						effRecoveryEntry(1, strength, fpX, fpY, fpZ);
						evt = evtEntry(subsetevt_regeneration, 10, 0);
						if (evt) {
							unit->attackEventId = evt->eventId;
							evt->unitId = unit->unitId;
						}
						found = TRUE;
					}
				}
			}
			if (!found) {
				TURN_SKIP(wp);
				break;
			}
			TURN_NEXT(wp);
			break;
		case 0x300000E:
			if (wp->turnCount <= 1) {
				TURN_SKIP(wp);
				break;
			}
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (unit && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) && unit->currentHp > 0) {
					n = 0;
					for (count = unit->badgesEquipped.HappyHeart; count > 0; count--) {
						rate = 33 - (unit->currentHp / unit->maxHp) * 3;
						if (irand(100) < rate) {
							n++;
						}
					}
					if (n > 0) {
						unit->currentHp += n;
						if (unit->currentHp > unit->maxHp) {
							unit->currentHp = unit->maxHp;
						}
						BtlUnit_GetPartsWorldPos(BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit)), &heartX, &heartY, &heartZ);
						heartY += unit->mSizeMultiplier * BtlUnit_GetHeight(unit);
						effRecoveryEntry(0, n, heartX, heartY, heartZ);
						evt = evtEntry(subsetevt_happy_heart, 10, 0);
						if (evt) {
							unit->attackEventId = evt->eventId;
							evt->unitId = unit->unitId;
						}
						found = TRUE;
					}
				}
			}
			if (!found) {
				TURN_SKIP(wp);
				break;
			}
			BattleIncSeq(wp, SEQ_TURN);
			break;
		case 0x3000010:
			if (wp->turnCount <= 1) {
				TURN_SKIP(wp);
				break;
			}
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (unit && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) && unit->currentHp > 0) {
					n = 0;
					for (rate = unit->badgesEquipped.HappyFlower; rate > 0; rate--) {
						count = 33 - (unit->currentFp / unit->maxFp) * 3;
						if (irand(100) < count) {

							n++;
						}
					}
					if (n > 0) {
						BtlUnit_RecoverFp(unit, n);
						BtlUnit_GetPartsWorldPos(BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit)), &flowerX, &flowerY, &flowerZ);
						flowerY += unit->mSizeMultiplier * BtlUnit_GetHeight(unit);
						effRecoveryEntry(1, n, flowerX, flowerY, flowerZ);
						evt = evtEntry(subsetevt_happy_flower, 10, 0);
						if (evt) {
							unit->attackEventId = evt->eventId;
							evt->unitId = unit->unitId;
						}
						found = TRUE;
					}
				}
			}
			if (!found) {
				TURN_SKIP(wp);
				break;
			}
			TURN_NEXT(wp);
			break;
		case 0x3000040:
			TURN_COMMAND_DISABLED_CASE(wp, 0);
		case 0x3000042:
			TURN_COMMAND_DISABLED_CASE(wp, 1);
		case 0x3000044:
			TURN_COMMAND_DISABLED_CASE(wp, 2);
		case 0x3000046:
			TURN_COMMAND_DISABLED_CASE(wp, 3);
		case 0x3000048:
			TURN_COMMAND_DISABLED_CASE(wp, 4);
		case 0x300004A:
			TURN_NEXT(wp);
			break;
		case 0x300004B:
			BattleIncSeq(wp, SEQ_TURN);
			BattleSetSeq(wp, SEQ_PHASE, 0x4000000);
			break;
		case 0x300004C:
			btlseqPhase(wp);
			break;
		case 0x300004D:
			BattleAudience_Case_TurnEnd();
			BattleSetSeq(wp, SEQ_TURN, 0x3000001);
			wp->flags &= ~0x2000;
			break;
	}
}


void _rule_disp(CameraId cameraId, void* param) {
	FieldBattleInfo* info = _battleWorkPointer->fbatInfo;
	char buf[256];
	f32 x;
	f32 y;
	u16 lines;
	u16 width;

	if (evtGetValue(NULL, GSW(0)) < 0xAC) {
		if (evtGetValue(NULL, GSW(580)) < 11) {
			sprintf(buf, msgSearch(_rule_msg_table_1bu[info->field_0x18]), info->field_0x1A);
		}
		else {
			sprintf(buf, msgSearch(_rule_msg_table_2bu[info->field_0x18]), info->field_0x1A);
		}
	}
	else {
		sprintf(buf, msgSearch(_rule_msg_table_after[info->field_0x18]), info->field_0x1A);
	}
	width = FontGetMessageWidthLine(buf, &lines);
	lines++;
	y = 0.0f;
	x = y - (f32)(width / 2);
	windowDispGX_Waku_col(0, color, x - 10.0f, 94.0f, (f32)(width + 20), (f32)(lines * 29 + 3), 10.0f);
	FontDrawStart();
	y = 120.0f;
	FontDrawMessage((s32)x, (s32)y - 29, buf);
}


void btlseqPhase(BattleWork* wp) {
	switch (BattleGetSeq(wp, SEQ_PHASE)) {
		case 0x4000000:
			BattleIncSeq(wp, SEQ_PHASE);
			BattleSetSeq(wp, SEQ_MOVE, 0x5000000);
			break;
		case 0x4000001:
		case 0x4000002:
		case 0x4000003:
		case 0x4000004:
		case 0x4000005:
			btlseqMove(wp);
			break;
		case 0x4000006:
			BattleIncSeq(wp, SEQ_TURN);
			BattleSetSeq(wp, SEQ_PHASE, 0x4000000);
			break;
	}
}

void btlseqMove(BattleWork* wp) {
	int j;
	BattleUnit* unit;
	PouchData* pouch;
	BattleUnit* mario;
	EventEntry* evt;
	BOOL loop;
	s32 id;
	s32 state;
	BOOL end;
	int i;

	do {
		loop = FALSE;
		switch (BattleGetSeq(wp, SEQ_MOVE)) {
			case 0x5000000:
				btlseqPhaseFirstProcess(wp);
				BattleIncSeq(wp, SEQ_MOVE);
				loop = TRUE;
				break;
			case 0x5000001:
				BattleIncSeq(wp, SEQ_MOVE);
				loop = TRUE;
				break;
			case 0x5000002:
				pouch = pouchGetPtr();
				mario = BattleGetMarioPtr(wp);
				if (pouch->mMerleeNextCurseType != 2 || BattleGetSeq(wp, SEQ_PHASE) != 0x4000004) {
					BattleIncSeq(wp, SEQ_MOVE);
					BattleIncSeq(wp, SEQ_MOVE);
					loop = TRUE;
					break;
				}
				wp->majinaiState = 2;
				BattleMajinaiDone();
				BattleMajinaiEndCheck();
				evt = evtEntry(marioAttackEvent_MajinaiDefenceUp, 10, 0);
				evt->unitId = mario->unitId;
				mario->attackEventId = evt->eventId;
				BattleIncSeq(wp, SEQ_MOVE);
			case 0x5000003:
				if (!BattleWaitAllActiveEvtEnd(wp)) {
					break;
				}
				BattleIncSeq(wp, SEQ_MOVE);
				loop = TRUE;
				break;
			case 0x5000004:
				for (j = 0; j < 64; j++) {
					unit = BattleGetUnitPtr(wp, j);
					if (unit && unit->unisonPhaseEventCode) {
						BattleRunPhaseEvent(unit, TRUE);
					}
				}
				BattleIncSeq(wp, SEQ_MOVE);
				loop = TRUE;
				break;
			case 0x5000005:
				if (!BattleWaitAllActiveEvtEnd(wp)) {
					break;
				}
				BattleIncSeq(wp, SEQ_MOVE);
				loop = TRUE;
				break;
			case 0x5000006:
				for (i = 0; i < 64; i++) {
					if (wp->phaseEvtTable[i].unitId != -1) {
						unit = BattleGetUnitPtr(wp, wp->phaseEvtTable[i].unitId);
						if (unit && unit->phaseEventCode) {
							break;
						}
					}
				}
				if (i < 64) {
					BattleRunPhaseEvent(unit, FALSE);
					wp->phaseEvtTable[i].unitId = -1;
					BattleIncSeq(wp, SEQ_MOVE);
				}
				else {
					BattleSetSeq(wp, SEQ_MOVE, 0x500000A);
					loop = TRUE;
				}
				break;
			case 0x5000007:
				if (!BattleWaitAllActiveEvtEnd(wp)) {
					break;
				}
				if (BattleCheckConcluded(wp)) {
					BattleSetSeq(wp, SEQ_UNKNOWN, 3);
					BattleSetSeq(wp, SEQ_END, 0x7000000);
				}
				else {
					BattleSetSeq(wp, SEQ_MOVE, 0x5000006);
					loop = TRUE;
				}
				break;
			case 0x5000008:
				memset(&wp->impendingBonuses, 0, sizeof(wp->impendingBonuses));
				for (i = 0; i < 64; i++) {
					unit = BattleGetUnitPtr(wp, i);
					if (unit) {
						unit->mTokenFlags &= 0x80002000;
						unit->totalHpDamageTaken = 0;
						unit->hpDamageTaken = 0;
						unit->hpDamageCount = 0;
						FIELD(unit, s8, 0x26F) = 0;
						FIELD(unit, s32, 0x264) = 0;
						unit->fpDamageTaken = 0;
						FIELD(unit, s32, 0x268) = 0;
						FIELD(unit, s8, 0x26E) = 0;
						FIELD(unit, s32, 0x27C) = 0;
					}
				}
				for (i = 0; i < 64; i++) {
					id = wp->movePriorityQueue[i];
					if (id != -1) {
						unit = BattleGetUnitPtr(wp, id);
						if (unit && unit->movesRemaining > 0) {
							state = unit->moveState;
							if (state != 10 && (!BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) || (unit->mAttributeFlags & 0x10000))) {
								if (state != 1) {
									break;
								}
								unit->moveState = 0;
							}
						}
					}
				}
				wp->activeUnitId = id;
				BattleGetUnitPtr(wp, id)->mTokenFlags |= 0x1000;
				BattleIncSeq(wp, SEQ_MOVE);
				BattleSetSeq(wp, SEQ_ACT, 0x6000000);
				for (i = 0; i < 64; i++) {
					id = wp->movePriorityQueue[i];
					if (id != -1) {
						unit = BattleGetUnitPtr(wp, id);
						if (unit && unit->moveState == 1) {
							unit->moveState = 0;
						}
					}
				}
				unit = BattleGetUnitPtr(wp, wp->activeUnitId);
				if (unit->currentKind == UNIT_MARIO) {
					pouch = pouchGetPtr();
					wp->flags |= 0x600;
					for (i = 0; i < 200; i++) {
						if (pouch->equippedBadges[i] >= 0xF0 && pouch->equippedBadges[i] < 0xF7) {
							wp->flags &= ~0x200;
							break;
						}
					}
					if (pouch->jumpLevel >= 2) {
						wp->flags &= ~0x200;
					}
					for (i = 0; i < 200; i++) {
						if (pouch->equippedBadges[i] >= 0xF7 && pouch->equippedBadges[i] < 0xFE) {
							wp->flags &= ~0x400;
							break;
						}
					}
					if (pouch->hammerLevel >= 2) {
						wp->flags &= ~0x400;
					}
				}
				break;
			case 0x5000009:
				btlseqAct(wp);
				break;
			case 0x500000A:
				if (BattleCheckConcluded(wp)) {
					BattleSetSeq(wp, SEQ_UNKNOWN, 3);
					BattleSetSeq(wp, SEQ_END, 0x7000000);
					break;
				}
				for (i = 0; i < 64; i++) {
					unit = BattleGetUnitPtr(wp, i);
					if (unit) {
						BtlUnit_SetTotalHitDamage(unit, 0);
						unit->mTokenFlags &= ~0x1000;
					}
				}
				end = BattlePhaseEndCheck();
				if (wp->flags & 0x2000) {
					end = TRUE;
				}
				if (end) {
					if (wp->majinaiState == 2) {
						wp->majinaiState = 0;
					}
					BattleAudience_PerPhase(BattleGetSeq(wp, SEQ_PHASE));
					BattleIncSeq(wp, SEQ_MOVE);
				}
				else {
					BattleSetSeq(wp, SEQ_MOVE, 0x5000008);
				}
				break;
			case 0x500000B:
				if ((u8)BattleAudience_CheckReactionPerPhase()) {
					break;
				}
				if (!BattleWaitAllActiveEvtEnd(wp)) {
					break;
				}
				BattleIncSeq(wp, SEQ_MOVE);
				break;
			case 0x500000C:
				if (BattleStatusChangeAnnouceMain(wp) | BattleStatusChangeMsgMain(wp)) {
					break;
				}
				BattleIncSeq(wp, SEQ_PHASE);
				BattleSetSeq(wp, SEQ_MOVE, 0x5000000);
				break;
			case 0x500000D:
				BattleIncSeq(wp, SEQ_MOVE);
				break;
			case 0x500000E:
				BattleIncSeq(wp, SEQ_PHASE);
				BattleSetSeq(wp, SEQ_MOVE, 0x5000000);
				break;
		}
	} while (loop);
}


BOOL BattlePhaseEndCheck(void) {
	BattleWork* wp = _battleWorkPointer;
	BattleUnit* unit;
	int i;

	for (i = 0; i < 64; i++) {
		if (wp->movePriorityQueue[i] != -1) {
			unit = BattleGetUnitPtr(wp, wp->movePriorityQueue[i]);
			if (unit && unit->moveState != 10 && unit->movesRemaining > 0 &&
				!BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
				break;
			}
		}
	}
	if (i >= 64) {
		return TRUE;
	}
	return FALSE;
}

void btlseqAct(BattleWork* wp) {
	BattleUnit* unit;
	EventEntry* evt;
	BOOL cantMove;
	BOOL decrement;

	switch (BattleGetSeq(wp, SEQ_ACT)) {
		case 0x6000000: {
			BattleUnit* unit;
			BOOL cantMove;

			unit = BattleGetUnitPtr(wp, wp->activeUnitId);
			effNiceAsync(1);
			FIELD(unit, s32, 0x280) = 0;
			FIELD(unit, s32, 0x2CC) = 0;
			BattleAudience_ActInit();
			cantMove = TRUE;
			switch (unit->currentKind) {
				case UNIT_MARIO:
					if (!BtlUnit_CheckStatus(unit, 1) &&
						!BtlUnit_CheckStatus(unit, 2) &&
						!BtlUnit_CheckStatus(unit, 9) &&
						!BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
						cantMove = FALSE;
					}
					break;
				default:
					if (unit->currentKind >= TYPE_PARTNER_MIN && unit->currentKind < TYPE_PARTNER_MAX) {
						if (!BtlUnit_CheckStatus(unit, 1) &&
							!BtlUnit_CheckStatus(unit, 2) &&
							!BtlUnit_CheckStatus(unit, 9) &&
							!BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
							cantMove = FALSE;
						}
					}
					break;
			}
			if (cantMove) {
				BattleSetSeq(wp, SEQ_ACT, 0x6000003);
			}
			else if (unit->mAttributeFlags & 0x10) {
				BattleSetSeq(wp, SEQ_ACT, 0x6000004);
			}
			else if (unit->currentKind == UNIT_VIVIAN && (BattleGetMarioPtr(wp)->mAttributeFlags & 0x10)) {
				BattleSetSeq(wp, SEQ_ACT, 0x6000004);
			}
			else if (unit->flippedTurns) {
				BattleSetSeq(wp, SEQ_ACT, 0x6000005);
			}
			else {
				BattleCommandInit(wp);
				BattleSetSeq(wp, SEQ_ACT, 0x6000001);
			}
			break;
		}
		case 0x6000001:
			if (battleCameraGetPosMoveSpeed() > 2.0f) {
				break;
			}
			BattleSetSeq(wp, SEQ_ACT, 0x6000002);
		case 0x6000002:
			if (!BattleCommandInput(wp)) {
				break;
			}
			wp->flags &= ~0x80;
			BattleSetSeq(wp, SEQ_ACT, 0x6000006);
			break;
		case 0x6000005:
			unit = BattleGetUnitPtr(wp, wp->activeUnitId);
			FIELD(unit, void*, 0x29C) = BtlUnit_GetData(unit, 2);
			FIELD(unit, s32, 0x2A8) = 1;
			BattleSetSeq(wp, SEQ_ACT, 0x6000006);
			break;
		case 0x6000003:
			unit = BattleGetUnitPtr(wp, wp->activeUnitId);
			FIELD(unit, s32, 0x2A8) = 1;
			unit->flags |= 0x4000000;
			BattleSetSeq(wp, SEQ_ACT, 0x6000006);
			break;
		case 0x6000004:
			unit = BattleGetUnitPtr(wp, wp->activeUnitId);
			FIELD(unit, void*, 0x29C) = BtlUnit_GetData(unit, 3);
			FIELD(unit, s32, 0x2A8) = 0x1B;
			BattleSetSeq(wp, SEQ_ACT, 0x6000006);
			break;
		case 0x6000006:
			unit = BattleGetUnitPtr(wp, wp->activeUnitId);
			if (!(FIELD(unit, s32, 0x2A8) & 1)) {
				BattleStatusWindowSystemOff();
			}
			unit = BattleGetUnitPtr(wp, wp->activeUnitId);
			evt = NULL;
			if ((wp->flags & 0x40000000) && !(unit->mAttributeFlags & 0x2000) &&
				!(FIELD(unit, s32, 0x2A8) & 8) && !(FIELD(unit, s32, 0x2A8) & 0x10)) {
				evt = evtEntry(btldefaultevt_CantMoveZeroGravity, 10, 0);
			}
			else if (FIELD(unit, s32, 0x2A8) & 0x10) {
				if (FIELD(unit, void*, 0x29C)) {
					evt = evtEntry(FIELD(unit, void*, 0x29C), 10, 0);
				}
			}
			else if (FIELD(unit, s32, 0x2A8) & 2) {
				evt = evtEntry(FIELD(unit, void*, 0x29C), 10, 0);
			}
			else if (!BtlUnit_CheckStatus(unit, 1) &&
				!BtlUnit_CheckStatus(unit, 2) &&
				!BtlUnit_CheckStatus(unit, 9) &&
				(!BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) || (unit->mAttributeFlags & 0x10000))) {
				if (BtlUnit_CheckStatus(unit, 6) && irand(100) < 0) {
					evt = evtEntry(btldefaultevt_BiribiriMove, 10, 0);
				}
				else if (BtlUnit_CheckStatus(unit, 5) && irand(100) < 50) {
					unit->mTokenFlags |= 0x10;
					evt = evtEntry(FIELD(unit, void*, 0x2A0), 10, 0);
				}
				else {
					evt = evtEntry(FIELD(unit, void*, 0x29C), 10, 0);
				}
			}
			if (evt) {
				unit->attackEventId = evt->eventId;
				evt->unitId = unit->unitId;
				if (unit->waitEventId) {
					evtDeleteID(unit->waitEventId);
					unit->waitEventId = 0;
				}
			}
			BattleIncSeq(wp, SEQ_ACT);
			break;
		case 0x6000007:
			if (!BattleWaitAllActiveEvtEnd(wp)) {
				break;
			}
			BattleIncSeq(wp, SEQ_ACT);
			BattleStatusChangeMsgAdjust(wp);
		case 0x6000008:
			if (!BattleWaitAllActiveEvtEnd(wp)) {
				break;
			}
			if (BattleStatusChangeAnnouceMain(wp) | BattleStatusChangeMsgMain(wp)) {
				break;
			}
			BattleIncSeq(wp, SEQ_ACT);
		case 0x6000009:
			if (!(wp->flags & 0x30)) {
				BattleStatusWindowSystemOn();
			}
			unit = BattleGetUnitPtr(wp, wp->activeUnitId);
			if (unit) {
				unit->flags &= ~0x4000000;
				if (unit->mTokenFlags & 8) {
					BtlUnit_SetStatus(unit, 0x10, 1, -99);
					unit->mTokenFlags &= ~8;
				}
				if (wp->majinaiState == 1) {
					wp->majinaiState = 0;
				}
				FIELD(unit, s32, 0x258) = 0;
				switch (unit->moveState) {
					case 5:
						unit->moveState = 6;
						break;
					case 6:
						unit->moveState = 7;
						break;
					case 8:
						unit->moveState = 9;
						break;
					case 7:
					case 9:
						unit->moveState = 0;
						break;
				}
				if (unit->moveState == 0) {
					decrement = TRUE;
					if (wp->flags & 0x2000) {
						decrement = TRUE;
					}
					if (decrement) {
						if (BtlUnit_CheckStatusFlag(unit, 0x2000000)) {
							BtlUnit_OffStatusFlag(unit, 0x2000000);
						}
						else {
							unit->movesRemaining--;
						}
						if (wp->flags & 0x2000) {
							unit->movesRemaining = 0;
						}
						if (unit->movesRemaining <= 0) {
							unit->moveState = 10;
							if (!(unit->mTokenFlags & 0x2000)) {
								unit->activeTurns = 0;
							}
							else {
								unit->mTokenFlags &= ~0x2000;
							}
							if (!BtlUnit_GetBelong(unit) && unit->attackPhase == 0x4000002 &&
								!BattlePhaseEndCheck() && !(wp->flags & 0x2000)) {
								unit->mMoveColorLv = 0x60;
							}
						}
					}
				}
				else if (unit->moveState == 3) {
					unit->moveState = 0;
				}
				else if (unit->moveState == 4) {
					unit->moveState = 0;
				}
			}
			BattleConsumeReserveItem();
			wp->flags &= ~4;
			BattleAudience_PerAct();
			BattleIncSeq(wp, SEQ_ACT);
		case 0x600000A:
			if ((u8)BattleAudience_CheckReaction()) {
				break;
			}
			if (!BattleCheckConcluded(wp) && (u8)BattleBreakSlot_CheckReaction()) {
				break;
			}
			BattleIncSeq(wp, SEQ_ACT);
		case 0x600000B:
			if (BattleStatusChangeAnnouceMain(wp) | BattleStatusChangeMsgMain(wp)) {
				break;
			}
			BattleIncSeq(wp, SEQ_ACT);
		case 0x600000C:
			if (!BattleWaitAllActiveEvtEnd(wp)) {
				break;
			}
			unit = BattleGetUnitPtr(wp, wp->activeUnitId);
			if (unit) {
				FIELD(unit, s32, 0x2C8) = 0;
				if (FIELD(unit, void*, 0x2C4)) {
					evt = evtEntry(FIELD(unit, void*, 0x2C4), 10, 0);
					if (evt) {
						FIELD(unit, s32, 0x2C8) = evt->eventId;
						evt->unitId = unit->unitId;
						if (unit->waitEventId) {
							evtDeleteID(unit->waitEventId);
							unit->waitEventId = 0;
						}
					}
				}
			}
			BattleIncSeq(wp, SEQ_ACT);
		case 0x600000D:
			unit = BattleGetUnitPtr(wp, wp->activeUnitId);
			if (unit && FIELD(unit, s32, 0x2C8)) {
				if (evtCheckID(FIELD(unit, s32, 0x2C8))) {
					break;
				}
				FIELD(unit, s32, 0x2C8) = 0;
			}
			BattleIncSeq(wp, SEQ_ACT);
		case 0x600000E:
			if (!BattleWaitAllActiveEvtEnd(wp)) {
				break;
			}
			BattleIncSeq(wp, SEQ_MOVE);
			break;
	}
}


void btlseqTurnFirstProcess(BattleWork* wp) {
	BattleUnitKind* kind;
	int i;
	BattleUnit* unit;

	BtlActRec_JudgeTurnRuleKeep();
	wp->turnCount++;
	if (wp->turnCount >= 2) {
		BattleMajinaiCheck();
	}
	wp->announce.field_36C = 0;
	wp->announce.field_368 = 0;
	BtlActRec_AddCount(&wp->field_16F3B);
	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(wp, i);
		if (unit) {
			unit->activeTurns++;
			BtlUnit_ResetMoveStatus(unit);
			kind = unit->mKindParams;
			unit->movePriority = kind->turnOrder + irand(kind->turnOrderVariance) - kind->turnOrderVariance / 2;
			if (BtlUnit_CheckStatusFlag(unit, 0x1000000)) {
				BtlUnit_OffStatusFlag(unit, 0x1000000);
			}
		}
	}
	BattleFogEndCheck();
}

void btlseqPhaseFirstProcess(BattleWork* wp) {
	BattleUnit* unit;
	BattleUnitPart* part;
	s32 count;
	int i;

	FIELD(wp, s32, 0x1C70) = 0;
	FIELD(wp, u8, 0x1C74) = 0;
	for (i = 0; i < 64; i++) {
		wp->movePriorityQueue[i] = -1;
	}
	count = 0;
	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(wp, i);
		if (unit) {
			unit->mMoveColorLv = 0xFF;
			if (unit->moveState == 10 && !BtlUnit_CheckStatus(unit, 0x14)) {
				continue;
			}
			if (unit->attackPhase == BattleGetSeq(wp, SEQ_PHASE) &&
				!(unit->mAttributeFlags & 0x80000)) {
				wp->movePriorityQueue[count] = unit->unitId;
				unit->protectId = -1;
				for (part = unit->mParts; part; part = part->mNextPart) {
					if (part->poseId != -1) {
						FIELD(part, u8, 0x4E4) = 1;
					}
				}
				count++;
			}
		}
	}
	battleSortPhaseMoveTable(wp, count);
	battleMakePhaseEvtTable(wp);
}

void battleSortPhaseMoveTable(BattleWork* wp, s32 count) {
	int i, j;
	BattleUnit* unit1;
	BattleUnit* unit2;
	f32 x1, y1, z1;
	f32 x2, y2, z2;
	s32 tmp;
	void* w;

	w = (void*)wp;
	if (count <= 0) {
		count = 0;
		for (i = 0; i < 64; i++) {
			if (((BattleWork*)w)->movePriorityQueue[i] == -1) {
				break;
			}
			count = count + 1;
		}
	}
	for (i = 0; i < count - 1; i++) {
		for (j = 0; j < count - i - 1; j++) {
			unit1 = BattleGetUnitPtr(((BattleWork*)w), ((BattleWork*)w)->movePriorityQueue[j]);
			unit2 = BattleGetUnitPtr(((BattleWork*)w), ((BattleWork*)w)->movePriorityQueue[j + 1]);
			BtlUnit_GetHomePos(unit1, &x1, &y1, &z1);
			BtlUnit_GetHomePos(unit2, &x2, &y2, &z2);
			if (__fabs(x1) > __fabs(x2)) {
				tmp = ((BattleWork*)w)->movePriorityQueue[j];
				((BattleWork*)w)->movePriorityQueue[j] = ((BattleWork*)w)->movePriorityQueue[j + 1];
				((BattleWork*)w)->movePriorityQueue[j + 1] = tmp;
			}
		}
	}
	for (i = 0; i < count - 1; i++) {
		for (j = 0; j < count - i - 1; j++) {
			unit1 = BattleGetUnitPtr(((BattleWork*)w), ((BattleWork*)w)->movePriorityQueue[j]);
			unit2 = BattleGetUnitPtr(((BattleWork*)w), ((BattleWork*)w)->movePriorityQueue[j + 1]);
			if (unit1->movePriority < unit2->movePriority) {
				tmp = ((BattleWork*)w)->movePriorityQueue[j];
				((BattleWork*)w)->movePriorityQueue[j] = ((BattleWork*)w)->movePriorityQueue[j + 1];
				((BattleWork*)w)->movePriorityQueue[j + 1] = tmp;
			}
		}
	}
}

void battleMakePhaseEvtTable(BattleWork* wp) {
	BattlePhaseEvtEntry* table;
	int i;
	BattlePhaseEvtEntry* entry;
	BattleUnit* unit;
	BattlePhaseEvtEntry tmp;
	f32 x, y, z;
	s32 count;
	int j;

	table = wp->phaseEvtTable;
	for (i = 0; i < 64; i++) {
		table[i].unitId = -1;
		table[i].priority = 0;
	}
	count = 0;
	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(wp, i);
		if (unit && !(unit->mAttributeFlags & 0x80000)) {
			table[count].unitId = i;
			BtlUnit_GetHomePos(unit, &x, &y, &z);
			if (x >= 0.0f) {
				table[count].priority = 1000.0f + x;
			}
			else {
				table[count].priority = __fabs(x);
			}
			count++;
		}
	}
	for (i = 0; i < count - 1; i++) {
		for (j = 0; j < count - i - 1; j++) {
			if (table[j].priority > table[j + 1].priority) {
				tmp = table[j];
				if (!i && !i && !i) {}
				if (!i) {}
				table[j] = table[j + 1];
				table[j + 1] = tmp;
			}
		}
	}
}

BOOL BattleWaitAllActiveEvtEnd(BattleWork* wp) {
	BattleSeqStageWork* stage = STAGE_WORK(wp);
	BOOL ret = BattleWaitAllActiveEvtEnd_NoBgSetEndWait(wp);

	if (!ret) {
		return ret;
	}
	if (stage->nozzleWorkEvtId) {
		if (evtCheckID(stage->nozzleWorkEvtId)) {
			return FALSE;
		}
		stage->nozzleWorkEvtId = 0;
	}
	if (stage->fallFrameEvtId) {
		if (evtCheckID(stage->fallFrameEvtId)) {
			return FALSE;
		}
		stage->fallFrameEvtId = 0;
	}
	if (stage->bgDestroyEvtId) {
		if (evtCheckID(stage->bgDestroyEvtId)) {
			return FALSE;
		}
		stage->bgDestroyEvtId = 0;
	}
	if (stage->objectFallEvtId) {
		if (evtCheckID(stage->objectFallEvtId)) {
			return FALSE;
		}
		stage->objectFallEvtId = 0;
	}
	if (stage->closeWallEvtId) {
		if (evtCheckID(stage->closeWallEvtId)) {
			return FALSE;
		}
		stage->closeWallEvtId = 0;
	}
	return TRUE;
}

BOOL BattleWaitAllActiveEvtEnd_NoBgSetEndWait(BattleWork* wp) {
	void* w; BattleUnit* unit; BOOL done; int i;
	done = TRUE;
	w = (void*)wp;
	for (i = 0; i < 0x40; i++) {
		unit = BattleGetUnitPtr((BattleWork*)w, i);
		if (unit) {
			while (unit->phaseEventId) {
				if (evtCheckID(unit->phaseEventId)) {
					done = FALSE;
					break;
				}
				unit->phaseEventId = 0;
				break;
			}
			while (unit->attackEventId) {
				if (evtCheckID(unit->attackEventId)) {
					done = FALSE;
					break;
				}
				unit->attackEventId = 0;
				break;
			}
			while (unit->damageEventId) {
				if (evtCheckID(unit->damageEventId)) {
					done = FALSE;
					break;
				}
				unit->damageEventId = 0;
				break;
			}
			while (FIELD(unit, s32, 0x2B8)) {
				if (evtCheckID(FIELD(unit, s32, 0x2B8))) {
					done = FALSE;
					break;
				}
				FIELD(unit, s32, 0x2B8) = 0;
				break;
			}
		}
	}
	if (!done) {
		return done;
	}
	if (BattleAfterReactionMain()) {
		done = FALSE;
	}
	return done;
}

BOOL BattleCheckConcluded(BattleWork* wp) {
	BattleUnit* unit;
	s32 count;
	s32 i;
	s32 j;

	count = 0;
	for (i = 0; i < 3; i++) {
		wp->alliances[i].clearConditionMet = 0;
		if (wp->alliances[i].allianceId == 0) {
			wp->alliances[i].clearConditionMet = 1;
			count++;
			continue;
		}
		if (wp->alliances[i].allianceId & 1) {
			for (j = 0; j < 64; j++) {
				unit = BattleGetUnitPtr(wp, j);
				if (unit && unit->mAlliance == i &&
					!BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) &&
					!(unit->mAttributeFlags & 0x20000) &&
					!(unit->mAttributeFlags & 0x10000000)) {
					break;
				}
			}
			if (j >= 64) {
				wp->alliances[i].clearConditionMet = 1;
				count++;
				continue;
			}
		}
		if (wp->alliances[i].allianceId & 2) {
			for (j = 0; j < 64; j++) {
				unit = BattleGetUnitPtr(wp, j);
				if (unit && unit->mAlliance == i &&
					(unit->mAttributeFlags & 0x40000) &&
					!BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) &&
					!(unit->mAttributeFlags & 0x10000000)) {
					break;
				}
			}
			if (j >= 64) {
				wp->alliances[i].clearConditionMet = 1;
				count++;
			}
		}
	}
	if (count >= 2) {
		return TRUE;
	}
	return (wp->flags & 0x30) != 0;
}

void _mapobj_data_touch_scale(s32 layer) {
	BattleWork* wp = _battleWorkPointer;
	BattleStageObject* obj;
	int i;

	for (i = 0; i < 32; i++) {
		obj = &wp->stageObjects[i];
		if (obj->id > 0 && obj->data->mLayer == layer) {
			obj->mFlags |= 2;
		}
	}
}

#pragma dont_inline on
USER_FUNC(_set_haikei_entry_scale) {
	BattleWork* wp = _battleWorkPointer;
	BOOL busy = FALSE;
	f32 sx, sy;
	BattleStageObject* obj;
	int i;

	if (isFirstCall) {
		event->userdata[0] = 60;
		event->userdata[1] = 0;
		if (mapGetMapObj("A1")) {
			mapObjScale("A1", 0.0f, 0.0f, 1.0f);
		}
		if (mapGetMapObj("A2")) {
			mapObjScale("A2", 0.0f, 0.0f, 1.0f);
		}
	}
	if (event->userdata[0] > 0) {
		event->userdata[0]--;
		if (event->userdata[0] > 0) {
			if (mapGetMapObj("A1")) {
				mapObjScale("A1", 0.0f, 0.0f, 1.0f);
				_mapobj_data_touch_scale(0);
			}
			if (mapGetMapObj("A2")) {
				mapObjScale("A2", 0.0f, 0.0f, 1.0f);
				_mapobj_data_touch_scale(1);
			}
			return EVT_RETURN_BLOCK;
		}
	}
	event->userdata[1]++;
	if (mapGetMapObj("A1")) {
		if (event->userdata[1] >= 40) {
			mapObjScale("A1", 1.0f, 1.0f, 1.0f);
			_mapobj_data_touch_scale(0);
		}
		else {
			busy = TRUE;
			if (event->userdata[1] < 15) {
				mapObjScale("A1", 0.0f, 0.0f, 1.0f);
				_mapobj_data_touch_scale(0);
			}
			else {
				mapObjScale("A1", 1.0f, 1.0f, 1.0f);
				_mapobj_data_touch_scale(0);
				if (event->userdata[1] < 25) {
					sy = intpl_sub(5, 0.0f, 110.0f, event->userdata[1] - 15, 10);
				}
				else if (event->userdata[1] < 35) {
					sy = intpl_sub(11, 110.0f, 90.0f, event->userdata[1] - 25, 10);
				}
				else if (event->userdata[1] < 40) {
					sy = intpl_sub(11, 90.0f, 100.0f, event->userdata[1] - 35, 5);
				}
				if (event->userdata[1] < 25) {
					sx = intpl_sub(5, 100.0f, 90.0f, event->userdata[1] - 15, 10);
				}
				else if (event->userdata[1] < 35) {
					sx = intpl_sub(11, 90.0f, 110.0f, event->userdata[1] - 25, 10);
				}
				else if (event->userdata[1] < 40) {
					sx = intpl_sub(11, 110.0f, 100.0f, event->userdata[1] - 35, 5);
				}
				for (i = 0; i < 32; i++) {
					obj = &wp->stageObjects[i];
					if (obj->id > 0 && obj->data->mLayer == 0) {
						mapObjScale(obj->data->name, sx / 100.0f, sy / 100.0f, 1.0f);
						obj->mFlags |= 2;
					}
				}
			}
		}
	}
	if (mapGetMapObj("A2")) {
		if (event->userdata[1] >= 45) {
			mapObjScale("A2", 1.0f, 1.0f, 1.0f);
			_mapobj_data_touch_scale(1);
		}
		else {
			busy = TRUE;
			if (event->userdata[1] < 20) {
				mapObjScale("A2", 0.0f, 0.0f, 1.0f);
				_mapobj_data_touch_scale(1);
			}
			else {
				mapObjScale("A2", 1.0f, 1.0f, 1.0f);
				_mapobj_data_touch_scale(1);
				if (event->userdata[1] < 30) {
					sy = intpl_sub(5, 0.0f, 110.0f, event->userdata[1] - 20, 10);
				}
				else if (event->userdata[1] < 40) {
					sy = intpl_sub(11, 110.0f, 90.0f, event->userdata[1] - 30, 10);
				}
				else if (event->userdata[1] < 45) {
					sy = intpl_sub(11, 90.0f, 100.0f, event->userdata[1] - 40, 5);
				}
				if (event->userdata[1] < 30) {
					sx = intpl_sub(5, 100.0f, 90.0f, event->userdata[1] - 20, 10);
				}
				else if (event->userdata[1] < 40) {
					sx = intpl_sub(11, 90.0f, 110.0f, event->userdata[1] - 30, 10);
				}
				else if (event->userdata[1] < 45) {
					sx = intpl_sub(11, 110.0f, 100.0f, event->userdata[1] - 40, 5);
				}
				for (i = 0; i < 32; i++) {
					obj = &wp->stageObjects[i];
					if (obj->id > 0 && obj->data->mLayer == 1) {
						mapObjScale(obj->data->name, sx / 100.0f, sy / 100.0f, 1.0f);
						obj->mFlags |= 2;
					}
				}
			}
		}
	}
	return busy ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}


