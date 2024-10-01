#include "MpLogger.h"
#include "MythicPlus.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "Map.h"

class MythicPlus_GlobalScript : public GlobalScript
{
public:

    MythicPlus_GlobalScript() : GlobalScript("MythicPlus_GlobalScript") { }

    // This adds the mythic+ item scaling to the loot table for enemies
    void OnBeforeDropAddItem(Player const* player, Loot& loot, bool /*canRate*/, uint16 /*lootMode*/, LootStoreItem* LootStoreItem, LootStore const& store) override {

        if(LootStoreItem->itemid == 0) {
            return;
        }

        // Not playing on mythic+ difficulty skip doing loot
        if(!sMythicPlus->IsDifficultySet(player)) {
            return;
        }

        // Not on an eligible map skip doing loot
        Map* map = player->GetMap();
        if (!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        MpInstanceData* mythicSettings = sMpDataStore->GetInstanceData(map->GetId(), map->GetInstanceId());

        // if there are not mythic settings set for this group and map skip
        if (!mythicSettings) {
            MpLogger::warn("No mythic settings found for map {} instance {}", map->GetMapName(), map->GetInstanceId());
            return;
        }

        // if the item rewards are disabled skip
        if (mythicSettings->itemRewards == false ) {
            return;
        }

        // get the item to scale up
        ItemTemplate const* origItem = sObjectMgr->GetItemTemplate(LootStoreItem->itemid);
        if (!origItem) {
            MpLogger::warn("Item not found for itemid {} in  OnBeforeDropAddItem()", LootStoreItem->itemid);
            return;
        }

        uint32 newItemId = origItem->ItemId + mythicSettings->itemOffset;
        ItemTemplate const* newItemTempl = sObjectMgr->GetItemTemplate(newItemId);

        if(!newItemTempl) {
            MpLogger::warn("New Loot Item not found for itemid {} original item: {} ({})", newItemId, origItem->Name1, origItem->ItemId);
            return;
        }

        LootStoreItem->itemid = newItemId;

        // Revalidate the LootStoreItem to ensure consistency
        if (!LootStoreItem->IsValid(store, newItemId)) {
            MpLogger::info("LootStoreItem is not valid after updating itemid to {} in OnBeforeDropAddItem()", newItemId);
            return;
        }
    }

};

void Add_MP_GlobalScripts()
{
    MpLogger::debug("Add_MP_GlobalScripts()");
    new MythicPlus_GlobalScript();
}
