#ifndef MYTHICPLUS_H
#define MYTHICPLUS_H

#include "Define.h"
#include "Map.h"
#include "MpDataStore.h"
#include <map>
#include <string>

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

    // Map is eligible for mythic+ scaling
    bool IsMapEligible(Map* map);

    // Check is difficulty is enabled in the configuration
    bool IsDifficultyEnabled(std::string difficulty);

    // if configuration has disabled the specific dungeon return false
    bool IsDungeonDisabled(uint32 dungeonId);

    // The creature should be given Mythic+ scaling and powers check for pets, npcs, etc
    bool IsCreatureEligible(Creature* creature);

    private:
        MythicPlus() { }
        ~MythicPlus() { }
};


#define sMythicPlus MythicPlus::instance()

#endif // MYTHICPLUS_H
