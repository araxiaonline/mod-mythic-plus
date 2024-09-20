#include "MapMgr.h"
#include "MpDataStore.h"
#include "ObjectMgr.h"
#include "MpLogger.h"
#include "MythicPlus.h"
#include "ScriptMgr.h"
#include "Log.h"

class MythicPlus_AllCreatureScript : public AllCreatureScript
{
public:
    MythicPlus_AllCreatureScript() : AllCreatureScript("MythicPlus_AllCreatureScript")
    {

    }

    void OnBeforeCreatureSelectLevel(const CreatureTemplate* /*creatureTemplate*/, Creature* creature, uint8& level) override {

        Map* map = creature->GetMap();
        if (!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        if (!sMythicPlus->IsCreatureEligible(creature)) {
            return;
        }

        MpInstanceData* instanceData = sMpDataStore->GetInstanceData(map->GetId(), map->GetInstanceId());
        if(!instanceData) {
            return;
        }

        MpLogger::debug("OnBeforeCreatureSelectLevel({}, {}) for instance {}",
            creature->GetName(),
            level,
            map->GetMapName()
        );

        // // bail if the creature is not eligible to be scaled
        // if (!sMythicPlus->IsCreatureEligible(creature)) {
        //     return;
        // }

        // // if we have instance data set for this map use it otherwise bail
        // MpInstanceData* instanceData = sMpDataStore->GetInstanceData(map->GetId(), map->GetInstanceId());
        // if(!instanceData) {
        //     return;
        // }

        // if (creature->IsDungeonBoss()) {
        //     level = instanceData->boss.avgLevel;
        // } else {
        //     uint8 level = instanceData->creature.avgLevel;
        //     level = uint8(irand(level-1, level+1));
        // }

    //    LOG_DEBUG("module.MythicPlus", "OnBeforeCreatureSelectLevel({}, {}) for instance {}",
    //         creature->GetName(),
    //         level,
    //         map->GetMapName()
    //     );
    }

    void OnCreatureAddWorld(Creature* creature) override
    {
        uint8 level;

        Map* map = creature->GetMap();
        if (!sMythicPlus->IsMapEligible(map)) {
            // MpLogger::debug("Map: {} is not eligible to adjust so creature was skipped: Creature {}", map->GetMapName(), creature->GetName());
            return;
        }

        // bail if the creature is not eligible to be scaled
        if (!sMythicPlus->IsCreatureEligible(creature)) {
            MpLogger::debug("Creature: {} Entry: {} is not eligible to adjust so creature was skipped.", creature->GetName(), creature->GetEntry());
            return;
        }

        MpInstanceData* instanceData = sMpDataStore->GetInstanceData(map->GetId(), map->GetInstanceId());
        if(!instanceData) {
            MpLogger::debug("Creature: {} Could not find instance data for Map {} and InstanceId: {}", creature->GetName(), map->GetMapName(), map->GetInstanceId());
            return;
        }

        if (creature->IsDungeonBoss()) {
            level = instanceData->boss.avgLevel;
        } else {
            level = uint8(urand(instanceData->creature.avgLevel-1, instanceData->creature.avgLevel+1));
        }

        // Scale the creature to its new level
        sMythicPlus->ScaleCreature(level, creature);


        MpLogger::debug("SetLevel and Updateded Creature {} Entry {} Id {} level from {} to {}",
            creature->GetName(),
            creature->GetEntry(),
            creature->GetGUID().GetCounter(),
            creature->GetLevel(),
            level
        );


        // creature->SetLevel(level, false);
        // MpLogger
        // if (map->IsDungeon()) {
        //     LOG_INFO("modules", "Creature {} added to map {}", creature->GetName(), map->GetMapName());
        //     MpLogger::warn("Creature {} added to map {}", creature->GetName(), map->GetMapName());
        // }
    }

    // void OnCreatureRemoveWorld(Creature* creature) override
    // {
    //     Map* map = creature->GetMap();
    //     if (map->IsDungeon()) {
    //         LOG_INFO("modules", "Creature {} removed from map {}", creature->GetName(), map->GetMapName());
    //         MpLogger::warn("Creature {} removed from map {}", creature->GetName(), map->GetMapName());
    //     }
    // }

    // void OnCreatureAddWorld(Creature* creature) override
    // {
    //     if(!sMythicPlus->IsMapEligible(creature->GetMap())) {
    //         return;
    //     }

    //     MpLogger::debug("OnCreatureAddWorld({}, {}) for instance {}",
    //         creature->GetName(),
    //         creature->GetLevel(),
    //         creature->GetMap()->GetMapName()
    //     );

    //     sMpDataStore->AddInstanceCreatureData(
    //         creature->GetGUID(),
    //         {
    //             creature,
    //             const_cast<MapEntry*>(creature->GetMap()->GetEntry())
    //         }
    //     );

    //     MpLogger::debug("Added creature {} to instance data for instance {}",
    //         creature->GetName(),
    //         creature->GetMap()->GetMapName()
    //     );
    // }

    // void OnCreatureRemoveWorld(Creature* creature) override
    // {
    //     if(!sMythicPlus->IsMapEligible(creature->GetMap())) {
    //         return;
    //     }

    //     MpLogger::debug("AllCreatureScript::OnCreatureRemoveWorld({}, {})", creature->GetName(), creature->GetLevel());

    //     sMpDataStore->RemoveInstanceCreatureData(creature->GetGUID());

    //     MpLogger::debug("Removed creature {} from instance data for instance {}",
    //         creature->GetName(),
    //         creature->GetMap()->GetMapName()
    //     );
    // }

    // void OnAllCreatureUpdate(Creature* creature, uint32 /*diff*/) override
    // {
    //     // If the config is out of date and the creature was reset, run modify against it
    //     // if (ResetCreatureIfNeeded(creature))
    //     // {
    //     //     LOG_DEBUG("module.MythicPlus",
    //     //         "MythicPlus_AllCreatureScript::OnAllCreatureUpdate(): Creature {} ({}) is reset to its original stats.",
    //     //         creature->GetName(),
    //     //         creature->GetLevel()
    //     //     );

    //     //     // Update the map's level if it is out of date
    //     //     sMythicPlus->UpdateMapLevelIfNeeded(creature->GetMap());

    //     //     ModifyCreatureAttributes(creature);
    //     // }
    // }

    bool UpdateCreature(Creature* creature)
    {
        // make sure we have a creature and that it's assigned to a map
        if (!creature || !creature->GetMap())
            return false;

        // if this isn't a dungeon or a battleground, make no changes
        if (!sMythicPlus->IsMapEligible(creature->GetMap()))
            return false;

        // if this is a pet or summon controlled by the player, make no changes
        if ((creature->IsHunterPet() || creature->IsPet() || creature->IsSummon()) && creature->IsControlledByPlayer())
            return false;

        // if this is a non-relevant creature, skip
        if (creature->IsCritter() || creature->IsTotem() || creature->IsTrigger())
            return false;

        if (creature->GetMap()->GetEntry()) {

        }

        return true;
    }

};

void Add_MP_AllCreatureScripts()
{
    MpLogger::debug("Add_MP_AllCreatureScripts");
    new MythicPlus_AllCreatureScript();
}
