#include "MythicPlus.h"
#include "MpLogger.h"
#include "ObjectMgr.h"
#include "MapMgr.h"
#include "ScriptMgr.h"
#include "Group.h"
#include "Unit.h"

#include <algorithm>
#include <cmath>

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

        if ((target->IsPet() || target->IsSummon() || target->IsHunterPet()) && target->GetOwner() && target->GetOwner()->IsNPCBot()) {
            return true;
        }
    #endif

    Creature* creature = target->ToCreature();
    if (creature && (creature->IsPet() || creature->IsSummon() || creature->IsHunterPet()) && creature->GetOwner() && creature->IsControlledByPlayer()) {
        return true;
    }

    return false;
}

bool MythicPlus::IsCreatureEligible(Creature* creature)
{
    if (!creature) {
        return false;
    }

    if (creature->IsDungeonBoss()) {
        return true;
    }

    // Check if the creature is a pet or summon controlled by a player
    if ((creature->IsHunterPet() || creature->IsPet() || creature->IsSummon()) && creature->IsControlledByPlayer()) {
        return false;
    }

    // Skip critters, totems, and triggers
    if (creature->IsCritter() || creature->IsTotem() || creature->IsTrigger()) {
        return false;
    }

    #if defined(MOD_PRESENT_NPCBOTS)
        // Safely check if the creature is an NPC Bot
        if (creature->IsNPCBot()) {
            MpLogger::debug("Creature {} is an NPC Bot, do not scale", creature->GetName());
            return false;
        }
    #endif

    // Check for NPC-related flags (vendor, gossip, quest giver, trainer, etc.)
    if ((creature->IsVendor() ||
         creature->HasNpcFlag(UNIT_NPC_FLAG_GOSSIP) ||
         creature->HasNpcFlag(UNIT_NPC_FLAG_QUESTGIVER) ||
         creature->HasNpcFlag(UNIT_NPC_FLAG_TRAINER) ||
         creature->HasNpcFlag(UNIT_NPC_FLAG_TRAINER_PROFESSION) ||
         creature->HasNpcFlag(UNIT_NPC_FLAG_REPAIR) ||
         creature->HasUnitFlag(UNIT_FLAG_IMMUNE_TO_PC) ||
         creature->HasUnitFlag(UNIT_FLAG_NOT_SELECTABLE)) &&
        (!creature->IsDungeonBoss()))
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
        ScaleCreature(instanceData->boss.avgLevel, creature, &instanceData->boss, instanceData->difficulty);
    } else {
        ScaleCreature(level, creature, &instanceData->creature, instanceData->difficulty);
    }

    MpCreatureData creatureData = MpCreatureData(creature);
    creatureData.SetScaled(true);
    sMpDataStore->AddCreatureData(creature->GetGUID(), creatureData);

    // MpLogger::debug("Scaled Creature {} Entry {} Id {} level from {} to {}",
    //     creature->GetName(),
    //     creature->GetEntry(),
    //     creature->GetGUID().GetCounter(),
    //     creature->GetLevel(),
    //     level
    // );
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

void MythicPlus::ScaleCreature(uint8 level, Creature* creature, MpMultipliers* multipliers, MpDifficulty difficulty)
{
    uint32 origLevel = creature->GetLevel();
    CreatureTemplate const* cInfo = creature->GetCreatureTemplate();
    uint32 mapId = creature->GetMapId();

    creature->SetLevel(level);
    CreatureBaseStats const* stats = sObjectMgr->GetCreatureBaseStats(
        level,
        cInfo->unit_class
    );

    uint32 basehp = stats->BaseHealth[EXPANSION_WRATH_OF_THE_LICH_KING];
    uint32 health = CalculateNewHealth(cInfo, mapId, difficulty, basehp, multipliers->health);

    MpLogger::debug("Creature {} base health scaled from {} to {}",
        creature->GetName(),
        basehp,
        health
    );

    creature->SetCreateHealth(health);
    creature->SetMaxHealth(health);
    creature->SetHealth(health);
    creature->ResetPlayerDamageReq();
    creature->SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, (float)health);

    /**
     * @TODO: Figure out mana later for unit_types 2 and 8
     */
    uint32 mana = uint32(std::ceil(stats->BaseMana * cInfo->ModMana));
    creature->SetCreateMana(mana);
    creature->SetMaxPower(POWER_MANA, mana);
    creature->SetPower(POWER_MANA, mana);

    if(cInfo->unit_class == UNIT_CLASS_MAGE) {
        creature->SetModifierValue(UNIT_MOD_MANA, BASE_VALUE, (float)mana * 10.0f);
    }

    if(cInfo->unit_class == UNIT_CLASS_PALADIN) {
        creature->SetModifierValue(UNIT_MOD_MANA, BASE_VALUE, (float)mana * 3.0f);
    }

    // need to calcuate a new base weapon damage that makes sense for the level and class
    uint32 ap = stats->AttackPower;
    uint32 rangeAp = irand(115, 157);
    MpLogger::debug("Creature {} base attack power{}",
        creature->GetName(),
        ap
    );

    ap = pow(float(creature->GetLevel() / origLevel), 1.8f) * 1000;

    creature->SetModifierValue(UNIT_MOD_ATTACK_POWER, BASE_VALUE, ap);
    creature->SetModifierValue(UNIT_MOD_ATTACK_POWER_RANGED, BASE_VALUE, rangeAp);

    // This works out a bonus damage to apply to the mob using the database and original mod settings.
    // the thought behind this is some mobs in dungeons are intended to hit harder than others
    // so applying a flat bonus keeps the ratios the same but increases the overall difficulty.
    // Of course within reason.
    int32 damageBonus = sMpDataStore->GetDamageScaleFactor(mapId, difficulty);
    int32 maxBonus = sMpDataStore->GetMaxDamageScaleFactor(mapId, difficulty);
    float dmgMod = cInfo->DamageModifier + damageBonus;

    // Allow bosses to scale as high as they want but limit non-bosses to a max bonus
    if(!creature->IsDungeonBoss() && damageBonus > maxBonus) {
        dmgMod = maxBonus;
    }
    float oldDmgModifier = creature->GetModifierValue(UNIT_MOD_DAMAGE_MAINHAND, BASE_VALUE);
    creature->SetModifierValue(UNIT_MOD_DAMAGE_MAINHAND,BASE_VALUE, dmgMod);
    creature->SetModifierValue(UNIT_MOD_DAMAGE_OFFHAND,BASE_VALUE, dmgMod*0.85f);
    creature->SetModifierValue(UNIT_MOD_DAMAGE_RANGED,BASE_VALUE, dmgMod);

    MpLogger::debug("Creature new attack damage scaled from {} to {}",
        oldDmgModifier,
        dmgMod
    );

    creature->UpdateAllStats();

    // Scale up the armor with some variance also to make some tougher enemies in the mix
    uint32 armor = uint32(std::ceil(stats->BaseArmor * multipliers->armor * cInfo->ModArmor));
    creature->SetArmor(armor);

    /**
     * @TODO: Explore scaling other variable stats and resistances on the creature type at a later date.
     */
    // creature->SetModifierValue(UNIT_MOD_ATTACK_POWER, BASE_VALUE, stats->AttackPower * multipliers->melee);
    // creature->SetModifierValue(UNIT_MOD_ATTACK_POWER_RANGED, BASE_VALUE, stats->RangedAttackPower * multipliers->melee);
}

int32 MythicPlus::ScaleDamageSpell(SpellInfo const * spellInfo, MpCreatureData* creatureData, Creature* creature, float damageMultiplier)
{

    if (!spellInfo) {
        MpLogger::error("Invalid spell info ScaleDamageSpell()");
        return 0;
    }

    if(!creatureData) {
        MpLogger::error("Invalid creature data ScaleDamageSpell()");
        return 0;
    }

    if(!creature) {
        MpLogger::error("Invalid creature ScaleDamageSpell()");
        return 0;
    }

    int32 originalLevel = creatureData->originalLevel;
    int32 totalDamage = 0;

    // Calculate the scaling factor using the 1.8 exponent
    float scalingFactor = pow(float(creature->GetLevel()*1.5 / originalLevel), 1.8f);
    auto effects = spellInfo->GetEffects();

    // Loop through all spell effects to scale their base damage
    for (uint8 i = 0; i < effects.size(); ++i)
    {
        MpLogger::debug("Spell effect {} base points: {}", i, effects[i].BasePoints);
        SpellEffectInfo effect = effects[i];
        totalDamage += effect.CalcValue(creature, &effect.BasePoints, nullptr);

        MpLogger::debug("Spell effect {} total damage: {}", i, totalDamage);

        if(effect.IsAreaAuraEffect()) {
            MpLogger::debug("Skipping area aura effect");
            continue;
        }

        if(effect.IsAura()) {
            MpLogger::debug("Skipping aura effect");
            continue;
        }


        if(effect.IsEffect(SPELL_EFFECT_WEAPON_DAMAGE) || effect.IsEffect(SPELL_EFFECT_WEAPON_PERCENT_DAMAGE)) {
            MpLogger::debug("Skipping weapon damage effect");
            continue;
        }
    }

    // Apply scaling factor and the set multiplier from the instance data
    totalDamage = int32(totalDamage * scalingFactor * damageMultiplier);


    MpLogger::debug("Spell damage scaled from for spell New Damage: {} using: scaling Factor: {} and damangeMulti", totalDamage, scalingFactor, damageMultiplier);
    return totalDamage;
}

int32 MythicPlus::ScaleHealSpell(SpellInfo const * spellInfo, MpCreatureData* creatureData, Creature* creature, Creature* target, float healMultiplier)
{
    if (!spellInfo) {
        MpLogger::error("Invalid spell info ScaleHealSpell()");
        return 0;
    }

    if(!creatureData) {
        MpLogger::error("Invalid creature data ScaleHealSpell()");
        return 0;
    }

    if(!creature) {
        MpLogger::error("Invalid creature ScaleHealSpell()");
        return 0;
    }

    if(!target) {
        MpLogger::error("Invalid target ScaleHealSpell()");
        return 0;
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
    MpLogger::debug("Spell healing scaled from for spell  New Damage: {}", totalHeal);
    return pow((totalHeal / originalHp) * currentHealth, 0.8f) * healMultiplier;
}

/**
 * Function is copied because was not accessible in core creature class
 */
float GetTypeHealthModifier(int32 Rank)
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

// This takes the orignal health and scales flat based on the factor then applies the configuration modifier from the conf file
uint32 CalculateNewHealth(CreatureTemplate const* cInfo, uint32 mapId, MpDifficulty difficulty, uint32 origHealth, float confHPMod)
{
    int32 rank = 0;
    if(cInfo && cInfo->rank > 0) {
        rank = cInfo->rank;
    }

    // Add some variance to the healthpool so enemies are not all the same
    float healthVariation = frand(0.85f, 1.15f);
    float unitTypeMod = GetTypeHealthModifier(rank);
    uint32 basehp = uint32(std::ceil(origHealth * unitTypeMod * healthVariation));

    int32 hpScaleFactor = sMpDataStore->GetHealthScaleFactor(mapId, difficulty);
    if(cInfo->ModHealth > 0.0f) {
        return uint32(basehp * (cInfo->ModHealth + hpScaleFactor) * confHPMod);
    } else {
        return uint32(basehp * (hpScaleFactor) * confHPMod);
    }
}

float GetTypeDamageModifier(int32 Rank)
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
