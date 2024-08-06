#include "Chat.h"
#include "Log.h"
#include "MapMgr.h"
#include "MpLogger.h"
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
        static MythicPlus* mp = MythicPlus::getInstance();
        MpLogger::debug("AllMapScript::OnCreateMap(): {}", map->GetMapName());

        if (!mp->IsMapEligible(map)) {
            return;
        }


    }

    void OnPlayerEnterAll(Map* map, Player* player)
    {
        static MythicPlus* mp = MythicPlus::getInstance();
        MpLogger::debug("AllMapScript::OnPlayerEnterAll(): {}", map->GetMapName());

        if (!mp->IsMapEligible(map)) {
            return;
        }

    }

    void OnPlayerLeaveAll(Map* map, Player* player)
    {
        static MythicPlus* mp = MythicPlus::getInstance();
        MpLogger::debug("AllMapScript::OnPlayerLeaveAll(): {}", map->GetMapName());

        if (!mp->IsMapEligible(map)) {
            return;
        }


   }
};

void Add_MP_AllMapScripts()
{
    MpLogger::debug("Add_MP_AllMapScripts()");
    new MythicPlus_AllMapScript();
}