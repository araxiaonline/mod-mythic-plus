#include "MpLogger.h"
#include "MpDataStore.h"
#include "MythicPlus.h"
#include "Player.h"
#include "Group.h"
#include "ScriptMgr.h"
#include "Chat.h"

class MythicPlus_PlayerScript : public PlayerScript
{
public:
    MythicPlus_PlayerScript() : PlayerScript("MythicPlus_PlayerScript") { }

    void OnPlayerKilledByCreature(Creature* killer, Player* player)
    {
        Map* map = player->GetMap();
        if(!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        if (!player) {
            return;
        }

        Group* group = player->GetGroup();
        if(!group) {
            MpLogger::warn("Missing group data for player {}", player->GetName());
            return;
        }

        MpGroupData *data = sMpDataStore->GetGroupData(player->GetGroup());
        if (!data) {
            MpLogger::warn("Missin group data for player {}", player->GetName());
            return;
        }

        MpPlayerData *playerData = sMpDataStore->GetPlayerData(player->GetGUID());
        if (!playerData) {
            MpLogger::warn("Missin player data for player {}", player->GetName());
            return;
        }

        playerData->AddDeath(map->GetId(), map->GetInstanceId());

        if(killer) {
            sMpDataStore->DBAddPlayerDeath(player, killer, data->difficulty);
        } else {
            sMpDataStore->DBAddPlayerDeath(player);
        }

        sMpDataStore->DBAddGroupDeath(group, player->GetMapId(), player->GetInstanceId(), data->difficulty);

        uint32 totalDeaths = data->GetDeaths(player->GetMapId(), player->GetInstanceId());
        if(totalDeaths > 1) {
            // kick the group out of the instance

            Map* map = player->GetMap();
            Map::PlayerList players = map->GetPlayers();

            if(!map || players.IsEmpty()) {
                return;
            }

            // map->RemoveAllPlayers();
            for(auto it = players.begin(); it != players.end(); ++it)
            {
                Player* player = it->GetSource();
                player->GetSession()->SendNotification("Your group has died too many time to continue. %u", totalDeaths);
            }
            map->RemoveAllPlayers();
            map->ToInstanceMap()->Reset(0);
        }

    }

    void OnSave(Player* player) {

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

        // get the player data or set it up
        MpPlayerData* playerData = sMpDataStore->GetPlayerData(player->GetGUID());
        if(!playerData) {
            playerData = new MpPlayerData(player, data->difficulty, group->GetGUID().GetCounter());
            sMpDataStore->AddPlayerData(player->GetGUID(), playerData);
        }

        // Add this players data to the group data
        data->AddPlayerData(playerData);

        auto mapKey = sMpDataStore->GetInstanceDataKey(mapId, player->GetInstanceId());
        playerData->instanceData.emplace(mapKey, MpPlayerInstanceData{
            .deaths = 0,
        });

        sMpDataStore->DBUpdatePlayerInstanceData(player->GetGUID(), data->difficulty, map->GetId(), map->GetInstanceId());
        sMpDataStore->DBUpdateGroupData(group->GetGUID(), data->difficulty, map->GetId(), map->GetInstanceId(), 0);
    }
};

void Add_MP_PlayerScripts()
{
    MpLogger::debug("Add_MP_PlayerScripts()");
    new MythicPlus_PlayerScript();
}
