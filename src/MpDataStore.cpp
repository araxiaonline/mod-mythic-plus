
#include "MpDataStore.h"
#include "MpLogger.h"
#include "Player.h"

// Adds an entry for the group difficult to memory and updats database
void MpDataStore::AddGroupData(Group *group, MpGroupData groupData) {
    ObjectGuid guid = group->GetGUID();

    if (!guid) {
        MpLogger::error("AddGroupData called with invalid group GUID");
        return;
    }

    if (!groupData.difficulty) {
        MpLogger::error("AddGroupData called with invalid difficulty");
        return;
    }
    // if we already have data override it
    if (_groupData->contains(guid)) {
        _groupData->at(guid) = groupData;
    } else {
        _groupData->insert({guid, groupData});
    }

    MpLogger::debug("Add Group difficulty for group {} to {}", guid.GetCounter());

    CharacterDatabase.Execute("REPLACE INTO group_difficulty (guid, difficulty) VALUES ({},{}) ",
        guid.GetCounter(),
        groupData.difficulty
    );
}

MpGroupData* MpDataStore::GetGroupData(Group* group) {

    if (auto it = _groupData->find(group->GetGUID()); it != _groupData->end()) {
        return &(it->second);
    }

    return nullptr;
}

/**
 * This keeps track of instance keys (mapId, instanceId) for a group this can be used to
 * reset instance settings in the instanceData store when a difficulty is changed by the group leader.
 */
void MpDataStore::PushGroupInstanceKey(Group *group, uint32 mapId, uint32 instanceId) {
    ObjectGuid guid = group->GetGUID();

    if (!guid) {
        MpLogger::error("PushGroupInstanceKey called with invalid group GUID");
        return;
    }

    if (!mapId || !instanceId) {
        MpLogger::error("PushGroupInstanceKey called with invalid mapId or instanceId");
        return;
    }

    if (!_groupData->contains(guid)) {
        MpLogger::error("PushGroupInstanceKey called with invalid group GUID");
        return;
    }

    _groupData->at(guid).instanceDataKeys.push_back(GetInstanceDataKey(mapId, instanceId));
}

// This clears out any group data from memory and the database
void MpDataStore::RemoveGroupData(Group *group) {
    MpLogger::debug("RemoveGroupData for group {}", group->GetGUID().GetCounter());
    _groupData->erase(group->GetGUID());

    CharacterDatabase.Execute("DELETE FROM group_difficulty WHERE guid = {}) ", group->GetGUID().GetCounter());
}

void MpDataStore::AddPlayerData(ObjectGuid guid, MpPlayerData pd) {
    MpLogger::debug("AddPlayerData for player {}", guid.GetCounter());
    _playerData->insert({guid, pd});
}

void MpDataStore::RemovePlayerData(ObjectGuid guid) {
    MpLogger::debug("RemovePlayerData for player {}", guid.GetCounter());
    _playerData->erase(guid);
}

void MpDataStore::AddInstanceData(uint32 mapId, uint32 instanceId, MpInstanceData instanceSettings) {
    _instanceData->insert({GetInstanceDataKey(mapId, instanceId), instanceSettings});
}

MpInstanceData* MpDataStore::GetInstanceData(uint32 mapId, uint32 instanceId) {

    if (!_instanceData->contains(GetInstanceDataKey(mapId, instanceId))) {
        return nullptr;
    }

    return &_instanceData->at(GetInstanceDataKey(mapId, instanceId));
}

void MpDataStore::RemoveInstanceData(uint32 mapId, uint32 instanceId) {
    _instanceData->erase(GetInstanceDataKey(mapId, instanceId));
}

void MpDataStore::AddCreatureData(ObjectGuid guid, MpCreatureData creatureData) {
    MpLogger::debug("AddInstanceCreatureData for creature {}", guid.GetCounter());
    _instanceCreatureData->insert({guid, creatureData});
}

MpCreatureData* MpDataStore::GetCreatureData(ObjectGuid guid) {
    if (!_instanceCreatureData->contains(guid)) {
        return nullptr;
    }

    return &_instanceCreatureData->at(guid);
}

 std::vector<MpCreatureData*> MpDataStore::GetUnscaledCreatures(uint32 mapId, uint32 instanceId) {
    std::vector<MpCreatureData*> creatures;
    for (auto& [guid, creatureData] : *_instanceCreatureData) {
        Creature* creature = creatureData.creature;

        if (creature->GetMapId() == mapId && creature->GetInstanceId() == instanceId && !creatureData.IsScaled()) {
            creatures.push_back(&creatureData);
        }
    }
    return creatures;
 }

void MpDataStore::RemoveCreatureData(ObjectGuid guid) {
    MpLogger::debug("RemoveInstanceCreatureData data for creature {}", guid.GetCounter());
    _instanceCreatureData->erase(guid);
}
