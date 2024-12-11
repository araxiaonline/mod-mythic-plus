#ifndef ADVANCEMENT_MGR_H
#define ADVANCEMENT_MGR_H

#include "SharedDefines.h"
#include "Player.h"

#include <memory>
#include <unordered_map>
#include <map>

enum MpAdvancements {
    MP_ADV_STRENGTH         = 0,
    MP_ADV_AGILITY          = 1,
    MP_ADV_STAMINA          = 2,
    MP_ADV_INTELLECT        = 3,
    MP_ADV_SPIRIT           = 4,
    MP_ADV_RESIST_FIRE      = 5,
    MP_ADV_RESIST_NATURE    = 6,
    MP_ADV_RESIST_FROST     = 7,
    MP_ADV_RESIST_SHADOW    = 8,
    MP_ADV_RESIST_ARCANE    = 9
};

/**
 * Advancement Rank represents each level for a stat increase that has can be purchases.
 * It includes materials, type, and range of successful roll.
 */
struct MpAdvancementRank
{
    uint32 rank;
    MpAdvancements advancementId;
    std::unordered_map<uint32 /*item_entry*/,uint32 /*quantity*/> materialCost;

    std::tuple<uint32 /*low-cost*/, uint32 /*mid-cost*/, uint32 /*high-cost*/> rollCost;

    // Range of status based on bet dice roll.
    std::pair<uint32 /*min*/, uint32 /*max*/> lowRange;
    std::pair<uint32 /*min*/, uint32 /*max*/> midRange;
    std::pair<uint32 /*min*/, uint32 /*max*/> highRange;
};

class AdvancementMgr
{

// All advancement levels for each stat [rank][advancement_id] = AdvancementLevel
std::map<std::pair<uint32 /*rank*/, MpAdvancements>, MpAdvancementRank> _advancementRanks;

// Map of player advancements [player_guid][advancement_id] = rank
std::unordered_map<uint32 /*player_guid*/, std::unordered_map<MpAdvancements, uint32 /*rank*/>> _playerAdvancements;

public:
    static AdvancementMgr* instance() {
        static AdvancementMgr instance;
        return &instance;
    }

    int32 LoadAdvencementRanks();
    int32 LoadPlayerAdvancements(Player* player);

private:
    AdvancementMgr() {}
    ~AdvancementMgr() {}

};


#define sAdvancementMgr AdvancementMgr::instance()
#endif
