
#include "MpDataStore.h"
#include "MpLogger.h"

MpDataStore::MpDataStore() {
    // constructor
}

MpDataStore::~MpDataStore() {
    // destructor
}

void MpDataStore::AddGroupData(ObjectGuid guid, GroupData gd) {
    MpLogger::debug("Adding group data for group %u", guid.GetCounter());
    groupData[guid] = gd;
}

void MpDataStore::RemoveGroupData(ObjectGuid guid) {
    MpLogger::debug("Removing group data for group %u", guid.GetCounter());
    groupData.erase(guid);
}

void MpDataStore::AddPlayerData(ObjectGuid guid, PlayerData pd) {
    MpLogger::debug("Adding player data for player %u", guid.GetCounter());
    playerData[guid] = pd;
}

void MpDataStore::RemovePlayerData(ObjectGuid guid) {
    MpLogger::debug("Removing player data for player %u", guid.GetCounter());
    playerData.erase(guid);
}

void MpDataStore::AddInstanceData(ObjectGuid guid, MapData md) {
    MpLogger::debug("Adding instance data for instance %u", guid.GetCounter());
    instanceData[guid] = md;
}

void MpDataStore::RemoveInstanceData(ObjectGuid guid) {
    MpLogger::debug("Removing instance data for instance %u", guid.GetCounter());
    instanceData.erase(guid);
}

void MpDataStore::AddInstanceCreatureData(ObjectGuid guid, MapCreatureData mcd) {
    MpLogger::debug("Adding instance creature data for creature %u", guid.GetCounter());
    instanceCreatureData[guid] = mcd;
}

void MpDataStore::RemoveInstanceCreatureData(ObjectGuid guid) {
    MpLogger::debug("Removing instance creature data for creature %u", guid.GetCounter());
    instanceCreatureData.erase(guid);
}
