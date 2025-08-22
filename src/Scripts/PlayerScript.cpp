#include "MpLogger.h"
#include "MpDataStore.h"
#include "MpScheduler.h"
#include "MythicPlus.h"
#include "Player.h"
#include "Group.h"
#include "ScriptMgr.h"
#include "TaskScheduler.h"
#include "AdvancementMgr.h"
#include "Formulas.h"

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
            MpLogger::warn("Missing group data for player {}", player->GetName());
            return;
        }

        MpPlayerData *playerData = sMpDataStore->GetPlayerData(player->GetGUID());
        if (!playerData) {
            MpLogger::warn("Missing player data for player {}", player->GetName());
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
    }

    void OnBeforeLootMoney(Player* player, Loot* loot) override
    {


        if (!loot->sourceWorldObjectGUID.IsCreature()) return;

        Creature* creature = player->GetMap()->GetCreature(loot->sourceWorldObjectGUID);
        if (!creature) return;

        #ifdef NPC_BOT
            if(creature->IsNPCBotOrPet()) {
                return;
            }
        #endif

        // Check if this is a Mythic+ scaled creature
        MpCreatureData* creatureData = sMpDataStore->GetCreatureData(creature->GetGUID());
        if (!creatureData || !creatureData->IsScaled()) return;

        // Different gold ranges based on creature rank
        uint32 bossMinGold = 10000; 
        uint32 bossMaxGold = 13500;
        
        uint32 minGold, maxGold;
        
        // Determine gold range based on creature rank
        if (creature->isWorldBoss() || creature->IsDungeonBoss())
        {
            // Boss: full range
            minGold = bossMinGold;
            maxGold = bossMaxGold;
        }
        else if (creature->GetCreatureTemplate()->rank == CREATURE_ELITE_RARE || 
                 creature->GetCreatureTemplate()->rank == CREATURE_ELITE_ELITE)
        {
            // Elite: 70% of boss range
            minGold = uint32(bossMinGold * 0.7f);
            maxGold = uint32(bossMaxGold * 0.7f);
        }
        else
        {
            // Normal: 40% of boss range
            minGold = uint32(bossMinGold * 0.4f);
            maxGold = uint32(bossMaxGold * 0.4f);
        }
        
        // Generate random gold amount in appropriate range
        uint32 newGold = urand(minGold, maxGold);

        // Apply server money rate
        newGold = uint32(newGold * sWorld->getRate(RATE_DROP_MONEY));

        loot->gold = newGold;
    }

    void OnGiveXP(Player* player, uint32& amount, Unit* victim, uint8 xpSource) override
    {
        if (xpSource != XPSOURCE_KILL || !victim) return;

        Creature* creature = victim->ToCreature();
        if (!creature) return;

        #ifdef NPC_BOT
            if(creature->IsNPCBotOrPet()) {
                return;
            }
        #endif

        // Check if this is a Mythic+ scaled creature
        MpCreatureData* creatureData = sMpDataStore->GetCreatureData(creature->GetGUID());
        if (!creatureData || !creatureData->IsScaled()) return;

        // Recalculate XP using scaled level instead of original level
        uint32 newBaseXP = Acore::XP::BaseGain(
            player->GetLevel(),
            creature->GetLevel(), // This is now the scaled level
            GetContentLevelsForMapAndZone(creature->GetMapId(), creature->GetZoneId())
        );

        // Apply same modifiers as original calculation
        float xpMod = 1.0f;
        if (creature->isElite()) {
            xpMod *= creature->GetMap()->IsDungeon() ? 2.75f : 2.0f;
        }
        xpMod *= creature->GetCreatureTemplate()->ModExperience;

        amount = uint32(newBaseXP * xpMod * 1.5f); // flat bonus modifier for mythic dungeons
    }
    void OnLogin(Player* player) override
    {
        MpLogger::info("Player {} logged in", player->GetName());

        // Load the player advancement data for the player when they login
        sAdvancementMgr->LoadPlayerAdvancements(player);

        // Cast all unique advancement spells
        for(uint32 i = 1; i <= 10; ++i) {
            uint32 spellId = 80000000 + i;
            MpLogger::info("Casting spell {} to player {}", spellId, player->GetName());
            player->AddAura(spellId, player);
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
