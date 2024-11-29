#include "MpLogger.h"
#include "MpDataStore.h"
#include "MpScheduler.h"
#include "MythicPlus.h"
#include "Player.h"
#include "Group.h"
#include "ScriptMgr.h"
#include "Chat.h"

class MythicPlus_PlayerScript : public PlayerScript
{
public:
    MythicPlus_PlayerScript() : PlayerScript("MythicPlus_PlayerScript") {}

    void OnPlayerKilledByCreature(Creature* killer, Player* player) override
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
            Map* map = player->GetMap();
            Map::PlayerList players = map->GetPlayers();

            if(!map || players.IsEmpty()) {
                return;
            }

            Group* group = player->GetGroup();
            if(!group) {
                MpLogger::warn("Player {} is not in a group.", player->GetName());
                return;
            }

            // map->RemoveAllPlayers();
            MpLogger::info("Starting scheduled failure notification");
            sMpScheduler->ScheduleWorldTask(1s, [](TaskContext ctx) {
                MpLogger::info("<<<<<<<<<<<  Player Death Scheduler fire >>>>>>>>>>>>>");
            });
            // sMpScheduler->GetWorldScheduler().Schedule(1s, [playerName = player->GetName()](TaskContext ctx) {
            //     MpLogger::info("<<<<<<<<<<<  Player Death Scheduler fire {} >>>>>>>>>>>>>", playerName);
            //     return;
            // });
                    // std::vector<Player*> players = GetGroupMembers(player);
                    // MpLogger::info("Failed mythic+ instance run notification fired. ");
                    // WorldPacket data;

                    // for(Player* player : players)
                    // {
                    //     MpLogger::info("Seding notification of failure to player: {}", player->GetName());
                    //     player->GetSession()->SendShowBank(player->GetGUID());
                    //     // player->GetSession()->SendNotification("Your group has died too many time to continue.");
                    //     // ChatHandler::BuildChatPacket(data, CHAT_MSG_RAID_BOSS_EMOTE, LANG_UNIVERSAL, nullptr, player, message);
                    //     // player->GetSession()->SendPacket(&data);
                    // }

                // map->ToInstanceMap()->Reset(0);
            // );

        }
        return;

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

    std::vector<Player*> GetGroupMembers(Player* currentPlayer)
    {
        std::vector<Player*> groupPlayers;

        Group* group = currentPlayer->GetGroup();
        if (!group)
        {
            MpLogger::warn("Player is not in a group.");
            return groupPlayers;
        }

        group->DoForAllMembers([&](Player* member) {
            groupPlayers.push_back(member);
        });

        return groupPlayers;
    }

};

void Add_MP_PlayerScripts()
{
    MpLogger::debug("Add_MP_PlayerScripts()");
    new MythicPlus_PlayerScript();
}
