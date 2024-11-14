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

        MpPlayerData *playerData = sMpDataStore->GetPlayerData(player->GetGUID());
        if (!playerData) {
            return;
        }

        playerData->AddDeath(map->GetId(), map->GetInstanceId());
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
                    // sMpDataStore->SavePlayerInstanceData(player, playerData);
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

        MpPlayerData* playerData = sMpDataStore->GetPlayerData(player->GetGUID());
        if(!playerData) {
            MpLogger::warn("PlayerData not found for player {} perhaps not in mythic+ group, bad player state?", player->GetName());
        }

        auto mapKey = sMpDataStore->GetInstanceDataKey(mapId, player->GetInstanceId());
        playerData->instanceData.emplace(mapKey, MpPlayerInstanceData{
            .deaths = 0,
        });

        sMpDataStore->DBUpdatePlayerInstanceData(player->GetGUID(), data->difficulty, map->GetId(), player->GetInstanceId(), 0);

        if(group->GetLeaderGUID() == player->GetGUID()) {
            sMpDataStore->DBUpdateGroupData(group->GetGUID(), data->difficulty, map->GetId(), player->GetInstanceId(), 0);
        }

    }
};

void Add_MP_PlayerScripts()
{
    MpLogger::debug("Add_MP_PlayerScripts()");
    new MythicPlus_PlayerScript();
}
