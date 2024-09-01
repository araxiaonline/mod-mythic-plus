
#include "MpDataStore.h"
#include "MpLogger.h"
#include "Player.h"

MpDataStore::MpDataStore() {
    // constructor
}

MpDataStore::~MpDataStore() {
    // destructor
}

// Adds an entry for the group difficult to memory and updats database
void MpDataStore::AddGroupData(Group *group, int8 difficulty) {
    ObjectGuid guid = group->GetGUID();

    if (!guid) {
        MpLogger::error("AddGroupData called with invalid group GUID");
        return;
    }
    MpLogger::debug("Add Group difficulty for group {} to {}", guid.GetCounter());

    CharacterDatabase.Execute("REPLACE INTO group_difficulty (guid, difficulty) VALUES ({},{}) ", guid.GetCounter(), difficulty);
}

void MpDataStore::RemoveGroupData(Group *group) {
    MpLogger::debug("RemoveGroupData for group {}", group->GetGUID().GetCounter());
    groupData->erase(group->GetGUID());

    CharacterDatabase.Execute("DELETE FROM group_difficulty WHERE guid = {}) ", group->GetGUID().GetCounter());
}

void MpDataStore::AddPlayerData(ObjectGuid guid, PlayerData pd) {
    MpLogger::debug("AddPlayerData for player {}", guid.GetCounter());
    playerData->insert({guid, pd});
}

void MpDataStore::RemovePlayerData(ObjectGuid guid) {
    MpLogger::debug("RemovePlayerData for player {}", guid.GetCounter());
    playerData->erase(guid);
}

void MpDataStore::AddInstanceData(ObjectGuid guid, MapData md) {
    MpLogger::debug("AddInstanceData for instance {}", guid.GetCounter());
    instanceData->insert({guid, md});
}

void MpDataStore::RemoveInstanceData(ObjectGuid guid) {
    MpLogger::debug("RemoveInstanceData for instance {}", guid.GetCounter());
    instanceData->erase(guid);
}

void MpDataStore::AddInstanceCreatureData(ObjectGuid guid, MapCreatureData mcd) {
    MpLogger::debug("AddInstanceCreatureData for creature {}", guid.GetCounter());
    instanceCreatureData->insert({guid, mcd});
}

void MpDataStore::RemoveInstanceCreatureData(ObjectGuid guid) {
    MpLogger::debug("RemoveInstanceCreatureData data for creature {}", guid.GetCounter());
    instanceCreatureData->erase(guid);
}
