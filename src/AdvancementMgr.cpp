#include "AdvancementMgr.h"
#include "CharacterDatabase.h"
#include "WorldDatabase.h"
#include "Player.h"
#include "MpLogger.h"
#include "MythicPlus.h"
#include "MpConstants.h"

#include <string_view>
#include <tuple>

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
int32 AdvancementMgr::LoadAdvencementRanks() {
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
            .highRange = std::make_pair(minIncrease3, maxIncrease3)
        };

        rank.materialCost.emplace(materialId1, materialCost1);
        rank.materialCost.emplace(materialId2, materialCost2);
        rank.materialCost.emplace(materialId3, materialCost3);

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
int32 AdvancementMgr::LoadPlayerAdvancements(Player* player) {

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

    // If the player does not have any upgrades just return not a problem until they purchase one.
    if(!result) {
        return 0;
    }

    do {
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

    } while (result->NextRow());

    return result->GetRowCount();
}

/**
 * Load Material Types from the database into memory
 */
int32 AdvancementMgr::LoadPlayerAdvancements(Player* player) {

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

    // If the player does not have any upgrades just return not a problem until they purchase one.
    if(!result) {
        return 0;
    }

    do {
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
        MpLogger::error("Advancment Id {} and rank {} could not be found", rank, advancement);
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

bool AdvancementMgr::UpgradeAdvancement(Player* player, MpAdvancements advancement, uint32 diceCostLevel, uint32 itemEntry1, uint32 itemEntry2, uint32 itemEntry3)
{
    // Validators to make sure inputs are correct to perform the upgrade
    if(!player) {
        MpLogger::error("Could not upgrade advancement for player, player was nullpointer");
        return false;
    }
    if(diceCostLevel < 1 || diceCostLevel > 3) {
        MpLogger::error("Invalid dice cost level valid vales (1,2,3) received {} for player {}", diceCostLevel, player->GetName());
        return false;
    }
    if(itemEntry1 == 0) {
        MpLogger::error("Material1 can not be 0 can not perform advancement upgrade for player {} Advancment {}", player->GetName(), advancement);
        return false;
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
        MpLogger::error("Player {} has reached the maximum rank for advancement {}", player->GetName(), advancement);
        return false;
    }

    uint32 newRank = playerRank->rank + 1;
    MpAdvancementRank* advancementRank = GetAdvancementRank(newRank, advancement);
    if(!advancementRank->IsValid()) {
        MpLogger::error("Advancement {} rank {} could not be found", advancement, newRank);
        return false;
    }

    // If the player has the items needed to upgrade this advancement, then remove the items from the player inventory and apply the upgrade
    if(!_PlayerHasItems(player, advancementRank, diceCostLevel, itemEntry1, itemEntry2, itemEntry3)) {
        MpLogger::info("Player {} does not have the required items to upgrade advancement {}", player->GetName(), advancement);
        return false;
    }

    // Charge the player the cost of the upgrade
    _ChargeItemCost(player, advancementRank, diceCostLevel, itemEntry1, itemEntry2, itemEntry3);

    // Finally get the bonus to apply for the player
    float roll = _RollAdvancement(advancementRank, diceCostLevel);

    // Update the player advancement rank in memory and database
    playerRank->rank = newRank;
    playerRank->diceSpent += advancementRank->rollCost[diceCostLevel];
    playerRank->bonus += roll;

    return true;
}

bool AdvancementMgr::ResetPlayerAdvancements(Player* player)
{
    return true;
}

/******************
 *
 * Private Methods
 *
 ******************/

void AdvancementMgr::_ResetPlayerAdvancement(Player* player, MpAdvancements advancement)
{

}

// Roll them stats DnD style.
float AdvancementMgr::_RollAdvancement(MpAdvancementRank* advancementRank, uint32 diceCostLevel)
{
    uint32 min, max;

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

    return frand(min, max);
}

// Checks the players inventory to validate they have the required items to perform an upgrade based on the set cost for the passed in level.
bool AdvancementMgr::_PlayerHasItems(Player* player, MpAdvancementRank* advancementRank, uint32 diceCostLevel, uint32 itemEntry1, uint32 itemEntry2, uint32 itemEntry3)
{
    // Check if player has the required dice to upgrade the advancement if not do nothing.
    uint32 diceCost = advancementRank->materialCost.at(diceCostLevel);
    if(!player->HasItemCount(MpConstants::ANCIENT_DICE, diceCost)) {
        MpLogger::info("Player {} does not have enough dice to upgrade advancement {}", player->GetName(), advancementRank->advancementId);
        return false;
    }

    // Validate the passed in item for materialId 1 is valid and the player has enough to purchase.
    if(itemEntry1 > 0) {
        if(!advancementRank->HasMaterial(itemEntry1)) {
            MpLogger::error("Material1 {} is not a valid material for advancement {}", itemEntry1, advancementRank->advancementId);
            return false;
        }

        if(!player->HasItemCount(itemEntry1, advancementRank->materialCost[itemEntry1])) {
            MpLogger::info("Player {} does not have enough material {} to upgrade advancement {}", player->GetName(), itemEntry1, advancementRank->advancementId);
            return false;
        }
    }

    if(itemEntry2 > 0) {
        if(!advancementRank->HasMaterial(itemEntry2)) {
            MpLogger::error("Material1 {} is not a valid material for advancement {}", itemEntry2, advancementRank->advancementId);
            return false;
        }

        if(!player->HasItemCount(itemEntry2, advancementRank->materialCost[itemEntry2])) {
            MpLogger::info("Player {} does not have enough material {} to upgrade advancement {}", player->GetName(), itemEntry2, advancementRank->advancementId);
            return false;
        }
    }

    if(itemEntry3 > 0) {
        if(!advancementRank->HasMaterial(itemEntry3)) {
            MpLogger::error("Material1 {} is not a valid material for advancement {}", itemEntry3, advancementRank->advancementId);
            return false;
        }

        if(!player->HasItemCount(itemEntry3, advancementRank->materialCost[itemEntry3])) {
            MpLogger::info("Player {} does not have enough material {} to upgrade advancement {}", player->GetName(), itemEntry3, advancementRank->advancementId);
            return false;
        }
    }

    return true;
}

// Remove all items required for the upgrade.
void AdvancementMgr::_ChargeItemCost(Player *player, MpAdvancementRank* advancementRank, uint32 diceCostLevel, uint32 itemEntry1, uint32 itemEntry2, uint32 itemEntry3)
{
    uint32 diceCost = advancementRank->materialCost[diceCostLevel];
    Item* item = player->GetItemByEntry(MpConstants::ANCIENT_DICE);
    item->SetCount(item->GetCount() - diceCost);
    item->SendUpdateToPlayer(player);

    // Remove the material from the player inventory
    if(itemEntry1 > 0) {
        item = player->GetItemByEntry(itemEntry1);
        item->SetCount(item->GetCount() - advancementRank->materialCost[itemEntry1]);
        item->SendUpdateToPlayer(player); // Update the client with the new dice count
    }

    if(itemEntry2 > 0) {
        item = player->GetItemByEntry(itemEntry2);
        item->SetCount(item->GetCount() - advancementRank->materialCost[itemEntry2]);
        item->SendUpdateToPlayer(player); // Update the client with the new dice count
    }

    if(itemEntry3 > 0) {
        item = player->GetItemByEntry(itemEntry3);
        item->SetCount(item->GetCount() - advancementRank->materialCost[itemEntry3]);
        item->SendUpdateToPlayer(player); // Update the client with the new dice count
    }

    return;
}



