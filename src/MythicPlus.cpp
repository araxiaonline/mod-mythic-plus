#include "MythicPlus.h"
#include "MpDataStore.h"
#include "MpLogger.h"
#include "ObjectMgr.h"
#include "MapMgr.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "Group.h"
#include "Map.h"
#include "Unit.h"
#include "Creature.h"
#include "SpellInfo.h"

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

bool MythicPlus::EligibleHealTarget(Unit* target)
{
    if (!target) {
        return false;
    }

    if (target->GetTypeId() == TYPEID_CORPSE || target->GetTypeId() == TYPEID_GAMEOBJECT) {
        return false;
    }

    #if defined(MOD_PRESENT_NPCBOTS)
        if (target->IsNPCBot()) {
            return false;
        }

        if ((target->IsPet() || target->IsSummon() || target->IsHunterPet()) && target->GetOwner()->IsNPCBot()) {
            return false;
        }
    #endif

    if(sMythicPlus->IsCreatureEligible(target->ToCreature())) {
        return true;
    }

    return false;
}

bool MythicPlus::EligibleDamageTarget(Unit* target)
{
    if (!target) {
        return false;
    }

    if (target->GetTypeId() == TYPEID_PLAYER) {
        MpLogger::debug("Target {} is a player", target->GetName());
        return true;
    }

    #if defined(MOD_PRESENT_NPCBOTS)
        if (target->IsNPCBot()) {
            return true;
        }

        if ((target->IsPet() || target->IsSummon() || target->IsHunterPet()) && target->GetOwner()->IsNPCBot()) {
            return true;
        }
    #endif

    Creature* creature = target->ToCreature();
    if((creature->IsPet() || creature->IsSummon() || creature->IsHunterPet()) && creature->IsControlledByPlayer()) {
        return true;
    }

    return false;
}

bool MythicPlus::IsCreatureEligible(Creature* creature)
{
    if(!creature) {
        return false;
    }

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
        if (creature->IsNPCBot()) {
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
    uint32 origLevel = creature->GetLevel();
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

    // Scales the creatures mana
    uint32 mana = uint32(std::ceil(stats->BaseMana * cInfo->ModMana));
    creature->SetCreateMana(mana);
    creature->SetMaxPower(POWER_MANA, mana);
    creature->SetPower(POWER_MANA, mana);

    creature->SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, (float)health);
    creature->SetModifierValue(UNIT_MOD_MANA, BASE_VALUE, (float)mana);

    // Normalize the damage from earlier expansions
    float cTemplateDmgMult = cInfo->DamageModifier;
    if (origLevel <= 60 && cTemplateDmgMult < 3.0f) {
        cTemplateDmgMult = 4.0f;
    }
    if (origLevel <= 70 && origLevel > 60 && cTemplateDmgMult < 4.0f) {
        cTemplateDmgMult = 4.0f;
    }

    float basedamage = uint32(std::ceil(stats->BaseDamage[EXPANSION_WRATH_OF_THE_LICH_KING]));
    float creatureTypeMult = GetDamageModifier(rank);
    float weaponBaseMinDamage = basedamage * cTemplateDmgMult * creatureTypeMult * multipliers->baseDamage;
    float weaponBaseMaxDamage = weaponBaseMinDamage * 1.5f;

    creature->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, weaponBaseMinDamage);
    creature->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, weaponBaseMaxDamage);
    creature->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, weaponBaseMinDamage);
    creature->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, weaponBaseMaxDamage);
    creature->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, weaponBaseMinDamage);
    creature->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, weaponBaseMaxDamage);

    creature->ApplyAttackTimePercentMod(BASE_ATTACK, 0.75, true);
    creature->ApplyAttackTimePercentMod(OFF_ATTACK, 0.95, true);
    creature->ApplyCastTimePercentMod(0.80, true);
    creature->UpdateAllStats();

    // Scale up the armor with some variance also to make some tougher enemies in the mix
    uint32 armor = uint32(std::ceil(stats->BaseArmor * multipliers->armor * cInfo->ModArmor));
    creature->SetArmor(armor);

    /**
     * @TODO Explore scaling other variable stats based on the creature type at a later date.
     */
    // creature->SetModifierValue(UNIT_MOD_ATTACK_POWER, BASE_VALUE, stats->AttackPower * multipliers->melee);
    // creature->SetModifierValue(UNIT_MOD_ATTACK_POWER_RANGED, BASE_VALUE, stats->RangedAttackPower * multipliers->melee);
}

int32 ScaleDamageSpell(SpellInfo const * spellInfo, MpCreatureData* creatureData, Creature* creature, float damageMultiplier)
{
    if (!spellInfo) {
        MpLogger::error("Invalid spell info ScaleDamageSpell()");
        return;
    }

    if(!creatureData) {
        MpLogger::error("Invalid creature data ScaleDamageSpell()");
        return;
    }

    if(!creature) {
        MpLogger::error("Invalid creature ScaleDamageSpell()");
        return;
    }

    int32 originalLevel = creatureData->originalLevel;
    int32 currentLevel = creature->GetLevel();
    int32 totalDamage = 0;

    // Calculate the scaling factor using the 1.8 exponent
    float scalingFactor = pow(float(originalLevel / originalLevel), 1.8f);
    auto effects = spellInfo->GetEffects();

    // Loop through all spell effects to scale their base damage
    for (uint8 i = 0; i < effects.size(); ++i)
    {
        SpellEffectInfo effect = effects[i];
        totalDamage += effect.CalcValue(creature, nullptr,nullptr);
    }

    // Apply scaling factor and the set multiplier from the instance data
    totalDamage = int32(totalDamage * scalingFactor * damageMultiplier);
    return totalDamage;
}

int32 ScaleHealSpell(SpellInfo const * spellInfo, MpCreatureData* creatureData, Creature* creature, Creature* target, float healMultiplier)
{
    if (!spellInfo) {
        MpLogger::error("Invalid spell info ScaleHealSpell()");
        return;
    }

    if(!creatureData) {
        MpLogger::error("Invalid creature data ScaleHealSpell()");
        return;
    }

    if(!creature) {
        MpLogger::error("Invalid creature ScaleHealSpell()");
        return;
    }

    if(!target) {
        MpLogger::error("Invalid target ScaleHealSpell()");
        return;
    }

    int32 originalHp = creatureData->originalStats->BaseHealth[EXPANSION_WRATH_OF_THE_LICH_KING];
    int32 currentHealth = creature->GetHealth();
    int32 totalHeal = 0;

    auto effects = spellInfo->GetEffects();
    // Loop through all spell effects to scale their base healing
    for (uint8 i = 0; i < effects.size(); ++i)
    {
        SpellEffectInfo effect = effects[i];
        totalHeal += effect.CalcValue(creature, nullptr, target);
    }

    // Apply scaling factor and the set multiplier from the instance data
    return pow((totalHeal / originalHp) * currentHealth, 0.8f) * healMultiplier;
    return totalHeal;
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
