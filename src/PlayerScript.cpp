#include "MpLogger.h"
#include "MpDataStore.h"
#include "MythicPlus.h"
#include "Player.h"
#include "Group.h"
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

        MpGroupData *data = sMpDataStore->GetGroupData(player->GetGroup());
        if (!data) {
            return;
        }

        MpPlayerData const *playerData = sMpDataStore->GetPlayerData(player->GetGUID());


        data->deaths++;

    }

    void OnSave(Player* player) override
    {
        // if the player is in a group save the current player difficulty
        Group* group = player->GetGroup();
        if(group) {
            MpGroupData* data = sMpDataStore->GetGroupData(group);
            if(data) {

                MpPlayerData const * playerData = sMpDataStore->GetPlayerData(player->GetGUID());
                if(playerData) {
                    sMpDataStore->SavePlayerInstanceData(player, playerData);
                }

            }
        }
    }

    // When a player is bound to an instance need to make sure they are saved in the data soure to retrieve later.
    void OnBindToInstance(Player* player, Difficulty /*difficulty*/, uint32 mapId, bool /*permanent*/) override
    {
        if(!player) {
            return;
        }

        Group* group = player->GetGroup();

        // If they are not in a group do nothing.
        if(!group) {
            return;
        }

        MpGroupData* data = sMpDataStore->GetGroupData(group);

        // If there is not any mythic+ data set for this group do nothing.
        if(!data) {
            return;
        }

        Map* map = player->GetMap();
        if(!map) {
            MpLogger::warn("Player {} is not in a map", player->GetName());
            return;
        }

        // If made it here we need to add player data for the map and instance
        MpPlayerData playerData = MpPlayerData{
            .player = player,
            .groupId = group->GetGUID().GetCounter(),
            .mapId = mapId,
            .instanceId = map->GetInstanceId(),
            .difficulty = data->difficulty,
            .deaths = 0
        };

        sMpDataStore->AddPlayerData(player->GetGUID(), playerData);
    }
};

void Add_MP_PlayerScripts()
{
    MpLogger::debug("Add_MP_PlayerScripts()");
    new MythicPlus_PlayerScript();
}
