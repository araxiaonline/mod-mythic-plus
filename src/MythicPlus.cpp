#include "MythicPlus.h"
#include "MapMgr.h"

MythicPlus::MythicPlus()
{

}

MythicPlus::~MythicPlus()
{

}

bool MythicPlus::IsMapEligible(Map* map)
{
    if (!Enabled) {
        return false;
    }

    if (map->IsDungeon()) {
        return true;
    }

    return false;
}
