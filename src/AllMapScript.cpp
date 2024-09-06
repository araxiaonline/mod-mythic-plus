#include "Chat.h"
#include "Log.h"
#include "MapMgr.h"
#include "MpLogger.h"
#include "MpDataStore.h"
#include "MythicPlus.h"
#include "Player.h"
#include "ScriptMgr.h"

class MythicPlus_AllMapScript : public AllMapScript
{
public:
    MythicPlus_AllMapScript() : AllMapScript("MythicPlus_AllMapScript")
    {
    }

    void OnCreateMap(Map* map)
    {
        MpLogger::debug("AllMapScript::OnCreateMap(): {}", map->GetMapName());
        if (!sMythicPlus->IsMapEligible(map)) {
            return;
        }

    }

    /**
     * When a player enters the map check it needs to set up the instance data
     */
    void OnPlayerEnterAll(Map* map, Player* player)
    {
        MpLogger::debug("AllMapScript::OnPlayerEnterAll(): {}", map->GetMapName());

        if (!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        Group* group = player->GetGroup();
        if (group) {
            MpLogger::debug("Player {} entered map {} in groupLeader {}", player->GetName(), map->GetMapName(), group->GetLeaderName());
        } else {
            return;
        }

        // if there is not any group data for this group then just bail
        const MpGroupData* groupData = sMpDataStore->GetGroupData(group->GetGUID());
        if (!groupData) {
            return;
        }

        uint8 avgLevel = 0;
        MpInstanceData instanceData;
        switch(groupData->difficulty) {
            case MP_DIFFICULTY_MYTHIC:
                MpLogger::debug("Setting up Mythic instance data for group {}", group->GetGUID().GetCounter());
                instanceData.boss = sMythicPlus->mythicBossModifiers;
                instanceData.creature = sMythicPlus->mythicDungeonModifiers;
                instanceData.itemRewards = sMythicPlus->EnableItemRewards;
                instanceData.deathLimits = sMythicPlus->mythicDeathAllowance;
                instanceData.itemOffset = sMythicPlus->mythicItemOffset;
                break;
            case MP_DIFFICULTY_LEGENDARY:
                MpLogger::debug("Setting up Legendary instance data for group {}", group->GetGUID().GetCounter());
                instanceData.boss = sMythicPlus->legendaryBossModifiers;
                instanceData.creature = sMythicPlus->legendaryDungeonModifiers;
                instanceData.itemRewards = sMythicPlus->EnableItemRewards;
                instanceData.deathLimits = sMythicPlus->legendaryDeathAllowance;
                instanceData.itemOffset = sMythicPlus->legendaryItemOffset;
                break;
            case MP_DIFFICULTY_ASCENDANT:
                MpLogger::debug("Setting up Ascendant instance data for group {}", group->GetGUID().GetCounter());
                instanceData.boss = sMythicPlus->ascendantBossModifiers;
                instanceData.creature = sMythicPlus->ascendantDungeonModifiers;
                instanceData.itemRewards = sMythicPlus->EnableItemRewards;
                instanceData.deathLimits = sMythicPlus->ascendantDeathAllowance;
                instanceData.itemOffset = sMythicPlus->ascendantItemOffset;
                break;
            default:
                MpLogger::debug("No difficulty set for group {}", group->GetGUID().GetCounter());
                return;
        }

        instanceData.difficulty = groupData->difficulty;
        instanceData.instance = (InstanceMap*)sMapMgr->FindMap(map->GetId(), map->GetInstanceId());

        MpLogger::debug("Setting up instance data for group {} for map {} instance {} data {}",
            group->GetGUID().GetCounter(),
            map->GetMapName(),
            map->GetInstanceId(),
            instanceData.ToString()
        );
        sMpDataStore->AddInstanceData(map->GetId(), map->GetInstanceId(), instanceData);
    }

    // When an instance is destroyed remove the instance data from the data store
    virtual void OnDestroyInstance(MapInstanced* /*mapInstanced*/, Map* map)
    {
        if (!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        MpLogger::debug("AllMapScript::OnDestroyInstance(): {}", map->GetMapName());
        sMpDataStore->RemoveInstanceData(map->GetId(), map->GetInstanceId());
    }
};

void Add_MP_AllMapScripts()
{
    MpLogger::debug("Add_MP_AllMapScripts()");
    new MythicPlus_AllMapScript();
}
