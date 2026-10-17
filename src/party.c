#include "party.h"
#include "driver/animdrv.h"
#include "mario/mario_pouch.h"
#include "mario/mario_party.h"
#include "mario/mario_cam.h"
#include "memory.h"
#include "system.h"
#include "mario/mariost.h"
#include <string.h>

//.data
typedef void (*PartyFunc)(PartyEntry* party);

typedef struct PartyData {
    char* name;            // 0x0
    PartyFunc init_func;   // 0x4
    PartyFunc move_func;   // 0x8
    PartyFunc use_func;    // 0xC
    PartyFunc post_func;   // 0x10
    PartyFunc bye_func;    // 0x14
    PartyFunc unk_6_func;  // 0x18
    PartyFunc unk_7_func;  // 0x1C
    PartyFunc unk_8_func;  // 0x20
    PartyFunc exit_func;   // 0x24
} PartyData;

extern PartyData partyDataTbl[];

extern char* yoshigroup[7];

extern char* partyPoseTbl[20][15];
extern char* partyDotPoseTbl[8][15];
extern char* yoshiDotPoseTbl[7][15];

/*
    000b95ec 000020 800bebcc  4 partyGetPtr 	party.o
    000b95c8 000024 800beba8  4 anotherPartyGetPtr 	party.o
    000b94a8 000120 800bea88  4 partyPaperOn 	party.o
    000b942c 00007c 800bea0c  4 partyPaperOff 	party.o
    000b9404 000028 800be9e4  4 partyPaperLightOff 	party.o
    UNUSED   000028 ........ partyPaperLightOn party.o
    UNUSED   000078 ........ setAnimPoseMaterialFlag party.o
    UNUSED   000040 ........ partyPaperRelease party.o
    000b92a0 000164 800be880  4 partyChgPoseId 	party.o
    000b9224 00007c 800be804  4 partyChgPose 	party.o
    000b91b4 000070 800be794  4 partyChgPaper 	party.o
    UNUSED   000078 ........ partyChgPaperTime party.o
    000b917c 000038 800be75c  4 partyCtrlOff 	party.o
    000b9144 000038 800be724  4 partyCtrlOn 	party.o
    000b8f2c 000218 800be50c  4 partyRun 	party.o
    000b8d1c 000210 800be2fc  4 partyStop 	party.o
    000b8af4 000228 800be0d4  4 partySetForceMove 	party.o
    UNUSED   00006c ........ partyGetNum party.o
    UNUSED   00007c ........ getPartyEntryId party.o
    UNUSED   00005c ........ getExtraPartyEntryId party.o
    UNUSED   000008 ........ partyCheck party.o
    UNUSED   000238 ........ partyHello_debug party.o
    000b88bc 000238 800bde9c  4 partyEntryHello 	party.o
    000b8704 0001b8 800bdce4  4 partyEntry 	party.o
    000b84e8 00021c 800bdac8  4 partyEntryPos 	party.o
    000b82b0 000238 800bd890  4 partyGoodbye 	party.o
    000b815c 000154 800bd73c  4 partyKill 	party.o
    000b8060 0000fc 800bd640  4 partyKill2 	party.o
    UNUSED   000080 ........ partyKill_debug party.o
    UNUSED   0000b8 ........ partyPoseEntry party.o
    000b7fc0 0000a0 800bd5a0  4 yoshiSetColor 	party.o
    000b7f90 000030 800bd570  4 partyGetHeight 	party.o
    000b7c0c 000384 800bd1ec  4 partyEntryMain 	party.o
    000b7894 000378 800bce74  4 partyEntry2 	party.o
    000b739c 0004f8 800bc97c  4 partyEntry2Hello 	party.o
    000b6e0c 000590 800bc3ec  4 partyEntry2Pos 	party.o
    000b6de0 00002c 800bc3c0  4 partyInit 	party.o
    000b6214 000bcc 800bb7f4  4 partyReInit 	party.o
    000b61b8 00005c 800bb798  4 partyUsePost 	party.o
    000b5fa0 000218 800bb580  4 partyChgRunMode 	party.o
    UNUSED   000208 ........ partyChgRunMode2 party.o
    000b5f0c 000094 800bb4ec  4 partyPrevRunMode 	party.o
    000b5e28 0000e4 800bb408  4 partyMain 	party.o
    000b5444 0009e4 800baa24  4 driveParty 	party.o
    000b51d0 000274 800ba7b0  4 partyDoWork 	party.o
    UNUSED   0000c4 ........ partyNormalie party.o
    000b503c 000194 800ba61c  4 partyHello 	party.o
    UNUSED   000158 ........ partyHelloInit party.o
    000b4990 0006ac 800b9f70  4 partyHelloMain 	party.o
    UNUSED   000190 ........ party_force_hello party.o
    000b48c4 0000cc 800b9ea4  4 partyGoodbyeInit 	party.o
    000b4014 0008b0 800b95f4  4 partyGoodbyeMain 	party.o
    UNUSED   000054 ........ party_dokan party.o
    000b3a6c 0005a8 800b904c  4 partyDokanInit 	party.o
    000b350c 000560 800b8aec  4 partyDokanMain 	party.o
    000b34a0 00006c 800b8a80  4 partyDokanEnd 	party.o
    UNUSED   000014 ........ partyGetRideScale party.o
    000b3068 000438 800b8648  4 allPartyRidePlane 	party.o
    000b2c30 000438 800b8210  4 allPartyRideShip 	party.o
    000b27f8 000438 800b7dd8  4 allPartyRideJabara 	party.o
    UNUSED   000094 ........ allPartyJabaraEscape party.o
    UNUSED   000358 ........ allPartyRideShip2 party.o
    000b25d8 000220 800b7bb8  4 getRidePos 	party.o
    UNUSED   00018c ........ rideInit party.o
    000b2174 000464 800b7754  4 allPartyForceRideOn 	party.o
    UNUSED   000434 ........ allPartyRideOn party.o
    000b20e0 000094 800b76c0  4 allPartyRideOff 	party.o
    000b204c 000094 800b762c  4 allPartyRideOff2 	party.o
    UNUSED   000018 ........ partyRideOff party.o
    UNUSED   000018 ........ partyRideOff2 party.o
    000b2010 00003c 800b75f0  4 partyRideChk 	party.o
    UNUSED   0000f4 ........ allPartyRideChk party.o
    UNUSED   000048 ........ party_ride party.o
    UNUSED   00000c ........ partyRideInit party.o
    000b111c 000ef4 800b66fc  4 partyRideMain 	party.o
    UNUSED   00003c ........ chkMapAttr party.o
    000b01c8 000f54 800b57a8  4 partyGetAppearPos 	party.o
    000afc44 000584 800b5224  4 partyGetAppearPos2 	party.o
    000af908 00033c 800b4ee8  4 partyGetAppearPos3 	party.o
    000af3e8 000520 800b49c8  4 partyGetAppearPos4 	party.o
    UNUSED   000214 ........ partyGetFrontPos party.o
    000af07c 00036c 800b465c  4 party_force_ride_yoshi 	party.o
    000aeff4 000088 800b45d4  4 party_ride_yoshi_force_move 	party.o
    UNUSED   000048 ........ party_ride_yoshi party.o
    000ae96c 000688 800b3f4c  4 partyRideYoshiMain 	party.o
    UNUSED   000024 ........ chkWater party.o
    000ae390 0005dc 800b3970  4 party_damage_return 	party.o
    000ae360 000030 800b3940  4 partyInitCamId 	party.o
    UNUSED   000008 ........ partyGetCamId party.o
    000ae358 000008 800b3938  4 partySetCamId 	party.o
    UNUSED   0000dc ........ partyGetDispDir party.o
    000ae018 000340 800b35f8  4 partyMakeDispDir 	party.o
    000adcf8 000320 800b32d8  4 partyPreDisp 	party.o
    000ad980 000378 800b2f60  4 partyDisp 	party.o
    000ad97c 000004 800b2f5c  4 partyKeyOn 	party.o
    000ad978 000004 800b2f58  4 partyKeyOff 	party.o
    UNUSED   000008 ........ partySetContNo party.o
    UNUSED   00000c ........ partyGetContNo party.o
    UNUSED   000038 ........ partyClearCont party.o
    000ad870 000108 800b2e50  4 partyUpdateKeyData 	party.o
    UNUSED   000018 ........ partyShadowOn party.o
    000ad858 000018 800b2e38  4 partyShadowOff 	party.o
    UNUSED   00004c ........ partyChkHerohero party.o
    000ad464 0003f4 800b2a44  4 partySetFamicomMode 	party.o
    000ad464 00dd4c 800b2a44  1 .text 	party.o
*/

PartyEntry* partyPtrTbl[2];
PartyData* party_dp;
s32 partyCtrlNo;

void partyChgRunMode(PartyEntry* party, s32 mode);
void partyChgMot(PartyEntry* party, s32 mot);
s32 partyEntryMain(s32 memberId);
void partyGetAppearPos2(PartyEntry* party, Vec* pos);

PartyEntry* partyGetPtr(s32 index) {
    if (index >= 0) {
        return partyPtrTbl[index];
    }
    return NULL;
}

PartyEntry* anotherPartyGetPtr(s32 index) {
    if (index < 0) {
        return NULL;
    }
    return *(partyPtrTbl + ((index + 1) & 1));
}

BOOL partyPaperOn(PartyEntry* party, const char* animGroup) {
    BOOL v4;
    BOOL v5;
    if (party->paperPoseId >= 0) {
        animPaperPoseRelease(party->paperPoseId);
        party->paperPoseId = -1;
    }
    party->flags2 |= 0x4000000;
    party->paperPoseId = animPaperPoseEntry(animGroup, 2);
    v4 = (!strcmp(animGroup, "p_roll")
        || !strcmp(animGroup, "p_plane")
        || !strcmp(animGroup, "p_dokan_x")
        || !strcmp(animGroup, "p_dokan_y"));
    v5 = v4 == 0;
    if (!strcmp(animGroup, "p_slit")) {
        v5 = party->currentMemberId == PARTY_MEMBER_FLURRIE;
    }
    animPoseSetPaperAnimGroup(party->paperAnimGroup, animGroup, v5);
    return TRUE;
}

void partyPaperOff(PartyEntry* party) {
    s32 paperPoseId; // r3

    if ((party->flags2 & 0x4000000) && party->field_1C) {
        animPoseSetPaperAnimGroup(party->paperAnimGroup, NULL, FALSE);
        party->field_1C = 0;
        party->flags2 &= ~0x4000000;
        paperPoseId = party->paperPoseId;
        if (paperPoseId >= 0) {
            animPaperPoseRelease(paperPoseId);
            party->paperPoseId = -1;
        }
    }
}

void partyPaperLightOff(PartyEntry* party) {
    animPoseSetMaterialLightFlagOff(party->paperAnimGroup, 1);
}





static inline char* partyChgPoseId_inl(PartyEntry* party, s32 index) {
    BOOL v5;
    s32 poseIdx = index;
    char* v6;
    char* v8;

    if ((party->flags2 & 2) != 0) {
        return party->field_18;
    }
    v5 = FALSE;
    if (party->currentMemberId < 8) {
        if (partyGetHp(party->currentMemberId) <= 5) v5 = TRUE;
    }
    if (v5 == TRUE) {
        if (index == 1) {
            poseIdx = 12;
        } else if (index == 2) {
            poseIdx = 13;
        } else if (index == 3) {
            poseIdx = 14;
        }
    }
    if (!(party->flags2 & 0x8000)) {
        v6 = partyPoseTbl[party->currentMemberId][poseIdx];
    } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
        v6 = partyDotPoseTbl[party->currentMemberId][poseIdx];
    } else {
        v6 = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][poseIdx];
    }
    v8 = party->field_18;
    if (v8 && !strcmp(v8, v6)) {
        return v6;
    }
    party->field_18 = v6;
    party->field_20 = 0;
    party->field_8 |= 0x2000u;
    return v6;
}

#define PCPI2(party, index) \
    do { \
        char* v6; \
        BOOL v5; \
        s32 poseIdx = (index); \
        char* v8; \
        if (((party)->flags2 & 2) == 0) { \
            v5 = FALSE; \
            if ((party)->currentMemberId < 8) { \
                if (partyGetHp((party)->currentMemberId) <= 5) v5 = TRUE; \
            } \
            if (v5 == TRUE) { \
                if ((index) == 1) { \
                    poseIdx = 12; \
                } else if ((index) == 2) { \
                    poseIdx = 13; \
                } else if ((index) == 3) { \
                    poseIdx = 14; \
                } \
            } \
            if (!((party)->flags2 & 0x8000)) { \
                v6 = partyPoseTbl[(party)->currentMemberId][poseIdx]; \
            } else if ((party)->currentMemberId != PARTY_MEMBER_YOSHI) { \
                v6 = partyDotPoseTbl[(party)->currentMemberId][poseIdx]; \
            } else { \
                v6 = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][poseIdx]; \
            } \
            v8 = (party)->field_18; \
            if (!(v8 && !strcmp(v8, v6))) { \
                (party)->field_18 = v6; \
                (party)->field_20 = 0; \
                (party)->field_8 |= 0x2000u; \
            } \
        } \
    } while (0)

char* partyChgPoseId(PartyEntry* party, s32 index) {
    char* v6;
    char* v8;
    BOOL v5;
    s32 poseIdx = index;

    if ((party->flags2 & 2) != 0) {
        return party->field_18;
    }
    v5 = 0;
    if ((party->currentMemberId < 8) && (partyGetHp(party->currentMemberId) <= 5)) {
        v5 = TRUE;
    }
    if (v5 == TRUE) {
        if (index == 1) {
            poseIdx = 12;
        } else if (index == 2) {
            poseIdx = 13;
        } else if (index == 3) {
            poseIdx = 14;
        }
    }
    if (!(party->flags2 & 0x8000)) {
        v6 = partyPoseTbl[party->currentMemberId][poseIdx];
    } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
        v6 = partyDotPoseTbl[party->currentMemberId][poseIdx];
    } else {
        v6 = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][poseIdx];
    }
    v8 = party->field_18;
    if (v8 && !strcmp(v8, v6)) {
        return v6;
    }
    party->field_18 = v6;
    party->field_20 = 0;
    party->field_8 |= 0x2000u;
    return v6;
}
#define partyChgPoseId partyChgPoseId_inl


void partyChgPose(PartyEntry* party, char* pose) {
    if (!(party->flags2 & 2)) {
        if (party->field_18 == NULL || strcmp(party->field_18, pose)) {
            if (!(party->flags2 & 0x8000)) {
                party->field_18 = pose;
                party->field_20 = 0;
                party->field_8 |= 0x2000;
            }
        }
    }
}

void partyChgPaper(PartyEntry* party, char* paper) {
    if (!(party->flags2 & 2)) {
        if (party->field_1C == 0 || strcmp((char*)party->field_1C, paper)) {
            party->field_1C = (u32)paper;
            party->field_20 = 0;
            party->field_8 |= 0x4000;
        }
    }
}

void partyCtrlOff(PartyEntry* party) {
    party = partyPtrTbl[0];
    if (party) {
        party->flags |= PARTY_FLAG_CTRL_OFF;
    }
    party = partyPtrTbl[1];
    if (party) {
        party->flags |= PARTY_FLAG_CTRL_OFF;
    }
}

void partyCtrlOn(PartyEntry* party) {
    party = partyPtrTbl[0];
    if (party) {
        party->flags &= ~PARTY_FLAG_CTRL_OFF;
    }
    party = partyPtrTbl[1];
    if (party) {
        party->flags &= ~PARTY_FLAG_CTRL_OFF;
    }
}

void partyRun(PartyEntry* party) {
    if (party->field_34 == 0) {
        partyChgRunMode(party, 1);
    }
}

void partyStop(PartyEntry* party) {
    partyChgRunMode(party, 0);
}

void partySetForceMove(PartyEntry* party, s16 a2, f32 a3, f32 a4) {
    party->field_100 = a3;
    party->field_28 = a2;
    party->field_104 = a4;
    partyChgRunMode(party, 9);
    partyChgMot(party, 1);
}

s32 partyEntryHello(s32 memberId) {
    s32 id = partyEntryMain(memberId);
    partyChgRunMode(partyGetPtr(id), 4);
    return id;
}

s32 partyEntry(s32 memberId) {
    PartyEntry* party;
    s32 id;
    MarioWork* player;

    id = partyEntryMain(memberId);
    party = partyGetPtr(id);
    party->scale = (Vec){1.0f, 1.0f, 1.0f};
    party->field_38 = 10;
    player = party->playerPtr;
    party->field_110 = player->unk1A8;
    party->field_10C = player->unk1A8;
    partyGetAppearPos2(party, &party->position);
    party->field_B8 = party->position;
    partyChgPoseId_inl(party, 1);
    party->field_34 = 1;
    return id;
}

s32 partyEntryPos(s32 memberId, f32 x, f32 y, f32 z) {
    PartyEntry* party;
    s32 index;
    s32 id;
    BOOL lowHp;
    MarioWork* player;

    id = partyEntryMain(memberId);
    party = partyGetPtr(id);
    party->scale = (Vec){1.0f, 1.0f, 1.0f};
    party->position = (Vec){x, y, z};
    player = party->playerPtr;
    party->scale = (Vec){1.0f, 1.0f, 1.0f};
    party->field_110 = player->unk1A8;
    party->field_10C = player->unk1A8;
    index = 1;
    if (!(party->flags2 & 2)) {
        lowHp = FALSE;
        if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
            lowHp = TRUE;
        }
        if (lowHp == TRUE) {
            index = 12;
        }
        if (!(party->flags2 & 0x8000)) {
            index = (s32)partyPoseTbl[party->currentMemberId][index];
        } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
            index = (s32)partyDotPoseTbl[party->currentMemberId][index];
        } else {
            index = (s32)yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][index];
        }
        if (party->field_18 == NULL || strcmp(party->field_18, (char*)index)) {
            party->field_18 = (char*)index;
            party->field_20 = 0;
            party->field_8 |= 0x2000;
        }
    }
    party->field_34 = 0;
    return id;
}

BOOL partyGoodbye(s32 id) {
    BOOL ret = FALSE;

    if (id >= 0) {
        partyChgRunMode(partyGetPtr(id), 8);
        ret = TRUE;
    }
    return ret;
}

s32 partyKill(s32 id) {
    PartyEntry* party;
    MarioWork* player;
    PartyEntry* other;

    party = partyGetPtr(id);
    if (party == NULL) {
        return 0;
    }
    player = party->playerPtr;
    party_dp = &partyDataTbl[partyGetPtr(player->currPartySlotId[id])->currentMemberId];
    if (party_dp->exit_func && party->field_34 != 11) {
        party_dp->exit_func(party);
    }
    if (partyGetPtr(player->currPartySlotId[id])) {
        player->prevPartyMemberId[id] = partyGetPtr(player->currPartySlotId[id])->currentMemberId;
    } else {
        player->prevPartyMemberId[id] = 0;
    }
    player->currPartySlotId[id] = -1;
    partyPtrTbl[id] = NULL;
    animPoseRelease(party->paperAnimGroup);
    __memFree(HEAP_DEFAULT, party);
    return 0;
}

s32 partyKill2(s32 id) {
    PartyEntry* party;
    MarioWork* player;

    party = partyGetPtr(id);
    if (party == NULL) {
        return 0;
    }
    player = party->playerPtr;
    party_dp = &partyDataTbl[partyGetPtr(player->currPartySlotId[id])->currentMemberId];
    if (party_dp->exit_func && party->field_34 != 11) {
        party_dp->exit_func(party);
    }
    player->prevPartyMemberId[id] = 0;
    player->currPartySlotId[id] = -1;
    partyPtrTbl[id] = NULL;
    animPoseRelease(party->paperAnimGroup);
    __memFree(HEAP_DEFAULT, party);
    return 0;
}

s32 yoshiSetColor(void) {
    s32 color;
    s32 r = irand(1000);

    if (r < 150) {
        color = 0;
    } else if (r < 300) {
        color = 1;
    } else if (r < 450) {
        color = 2;
    } else if (r < 600) {
        color = 3;
    } else if (r < 750) {
        color = 4;
    } else if (r < 900) {
        color = 5;
    } else {
        color = 6;
    }
    pouchSetPartyColor(PARTNER_YOSHI, color);
    return color;
}

f32 partyGetHeight(PartyEntry* party) {
    if (party->currentMemberId == PARTY_MEMBER_YOSHI && (party->flags & 0x100)) {
        return 18.5f + party->field_F0;
    }
    return party->field_F0;
}

static inline s32 partyGetFreeSlot(s32 start) {
    s32 i;

    for (i = start; i < 2; i++) {
        if (partyPtrTbl[i] == NULL) {
            memset(partyPtrTbl[i] = __memAlloc(HEAP_DEFAULT, sizeof(PartyEntry)), 0, sizeof(PartyEntry));
            return i;
        }
    }
    return -1;
}

static inline void partyPoseEntry(PartyEntry* party) {
    char* animName;

    if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
        if (!(party->flags2 & 0x8000)) {
            animName = party_dp->name;
        } else {
            animName = "d_mario";
        }
    } else {
        animName = yoshigroup[pouchGetPartyColor(PARTNER_YOSHI)];
        if (party->flags2 & 0x8000) {
            animName = "d_mario";
        }
    }
    if (party->paperAnimGroup >= 0) {
        animPoseRelease(party->paperAnimGroup);
    }
    party->paperAnimGroup = animPoseEntry(animName, 2);
}

static inline void setAnimPoseMaterialFlag(PartyEntry* party) {
    MarioWork* player = party->playerPtr;

    animPoseSetMaterialFlagOn(party->paperAnimGroup, animPoseGetMaterialFlag(player->paperAnimGroupId[marioAnimeId()]));
    animPoseSetMaterialLightFlagOn(party->paperAnimGroup,
                                   animPoseGetMaterialLightFlag(player->paperAnimGroupId[marioAnimeId()]));
}

static inline s32 partyGetFreeSlot2(void) {
    if (partyPtrTbl[1] == NULL) {
        memset(partyPtrTbl[1] = __memAlloc(HEAP_DEFAULT, sizeof(PartyEntry)), 0, sizeof(PartyEntry));
        return 1;
    }
    return -1;
}


__declspec(section ".sdata") char* yoshigroup[7] = {
        "c_babyyoshi",
        "c_babyyoshi2",
        "c_babyyoshi3",
        "c_babyyoshi4",
        "c_babyyoshi5",
        "c_babyyoshi6",
        "c_babyyoshi7"
};
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_8041061C_sdata[1] = {0};
#pragma force_active reset
s32 partyEntryMain(s32 memberId) {
    MarioWork* player;
    s32 slot;
    PartyEntry* party;

    player = marioGetPtr();
    slot = partyGetFreeSlot(0);
    if (slot < 0) {
        return -1;
    }
    party = partyGetPtr(slot);
    memset(party, 0, sizeof(PartyEntry));
    party->camId = marioGetCamId();
    party->paperAnimGroup = -1;
    party->field_10 = -1;
    party->paperPoseId = -1;
    party->flags = 1;
    party->currentSlotId = slot;
    party->playerPtr = player;
    party->currentMemberId = memberId;
    if (player->prevPartyMemberId[0] == 0) {
        player->prevPartyMemberId[0] = party->currentMemberId;
    }
    if (slot == 1) {
        party->flags |= 4;
    }
    party_dp = &partyDataTbl[memberId];
    if (player->flags & 0x80000000) {
        party->flags2 |= 0x8000;
    }
    if (!party && !party && !party) {
    }
    partyPoseEntry(party);
    animPosePeraOff(party->paperAnimGroup);
    setAnimPoseMaterialFlag(party);
    partyChgPose(party, partyChgPoseId(party, 1));
    party->field_1C = 0;
    party_dp->init_func(party);
    return slot;
}

s32 partyEntry2(s32 memberId) {
    MarioWork* player;
    s32 slot;
    PartyEntry* party;

    player = marioGetPtr();
    slot = partyGetFreeSlot2();
    if (slot < 0) {
        return -1;
    }
    party = partyGetPtr(slot);
    memset(party, 0, sizeof(PartyEntry));
    party->camId = marioGetCamId();
    party->paperAnimGroup = -1;
    party->field_10 = -1;
    party->paperPoseId = -1;
    party->flags = 9;
    party->currentSlotId = slot;
    party->playerPtr = player;
    party->currentMemberId = memberId;
    if (slot == 1) {
        party->flags |= 4;
    }
    party_dp = &partyDataTbl[memberId];
    partyPoseEntry(party);
    animPosePeraOff(party->paperAnimGroup);
    setAnimPoseMaterialFlag(party);
    partyChgPose(party, partyChgPoseId(party, 1));
    party->field_1C = 0;
    party_dp->init_func(party);
    party->position = player->position;
    party->field_B8 = party->position;
    party->scale = (Vec){1.0f, 1.0f, 1.0f};
    party->field_34 = 0;
    return slot;
}

s32 partyEntry2Hello(s32 memberId) {
    MarioWork* player;
    s32 slot;
    PartyEntry* party;

    player = marioGetPtr();
    slot = partyGetFreeSlot2();
    if (slot < 0) {
        return -1;
    }
    party = partyGetPtr(slot);
    memset(party, 0, sizeof(PartyEntry));
    party->camId = marioGetCamId();
    party->paperAnimGroup = -1;
    party->field_10 = -1;
    party->paperPoseId = -1;
    party->flags = 9;
    party->currentSlotId = slot;
    party->playerPtr = player;
    party->currentMemberId = memberId;
    if (slot == 1) {
        party->flags |= 4;
    }
    party_dp = &partyDataTbl[memberId];
    partyPoseEntry(party);
    animPosePeraOff(party->paperAnimGroup);
    setAnimPoseMaterialFlag(party);
    partyChgPose(party, partyChgPoseId(party, 1));
    party->field_1C = 0;
    party_dp->init_func(party);
    partyChgRunMode(party, 4);
    return slot;
}

s32 partyEntry2Pos(s32 memberId, f32 x, f32 y, f32 z) {
    MarioWork* player;
    s32 slot;
    PartyEntry* party;

    player = marioGetPtr();
    slot = partyGetFreeSlot2();
    if (slot < 0) {
        return -1;
    }
    party = partyGetPtr(slot);
    memset(party, 0, sizeof(PartyEntry));
    party->camId = marioGetCamId();
    party->paperAnimGroup = -1;
    party->field_10 = -1;
    party->paperPoseId = -1;
    party->flags = 9;
    party->currentSlotId = slot;
    party->playerPtr = player;
    party->currentMemberId = memberId;
    if (slot == 1) {
        party->flags |= 4;
    }
    party_dp = &partyDataTbl[memberId];
    partyPoseEntry(party);
    animPosePeraOff(party->paperAnimGroup);
    setAnimPoseMaterialFlag(party);
    { char* id = partyChgPoseId(party, 1); partyChgPose(party, id); }
    party->field_1C = 0;
    party_dp->init_func(party);
    partyChgRunMode(party, 2);
    party->position = (Vec){x, y, z};
    party->field_B8 = party->position;
    party->scale.x = 1.0f;
    party->scale.y = 1.0f;
    party->scale.z = 1.0f;
    return slot;
}

#define PU8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PS8(p, o) (*(s8*)((u8*)(p) + (o)))
void bomhei_use_cancel(void);
void partyShadowOff(PartyEntry* party);
void vivianUnhold(PartyEntry* party);
void marioGetoffYoshi(void);
void L_partyVivianTailStart(PartyEntry* party);
void getRidePos(PartyEntry* party, Vec* pos);
void driveParty(PartyEntry* party);
#include "mario/mario_sbr.h"
s32 partyHelloMain(PartyEntry* party);
double sin(double);
double cos(double);
f32 toMovedir2(f32 a, f32 b);
f32 toMovedirSimple(f32 a);
s32 yoshiGetStatus(void);
BOOL N_marioShipGetDir(void);
void partyClearJumpPara(PartyEntry* party);
void L_partyForceSlitOff(PartyEntry* party);
static inline void partyShadowOn(PartyEntry* party) {
    if (party) {
        party->flags2 &= ~0x100000;
    }
}
#define PU16(p, o) (*(u16*)((u8*)(p) + (o)))
#define PF32(p, o) (*(f32*)((u8*)(p) + (o)))

void partyInit(void) {
    memset(partyPtrTbl, 0, sizeof(partyPtrTbl));
}



void partyClearFootmark(void);
void chuchu_reinit(PartyEntry* party);
void unk_8014c47c(void);

static inline void partyReInitClear(PartyEntry* party) {
    s32 poseId;
    poseId = party->paperAnimGroup;
    if (poseId >= 0) {
        animPoseSetMaterialFlagOff(poseId, -1);
        animPoseSetMaterialLightFlagOn(poseId, 2);
    }
}

void partyReInit(void) {
    s32 poseId;
    s32 dotted;
    PartyEntry* party;
    char* pose;

    party = partyGetPtr(marioGetExtraPartyId());
    if (party) {
        party->flags = 0xD;
        party->flags2 = 0;
        poseId = party->paperAnimGroup;
        if (poseId >= 0) {
            animPoseSetMaterialFlagOff(poseId, -1);
            animPoseSetMaterialLightFlagOn(poseId, 2);
        }
        party->field_37 = 0;
        if (party->field_1C) {
            animPoseSetPaperAnimGroup(party->paperAnimGroup, NULL, FALSE);
            party->field_1C = 0;
            party->flags2 &= ~0x4000000;
            if (party->paperPoseId >= 0) {
                animPaperPoseRelease(party->paperPoseId);
                party->paperPoseId = -1;
            }
        }
        *(s32*)((u8*)party + 0x130) = 0;
        *(s32*)((u8*)party + 0x12C) = 0;
        *(s32*)((u8*)party + 0x148) = 0;
        *(s32*)((u8*)party + 0x138) = 0;
        *(s32*)((u8*)party + 0x13C) = 0;
        *(s32*)((u8*)party + 0x134) = 0;
        *(s32*)((u8*)party + 0x140) = 0;
        *(s32*)((u8*)party + 0x144) = 0;
        partyChgPose(party, partyChgPoseId(party, 1));
        partyChgRunMode(party, 2);
        partyChgMot(party, 0);
        party->yoshiPsndSFXId = -1;
        party->scale = (Vec){1.0f, 1.0f, 1.0f};
    }
    party = partyGetPtr(marioGetPartyId());
    if (party) {
        dotted = party->flags2 & 0x8000;
        party->flags = 1;
        party->flags2 = 0;
        party->field_37 = 0;
        party->camId = marioGetCamId();
        if (party) {
            poseId = party->paperAnimGroup;
        if (poseId >= 0) {
            animPoseSetMaterialFlagOff(poseId, -1);
            animPoseSetMaterialLightFlagOn(poseId, 2);
        }
            if (party->field_1C) {
            animPoseSetPaperAnimGroup(party->paperAnimGroup, NULL, FALSE);
            party->field_1C = 0;
            party->flags2 &= ~0x4000000;
            if (party->paperPoseId >= 0) {
                animPaperPoseRelease(party->paperPoseId);
                party->paperPoseId = -1;
            }
        }
            *(s32*)((u8*)party + 0x130) = 0;
            *(s32*)((u8*)party + 0x12C) = 0;
            *(s32*)((u8*)party + 0x148) = 0;
            *(s32*)((u8*)party + 0x138) = 0;
            *(s32*)((u8*)party + 0x13C) = 0;
            *(s32*)((u8*)party + 0x134) = 0;
            *(s32*)((u8*)party + 0x140) = 0;
            *(s32*)((u8*)party + 0x144) = 0;
            party->scale = (Vec){1.0f, 1.0f, 1.0f};
        }
        partyClearFootmark();
        if (dotted) {
            partyPoseEntry(party);
        }
        if (party->currentMemberId == PARTY_MEMBER_YOSHI && party->field_34 == 3) {
            return;
        }
        if (!(marioGetPtr()->flags & 0x400000)) {
            pose = partyChgPoseId(party, 1);
            partyChgPose(party, pose);
            partyChgRunMode(party, 2);
            partyChgMot(party, 0);
        } else {
            pose = partyChgPoseId(party, 1);
            partyChgPose(party, pose);
            partyChgRunMode(party, 2);
            party->field_3A[1] = 6;
            party->field_38 = 0;
            party->flags |= 0x8000;
        }
        if (party->currentMemberId == 7) {
            chuchu_reinit(party);
        }
        unk_8014c47c();
    }
}
void partyUsePost(PartyEntry* party) {
    PartyFunc func;
    if (party && party->field_34 == 3) {
        func = partyDataTbl[party->currentMemberId].post_func;
        if (func) {
            func(party);
        }
    }
}


static inline void partyChgRunM(PartyEntry* party);
static inline void partyChgRunMo(PartyEntry* party);
void N__partyChgRunMode(PartyEntry* party, s32 prev, s32 mode) {
    MarioWork* mario;
    PartyFunc func;

    switch (prev) {
    case 11:
        if (mode != 8) {
            mario = party->playerPtr;
            partyChgRunMo(party);
            party->field_FC = mario->unk19C;
            party->field_100 = mario->directionView;
            party->field_110 = mario->unk1AC;
            party->flags &= ~0x70000;
            partyClearJumpPara(party);
            partyChgPoseId(party, 1);
            party->flags2 &= ~0x10;
            party->flags2 &= ~1;
            partyShadowOn(party);
        }
        break;
    case 4:
        mario = party->playerPtr;
        partyChgRunM(party);
        party->flags2 &= ~0x10;
        party->field_FC = mario->unk19C;
        party->field_100 = mario->directionView;
        party->field_110 = mario->unk1AC;
        party->field_10C = mario->unk1A8;
        partyClearJumpPara(party);
        {
            char* v6;
            BOOL v5;
            s32 poseIdx = 1;
            char* v8;
            if ((party->flags2 & 2) == 0) {
                v5 = FALSE;
                if (party->currentMemberId < 8) {
                    if (partyGetHp(party->currentMemberId) <= 5) v5 = TRUE;
                }
                if (v5 == TRUE) {
                    poseIdx = 12;
                }
                if (!(party->flags2 & 0x8000)) {
                    v6 = partyPoseTbl[party->currentMemberId][poseIdx];
                } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
                    v6 = partyDotPoseTbl[party->currentMemberId][poseIdx];
                } else {
                    v6 = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][poseIdx];
                }
                v8 = party->field_18;
                if (v8 == NULL || strcmp(v8, v6)) {
                    party->field_18 = v6;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000u;
                }
            }
        }
        party->flags2 &= ~0x10;
        party->field_34 = 1;
        break;
    case 12:
        partyShadowOn(party);
        break;
    case 3:
        func = partyDataTbl[party->currentMemberId].post_func;
        if (func) {
            func(party);
        }
        break;
    case 1:
    case 2:
        L_partyForceSlitOff(party);
        break;
    }
}

#pragma dont_inline on
void partyChgRunMode(PartyEntry* party, s32 mode) {
    if (party->field_34 != 9 && party->field_34 != 3) {
        PU8(party, 0x35) = party->field_34;
    }
    N__partyChgRunMode(party, party->field_34, mode);
    party->field_34 = mode;
    party->field_8 |= 8;
}
#pragma dont_inline reset

#pragma dont_inline on
void N_partyForceChgRunMode(PartyEntry* party, s32 mode, s32 sub) {
    if (party->field_34 != 9 && party->field_34 != 3) {
        PU8(party, 0x35) = party->field_34;
    }
    N__partyChgRunMode(party, party->field_34, mode);
    party->field_34 = mode;
    party->field_8 &= ~8;
    party->field_38 = sub;
}
#pragma dont_inline reset

/* @@MID1@@ */
void unk_800ba2dc(PartyEntry* party) {
    if (party->field_34 == PU8(party, 0x35)) {
        return;
    }
    if (party->field_34 == 4) {
        MarioWork* mario = party->playerPtr;
        partyChgRunM(party);
        party->flags2 &= ~0x10;
        PF32(party, 0xFC) = PF32(mario, 0x19C);
        PF32(party, 0x100) = PF32(mario, 0x1A0);
        PF32(party, 0x110) = PF32(mario, 0x1AC);
        PF32(party, 0x10C) = PF32(mario, 0x1A8);
        partyClearJumpPara(party);
        {
            char* v6;
            BOOL v5;
            s32 poseIdx = 1;
            char* v8;
            if ((party->flags2 & 2) == 0) {
                v5 = FALSE;
                if (party->currentMemberId < 8) {
                    if (partyGetHp(party->currentMemberId) <= 5) v5 = TRUE;
                }
                if (v5 == TRUE) {
                    poseIdx = 12;
                }
                if (!(party->flags2 & 0x8000)) {
                    v6 = partyPoseTbl[party->currentMemberId][poseIdx];
                } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
                    v6 = partyDotPoseTbl[party->currentMemberId][poseIdx];
                } else {
                    v6 = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][poseIdx];
                }
                v8 = party->field_18;
                if (!(v8 && !strcmp(v8, v6))) {
                    party->field_18 = v6;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000u;
                }
            }
        }
        party->flags2 &= ~0x10;
        party->field_34 = 1;
        partyUsePost(party);
        partyChgMot(party, 0);
    } else if (party->field_34 == 11) {
        MarioWork* mario = party->playerPtr;
        partyChgRunMo(party);
        PF32(party, 0xFC) = PF32(mario, 0x19C);
        PF32(party, 0x100) = PF32(mario, 0x1A0);
        PF32(party, 0x110) = PF32(mario, 0x1AC);
        party->flags &= ~0x70000;
        partyClearJumpPara(party);
        partyChgPoseId(party, 1);
        party->flags2 &= ~0x10;
        party->flags2 &= ~1;
        partyShadowOn(party);
        partyUsePost(party);
        partyChgMot(party, 0);
    } else {
        if (party->field_34 == 12) {
            partyShadowOn(party);
        }
        partyUsePost(party);
        party->field_34 = PU8(party, 0x35);
        party->field_8 |= 8;
        partyChgMot(party, 0);
    }
}


static inline s32 partyCountActive(void) {
    s32 count = 0;
    s32 i;
    for (i = 0; i < 2; i++) {
        if (partyPtrTbl[i] && (partyGetPtr(i)->flags & 1)) {
            count++;
        }
    }
    return count;
}
void partyMain(void) {
    s32 i;
    PartyEntry* party;

    marioGetPtr();
    if (partyCountActive()) {
        unk_8013be14();
        for (i = 0; i < 2; i++) {
            party = partyPtrTbl[i];
            if (party && (party->flags & 1) && marioBgmodeChk() != 1) {
                driveParty(party);
            }
        }
    }
}

/* @@DRIVE@@ */
void party_kemuri(PartyEntry* party);
void vivianShadow(PartyEntry* party);
s32 vivianGetStatus(void);
void unk_800c7c98(PartyEntry* party);
void partyForceWalk(PartyEntry* party);
void L_partyForceSlitOff(PartyEntry* party);
void partyPreDisp(PartyEntry* party);
void partyHello(PartyEntry* party);
void partyRideMain(PartyEntry* party);
s32 partyRideYoshiMain(PartyEntry* party);
void party_damage_return(PartyEntry* party);
s32 partyDokanInit(PartyEntry* party);
s32 partyDokanMain(PartyEntry* party);

static inline void partyHello_inl(PartyEntry* party);

void driveParty(PartyEntry* party) {
    MarioWork* mario;
    f32 x, y, z;

    mario = party->playerPtr;
    *(f32*)((u8*)party + 0xF8) = *(f32*)((u8*)mario + 0x198);
    if (*(s16*)((u8*)party + 0x2A) > 0) {
        (*(s16*)((u8*)party + 0x2A))--;
    }
    switch (party->field_34) {
    case 4: case 5: case 6: case 7: case 8: case 14:
        break;
    default:
        if (!(party->flags & 0x20000000)) break;
        if (vivianGetStatus() != 2) {
            vivianShadow(party);
        }
        partyPreDisp(party);
        return;
    }
    if (*(u32*)mario & 0x2000000) {
        party->flags |= 0x800000;
    } else {
        party->flags &= ~0x800000;
    }
    *(s32*)((u8*)party + 0x138) = 0;
    *(s32*)((u8*)party + 0x12C) = 0;
    *(s32*)((u8*)party + 0x140) = 0;
    *(s32*)((u8*)party + 0x134) = 0;
    *(s32*)((u8*)party + 0x130) = 0;
    switch (party->field_34) {
    case 0:
        if ((party->flags & 0x20000000) && !(party->flags & 0x10000000)) break;
        if (party->field_8 & 8) {
            party->field_8 &= ~8;
            partyChgPoseId_inl(party, 1);
            party->field_104 = 0.0f;
            party->flags &= ~0x100;
            partyChgMot(party, 0);
        }
        party_dp = &partyDataTbl[party->currentMemberId];
        party_dp->move_func(party);
        break;
    case 1:
        if ((party->flags & 0x20000000) && !(party->flags & 0x10000000)) break;
        unk_800c7c98(party);
        break;
    case 2:
        if ((party->flags & 0x20000000) && !(party->flags & 0x10000000)) break;
        party_dp = &partyDataTbl[party->currentMemberId];
        party_dp->move_func(party);
        break;
    case 9:
        if ((party->flags & 0x20000000) && !(party->flags & 0x10000000)) break;
        partyForceWalk(party);
        break;
    case 3:
        if ((party->flags & 0x20000000) && !(party->flags & 0x10000000)) break;
        {
            void (*fn)(PartyEntry*);
            party_dp = &partyDataTbl[party->currentMemberId];
            fn = party_dp->use_func;
            party->flags |= 0x100;
            fn(party);
        }
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        partyHello_inl(party);
        break;
    case 8:
        party_dp = &partyDataTbl[party->currentMemberId];
        party_dp->bye_func(party);
        break;
    case 10:
        if (party->field_8 & 8) {
            party->field_8 &= ~8;
            partyDokanInit(party);
            party->flags &= ~0x100;
        }
        partyDokanMain(party);
        break;
    case 11:
        if ((party->flags & 0x20000000) && !(party->flags & 0x10000000)) break;
        if (party->field_8 & 8) {
            party->field_8 &= ~8;
            party->field_38 = 0;
            party->flags &= ~0x100;
            L_partyForceSlitOff(party);
        }
        partyRideMain(party);
        break;
    case 12:
        if ((party->flags & 0x20000000) && !(party->flags & 0x10000000)) break;
        if (party->field_8 & 8) {
            party->field_8 &= ~8;
            party->field_38 = 0;
            party->flags &= ~0x100;
        }
        partyRideYoshiMain(party);
        break;
    case 13:
        if ((party->flags & 0x20000000) && !(party->flags & 0x10000000)) break;
        party_damage_return(party);
        break;
    case 14:
        if (party->field_8 & 8) {
            party->field_8 &= ~8;
        }
        mario->prevPartyMemberId[party->currentSlotId] = party->currentMemberId;
        mario->currPartySlotId[party->currentSlotId] = -1;
        partyPtrTbl[party->currentSlotId] = NULL;
        animPoseRelease(party->paperAnimGroup);
        __memFree(0, party);
        return;
    }
    party_kemuri(party);
    marioGetScreenPos(party->position, &x, &y, &z);
    if (marioChkInScreen(x, y)) {
        party->flags2 &= ~0x800000;
    } else {
        party->flags2 |= 0x800000;
    }
    *(Vec*)((u8*)party + 0xAC) = (Vec){0.0f, 0.0f, 0.0f};
    partyPreDisp(party);
}
static inline void partyHello_inl(PartyEntry* party) {
    MarioWork* mario;

    if (party->field_8 & 8) {
        party->field_8 &= ~8;
        mario = party->playerPtr;
        if (mario->currMotionId == 0x16 || mario->currMotionId == 0x18 || mario->currMotionId == 0x19) {
            party->field_38 = 30;
        } else if (party->field_34 == 4 || party->field_34 == 5) {
            party->flags2 |= 0x10;
            party->scale = (Vec){0.0f, 0.0f, 0.0f};
            party->field_38 = 0;
            party->field_110 = mario->unk1A8;
            party->field_10C = mario->unk1A8;
            party->position = mario->position;
            party->field_B8 = party->position;
        } else if (party->field_34 == 7) {
            party->scale = (Vec){1.0f, 1.0f, 1.0f};
            party->field_38 = 20;
            party->field_110 = mario->unk1A8;
            party->field_10C = mario->unk1A8;
        } else {
            party->scale = (Vec){1.0f, 1.0f, 1.0f};
            party->field_38 = 10;
            party->field_110 = mario->unk1A8;
            party->field_10C = mario->unk1A8;
            party->position = mario->position;
            party->field_B8 = party->position;
        }
        party->flags &= ~0x100;
    }
    partyHelloMain(party);
}



BOOL partyDoWork(void) {
    PartyEntry* party = partyGetPtr(marioGetPtr()->currPartySlotId[0]);
    if (!party) {
        return FALSE;
    }
    if (party->field_34 != 0 && party->field_34 != 1 && party->field_34 != 2) {
        return FALSE;
    }
    marioGetPtr()->baseSpeed = 0.0f;
    partyChgRunMode(party, 3);
    return TRUE;
}


void partyHello(PartyEntry* party) {
    partyHello_inl(party);
}

/* @@HELLO@@ */
void partyMoveNoHosei(PartyEntry* party);
f32 partyToMovedir(PartyEntry* party, f32 dir);
void partyGetAppearPos(PartyEntry* party, Vec* pos);
void allPartyForceRideOn(void);

s32 partyHelloMain(PartyEntry* party) {
    s32 ret = 0;
    u8* mario = (u8*)party->playerPtr;

    switch (party->field_38) {
    case 0: {
        f32 x, y, z, h, dy;
        x = PF32(mario, 0x8C);
        y = PF32(mario, 0x90);
        z = PF32(mario, 0x94);
        party->position.x = x;
        party->position.y = y + 0.5f * PF32(mario, 0x1B8);
        party->position.z = z;
        partyGetAppearPos(party, (Vec*)((u32)party + 0x94));
        *(s32*)((u8*)party + 0x24) = 16;
        PF32(party, 0x128) = distABf(x, z, PF32(party, 0x94), PF32(party, 0x9C));
        PF32(party, 0x100) = angleABf(x, z, PF32(party, 0x94), PF32(party, 0x9C));
        PF32(party, 0x104) = PF32(party, 0x128) / (f32)*(s32*)((u8*)party + 0x24);
        h = 0.5f * (f32)*(s32*)((u8*)party + 0x24);
        dy = PF32(party, 0x98) - party->position.y;
        if (__fabsf(dy) < 1.0f) {
            dy = 10.0f;
        } else if (dy >= 0.0f) {
            dy += 0.5f * PF32(party, 0xF0);
        } else {
            dy = 10.0f;
        }
        dy = (dy + dy) / (h * h - h);
        PF32(party, 0x114) = dy * h;
        PF32(party, 0x118) = -dy;
        party->flags |= 0x10;
        party->field_38 = 1;
        break;
    }
    case 1:
        partyMoveNoHosei(party);
        if (PF32(party, 0x114) <= 0.0f) {
            party->flags &= ~0x10;
            PF32(party, 0x118) = -1.6f;
        }
        if (party->position.y < -2000.0f) {
            *(s32*)((u8*)party + 0x24) = 0;
            party->field_38 = 2;
            break;
        }
        if (*(u32*)((u8*)party + 0x138) == 0) {
            s32 n = 16 - *(s32*)((u8*)party + 0x24);
        f32 v = (f32)n / 16.0f;
            Vec s;
        s = (Vec){0.0f, 0.0f, 0.0f};
            s.x = s.y = s.z = v;
            party->scale = s;
            if (--*(s32*)((u8*)party + 0x24) <= 0) {
                *(s32*)((u8*)party + 0x24) = 0;
            }
            break;
        }
        party->field_38 = 2;
    case 2: {
        f32 v = (f32)(16 - *(s32*)((u8*)party + 0x24)) / 16.0f;
        Vec s;
        s = (Vec){0.0f, 0.0f, 0.0f};
        s.x = s.y = s.z = v;
        party->scale = s;
        if (--*(s32*)((u8*)party + 0x24) > 0) {
            break;
        }
        *(s32*)((u8*)party + 0x24) = 0;
        if (PF32(party, 0x110) >= 90.0f && PF32(party, 0x110) <= 270.0f) {
            PF32(party, 0xFC) = partyToMovedir(party, 180.0f);
            PF32(party, 0x100) = PF32(party, 0xFC);
            PF32(party, 0x110) = PF32(party, 0x10C) = 180.0f;
        } else {
            PF32(party, 0xFC) = partyToMovedir(party, 0.0f);
            PF32(party, 0x100) = PF32(party, 0xFC);
            PF32(party, 0x110) = PF32(party, 0x10C) = 0.0f;
        }
        partyClearJumpPara(party);
        partyChgPoseId_inl(party, 1);
        party->flags2 &= ~0x10;
        ret = 1;
        party->field_34 = 1;
        psndSFXOn_3D((const char*)0x20920, (Vec*)(mario + 0x8C));
        break;
    }
    case 10:
        partyGetAppearPos2(party, &party->position);
        party->field_B8 = party->position;
        PCPI2(party, 1);
        party->field_34 = 1;
        ret = 1;
        break;
    case 20:
        PCPI2(party, 1);
        party->field_34 = 0;
        ret = 1;
        break;
    case 30:
        allPartyForceRideOn();
        ret = 1;
        break;
    }
    return ret;
}


s32 partyGoodbyeInit(PartyEntry* party) {
    MarioWork* mario;
    party->field_38 = 10;
    party_dp = &partyDataTbl[party->currentMemberId];
    if (party_dp->init_func) {
        party_dp->init_func(party);
    }
    if (party->flags & 0x100) {
        if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
            vivianUnhold(party);
            party->field_38 = 0;
        }
        if (party->currentMemberId == PARTY_MEMBER_YOSHI) {
            marioGetoffYoshi();
            party->field_38 = 0;
        }
    }
    mario = party->playerPtr;
    if (mario->currMotionId == 0x16 || mario->currMotionId == 0x18 || mario->currMotionId == 0x19) {
        party->field_38 = 20;
    }
    return 0;
}

/* @@DOKAN@@ */
s32 partyGoodbyeMain(PartyEntry* party) {
    MarioWork* mario = marioGetPtr();
    f32 tx, ty, tz, px, py, pz;

    switch (party->field_38) {
    case 0:
        party_dp = &partyDataTbl[party->currentMemberId];
        party_dp->use_func(party);
        if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
            if (vivianGetStatus() == 0) {
                party->field_38 = 10;
                party->field_34 = 8;
            }
        } else if (party->currentMemberId == PARTY_MEMBER_YOSHI) {
            if (yoshiGetStatus() == 0) {
                party->field_38 = 10;
                party->field_34 = 8;
            }
        } else {
            party->field_38 = 10;
            party->field_34 = 8;
        }
        break;
    case 10:
        psndSFXOn_3D((const char*)0x2091F, &mario->position);
        tx = PF32(party, 0x94) = PF32(mario, 0x8C);
        ty = PF32(party, 0x98) = 0.5f * PF32(mario, 0x1B8) + PF32(mario, 0x90);
        tz = PF32(party, 0x9C) = PF32(mario, 0x94);
        px = party->position.x;
        pz = party->position.z;
        py = party->position.y;
        PF32(party, 0x104) = 4.0f;
        PF32(party, 0x11C) = 1.6f;
        PF32(party, 0x128) = distABf(px, pz, tx, tz);
        PF32(party, 0x100) = angleABf(px, pz, tx, tz);
        ty -= py;
        *(s32*)((u8*)party + 0x24) = 15;
        PF32(party, 0x104) = PF32(party, 0x128) / (f32)*(s32*)((u8*)party + 0x24);
        PF32(party, 0x114) = (0.5f * ((f32)*(s32*)((u8*)party + 0x24) * (PF32(party, 0x11C) * (f32)*(s32*)((u8*)party + 0x24))) + ty) / (f32)*(s32*)((u8*)party + 0x24);
        partyChgPoseId_inl(party, 4);
        party->field_38 = 11;
        break;
    case 11: {
        f32 t;
        PF32(party, 0x114) -= PF32(party, 0x11C);
        party->position.y += PF32(party, 0x114);
        movePos(&party->position.x, &party->position.z, PF32(party, 0x104), PF32(party, 0x100));
        PF32(party, 0x10C) = revise360(50.0f + PF32(party, 0x10C));
        t = *(s32*)((u8*)party + 0x24);
        if (t > 10.0f) {
            t = 10.0f;
        }
        party->scale.x = t / 10.0f;
        party->scale.y = party->scale.x;
        party->scale.z = party->scale.x;
        if (--*(s32*)((u8*)party + 0x24) < 0) {
            party->field_38 = 12;
        }
        break;
    }
    case 12:
        partyChgPoseId_inl(party, 1);
        PF32(party, 0x114) = 0.0f;
        party->position.y = PF32(party, 0x98);
        partyChgRunMode(party, 14);
        return 1;
    case 20:
        partyChgRunMode(party, 14);
        return 1;
    }
    return 0;
}

void L_partyVivianTailStop(PartyEntry* party);

static inline void partyPaperOn_inl(PartyEntry* party, const char* animGroup) {
    BOOL v4;
    BOOL v5;

    if (party->paperPoseId >= 0) {
        animPaperPoseRelease(party->paperPoseId);
        party->paperPoseId = -1;
    }
    party->flags2 |= 0x4000000;
    party->paperPoseId = animPaperPoseEntry(animGroup, 2);
    v4 = FALSE;
    if (!strcmp(animGroup, "p_roll") || !strcmp(animGroup, "p_plane") || !strcmp(animGroup, "p_dokan_x") ||
        !strcmp(animGroup, "p_dokan_y")) {
        v4 = TRUE;
    }
    v5 = !v4;
    if (!strcmp(animGroup, "p_slit")) {
        v5 = party->currentMemberId == PARTY_MEMBER_FLURRIE;
    }
    animPoseSetPaperAnimGroup(party->paperAnimGroup, animGroup, v5);
}

static inline void partyChgRunM(PartyEntry* party) {
    party->scale = (Vec){1.0f, 1.0f, 1.0f};
}
s32 partyDokanInit(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;

    party->flags2 |= 0x10;
    switch (PS8(mario, 0x3E)) {
    case 2:
    case 3:
        partyPaperOn_inl(party, "p_dokan_y");
        PF32(party, 0x124) = 30.0f;
        party->field_38 = 0;
        break;
    case 0:
        *(Vec*)((u8*)party + 0x70) = (Vec){0.0f, 0.0f, 0.0f};
    case 1:
        partyPaperOn_inl(party, "p_dokan_y");
        PF32(party, 0x124) = 30.0f;
        party->field_38 = 0;
        break;
    case 5:
        partyPaperOn_inl(party, "p_dokan_x");
        PF32(party, 0x124) = 26.0f;
        party->field_38 = 10;
        PF32(party, 0x8C) = 10.0f;
        break;
    case 4:
        partyPaperOn_inl(party, "p_dokan_x");
        PF32(party, 0x124) = -26.0f;
        party->field_38 = 10;
        PF32(party, 0x8C) = 10.0f;
        PF32(party, 0x64) = revise360(180.0f + PF32(mario, 0xBC));
        break;
    }
    if (!(party->flags2 & 2)) {
        if (party->field_1C == 0 || strcmp((char*)party->field_1C, "PM_D_1A")) {
            party->field_1C = (u32)"PM_D_1A";
            party->field_20 = 0;
            party->field_8 |= 0x4000;
        }
    }
    partyChgPoseId_inl(party, 0);
    party->flags2 |= 8;
    party->flags2 |= 0x10;
    animPoseSetMaterialLightFlagOff(party->paperAnimGroup, 1);
    party->field_110 = revise360(180.0f + PF32(mario, 0x1A8));
    PF32(party, 0x10C) = PF32(mario, 0x1A8);
    party->field_FC = PF32(mario, 0x19C);
    party->position = mario->position;
    L_partyVivianTailStop(party);
    return 0;
}

s32 partyDokanMain(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    s32 n;

    switch (PU8(party, 0x38)) {
    case 0:
        party->position = *(Vec*)((u8*)mario + 0x8C);
        PF32(party, 0x10C) = revise360(PF32(party, 0x10C) + PF32(party, 0x124));
        party->scale.x += 0.05f;
        if (party->scale.x >= 1.0f) {
            party->scale.x = 1.0f;
        }
        party->scale.y = party->scale.x;
        party->scale.z = party->scale.x;
        break;
    case 10:
        PF32(party, 0x64) += PF32(party, 0x124);
        break;
    case 100:
        switch (PS8(party, 0x2E)) {
        case 0:
        case 1:
            partyChgPaper(party, "PM_D_1B");
            *(s32*)((u8*)party + 0x24) = 18;
            PU8(party, 0x38) = 101;
            break;
        case 4:
            partyChgPaper(party, "PM_D_1B");
            *(s32*)((u8*)party + 0x24) = 18;
            PU8(party, 0x38) = 101;
            break;
        }
        break;
    case 101:
        switch (PS8(party, 0x2E)) {
        case 0:
        case 1:
            party->position.z += -0.2f;
            break;
        }
        if (--*(s32*)((u8*)party + 0x24) <= 0) {
            PU8(party, 0x38) = 102;
        }
        break;
    case 102:
        party->scale = (Vec){1.0f, 1.0f, 1.0f};
        *(Vec*)((u8*)party + 0x64) = (Vec){0.0f, 0.0f, 0.0f};
        *(Vec*)((u8*)party + 0x88) = (Vec){0.0f, 0.0f, 0.0f};
        *(Vec*)((u8*)party + 0x7C) = (Vec){0.0f, 0.0f, 0.0f};
        PF32(party, 0x110) = PF32(mario, 0x1AC);
        PF32(party, 0x10C) = PF32(party, 0x110);
        PF32(party, 0x100) = PF32(mario, 0x1A0);
        PF32(party, 0xFC) = PF32(mario, 0x1A0);
        partyPaperOff(party);
        animPoseSetMaterialLightFlagOn(party->paperAnimGroup, 1);
        party->flags2 &= ~8;
        party->flags2 &= ~0x10;
        party->flags &= ~0x4000000;
        party->flags &= ~0x2000000;
        partyShadowOn(party);
        L_partyVivianTailStart(party);
        party->field_B8 = party->position;
        partyChgRunMode(party, 1);
        n = 0;
        if (partyGetPtr(marioGetPartyId())) {
            n = 1;
        }
        if (partyGetPtr(marioGetExtraPartyId())) {
            n++;
        }
        if ((s8)PU8(party, 0x2E) == 4) {
            if (n == 1) {
                party->position.z += -4.0f;
            } else if (!(party->flags & 8)) {
                party->position.z += -4.0f;
                party->position.x += 1.5f;
            } else {
                party->position.z += -3.0f;
                party->position.x += -1.5f;
            }
        }
        if ((s8)PU8(party, 0x2E) == 1) {
            if (n == 1) {
                party->position.z += -4.0f;
            } else if (!(party->flags & 8)) {
                party->position.z += -4.0f;
                party->position.x += 1.5f;
            } else {
                party->position.z += -3.0f;
                party->position.x += -1.5f;
            }
        }
        break;
    }
    return 0;
}


void partyDokanEnd(s32 id, s32 dir) {
    PartyEntry* party = partyGetPtr(id);
    if (party && party->field_34 == 10) {
        party->field_38 = 100;
        PU8(party, 0x2E) = dir;
        party->flags2 &= ~0x8;
        party->flags2 &= ~0x10;
        party->flags2 &= ~0xC0;
        party->flags &= ~0x4000000;
        party->flags &= ~0x2000000;
        partyShadowOn(party);
        party->field_B8 = party->position;
        L_partyVivianTailStart(party);
    }
}


/* @@MID2@@ */

void allPartyRidePlane(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (party) {
        bomhei_use_cancel();
        party->flags |= 0x10000;
        partyChgRunMode(party, 11);
    }
    party = partyPtrTbl[1];
    if (party) {
        party->flags |= 0x10000;
        partyChgRunMode(party, 11);
    }
}

void allPartyRideShip(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (party) {
        bomhei_use_cancel();
        party->flags |= 0x20000;
        partyChgRunMode(party, 11);
    }
    party = partyPtrTbl[1];
    if (party) {
        party->flags |= 0x20000;
        partyChgRunMode(party, 11);
    }
}


void getRidePos(PartyEntry* party, Vec* pos) {
    MarioWork* mario = party->playerPtr;
    PartyEntry* ride;
    f32 angle;
    f32 s;
    f32 c;

    if (mario->currMotionId == 0x1A) {
        angle = 3.1415927f * toMovedir(mario->unk1AC) / 180.0f;
        if (yoshiGetStatus() != 2) {
            pos->x = mario->position.x - 10.0f * (f32)sin(angle);
            pos->z = mario->position.z - 10.0f * (f32)cos(angle);
            pos->y = 20.0f + mario->position.y;
        } else {
            pos->x = mario->position.x - 20.0f * (f32)sin(angle);
            pos->z = mario->position.z - 20.0f * (f32)cos(angle);
            pos->y = 16.0f + mario->position.y;
        }
        if (party->currentSlotId) {
            ride = partyGetPtr(marioGetPartyId());
            sincosf(&s, &c, toMovedir2(mario->unk1AC, 10.0f));
            if (yoshiGetStatus() != 2) {
                pos->x = ride->position.x - 12.0f * s;
                pos->z = ride->position.z - 12.0f * c;
                pos->y = 20.0f + ride->position.y;
            } else {
                pos->x = ride->position.x - 22.0f * s;
                pos->z = ride->position.z - 22.0f * c;
                pos->y = 16.0f + ride->position.y;
            }
        }
    } else if (mario->currMotionId == 0x18) {
        pos->x = mario->position.x;
        pos->z = mario->position.z;
        pos->y = 40.0f + mario->position.y;
    } else if (mario->currMotionId == 0x19) {
        sincosf(&s, &c, toMovedirSimple(N_marioShipGetDir() ? 0.0f : 180.0f));
        pos->x = mario->position.x + 22.0f * s;
        pos->z = mario->position.z + 22.0f * c;
        pos->y = mario->position.y;
    } else {
        pos->x = mario->position.x;
        pos->z = mario->position.z;
        pos->y = 20.0f + mario->position.y;
    }
}


#pragma dont_inline on
void allPartyForceRideOn(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (party) {
        N_partyForceChgRunMode(party, 11, 11);
        party->flags2 |= 1;
        partyShadowOff(party);
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party) {
        N_partyForceChgRunMode(party, 11, 11);
        party->flags2 |= 1;
        partyShadowOff(party);
    }
}
#pragma dont_inline reset

void L_allPartyRideOn(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (party) {
        partyChgRunMode(party, 11);
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party) {
        partyChgRunMode(party, 11);
    }
}

void allPartyRideOff(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (party && party->field_34 == 11) {
        party->field_38 = 100;
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party && party->field_34 == 11) {
        party->field_38 = 100;
    }
}

void allPartyRideOff2(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (party && party->field_34 == 11) {
        party->field_38 = 101;
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party && party->field_34 == 11) {
        party->field_38 = 101;
    }
}


s32 partyRideChk(PartyEntry* party) {
    s32 ret = 0;
    if (party == NULL) {
        return 0;
    }
    if (party->field_34 == 11) {
        ret = 1;
        if (party->field_38 >= 2) {
            ret = 2;
        }
    }
    return ret;
}

/* @@MID3@@ */

f32 toMovedir(f32 a);


void partyRideMain(PartyEntry *party)
{
  f32 new_var;
  Vec pos;
  Vec s;
  f32 dy0;
  u8 *mario = (u8 *) party->playerPtr;
  switch (party->field_38)
  {
    case 0:
      getRidePos(party, &pos);
      *((Vec *) (((u8 *) party) + 0x94)) = pos;
      *((f32 *) (((u8 *) party) + 0x128)) = distABf(party->position.x, party->position.z, *((f32 *) (((u8 *) party) + 0x94)), *((f32 *) (((u8 *) party) + 0x9C)));
      *((f32 *) (((u8 *) party) + 0x100)) = angleABf(party->position.x, party->position.z, *((f32 *) (((u8 *) party) + 0x94)), *((f32 *) (((u8 *) party) + 0x9C)));
      *((s32 *) (((u8 *) party) + 0x24)) = 30;
      *((f32 *) (((u8 *) party) + 0x104)) = (*((f32 *) (((u8 *) party) + 0x128))) / ((f32) (*((s32 *) (((u8 *) party) + 0x24))));
      dy0 = (*((f32 *) (((u8 *) party) + 0x98))) - party->position.y;
      *((f32 *) (((u8 *) party) + 0x11C)) = 0.4f;
      *((f32 *) (((u8 *) party) + 0x114)) = (dy0 + (0.5f * (((f32) (*((s32 *) (((u8 *) party) + 0x24)))) * ((*((f32 *) (((u8 *) party) + 0x11C))) * ((f32) (*((s32 *) (((u8 *) party) + 0x24)))))))) / ((f32) (*((s32 *) (((u8 *) party) + 0x24))));
      *((f32 *) (((u8 *) party) + 0x124)) = 1.0f;
      party->flags2 |= 0x10;
      do
    {
      char *v6;
      BOOL v5;
      s32 poseIdx = 4;
      char *v8;
      if ((party->flags2 & 2) == 0)
      {
        v5 = 0;
        if (party->currentMemberId < 8)
        {
          if (partyGetHp(party->currentMemberId) <= 5)
          {
            v5 = 1;
          }
        }
        if (v5 == 1)
        {
          if (4 == 1)
          {
            poseIdx = 12;
          }
          else
            if (4 == 2)
          {
            poseIdx = 13;
          }
          else
            if (4 == 3)
          {
            poseIdx = 14;
          }
        }
        if (!(party->flags2 & 0x8000))
        {
          v6 = partyPoseTbl[party->currentMemberId][poseIdx];
        }
        else
          if (party->currentMemberId != PARTY_MEMBER_YOSHI)
        {
          v6 = partyDotPoseTbl[party->currentMemberId][poseIdx];
        }
        else
        {
          v6 = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][poseIdx];
        }
        v8 = party->field_18;
        if (!(v8 && (!strcmp(v8, v6))))
        {
          party->field_18 = v6;
          party->field_20 = 0;
          party->field_8 |= 0x2000u;
        }
      }
    }
    while (0);
      party->field_38 = 1;
      break;

    case 1:
    {
      f32 t;
      *((f32 *) (((u8 *) party) + 0x114)) = (*((f32 *) (((u8 *) party) + 0x114))) - (*((f32 *) (((u8 *) party) + 0x11C)));
      party->position.y = party->position.y + (*((f32 *) (((u8 *) party) + 0x114)));
      movePos(&party->position.x, &party->position.z, *((f32 *) (((u8 *) party) + 0x104)), *((f32 *) (((u8 *) party) + 0x100)));
      *((f32 *) (((u8 *) party) + 0x10C)) = revise360((*((f32 *) (((u8 *) party) + 0x10C))) + (*((f32 *) (((u8 *) party) + 0x124))));
      *((f32 *) (((u8 *) party) + 0x124)) += 2.0f;
      if ((*((f32 *) (((u8 *) party) + 0x124))) >= 30.0f)
      {
        *((f32 *) (((u8 *) party) + 0x124)) = 30.0f;
      }
      t = (f32) (*((s32 *) (((u8 *) party) + 0x24)));
      if (t > 10.0f)
      {
        t = 10.0f;
      }
      party->scale.x = t / 10.0f;
      party->scale.y = party->scale.x;
      party->scale.z = party->scale.x;
      if ((--(*((s32 *) (((u8 *) party) + 0x24)))) < 0)
      {
        party->field_38 = 2;
      }
      break;
    }

    case 2:
    {
      do
      {
        char *v6;
        BOOL v5;
        s32 poseIdx = 1;
        char *v8;
        if ((party->flags2 & 2) == 0)
        {
          v5 = 0;
          if (party->currentMemberId < 8)
          {
            if (partyGetHp(party->currentMemberId) <= 5)
            {
              v5 = 1;
            }
          }
          if (v5 == 1)
          {
            if (1 == 1)
            {
              poseIdx = 12;
            }
            else
              if (1 == 2)
            {
              poseIdx = 13;
            }
            else
              if (1 == 3)
            {
              poseIdx = 14;
            }
          }
          if (!(party->flags2 & 0x8000))
          {
            v6 = partyPoseTbl[party->currentMemberId][poseIdx];
          }
          else
            if (party->currentMemberId != PARTY_MEMBER_YOSHI)
          {
            v6 = partyDotPoseTbl[party->currentMemberId][poseIdx];
          }
          else
          {
            v6 = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][poseIdx];
          }
          v8 = party->field_18;
          if (!(v8 && (!strcmp(v8, v6))))
          {
            party->field_18 = v6;
            party->field_20 = 0;
            party->field_8 |= 0x2000u;
          }
        }
      }
      while (0);
      *((f32 *) (((u8 *) party) + 0x114)) = 0.0f;
      party->position.y = *((f32 *) (((u8 *) party) + 0x98));
      party->scale = (Vec){0.0f, 0.0f, 0.0f};
      party->flags2 |= 1;
      partyShadowOff(party);
      party->field_38 = 10;
      break;
    }

    case 10:
      party->position = *((Vec *) (mario + 0x8C));
      *((f32 *) (((u8 *) party) + 0x110)) = *((f32 *) (((u8 *) mario) + 0x1AC));
      *((f32 *) (((u8 *) party) + 0xFC)) = toMovedir(*((f32 *) (((u8 *) party) + 0x110)));
      break;

    case 11:
    {
      do
      {
        char *v6;
        BOOL v5;
        s32 poseIdx = 1;
        char *v8;
        if ((party->flags2 & 2) == 0)
        {
          v5 = 0;
          if (party->currentMemberId < 8)
          {
            if (partyGetHp(party->currentMemberId) <= 5)
            {
              v5 = 1;
            }
          }
          if (v5 == 1)
          {
            if (1 == 1)
            {
              poseIdx = 12;
            }
            else
              if (1 == 2)
            {
              poseIdx = 13;
            }
            else
              if (1 == 3)
            {
              poseIdx = 14;
            }
          }
          if (!(party->flags2 & 0x8000))
          {
            v6 = partyPoseTbl[party->currentMemberId][poseIdx];
          }
          else
            if (party->currentMemberId != PARTY_MEMBER_YOSHI)
          {
            v6 = partyDotPoseTbl[party->currentMemberId][poseIdx];
          }
          else
          {
            v6 = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][poseIdx];
          }
          v8 = party->field_18;
          if (!(v8 && (!strcmp(v8, v6))))
          {
            party->field_18 = v6;
            party->field_20 = 0;
            party->field_8 |= 0x2000u;
          }
        }
      }
      while (0);
      party->flags2 |= 1;
      partyShadowOff(party);
      getRidePos(party, &pos);
      party->scale = (Vec){0.0f, 0.0f, 0.0f};
      party->position = pos;
      *((f32 *) (((u8 *) party) + 0x114)) = 0.0f;
      *((f32 *) (((u8 *) party) + 0x110)) = *((f32 *) (((u8 *) ((u8 *) party->playerPtr)) + 0x1AC));
      *((f32 *) (((u8 *) party) + 0xFC)) = toMovedir(*((f32 *) (((u8 *) party) + 0x110)));
      break;
    }

    case 20:
      party->field_38 = 21;
      *((s32 *) (((u8 *) party) + 0x24)) = 16;
      *((f32 *) (((u8 *) party) + 0x114)) = 16.0f;
      *((f32 *) (((u8 *) party) + 0x174)) = 0.5f;

    case 21:
      party->position.y = party->position.y + (*((f32 *) (((u8 *) party) + 0x114)));
      *((f32 *) (((u8 *) party) + 0x114)) = (*((f32 *) (((u8 *) party) + 0x114))) + (*((f32 *) (((u8 *) party) + 0x174)));
      if ((--(*((s32 *) (((u8 *) party) + 0x24)))) > 0)
    {
      break;
    }
      party->field_38 = 22;
      party->flags |= 0x2000000;
      break;

    case 22:
      break;

    case 200:
      party->flags &= ~0x70000;
      *((f32 *) (((u8 *) party) + 0x80)) = 0.0f;
      party->scale.x = 1.0f;
      party->scale.y = party->scale.x;
      party->scale.z = party->scale.x;
      partyShadowOn(party);
      partyChgRunMode(party, 1);
      break;

    case 100:

    case 101:
    {
      f32 x;
      f32 z;
      f32 h;
      f32 dy;
      party->flags2 &= ~1;
      x = *((f32 *) (((u8 *) mario) + 0x8C));
      z = *((f32 *) (((u8 *) mario) + 0x94));
      new_var = *((f32 *) (((u8 *) mario) + 0x90));
      party->position.x = x;
      party->position.y = new_var + (0.5f * (*((f32 *) (((u8 *) mario) + 0x1B8))));
      party->position.z = z;
      if (party->field_38 == 100)
      {
        partyGetAppearPos(party, (Vec *) (((u8 *) party) + 0x94));
      }
      else
      {
        partyGetAppearPos2(party, (Vec *) (((u8 *) party) + 0x94));
      }
      *((f32 *) (((u8 *) party) + 0x110)) = *((f32 *) (((u8 *) mario) + 0x1AC));
      *((s32 *) (((u8 *) party) + 0x24)) = 16;
      *((f32 *) (((u8 *) party) + 0x128)) = distABf(x, z, *((f32 *) (((u8 *) party) + 0x94)), *((f32 *) (((u8 *) party) + 0x9C)));
      *((f32 *) (((u8 *) party) + 0x100)) = angleABf(x, z, *((f32 *) (((u8 *) party) + 0x94)), *((f32 *) (((u8 *) party) + 0x9C)));
      *((f32 *) (((u8 *) party) + 0x104)) = (*((f32 *) (((u8 *) party) + 0x128))) / ((f32) (*((s32 *) (((u8 *) party) + 0x24))));
      h = 0.5f * ((f32) (*((s32 *) (((u8 *) party) + 0x24))));
      dy = (*((f32 *) (((u8 *) party) + 0x98))) - party->position.y;
      if (__fabsf(dy) < 1.0f)
      {
        dy = 10.0f;
      }
      else
        if (dy >= 0.0f)
      {
        dy = (0.5f * (*((f32 *) (((u8 *) party) + 0xF0)))) + dy;
      }
      else
      {
        dy = 10.0f;
      }
      dy = (dy + dy) / ((h * h) - h);
      *((f32 *) (((u8 *) party) + 0x114)) = dy * h;
      *((f32 *) (((u8 *) party) + 0x118)) = -dy;
      party->flags |= 0x10;
      party->flags2 &= ~1;
      partyShadowOn(party);
      party->field_38 = 110;
      break;
    }

    case 110:
      partyMoveNoHosei(party);
      if ((*((f32 *) (((u8 *) party) + 0x114))) <= 0.0f)
    {
      party->flags &= ~0x10;
      *((f32 *) (((u8 *) party) + 0x118)) = -1.6f;
    }
      if (party->position.y < (-2000.0f))
    {
      *((s32 *) (((u8 *) party) + 0x24)) = 0;
      party->field_38 = 111;
      break;
    }
      if ((*((u32 *) (((u8 *) party) + 0x138))) == 0)
    {
      s = (Vec){0.0f, 0.0f, 0.0f};
      s.x = (s.y = (s.z = ((f32) (16 - (*((s32 *) (((u8 *) party) + 0x24))))) * 0.0625f));
      party->scale = s;
      if ((--(*((s32 *) (((u8 *) party) + 0x24)))) <= 0)
      {
        *((s32 *) (((u8 *) party) + 0x24)) = 0;
      }
      break;
    }
      party->field_38 = 111;

    case 111:
    {
      s = (Vec){0.0f, 0.0f, 0.0f};
      s.x = (s.y = (s.z = ((f32) (16 - (*((s32 *) (((u8 *) party) + 0x24))))) * 0.0625f));
      party->scale = s;
      if ((--(*((s32 *) (((u8 *) party) + 0x24)))) > 0)
      {
        break;
      }
      *((s32 *) (((u8 *) party) + 0x24)) = 0;
      *((f32 *) (((u8 *) party) + 0x110)) = *((f32 *) (((u8 *) mario) + 0x1AC));
      *((f32 *) (((u8 *) party) + 0x10C)) = *((f32 *) (((u8 *) mario) + 0x1AC));
      *((f32 *) (((u8 *) party) + 0xFC)) = toMovedirSimple(*((f32 *) (((u8 *) party) + 0x10C)));
      *((f32 *) (((u8 *) party) + 0x100)) = *((f32 *) (((u8 *) party) + 0xFC));
      partyClearJumpPara(party);
      partyChgPoseId_inl(party, 1);
      party->flags2 &= ~0x10;
      party->flags &= ~0x70000;
      *((f32 *) (((u8 *) party) + 0x80)) = 0.0f;
      party->scale.x = 1.0f;
      party->scale.y = party->scale.x;
      party->scale.z = party->scale.x;
      party->flags2 &= ~1;
      partyShadowOn(party);
      partyChgRunMode(party, 1);
      break;
    }

  }

  party->field_B8 = party->position;
}
static inline void partyChgRunMo(PartyEntry* party) {
    party->scale = (Vec){1.0f, 1.0f, 1.0f};
    *(Vec*)((u8*)party + 0x7C) = (Vec){0.0f, 0.0f, 0.0f};
}


void party_force_ride_yoshi(PartyEntry* party) {
    Vec pos;

    N_partyForceChgRunMode(party, 12, 10);
    party->flags &= ~0x100;
    getRidePos(party, &pos);
    party->position = pos;
    partyChgPoseId(party, 1);
    PF32(party, 0x114) = 0.0f;
    partyShadowOff(party);
    party->field_110 = party->playerPtr->unk1AC;
    party->field_FC = toMovedir(party->field_110);
}


void party_ride_yoshi_force_move(void) {
    Vec pos;
    PartyEntry* party = partyGetPtr(marioGetExtraPartyId());
    if (party) {
        getRidePos(party, &pos);
        party->position = pos;
        party->field_110 = party->playerPtr->unk1AC;
        party->field_FC = toMovedir(party->field_110);
    }
}

void partyGetAppearPos(PartyEntry* party, Vec* pos);

s32 partyRideYoshiMain(PartyEntry* party) {
    Vec pos;
    Vec pos2;
    MarioWork* mario = party->playerPtr;

    switch (party->field_38) {
    case 0:
        PF32(party, 0x11C) = 1.6f;
        getRidePos(party, &pos);
        *(Vec*)((u8*)party + 0x94) = pos;
        PF32(party, 0x128) = distABf(party->position.x, party->position.z, PF32(party, 0x94), PF32(party, 0x9C));
        PF32(party, 0x100) = angleABf(party->position.x, party->position.z, PF32(party, 0x94), PF32(party, 0x9C));
        *(s32*)((u8*)party + 0x24) = 15;
        PF32(party, 0x104) = PF32(party, 0x128) / (f32)*(s32*)((u8*)party + 0x24);
        PF32(party, 0x114) = (0.5f * ((f32)*(s32*)((u8*)party + 0x24) * (PF32(party, 0x11C) * (f32)*(s32*)((u8*)party + 0x24))) + (PF32(party, 0x98) - party->position.y)) / (f32)*(s32*)((u8*)party + 0x24);
        partyChgPoseId_inl(party, 4);
        party->field_38 = 1;
        break;
    case 1:
        getRidePos(party, &pos);
        *(Vec*)((u8*)party + 0x94) = pos;
        PF32(party, 0x128) = distABf(party->position.x, party->position.z, PF32(party, 0x94), PF32(party, 0x9C));
        PF32(party, 0x100) = angleABf(party->position.x, party->position.z, PF32(party, 0x94), PF32(party, 0x9C));
        PF32(party, 0x104) = PF32(party, 0x128) / (f32)*(s32*)((u8*)party + 0x24);
        PF32(party, 0x114) -= PF32(party, 0x11C);
        party->position.y += PF32(party, 0x114);
        movePos(&party->position.x, &party->position.z, PF32(party, 0x104), PF32(party, 0x100));
        if (--*(s32*)((u8*)party + 0x24) <= 0) {
            party->field_38 = 2;
        }
        break;
    case 2:
        partyChgPoseId_inl(party, 1);
        PF32(party, 0x114) = 0.0f;
        party->position.y = PF32(party, 0x98);
        party->field_38 = 10;
        if (party) {
            party->flags2 |= 0x100000;
        }
        break;
    case 10:
        getRidePos(party, &pos);
        party->position = pos;
        party->field_110 = mario->unk1AC;
        party->field_FC = toMovedir(party->field_110);
        if (*(u16*)((u8*)mario + 0x2E) != 0x1A) {
            partyGetAppearPos(party, &pos2);
            party->position.x = pos2.x;
            party->position.z = pos2.z;
            partyChgRunMode(party, 2);
            if (party) {
                party->flags2 &= ~0x100000;
            }
            *(Vec*)((u8*)party + 0x7C) = (Vec){0.0f, 0.0f, 0.0f};
        }
        break;
    }
    return 0;
}


void marioGetScreenPos(Vec position, f32* x, f32* y, f32* z);
s32 hitGetAttr(struct HitEntry* hit);
s32 sysMsec2Frame(s32 msec);
BOOL marioWaitDamageReturnFall(void);
BOOL marioWaitDamageTogeReturnFall(void);
void vivianShadow(PartyEntry* party);
void partyMove(PartyEntry* party);

void party_damage_return(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    f32 x, y, z;
    f32 mx, my, mz;
    f32 lim;
    Vec pos;

    if (party->field_8 & 8) {
        party->field_8 &= ~8;
        party->flags &= ~0x100;
        if (!(party->field_8 & 0x1000000)) {
            party->field_38 = 10;
        } else {
            party->field_38 = 0;
            *(s32*)((u8*)party + 0x24) = 100;
        }
    }
    switch (party->field_38) {
    case 0:
        if (--*(s32*)((u8*)party + 0x24) > 0) {
            break;
        }
        party->field_38 = 10;
    case 10:
        partyChgPoseId_inl(party, 8);
        *(s32*)((u8*)party + 0x24) = sysMsec2Frame(200);
        party->field_38 = 11;
    case 11:
        if (--*(s32*)((u8*)party + 0x24) > 0) {
            break;
        }
        PF32(party, 0x114) = 15.0f;
        PF32(party, 0x118) = 0.0f;
        party->field_38 = 12;
    case 12:
        party->position.y += PF32(party, 0x114);
        marioGetScreenPos(party->position, &x, &y, &z);
        marioGetScreenPos(mario->position, &mx, &my, &mz);
        lim = (my < -100.0f) ? my : -100.0f;
        if (y <= lim) {
            *(s32*)((u8*)party + 0x24) = 32;
            party->field_38 = 20;
        }
        break;
    case 20:
        marioGetScreenPos(party->position, &x, &y, &z);
        if (y <= -100.0f) {
            party->position.y += 1.0f;
        }
        if (mario->currMotionId == 0x1F) {
            if (!marioWaitDamageReturnFall()) {
                break;
            }
        } else if (mario->currMotionId == 0x20) {
            if (!marioWaitDamageTogeReturnFall()) {
                break;
            }
        }
        party->field_38 = 21;
    case 21:
        if (--*(s32*)((u8*)party + 0x24) > 0) {
            break;
        }
        partyGetAppearPos(party, &pos);
        party->position.x = pos.x;
        party->position.z = pos.z;
        PF32(party, 0x114) = 0.0f;
        PF32(party, 0x118) = -1.0f;
        PF32(party, 0x104) = 0.0f;
        party->flags |= 0x20;
        party->flags |= 0x2000000;
        party->field_38 = 22;
        break;
    case 22:
        marioGetScreenPos(party->position, &x, &y, &z);
        if (y > 240.0f) {
            party->flags &= ~0x2000000;
        }
        partyMove(party);
        if (PF32(party, 0x114) <= -6.0f) {
            PF32(party, 0x118) = 0.0f;
            PF32(party, 0x114) = -6.0f;
        }
        if (party->position.y < -2000.0f) {
            party->position = mario->position;
            party->position.y += 200.0f;
            party->flags &= ~0x70;
        }
        if (*(struct HitEntry**)((u8*)party + 0x138) && !((hitGetAttr(*(struct HitEntry**)((u8*)party + 0x138)) >> 9) & 1)) {
            partyChgPoseId_inl(party, 9);
            party->field_38 = 23;
            *(s32*)((u8*)party + 0x24) = 30;
        }
        break;
    case 23:
        if (--*(s32*)((u8*)party + 0x24) > 0) {
            break;
        }
        partyChgRunMode(party, 2);
        PU8(party, 0x3B) = 0;
        party->flags2 &= ~0x10;
        break;
    }
    if (vivianGetStatus() != 2) {
        vivianShadow(party);
    }
}


void partyInitCamId(PartyEntry* party) {
    party->camId = marioGetCamId();
}

/*
//.data
typedef struct PartyData {
        char* name; //0, TODO: modelname? animname?
        void* init_func; //1
        void* move_func; //2
        void* use_func; //3
        void* post_func; //4
        void* bye_func; //5
        void* unk_6_func; //6
        void* unk_7_func; //7
        void* unk_8_func; //8
        void* exit_func; //9
} PartyData;

PartyData partyDataTbl[] = { //change to [20]
        {"c_pkr", kuribo_init, partyMoveWalk, kuribo_use, kuribo_use_post, kuribo_bye, NULL, NULL, NULL, NULL},
        {"c_pnk", nokonoko_init, nokonoko_move, nokonoko_use, nokonoko_use_post, nokonoko_bye, NULL, NULL, NULL, NULL},
        {"c_bomt_n", bomhei_init, partyMoveWalk, bomhei_use, bomhei_use_post, bomhei_bye, NULL, NULL, NULL,
bomhei_exit},
        {"c_babyyoshi", yoshi_init, partyMoveWalk, yoshi_use, NULL, yoshi_bye, NULL, NULL, NULL, yoshi_exit},
        {"c_windy", cloud_init, cloud_move, cloud_use, NULL, kuribo_bye, NULL, NULL, NULL, cloud_exit},
        {"c_vivian", vivian_init, vivian_move, vivian_use, NULL, kuribo_bye, NULL, NULL, NULL, NULL},
        {"c_tyutyu", chuchu_init, chuchu_move, chuchu_use, NULL, kuribo_bye, NULL, NULL, NULL, chuchu_exit},
        {"c_babyyoshi", kuribo_init, partyMoveWalk, kuribo_use, NULL, kuribo_bye, NULL, NULL, NULL, NULL},
        {"c_marco", kuribo_init, partyMoveWalk, kuribo_use, NULL, kuribo_bye, NULL, NULL, NULL, NULL},
        {"c_pguide", kuribo_init, partyMoveWalk, kuribo_use, NULL, kuribo_bye, NULL, NULL, NULL, NULL},
        {"c_kuri_h", kuribo_init, partyMoveWalk, kuribo_use, NULL, kuribo_bye, NULL, NULL, NULL, NULL},
        {"c_monban", kuribo_init, partyMoveWalk, kuribo_use, NULL, kuribo_bye, NULL, NULL, NULL, NULL},
        {"c_pkr", kuribo_init, partyMoveWalk, kuribo_use, NULL, kuribo_bye, NULL, NULL, NULL, NULL},
        {"c_pnk", nokonoko_init, nokonoko_move, nokonoko_use, NULL, nokonoko_bye, NULL, NULL, NULL, NULL},
        {"c_bomt_n", bomhei_init, partyMoveWalk, bomhei_use, bomhei_use_post, bomhei_bye, NULL, NULL, NULL, NULL},
        {"c_babyyoshi", yoshi_init, partyMoveWalk, yoshi_use, NULL, yoshi_bye, NULL, NULL, NULL, NULL},
        {"c_windy", cloud_init, cloud_move, cloud_use, NULL, kuribo_bye, NULL, NULL, NULL, cloud_exit},
        {"c_vivian", vivian_init, vivian_move, vivian_use, NULL, kuribo_bye, NULL, NULL, NULL, NULL},
        {"c_tyutyu", chuchu_init, partyMoveWalk, chuchu_use, NULL, kuribo_bye, NULL, NULL, NULL, NULL},
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
};

char* partyPoseTbl[20][15] = {
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
        {"PKR_Z_1", "PKR_S_1", "PKR_W_1", "PKR_R_1", "PKR_J_1A", "PKR_J_1B", "PKR_N_1", "PKR_T_1", "PKR_D_1", "PKR_D_4",
"PKR_S_1", "PKR_S_1", "PKR_D_2", "PKR_W_2", "PKR_R_2"},
        {"PNK_Z_1", "PNK_S_1", "PNK_W_1", "PNK_R_1", "PNK_J_1A", "PNK_J_1B", "PNK_N_1", "PNK_T_1", "PNK_D_1", "PNK_D_4",
"PNK_S_1", "PNK_S_1", "PNK_D_2", "PNK_W_2", "PNK_R_2"},
        {"Z_1", "S_1", "W_1", "R_1", "J_1A", "J_1B", "N_1", "T_1", "D_1", "F_1", "S_1", "S_1", "D_2", "W_3", "R_3"},
        {"PYS_Z_1", "PYS_S_1", "PYS_W_1", "PYS_R_1", "PYS_J_1A", "PYS_J_1B", "PYS_N_1", "PYS_T_1", "PYS_D_1", "PYS_D_4",
"PYS_S_1", "PYS_S_1", "PYS_D_2", "PYS_W_3", "PYS_R_3"},
        {"PWD_Z_1", "PWD_S_1", "PWD_W_1", "PWD_R_1", "PWD_S_1", "PWD_S_1", "PWD_N_1", "PWD_T_1", "PWD_D_1", "PWD_F_1",
"PWD_S_1", "PWD_S_1", "PWD_D_2", "PWD_W_2", "PWD_R_2"},
        {"PTR_Z_1", "PTR_S_1", "PTR_W_1", "PTR_R_1", "PTR_S_1", "PTR_S_1", "PTR_N_1", "PTR_T_1", "PTR_D_1", "PTR_F_1",
"PTR_Z_1", "PTR_Z_1", "PTR_D_2", "PTR_W_2", "PTR_R_2"},
        {"PCH_Z_1", "PCH_S_1", "PCH_W_1", "PCH_R_1", "PCH_J_1A", "PCH_J_1B", "PCH_N_1", "PCH_T_1", "PCH_D_1", "PCH_F_1",
"PCH_S_1", "PCH_S_1", "PCH_D_2", "PCH_R_2", "PCH_W_2"},
        {"PYS_E_1", "PYS_E_2", "PYS_E_3", "PYS_E_3", "PYS_E_2", "PYS_E_2", "PYS_E_2", "PYS_E_2", "PYS_E_2", "PYS_E_2",
"PYS_E_2", "PYS_E_2", "PYS_E_2", "PYS_E_3", "PYS_E_3"},
        {"Z_1", "S_1", "W_1", "R_1", "J_1A", "J_1B", "S_1", "T_1", "D_1", "D_1", "Z_1", "Z_1", "S_1", "W_1", "R_1"},
        {"Z_1", "S_1", "W_1", "R_1", "J_1A", "J_1B", "S_1", "T_1", "Z_1", "Z_1", "S_1", "Z_1", "Z_1", "W_1", "R_1"},
        {"Z_1", "S_1", "W_1", "R_1", "W_1", "W_1", "S_1", "T_1", "Z_1", "Z_1", "S_1", "Z_1", "Z_1", "W_1", "R_1"},
        {"Z_1", "S_1", "W_1", "R_1", "S_1", "S_1", "S_1", "T_1", "S_1", "S_1", "S_1", "S_1", "S_1", "W_1", "R_1"},
        {"PKR_Z_1", "PKR_S_1", "PKR_W_1", "PKR_R_1", "PKR_J_1A", "PKR_J_1B", "PKR_N_1", "PKR_T_1", "PKR_D_1", "PKR_D_4",
"PKR_S_1", "PKR_S_1", "PKR_D_2", "PKR_W_2", "PKR_R_2"},
        {"PNK_Z_1", "PNK_S_1", "PNK_W_1", "PNK_R_1", "PNK_J_1A", "PNK_J_1B", "PNK_N_1", "PNK_T_1", "PNK_D_1", "PNK_D_4",
"PNK_S_1", "PNK_S_1", "PNK_D_2", "PNK_W_2", "PNK_R_2"},
        {"Z_1", "S_1", "W_1", "R_1", "J_1A", "J_1B", "N_1", "T_1", "D_1", "F_1", "S_1", "S_1", "D_2", "W_3", "R_3"},
        {"PYS_Z_1", "PYS_S_1", "PYS_W_1", "PYS_R_1", "PYS_J_1A", "PYS_J_1B", "PYS_N_1", "PYS_T_1", "PYS_D_1", "PYS_D_4",
"PYS_S_1", "PYS_S_1", "PYS_D_2", "PYS_W_2", "PYS_R_2"},
        {"PWD_Z_1", "PWD_S_1", "PWD_W_1", "PWD_R_1", "PWD_S_1", "PWD_S_1", "PWD_N_1", "PWD_T_1", "PWD_D_1", "PWD_F_1",
"PWD_S_1", "PWD_S_1", "PWD_D_2", "PWD_W_2", "PWD_R_2"},
        {"PTR_Z_1", "PTR_S_1", "PTR_W_1", "PTR_R_1", "PTR_S_1", "PTR_S_1", "PTR_N_1", "PTR_T_1", "PTR_D_1", "PTR_F_1",
"PTR_Z_1", "PTR_Z_1", "PTR_D_2", "PTR_W_2", "PTR_R_2"},
        {"PCH_Z_1", "PCH_S_1", "PCH_W_1", "PCH_R_1", "PCH_J_1A", "PCH_J_1B", "PCH_N_1", "PCH_T_1", "PCH_D_1", "PCH_F_1",
"PCH_S_1", "PCH_S_1", "PCH_D_2", "PCH_R_2", "PCH_W_2"}
};

char* partyDotPoseTbl[8][15] = {
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
        {"Z_2", "Z_2", "W_2", "R_2", "Z_2", "Z_2", "Z_2", "Z_2", "Z_2", "Z_2", "Z_2", "Z_2", "Z_2", "W_2", "R_2"},
        {"Z_3", "Z_3", "W_3", "R_3", "Z_3", "Z_3", "Z_3", "Z_3", "Z_3", "Z_3", "Z_3", "Z_3", "Z_3", "W_3", "R_3"},
        {"Z_13", "Z_13", "W_13", "R_13", "Z_13", "Z_13", "Z_13", "Z_13", "Z_13", "Z_13", "Z_13", "Z_13", "Z_13", "W_13",
"R_13"},
        {"Z_5", "Z_5", "W_5", "R_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "W_5", "R_5"},
        {"Z_4", "Z_4", "W_4", "R_4", "Z_4", "Z_4", "Z_4", "Z_4", "Z_4", "Z_4", "Z_4", "Z_4", "Z_4", "W_4", "R_4"},
        {"Z_12", "Z_12", "W_12", "R_12", "Z_12", "Z_12", "Z_12", "Z_12", "Z_12", "Z_12", "Z_12", "Z_12", "Z_12", "W_12",
"R_12"},
        {"Z_14", "Z_14", "W_14", "R_14", "Z_14", "Z_14", "Z_14", "Z_14", "Z_14", "Z_14", "Z_14", "Z_14", "Z_14", "W_14",
"R_14"}
};

char* yoshiDotPoseTbl[7][15] = {
        {"Z_5", "Z_5", "W_5", "R_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "W_5", "R_5"},
        {"Z_6", "Z_6", "W_6", "R_6", "Z_6", "Z_6", "Z_6", "Z_6", "Z_6", "Z_6", "Z_6", "Z_6", "Z_6", "W_6", "R_6"},
        {"Z_7", "Z_7", "W_7", "R_7", "Z_7", "Z_7", "Z_7", "Z_7", "Z_7", "Z_7", "Z_7", "Z_7", "Z_7", "W_7", "R_7"},
        {"Z_8", "Z_8", "W_8", "R_8", "Z_8", "Z_8", "Z_8", "Z_8", "Z_8", "Z_8", "Z_8", "Z_8", "Z_8", "W_8", "R_8"},
        {"Z_9", "Z_9", "W_9", "R_9", "Z_9", "Z_9", "Z_9", "Z_9", "Z_9", "Z_9", "Z_9", "Z_9", "Z_9", "W_9", "R_9"},
        {"Z_10", "Z_10", "W_10", "R_10", "Z_10", "Z_10", "Z_10", "Z_10", "Z_10", "Z_10", "Z_10", "Z_10", "Z_10", "W_10",
"R_10"},
        {"Z_11", "Z_11", "W_11", "R_11", "Z_11", "Z_11", "Z_11", "Z_11", "Z_11", "Z_11", "Z_11", "Z_11", "Z_11", "W_11",
"R_11"}
};

//.sbss
PartyEntry* partyPtrTbl[2];
PartyData* party_dp;
s32 partyCtrlNo;

void partyChgRunMode(PartyEntry* party, s32 mode);
void partyChgMot(PartyEntry* party, s32 mot);
s32 partyEntryMain(s32 memberId);
void partyGetAppearPos2(PartyEntry* party, Vec* pos);

//.sdata

PartyEntry* partyGetPtr(s32 id) {
        if (id >= 0) {
                return partyPtrTbl[id];
        }
        return NULL;
}

PartyEntry* anotherPartyGetPtr(s32 id) {
        if (id < 0) {
                return NULL;
        }
        else {
                return partyPtrTbl[(id + 1) & 1];
        }
}

BOOL partyPaperOn(PartyEntry* entry, char* anim) {
    BOOL v4 = FALSE;
    BOOL v5;
    if (entry->paperPoseId >= 0) {
        animPaperPoseRelease(entry->paperPoseId);
        entry->paperPoseId = -1;
    }
    entry->flags2 |= 0x4000000;
    entry->paperPoseId = animPaperPoseEntry(anim, 2);
    if (!strcmp(anim, "p_roll") || !strcmp(anim, "p_plane") || !strcmp(anim, "p_dokan_x") ||
        !strcmp(anim, "p_dokan_y")) {
        v4 = TRUE;
    }
    v5 = !v4;
    if (!strcmp(anim, "p_slit")) {
        v5 = entry->currentMemberId == PARTY_MEMBER_FLURRIE;
    }
    animPoseSetPaperAnimGroup(entry->paperAnimGroup, anim, v5);
    return TRUE;
}







s32 partyEntry2(s32 memberId) {
        PartyEntry* entry;
        MarioWork* player;
        char* animName;
        s32 slotId;

        player = marioGetPtr();
        if (partyPtrTbl[1]) {
                slotId = -1;
        }
        else {
                partyPtrTbl[1] = (PartyEntry*)__memAlloc(HEAP_DEFAULT, sizeof(PartyEntry));
                memset(partyPtrTbl[1], 0, sizeof(PartyEntry));
                slotId = 1;
        }
        if (slotId < 0) {
                return -1;
        }
        entry = partyPtrTbl[slotId];
        memset(entry, 0, sizeof(PartyEntry));
        entry->camId = marioGetCamId();
        entry->field_0xC = -1;
        entry->field_0x10 = -1;
        entry->field_0x14 = -1;
        entry->flags = 9;
        entry->currentSlotId = (s8)slotId; //TODO: re-type?
        entry->currentMemberId = (PartyMember)memberId;
        entry->flags |= 4;
        party_dp = &partyDataTbl[memberId];
        if (entry->currentMemberId == PARTY_MEMBER_YOSHI) {
                animName = yoshigroup[pouchGetPartyColor(PARTNER_YOSHI)];
                if ((entry->flags2 & 0x8000)) {
                        animName = "d_mario";
                }
        } else if ((entry->flags2 & 0x8000)) {
                animName = "d_mario";
        }
        else {
                animName = partyDataTbl[memberId].name;
        }
        if (entry->field_0xC >= 0) {
                animPoseRelease(entry->field_0xC);
        }
        entry->field_0xC = animPoseEntry(animName, 2);






        return -1;
}
*/

void partySetCamId(PartyEntry* party, s32 cameraId) {
    party->camId = cameraId;
}

/* @@DISP@@ */
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#define PS16(p, o) (*(s16*)((u8*)(p) + (o)))
s32 shadowEntryMode(s32 mode, f32 x, f32 y, f32 z, f32 size);
void shadowSetCamId(s32 id, s32 camId);
void shadowFlagOn(s32 id, u32 flag);
BOOL N_vivianDisplayTailCheck(PartyEntry* party);
void animPoseVivianMain(s32 poseId, Vec* pos);
struct HitEntry* partyHitCheck(PartyEntry* party, Vec* pos, Vec* dir, Vec* outPos, Vec* outNrm, f32* dist);
void partyDisp(CameraId cam, void* param);

void partyMakeDispDir(PartyEntry* party) {
    f32 diff;
    f32 tgt;
    f32 cur;
    f32 step;

    if (party->flags2 & 0x100) {
        return;
    }
    if (!(party->flags2 & 0x10)) {
        diff = revise360(PF32(party, 0xFC) - PF32(party, 0xF8));
        tgt = PF32(party, 0x110);
        if (!(party->flags2 & 0x100)) {
            if (290.0f == tgt || 250.0f == tgt) {
                tgt = 180.0f;
                if (diff >= tgt) {
                    tgt = 0.0f;
                }
            }
        }
        if (PF32(party, 0x104)) {
            if (diff >= 190.0f && diff <= 350.0f) {
                tgt = 0.0f;
            } else if (diff >= 10.0f && diff <= 170.0f) {
                tgt = 180.0f;
            }
        }
        PF32(party, 0x110) = tgt;
        if (party->flags2 & 0x20) {
            PF32(party, 0x10C) = PF32(party, 0x110);
        }
        if (PF32(party, 0x10C) == PF32(party, 0x110)) {
            return;
        }
    }
    if ((cur = PF32(party, 0x10C)) <= 180.0f) {
        if (0.0f == PF32(party, 0x110)) {
            step = 0.125f * (((party->field_8 & 0x1000) ? 20.0f : -40.0f) - PF32(party, 0x10C));
            PF32(party, 0x10C) += step;
            if (!(party->field_8 & 0x1000)) {
                if (PF32(party, 0x10C) < -30.0f) {
                    party->field_8 |= 0x1000;
                }
            } else if (PF32(party, 0x10C) >= 0.0f) {
                PF32(party, 0x10C) = 0.0f;
                party->field_8 &= ~0x1000;
            }
        } else if (180.0f == PF32(party, 0x110)) {
            step = 0.125f * (((party->field_8 & 0x1000) ? 170.0f : 220.0f) - PF32(party, 0x10C));
            PF32(party, 0x10C) += step;
            if (!(party->field_8 & 0x1000)) {
                if (PF32(party, 0x10C) > 210.0f) {
                    party->field_8 |= 0x1000;
                }
            } else if (PF32(party, 0x10C) <= 180.0f) {
                PF32(party, 0x10C) = 180.0f;
                party->field_8 &= ~0x1000;
            }
        }
    } else if (cur > 180.0f) {
        if (0.0f == PF32(party, 0x110)) {
            step = 0.125f * (((party->field_8 & 0x1000) ? 20.0f : -40.0f) - PF32(party, 0x10C));
            PF32(party, 0x10C) += step;
            if (!(party->field_8 & 0x1000)) {
                if (PF32(party, 0x10C) < -30.0f) {
                    party->field_8 |= 0x1000;
                }
            } else if (PF32(party, 0x10C) >= 0.0f) {
                PF32(party, 0x10C) = 0.0f;
                party->field_8 &= ~0x1000;
            }
        } else if (180.0f == PF32(party, 0x110)) {
            step = 0.125f * (((party->field_8 & 0x1000) ? 170.0f : 220.0f) - PF32(party, 0x10C));
            PF32(party, 0x10C) += step;
            if (!(party->field_8 & 0x1000)) {
                if (PF32(party, 0x10C) > 210.0f) {
                    party->field_8 |= 0x1000;
                } else if (PF32(party, 0x10C) <= 180.0f) {
                    PF32(party, 0x10C) = 180.0f;
                    party->field_8 &= ~0x1000;
                }
            }
        }
    }
}

void partyPreDisp(PartyEntry* party) {
    Vec screen;
    f32 z;
    f32 size;
    s32 mode;
    s32 v;

    if (!(party->flags2 & 0x10) && !(party->flags & 0x200)) {
        partyMakeDispDir(party);
    }
    if (party->field_8 & 0x6000) {
        if (party->field_8 & 0x2000) {
            party->field_8 &= ~0x2000;
        }
        if (party->field_8 & 0x4000) {
            party->field_8 &= ~0x4000;
        }
        animPoseSetAnim(party->paperAnimGroup, party->field_18, 1);
        if (party->field_1C) {
            animPoseSetPaperAnim(party->paperAnimGroup, (const char*)party->field_1C, 1);
        }
        animPoseSetLocalTime(party->paperAnimGroup, party->field_20);
    }
    animPoseMain(party->paperAnimGroup);
    if (party->flags2 & 1) {
        return;
    }
    screen = party->position;
    PSMTX44MultVec(camGetPtr((CameraId)4)->projection, &screen, &screen);
    z = 5000.0f * screen.z + 5000.0f;
    if (z < 0.0f) {
        z = 0.0f;
    } else if (z > 10000.0f) {
        z = 10000.0f;
    }
    z *= -1.0f;
    v = PS16(party, 0x2A);
    if (v > 0) {
        if (!(v & 2)) {
            dispEntry((CameraId)party->camId, z, 1, partyDisp, party);
        }
    } else {
        dispEntry((CameraId)party->camId, z, 1, partyDisp, party);
    }
    mode = 2;
    size = 13.333333f;
    if ((s8)PU8(party, 0x31) != 6) {
        party->field_B8 = party->position;
    } else {
        if (party->field_34 == 4 || (party->field_34 == 8 && !(party->flags & 0x100))) {
            party->field_B8 = party->position;
        }
        if (party->flags & 0x100) {
            size = PF32(party, 0x174);
            if (party->flags & 0x80000000) {
                mode = 1;
            }
        }
    }
    if (N_vivianDisplayTailCheck(party)) {
        Vec pos;
        Vec dir;
        Vec hitPos;
        Vec hitNrm;
        f32 dist;

        dist = 1000.0f;
        pos = party->position;
        pos.y += 10.0f;
        dir = (Vec){0.0f, -1.0f, 0.0f};
        party->field_B8.y = partyHitCheck(party, &pos, &dir, &hitPos, &hitNrm, &dist) ? hitPos.y : party->position.y;
        animPoseVivianMain(party->paperAnimGroup, &party->field_B8);
    }
    if (!(party->flags2 & 0x100000)) {
        mode = shadowEntryMode(mode, party->field_B8.x, party->field_B8.y, party->field_B8.z, size);
        if (party->camId != 4) {
            shadowSetCamId(mode, party->camId);
        }
        if (party->flags2 & 0x200000) {
            shadowFlagOn(mode, 4);
        }
    }
}

void partyDisp(CameraId cam, void* param) {
    PartyEntry* party = param;
    Mtx m;
    Mtx t;
    Mtx s;
    f32 scale;
    f32 dir;
    f32 yoff;
    BOOL flip;

    if (!partyCountActive()) {
        return;
    }
    if (party->flags & 0x200) {
        PF32(party, 0x88) = -6.8f;
        PF32(party, 0x8C) = 6.8f;
    }
    scale = (party->flags & 0x800000) ? 1.2f : 2.0f;
    PSMTXScale(m, party->scale.x * scale, party->scale.y * scale, party->scale.z * scale);
    if (!(party->flags2 & 8)) {
        dir = revise360(PF32(party, 0x10C));
        flip = FALSE;
        if (dir >= 90.0f && dir <= 270.0f) {
            flip = TRUE;
        }
        PSMTXScale(s, 1.0f, 1.0f, flip ? -1.0f : 1.0f);
        PSMTXConcat(s, m, m);
    }
    PSMTXTrans(t, -PF32(party, 0x88), -PF32(party, 0x8C), -PF32(party, 0x90));
    PSMTXConcat(t, m, m);
    PSMTXRotRad(t, 'z', 0.017453292f * party->field_6C);
    PSMTXConcat(t, m, m);
    PSMTXRotRad(t, 'x', 0.017453292f * PF32(party, 0x64));
    PSMTXConcat(t, m, m);
    PSMTXRotRad(t, 'y', 0.017453292f * revise360(PF32(party, 0x10C) - PF32(party, 0xF8)));
    PSMTXConcat(t, m, m);
    if (party->flags2 & 0xC0) {
        dir = revise360(PF32(party, 0x10C));
        if (dir >= 90.0f && dir <= 270.0f) {
            PSMTXRotRad(t, 'y', 0.017453292f * ((party->flags2 & 0x40) ? 180.0f : 0.0f));
            PSMTXConcat(t, m, m);
        } else {
            PSMTXRotRad(t, 'y', 0.017453292f * ((party->flags2 & 0x40) ? 0.0f : 180.0f));
            PSMTXConcat(t, m, m);
        }
    }
    PSMTXTrans(t, PF32(party, 0x88), PF32(party, 0x8C), PF32(party, 0x90));
    PSMTXConcat(t, m, m);
    yoff = 0.0f;
    if (party->field_1C) {
        if (party->currentMemberId == 4) {
            yoff = 1.0f;
        } else if (party->currentMemberId == 3) {
            yoff = 2.0f;
        } else if (party->currentMemberId == 7) {
            yoff = 2.0f;
        } else if (party->currentMemberId == 1) {
            yoff = 2.0f;
        } else if (party->currentMemberId == 11) {
            yoff = 3.0f;
        } else if (party->currentMemberId == 8) {
            yoff = 3.5f;
        } else if (party->currentMemberId == 10) {
            yoff = 4.0f;
        } else if (party->currentMemberId == 9) {
            yoff = -1.0f;
        }
    }
    PSMTXTrans(t, party->position.x + PF32(party, 0x7C), yoff + (party->position.y + PF32(party, 0x80)),
               party->position.z + PF32(party, 0x84));
    PSMTXConcat(t, m, m);
    animPoseDrawMtx(party->paperAnimGroup, m, 2, 0.0f, 1.0f);
    animPoseDrawMtx(party->paperAnimGroup, m, 1, 0.0f, 1.0f);
    animPoseSetLocalTimeRate(party->paperAnimGroup, (party->flags & 0x80) ? 0.75f : 1.0f);
}


void partyKeyOn(void) {
}

void partyKeyOff(void) {
}

void L_partyClearCont(PartyEntry* party) {
    PS8(party, 0x158) = 0;
    PS8(party, 0x159) = 0;
    PS8(party, 0x15A) = 0;
    PS8(party, 0x15B) = 0;
    PS8(party, 0x15C) = 0;
    PS8(party, 0x15D) = 0;
    PS8(party, 0x15E) = 0;
    PS8(party, 0x15F) = 0;
    PU16(party, 0x150) = 0;
    PU16(party, 0x152) = 0;
    PU16(party, 0x154) = 0;
    PU16(party, 0x156) = 0;
}

void partyUpdateKeyData(PartyEntry* party) {
    s32 port = PS8(party, 0x30);
    if (party->flags & 0x60000000) {
        L_partyClearCont(party);
        return;
    }
    PS8(party, 0x158) = gp->stickX[port];
    PS8(party, 0x159) = -gp->stickY[port];
    PS8(party, 0x15A) = gp->substickX[port];
    PS8(party, 0x15B) = -gp->substickY[port];
    PS8(party, 0x15C) = gp->triggerLeft[port];
    PS8(party, 0x15D) = gp->triggerRight[port];
    PS8(party, 0x15E) = 0;
    PS8(party, 0x15F) = 0;
    PU16(party, 0x150) = gp->button[port];
    PU16(party, 0x152) = gp->buttonNew[port];
    PU16(party, 0x154) = gp->buttonRepeat[port];
    PU16(party, 0x156) = gp->buttonUp[port];
}

void partyShadowOff(PartyEntry* party) {
    if (party) {
        party->flags2 |= 0x100000;
    }
}

/* @@FAMI@@ */
static inline void partyPoseEntryF(PartyEntry* party) {
    char* animName;

    if ((s8)PU8(party, 0x31) != PARTY_MEMBER_YOSHI) {
        if (!(party->flags2 & 0x8000)) {
            animName = party_dp->name;
        } else {
            animName = "d_mario";
        }
    } else {
        animName = yoshigroup[pouchGetPartyColor(PARTNER_YOSHI)];
        if (party->flags2 & 0x8000) {
            animName = "d_mario";
        }
    }
    if (party->paperAnimGroup >= 0) {
        animPoseRelease(party->paperAnimGroup);
    }
    party->paperAnimGroup = animPoseEntry(animName, 2);
}


void partySetFamicomMode(PartyEntry* party, s32 mode) {
    if (mode == 1) {
        if (party->flags2 & 0x8000) {
            return;
        }
        L_partyForceSlitOff(party);
        party->flags2 |= 0x8000;
        partyPoseEntryF(party);
        partyChgPose(party, partyChgPoseId(party, 1));
    } else {
        if (!(party->flags2 & 0x8000)) {
            return;
        }
        party->flags2 &= ~0x8000;
        partyPoseEntryF(party);
        { char* id = partyChgPoseId(party, 1); partyChgPose(party, id); }
    }
}

