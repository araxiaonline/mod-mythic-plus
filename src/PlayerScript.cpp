#include "MpLogger.h"
#include "Player.h"
#include "ScriptMgr.h"

class MythicPlus_PlayerScript : public PlayerScript
{
public:
    MythicPlus_PlayerScript() : PlayerScript("MythicPlus_PlayerScript") { }

    void OnLogin(Player *Player) override
    {
        // if (sMythicPlus->EnableGlobal && sMythicPlus->Announcement) {
        //     ChatHandler(Player->GetSession()).SendSysMessage("This server is running the |cff4CFF00MythicPlus |rmodule.");
        // }
    }

    // virtual void OnPlayerEnterMap(Player* player, Map* map) override
    // {
    //     // if (!map || !map->IsDungeon())
    //     //     return;

    //     // sMythicPlus->UpdateMapPlayerStats(map);
    // }

    virtual void OnLevelChanged(Player* player, uint8 oldlevel) override
    {
        // Map* map = player->GetMap();

        // if (!map || !map->IsDungeon())
        //     return;

        // first update the map's player stats
        // sMythicPlus->UpdateMapPlayerStats(map);

        // // schedule all creatures for an update
        // sMythicPlus->lastConfigTime =
        //     std::chrono::duration_cast<std::chrono::microseconds>(
        //         std::chrono::system_clock::now().time_since_epoch()
        //     ).count();
    }

    void OnGiveXP(Player* player, uint32& amount, Unit* victim, uint8 /*xpSource*/) override
    {
        // Map* map = player->GetMap();

    }


    // void OnBeforeDropAddItem
    void OnBeforeLootMoney(Player* player, Loot* loot) override
    {
        // Map* map = player->GetMap();

        // // If this isn't a dungeon, make no changes
        // if (!map->IsDungeon())
        //     return;

    }
};

void Add_MP_PlayerScripts()
{
    MpLogger::debug("Add_MP_PlayerScripts()");
    new MythicPlus_PlayerScript();
}
