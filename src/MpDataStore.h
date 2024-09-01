#ifndef MYTHICPLUS_DATASTORE_H
#define MYTHICPLUS_DATASTORE_H

#include "Group.h"
#include "MapMgr.h"
#include "Player.h"

enum MpDifficulty {
    MP_DIFFICULTY_NORMAL    = 0,
    MP_DIFFICULTY_HEROIC    = 1,
    MP_DIFFICULTY_EPIC      = 2,
    MP_DIFFICULTY_MYTHIC    = 3,
    MP_DIFFICULTY_LEGENDARY = 4,
    MP_DIFFICULTY_ASCENDANT = 5
};

struct GroupData
{
    Group* group;
    uint8 difficulty;
};

struct PlayerData
{
    Player* player;
    uint8 difficulty;
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

    std::map<ObjectGuid, GroupData>* groupData;
    std::map<ObjectGuid, PlayerData>* playerData;
    std::map<ObjectGuid, MapData>* instanceData;
    std::map<ObjectGuid, MapCreatureData>* instanceCreatureData;

public:

    // ensure we only ever have one instance of this class
    MpDataStore(const MpDataStore&) = delete;
    MpDataStore& operator=(const MpDataStore&) = delete;

    const PlayerData* GetPlayerData(ObjectGuid guid) const {
        try {
            return &playerData->at(guid);
        } catch (const std::out_of_range& oor) {
            return nullptr;
        }
    }

    const GroupData* GetGroupData(ObjectGuid guid) const {
        try {
            return &groupData->at(guid);
        } catch (const std::out_of_range& oor) {
            return nullptr;
        }
    }
    const GroupData* GetGroupData(Player *player) const {
        return GetGroupData(player->GetGroup()->GetGUID());
    };

    void AddGroupData(Group *group, int8 difficulty);
    void RemoveGroupData(Group *group);

    void AddPlayerData(ObjectGuid guid, PlayerData pd);
    void RemovePlayerData(ObjectGuid guid);
    void SetGroupDifficulty(ObjectGuid guid, uint8 difficulty);

    void AddInstanceData(ObjectGuid guid, MapData md);
    void RemoveInstanceData(ObjectGuid guid);

    void AddInstanceCreatureData(ObjectGuid guid, MapCreatureData mcd);
    void RemoveInstanceCreatureData(ObjectGuid guid);

    static MpDataStore* instance() {
        static MpDataStore instance;
        return &instance;
    }
};

#define sMpDataStore MpDataStore::instance()

#endif // MpDataStore_DATASTORE_H
