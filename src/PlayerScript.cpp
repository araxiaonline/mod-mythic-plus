#include "MpLogger.h"
#include "MpDataStore.h"
#include "MythicPlus.h"
#include "Player.h"
#include "ScriptMgr.h"

class MythicPlus_PlayerScript : public PlayerScript
{
public:
    MythicPlus_PlayerScript() : PlayerScript("MythicPlus_PlayerScript") { }

    void OnPlayerJustDied(Player* player, Unit* killer)
    {
        Map* map = player->GetMap();
        if(!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        if (!player || !killer) {
            return;
        }

        MpGroupData *data = sMpDataStore->GetGroupData(player->GetGroup());
        if (!data) {
            return;
        }

        data->deaths++; //


    }
};

void Add_MP_PlayerScripts()
{
    MpLogger::debug("Add_MP_PlayerScripts()");
    new MythicPlus_PlayerScript();
}
