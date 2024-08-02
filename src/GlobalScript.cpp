#include "MythicPlus.h"
#include "ScriptMgr.h"

class MythicPlus_GlobalScript : public GlobalScript
{
public:

    MythicPlus_GlobalScript() : GlobalScript("MythicPlus_GlobalScript") { }

    void OnAfterUpdateEncounterState(Map* map, EncounterCreditType type,  uint32 /*creditEntry*/, Unit* /*source*/, Difficulty /*difficulty_fixed*/, DungeonEncounterList const* /*encounters*/, uint32 /*dungeonCompleted*/, bool updated) override
    {

    }

    /**
     * This is used to provide better rewards to players when they complete a dungeon.
     * The rewards are scaled up versions of the original drop.
     * @brief Called after a player has looted an item from a creature.
    */
    void OnBeforeDropAddItem(Player const* player, Loot& loot, bool canRate, uint16 lootMode, LootStoreItem* LootStoreItem, LootStore const& store) override
    {


    }

};

void AddGlobalScripts()
{
    new MythicPlus_GlobalScript();
}
