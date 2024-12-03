#include "AdvancementMgr.h"
#include "CharacterDatabase.h"
#include "WorldDatabase.h"
#include "Player.h"
#include "MpLogger.h"

#include <string_view>

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
void AdvancementMgr::LoadAdvencementRanks() {
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
        return;
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

        // Should add validator... but let's do it without and trust the o-DB-wan-kenobee
        MpAdvancements advancement = static_cast<MpAdvancements>(advancementId);

        // List of all ranks keyed by rank, advancementId
        MpAdvancementRank rank = {
            .rank = upgradeRank,
            .advancementId = advancement,
            .materialCost = std::unordered_map<uint32, uint32>(),
            .rollCost = std::make_tuple(chanceCost1, chanceCost2, chanceCost3),
            .lowRange = std::make_pair(minIncrease1, maxIncrease1),
            .midRange = std::make_pair(minIncrease2, maxIncrease2),
            .highRange = std::make_pair(minIncrease3, maxIncrease3)
        };

        rank.materialCost.emplace(materialId1, materialCost1);
        rank.materialCost.emplace(materialId2, materialCost2);
        rank.materialCost.emplace(materialId3, materialCost3);

        _advancementRanks.try_emplace(std::make_pair(upgradeRank, advancement), rank);

    } while (result->NextRow());

    MpLogger::info("Loaded {} advancement ranks", _advancementRanks.size());
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

    constexpr std::string_view query = R"(
    SELECT
        guid,
        advancementId,
        bonus,
        upgradeRank,
        diceSpent
    FROM mp_player_advancements
    WHERE guid = ?
    )";

    QueryResult result = CharacterDatabase.Query(query, player->GetGUID().GetCounter());

    // If the player does not have any upgrades just return not a problem until they purchase one.
    if(!result) {
        return;
    }

    do {
        Field* fields = result->Fetch();
        uint32 guid = fields[0].Get<uint32>();
        uint32 advancementId = fields[1].Get<uint32>();
        float bonus = fields[2].Get<float>();
        uint32 upgradeRank = fields[3].Get<uint32>();
        uint32 diceSpent = fields[4].Get<uint32>();

        MpAdvancements advancement = static_cast<MpAdvancements>(advancementId);

        // List of all ranks keyed by rank, advancementId
        _playerAdvancements[guid][advancement] = upgradeRank;

    } while (result->NextRow());

    MpLogger::info("Loaded {} player advancements for player {}", _playerAdvancements.size(), player->GetName());
}
