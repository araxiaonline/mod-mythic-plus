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

/**
 * Simple struct for managing information about creatures that
 * are in a mythic+ instance.
 */
struct MpCreatureData
{
    Creature* creature;
    bool scaled;

    // Original information about the creature that was altered.
    uint8 originalLevel;
    CreatureBaseStats const* originalStats;

    // Custom difficulty modifiers to creatures at higher difficulties.
    std::vector<uint32> auras;
    std::vector<std::string> affixes;

    MpCreatureData(Creature* creature)
        : creature(creature), scaled(false)
    {
        if(creature) {
            originalLevel = creature->GetLevel();
            originalStats = sObjectMgr->GetCreatureBaseStats(
                originalLevel,
                creature->GetCreatureTemplate()->unit_class
            );
        }
    }

    void SetScaled(bool scaled) {
        this->scaled = scaled;
    }

    bool IsScaled() {
        return scaled;
    }

    /**@todo Add Affixes and Aura Spell methods */
};

class MpDataStore {
private:
    MpDataStore()
    : _playerData(std::make_unique<std::unordered_map<ObjectGuid, MpPlayerData>>()),
      _instanceData(std::make_unique<std::map<std::pair<uint32, uint32>, MpInstanceData>>()),
      _groupData(std::make_unique<std::unordered_map<ObjectGuid, MpGroupData>>()),
      _instanceCreatureData(std::make_unique<std::unordered_map<ObjectGuid, MpCreatureData>>()) {};

    inline ~MpDataStore() {

    }

    std::unique_ptr<std::unordered_map<ObjectGuid, MpPlayerData>> _playerData;

    // Instance Data map key is unique instance pair and values are modifiers of instance
    std::unique_ptr<std::map<std::pair<uint32,uint32>,MpInstanceData>> _instanceData;

    // Group Data map key is group guid and values are mythic settings set by group leader
    std::unique_ptr<std::unordered_map<ObjectGuid, MpGroupData>> _groupData;

    // Instance Creature Data map key is creature guid and values are creature itself from a mythic instance
    std::unique_ptr<std::unordered_map<ObjectGuid, MpCreatureData>> _instanceCreatureData;

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
    void AddGroupData(Group *group, MpGroupData groupData);
    void RemoveGroupData(Group *group);
    MpGroupData* GetGroupData(Group *group);
    void PushGroupInstanceKey(Group *group, uint32 mapId, uint32 instanceId);

    void AddPlayerData(ObjectGuid guid, MpPlayerData pd);
    void RemovePlayerData(ObjectGuid guid);

    // Each Map/Instance is a unique key that contains scaling information based on difficulty
    void AddInstanceData(uint32 mapId, uint32 instanceId, MpInstanceData );
    MpInstanceData* GetInstanceData(uint32 mapId, uint32 instanceId);
    void RemoveInstanceData(uint32 mapId, uint32 instanceId);

    // Methods for interacting with the map of creatures in a mythic instances
    void AddCreatureData(ObjectGuid guid, MpCreatureData creatureData);
    MpCreatureData* GetCreatureData(ObjectGuid guid);
    void RemoveCreatureData(ObjectGuid guid);
    std::vector<MpCreatureData*> GetUnscaledCreatures(uint32 mapId, uint32 instanceId);

    // creates a unique instance key into the instance data store
    auto GetInstanceDataKey(uint32 mapId, uint32 instanceId) {
        return std::make_pair(mapId, instanceId);
    }

    // accessor for this singleton
    static MpDataStore* instance() {
        static MpDataStore instance;
        return &instance;
    }
};

#define sMpDataStore MpDataStore::instance()

#endif // MpDataStore_DATASTORE_H
