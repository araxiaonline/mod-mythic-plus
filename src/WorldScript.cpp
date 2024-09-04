#include "Config.h"
#include "MythicPlus.h"
#include "MpDataStore.h"
#include "MpLogger.h"
#include "Player.h"
#include "ScriptMgr.h"

class MythicPlus_WorldScript : public WorldScript
{
public:
    MythicPlus_WorldScript() : WorldScript("MythicPlus_WorldScript") { }

    void OnBeforeConfigLoad(bool /*reload*/) override
    {
        SetInitialWorldSettings();
        // sMythicPlus->lastConfigTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    void OnStartup() override
    {

    }

    void SetInitialWorldSettings()
    {
        // Clear existing data
        sMythicPlus->enabledDifficulties.clear();
        sMythicPlus->disabledDungeons.clear();
        sMythicPlus->mythicBossModifiers = {};
        sMythicPlus->mythicDungeonModifiers = {};
        sMythicPlus->legendaryBossModifiers = {};
        sMythicPlus->legendaryDungeonModifiers = {};
        sMythicPlus->ascendantBossModifiers = {};
        sMythicPlus->ascendantDungeonModifiers = {};

        // Global Settings
        sMythicPlus->Enabled = sConfigMgr->GetOption<bool>("MythicPlus.Enabled", 1);
        sMythicPlus->EnableItemRewards = sConfigMgr->GetOption<bool>("MythicPlus.EnableItemRewards", 1);
        sMythicPlus->EnableDeathLimits = sConfigMgr->GetOption<bool>("MythicPlus.EnableDeathLimits", 1);

        /** @todo Add these back in once I write the parsers for handling the different configuration values.  */
        // sMythicPlus->enabledDifficulties = sConfigMgr->GetOption<std::vector<std::string>>("MythicPlus.EnabledDifficulties", {"mythic", "legendary", "ascendant"});
        // sMythicPlus->disabledDungeons = sConfigMgr->GetOption<std::vector<uint32>>("MythicPlus.DisabledDungeons", {});

        // Mythic Difficulty Modifiers
        sMythicPlus->mythicDungeonModifiers = {
            .health = sConfigMgr->GetOption<float>("MythicPlus.Mythic.DungeonHealth", 1.25f),
            .melee = sConfigMgr->GetOption<float>("MythicPlus.Mythic.DungeonMelee", 1.25f),
            .spell = sConfigMgr->GetOption<float>("MythicPlus.Mythic.DungeonSpell", 1.15f),
            .armor = sConfigMgr->GetOption<float>("MythicPlus.Mythic.DungeonArmor", 1.25f),
            .avgLevel = sConfigMgr->GetOption<uint32>("MythicPlus.Mythic.DungeonAvgLevel", 83)
        };

        sMythicPlus->mythicBossModifiers = {
            .health = sConfigMgr->GetOption<float>("MythicPlus.Mythic.DungeonBossHealth", 1.50f),
            .melee = sConfigMgr->GetOption<float>("MythicPlus.Mythic.DungeonBossMelee", 1.35f),
            .spell = sConfigMgr->GetOption<float>("MythicPlus.Mythic.DungeonBossSpell", 1.25f),
            .armor = sConfigMgr->GetOption<float>("MythicPlus.Mythic.DungeonBossArmor", 1.35f),
            .avgLevel = sConfigMgr->GetOption<uint32>("MythicPlus.Mythic.DungeonBossLevel", 85)
        };

        // Legendary Difficulty Modifiers
        sMythicPlus->legendaryDungeonModifiers = {
            .health = sConfigMgr->GetOption<float>("MythicPlus.Legendary.DungeonHealth", 2.25f),
            .melee = sConfigMgr->GetOption<float>("MythicPlus.Legendary.DungeonMelee", 2.25f),
            .spell = sConfigMgr->GetOption<float>("MythicPlus.Legendary.DungeonSpell", 2.25f),
            .armor = sConfigMgr->GetOption<float>("MythicPlus.Legendary.DungeonArmor", 2.25f),
            .avgLevel = sConfigMgr->GetOption<uint32>("MythicPlus.Legendary.DungeonAvgLevel", 85)
        };

        sMythicPlus->legendaryBossModifiers = {
            .health = sConfigMgr->GetOption<float>("MythicPlus.Legendary.DungeonBossHealth", 2.25f),
            .melee = sConfigMgr->GetOption<float>("MythicPlus.Legendary.DungeonBossMelee", 2.25f),
            .spell = sConfigMgr->GetOption<float>("MythicPlus.Legendary.DungeonBossSpell", 2.25f),
            .armor = sConfigMgr->GetOption<float>("MythicPlus.Legendary.DungeonBossArmor", 3.25f),
            .avgLevel = sConfigMgr->GetOption<uint32>("MythicPlus.Legendary.DungeonBossLevel", 87)
        };

        sMythicPlus->ascendantDungeonModifiers = {
            .health = sConfigMgr->GetOption<float>("MythicPlus.Ascendant.DungeonHealth", 3.25f),
            .melee = sConfigMgr->GetOption<float>("MythicPlus.Ascendant.DungeonMelee", 3.25f),
            .spell = sConfigMgr->GetOption<float>("MythicPlus.Ascendant.DungeonSpell", 3.25f),
            .armor = sConfigMgr->GetOption<float>("MythicPlus.Ascendant.DungeonArmor", 3.25f),
            .avgLevel = sConfigMgr->GetOption<uint32>("MythicPlus.Ascendant.DungeonAvgLevel", 87)
        };

        sMythicPlus->ascendantBossModifiers = {
            .health = sConfigMgr->GetOption<float>("MythicPlus.Ascendant.DungeonBossHealth", 3.25f),
            .melee = sConfigMgr->GetOption<float>("MythicPlus.Ascendant.DungeonBossMelee", 3.25f),
            .spell = sConfigMgr->GetOption<float>("MythicPlus.Ascendant.DungeonBossSpell", 3.25f),
            .armor = sConfigMgr->GetOption<float>("MythicPlus.Ascendant.DungeonBossArmor", 3.25f),
            .avgLevel = sConfigMgr->GetOption<uint32>("MythicPlus.Ascendant.DungeonBossLevel", 90)
        };


        // Death Allowances
        sMythicPlus->mythicDeathAllowance = sConfigMgr->GetOption<uint32>("MythicPlus.Mythic.DeathAllowance", 1000);
        sMythicPlus->legendaryDeathAllowance = sConfigMgr->GetOption<uint32>("MythicPlus.Legendary.DeathAllowance", 30);
        sMythicPlus->ascendantDeathAllowance = sConfigMgr->GetOption<uint32>("MythicPlus.Ascendant.DeathAllowance", 15);

        // Itemization Offsets
        sMythicPlus->mythicItemOffset = sConfigMgr->GetOption<uint32>("MythicPlus.Mythic.ItemOffset", 20000000);
        sMythicPlus->legendaryItemOffset = sConfigMgr->GetOption<uint32>("MythicPlus.Legendary.ItemOffset", 21000000);
        sMythicPlus->ascendantItemOffset = sConfigMgr->GetOption<uint32>("MythicPlus.Ascendant.ItemOffset", 22000000);
    }

};

void Add_MP_WorldScripts()
{
    MpLogger::debug("Add_MP_WorldScripts()");
    new MythicPlus_WorldScript();
}
