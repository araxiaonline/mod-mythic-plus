#include "ScriptMgr.h"
#include "Log.h"
#include "Player.h"
#include "MythicPlus.h"
#include "Chat.h"
#include "MapMgr.h"

class MythicPlus_AllMapScript : public AllMapScript
{
public:
    MythicPlus_AllMapScript() : AllMapScript("MythicPlus_AllMapScript")
    {
    }

    void OnCreateMap(Map* map)
    {
        // LOG_DEBUG("module.MythicPlus", "MythicPlus_AllMapScript::OnCreateMap(): {}", map->GetMapName());

        // if (!map->IsDungeon() && !map->IsRaid())
        //     return;

    }

    void OnPlayerEnterAll(Map* map, Player* player)
    {
        // LOG_DEBUG("module.MythicPlus", "MythicPlus_AllMapScript::OnPlayerEnterAll(): {}", map->GetMapName());

        // if (!map->IsDungeon() && !map->IsRaid())
        //     return;

        // if (player->IsGameMaster())
        //     return;
    }

    void OnPlayerLeaveAll(Map* map, Player* player)
    {
        // LOG_DEBUG("module.MythicPlus", "MythicPlus_AllMapScript::OnPlayerLeaveAll(): {}", map->GetMapName());

        // if (!sMythicPlus->EnableGlobal)
        //     return;
   }
};

void AddAllMapScripts()
{
    new MythicPlus_AllMapScript();
}