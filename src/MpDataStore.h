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
struct MpGroupData
{
    Group* group;
    MpDifficulty difficulty;
    uint32 deaths;

    std::vector<std::pair<uint32,uint32>> instanceDataKeys;

    std::string ToString() const {
        return "MpGroupData: { group: " + std::to_string(group->GetGUID().GetCounter()) +
               ", difficulty: " + std::to_string(difficulty) +
               ", deaths: " + std::to_string(deaths) + " }";
    }
};

struct MpPlayerData
{
    Player* player;
    uint8 difficulty;
    uint32 deaths;
};

struct MpMultipliers
{
    float health;
    float melee;
    float spell;
    float armor;
    uint8 avgLevel;

    std::string ToString() const {
    return "MpMultipliers: { health: " + std::to_string(health) +
            ", melee: " + std::to_string(melee) +
            ", spell: " + std::to_string(spell) +
            ", armor: " + std::to_string(armor) +
            ", avgLevel: " + std::to_string(avgLevel) + " }";
    }
};

struct MpInstanceData
{
    InstanceMap* instance;
    MpDifficulty difficulty;

    // Enemy data
    MpMultipliers boss;
    MpMultipliers creature;

    // Instance Settings
    bool itemRewards;
    uint32 deathLimits;
    uint32 itemOffset;

    std::string ToString() const {
        return "MpInstanceData: { " +
               std::string("instance: ") + (instance ? "valid instance" : "nullptr") +
               ", boss: " + boss.ToString() +
               ", creature: " + creature.ToString() +
               ", itemRewards: " + (itemRewards ? "true" : "false") +
               ", deathLimits: " + std::to_string(deathLimits) +
               ", itemOffset: " + std::to_string(itemOffset) + " }";
    }

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

    std::map<ObjectGuid, MpGroupData>* _groupData;
    std::map<ObjectGuid, MpPlayerData>* _playerData;
    std::map<std::pair<uint32,uint32>,MpInstanceData>* _instanceData;
    std::map<ObjectGuid, MapCreatureData>* _instanceCreatureData;

public:

    // ensure we only ever have one instance of this class
    MpDataStore(const MpDataStore&) = delete;
    MpDataStore& operator=(const MpDataStore&) = delete;

    const MpPlayerData* GetPlayerData(ObjectGuid guid) const {
        try {
            return &_playerData->at(guid);
        } catch (const std::out_of_range& oor) {
            return nullptr;
        }
    }

    const MpGroupData* GetGroupData(ObjectGuid guid) const {

        if (_groupData->contains(guid)) {
            return &_groupData->at(guid);
        } else {
            return nullptr;
        }

    }
    const MpGroupData* GetGroupData(Player *player) const {
        return GetGroupData(player->GetGroup()->GetGUID());
    };

    // Set and remove difficluty settig for a group
    void AddGroupData(Group *group, MpGroupData gd);
    void RemoveGroupData(Group *group);
    void PushGroupInstanceKey(Group *group, uint32 mapId, uint32 instanceId);

    void AddPlayerData(ObjectGuid guid, MpPlayerData pd);
    void RemovePlayerData(ObjectGuid guid);

    void AddInstanceData(uint32 mapId, uint32 instanceId, MpInstanceData );
    MpInstanceData* GetInstanceData(uint32 mapId, uint32 instanceId);
    void RemoveInstanceData(uint32 mapId, uint32 instanceId);

    void AddInstanceCreatureData(ObjectGuid guid, MapCreatureData mcd);
    void RemoveInstanceCreatureData(ObjectGuid guid);

    // creates a unique instance key into the instance data store
    auto GetInstanceDataKey(uint32 mapId, uint32 instanceId) {
        return std::make_pair(mapId, instanceId);
    }

    static MpDataStore* instance() {
        static MpDataStore instance;
        return &instance;
    }
};

#define sMpDataStore MpDataStore::instance()

#endif // MpDataStore_DATASTORE_H
