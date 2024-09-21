#include "MythicPlus.h"
#include "MpDataStore.h"
#include "MpLogger.h"
#include "ObjectMgr.h"
#include "MapMgr.h"

bool MythicPlus::IsMapEligible(Map* map)
{
    if (!Enabled) {
        return false;
    }

    if (map->IsDungeon()) {
        return true;
    }

    return false;
}

bool MythicPlus::IsDifficultySet(Player* player)
{
    Group* group = player->GetGroup();
    if (!group) {
        return false;
    }

    MpGroupData const* groupData = sMpDataStore->GetGroupData(group->GetGUID());
    if (!groupData) {
        return false;
    }

    return true;
}


bool MythicPlus::IsDifficultyEnabled(std::string difficulty)
{
    return std::find(enabledDifficulties.begin(), enabledDifficulties.end(), difficulty) != enabledDifficulties.end();
}

bool MythicPlus::IsDungeonDisabled(uint32 dungeon)
{
    return std::find(disabledDungeons.begin(), disabledDungeons.end(), dungeon) != disabledDungeons.end();
}

bool MythicPlus::IsCreatureEligible(Creature* creature)
{
    if (creature->IsDungeonBoss()) {
        return true;
    }

    if ((creature->IsHunterPet() || creature->IsPet() || creature->IsSummon()) && creature->IsControlledByPlayer()) {
        return false;
    }

    if (creature->IsCritter() || creature->IsTotem() || creature->IsTrigger()) {
        return false;
    }

    # if defined(MOD_PRESENT_NPCBOTS)
        if (creature->IsNpcBot()) {
            return false;
        }
    # endif

    // throw out NPCs
    if  ((creature->IsVendor() ||
            creature->HasNpcFlag(UNIT_NPC_FLAG_GOSSIP) ||
            creature->HasNpcFlag(UNIT_NPC_FLAG_QUESTGIVER) ||
            creature->HasNpcFlag(UNIT_NPC_FLAG_TRAINER) ||
            creature->HasNpcFlag(UNIT_NPC_FLAG_TRAINER_PROFESSION) ||
            creature->HasNpcFlag(UNIT_NPC_FLAG_REPAIR) ||
            creature->HasUnitFlag(UNIT_FLAG_IMMUNE_TO_PC) ||
            creature->HasUnitFlag(UNIT_FLAG_NOT_SELECTABLE)) &&
            (!creature->IsDungeonBoss())
        )
    {
        return false;
    }

    return true;
}

void MythicPlus::AddCreatureForScaling(Creature* creature)
{
    if (!IsCreatureEligible(creature)) {
        return;
    }

    sMpDataStore->AddCreatureData(creature->GetGUID(), MpCreatureData(creature));
    MpLogger::debug("Added creature {} to instance data for instance {}",
        creature->GetName(),
        creature->GetMap()->GetMapName()
    );
}

void MythicPlus::ScaleOnUpdate(Creature* creature, uint32 diff)
{
    // Check to see if the creature is in our list to be scaled to mythic+ levels
    MpCreatureData* creatureData = sMpDataStore->GetCreatureData(creature->GetGUID());
    if (!creatureData) {
        return;
    }

    // if it has already been scaled then do nothing.
    if (creatureData->IsScaled()) {
        MpLogger::debug("Creature {} Entry {} Id {} already scaled in ScaleOnUpdate()",
            creature->GetName(),
            creature->GetEntry(),
            creature->GetGUID().GetCounter()
        );
        return;
    }

    // Otherwise we handle scaling the stats of the creature based on map difficulty set
    Map* map = creature->GetMap();
    if(!map) {
        MpLogger::warn("Map is null for creature {} Entry {} Id {} could not scale in ScaleOnUpdate()",
            creature->GetName(),
            creature->GetEntry(),
            creature->GetGUID().GetCounter()
        );
        return;
    }

    // The instance data for this map determines how to scale the creature by difficulty set
    MpInstanceData* instanceData = sMpDataStore->GetInstanceData(map->GetId(), map->GetInstanceId());
    if (!instanceData) {
        MpLogger::warn("Instance data is null for creature {} Entry {} Id {} could not scale in ScaleOnUpdate()",
            creature->GetName(),
            creature->GetEntry(),
            creature->GetGUID().GetCounter()
        );
        return;
    }

    // allow small variance in level for non-boss creatures
    uint8 level = uint8(urand(instanceData->creature.avgLevel - 1, instanceData->creature.avgLevel + 1));
    if(creature->IsDungeonBoss()) {
        level = instanceData->boss.avgLevel;
    }
    ScaleCreature(level, creature);
    creatureData->SetScaled(true);  // Since updates happen frequently set the flag so we do not re-scale

    MpLogger::debug("Sca Creature {} Entry {} Id {} level from {} to {}",
        creature->GetName(),
        creature->GetEntry(),
        creature->GetGUID().GetCounter(),
        creature->GetLevel(),
        level
    );
}

void MythicPlus::ScaleCreature(uint8 level, Creature* creature)
{
    creature->SetLevel(level);
    CreatureBaseStats const* stats = sObjectMgr->GetCreatureBaseStats(
        level,
        creature->GetCreatureTemplate()->unit_class
    );

    int32 rank = 0;
    CreatureTemplate const* cInfo = creature->GetCreatureTemplate();
    if(cInfo && cInfo->rank > 0) {
        rank = cInfo->rank;
    }

    // Scales the creatures hitpoints
    float healthmod = GetHealthModifier(rank);
    uint32 basehp = std::max<uint32>(1, stats->GenerateHealth(cInfo));
    uint32 health = uint32(basehp * healthmod);

    creature->SetCreateHealth(health);
    creature->SetMaxHealth(health);
    creature->SetHealth(health);
    creature->ResetPlayerDamageReq();

    // Scales the creatures mana
    uint32 mana = stats->GenerateMana(cInfo);
    creature->SetCreateMana(mana);
    creature->SetMaxPower(POWER_MANA, mana);
    creature->SetPower(POWER_MANA, mana);

    creature->SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, (float)health);
    creature->SetModifierValue(UNIT_MOD_MANA, BASE_VALUE, (float)mana);

    // Scales the damage
    float basedamage = stats->GenerateBaseDamage(cInfo);

    float weaponBaseMinDamage = basedamage;
    float weaponBaseMaxDamage = basedamage * 1.5;

    creature->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, weaponBaseMinDamage);
    creature->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, weaponBaseMaxDamage);
    creature->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, weaponBaseMinDamage);
    creature->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, weaponBaseMaxDamage);
    creature->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, weaponBaseMinDamage);
    creature->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, weaponBaseMaxDamage);

    creature->SetModifierValue(UNIT_MOD_ATTACK_POWER, BASE_VALUE, stats->AttackPower);
    creature->SetModifierValue(UNIT_MOD_ATTACK_POWER_RANGED, BASE_VALUE, stats->RangedAttackPower);
}

/**
 * Function is copied because was not accessible in core creature class
 */
float GetHealthModifier(int32 Rank)
{
    switch (Rank)
    {
        case CREATURE_ELITE_NORMAL:
            return sWorld->getRate(RATE_CREATURE_NORMAL_HP);
        case CREATURE_ELITE_ELITE:
            return sWorld->getRate(RATE_CREATURE_ELITE_ELITE_HP);
        case CREATURE_ELITE_RAREELITE:
            return sWorld->getRate(RATE_CREATURE_ELITE_RAREELITE_HP);
        case CREATURE_ELITE_WORLDBOSS:
            return sWorld->getRate(RATE_CREATURE_ELITE_WORLDBOSS_HP);
        case CREATURE_ELITE_RARE:
            return sWorld->getRate(RATE_CREATURE_ELITE_RARE_HP);
        default:
            return sWorld->getRate(RATE_CREATURE_ELITE_ELITE_HP);
    }
}
