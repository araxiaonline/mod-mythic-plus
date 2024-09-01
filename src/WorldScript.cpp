#include "Config.h"
#include "MythicPlus.h"
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
        sMythicPlus->mythicDungeonModifiers.clear();
        sMythicPlus->mythicBossModifiers.clear();

        // Global Settings
        sMythicPlus->Enabled = sConfigMgr->GetOption<bool>("MythicPlus.Enabled", 1);
        sMythicPlus->Debug = sConfigMgr->GetOption<bool>("MythicPlus.Debug", 0);
        sMythicPlus->EnableItemRewards = sConfigMgr->GetOption<bool>("MythicPlus.EnableItemRewards", 1);
        sMythicPlus->EnableDeathLimits = sConfigMgr->GetOption<bool>("MythicPlus.EnableDeathLimits", 1);

        // Mythic Difficulty Modifiers
        sMythicPlus->mythicDungeonModifiers = {
            {"DungeonHealth", sConfigMgr->GetOption<float>("MythicPlus.Mythic.DungeonHealth", 1.25f)},
            {"DungeonMelee", sConfigMgr->GetOption<float>("MythicPlus.Mythic.DungeonMelee", 1.25f)},
            {"DungeonSpell", sConfigMgr->GetOption<float>("MythicPlus.Mythic.DungeonSpell", 1.15f)},
            {"DungeonArmor", sConfigMgr->GetOption<float>("MythicPlus.Mythic.DungeonArmor", 1.25f)},
            {"DungeonAvgLevel", sConfigMgr->GetOption<uint32>("MythicPlus.Mythic.DungeonAvgLevel", 83)}
        };

        sMythicPlus->mythicBossModifiers = {
            {"DungeonBossHealth", sConfigMgr->GetOption<float>("MythicPlus.Mythic.DungeonBossHealth", 1.50f)},
            {"DungeonBossMelee", sConfigMgr->GetOption<float>("MythicPlus.Mythic.DungeonBossMelee", 1.35f)},
            {"DungeonBossSpell", sConfigMgr->GetOption<float>("MythicPlus.Mythic.DungeonBossSpell", 1.25f)},
            {"DungeonBossArmor", sConfigMgr->GetOption<float>("MythicPlus.Mythic.DungeonBossArmor", 1.35f)},
            {"DungeonBossLevel", sConfigMgr->GetOption<uint32>("MythicPlus.Mythic.DungeonBossLevel", 85)}
        };

        // Legendary Difficulty Modifiers
        sMythicPlus->legendaryDungeonModifiers = {
            {"DungeonHealth", sConfigMgr->GetOption<float>("MythicPlus.Legendary.DungeonHealth", 2.25f)},
            {"DungeonMelee", sConfigMgr->GetOption<float>("MythicPlus.Legendary.DungeonMelee", 2.25f)},
            {"DungeonSpell", sConfigMgr->GetOption<float>("MythicPlus.Legendary.DungeonSpell", 2.25f)},
            {"DungeonArmor", sConfigMgr->GetOption<float>("MythicPlus.Legendary.DungeonArmor", 2.25f)},
            {"DungeonAvgLevel", sConfigMgr->GetOption<uint32>("MythicPlus.Legendary.DungeonAvgLevel", 85)}
        };

        sMythicPlus->legendaryBossModifiers = {
            {"DungeonBossHealth", sConfigMgr->GetOption<float>("MythicPlus.Legendary.DungeonBossHealth", 2.25f)},
            {"DungeonBossMelee", sConfigMgr->GetOption<float>("MythicPlus.Legendary.DungeonBossMelee", 2.25f)},
            {"DungeonBossSpell", sConfigMgr->GetOption<float>("MythicPlus.Legendary.DungeonBossSpell", 2.25f)},
            {"DungeonBossArmor", sConfigMgr->GetOption<float>("MythicPlus.Legendary.DungeonBossArmor", 3.25f)},
            {"DungeonBossLevel", sConfigMgr->GetOption<uint32>("MythicPlus.Legendary.DungeonBossLevel", 87)}
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
