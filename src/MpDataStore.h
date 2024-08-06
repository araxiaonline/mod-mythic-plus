#ifndef MYTHICPLUS_DATASTORE_H
#define MYTHICPLUS_DATASTORE_H

#include "Group.h"
#include "MapMgr.h"

enum MpDifficulty {
    MP_DIFFICULTY_NORMAL    = 0,
    MP_DIFFICULTY_HEROIC    = 1,
    MP_DIFFICULTY_EPIC      = 2,
    MP_DIFFICULTY_HEROIC_25 = 3,
    MP_DIFFICULTY_MYTHIC    = 4,
    MP_DIFFICULTY_LEGENDARY = 8,
    MP_DIFFICULTY_ASCENDANT = 12
};

struct GroupData
{
    Group* group;
    uint8 difficulty;
};

struct PlayerData
{
    Player* player;
};

struct MapData
{
    Map* instance;
};

struct MapCreatureData
{
    Creature* creature;
    MapEntry* instance;
};

class MpDataStore {
private:
    MpDataStore();
    ~MpDataStore();

    std::map<ObjectGuid, GroupData> groupData;
    std::map<ObjectGuid, PlayerData> playerData;
    std::map<ObjectGuid, MapData> instanceData;
    std::map<ObjectGuid, MapCreatureData> instanceCreatureData;

public:
    void AddGroupData(ObjectGuid guid, GroupData gd);
    void RemoveGroupData(ObjectGuid guid);
    void AddPlayerData(ObjectGuid guid, PlayerData pd);
    void RemovePlayerData(ObjectGuid guid);
    void AddInstanceData(ObjectGuid guid, MapData md);
    void RemoveInstanceData(ObjectGuid guid);
    void AddInstanceCreatureData(ObjectGuid guid, MapCreatureData mcd);
    void RemoveInstanceCreatureData(ObjectGuid guid);

    static MpDataStore * getInstance() {
        static MpDataStore instance;
        return &instance;
    }
};

#endif // MYTHICPLUS_DATASTORE_H

