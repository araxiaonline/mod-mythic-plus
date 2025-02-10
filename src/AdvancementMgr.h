#ifndef ADVANCEMENT_MGR_H
#define ADVANCEMENT_MGR_H

#include "SharedDefines.h"
#include "Player.h"

#include <memory>
#include <unordered_map>
#include <map>
#include <mutex>

enum MpAdvancements
{
    MP_ADV_INTELLECT        = 0,
    MP_ADV_SPIRIT           = 1,
    MP_ADV_STRENGTH         = 2,
    MP_ADV_AGILITY          = 3,
    MP_ADV_STAMINA          = 4,
    MP_ADV_RESIST_ARCANE    = 5,
    MP_ADV_RESIST_FIRE      = 6,
    MP_ADV_RESIST_NATURE    = 7,
    MP_ADV_RESIST_FROST     = 8,
    MP_ADV_RESIST_SHADOW    = 9,
    MP_ADV_MAX              = 10
};

std::string MpAdvancementsToString(MpAdvancements advancement);

/**
 * Advancement Rank represents each level for a stat increase that has can be purchases.
 * It includes materials, type, and range of successful roll.
 */
struct MpAdvancementRank
{
    uint32 rank;
    MpAdvancements advancementId;
    std::unordered_map<uint32 /*item_entry*/,uint32 /*quantity*/> materialCost;

    std::array<int, 3> rollCost; // 0 = low, 1 = mid, 2 = high

    // Range of status based on bet dice roll.
    std::pair<uint32 /*min*/, uint32 /*max*/> lowRange;
    std::pair<uint32 /*min*/, uint32 /*max*/> midRange;
    std::pair<uint32 /*min*/, uint32 /*max*/> highRange;

    // materialId levels
    std::pair<uint32 /*materialId*/, uint32 /*quantity*/> material1;
    std::pair<uint32 /*materialId*/, uint32 /*quantity*/> material2;
    std::pair<uint32 /*materialId*/, uint32 /*quantity*/> material3;

    // Used to validate this struct is set correctly
    bool IsValid() {
        return (rank > 0 && advancementId >= 0 && advancementId < MP_ADV_MAX);
    }

    // Check if the map has an the item entry for the passed in material
    bool HasMaterial(uint32 itemEntry) {
        return materialCost.contains(itemEntry);
    }
};

// Struct is used for tracking player advancement bonuses for improving stats
struct MpPlayerRank
{
    uint32 rank;
    MpAdvancements advancementId;
    uint32 diceSpent;
    float bonus;

    MpPlayerRank() : rank(0), diceSpent(0), bonus(0.0f) {}
};

/**
 * This singleton class is responsible for managing the advancement system
 * used to improve player stats enough to challenge harder difficulties on existing dungeons.
 *
 * Advancements are purchased by players using based on the mp_material_type table with a "bet"
 * on dice roll.  This enables players to increase their stats in a random way that is not
 * guaranteed to be successful. (Similar to how DND stats rolls work on character creation. )
 */
class AdvancementMgr
{

// Shared mutex for handling writes to shared player advancement data
std::mutex _playerAdvancementMutex;

// All advancement levels for each stat [rank][advancement_id] = AdvancementLevel
std::map<std::pair<uint32 /*rank*/, MpAdvancements>, MpAdvancementRank> _advancementRanks;

// Map of player advancements [player_guid][advancement_id] = rank
std::unordered_map<uint32 /*player_guid*/, std::unordered_map<MpAdvancements, MpPlayerRank>> _playerAdvancements;

// Map of different material types used fo advancing stats for players
std::unordered_map<uint32 /*material_id*/, std::vector<uint32> /* item entries */> _materialTypes;

public:
    static AdvancementMgr* instance() {
        static AdvancementMgr instance;
        return &instance;
    }

    // Loads advancement information from the database into memory when players are logged in or server starts.
    int32 LoadAdvancementRanks();
    int32 LoadMaterialTypes();
    void LoadPlayerAdvancements(Player* player);

    // Methods for looking up advancement rank data
    MpAdvancementRank* GetAdvancementRank(uint32 rank, MpAdvancements advancement);

    // Methods for updating and setting data related to current player advancements
    MpPlayerRank* GetPlayerAdvancementRank(Player* player, MpAdvancements advancement);

    /**
     * This upgrades a player Advancement on the server side, which will handle the following actions:
     * 1. Validating player has enough dice and materials to upgrade the advancement
     * 2. Rolling the dice to see what bonus is rewarded
     * 3. Removing the dice and materials from the player inventory
     * 4. Updating the player advancement rank in memory and database
     *
     * Since different materials can be used for each advancement, at the moment only support one material type from the list. supporting
     * mixed materials is more complicated and the UI to support it is much more complex, while this is not as nice it is much simpler to implement.
     * That means all materials have to be selected and passed in at the time of making this call.
     */
    uint32 UpgradeAdvancement(Player* player, MpAdvancements advancement, uint32 diceCostLevel, uint32 itemEntry1, uint32 itemEntry2, uint32 itemEntry3);

    // Used to reset all advancements for a specific player
    bool ResetPlayerAdvancements(Player* player);

private:
    AdvancementMgr() {}
    ~AdvancementMgr() {}

    // Will reset all the player advancements and refund the spent dice and material with a penalty for the reset.
    void _ResetPlayerAdvancement(Player* player, MpAdvancements advancement);

    // Rolls the dice to see how much a bonus is given based on the dice spend level
    float _RollAdvancement(MpAdvancementRank* advancementRank, uint32 diceCostLevel);

    // Determines if a player has required items to upgrade
    bool _PlayerHasItems(Player* player, MpAdvancementRank* advancementRank, uint32 diceCostLevel, uint32 itemEntry1, uint32 itemEntry2, uint32 itemEntry3);

    // Removes items from player inventory based on the required advancement rank.
    void _ChargeItemCost(Player* player, MpAdvancementRank* advancementRank, uint32 diceCostLevel, uint32 itemEntry1, uint32 itemEntry2, uint32 itemEntry3);

    // This will save the advancement for the player advancement in memory and database
    void _SaveAdvancement(Player* player, MpAdvancementRank* advancementRank, MpPlayerRank* playerRank, uint32 diceCost, float roll, uint32 itemEntry1, uint32 itemEntry2 = 0, uint32 itemEntry3 = 0);

    // This will save the advancement purchase to the history database
    void _DBSaveAdvancement(Player* player, MpAdvancementRank* advancementRank, MpPlayerRank* playerRank, uint32 diceCost, float roll);

};

#define sAdvancementMgr AdvancementMgr::instance()
#endif
