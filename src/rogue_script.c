#include "global.h"
#include "constants/battle.h"
#include "constants/items.h"
#include "constants/rogue.h"

#include "event_data.h"
#include "item_menu.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "random.h"

#include "rogue_baked.h"
#include "rogue_controller.h"
#include "rogue_query.h"

#include "rogue.h"
#include "rogue_script.h"
#include "rogue_quest.h"

bool8 Rogue_CheckPartyHasRoomForMon(void)
{
    if(Rogue_IsRunActive())
    {
        u8 partySize = VarGet(VAR_ROGUE_MAX_PARTY_SIZE);

        // We don't actually want to shift around the party as that can cause issue in EX
        // Where stuff like megas keep track of exact indicies which mega evolve
        //RemoveAnyFaintedMons(FALSE);

        if (CalculatePlayerPartyCount() >= partySize)
        {
            return FALSE;
        }
    }

    return TRUE;
}

void Rogue_SeedRandomGenerators(void)
{
    u32 startSeed = gRngRogueValue;

    RogueRandom();
    SeedRng(RogueRandom());
    SeedRng2(RogueRandom());

    gRngRogueValue = startSeed;
}

void Rogue_RandomisePartyMon(void)
{
    u8 monIdx = gSpecialVar_0x8004;

    if(monIdx == 255)
    {
        // Entire team
        u8 i;
        u16 queryCount;
        u16 species;
        u16 heldItem;
        u8 targetlevel = GetMonData(&gPlayerParty[0], MON_DATA_LEVEL);

        // Query for the current route type
        RogueQuery_Clear();

        RogueQuery_SpeciesIsValid();
        RogueQuery_SpeciesExcludeCommon();

        if(gRogueRun.currentDifficulty < 2)
            RogueQuery_SpeciesIsNotLegendary();

        RogueQuery_TransformToEggSpecies();

        // Evolve the species to just below the wild encounter level
        RogueQuery_EvolveSpeciesAndKeepPreEvo(targetlevel, TRUE);

        queryCount = RogueQuery_UncollapsedSpeciesSize();

        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            targetlevel = GetMonData(&gPlayerParty[i], MON_DATA_LEVEL);
            heldItem = GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM);

            species = RogueQuery_AtUncollapsedIndex(Random() % queryCount);

            ZeroMonData(&gPlayerParty[i]);
            CreateMon(&gPlayerParty[i], species, targetlevel, USE_RANDOM_IVS, 0, 0, OT_ID_PLAYER_ID, 0);

            SetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, &heldItem);
        }
    }
    else
    {
        u16 queryCount;
        u16 species;
        u8 targetlevel = GetMonData(&gPlayerParty[monIdx], MON_DATA_LEVEL);
        u16 heldItem = GetMonData(&gPlayerParty[monIdx], MON_DATA_HELD_ITEM);

        // Query for the current route type
        RogueQuery_Clear();

        RogueQuery_SpeciesIsValid();
        RogueQuery_SpeciesExcludeCommon();

        if(gRogueRun.currentDifficulty < 2)
            RogueQuery_SpeciesIsNotLegendary();

        RogueQuery_TransformToEggSpecies();

        // Evolve the species to just below the wild encounter level
        RogueQuery_EvolveSpeciesAndKeepPreEvo(targetlevel, TRUE);

        queryCount = RogueQuery_UncollapsedSpeciesSize();
        species = RogueQuery_AtUncollapsedIndex(Random() % queryCount);

        ZeroMonData(&gPlayerParty[monIdx]);
        CreateMon(&gPlayerParty[monIdx], species, targetlevel, USE_RANDOM_IVS, 0, 0, OT_ID_PLAYER_ID, 0);

        SetMonData(&gPlayerParty[monIdx], MON_DATA_HELD_ITEM, &heldItem);
    }
}

void Rogue_AlterMonIVs(void)
{
    const u16 delta = 10;

    u16 statId;
    u16 ivAmount;
    u16 monIdx = gSpecialVar_0x8004;
    u16 statOp = gSpecialVar_0x8005;

    if(monIdx == 255)
    {
        // Entire team
        u8 i;

        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            for(statId = MON_DATA_HP_IV; statId <= MON_DATA_SPDEF_IV; ++statId)
            {
                ivAmount = GetMonData(&gPlayerParty[i], statId);

                if(statOp == 0)
                {
                    ivAmount += delta;
                    ivAmount = min(31, ivAmount);
                }
                else
                {
                    if(ivAmount < delta)
                        ivAmount = 0;
                    else
                        ivAmount -= delta;
                }

                SetMonData(&gPlayerParty[i], statId, &ivAmount);
                CalculateMonStats(&gPlayerParty[i]);
            }
        }
    }
    else
    {
        // Modify just 1 mon
        for(statId = MON_DATA_HP_IV; statId <= MON_DATA_SPDEF_IV; ++statId)
        {
            ivAmount = GetMonData(&gPlayerParty[monIdx], statId);

            if(statOp == 0)
            {
                ivAmount += delta;
                ivAmount = min(31, ivAmount);
            }
            else
            {
                if(ivAmount < delta)
                    ivAmount = 0;
                else
                    ivAmount -= delta;
            }

            SetMonData(&gPlayerParty[monIdx], statId, &ivAmount);
            CalculateMonStats(&gPlayerParty[monIdx]);
        }
    }
}

void Rogue_ApplyStatusToMon(void)
{
    u16 statusAilment;
    u16 monIdx = gSpecialVar_0x8004;

    switch(gSpecialVar_0x8005)
    {
        case 0:
            statusAilment = STATUS1_POISON;
            break;

        case 1:
            statusAilment = STATUS1_PARALYSIS;
            break;

        case 2:
            statusAilment = STATUS1_SLEEP;
            break;

        case 3:
            statusAilment = STATUS1_FREEZE;
            break;

        case 4:
            statusAilment = STATUS1_BURN;
            break;
    }

    if(monIdx == 255)
    {
        // Entire team
        u8 i;

        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            SetMonData(&gPlayerParty[i], MON_DATA_STATUS, &statusAilment);
        }
    }
    else
    {
        SetMonData(&gPlayerParty[monIdx], MON_DATA_STATUS, &statusAilment);
    }
}

void Rogue_ReducePartySize(void)
{
    u16 monIdx = gSpecialVar_0x8004;

    if(monIdx < gPlayerPartyCount)
    {
        RemoveMonAtSlot(monIdx, TRUE, TRUE);
    }

    VarSet(VAR_ROGUE_MAX_PARTY_SIZE, VarGet(VAR_ROGUE_MAX_PARTY_SIZE) - 1);
}

u16 Rogue_GetMonEvoCount(void)
{
    u16 monIdx = gSpecialVar_0x8004;
    u16 species = GetMonData(&gPlayerParty[monIdx], MON_DATA_SPECIES);

    if(species != SPECIES_NONE)
    {
        u16 e;
        struct Evolution evo;
        u16 count = 0;

        for (e = 0; e < EVOS_PER_MON; e++)
        {
            Rogue_ModifyEvolution(species, e, &evo);

            if (evo.targetSpecies != SPECIES_NONE)
            {
#ifdef ROGUE_EXPANSION
                if(evo.method != EVO_MEGA_EVOLUTION &&
                    evo.method != EVO_MOVE_MEGA_EVOLUTION &&
                    evo.method != EVO_PRIMAL_REVERSION
                )
#endif
                {
                    ++count;
                }
            }
        }

        return count;
    }

    return 0;
}

void Rogue_GetMonEvoParams(void)
{
    u16 monIdx = gSpecialVar_0x8004;
    u16 evoIdx = gSpecialVar_0x8005;
    u16 species = GetMonData(&gPlayerParty[monIdx], MON_DATA_SPECIES);

    gSpecialVar_0x8006 = 0;
    gSpecialVar_0x8007 = 0;

    if(species != SPECIES_NONE)
    {        // evoIdx doesn't mean array idx annoyingly as evos can be toggled/changed
        u16 e;
        struct Evolution evo;
        u16 count = 0;

        for (e = 0; e < EVOS_PER_MON; e++)
        {
            Rogue_ModifyEvolution(species, e, &evo);

            if (evo.targetSpecies != SPECIES_NONE)
            {
#ifdef ROGUE_EXPANSION
                if(evo.method != EVO_MEGA_EVOLUTION &&
                    evo.method != EVO_MOVE_MEGA_EVOLUTION &&
                    evo.method != EVO_PRIMAL_REVERSION
                )
#endif
                {
                    if(count++ == evoIdx)
                    {
                        gSpecialVar_0x8006 = evo.method;
                        gSpecialVar_0x8007 = evo.param;
                        return;
                    }
                }
            }
        }
    }
}

void RogueDebug_FillGenPC(void)
{
#ifdef ROGUE_DEBUG
    u8 i;
    u16 species;
    u16 writeIdx = 0;
    u16 genId = gSpecialVar_0x8004;
    struct Pokemon mon;

    for(species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
    {
        if(SpeciesToGen(species) == genId)
        {
            struct Pokemon mon;
            u16 currIdx = writeIdx++;
            u16 targetBox = currIdx / IN_BOX_COUNT;
            u16 boxIndex = currIdx % IN_BOX_COUNT;

            GetSetPokedexFlag(species, FLAG_SET_SEEN);
            GetSetPokedexFlag(species, FLAG_SET_CAUGHT);

            CreateMon(&mon, species, 5, MAX_PER_STAT_IVS, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);

            SetBoxMonAt(targetBox, boxIndex, &mon.box);
        }
    }

    // Clear a box of space
    for(i = 0; i < IN_BOX_COUNT; ++i)
    {
        u16 currIdx = writeIdx++;
        u16 targetBox = currIdx / IN_BOX_COUNT;
        u16 boxIndex = currIdx % IN_BOX_COUNT;
        ZeroBoxMonAt(targetBox, boxIndex);
    }
#endif
}

enum BerryTreatBuff
{
    BERRY_BUFF_FRIEND,
    BERRY_BUFF_HP,
    BERRY_BUFF_ATK,
    BERRY_BUFF_DEF,
    BERRY_BUFF_SPD,
    BERRY_BUFF_SPATK,
    BERRY_BUFF_SPDEF,
    BERRY_BUFF_WEAKEN,
};

static u8 BerryItemToTreatBuff(u16 berryItem)
{
    switch(berryItem)
    {
        case ITEM_ORAN_BERRY:
        case ITEM_SITRUS_BERRY:
        case ITEM_POMEG_BERRY:
            return BERRY_BUFF_HP;

        case ITEM_KELPSY_BERRY:
            return BERRY_BUFF_ATK;

        case ITEM_QUALOT_BERRY:
            return BERRY_BUFF_DEF;

        case ITEM_HONDEW_BERRY:
            return BERRY_BUFF_SPATK;

        case ITEM_GREPA_BERRY:
            return BERRY_BUFF_SPDEF;

        case ITEM_TAMATO_BERRY:
        case ITEM_SALAC_BERRY:
            return BERRY_BUFF_SPD;

        case ITEM_LEPPA_BERRY:
            return BERRY_BUFF_WEAKEN;

#ifdef ROGUE_EXPANSION
        case ITEM_LIECHI_BERRY:
            return BERRY_BUFF_ATK;

        case ITEM_GANLON_BERRY:
            return BERRY_BUFF_DEF;

        case ITEM_PETAYA_BERRY:
            return BERRY_BUFF_SPATK;

        case ITEM_APICOT_BERRY:
            return BERRY_BUFF_SPDEF;
#endif
        default:
            return BERRY_BUFF_FRIEND;
    }
}

void Rogue_CheckBerryTreat(void)
{
    u16 itemId = gSpecialVar_ItemId;
    gSpecialVar_Result = BerryItemToTreatBuff(itemId);
}

void Rogue_ApplyBerryTreat(void)
{
    u16 monIdx = gSpecialVar_0x8004;
    u16 itemId = gSpecialVar_ItemId;
    u16 buffAmount = gSpecialVar_0x8005;
    u16 berryBuff = BerryItemToTreatBuff(itemId);

    if(berryBuff == BERRY_BUFF_FRIEND)
    {
        u16 friendship = GetMonData(&gPlayerParty[monIdx], MON_DATA_FRIENDSHIP);

        if(friendship < MAX_FRIENDSHIP)
        {
            gSpecialVar_Result = TRUE;

            friendship += buffAmount * 10;
            if(friendship > MAX_FRIENDSHIP)
                friendship = MAX_FRIENDSHIP;

            SetMonData(&gPlayerParty[monIdx], MON_DATA_FRIENDSHIP, &friendship);
            CalculateMonStats(&gPlayerParty[monIdx]);
        }
        else
        {
            gSpecialVar_Result = FALSE;
        }
    }
    else if(berryBuff == BERRY_BUFF_WEAKEN)
    {
        u16 statOffset;
        u16 ivCount;

        gSpecialVar_Result = FALSE;

        for(statOffset = 0; statOffset < 6; ++statOffset)
        {
            u16 ivCount = GetMonData(&gPlayerParty[monIdx], MON_DATA_HP_IV + statOffset);

            if(ivCount != 0)
            {
                gSpecialVar_Result = TRUE;

                if(ivCount < buffAmount)
                    ivCount = 0;
                else
                    ivCount -= buffAmount;

                SetMonData(&gPlayerParty[monIdx], MON_DATA_HP_IV + statOffset, &ivCount);
            }
        }

        CalculateMonStats(&gPlayerParty[monIdx]);
    }
    else
    {
        u16 statOffset = berryBuff - 1;
        u16 ivCount = GetMonData(&gPlayerParty[monIdx], MON_DATA_HP_IV + statOffset);

        if(ivCount < 31)
        {
            gSpecialVar_Result = TRUE;

            ivCount += buffAmount;
            if(ivCount > 31)
                ivCount = 31;

            SetMonData(&gPlayerParty[monIdx], MON_DATA_HP_IV + statOffset, &ivCount);
            CalculateMonStats(&gPlayerParty[monIdx]);
        }
        else
        {
            gSpecialVar_Result = FALSE;
        }
    }
}

void Rogue_GetBufferedShinySpecies(void)
{
    u16 i;
    u16 offset = gSpecialVar_0x8004;

    for(i = 0; i < ARRAY_COUNT(gRogueRun.safariShinyBuffer); ++i)
    {
        if(gRogueRun.safariShinyBuffer[i] != (u16)-1)
        {
            if(offset == 0)
            {
                gSpecialVar_Result = gRogueRun.safariShinyBuffer[i];
                return;
            }
            else
                --offset;
        }
    }

    gSpecialVar_Result = SPECIES_NONE;
}

void Rogue_AnyNewQuestsPending(void)
{
    gSpecialVar_Result = AnyNewQuestsPending();
}