#include "MpLogger.h"
#include "MpDataStore.h"
#include "MpScheduler.h"
#include "MythicPlus.h"
#include "Player.h"
#include "Group.h"
#include "ScriptMgr.h"
#include "TaskScheduler.h"
#include "AdvancementMgr.cpp"
#include "Chat.h"
#include "Channel.h"
#include "ChannelMgr.h"

class MythicPlus_PlayerScript : public PlayerScript
{
public:
    MythicPlus_PlayerScript() : PlayerScript("MythicPlus_PlayerScript") {}

    /**
     * Mythic+ special actions when a player dies:
     * - Track the death for the group
     * - Update the player death stats
     * - Determine whether or not the Group failed the instance due to death count setting.
     */

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
        MpLogger::info("Total Deaths: {}", totalDeaths);
        if(totalDeaths > 1) {
            MpLogger::debug(" :::: Player Deaths for Group too high! ::::::");

            TaskScheduler& wScheduler = sMpScheduler->GetWorldScheduler();
            wScheduler.Schedule(10s, MP_WORLD_TASK_GROUP, [player, map](TaskContext /*ctx*/) {
                Group* group = player->GetGroup();
                if(!group) {
                    return;
                }

                MythicPlus::GroupReset(group, map);
            });
        }
//         if(totalDeaths > 1) {
//             Map* map = player->GetMap();
//             if(!map) {
//                 return;
//             }
//             Group* group = player->GetGroup();
//             if(!group) {
//                 MpLogger::warn("Player {} is not in a group.", player->GetName());
//                 return;
//             }

// // map->RemoveAllPlayers();
//                 MpLogger::info("Starting scheduled failure notification");
//                 // auto testlambda = [](TaskContext ctx) { return; };
                // sMpScheduler->ScheduleWorldTask(1s, [](TaskContext ctx) {
                //     MpLogger::info("<<<<<<<<<<<  Player Death Scheduler fire >>>>>>>>>>>>>");
                // });

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
    // }
    }

    void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Channel* channel) override
    {
        if(!player || !channel) {
            return;
        }

        // Handle chat message coming in to the data channel from the client. This allows backedend calls into the module
        // from Eluna / AIO without the need to modify mod-eluna directly.
        if(type == CHAT_MSG_CHANNEL && MP_DATA_CHAT_CHANNEL == channel->GetName()) {
            MpLogger::info("Player {} sent a message {} to channel {}", player->GetName(), msg, channel->GetName());
        }
    }

    /**
     * When a player logs in add them to the data channel specifically for Mythic+ communication
     * between UI and server module.
     *
     * Load advancement data for the player at load time used to apply buffs.
     */
    void OnLogin(Player* player) override
    {
        if(!player) {
            return;
        }

        // Create a channel called MpEx if it does not exist
        ChannelMgr* cmg = ChannelMgr::forTeam(TEAM_NEUTRAL);
        Channel* channel = cmg->GetChannel("MpEx", player);

        // If the channel does not yet, exist the first player to login to the server will ensure it is created.
        if(!channel) {
            channel = cmg->GetJoinChannel("MPEx",0);
        }

        // Load the player advancement data for the player when they login
        int32 size = sAdvancementMgr->LoadPlayerAdvancements(player);
        MpLogger::info("Loaded {} player advancements for player {}", size, player->GetName());

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
