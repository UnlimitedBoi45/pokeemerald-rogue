#include "global.h"
#include "constants/abilities.h"
#include "constants/heal_locations.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/rogue.h"
#include "data.h"

#include "battle.h"
#include "battle_setup.h"
#include "berry.h"
#include "event_data.h"
#include "item.h"
#include "load_save.h"
#include "money.h"
#include "overworld.h"
#include "pokemon.h"
#include "random.h"
#include "strings.h"
#include "string_util.h"
#include "text.h"

#include "rogue_controller.h"
#include "rogue_query.h"

#define ROGUE_TRAINER_COUNT (FLAG_ROGUE_TRAINER_END - FLAG_ROGUE_TRAINER_START + 1)
#define ROGUE_ITEM_COUNT (FLAG_ROGUE_ITEM_END - FLAG_ROGUE_ITEM_START + 1)

// Gym leaders
#define ROOM_IDX_BOSS0      3
#define ROOM_IDX_BOSS1      6
#define ROOM_IDX_BOSS2      10
#define ROOM_IDX_BOSS3      14
#define ROOM_IDX_BOSS4      18
#define ROOM_IDX_BOSS5      22
#define ROOM_IDX_BOSS6      26
#define ROOM_IDX_BOSS7      30

// Elite 4
#define ROOM_IDX_BOSS8      35
#define ROOM_IDX_BOSS9      40
#define ROOM_IDX_BOSS10     45
#define ROOM_IDX_BOSS11     50

// Champions
#define ROOM_IDX_BOSS12     55
#define ROOM_IDX_BOSS13     60

#define ROOM_BOSS_GYM_START             ROOM_IDX_BOSS0
#define ROOM_BOSS_GYM_END               ROOM_IDX_BOSS7
#define ROOM_BOSS_ELITEFOUR_START       ROOM_IDX_BOSS8
#define ROOM_BOSS_ELITEFOUR_END         ROOM_IDX_BOSS11
#define ROOM_BOSS_CHAMPION_START        ROOM_IDX_BOSS12
#define ROOM_BOSS_CHAMPION_END          ROOM_IDX_BOSS13

#define OVERWORLD_FLAG 0

#ifdef ROGUE_DEBUG
EWRAM_DATA u8 gDebug_WildOptionCount = 0;
EWRAM_DATA u8 gDebug_ItemOptionCount = 0;

extern const u8 gText_RogueDebug_Header[];
extern const u8 gText_RogueDebug_Room[];
extern const u8 gText_RogueDebug_Difficulty[];
extern const u8 gText_RogueDebug_PlayerLvl[];
extern const u8 gText_RogueDebug_WildLvl[];
extern const u8 gText_RogueDebug_WildCount[];
extern const u8 gText_RogueDebug_ItemCount[];
extern const u8 gText_RogueDebug_Seed[];
#endif

EWRAM_DATA struct RogueRunData gRogueRun = {};
EWRAM_DATA struct RogueHubData gRogueSaveData = {};

static u8 GetDifficultyLevel(u16 roomIdx);

static u8 CalculatePlayerLevel(void);
static u8 CalculateWildLevel(void);
static u8 CalculateTrainerLevel(u16 trainerNum);

static void RandomiseWildEncounters(void);
static void ResetTrainerBattles(void);
static void RandomiseEnabledTrainers(void);
static void RandomiseEnabledItems(void);
static void RandomiseBerryTrees(void);

static u16 RogueRandomRange(u16 range, u8 flag)
{
    // Always use rogue random to avoid seeding issues based on flag
    u16 res = RogueRandom();

    if(FlagGet(FLAG_SET_SEED_ENABLED) && (flag == 0 || FlagGet(flag)))
        return res % range;
    else
        return Random() % range;
}

static u16 Rogue_GetSeed(void)
{
    u32 word0 = gSaveBlock1Ptr->dewfordTrends[0].words[0];
    u32 word1 = gSaveBlock1Ptr->dewfordTrends[0].words[1];
    u32 offset = 3;

    //if(Rogue_IsRunActive())
    //{
    //    offset = gRogueRun.currentRoomIdx * 3;
    //}

    return (u16)(word0 + word1 * offset);
}

bool8 Rogue_IsRunActive(void)
{
    return FlagGet(FLAG_ROGUE_RUN_ACTIVE);
}

bool8 Rogue_ForceExpAll(void)
{
    return FlagGet(FLAG_ROGUE_EXP_ALL);
}

void Rogue_ModifyExpGained(struct Pokemon *mon, s32* expGain)
{
    if(Rogue_IsRunActive())
    {
        u8 targetLevel = CalculatePlayerLevel();
        u8 currentLevel = GetMonData(mon, MON_DATA_LEVEL);

        if(currentLevel != 100)
        {
            // Could base of gBaseStats[species].growthRate?
            u32 currLvlExp = gExperienceTables[GROWTH_FAST][currentLevel];
            u32 nextLvlExp = gExperienceTables[GROWTH_FAST][currentLevel + 1];
            u32 lvlExp = (nextLvlExp - currLvlExp);

            if(currentLevel < targetLevel)
            {
                u8 delta = targetLevel - currentLevel;
                s32 desiredExpGain = 0;
                if(delta <= 3)
                {
                    // Give 25% of level always
                    desiredExpGain = (nextLvlExp * 25) / 100;
                }
                else
                {
                    // Give faster levels to catch up
                    delta = (delta - 3);
                    desiredExpGain = (nextLvlExp * 75 * delta) / 100;
                }

                *expGain = max(*expGain, desiredExpGain);
            }
            else
            {
                // If flag not set, just use normal EXP calculations (For now)
                if(!FlagGet(FLAG_ROGUE_CAN_OVERLVL))
                {
                    // No EXP once at target level
                    *expGain = 0;
                }
            }
        }
    }
    else
    {
        // No EXP outside of runs
        *expGain = 0;
    }
}

void Rogue_ModifyCatchRate(u8* catchRate, u8* ballMultiplier)
{
#ifdef ROGUE_DEBUG
    *ballMultiplier = 255; // Masterball equiv
#else
    *ballMultiplier = *ballMultiplier * 2;
#endif

    // Equiv to chansey
    if(*catchRate < 30)
        *catchRate = 30;
}

#ifdef ROGUE_DEBUG

bool8 Rogue_ShouldShowMiniMenu(void)
{
    return TRUE;
}

static u8* AppendNumberField(u8* strPointer, const u8* field, u32 num)
{
    u8 pow = 2;

    if(num >= 1000)
    {
        pow = 9;
    }
    else if(num >= 100)
    {
        pow = 3;
    }

    ConvertUIntToDecimalStringN(gStringVar1, num, STR_CONV_MODE_RIGHT_ALIGN, pow);

    strPointer = StringAppend(strPointer, field);
    return StringAppend(strPointer, gStringVar1);
}

u8* Rogue_GetMiniMenuContent(void)
{
    u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);
    u8 playerLevel = CalculatePlayerLevel();
    u8 wildLevel = CalculateWildLevel();

    u8* strPointer = &gStringVar4[0];
    *strPointer = EOS;

    strPointer = StringAppend(strPointer, gText_RogueDebug_Header);

    if(FlagGet(FLAG_SET_SEED_ENABLED))
    {
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_Seed, Rogue_GetSeed());
    }

    strPointer = AppendNumberField(strPointer, gText_RogueDebug_Room, gRogueRun.currentRoomIdx);
    strPointer = AppendNumberField(strPointer, gText_RogueDebug_Difficulty, difficultyLevel);
    strPointer = AppendNumberField(strPointer, gText_RogueDebug_PlayerLvl, playerLevel);
    strPointer = AppendNumberField(strPointer, gText_RogueDebug_WildLvl, wildLevel);
    strPointer = AppendNumberField(strPointer, gText_RogueDebug_WildCount, gDebug_WildOptionCount);
    strPointer = AppendNumberField(strPointer, gText_RogueDebug_ItemCount, gDebug_ItemOptionCount);

    return gStringVar4;
}
#else

bool8 Rogue_ShouldShowMiniMenu(void)
{
    return Rogue_IsRunActive();
}

u8* Rogue_GetMiniMenuContent(void)
{
    u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);

    ConvertIntToDecimalStringN(gStringVar1, gRogueRun.currentRoomIdx, STR_CONV_MODE_RIGHT_ALIGN, 2);
    ConvertIntToDecimalStringN(gStringVar2, difficultyLevel, STR_CONV_MODE_RIGHT_ALIGN, 2);
    StringExpandPlaceholders(gStringVar4, gText_RogueRoomProgress);
    return gStringVar4;
}
#endif

static void SelectStartMons(void)
{
    u8 i, j;
    bool8 isValid;
    u16 randIdx;
    u16 queryCount;
    u16 species;

    RogueQuery_Clear();

    RogueQuery_SpeciesIsValid();
    RogueQuery_SpeciesIsNotLegendary();
    RogueQuery_TransformToEggSpecies();
    RogueQuery_SpeciesWithEvolutionStages(2);

    RogueQuery_CollapseSpeciesBuffer();
    queryCount = RogueQuery_BufferSize();

    for(i = 0; i < 3;)
    {
        isValid = TRUE;
        randIdx = Random() % queryCount;
        species = RogueQuery_BufferPtr()[randIdx];

        // Check other starter is not already this
        for(j = 0; j < i; ++j)
        {
            if(VarGet(VAR_ROGUE_STARTER0 + j) == species)
            {
                isValid = FALSE;
                break;
            }
        }

        if(isValid)
        {
            VarSet(VAR_ROGUE_STARTER0 + i, species);
            ++i;
        }
    }
}

void Rogue_OnNewGame(void)
{
    SelectStartMons();

    FlagClear(FLAG_ROGUE_RUN_ACTIVE);

    // Seed settings
    FlagClear(FLAG_SET_SEED_ENABLED);
    FlagSet(FLAG_SET_SEED_ITEMS);
    FlagSet(FLAG_SET_SEED_TRAINERS);
    FlagSet(FLAG_SET_SEED_BOSSES);
    FlagSet(FLAG_SET_SEED_WILDMONS);
    
    // Run settings
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    FlagSet(FLAG_ROGUE_EXP_ALL);
    FlagClear(FLAG_ROGUE_DOUBLE_BATTLES);
    FlagClear(FLAG_ROGUE_CAN_OVERLVL);
    FlagClear(FLAG_ROGUE_HARD_TRAINERS);

    VarSet(VAR_ROGUE_DIFFICULTY, 0);
    VarSet(VAR_ROGUE_FURTHEST_DIFFICULTY, 0);

    FlagSet(FLAG_SYS_B_DASH);
    EnableNationalPokedex();

    SetLastHealLocationWarp(HEAL_LOCATION_ROGUE_HUB);

#ifdef ROGUE_DEBUG
    //AddBagItem(ITEM_RARE_CANDY, 99);
    //AddBagItem(ITEM_RARE_CANDY, 99);
    //AddBagItem(ITEM_RARE_CANDY, 99);
    //SetMoney(&gSaveBlock1Ptr->money, 60000);
//
    //struct Pokemon starterMon;
    //CreateMon(&starterMon, SPECIES_RAYQUAZA, 100, MAX_PER_STAT_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    //GiveMonToPlayer(&starterMon);
//
    //CreateMon(&starterMon, SPECIES_GROUDON, 100, MAX_PER_STAT_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    //GiveMonToPlayer(&starterMon);
//
    //CreateMon(&starterMon, SPECIES_KYOGRE, 100, MAX_PER_STAT_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    //GiveMonToPlayer(&starterMon);
//
    //CreateMon(&starterMon, SPECIES_DEOXYS, 100, MAX_PER_STAT_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    //GiveMonToPlayer(&starterMon);
//
    //CreateMon(&starterMon, SPECIES_LUGIA, 100, MAX_PER_STAT_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    //GiveMonToPlayer(&starterMon);
//
    //CreateMon(&starterMon, SPECIES_HO_OH, 100, MAX_PER_STAT_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    //GiveMonToPlayer(&starterMon);
#endif
}

void Rogue_GameClear(void)
{
    SetContinueGameWarpToHealLocation(HEAL_LOCATION_ROGUE_HUB);
}

void Rogue_SetDefaultOptions(void)
{
#ifdef ROGUE_DEBUG
    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_FAST;
#else
    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_SLOW;
#endif
    gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SET;
    //gSaveBlock2Ptr->optionsSound = OPTIONS_SOUND_MONO;
    //gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SHIFT;
    //gSaveBlock2Ptr->optionsBattleSceneOff = FALSE;
    //gSaveBlock2Ptr->regionMapZoom = FALSE;
}

void Rogue_OnLoadMap(void)
{
    // TODO - Remove this

    // Seems to be working? Need to track against flag here though, as this gets called for started maps
    //FlagSet(FLAG_SYS_POKEMON_GET);
    
        //*((s32*)((void*)0)) = 123;

    //s32 i;
    //struct WarpEvent *warpEvent = gMapHeader.events->warps;
    //u8 warpCount = gMapHeader.events->warpCount;
    //
    //for (i = 0; i < warpCount; i++, warpEvent++)
    //{
    //    if(i == 0)
    //    {
    //        // Skip first warp as that should always be left as the entry warp
    //        continue;
    //    }
//
    //    // Should be Prof lab
    //    warpEvent->warpId = 0;
    //    warpEvent->mapNum = 4;
    //    warpEvent->mapGroup = 0;
    //}

    // TODO - Do something
}

static void BeginRogueRun(void)
{
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);

    if(FlagGet(FLAG_SET_SEED_ENABLED))
    {
        SeedRogueRng(Rogue_GetSeed());
    }

    ClearBerryTrees();

    gRogueRun.currentRoomIdx = 0;
    gRogueRun.nextRestStopRoomIdx = ROOM_IDX_BOSS0;
    gRogueRun.currentRouteType = ROGUE_ROUTE_FIELD;
    
    VarSet(VAR_ROGUE_DIFFICULTY, 0);
    
    // Store current states
    SavePlayerParty();
    LoadPlayerBag(); // Bag funcs named in opposite

    gRogueSaveData.money = GetMoney(&gSaveBlock1Ptr->money);
    gRogueSaveData.registeredItem = gSaveBlock1Ptr->registeredItem;

    SetMoney(&gSaveBlock1Ptr->money, 0);

    FlagClear(FLAG_ROGUE_DEFEATED_BOSS00);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS01);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS02);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS03);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS04);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS05);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS06);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS07);
    
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS08);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS09);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS10);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS11);

    FlagClear(FLAG_ROGUE_DEFEATED_BOSS12);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS13);

#ifdef ROGUE_DEBUG
    // TEMP - Testing only
    //gRogueRun.currentRoomIdx = ROOM_BOSS_CHAMPION_END - 1;
    //gRogueRun.nextRestStopRoomIdx = ROOM_BOSS_CHAMPION_END;

    gRogueRun.currentRouteType = ROGUE_ROUTE_WATERFRONT;
#endif
}

static void EndRogueRun(void)
{
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    FlagClear(FLAG_SET_SEED_ENABLED);
    //gRogueRun.currentRoomIdx = 0;

    // Restore current states
    LoadPlayerParty();
    SavePlayerBag(); // Bag funcs named in opposite

    SetMoney(&gSaveBlock1Ptr->money, gRogueSaveData.money);
    gSaveBlock1Ptr->registeredItem = gRogueSaveData.registeredItem;
}

static bool8 IsBossRoom(u16 roomIdx)
{
    switch(roomIdx)
    {
        case ROOM_IDX_BOSS0:
        case ROOM_IDX_BOSS1:
        case ROOM_IDX_BOSS2:
        case ROOM_IDX_BOSS3:
        case ROOM_IDX_BOSS4:
        case ROOM_IDX_BOSS5:
        case ROOM_IDX_BOSS6:
        case ROOM_IDX_BOSS7:
        case ROOM_IDX_BOSS8:
        case ROOM_IDX_BOSS9:
        case ROOM_IDX_BOSS10:
        case ROOM_IDX_BOSS11:
        case ROOM_IDX_BOSS12:
        case ROOM_IDX_BOSS13:
            return TRUE;
    };

    return FALSE;
}

static bool8 IsSpecialEncounterRoomWarp(struct WarpData *warp)
{
    // Just an arbitrary number we're gonna use to indicate special encounter rooms
    return warp->warpId == 55;
}

static u8 GetDifficultyLevel(u16 roomIdx)
{
    if(roomIdx <= ROOM_IDX_BOSS0)
        return 0;
    if(roomIdx <= ROOM_IDX_BOSS1)
        return 1;
    if(roomIdx <= ROOM_IDX_BOSS2)
        return 2;
    if(roomIdx <= ROOM_IDX_BOSS3)
        return 3;
    if(roomIdx <= ROOM_IDX_BOSS4)
        return 4;
    if(roomIdx <= ROOM_IDX_BOSS5)
        return 5;
    if(roomIdx <= ROOM_IDX_BOSS6)
        return 6;
    if(roomIdx <= ROOM_IDX_BOSS7)
        return 7;

    if(roomIdx <= ROOM_IDX_BOSS8)
        return 8;
    if(roomIdx <= ROOM_IDX_BOSS9)
        return 9;
    if(roomIdx <= ROOM_IDX_BOSS10)
        return 10;
    if(roomIdx <= ROOM_IDX_BOSS11)
        return 11;

    if(roomIdx <= ROOM_IDX_BOSS12)
        return 12;
    return 13;
}

static void SelectBossRoom(u16 nextRoomIdx, struct WarpData *warp)
{
    u8 bossId = 0;

    do
    {
        if(nextRoomIdx <= ROOM_BOSS_GYM_END)
        {
            bossId = RogueRandomRange(8, OVERWORLD_FLAG);
        }
        else if(nextRoomIdx <= ROOM_BOSS_ELITEFOUR_END)
        {
            bossId = 8 + RogueRandomRange(4, OVERWORLD_FLAG);
        }
        else if(nextRoomIdx == ROOM_BOSS_CHAMPION_START)
        {
            bossId = 12;
            break;
        }
        else //if(nextRoomIdx == ROOM_BOSS_CHAMPION_END)
        {
            bossId = 13;
            break;
        }
    }
    while(FlagGet(FLAG_ROGUE_DEFEATED_BOSS00 + bossId));

    switch(bossId)
    {
        case 0:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_0);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_0);
            break;

        case 1:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_1);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_1);
            break;

        case 2:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_2);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_2);
            break;

        case 3:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_3);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_3);
            break;

        case 4:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_4);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_4);
            break;

        case 5:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_5);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_5);
            break;

        case 6:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_6);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_6);
            break;

        case 7:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_7);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_7);
            break;


        case 8:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_8);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_8);
            break;

        case 9:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_9);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_9);
            break;

        case 10:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_10);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_10);
            break;

        case 11:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_11);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_11);
            break;


        case 12:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_12);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_12);
            break;

        case 13:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_13);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_13);
            break;
    };
}

static void SelectRouteRoom(u16 nextRoomIdx, struct WarpData *warp)
{
    u8 mapCount;
    u8 mapIdx;
    u8 swapRouteChance = 60;
    const struct RogueRouteMap* selectedMap = NULL;

#ifdef ROGUE_DEBUG
    if(nextRoomIdx == 1)
    {
        // Always force first route type for debug
        swapRouteChance = 0;
    }
    else 
#endif
    if(nextRoomIdx < ROOM_IDX_BOSS0)
    {
        swapRouteChance = 100;
    }

    if(swapRouteChance != 0)
    {
        if(swapRouteChance >= 100 || RogueRandomRange(100, OVERWORLD_FLAG) < swapRouteChance)
        {
            gRogueRun.currentRouteType = ROGUE_ROUTE_START + RogueRandomRange(ROGUE_ROUTE_COUNT, OVERWORLD_FLAG);
        }
    }

    mapCount = gRogueRouteTable[gRogueRun.currentRouteType].mapCount;

    // Avoid doing same route in a row
    do
    {
        mapIdx = RogueRandomRange(mapCount,  OVERWORLD_FLAG);
        selectedMap = &gRogueRouteTable[gRogueRun.currentRouteType].mapTable[mapIdx];
    }
    while(mapCount > 1 && gMapHeader.mapLayoutId == selectedMap->layout);

    warp->mapGroup = selectedMap->group;
    warp->mapNum = selectedMap->num;
}

static void ResetSpecialEncounterStates(void)
{
    // Special states
    // Rayquaza
    VarSet(VAR_SKY_PILLAR_STATE, 2); // Keep in clean layout, but act as is R is has left for G/K cutscene
    VarSet(VAR_SKY_PILLAR_RAQUAZA_CRY_DONE, 1); // Hide cutscene R
    FlagClear(FLAG_DEFEATED_RAYQUAZA);
    FlagClear(FLAG_HIDE_SKY_PILLAR_TOP_RAYQUAZA_STILL); // Show battle
    FlagSet(FLAG_HIDE_SKY_PILLAR_TOP_RAYQUAZA); // Hide cutscene R

    // Groudon + Kyogre
    FlagClear(FLAG_DEFEATED_GROUDON);
    FlagClear(FLAG_DEFEATED_KYOGRE);

    // Mew
    FlagClear(FLAG_HIDE_MEW);
    FlagClear(FLAG_DEFEATED_MEW);
    FlagClear(FLAG_CAUGHT_MEW);

    // Deoxys
    FlagClear(FLAG_BATTLED_DEOXYS);
    FlagClear(FLAG_DEFEATED_DEOXYS);

    // Ho-oh + Lugia
    FlagClear(FLAG_CAUGHT_HO_OH);
    FlagClear(FLAG_CAUGHT_LUGIA);

    // Regis
    FlagClear(FLAG_DEFEATED_REGICE);
    FlagClear(FLAG_DEFEATED_REGISTEEL);
    FlagClear(FLAG_DEFEATED_REGIROCK);

    // Latis
    //FlagClear(FLAG_DEFEATED_LATIAS_OR_LATIOS);
    //FlagClear(FLAG_CAUGHT_LATIAS_OR_LATIOS);

    // Reset ledgendary encounters
    //FlagSet(FLAG_HIDE_SOUTHERN_ISLAND_EON_STONE);
}

static bool8 PartyContainsSpecies(struct Pokemon *party, u8 partyCount, u16 species)
{
    u8 i;
    u16 s;
    for(i = 0; i < partyCount; ++i)
    {
        s = GetMonData(&party[i], MON_DATA_SPECIES);

        if(s == species)
            return TRUE;
    }

    return FALSE;
}

static void SelectSpecialEncounterRoom(u16 nextRoomIdx, struct WarpData *warp)
{
    u8 mapCount;
    u8 mapIdx;
    u8 swapRouteChance = 60;
    const struct RogueEncounterMap* selectedMap = NULL;
    
    ResetSpecialEncounterStates();

    mapCount = gRogueSpecialEncounterInfo.mapCount;

    // Avoid repeating same ecounter (TODO)
    do
    {
        mapIdx = RogueRandomRange(mapCount, OVERWORLD_FLAG);
        selectedMap = &gRogueSpecialEncounterInfo.mapTable[mapIdx];
    }
    while(mapCount > 6 && PartyContainsSpecies(&gPlayerParty[0], gPlayerPartyCount, selectedMap->encounterSpecies));

    warp->mapGroup = selectedMap->group;
    warp->mapNum = selectedMap->num;
}

static bool8 IsSpecialEncounterRoom(void)
{
    u8 i;
    u8 mapCount = gRogueSpecialEncounterInfo.mapCount;

    for(i = 0; i < mapCount; ++i)
    {
        if(gRogueSpecialEncounterInfo.mapTable[i].layout == gMapHeader.mapLayoutId)
            return TRUE;
    }

    return FALSE;
}

void Rogue_OnWarpIntoMap(void)
{
    u8 difficultyLevel;

    if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_HUB_TRANSITION)
    {
        if(!Rogue_IsRunActive())
        {
            BeginRogueRun();
        }
    }
    else if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_HUB && Rogue_IsRunActive())
    {
        EndRogueRun();
    }
    else if(Rogue_IsRunActive())
    {
        if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_ENCOUNTER_REST_STOP)
        {
            RandomiseEnabledTrainers();
        }
        else if(IsSpecialEncounterRoom())
        {
        }
        else
        {
            ++gRogueRun.currentRoomIdx;
            difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);
            
            // Update VARs
            VarSet(VAR_ROGUE_DIFFICULTY, difficultyLevel);
            VarSet(VAR_ROGUE_FURTHEST_DIFFICULTY, max(difficultyLevel, VarGet(VAR_ROGUE_FURTHEST_DIFFICULTY)));
        
            if(IsBossRoom(gRogueRun.currentRoomIdx))
            {
                ResetTrainerBattles();
                RandomiseEnabledItems();
            }
            else
            {
                RandomiseWildEncounters();
                ResetTrainerBattles();
                RandomiseEnabledTrainers();
                RandomiseEnabledItems();
                RandomiseBerryTrees();
            }
        }
    }
}


void Rogue_OnSetWarpData(struct WarpData *warp)
{
    // Configure random warp
    //if(warp->mapGroup == MAP_GROUP(ROGUE_HUB_TRANSITION) && warp->mapNum == MAP_NUM(ROGUE_HUB_TRANSITION) && warp->warpId == 1)

    if(Rogue_IsRunActive())
    {
        u16 nextRoomIdx = gRogueRun.currentRoomIdx + 1;

        if(IsSpecialEncounterRoomWarp(warp))
        {
            SelectSpecialEncounterRoom(nextRoomIdx, warp);
        }
        else if(nextRoomIdx >= gRogueRun.nextRestStopRoomIdx)
        {
            // We're about to hit a rest stop so force it here
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_REST_STOP);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_REST_STOP);

            // Will encounter the next rest stop in 4-6 rooms
            gRogueRun.nextRestStopRoomIdx = nextRoomIdx + 4 + RogueRandomRange(3, OVERWORLD_FLAG);
        }
        else if(IsBossRoom(nextRoomIdx))
        {
            SelectBossRoom(nextRoomIdx, warp);
        }
        else
        {
            SelectRouteRoom(nextRoomIdx, warp);
        }

        warp->warpId = 0;
        warp->x = -1;
        warp->y = -1;
    }
}

void RemoveAnyFaintedMons(void)
{
    u8 read;
    u8 write = 0;
    u8 alivePartyCount = 0;

    for(read = 0; read < gPlayerPartyCount; ++read)
    {
        if(GetMonData(&gPlayerParty[read], MON_DATA_HP) != 0)
        {
            if(write != read)
                CopyMon(&gPlayerParty[write], &gPlayerParty[read], sizeof(gPlayerParty[read]));

            ++write;
            ++alivePartyCount;
        }
        else
        {
            // Dead so give back held item
            u16 heldItem = GetMonData(&gPlayerParty[read], MON_DATA_HELD_ITEM);
            if(heldItem != ITEM_NONE)
                AddBagItem(heldItem, 1);
        }
    }

    gPlayerPartyCount = alivePartyCount;

    for(read = gPlayerPartyCount; read < PARTY_SIZE; ++read)
        ZeroMonData(&gPlayerParty[read]);
}

void Rogue_Battle_StartTrainerBattle(void)
{
    if(FlagGet(FLAG_ROGUE_DOUBLE_BATTLES)) //NoOfApproachingTrainers != 2 
    {
        // No need to check opponent party as we force it to 2 below
        if(gPlayerPartyCount >= 2) // gEnemyPartyCount >= 2
        {
             // Force double?
            gBattleTypeFlags |= BATTLE_TYPE_DOUBLE;
        }
    }
}

static bool32 IsPlayerDefeated(u32 battleOutcome)
{
    switch (battleOutcome)
    {
    case B_OUTCOME_LOST:
    case B_OUTCOME_DREW:
        return TRUE;
    case B_OUTCOME_WON:
    case B_OUTCOME_RAN:
    case B_OUTCOME_PLAYER_TELEPORTED:
    case B_OUTCOME_MON_FLED:
    case B_OUTCOME_CAUGHT:
        return FALSE;
    default:
        return FALSE;
    }
}

void Rogue_Battle_EndTrainerBattle(void)
{
    if(Rogue_IsRunActive())
    {
        if (IsPlayerDefeated(gBattleOutcome) != TRUE)
        {
            RemoveAnyFaintedMons();
        }
    }
}

void Rogue_Battle_EndWildBattle(void)
{
    if(Rogue_IsRunActive())
    {
        if (IsPlayerDefeated(gBattleOutcome) != TRUE)
        {
            RemoveAnyFaintedMons();
        }
    }
}

static bool8 IsBossTrainer(u16 trainerNum)
{
    switch(trainerNum)
    {
        case TRAINER_ROXANNE_1:
        case TRAINER_BRAWLY_1:
        case TRAINER_WATTSON_1:
        case TRAINER_FLANNERY_1:
        case TRAINER_NORMAN_1:
        case TRAINER_WINONA_1:
        case TRAINER_TATE_AND_LIZA_1:
        case TRAINER_JUAN_1:

        case TRAINER_SIDNEY:
        case TRAINER_PHOEBE:
        case TRAINER_GLACIA:
        case TRAINER_DRAKE:

        case TRAINER_WALLACE:
        case TRAINER_STEVEN:
            return TRUE;
    };

    return FALSE;
}

static bool8 UseCompetitiveMoveset(u16 trainerNum)
{
    // Start using competitive movesets on 3rd gym
    if(IsBossTrainer(trainerNum) && GetDifficultyLevel(gRogueRun.currentRoomIdx) >= 2)
    {
        return TRUE;
    }

    return FALSE;
}

static void SeedRogueTrainer(u16 seed, u16 trainerNum, u16 offset)
{
    SeedRogueRng(seed + trainerNum * 3 + offset * 7);
}

static void ConfigureTrainer(u16 trainerNum, u8* forceType, bool8* allowItemEvos, bool8* allowLedgendaries, u8* monsCount)
{
    u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);

    switch(trainerNum)
    {
        case TRAINER_ROXANNE_1:
            *forceType = TYPE_ROCK;
            break;
        case TRAINER_BRAWLY_1:
            *forceType = TYPE_FIGHTING;
            break;
        case TRAINER_WATTSON_1:
            *forceType = TYPE_ELECTRIC;
            break;
        case TRAINER_FLANNERY_1:
            *forceType = TYPE_FIRE;
            break;
        case TRAINER_NORMAN_1:
            *forceType = TYPE_NORMAL;
            break;
        case TRAINER_WINONA_1:
            *forceType = TYPE_FLYING;
            break;
        case TRAINER_TATE_AND_LIZA_1:
            *forceType = TYPE_PSYCHIC;
            break;
        case TRAINER_JUAN_1:
            *forceType = TYPE_WATER;
            break;

        case TRAINER_SIDNEY:
            *forceType = TYPE_DARK;
            break;
        case TRAINER_PHOEBE:
            *forceType = TYPE_GHOST;
            break;
        case TRAINER_GLACIA:
            *forceType = TYPE_ICE;
            break;
        case TRAINER_DRAKE:
            *forceType = TYPE_DRAGON;
            break;

        case TRAINER_WALLACE:
            *forceType = TYPE_WATER;
            break;
        case TRAINER_STEVEN:
            *forceType = TYPE_STEEL;
            break;
    };

    if(IsBossTrainer(trainerNum))
    {
        if(difficultyLevel == 0)
        {
            *monsCount = 3;
            *allowItemEvos = FALSE;
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel == 1)
        {
            *monsCount = 4;
            *allowItemEvos = FALSE;
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel == 2)
        {
            *monsCount = 4;
            *allowItemEvos = FALSE;
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel <= 5)
        {
            *monsCount = 5;
            *allowItemEvos = TRUE;
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel <= 6) 
        {
            *monsCount = 6;
            *allowItemEvos = TRUE;
            *allowLedgendaries = FALSE;
        }
        else // From last gym leader we can see ledgendaries
        {
            *monsCount = 6;
            *allowItemEvos = TRUE;
            *allowLedgendaries = TRUE;
        }
    }
    else
    {
        if(difficultyLevel == 0)
        {
            *monsCount = 1 + RogueRandomRange(2, FLAG_SET_SEED_TRAINERS);
            *forceType = TYPE_NORMAL;
            *allowItemEvos = FALSE;
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel == 1)
        {
            *monsCount = 1 + RogueRandomRange(3, FLAG_SET_SEED_TRAINERS);
            *forceType = TYPE_NORMAL;
            *allowItemEvos = FALSE;
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel == 2)
        {
            *monsCount = 2 + RogueRandomRange(2, FLAG_SET_SEED_TRAINERS);
            *allowItemEvos = FALSE;
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel <= 7)
        {
            *monsCount = 3 + RogueRandomRange(1, FLAG_SET_SEED_TRAINERS);
            *allowItemEvos = FALSE;
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel <= 11)
        {
            // Elite 4
            *monsCount = 3 + RogueRandomRange(1, FLAG_SET_SEED_TRAINERS);
            *allowItemEvos = TRUE;
            *allowLedgendaries = TRUE;
        }
        else
        {
            // Champion
            *monsCount = 4 + (RogueRandomRange(5, FLAG_SET_SEED_TRAINERS) == 0 ? 2 : 0);
            *allowItemEvos = TRUE;
            *allowLedgendaries = TRUE;
        }
    }

    if(FlagGet(FLAG_ROGUE_DOUBLE_BATTLES)) 
    {
        if(*monsCount < 2)
        {
            *monsCount = 2;
        }
    }
}

bool8 Rogue_OverrideTrainerItems(u16* items)
{
    if(Rogue_IsRunActive())
    {
        //for (i = 0; i < MAX_TRAINER_ITEMS; i++)
        //{
        //    items[i] = ITEM_NONE;
        //}

        return TRUE;
    }

    return FALSE;
}

void Rogue_PreCreateTrainerParty(u16 trainerNum, bool8* useRogueCreateMon, u8* monsCount)
{
    if(Rogue_IsRunActive())
    {
        u16 startSeed = gRngRogueValue;
        u8 allowedType = TYPE_NONE;
        bool8 allowItemEvos = FALSE;
        bool8 allowLedgendaries = FALSE;

        SeedRogueTrainer(startSeed, trainerNum, 0);
        ConfigureTrainer(trainerNum, &allowedType, &allowItemEvos, &allowLedgendaries, monsCount);

        // Query for the current trainer team
        RogueQuery_Clear();

        RogueQuery_SpeciesIsValid();

        if(!allowLedgendaries)
            RogueQuery_SpeciesIsNotLegendary();

        if(allowedType != TYPE_NONE)
            RogueQuery_SpeciesOfType(allowedType);

        RogueQuery_TransformToEggSpecies();

        // Evolve the species to just below the wild encounter level
        RogueQuery_EvolveSpeciesToLevel(CalculateTrainerLevel(trainerNum));
        
        if(allowItemEvos)
            RogueQuery_EvolveSpeciesByItem();

        if(allowedType != TYPE_NONE)
            RogueQuery_SpeciesOfType(allowedType);

        RogueQuery_CollapseSpeciesBuffer();

        *useRogueCreateMon = TRUE;

        SeedRogueRng(startSeed);
        return;
    }

    *useRogueCreateMon = FALSE;
}

static u16 NextTrainerSpecies(u16 trainerNum, bool8 isBoss, struct Pokemon *party, u8 monIdx, u8 totalMonCount)
{
    u16 species;
    u16 randIdx;
    u16 queryCount = RogueQuery_BufferSize();
    
    // Prevent duplicates, if possible (Only for bosses)
    do
    {
        randIdx = RogueRandomRange(queryCount, isBoss ? FLAG_SET_SEED_BOSSES : FLAG_SET_SEED_TRAINERS);
        species = RogueQuery_BufferPtr()[randIdx];
    }
    while(isBoss && PartyContainsSpecies(party, monIdx, species) && queryCount > totalMonCount);

    return species;
}

void Rogue_CreateTrainerMon(u16 trainerNum, struct Pokemon *party, u8 monIdx, u8 totalMonCount)
{
    u16 species;
    u8 level;
    u8 fixedIV;
    u16 startSeed = gRngRogueValue;
    bool8 isBoss = IsBossTrainer(trainerNum);
    struct Pokemon *mon = &party[monIdx];

    SeedRogueTrainer(startSeed, trainerNum, monIdx);

    // For wallace will will always have at least 1 ledgendary in last slot
    if(trainerNum == TRAINER_WALLACE)
    {
        if(monIdx == 5)
        {
            // Requery with just legendaries
            RogueQuery_Clear();
            RogueQuery_SpeciesIsValid();
            RogueQuery_SpeciesIsLegendary();
            RogueQuery_SpeciesOfType(TYPE_WATER);
            RogueQuery_CollapseSpeciesBuffer();
        }
    }
    // For steven will will always have at least 2 ledgendarys, both in last 2 slots
    else if(trainerNum == TRAINER_STEVEN)
    {
        if(monIdx == 4)
        {
            // Requery with just legendaries
            RogueQuery_Clear();
            RogueQuery_SpeciesIsValid();
            RogueQuery_SpeciesIsLegendary();
            RogueQuery_SpeciesOfType(TYPE_PSYCHIC);
            RogueQuery_CollapseSpeciesBuffer();
        }
    }

    species = NextTrainerSpecies(trainerNum, isBoss, party, monIdx, totalMonCount);
    level = CalculateTrainerLevel(trainerNum) - 1;

    if(FlagGet(FLAG_ROGUE_HARD_TRAINERS))
        fixedIV = MAX_PER_STAT_IVS;
    else
        fixedIV = 0;//isBoss ? 11 : 0;

    if(!isBoss)
    {
        if(monIdx == 0)
            level--;
    }

    if(monIdx == totalMonCount - 1)
        level++;

    CreateMon(mon, species, level, fixedIV, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);

    if(UseCompetitiveMoveset(trainerNum) && gPresetMonTable[species].presetCount != 0)
    {
        s32 i;
        u8 presetIdx = RogueRandomRange(gPresetMonTable[species].presetCount, isBoss ? FLAG_SET_SEED_BOSSES : FLAG_SET_SEED_TRAINERS);
        const struct RogueMonPreset* preset = &gPresetMonTable[species].presets[presetIdx];

        if(preset->abilityNum != ABILITY_NONE)
        {
            SetMonData(mon, MON_DATA_ABILITY_NUM, &preset->abilityNum);
        }

        if(preset->heldItem != ITEM_NONE)
        {
            SetMonData(mon, MON_DATA_HELD_ITEM, &preset->heldItem);
        }

        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            if(preset->moves[i] != MOVE_NONE)
            {
                SetMonData(mon, MON_DATA_MOVE1 + i, &preset->moves[i]);
                SetMonData(mon, MON_DATA_PP1 + i, &gBattleMoves[preset->moves[i]].pp);
            }
        }
    }

    SeedRogueRng(startSeed);
}

void Rogue_CreateWildMon(u8 area, u16* species, u8* level)
{
    // Note: Don't seed individual encounters
    if(Rogue_IsRunActive())
    {
        u8 maxlevel = CalculateWildLevel();
        u8 levelVariation = min(6,maxlevel - 1);
        const u16 count = ARRAY_COUNT(gRogueRun.wildEncounters);
        u16 randIdx = Random() % count; 

        *species = gRogueRun.wildEncounters[randIdx];
        *level = maxlevel - (Random() % levelVariation);
    }
}

void Rogue_CreateEventMon(u16* species, u8* level, u16* itemId)
{
    *level = CalculateWildLevel();
}

static void RandomiseWildEncounters(void)
{
    u8 maxlevel = CalculateWildLevel();

    // Query for the current route type
    RogueQuery_Clear();

    RogueQuery_SpeciesIsValid();
    RogueQuery_SpeciesIsNotLegendary();
    RogueQuery_SpeciesOfTypes(gRogueRouteTable[gRogueRun.currentRouteType].wildTypeTable, gRogueRouteTable[gRogueRun.currentRouteType].wildTypeTableCount);
    RogueQuery_TransformToEggSpecies();

    // Evolve the species to just below the wild encounter level
    RogueQuery_EvolveSpeciesToLevel(maxlevel - min(6, maxlevel - 1));
    RogueQuery_SpeciesOfTypes(gRogueRouteTable[gRogueRun.currentRouteType].wildTypeTable, gRogueRouteTable[gRogueRun.currentRouteType].wildTypeTableCount);

    RogueQuery_CollapseSpeciesBuffer();

    {
        u8 i;
        u16 randIdx;
        u16 queryCount = RogueQuery_BufferSize();

#ifdef ROGUE_DEBUG
        gDebug_WildOptionCount = queryCount;
#endif

        for(i = 0; i < ARRAY_COUNT(gRogueRun.wildEncounters); ++i)
        {
            randIdx = RogueRandomRange(queryCount, FLAG_SET_SEED_WILDMONS);
            gRogueRun.wildEncounters[i] = RogueQuery_BufferPtr()[randIdx];
        }
    }
}

static void ResetTrainerBattles(void)
{
    // Reset trainer encounters
    s16 i;
    for(i = 0; i < TRAINERS_COUNT; ++i)
    {
        ClearTrainerFlag(i);
    }
}

static u8 CalculateWildLevel(void)
{
    return CalculatePlayerLevel() - 7;
}

static u8 CalculateBossLevel(u8 difficulty)
{
    // Gym leaders lvs 10 -> 80
    if(difficulty <= 7)
    {
        return min(100, 10 + 10 * difficulty);
    }
    else
    {
        // Both champions are lvl 100
        difficulty -= 7;
        return min(100, 80 + 4 * difficulty);
    }
}

static u8 CalculatePlayerLevel(void)
{
    u8 prevLevel;
    u8 currLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);

    if(currLevel == 0)
    {
        // Cannot do blending
        return CalculateBossLevel(currLevel);
    }
    else
    {
        prevLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx - 1);
    }

    if(currLevel == prevLevel)
    {
        return CalculateBossLevel(currLevel);
    }
    else
    {
        // We've just transitioned so use midpoint
        return (CalculateBossLevel(prevLevel) + CalculateBossLevel(currLevel)) / 2;
    }
}

static u8 CalculateTrainerLevel(u16 trainerNum)
{
    if(IsBossTrainer(trainerNum))
    {
        return CalculatePlayerLevel();
    }
    else
    {
        u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);

        if(difficultyLevel == 0)
        {
            return 2 + gRogueRun.currentRoomIdx;
        }
        else
        {
            // Trainers will lag behind to make grinding easier
            return CalculateBossLevel(difficultyLevel - 1);
        }
    }
}

static bool8 RandomChance(u8 chance, u16 seedFlag)
{
    if(chance == 0)
        return FALSE;
    else if(chance >= 100)
        return TRUE;

    return (RogueRandomRange(100, seedFlag) + 1) <= chance;
}

static bool8 RandomChanceTrainer()
{
    u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);
    u8 chance = 10 + 5 * difficultyLevel;

    return RandomChance(chance, FLAG_SET_SEED_TRAINERS);
}

static bool8 RandomChanceItem()
{
    u8 chance;

    if(IsBossRoom(gRogueRun.currentRoomIdx))
    {
        chance = 75;
    }
    else
    {
        u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);
        chance = 75 - 5 * difficultyLevel;
    }

    return RandomChance(chance, FLAG_SET_SEED_ITEMS);
}

static bool8 RandomChanceBerry()
{
    u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);
    u8 chance = 95 - 7 * difficultyLevel;

    return RandomChance(chance, FLAG_SET_SEED_ITEMS);
}

static void RandomiseEnabledTrainers(void)
{
    s32 i;
    for(i = 0; i < ROGUE_TRAINER_COUNT; ++i)
    {
        if(RandomChanceTrainer())
        {
            // Clear flag to show
            FlagClear(FLAG_ROGUE_TRAINER_START + i);
        }
        else
        {
            // Set flag to hide
            FlagSet(FLAG_ROGUE_TRAINER_START + i);
        }
    }
}

static void RandomiseItemContent(u8 difficultyLevel)
{
    u16 queryCount;

    // Queue up random items
    RogueQuery_Clear();

    if(gRogueRun.currentRouteType == ROGUE_ROUTE_CAVE)
    {
        // In cave we have higher chance of finding valuables
        RogueQuery_ItemsInPriceRange(100 + 400 * difficultyLevel, 300 + 800 * difficultyLevel);
    }
    else
    {
        RogueQuery_ItemsInPriceRange(50 + 100 * difficultyLevel, 300 + 800 * difficultyLevel);
    }

    RogueQuery_ItemsIsValid();
    RogueQuery_ItemsNotInPocket(POCKET_KEY_ITEMS);
    RogueQuery_ItemsNotInPocket(POCKET_BERRIES);

    RogueQuery_Exclude(ITEM_SACRED_ASH);
    RogueQuery_Exclude(ITEM_REVIVAL_HERB);
    RogueQuery_Exclude(ITEM_REVIVE);
    RogueQuery_Exclude(ITEM_MAX_REVIVE);
    RogueQuery_Exclude(ITEM_ESCAPE_ROPE);

    RogueQuery_ItemsExcludeRange(FIRST_MAIL_INDEX, LAST_MAIL_INDEX);
    RogueQuery_ItemsExcludeRange(ITEM_RED_SCARF, ITEM_YELLOW_SCARF);
    RogueQuery_ItemsExcludeRange(ITEM_RED_SHARD, ITEM_GREEN_SHARD);
    RogueQuery_ItemsExcludeRange(ITEM_BLUE_FLUTE, ITEM_WHITE_FLUTE);

    //ITEM_SACRED_ASH

    if(difficultyLevel <= 1)
    {
        RogueQuery_ItemsNotInPocket(POCKET_TM_HM);
    }

    if(difficultyLevel <= 3)
    {
        RogueQuery_ItemsNotHeldItem();
    }

    RogueQuery_CollapseItemBuffer();
    queryCount = RogueQuery_BufferSize();

#ifdef ROGUE_DEBUG
    gDebug_ItemOptionCount = queryCount;
#endif

    // These VARs aren't sequential
    VarSet(VAR_ROGUE_ITEM0, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM1, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM2, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM3, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM4, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM5, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM6, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM7, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM8, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM9, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM10, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM11, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM12, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM13, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM14, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM15, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM16, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM17, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM18, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
}

static void RandomiseEnabledItems(void)
{
    s32 i;
    u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);

    for(i = 0; i < ROGUE_ITEM_COUNT; ++i)
    {
        if(RandomChanceItem())
        {
            // Clear flag to show
            FlagClear(FLAG_ROGUE_ITEM_START + i);
        }
        else
        {
            // Set flag to hide
            FlagSet(FLAG_ROGUE_ITEM_START + i);
        }
    }

    RandomiseItemContent(difficultyLevel);
}

// Only take up to Sitrus berry, as past that is just misc non-battle related berries or reskins
#define BERRY_COUNT (ITEM_SITRUS_BERRY - FIRST_BERRY_INDEX + 1)

static void RandomiseBerryTrees(void)
{
    s32 i;

    for (i = 0; i < BERRY_TREES_COUNT; i++)
    {
        if(RandomChanceBerry())
        {
            u8 berryItem = FIRST_BERRY_INDEX + RogueRandomRange(BERRY_COUNT, FLAG_SET_SEED_ITEMS);
            u8 berry = ItemIdToBerryType(berryItem);
            PlantBerryTree(i, berry, BERRY_STAGE_BERRIES, FALSE);
        }
        else
        {
            RemoveBerryTree(i);
        }
    }
}