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
        static MythicPlus* mp = MythicPlus::getInstance();
        mp->debug("AllMapScript::OnCreateMap(): {}", map->GetMapName());

        if (!mp->IsMapEligible(map)) {
            return;
        }


    }

    void OnPlayerEnterAll(Map* map, Player* player)
    {
        static MythicPlus* mp = MythicPlus::getInstance();
        mp->debug("AllMapScript::OnPlayerEnterAll(): {}", map->GetMapName());

        if (!mp->IsMapEligible(map)) {
            return;
        }

    }

    void OnPlayerLeaveAll(Map* map, Player* player)
    {
        static MythicPlus* mp = MythicPlus::getInstance();
        mp->debug("AllMapScript::OnPlayerLeaveAll(): {}", map->GetMapName());

        if (!mp->IsMapEligible(map)) {
            return;
        }


   }
};

void Add_MP_AllMapScripts()
{
    static MythicPlus* mp = MythicPlus::getInstance();
    mp->debug("Add_MP_AllMapScripts()");
    new MythicPlus_AllMapScript();
}