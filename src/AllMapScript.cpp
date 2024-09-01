#include "Chat.h"
#include "Log.h"
#include "MapMgr.h"
#include "MpLogger.h"
#include "MpDataStore.h"
#include "MythicPlus.h"
#include "Player.h"
#include "ScriptMgr.h"

class MythicPlus_AllMapScript : public AllMapScript
{
public:
    MythicPlus_AllMapScript() : AllMapScript("MythicPlus_AllMapScript")
    {
    }

    void OnCreateMap(Map* map)
    {
        MpLogger::debug("AllMapScript::OnCreateMap(): {}", map->GetMapName());

        if (!sMythicPlus->IsMapEligible(map)) {
            return;
        }


    }

    void OnPlayerEnterAll(Map* map, Player* player)
    {
        MpLogger::debug("AllMapScript::OnPlayerEnterAll(): {}", map->GetMapName());

        if (!sMythicPlus->IsMapEligible(map)) {
            return;
        }

    }

    void OnPlayerLeaveAll(Map* map, Player* player)
    {
        MpLogger::debug("AllMapScript::OnPlayerLeaveAll(): {}", map->GetMapName());

        if (!sMythicPlus->IsMapEligible(map)) {
            return;
        }


   }
};

void Add_MP_AllMapScripts()
{
    MpLogger::debug("Add_MP_AllMapScripts()");
    new MythicPlus_AllMapScript();
}
