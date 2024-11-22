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

    void OnPlayerKilledByCreature(Creature* killer, Player* player)
    {
        MpLogger::debug(">>>>>>>>>>>>>>>>>>>>>>> OnPlayerJustDied: %s", player->GetName());

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
            auto newPlayerData = std::make_unique<MpPlayerData>(player, data->difficulty, group->GetGUID().GetCounter());
            sMpDataStore->AddPlayerData(player->GetGUID(), *newPlayerData);
            playerData = newPlayerData.get();
            newPlayerData.release();
        }

        auto mapKey = sMpDataStore->GetInstanceDataKey(mapId, player->GetInstanceId());
        playerData->instanceData.emplace(mapKey, MpPlayerInstanceData{
            .deaths = 0,
        });

        sMpDataStore->SavePlayerInstanceData(player,playerData);
    }
};

void Add_MP_PlayerScripts()
{
    MpLogger::debug("Add_MP_PlayerScripts()");
    new MythicPlus_PlayerScript();
}
