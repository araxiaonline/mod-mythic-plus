#ifndef MYTHICPLUS_H
#define MYTHICPLUS_H

#include "Creature.h"
#include "Define.h"
#include "Map.h"
#include "MpDataStore.h"
#include "Player.h"
#include "SpellInfo.h"
#include "Unit.h"
#include "TaskScheduler.h"

#include <map>
#include <string>
#include <vector>
#include <unordered_map>

// Used to limit the total advancment rank and allow for changing the max rank in one place.
inline const uint8 MP_MAX_ADVANCEMENT_RANK = 50;

/**
 * Main Class for the mod responsible for controls related to scaling instances,
 * handling logic related to setting up instances for MythicPlus to work.
 *
 * MpDataStore is heavily used as well for storing data in memory and interactions with
 * database storage.
 *
 * This is a singleton instance that can be accessed through sMythicPlus.
 */
class MythicPlus
{
public:

    // accessor for this singleton
    static MythicPlus* instance()
    {
        static MythicPlus instance;
        return &instance;
    }

    // ensure we only ever have one instance of this class
    MythicPlus(const MythicPlus&) = delete;
    MythicPlus& operator=(const MythicPlus&) = delete;

    // Global Settings
    bool Enabled;
    bool EnableItemRewards;
    bool EnableDeathLimits;

    // Turn off specific features of mythic+
    std::vector<std::string> enabledDifficulties;
    std::vector<uint32> disabledDungeons;

    // Difficulty Modifiers
    MpMultipliers mythicDungeonModifiers;
    MpMultipliers mythicBossModifiers;
    MpMultipliers legendaryDungeonModifiers;
    MpMultipliers legendaryBossModifiers;
    MpMultipliers ascendantDungeonModifiers;
    MpMultipliers ascendantBossModifiers;

    // Death Allowances
    uint32 mythicDeathAllowance;
    uint32 legendaryDeathAllowance;
    uint32 ascendantDeathAllowance;

    // Itemization Offsets
    uint32 mythicItemOffset;
    uint32 legendaryItemOffset;
    uint32 ascendantItemOffset;

    // Scaling modifiers (Deprecated)
    uint32 meleeAttackPowerDampener;
    uint32 meleeAttackPowerStart;

    // Spell Damage Diminishing Returns
    float diminishingExponent;
    std::unordered_map<MpDifficulty, uint32> diminishingThresholds;

    enum MP_UNIT_EVENT_TYPE
    {
        UNIT_EVENT_MELEE,
        UNIT_EVENT_HEAL,
        UNIT_EVENT_DOT,
        UNIT_EVENT_SPELL,
        UNIT_EVENT_HOT
    };

    // Map is eligible for mythic+ scaling
    bool IsMapEligible(Map* map);

    // If a player difficulty is set that is eligible for mythic+ scaling
    bool IsDifficultySet(Player const* player);

    // Check is difficulty is enabled in the configuration
    bool IsDifficultyEnabled(std::string difficulty);

    // if configuration has disabled the specific dungeon return false
    bool IsDungeonDisabled(uint32 dungeonId);

    // Is it a scaled creature that is being healed
    bool EligibleHealTarget(Unit* target);

    // Validates if the target of an attack should receive mythic+ damage/heal/dot scaling
    bool EligibleDamageTarget(Unit* target);

    // The creature should be given Mythic+ scaling and powers check for pets, npcs, etc
    bool IsCreatureEligible(Creature* creature);

    // Adds the creature if eligible to be scaled
    void AddCreatureForScaling(Creature* creature);

    // Removes the creature from the scaling list and cleans up memory
    void RemoveCreature(Creature* creature);

    /**
     * Creatures are added to an instance before a player enter event is fired
     * therefore it is necessary to scan the instance creature information and
     * and scale any creatures that were loaded before the first player using
     * the instance data from the group settings.
     */
    void ScaleRemaining(Player* player, MpInstanceData* instanceData);

    // Rescales all creatures for an instance based on set data
    void ScaleAll(Player* player, MpInstanceData* instanceData);

    // This will attempt to scale a creature using instancedata
    void AddScaledCreature(Creature* creature, MpInstanceData* instanceData);

    // Scales the creature based on the level and the creature base stats
    void ScaleCreature(uint8 level, Creature* creature, MpMultipliers* multipliers, MpDifficulty difficulty);

    // Scales a damage spell up based on the level increase
    int32 ScaleDamageSpell(SpellInfo const * spellInfo, uint32 damage, MpCreatureData* creatureData, Creature* creature, Unit* target, float damageMultiplier);

    // This scales a heal spell up based on the how much % the original heal spell was
    int32 ScaleHealSpell(SpellInfo const * spellInfo, uint32 heal, MpCreatureData* creatureData, Creature* creature, Creature* target, float healMultiplier);

    // Calculate spell damage based on player health pools
    int32 CalculateSpellDamage(uint32 baseDamage, int originalLevel, int targetLevel);

    // Calculate heal scaling based on creature health percentages
    int32 CalculateHealScaling(uint32 baseHeal, uint32 originalHealth, uint32 currentMaxHealth);

    static bool IsFinalBoss(Creature* creature);
    static void GroupReset(Group* group, Map* map);

    private:
        MythicPlus() { }
        ~MythicPlus() { }
};

float GetTypeHealthModifier(int32 rank);
float GetTypeDamageModifier(int32 rank);
float CalculateScaling(int levelDifference, float scaleFactor, float constant = 1.25f, float growthFactor = 20.0f);
uint32 CalculateNewHealth(Creature* creature, CreatureTemplate const* cInfo, uint32 mapId, MpDifficulty difficulty, uint32 origHealth, float confHPMod);
float CalculateNewBaseDamage(CreatureTemplate const* cInfo, uint32 mapId, MpDifficulty difficulty, float origDamage);



#define sMythicPlus MythicPlus::instance()

#endif // MYTHICPLUS_H
