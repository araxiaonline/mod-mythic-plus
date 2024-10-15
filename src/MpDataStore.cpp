#include "CharacterDatabase.h"
#include "MpDataStore.h"
#include "MpLogger.h"

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
        _groupData->emplace(guid, groupData);
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
    _playerData->emplace(guid, pd);
}

void MpDataStore::RemovePlayerData(ObjectGuid guid) {
    MpLogger::debug("RemovePlayerData for player {}", guid.GetCounter());
    _playerData->erase(guid);
}

void MpDataStore::AddInstanceData(uint32 mapId, uint32 instanceId, MpInstanceData instanceSettings) {
    _instanceData->emplace(GetInstanceDataKey(mapId, instanceId), instanceSettings);
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
    // MpLogger::debug("AddInstanceCreatureData for creature {}", guid.GetCounter());
    _instanceCreatureData->emplace(guid, creatureData);
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
    // MpLogger::debug("RemoveInstanceCreatureData data for creature {}", guid.GetCounter());
    _instanceCreatureData->erase(guid);
}

MpScaleFactor MpDataStore::GetScaleFactor(int32 mapId, int32 difficulty) const {

    auto key = GetScaleFactorKey(mapId, difficulty);
    if (_scaleFactors->contains(key)) {
        return _scaleFactors->at(key);
    }

    return MpScaleFactor{
        .dmgBonus = 3,
        .healthBonus = 2,
        .maxDamageBonus = 30
    };
}

int32 MpDataStore::GetHealthScaleFactor(int32 mapId, int32 difficulty) const {
    return GetScaleFactor(mapId, difficulty).healthBonus;
}

int32 MpDataStore::GetDamageScaleFactor(int32 mapId, int32 difficulty) const {
    return GetScaleFactor(mapId, difficulty).dmgBonus;
}

int32 MpDataStore::GetSpellScaleFactor(int32 mapId, int32 difficulty) const {
    return GetScaleFactor(mapId, difficulty).spellBonus;
}

int32 MpDataStore::GetMaxDamageScaleFactor(int32 mapId, int32 difficulty) const {
    return GetScaleFactor(mapId, difficulty).maxDamageBonus;
}

int32 MpDataStore::LoadScaleFactors() {
    //                                                 0       1          2              3        4        5
    QueryResult result = WorldDatabase.Query("SELECT mapId, dmg_bonus, spell_bonus, hp_bonus, difficulty, max FROM mythic_plus_scale_factors");
    if (!result) {
        MpLogger::error("Failed to load mythic scale factors from database");
        return 0;
    }

    do {
        Field* fields = result->Fetch();
        uint32 mapId = fields[0].Get<uint32>();
        int32 damageBonus = fields[1].Get<int32>();
        int32 spellBonus = fields[2].Get<int32>();
        int32 healthBonus = fields[3].Get<int32>();
        int32 difficulty = fields[4].Get<int32>();
        int32 maxDamageBonus = fields[5].Get<int32>();

        MpScaleFactor scaleFactor = {
            .dmgBonus = damageBonus,
            .healthBonus = healthBonus,
            .spellBonus = spellBonus,
            .maxDamageBonus = maxDamageBonus
        };

        _mutableScaleFactors->emplace(GetScaleFactorKey(mapId, difficulty), scaleFactor);

    } while (result->NextRow());

    // move to const map one loaded so can not be changed after
    _scaleFactors = std::move(_mutableScaleFactors);

    return int32(_scaleFactors->size());
}
