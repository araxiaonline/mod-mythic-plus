#ifndef MYTHICPLUS_H
#define MYTHICPLUS_H

#include "Define.h"
#include "Map.h"
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

    // Methods to clear and load settings
    void ClearSettings();

    // Check if a map is eligible for Mythic+ modifications
    bool IsMapEligible(Map* map);

    // Global Settings
    bool Enabled;
    bool Debug;
    bool EnableItemRewards;
    bool EnableDeathLimits;

    // Difficulty Modifiers
    std::map<std::string, float> mythicDungeonModifiers;
    std::map<std::string, float> mythicBossModifiers;

    std::map<std::string, float> legendaryDungeonModifiers;
    std::map<std::string, float> legendaryBossModifiers;

    // Death Allowances
    uint32 mythicDeathAllowance;
    uint32 legendaryDeathAllowance;
    uint32 ascendantDeathAllowance;

    // Itemization Offsets
    uint32 mythicItemOffset;
    uint32 legendaryItemOffset;
    uint32 ascendantItemOffset;

    private:
        MythicPlus() { }
        ~MythicPlus() { }
};


#define sMythicPlus MythicPlus::instance()

#endif // MYTHICPLUS_H
