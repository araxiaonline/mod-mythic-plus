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

bool MythicPlus::IsDifficultySet(Player const* player)
{
    Group const* group = player->GetGroup();
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

bool MythicPlus::EligibleTarget(Unit* target)
{
    if (!target) {
        return false;
    }

MpLogger::debug("EligibleTarget: target: {} BOT?{}", target->GetName(), target->IsNPCBot());
    if (target->GetTypeId() == TYPEID_PLAYER) {
        return true;
    }

    # if defined(MOD_PRESENT_NPCBOTS)
    MpLogger::debug("IN BOT DEFINED STUFF: target: {} BOT?{}", target->GetName(), target->IsNPCBot());
        if (target->IsNPCBot()) {
            MpLogger::debug("Target {} is an NPC eligible to be smacked hard", target->GetName());
            return true;
        }

        if ((target->IsPet() || creature->IsSummon() || creature->IsHunterPet()) && target->GetOwner()->IsNPCBot()) {
            return true;
        }
    # endif

    Creature* creature = target->ToCreature();
    if((creature->IsPet() || creature->IsSummon() || creature->IsHunterPet()) && creature->IsControlledByPlayer()) {
        return true;
    }

    return false;
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
            MpLogger::debug("Creature {} is an NPC Bot do not scale", creature->GetName());
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

void MythicPlus::AddScaledCreature(Creature* creature, MpInstanceData* instanceData)
{
    // allow small variance in level for non-boss creatures
    uint8 level = uint8(urand(instanceData->creature.avgLevel - 1, instanceData->creature.avgLevel + 1));
    if(creature->IsDungeonBoss()) {
        ScaleCreature(instanceData->boss.avgLevel, creature, &instanceData->boss);
    } else {
        ScaleCreature(level, creature, &instanceData->creature);
    }

    MpCreatureData creatureData = MpCreatureData(creature);
    creatureData.SetScaled(true);
    sMpDataStore->AddCreatureData(creature->GetGUID(), creatureData);

    MpLogger::debug("Scaled Creature {} Entry {} Id {} level from {} to {}",
        creature->GetName(),
        creature->GetEntry(),
        creature->GetGUID().GetCounter(),
        creature->GetLevel(),
        level
    );
}

void MythicPlus::ScaleRemaining(Player* player, MpInstanceData* instanceData)
{
    std::vector<MpCreatureData*> creatures = sMpDataStore->GetUnscaledCreatures(player->GetMapId(), player->GetInstanceId());
    for (MpCreatureData* creatureData : creatures) {
        AddScaledCreature(creatureData->creature, instanceData);
    }
}

// Perform any memory cleanup when the creature is removed from the world and no longer needed.
void MythicPlus::RemoveCreature(Creature* creature)
{
    MpCreatureData* creatureData = sMpDataStore->GetCreatureData(creature->GetGUID());
    if (!creatureData) {
        return;
    }

    sMpDataStore->RemoveCreatureData(creature->GetGUID());
}

void MythicPlus::ScaleCreature(uint8 level, Creature* creature, MpMultipliers* multipliers)
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
    // Add some variance to the healthpool so enemies are not all the same
    float healthVariation = frand(0.85f, 1.15f);
    uint32 basehp = uint32(std::ceil(stats->BaseHealth[EXPANSION_WRATH_OF_THE_LICH_KING] * cInfo->ModHealth));
    uint32 health = uint32(basehp * healthmod * multipliers->health * healthVariation);

    creature->SetCreateHealth(health);
    creature->SetMaxHealth(health);
    creature->SetHealth(health);
    creature->ResetPlayerDamageReq();

    // Scale up the armor with some variance also to make some tougher enemies in the mix
    uint32 armor = uint32(std::ceil(stats->BaseArmor * cInfo->ModArmor * multipliers->armor));
    creature->SetArmor(armor);

    // Scales the creatures mana
    uint32 mana = uint32(std::ceil(stats->BaseMana * cInfo->ModMana));
    creature->SetCreateMana(mana);
    creature->SetMaxPower(POWER_MANA, mana);
    creature->SetPower(POWER_MANA, mana);

    creature->SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, (float)health);
    creature->SetModifierValue(UNIT_MOD_MANA, BASE_VALUE, (float)mana);

    // Scales the damage based on the melee multiplier
    float basedamage = uint32(std::ceil(stats->BaseDamage[EXPANSION_WRATH_OF_THE_LICH_KING] * cInfo->DamageModifier));
    float creatureTypeMult = GetDamageModifier(rank);
    float weaponBaseMinDamage = basedamage * multipliers->melee * creatureTypeMult;
    float weaponBaseMaxDamage = basedamage * multipliers->melee * creatureTypeMult * 1.5;

    creature->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, weaponBaseMinDamage);
    creature->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, weaponBaseMaxDamage);
    creature->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, weaponBaseMinDamage);
    creature->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, weaponBaseMaxDamage);
    creature->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, weaponBaseMinDamage);
    creature->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, weaponBaseMaxDamage);

    creature->SetModifierValue(UNIT_MOD_ATTACK_POWER, BASE_VALUE, stats->AttackPower * multipliers->melee);
    creature->SetModifierValue(UNIT_MOD_ATTACK_POWER_RANGED, BASE_VALUE, stats->RangedAttackPower * multipliers->melee);
    MpLogger::debug("Scaled creature base damage from {} to {}", weaponBaseMinDamage, weaponBaseMaxDamage);
    creature->UpdateAllStats();
    MpLogger::debug("Scaled creature reported base damage from {} to {}", creature->GetWeaponDamageRange(BASE_ATTACK, MINDAMAGE), creature->GetWeaponDamageRange(BASE_ATTACK, MAXDAMAGE));


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

float GetDamageModifier(int32 Rank)
{
    switch (Rank)
    {
        case CREATURE_ELITE_NORMAL:
            return sWorld->getRate(RATE_CREATURE_NORMAL_DAMAGE);
        case CREATURE_ELITE_ELITE:
            return sWorld->getRate(RATE_CREATURE_ELITE_ELITE_DAMAGE);
        case CREATURE_ELITE_RAREELITE:
            return sWorld->getRate(RATE_CREATURE_ELITE_RAREELITE_DAMAGE);
        case CREATURE_ELITE_WORLDBOSS:
            return sWorld->getRate(RATE_CREATURE_ELITE_WORLDBOSS_DAMAGE);
        case CREATURE_ELITE_RARE:
            return sWorld->getRate(RATE_CREATURE_ELITE_RARE_DAMAGE);
        default:
            return sWorld->getRate(RATE_CREATURE_ELITE_ELITE_DAMAGE);
    }
}
