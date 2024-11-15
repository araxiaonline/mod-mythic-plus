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

        // Update in memory store
        playerData->AddDeath(map->GetId(), map->GetInstanceId());

        // Track deaths and add to mp_player_death_stats
        Creature* creature = killer->ToCreature();
        if(creature) {
            sMpDataStore->DBAddPlayerDeath(player, creature);
        } else {
            sMpDataStore->DBAddPlayerDeath(player);
        }

        // update that group data in the database
        sMpDataStore->DBAddGroupDeath(data->group, map->GetId(), map->GetInstanceId(), data->difficulty);
    }

    void OnSave(Player* player) override { }

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
