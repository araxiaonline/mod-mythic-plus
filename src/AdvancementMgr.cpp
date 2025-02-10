#include "AdvancementMgr.h"
#include "CharacterDatabase.h"
#include "WorldDatabase.h"
#include "Player.h"
#include "MpLogger.h"
#include "MythicPlus.h"
#include "MpConstants.h"

#include <string_view>
#include <tuple>
#include <mutex>
#include <exception>

std::string MpAdvancementsToString(MpAdvancements advancement)
{
    switch (advancement)
    {
        case MP_ADV_INTELLECT: return "Intellect";
        case MP_ADV_SPIRIT: return "Spirit";
        case MP_ADV_STRENGTH: return "Strength";
        case MP_ADV_AGILITY: return "Agility";
        case MP_ADV_STAMINA: return "Stamina";
        case MP_ADV_RESIST_ARCANE: return "Arcane Resistance";
        case MP_ADV_RESIST_FIRE: return "Fire Resistance";
        case MP_ADV_RESIST_NATURE: return "Nature Resistance";
        case MP_ADV_RESIST_FROST: return "Frost Resistance";
        case MP_ADV_RESIST_SHADOW: return "Shadow Resistance";
        default: return "Unknown Advancement";
    }
}

/**
 * Table schema for upgrade ranks populated by go script:
 *
 *   upgradeRank INT UNSIGNED NOT NULL,
 *   advancementId  INT UNSIGNED NOT NULL,
 *   materialId1  INT UNSIGNED NOT NULL,
 *   materialId2  INT UNSIGNED NOT NULL,
 *   materialId3  INT UNSIGNED NOT NULL,
 *   materialCost INT UNSIGNED NOT NULL,
 *   materialCost2 INT UNSIGNED NOT NULL,
 *   materialCost3 INT UNSIGNED NOT NULL,
 *   minIncrease1 INT UNSIGNED NOT NULL,
 *   maxIncrease1 INT UNSIGNED NOT NULL,
 *   minIncrease2 INT UNSIGNED NOT NULL,
 *   maxIncrease2 INT UNSIGNED NOT NULL,
 *   minIncrease3 INT UNSIGNED NOT NULL,
 *   maxIncrease3 INT UNSIGNED NOT NULL,
 *   chanceCost1 INT UNSIGNED NOT NULL,
 *   chanceCost2 INT UNSIGNED NOT NULL,
 *   chanceCost3 INT UNSIGNED NOT NULL,
 *
 *   PRIMARY KEY (upgradeRank, statTypeId)
 *
 *   This loads the ranks from the database and stores them into memory for access.  This should only be
 *   called on server start-up as it is static data that should only change no new builds.
 */
int32 AdvancementMgr::LoadAdvancementRanks() {
    _advancementRanks.clear();

    //
    // const char*
    constexpr std::string_view query = R"(
        SELECT
            upgradeRank,
            advancementId,
            materialId1,
            materialId2,
            materialId3,
            materialCost1,
            materialCost2,
            materialCost3,
            minIncrease1,
            maxIncrease1,
            minIncrease2,
            maxIncrease2,
            minIncrease3,
            maxIncrease3,
            chanceCost1,
            chanceCost2,
            chanceCost3
        FROM mp_upgrade_ranks
    )";

    QueryResult result = WorldDatabase.Query(query);
    if (!result) {
        MpLogger::error("Failed to load mythic scale factors from database");
        return 0;
    }

    // Load all the material types into memory for reference



    do {
        Field* fields = result->Fetch();
        uint32 upgradeRank = fields[0].Get<uint32>();
        uint32 advancementId = fields[1].Get<uint32>();
        uint32 materialId1 = fields[2].Get<uint32>();
        uint32 materialId2 = fields[3].Get<uint32>();
        uint32 materialId3 = fields[4].Get<uint32>();
        uint32 materialCost1 = fields[5].Get<uint32>();
        uint32 materialCost2 = fields[6].Get<uint32>();
        uint32 materialCost3 = fields[7].Get<uint32>();
        uint32 minIncrease1 = fields[8].Get<uint32>();
        uint32 maxIncrease1 = fields[9].Get<uint32>();
        uint32 minIncrease2 = fields[10].Get<uint32>();
        uint32 maxIncrease2 = fields[11].Get<uint32>();
        uint32 minIncrease3 = fields[12].Get<uint32>();
        uint32 maxIncrease3 = fields[13].Get<uint32>();
        uint32 chanceCost1 = fields[14].Get<uint32>();
        uint32 chanceCost2 = fields[15].Get<uint32>();
        uint32 chanceCost3 = fields[16].Get<uint32>();

        // Should add validator... but let's do it without and trust in the o-DB-Wan-kenobe
        MpAdvancements advancement = static_cast<MpAdvancements>(advancementId);

        // List of all ranks keyed by rank, advancementId
        MpAdvancementRank rank = {
            .rank = upgradeRank,
            .advancementId = advancement,
            .materialCost = std::unordered_map<uint32, uint32>(),
            .rollCost = {chanceCost1, chanceCost2, chanceCost3},
            .lowRange = std::make_pair(minIncrease1, maxIncrease1),
            .midRange = std::make_pair(minIncrease2, maxIncrease2),
            .highRange = std::make_pair(minIncrease3, maxIncrease3),
            .material1 = std::make_pair(materialId1, materialCost1),
            .material2 = std::make_pair(materialId2, materialCost2),
            .material3 = std::make_pair(materialId3, materialCost3)
        };

        _advancementRanks.try_emplace(std::make_pair(upgradeRank, advancement), rank);

    } while (result->NextRow());

    return _advancementRanks.size();
}

/**
 *   guid          INT UNSIGNED NOT NULL,
 *   advancementId INT UNSIGNED NOT NULL,
 *   bonus         FLOAT        NOT NULL,
 *   upgradeRank   INT UNSIGNED NOT NULL,
 *   diceSpent     INT UNSIGNED NOT NULL DEFAULT '0',
 *
 *   PRIMARY KEY (guid, advancementId)
 *
 *   This loads the player advancements when a player logs in stores it into memory for access by spell scripts that
 *   are applied to the player on login to apply the bonuses.
 */
void AdvancementMgr::LoadPlayerAdvancements(Player* player) {

    std::lock_guard<std::mutex> lock(_playerAdvancementMutex);

    constexpr std::string_view query = R"(
    SELECT
        guid,
        advancementId,
        bonus,
        upgradeRank,
        diceSpent
    FROM mp_player_advancements
    WHERE guid = {}
    )";

    QueryResult result = CharacterDatabase.Query(query, player->GetGUID().GetCounter());

    // If the player does not have any upgrades just return perfectly fine not a problem until they purchase one.

    if(!result) {
        MpLogger::info("Player {} has no advancements", player->GetName());
        return;
    }

    Field* fields = result->Fetch();
    uint32 guid = fields[0].Get<uint32>();
    uint32 advancementId = fields[1].Get<uint32>();
    float bonus = fields[2].Get<float>();
    uint32 upgradeRank = fields[3].Get<uint32>();
    uint32 diceSpent = fields[4].Get<uint32>();

    MpAdvancements advancement = static_cast<MpAdvancements>(advancementId);
    MpPlayerRank playerRank = MpPlayerRank();
    playerRank.rank = upgradeRank;
    playerRank.advancementId = advancement;
    playerRank.diceSpent = diceSpent;
    playerRank.bonus = bonus;

    // List of all ranks keyed by rank, advancementId
    _playerAdvancements[guid][advancement] = playerRank;

    MpLogger::info("Loaded player {} advancement {} rank {}", player->GetName(), playerRank.advancementId, upgradeRank);
}

/**
 * Load Material Types from the database into memory
 */
int32 AdvancementMgr::LoadMaterialTypes() {

    constexpr std::string_view query = R"(
    SELECT
        materialId,
        entry
    FROM mp_material_types
    )";

    QueryResult result = WorldDatabase.Query(query);

    do {
        Field* fields = result->Fetch();
        uint32 materialId = fields[0].Get<uint32>();
        uint32 entry = fields[1].Get<uint32>();

        if(!_materialTypes.contains(materialId)) {
            _materialTypes.emplace(materialId,std::vector<uint32>());
        }
        _materialTypes.at(materialId).push_back(entry);

    } while (result->NextRow());

    return result->GetRowCount();
}

MpAdvancementRank* AdvancementMgr::GetAdvancementRank(uint32 rank, MpAdvancements advancement)
{
    auto key = std::make_pair(rank, advancement);
    if (_advancementRanks.contains(key))
    {
        return &_advancementRanks.at(key);
    }
    else
    {
        MpLogger::error("Advancement Id {} and rank {} could not be found", rank, advancement);
        return nullptr;
    }
}

MpPlayerRank* AdvancementMgr::GetPlayerAdvancementRank(Player* player, MpAdvancements advancement)
{
    if(!player) {
        MpLogger::error("Could not retrieve player advancement for null player {}", player->GetName());
        return nullptr;
    }

    if (_playerAdvancements.contains(player->GetGUID().GetCounter()) && _playerAdvancements[player->GetGUID().GetCounter()].contains(advancement))
    {
        return &_playerAdvancements[player->GetGUID().GetCounter()][advancement];
    }

    return nullptr;
}

uint32 AdvancementMgr::UpgradeAdvancement(Player* player, MpAdvancements advancement, uint32 diceCostLevel, uint32 itemEntry1, uint32 itemEntry2, uint32 itemEntry3)
{
    std::lock_guard<std::mutex> lock(_playerAdvancementMutex);

    // Validators to make sure inputs are correct to perform the upgrade
    if(!player) {
        MpLogger::error("Could not upgrade advancement for player, player was nullpointer");
        throw new std::runtime_error("Could not upgrade advancement for player, player was nullpointer");
    }
    if(diceCostLevel < 1 || diceCostLevel > 3) {
        throw new std::runtime_error(Acore::StringFormat("Invalid dice cost level valid vales (1,2,3) received {} for player {}", diceCostLevel, player->GetName()));
    }
    if(itemEntry1 == 0) {
       throw new std::runtime_error(Acore::StringFormat("Material1 can not be 0 can not perform advancement upgrade for player {} Advancement {}", player->GetName(), advancement));
    }

    MpPlayerRank* playerRank = GetPlayerAdvancementRank(player, advancement);

    // IF there is not create the base struct and add to the player map for this advancement
    if(!playerRank) {
        MpPlayerRank newPlayerRank;
        newPlayerRank.advancementId = advancement;

        auto& playerAdvMap = _playerAdvancements[player->GetGUID().GetCounter()];
        playerAdvMap.emplace(advancement, newPlayerRank);
        playerRank = &playerAdvMap.at(advancement);
    }

    if(playerRank->rank == MP_MAX_ADVANCEMENT_RANK) {
        MpLogger::debug("Player {} has reached the maximum rank for advancement {}", player->GetName(), advancement);
        return 0;
    }

    uint32 newRank = playerRank->rank + 1;
    MpAdvancementRank* advancementRank = GetAdvancementRank(newRank, advancement);
    if(!advancementRank->IsValid()) {
        throw std::runtime_error("Advancement rank could not be found. Rank: " + std::to_string(newRank) + " Advancement: " + std::to_string(advancement));
    }

    // If the player has the items needed to upgrade this advancement, then remove the items from the player inventory and apply the upgrade
    if(!_PlayerHasItems(player, advancementRank, diceCostLevel, itemEntry1, itemEntry2, itemEntry3)) {
        MpLogger::debug("Player {} does not have the required items to upgrade advancement {}", player->GetName(), advancement);
        return 0;
    }

    // Charge the player the cost of the upgrade
    _ChargeItemCost(player, advancementRank, diceCostLevel, itemEntry1, itemEntry2, itemEntry3);

    MpLogger::debug("Player {} has upgraded advancement {} to rank {}", player->GetName(), advancement, newRank);

    // Finally get the bonus to apply for the player
    float roll = round(_RollAdvancement(advancementRank, diceCostLevel));

    // Update the player advancement rank in memory and database
    playerRank->rank = newRank;
    playerRank->diceSpent += advancementRank->rollCost[diceCostLevel-1];

    // round the roll to a whole number and add to player rank
    playerRank->bonus += roll;

    MpLogger::debug("Player {} has upgraded advancement {} to rank {} with bonus {} to new total bonus of {}", player->GetName(), advancement, newRank, roll, playerRank->bonus);

    // Save the advancement to the database
    _SaveAdvancement(player, advancementRank, playerRank, advancementRank->rollCost[diceCostLevel-1], roll, itemEntry1, itemEntry2, itemEntry3);

    return roll;
}

bool AdvancementMgr::ResetPlayerAdvancements(Player* /*player*/)
{
    std::lock_guard<std::mutex> lock(_playerAdvancementMutex);

    return true;
}

void AdvancementMgr::_ResetPlayerAdvancement(Player* /*player*/, MpAdvancements /*advancement*/)
{
    std::lock_guard<std::mutex> lock(_playerAdvancementMutex);
    return;
}

// Roll them stats DnD style.
float AdvancementMgr::_RollAdvancement(MpAdvancementRank* advancementRank, uint32 diceCostLevel)
{
    uint32 min, max;
    min = max = 0;

    switch (diceCostLevel)
    {
    case 1:
        min = advancementRank->lowRange.first;
        max = advancementRank->lowRange.second;
        break;
    case 2:
        min = advancementRank->midRange.first;
        max = advancementRank->midRange.second;
        break;
    case 3:
        min = advancementRank->highRange.first;
        max = advancementRank->highRange.second;
        break;
    default:
        MpLogger::error("Invalid dice cost level valid vales (1,2,3) received {} for rank roll", diceCostLevel, advancementRank->rank);
        break;
    }

    MpLogger::debug("Rolling for rank {} dice level {} min {} max {}", advancementRank->rank, diceCostLevel, min, max);

    return frand(min, max);
}

/**
 * Checks the players inventory to validate they have the required items to perform an upgrade based on the set cost for the passed in level.
 * @todo refactor this to be more generic
 */
bool AdvancementMgr::_PlayerHasItems(Player* player, MpAdvancementRank* advancementRank, uint32 diceCostLevel, uint32 itemEntry1, uint32 itemEntry2, uint32 itemEntry3)
{
    MpLogger::debug("Debugging player dice {} item1 {} item2 {} item3 {}", diceCostLevel, itemEntry1, itemEntry2, itemEntry3);
    MpLogger::debug("Advancement Rank dice info {} {} {} {}", advancementRank->materialCost[1], advancementRank->materialCost[2], advancementRank->materialCost[3]);

    // Check if player has the required dice to upgrade the advancement if not do nothing.
    uint32 diceCost = advancementRank->rollCost[diceCostLevel-1];
    if(!player->HasItemCount(MpConstants::ANCIENT_DICE, diceCost)) {
        MpLogger::info("Player {} does not have enough dice to upgrade advancement {}", player->GetName(), advancementRank->advancementId);
        return false;
    }

    // Create arrays of material data for easier iteration
    std::pair<uint32, uint32> materials[] = {
        advancementRank->material1,
        advancementRank->material2,
        advancementRank->material3
    };

    uint32 itemEntries[] = {itemEntry1, itemEntry2, itemEntry3};

    // Validate each material
    for (size_t i = 0; i < 3; ++i) {
        uint32 materialId = materials[i].first;
        uint32 requiredCount = materials[i].second;
        uint32 itemEntry = itemEntries[i];

        // if the materialID is not set and is not the first material then return true as no material is required
        if (materialId == 0 && i != 0) {
            return true;
        }

        if (itemEntry == 0 && materialId != 0) {
            throw std::runtime_error("The entry for materialId: " + std::to_string(materialId) + " was not passed in.");
        }

        std::vector<uint32> entries = _materialTypes.at(materialId);
        if (entries.empty()) {
            throw std::runtime_error("MaterialId: " + std::to_string(materialId) + " could not be found in the materials list");
        }

        if (std::find(entries.begin(), entries.end(), itemEntry) == entries.end()) {
            throw std::runtime_error("Incorrect material entry" + std::to_string(itemEntry) + " passed in passed in for Advancement id:" + std::to_string(advancementRank->advancementId));
        }

        if (!player->HasItemCount(itemEntry, requiredCount)) {
            MpLogger::info("Player {} does not have enough material {} to upgrade advancement {} requires: {}",
                player->GetName(), itemEntry, advancementRank->advancementId, requiredCount);
            return false;
        }
    }

    return true;
}

// Remove all items required for the upgrade.
void AdvancementMgr::_ChargeItemCost(Player *player, MpAdvancementRank* advancementRank, uint32 diceCostLevel, uint32 itemEntry1, uint32 itemEntry2, uint32 itemEntry3)
{
    MpLogger::debug("Charging player dice {} item1 {} item2 {} item3 {}", diceCostLevel, itemEntry1, itemEntry2, itemEntry3);
    uint32 diceCost = advancementRank->rollCost[diceCostLevel-1];
    player->DestroyItemCount(MpConstants::ANCIENT_DICE, diceCost, true);

    std::vector<std::pair<uint32, uint32>> items = {
        {itemEntry1, advancementRank->material1.second},
        {itemEntry2, advancementRank->material2.second},
        {itemEntry3, advancementRank->material3.second}
    };

    for (const auto& item : items) {
        if (item.first > 0) {
            player->DestroyItemCount(item.first, item.second, true);
        }
    }

    return;
}

void AdvancementMgr::_SaveAdvancement(Player* player, MpAdvancementRank* advancementRank, MpPlayerRank* playerRank, uint32 diceCost, float roll, uint32 itemEntry1, uint32 itemEntry2, uint32 itemEntry3)
{
    // Save the advancement to the database
    try {
        constexpr std::string_view insert = R"(
            INSERT INTO mp_player_advancements (guid, advancementId, bonus, upgradeRank, diceSpent)
            VALUES ({}, {}, {}, {}, {})
            ON DUPLICATE KEY UPDATE
                bonus = {},
                upgradeRank = {},
                diceSpent = {}
        )";

        MpLogger::debug("Saving advancement for player {} bonus {} advancement {} rank {}", player->GetName(), playerRank->bonus, advancementRank->advancementId, playerRank->rank);
        CharacterDatabase.DirectExecute(insert,
                player->GetGUID().GetCounter(),
                advancementRank->advancementId,
                playerRank->bonus,
                playerRank->rank,
                playerRank->diceSpent,
                playerRank->bonus,
                playerRank->rank,
                playerRank->diceSpent
            );
    } catch (const std::exception& e) {
        MpLogger::error("Failed to save advancement for player {}: {}", player->GetName(), e.what());
    } catch (...) {
        MpLogger::error("Failed to save advancement for player {}: unknown error", player->GetName());
    }

    try {
        constexpr std::string_view insertHistory = R"(
            INSERT INTO mp_player_advancement_history (guid, advancementId, bonus, upgradeRank, diceSpent, entryId1, entryId2, entryId3, itemCost1, itemCost2, itemCost3)
            VALUES ({}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {})
        )";

        MpLogger::debug("Saving advancement history for player {} advancement {} rank {}", player->GetName(), advancementRank->advancementId, playerRank->rank);
        CharacterDatabase.Execute(insertHistory,
                player->GetGUID().GetCounter(),
                advancementRank->advancementId,
                roll,
                playerRank->rank,
                diceCost,
                itemEntry1,
                itemEntry2,
                itemEntry3,
                advancementRank->material1.second,
                advancementRank->material2.second,
                advancementRank->material3.second
            );
    } catch (const std::exception& e) {
        MpLogger::error("Failed to save advancement history for player {}: {}", player->GetName(), e.what());
    } catch (...) {
        MpLogger::error("Failed to save advancement history for player {}: unknown error", player->GetName());
    }
}

// void _DBSaveAdvancement(Player* player, MpAdvancementRank* advancementRank, MpPlayerRank* playerRank)
// {
//     // Save the advancement to the database
//     constexpr std::string_view query = R"(
//         INSERT INTO mp_player_advancements (guid, advancementId, bonus, upgradeRank, diceSpent)
//         VALUES ({}, {}, {}, {}, {})
//         ON DUPLICATE KEY UPDATE
//             bonus = {},
//             upgradeRank = {},
//             diceSpent = {}
//     )";

//     CharacterDatabase.Execute(query,
//         player->GetGUID().GetCounter(),
//         advancementRank->advancementId,
//         roll,
//         playerRank->rank,
//         playerRank->diceSpent,
//         roll,
//         playerRank->rank,
//         playerRank->diceSpent
//     );
// }
