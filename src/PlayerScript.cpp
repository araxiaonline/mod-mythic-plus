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

        if (!player) {
            return;
        }

        if (!killer) {
            return;
        }

        MpGroupData *data = sMpDataStore->GetGroupData(player->GetGroup());

        if (player->GetMap()->IsDungeon()) {
            MpLogger::debug("Player {} just died in dungeon {} by {}", player->GetName(), player->GetMap()->GetMapName(), killer->GetName());
        }
    }
};

void Add_MP_PlayerScripts()
{
    MpLogger::debug("Add_MP_PlayerScripts()");
    new MythicPlus_PlayerScript();
}
