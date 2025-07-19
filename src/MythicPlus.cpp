#include "MythicPlus.h"
#include "MpLogger.h"
#include "ObjectMgr.h"
#include "MapMgr.h"
#include "ScriptMgr.h"
#include "Group.h"
#include "Unit.h"
#include "WorldPacket.h"
#include "UpdateMask.h"
#include "MpScriptAI.h"

#include <algorithm>
#include <cmath>

// Special case for Headless Horseman Event
const uint32 HEADLESS_HORSEMAN = 23682;

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

        // Null check for GetOwner to avoid dereferencing a null pointer
        if ((target->IsPet() || target->IsSummon() || target->IsHunterPet()) && target->GetOwner() && target->GetOwner()->IsNPCBot()) {
            return false;
        }
    #endif

    // Ensure target is a valid creature before checking eligibility
    Creature* creatureTarget = target->ToCreature();
    if (creatureTarget && sMythicPlus->IsCreatureEligible(creatureTarget)) {
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

    std::string scriptName = creature->GetScriptName();
    if(scriptName.starts_with("boss_")) {
        return true;
    }

    if (creature->IsDungeonBoss()) {
        return true;
    }

    if (creature->GetEntry() == HEADLESS_HORSEMAN) {
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

        if(creature->GetBotOwner()) {
            MpLogger::debug("Creature is owned by a NPC Bot {} skip scaling creature: {}", creature->GetBotOwner()->GetName(), creature->GetName());
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
    // MpLogger::debug("Added creature {} to instance data for instance {}",
    //     creature->GetName(),
    //     creature->GetMap()->GetMapName()
    // );
}

void MythicPlus::AddScaledCreature(Creature* creature, MpInstanceData* instanceData)
{
    MpCreatureData creatureData = MpCreatureData(creature);
    sMpDataStore->AddCreatureData(creature->GetGUID(), creatureData);

    // allow small variance in level for non-boss creatures
    uint8 level = uint8(urand(instanceData->creature.avgLevel - 1, instanceData->creature.avgLevel + 1));
    if(creature->IsDungeonBoss() || creature->GetEntry() == 23682) {
        ScaleCreature(instanceData->boss.avgLevel, creature, &instanceData->boss, instanceData->difficulty);
    } else {
        ScaleCreature(level, creature, &instanceData->creature, instanceData->difficulty);
    }

    // Update AI now the creature has been scaled.
    // auto ai = new MpScriptAI(creature, instanceData->difficulty);
    // creature->SetAI(ai);

    // We know the creature is scaled and in the instance to fire the event.
    // sCreatureHooks->AddToInstance(creature);
    std::string name = creature->GetName();

    // Assign random affix for now.
    if (roll_chance_i(50)) {
        uint32 irand = urand(0, 2);

        if(irand == 0) {
            creature->AddAura(23341, creature);
        } else if(irand == 1) {
            creature->AddAura(34711, creature);

        } else {
            creature->AddAura(774, creature);
        }
    }

    creatureData.SetScaled(true);
    creatureData.SetDifficulty(instanceData->difficulty);

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

void MythicPlus::ScaleAll(Player* player, MpInstanceData* instanceData)
{
    std::vector<MpCreatureData*> creatures = sMpDataStore->GetInstanceCreatures(player->GetMapId(), player->GetInstanceId());
    for (MpCreatureData* creatureData : creatures) {
        ScaleCreature(creatureData->creature->GetLevel(), creatureData->creature, &instanceData->creature, instanceData->difficulty);
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

    // Scale the creatures base health
    uint32 basehp = stats->BaseHealth[EXPANSION_WRATH_OF_THE_LICH_KING];
    uint32 health = CalculateNewHealth(creature, cInfo, mapId, difficulty, basehp, multipliers->health);

    creature->SetCreateHealth(health);
    creature->SetMaxHealth(health);
    creature->SetHealth(health);
    creature->ResetPlayerDamageReq();
    creature->SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, (float)health);

    /**
     * @TODO: Figure out mana later for unit_types 2 and 8
     */
    // Scale the creatures mana pool
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

    MpInstanceData *instanceData = sMpDataStore->GetInstanceData(creature->GetMapId(), creature->GetInstanceId());

    // Handle new melee/range scaling with simple formula (for simplicity range will just be 80% of melee bonus)
    float meleeMultiplier = sMpDataStore->GetMeleeScaleFactor(creature->GetMapId(), instanceData->difficulty);

    // Scale up the attack power based on the instance set in the database difficulty table.
    uint32 ap = std::ceil(stats->AttackPower * meleeMultiplier);
    uint32 rangeAp = std::ceil(stats->RangedAttackPower * meleeMultiplier * 0.5f);

    MpCreatureData* creatureData = sMpDataStore->GetCreatureData(creature->GetGUID());
    if(creatureData) {
        creatureData->NewAttackPower = ap;
        creatureData->AttackPowerScaleMultiplier = meleeMultiplier;
    }

    // Set scaled attack power
    creature->SetModifierValue(UNIT_MOD_ATTACK_POWER, BASE_VALUE, ap);
    creature->SetModifierValue(UNIT_MOD_ATTACK_POWER_RANGED, BASE_VALUE, rangeAp);

    // set the base weapon damage
    creature->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, stats->BaseDamage[EXPANSION_WRATH_OF_THE_LICH_KING], 0);
    creature->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, stats->BaseDamage[EXPANSION_WRATH_OF_THE_LICH_KING] * 1.5f, 0);

    creature->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, stats->BaseDamage[EXPANSION_WRATH_OF_THE_LICH_KING] * 0.6f, 0);
    creature->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, stats->BaseDamage[EXPANSION_WRATH_OF_THE_LICH_KING] * 1.0f, 0);

    // Update all stats to apply the new damage values
    creature->UpdateAllStats();

    // Scale up the armor with some variance also to make some tougher enemies in the mix
    uint32 armor = uint32(std::ceil(stats->BaseArmor * multipliers->armor * cInfo->ModArmor));
    creature->SetArmor(armor);

}

int32 MythicPlus::CalculateSpellDamage(uint32 baseDamage, int originalLevel, int targetLevel) {
    float origHpPool = sMpDataStore->GetPlayerHealthAvg(originalLevel);
    float targetHpPool = sMpDataStore->GetPlayerHealthAvg(targetLevel);

    // Using a % of expected damage of the average player pool creates a better consistent experience when scaling spells
    float percentDamage = baseDamage / origHpPool;
    int32 scaledDamage = static_cast<int32>(std::ceil(percentDamage * targetHpPool));

    return scaledDamage;
}

int32 MythicPlus::ScaleDamageSpell(SpellInfo const * spellInfo, uint32 damage, MpCreatureData* creatureData, Creature* creature, Unit* target, float damageMultiplier)
{
    if (!spellInfo) {
        MpLogger::error("Invalid spell info ScaleDamageSpell()");
        return damage;
    }

    MpInstanceData *instanceData = sMpDataStore->GetInstanceData(creature->GetMapId(), creature->GetInstanceId());
    if (!instanceData) {
        MpLogger::debug("No instance data found for spell scaling, using original damage");
        return damage;
    }

    float scaleFactor = sMpDataStore->GetSpellScaleFactor(creature->GetMapId(), instanceData->difficulty);

    // calculate the global modifier x instance modifier
    float totalModifier = damageMultiplier * scaleFactor;

    // If for some reason there is not a creature, just use the global modifier x instance modifier
    if(!creature) {
        MpLogger::error("Invalid creature ScaleDamageSpell()");
        return damage * totalModifier;
    }

    // Use the already calculated damage as the base for scaling
    int32 newDamage = damage;

    // Handle Summoned unit modifiers as
    if(!creatureData) {

        // handle if bot pets if NPCBot is installed.
        #ifdef NPCBOT
            if(creature->IsNPCBotOrPet()) {
                return damage;
            }
        #endif

        // Handle totems and summons - scale based on owner's details because they will not have creature data
        if(creature->IsTotem() || creature->IsSummon()) {
            Unit* owner = creature->GetOwner();
            if(owner && owner->IsCreature()) {
                Creature* ownerCreature = owner->ToCreature();

                // Look up the owner creature's original level from MpDataStore
                MpCreatureData* ownerCreatureData = sMpDataStore->GetCreatureData(ownerCreature->GetGUID());
                if (ownerCreatureData) {
                    int32 ownerOriginalLevel = ownerCreatureData->originalLevel;

                    if (ownerCreature->GetCreatureTemplate()->rank == CREATURE_ELITE_NORMAL) {
                        totalModifier = totalModifier * 0.5f;
                    }
                    newDamage = CalculateSpellDamage(damage, ownerOriginalLevel, ownerCreature->GetLevel());
                } else {
                    // Fallback if no creature data found - use current level
                    if(ownerCreature->GetCreatureTemplate()->rank == CREATURE_ELITE_NORMAL) {
                        totalModifier = totalModifier * 0.5f;
                    }
                    newDamage = CalculateSpellDamage(damage, ownerCreature->GetLevel(), ownerCreature->GetLevel());
                    MpLogger::debug("No creature data found for owner {}, using current level for scaling", ownerCreature->GetGUID().ToString());
                }
            }
        }
        else {
            MpLogger::error("Invalid creature data ScaleDamageSpell()");
            return damage * totalModifier;
        }
    } else {
        newDamage = CalculateSpellDamage(damage, creatureData->originalLevel, creature->GetLevel());
        if (creature->GetCreatureTemplate()->rank == CREATURE_ELITE_NORMAL) {
            // Reduced scaling for elite/boss spells to prevent them from hitting too hard
            totalModifier = totalModifier * 0.5f;
        }
    }



    MpLogger::debug(" >>> Spell {} damage scaled from for spell  New Damage: {} using: scaling Factor: {} and damage Multi: {}",spellInfo->SpellName[0], totalDamage, scalingFactor, damageMultiplier);

    // Apply scaling factor and the set multiplier from the instance data
    totalDamage = int32(totalDamage * scalingFactor * damageMultiplier);

    // MpLogger::debug("Spell {} damage scaled from for spell New Damage: {} using: scaling Factor: {} and damage Multi: {}",spellInfo->SpellName[0], totalDamage, scalingFactor, damageMultiplier);
    return totalDamage;
}

int32 MythicPlus::ScaleHealSpell(SpellInfo const * spellInfo, uint32 heal, MpCreatureData* creatureData, Creature* creature, Creature* /* target */, float healMultiplier)
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

    auto effects = spellInfo->GetEffects();
    for (uint8 i = 0; i < effects.size(); ++i)
    {
        MpLogger::debug(" >>> Spell {} effect {} value: {} by creature {}", spellInfo->SpellName[i], effects[i].Effect, heal, creature->GetName());
    }

    int32 originalLevel = creatureData->originalLevel;

    float levelDifference = creature->GetLevel() - originalLevel;
    float spellBonus = sMpDataStore->GetSpellScaleFactor(creature->GetMapId(), creature->GetInstanceId());

    // Reduced scaling for heals to prevent them from being too powerful
    float scalingFactor = CalculateScaling(levelDifference, spellBonus * 0.4f, 1.8f); // Reduced scaling factor and constant

    MpLogger::debug(" >>> Spell {} healed scaled from for spell  New Heal: {} using: scaling Factor: {} and damage Multi: {}",spellInfo->SpellName[0], heal, scalingFactor, healMultiplier);
    return int32(heal * scalingFactor * healMultiplier);
}

void MythicPlus::GroupReset(Group* /*group*/, Map* /* map */) {
    // Stubbed out for later implementation
}

bool MythicPlus::IsFinalBoss(Creature* creature) {
    std::array<uint32, 128> finalBosses = {
        // --- WoW Classic Dungeons ---
        11519,  /* Bazzalan Ragefire */
        639,    /* Edwin VanCleef Deadmines */
        3654,   /* Mutanus the Devourer Wailing Caverns */
        4275,   /* Archmage Arugal Shadowfang Keep */
        1716,   /* Bazil Thredd Stockades */
        4829,   /* Aku'mai blackfathom Deeps */
        7800,   /* Mekgineer Thermaplugg Gnomeregan */
        4421,   /* Charlga Razorflank Razorfen Kraul */
        4543,   /* Bloodmage Thalnos Scarlet Monastery */
        3975,   /* Herod Scarlet Monastery */
        3977,   /* High Inquisitor Whitemane Scarlet Monastery */
        7350,   /* Amnennar the Coldbringer Razorfen Downs */
        2748,   /* Archaedas Uldaman */
        7267,   /* Chief Ukorz Sandscalp Zul'Farrak */
        12201,  /* Princess Theradras Maraudon */
        5709,   /* Shade of Eranikus Sunken Temple */
        9019,   /* Emperor Dagran Thaurissan Blackrock Depths */
        9568,   /* Overlord Wyrmthalak Lower Blackrock Spire */
        10363,  /* General Drakkisath Upper Blackrock Spire */
        11492,  /* alzzin the Wildshaper Dire Maul */
        11496,  /* Immol'thar Dire Maul */
        11501,  /* King Gordok Dire Maul */
        1853,   /* Darkmaster Gandling Scholomance */
        10812,  /* Grand Crusader Dathrohan Stratholme */
        10440,  /* Baron Rivendare Stratholme */

        // --- WoW Classic Raids ---
        11583,  /* Nefarian Blackwing Lair */
        11502,  /* Ragnaros Molten Core */
        14834,  /* Hakkar Zul'Gurub */
        15727,  /* C'Thun Temple of Ahn'Qiraj */
        15339,  /* Ossirian the Unscarred Ruins of Ahn'Qiraj */

        // --- The Burning Crusade ---
        17536,  /* Nazan - Hellfire Ramparts Normal */
        17536,  /* Nazan - Hellfire Ramparts Normal */
        17377,  /* Kelidan the Breaker - Blood Furnace Normal */
        18607,  /* Kelidan the Breaker - Blood Furnace Heroic */
        17942,  /* Quagmirran - The Slave Pens Normal */
        19894,  /* Quagmirran - The Slave Pens Heoric */
        17882,  /* The Black Stalker - The Underbog Normal */
        20184,  /* The Black Stalker - The Underbog Heroic */
        24420,  /* The Black Stalker - The Underbog Heroic(2) */
        18344,  /* Nexus-Prince Shaffar - Mana-Tombs Normal */
        20256,  /* Nexus-Prince Shaffar - Mana-Tombs Heroic */
        18373,  /* Exarch Maladaar - Auchenai Crypts Normal */
        20306,  /* Exarch Maladaar - Auchenai Crypts Heoric */
        18096,  /* Epoch Hunter - Old Hillsbrad Foothills Normal */
        20531,  /* Epoch Hunter - Old Hillsbrad Foothills Heroic */
        18473,  /* Talon King Ikiss - Sethekk Halls Normal */
        20706,  /* Talon King Ikiss - Sethekk Halls Heroic */
        17798,  /* Warlord Kalithresh - The Steamvault Normal */
        20633,  /* Warlord Kalithresh - The Steamvault Heroic */
        18708,  /* Murmur - Shadow Labyrinth Normal */
        20657,  /* Murmur - Shadow Labyrinth Heroic */
        16808,  /* Warchief Kargath Bladefist - The Shattered Halls Normal */
        20597,  /* Warchief Kargath Bladefist - The Shattered Halls Heroic */
        17881,  /* Aeonus - The Black Morass Normal */
        20737,  /* Aeonus - The Black Morass Heroic */
        17977,  /* Warp Splinter - The Botanica Normal */
        21582,  /* Warp Splinter - The Botanica Heroic */
        19220,  /* Pathaleon the Calculator - The Mechanar Normal */
        21537,  /* Pathaleon the Calculator - The Mechanar Heroic */
        20912,  /* Harbinger Skyriss - The Arcatraz Normal */
        21601,  /* Harbinger Skyriss - The Arcatraz Heroic */
        19622,  /* Kael'thas Sunstrider - Magisters' Terrace Normal */
        24857,  /* Kael'thas Sunstrider - Magisters' Terrace Heroic */

        // --- Burning Crusade Raids ---
        15690,  /* Prince Malchezaar - Karazhan */
        23863,  /* Zul'jin - Zul'Aman */
        19044,  /* Gruul the Dragonkiller - Gruul's Lair */
        17257,  /* Magtheridon - Magtheridon's Lair */
        21212,  /* Lady Vashj - Serpentshrine Cavern */
        24664,  /* Kael'thas Sunstrider - The Eye */
        24855,  /* Kael'thas Sunstrider - The Eye */
        17968,  /* Archimonde - Hyjal Summit */
        22917,  /* Illidan Stormrage - Black Temple */
        25315,  /* Kil'jaeden - Sunwell Plateau */

        // --- Wrath of the Lich King ---
        23954,  /* Ingvar the Plunderer - Utgarde Keep Normal */
        31673,  /* Ingvar the Plunderer - Utgarde Keep Heroic */
        26861,  /* King Ymiron - Utgarde Pinnacle Normal */
        30788,  /* King Ymiron - Utgarde Pinnacle Heroic */
        26723,  /* Keristrasza - The Nexus Normal */
        30540,  /* Keristrasza - The Nexus Heroic */
        26632,  /* The Prophet Tharon'ja - Drak'Tharon Keep Normal */
        31360,  /* The Prophet Tharon'ja - Drak'Tharon Keep Heroic */
        27656,  /* Ley-Guardian Eregos - The Oculus Normal */
        31561,  /* Ley-Guardian Eregos - The Oculus Heroic */
        29311,  /* Herald Volazj - Ahn'kahet: The Old Kingdom Normal */
        31464,  /* Herald Volazj - Ahn'kahet: The Old Kingdom Heroic */
        29120,  /* Anub'arak - Azjol-Nerub Normal */
        31610,  /* Anub'arak - Azjol-Nerub Heroic */
        29306,  /* Gal'darah - Gundrak Normal */
        31368,  /* Gal'darah - Gundrak Heroic */
        26533,  /* Mal'Ganis - Culling of Stratholme Normal */
        31217,  /* Mal'Ganis - Culling of Stratholme Heroic */
        31134,  /* Cyanigosa - Violet Hold Normal */
        31506,  /* Cyanigosa - Violet Hold Heroic */
        27978,  /* Sjonnir the Ironshaper - Halls of Stone Normal */
        31386,  /* Sjonnir the Ironshaper - Halls of Stone Heroic */
        28923,  /* Loken - Halls of Lightning Normal */
        31538,  /* Loken - Halls of Lightning Heroic */
        35451,  /* The Black Knight - Trial of the Champion Normal */
        35490,  /* The Black Knight - Trial of the Champion Heroic */
        36502,  /* Devourer of Souls - The Forge of Souls Normal */
        37677,  /* Devourer of Souls - The Forge of Souls Heroic */
        36658,  /* Scourgelord Tyrannus - Pit of Saron Normal */
        36938,  /* Scourgelord Tyrannus - Pit of Saron Heroic */
        37226,  /* The Lich King Encounter - Halls of Reflection Normal */
        39166,  /* The Lich King Encounter - Halls of Reflection Heroic */

        // --- Wrath of the Lich King Raids ---
        15990,  /* Kel'Thuzad - Naxxramas */
        28859,  /* Malygos - The Eye of Eternity */
        28860,  /* Sartharion - Obsidian Sanctum */
        31125,  /* Archavon the Stone Watcher - Vault of Archavon */
        33993,  /* Emalon the Storm Watcher - Vault of Archavon */
        35013,  /* Koralon the Flame Watcher - Vault of Archavon */
        38433,  /* Toravon the Ice Watcher - Vault of Archavon */
        33288,  /* Yogg-Saron - Ulduar */
        34564,  /* Anub'arak - Trial of the Crusader */
        10184,  /* Onyxia - Onyxia's Lair (re-released in Wrath) */
        36597,  /* The Lich King - Icecrown Citadel */
        39863,  /* Halion - Ruby Sanctum */
    };

    return std::find(finalBosses.begin(), finalBosses.end(), creature->GetEntry()) != finalBosses.end();
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
uint32 CalculateNewHealth(Creature* creature, CreatureTemplate const* cInfo, uint32 mapId, MpDifficulty difficulty, uint32 origHealth, float confHPMod)
{
    //
    int32 rank = 0;
    if(cInfo && cInfo->rank > 0) {
        rank = cInfo->rank;
    }

    // These Factors that increase or decrease health based on different settings applied to the creature

    // Health Variation is used to create some random element to HP so not all creatures of the same level
    // have the same HP for more variety.
    float healthVariation;

    //  This is the fine grained hpScaleFactor set for the instance (and/or) creature overrides in the database.
    float hpScaleFactor = sMpDataStore->GetHealthScaleFactor(mapId, difficulty);

    // Add some variance to the healthpool so enemies are not all the same
    if(creature->IsDungeonBoss() || creature->isWorldBoss() || creature->isElite() || cInfo->rank == CREATURE_ELITE_RARE) {
        healthVariation = frand(1.0f, 1.15f);
    } else {  // This addresses Normals and other trash from getting to big a HP bonus
        healthVariation = frand(1.0f, 1.05f);
        hpScaleFactor *= 0.40;
    }

    // Add in special overrides here as necessary:
    if(creature->GetEntry() == HEADLESS_HORSEMAN) {
        healthVariation = frand(1.0f, 1.1f);
    }

    float unitTypeMod = GetTypeHealthModifier(rank);
    uint32 basehp = uint32(std::ceil(origHealth * healthVariation));

    /**
     * @brief Calculating the final creature health encompasses all the potential modifiers
     * CreatureTemplate.HealthModifier (ModHealth) - Creatures that are capable of being in a Heroic instance get a boost here
     * even though they are the same.  In this case we allow
     *
     * hpScaleFactor:  allows to tweak the bonus modifier more directly at a creature or instance level, since
     * you can not override it in cInfo directly as it is loaded statically from database
     *
     * confHPMod: is from the mythic settings directly.
     */
    if(cInfo->ModHealth > 0.0f) {
        return uint32(basehp * (cInfo->ModHealth + hpScaleFactor) * confHPMod);

    } else {
        return uint32(basehp * hpScaleFactor * confHPMod);
    }
}

// Calculates a balanced growth curve that provides good scaling across all level ranges
float CalculateScaling(int levelDifference, float scaleFactor, float constant, float growthFactor) {
    float levelMultiplier;

    if (levelDifference <= 0) {
        // High-level creatures need a minimum boost
        levelMultiplier = 1.5f; // Minimum 50% boost for near-level or higher creatures
    } else if (levelDifference <= 10) {
        // Moderate scaling for small level differences
        levelMultiplier = 1.5f + (levelDifference * 0.2f); // 1.5x to 3.5x
    } else if (levelDifference <= 30) {
        // Higher scaling for medium level differences
        levelMultiplier = 3.5f + ((levelDifference - 10) * 0.15f); // 3.5x to 6.5x
    } else {
        // Cap extreme scaling for very low level creatures
        levelMultiplier = 6.5f + std::min((levelDifference - 30) * 0.05f, 3.5f); // Cap at 10x
    }

    float scaling = constant * levelMultiplier * (1 + (scaleFactor / 100.0f));
    return scaling;
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
