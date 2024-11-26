#ifndef MYTHICPLUS_DATASTORE_H
#define MYTHICPLUS_DATASTORE_H

#include "Creature.h"
// #include "CreatureOverride.h"
#include "Group.h"
#include "MapMgr.h"
#include "Player.h"
#include "MpLogger.h"
#include "ObjectGuid.h"

#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include <memory>

enum MpDifficulty {
    MP_DIFFICULTY_NORMAL    = 0,
    MP_DIFFICULTY_HEROIC    = 1,
    MP_DIFFICULTY_EPIC      = 2,
    MP_DIFFICULTY_MYTHIC    = 3,
    MP_DIFFICULTY_LEGENDARY = 4,
    MP_DIFFICULTY_ASCENDANT = 5
};

class MpDataStore;

struct MpPlayerInstanceData {
    uint32 deaths = 0;
};

// struct used to track player performance in an instance.
struct MpPlayerData
{
    Player* player;
    MpDifficulty difficulty;
    uint32 groupId;

    // list of maps and instance player is bound to and mythic data related to it
    std::map<std::pair<uint32,uint32>,MpPlayerInstanceData> instanceData;

    MpPlayerData(Player* p, MpDifficulty diff, uint32_t groupId)
        : player(p), difficulty(diff), groupId(groupId)  {
        }

    void AddDeath(uint32 mapId, uint32 instanceId) {
        auto key = std::make_pair(mapId, instanceId);

        if(instanceData.contains(key)) {
            instanceData[key].deaths++;
        } else {
            instanceData[key] = MpPlayerInstanceData{.deaths = 1};
        }

        MpLogger::info("========= Player {} added death to instance data {}", player->GetName(), instanceData[key].deaths);
    }

    uint32 GetDeaths(uint32 mapId, uint32 instanceId) const {
        auto key = std::make_pair(mapId, instanceId);
        if(instanceData.contains(key)) {
            return instanceData.at(key).deaths;
        }
        return 0;
    }

    void ResetDeathCount(uint32 mapId, uint32 instanceId) {
        auto key = std::make_pair(mapId, instanceId);
        if(instanceData.contains(key)) {
            instanceData[key].deaths = 0;
        }
    }

    void ResetAllDeathCounts() {
        for(auto& [key, data] : instanceData) {
            data.deaths = 0;
        }
    }
};

// Struct used to track entire group performance.
struct MpGroupData
{
    Group* group;
    MpDifficulty difficulty;
    std::vector<MpPlayerData*> players;


    MpGroupData() : group(nullptr), difficulty(MpDifficulty{}) {
        players.reserve(5);

    }

    MpGroupData(Group* group, MpDifficulty difficulty)
        : group(group), difficulty(difficulty) {
        players.reserve(5);
    }

    uint32 GetDeaths(uint32 mapId, uint32 instanceId) const {
        uint32 deaths = 0;
        for (const MpPlayerData* player : players) {

            auto mapKey = std::make_pair(mapId, instanceId);
            auto instanceData = player->instanceData;

            MpLogger::info(">>>>>>>>>>> Player: {} has {} deaths", player->player->GetName(), player->GetDeaths(mapId, instanceId));
            deaths += player->GetDeaths(mapId, instanceId);
        }
        return deaths;
    }

    void ResetGroupDeaths(uint32 mapId, uint32 instanceId) {
        for (MpPlayerData* player : players) {
            player->ResetDeathCount(mapId, instanceId);


        }
    }

    void AddPlayerData(MpPlayerData* playerData) {
        // Check if the playerData is already in the vector
        if (std::any_of(players.begin(), players.end(), [playerData](MpPlayerData* existingData) {
            return existingData->player == playerData->player;
        })) {

            MpLogger::warn("PlayerData for player {} is already in the players vector", playerData->player->GetName());
            return;
        }

        // Add playerData to the vector
        players.push_back(playerData);
    }

    std::string ToString() const {
        Map* map = group->GetLeader()->GetMap();

        uint32 deaths = 0;
        if(map) {
            deaths = GetDeaths(map->GetId(), map->GetInstanceId());
        }

        return "MpGroupData: { group: " + std::to_string(group->GetGUID().GetCounter()) +
               ", difficulty: " + std::to_string(difficulty) +
               ", deaths: " + std::to_string(deaths) + " }";
    }

};

struct MpScaleFactor
{
    int32 dmgBonus;
    int32 healthBonus;
    int32 spellBonus;
    int32 maxDamageBonus;

    std::string ToString() const {
        return "MpScaleFactor: { dmgBonus: " + std::to_string(dmgBonus) +
               ", healthBonus: " + std::to_string(healthBonus) +
               ", spellBonus: " + std::to_string(spellBonus) + "}";
    }

};

struct MpMultipliers
{
    float health;
    float melee;
    float baseDamage;
    float spell;
    float armor;
    uint8 avgLevel;

    std::string ToString() const {
    return "MpMultipliers: { health: " + std::to_string(health) +
            ", melee: " + std::to_string(melee) +
            ", melee: " + std::to_string(baseDamage) +
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

    // AttackPower calculated based on settings
    uint32 NewAttackPower;

    // New Scaling Multiplier based on database factors + level growth formula
    float AttackPowerScaleMultiplier;

    // Original information about the creature that was altered.
    uint8 originalLevel;

    CreatureBaseStats const* originalStats;
    MpDifficulty difficulty;

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

        auras.reserve(3);
        affixes.reserve(3);
    }

    void SetScaled(bool scaled) {
        this->scaled = scaled;
    }

    void SetDifficulty(MpDifficulty difficulty) {
        this->difficulty = difficulty;
    }

    bool IsScaled() {
        return scaled;
    }

    std::string ToString() const {

        std::string origStatsStr;
        if(originalStats) {
            uint32 health = *originalStats->BaseHealth;
            uint32 mana = originalStats->BaseMana;
            uint32 armor = originalStats->BaseArmor;
            uint32 ap = originalStats->AttackPower;

            origStatsStr = "Original Stats: \n Health: " + std::to_string(health) + "\n" +
                            "Mana: " + std::to_string(mana) + "\n" +
                            "Armor: " + std::to_string(armor) + "\n" +
                            "Attack Power: " + std::to_string(ap) + "\n";

        } else {
            origStatsStr = "Original Stats Display Failed: \n Did you select target or in an non-scaled instance? \n";
        }



        return " MpCreatureData: \n Original level: " + std::to_string(originalLevel) + "\n" +
                origStatsStr +
                " NewAttackPower: " + std::to_string(NewAttackPower) + "\n" +
                " AttackPowerScaleMultiplier: " + std::to_string(AttackPowerScaleMultiplier) + "\n" +
                " Difficulty: " + std::to_string(difficulty) + "\n" +
                " Scaled: " + (scaled ? "true" : "false") + "\n";
    }

    /**@todo Add Affixes and Aura Spell methods */
};

class MpDataStore {
private:
    MpDataStore()
    : _playerData(std::make_unique<std::unordered_map<ObjectGuid, MpPlayerData*>>()),
      _instanceData(std::make_unique<std::map<std::pair<uint32, uint32>, MpInstanceData>>()),
      _groupData(std::make_unique<std::unordered_map<ObjectGuid, MpGroupData>>()),
      _instanceCreatureData(std::make_unique<std::unordered_map<ObjectGuid, MpCreatureData>>()),
      _scaleFactors(std::make_unique<std::map<std::pair<int32, int32>,MpScaleFactor>>())
      {
        _playerData->reserve(32);
        _groupData->reserve(32);
        _instanceCreatureData->reserve(500);
      };

    inline ~MpDataStore() {
        for (auto& [guid, playerData] : *_playerData) {
            delete playerData;
        }

         _playerData->clear();
    }

    std::unique_ptr<std::unordered_map<ObjectGuid, MpPlayerData*>> _playerData;

    // Instance data containes information about how to scale creatures
    std::unique_ptr<std::map<std::pair<uint32,uint32>,MpInstanceData>> _instanceData; // {mapId,instanceId}

    // Group data stored current group difficulty setting, and stats of group
    std::unique_ptr<std::unordered_map<ObjectGuid, MpGroupData>> _groupData;

    // This is all creatures that have been scaled used for determining what has been scaled
    std::unique_ptr<std::unordered_map<ObjectGuid, MpCreatureData>> _instanceCreatureData;

    // use to mimic pattern normals scale to heroic  (loaded at server start)
    std::unique_ptr<std::map<std::pair<int32,int32>,MpScaleFactor>> _scaleFactors; // {mapId,difficulty}

    // Single creature multipliers used to scale creatures individually that may need tuned up or down.
    // std::unique_ptr<std::unordered_map<uint32, CreatureOverride>> _creatureOverrides;

public:

    // ensure we only ever have one instance of this class
    MpDataStore(const MpDataStore&) = delete;
    MpDataStore& operator=(const MpDataStore&) = delete;

    MpPlayerData* GetPlayerData(ObjectGuid guid) {
        try {
            return _playerData->at(guid);
        } catch (const std::out_of_range& oor) {
            return nullptr;
        }
    }

    MpGroupData* GetGroupData(ObjectGuid guid) {

        if (_groupData->contains(guid)) {
            return &_groupData->at(guid);
        } else {
            return nullptr;
        }

    }
    MpGroupData* GetGroupData(Player *player) {
        return GetGroupData(player->GetGroup()->GetGUID());
    };

    // Set and remove settigs for a group options (difficulty, deaths, stats, etc)
    void AddGroupData(Group *group, MpGroupData groupData);
    void RemoveGroupData(Group *group);
    MpGroupData* GetGroupData(Group *group);
    void PushGroupInstanceKey(Group *group, uint32 mapId, uint32 instanceId);

    // Data related to player settings and stags
    void AddPlayerData(ObjectGuid guid, MpPlayerData* pd);
    void RemovePlayerData(ObjectGuid guid);
    void ResetPlayerData(ObjectGuid guid);

    // Each Map/Instance is a unique key that contains scaling information based on difficulty
    void AddInstanceData(uint32 mapId, uint32 instanceId, MpInstanceData );
    MpInstanceData* GetInstanceData(uint32 mapId, uint32 instanceId);
    void RemoveInstanceData(uint32 mapId, uint32 instanceId);

    // Methods for interacting with the map of creatures in a mythic instances
    void AddCreatureData(ObjectGuid guid, MpCreatureData creatureData);
    MpCreatureData* GetCreatureData(ObjectGuid guid);
    void RemoveCreatureData(ObjectGuid guid);
    std::vector<MpCreatureData*> GetInstanceCreatures(uint32 mapId, uint32 instanceId);
    std::vector<MpCreatureData*> GetUnscaledCreatures(uint32 mapId, uint32 instanceId);

    // Scale factors are used to determine a base bonus for enemies base on the instance difficulty
    int32 GetHealthScaleFactor(int32 mapId, int32 difficulty) const;
    int32 GetDamageScaleFactor(int32 mapId, int32 difficulty) const;
    int32 GetMaxDamageScaleFactor(int32 mapId, int32 difficulty) const;
    int32 GetSpellScaleFactor(int32 mapId, int32 difficulty) const;
    MpScaleFactor GetScaleFactor(int32 mapId, int32 difficulty) const;

    void SetDamageScaleFactor(int32 mapId, int32 difficulty, int32 value);
    void SetHealthScaleFactor(int32 mapId, int32 difficulty, int32 value);
    void SetSpellScaleFactor(int32 mapId, int32 difficulty, int32 value);

    // Individual Creature Scaling Multipliers
    // void AddCreatureOverride(uint32 entry, CreatureOverride* override);
    // MpMultipliers* GetCreatureOverride(uint32 entry);

    auto GetInstanceDataKey(uint32 mapId, uint32 instanceId) const {
        return std::make_pair(mapId, instanceId);
    }
    auto GetScaleFactorKey(int32 mapId, int32 difficulty) const {
        return std::make_pair(mapId, difficulty);
    }

    // Used at initial server load
    int32 LoadScaleFactors();

    // Database API calls
    void DBUpdatePlayerInstanceData(ObjectGuid playerGuid, MpDifficulty difficulty, uint32 mapId = 0, uint32 instanceId = 0, uint32 deaths = 0);

    void DBResetPlayerDeaths(Player* player);
    void DBAddPlayerDeath(Player* player, Creature* killer, MpDifficulty difficulty);
    void DBAddPlayerDeath(Player* player);

    void DBRemovePlayerData(ObjectGuid playerGuid);
    void DBRemovePlayerInstanceData(uint32 instanceId);
    void DBRemoveGroupInstanceData(uint32 instanceId);
    void DBUpdateGroupData(ObjectGuid groupGuid, MpDifficulty difficulty, uint32 mapId, uint32 instanceId, uint32 deaths);
    void DBUpdateGroupTimerDeaths(ObjectGuid groupGuid, uint32 mapId, uint32 instanceId, uint32 timer, uint32 deaths);
    void DBRemoveGroupData(ObjectGuid groupGuid);
    void DBAddGroupDeath(Group* group, uint32 mapId, uint32 instanceId, MpDifficulty difficulty);

    //SavePlayerDungeonStats(Group* group, MpGroupData const* groupData);

    static MpDataStore* instance() {
        static MpDataStore instance;
        return &instance;
    }
};

#define sMpDataStore MpDataStore::instance()

#endif // MpDataStore_DATASTORE_H
