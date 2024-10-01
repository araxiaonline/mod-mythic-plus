#ifndef MYTHICPLUS_H
#define MYTHICPLUS_H

#include "Creature.h"
#include "Define.h"
#include "Map.h"
#include "MpDataStore.h"
#include "Player.h"
#include "SpellInfo.h"
#include "Unit.h"

#include <map>
#include <string>
#include <vector>
#include <unordered_map>

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

    // This will attempt to scale a creature using instancedata
    void AddScaledCreature(Creature* creature, MpInstanceData* instanceData);

    // Scales the creature based on the level and the creature base stats
    void ScaleCreature(uint8 level, Creature* creature, MpMultipliers* multipliers, MpDifficulty difficulty);

    // Scales a damage spell up based on the level increase
    int32 ScaleDamageSpell(SpellInfo const * spellInfo, MpCreatureData* creatureData, Creature* creature, float damageMultiplier);

    // This scales a heal spell up based on the how much % the original heal spell was
    int32 ScaleHealSpell(SpellInfo const * spellInfo, MpCreatureData* creatureData, Creature* creature, Creature* target, float healMultiplier);

    private:
        MythicPlus() { }
        ~MythicPlus() { }
};

float GetTypeHealthModifier(int32 rank);
float GetTypeDamageModifier(int32 rank);
uint32 CalculateNewHealth(CreatureTemplate const* cInfo, uint32 mapId, MpDifficulty difficulty, uint32 origHealth, float confHPMod);
float CalculateNewBaseDamage(CreatureTemplate const* cInfo, uint32 mapId, MpDifficulty difficulty, float origDamage);

#define sMythicPlus MythicPlus::instance()

#endif // MYTHICPLUS_H
