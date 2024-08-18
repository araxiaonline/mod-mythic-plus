
#include "MpDataStore.h"
#include "MpLogger.h"

MpDataStore::MpDataStore() {
    // constructor
}

MpDataStore::~MpDataStore() {
    // destructor
}

void MpDataStore::AddGroupData(ObjectGuid guid, GroupData gd) {
    MpLogger::debug("AddGroupData for group {}", guid.GetCounter());
    groupData->insert({guid, gd});
}

void MpDataStore::RemoveGroupData(ObjectGuid guid) {
    MpLogger::debug("RemoveGroupData for group {}", guid.GetCounter());
    groupData->erase(guid);
}

void MpDataStore::AddPlayerData(ObjectGuid guid, PlayerData pd) {
    MpLogger::debug("AddPlayerData for player {}", guid.GetCounter());
    playerData->insert({guid, pd});
}

const PlayerData* MpDataStore::GetPlayerData() {
    return playerData;
}

void MpDataStore::SetPlayerDifficulty(ObjectGuid guid, uint8 difficulty) {
    MpLogger::debug("SetPlayerDifficulty for player {}", guid.GetCounter());
    playerData->at(guid).difficulty = difficulty;
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
