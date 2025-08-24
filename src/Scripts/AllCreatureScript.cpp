#include "CreatureAI.h"
#include "MpDataStore.h"
#include "MpLogger.h"
#include "MythicPlus.h"
#include "MpScriptAI.h"
#include "ScriptMgr.h"

class MythicPlus_AllCreatureScript : public AllCreatureScript
{
private:
    std::unordered_map<ObjectGuid, uint32> m_creatureUpdateTimers;

public:
    MythicPlus_AllCreatureScript() : AllCreatureScript("MythicPlus_AllCreatureScript") {}

    // void OnBeforeCreatureSelectLevel(const CreatureTemplate* /*creatureTemplate*/, Creature* creature, uint8& level) override
    // {
    // }

    void OnCreatureRespawn(Creature* creature)
    {
        Map* map = creature->GetMap();
        if (!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        if (!sMythicPlus->IsCreatureEligible(creature)) {
            return;
        }

        // If we have instance data, scale the creature, otherwise add it to be scaled later
        if (MpInstanceData* instanceData = sMpDataStore->GetInstanceData(map->GetId(), map->GetInstanceId())) {
            sMythicPlus->AddScaledCreature(creature, instanceData);
        } else {
            sMythicPlus->AddCreatureForScaling(creature);
        }
    }

    /**
     * @brief This hook runs every update for all creatures in the world.
     * We only need to concern ourselves with creatures in the scope of our mythic+ instances.
     * Need to detect the following changes:
     *  - Creature Death State - to trigger respawn scaling.
     *  - Other events where a creature enters the instance that is not scaled, then should be scaled up. Some special events normal enemies will be scripted
     *    to show up in encounters these will not trigger the OnCreatureAddWorld, because they were not during the initial load of the instance. (Though sometimes summons do trigger this?)
     *
     * @param creature
     * @param diff
     */
    void OnAllCreatureUpdate(Creature* creature, uint32 diff) override
    {
        // Skip any creatures not in an instance we are scaling first to avoid unnecessary work
        if (!sMythicPlus->IsMapEligible(creature->GetMap())) {
            return;
        }

        if (!sMythicPlus->IsCreatureEligible(creature)) {
            return;
        }

        // throttle this check per creature to only run if more than 20ms has passed since last check
        ObjectGuid creatureGuid = creature->GetGUID();
        m_creatureUpdateTimers[creatureGuid] += diff;
        if(m_creatureUpdateTimers[creatureGuid] < 20) {
            return;
        }
        m_creatureUpdateTimers[creatureGuid] = 0;


        auto instanceData = sMpDataStore->GetInstanceData(creature->GetMapId(), creature->GetInstanceId());
        // no instance data yet means dont scale.
        if(!instanceData) {
            return;
        }

        MpCreatureData* creatureData = sMpDataStore->GetCreatureData(creature->GetGUID());

        // this is a creature that was not scaled at instance load time, we need to scale it now.
        if(!creatureData) {
            MpLogger::debug("OnAllCreatureUpdate: Unknown Creature Add event scaling creature: {}", creature->GetName());
            sMythicPlus->AddScaledCreature(creature, sMpDataStore->GetInstanceData(creature->GetMap()->GetId(), creature->GetMap()->GetInstanceId()));
            return;
        }
        DeathState currentState = creature->getDeathState();

        // record the death of our scaled creature
        if(currentState == DeathState::Corpse && creatureData->lastDeathState != DeathState::Corpse) {
            creatureData->lastDeathState = currentState;
            return;
        }

        if(currentState == DeathState::Alive && creatureData->lastDeathState == DeathState::Corpse) {
            MpLogger::debug("OnAllCreatureUpdate: Creature Death event scaling creature: {} level: {} guid: {} event: {}", creature->GetName(), creatureData->creature->GetLevel(), creature->GetGUID().ToString(), creature->getDeathState());
            if(creature->IsDungeonBoss() || creature->GetEntry() == 23682) {
                sMythicPlus->AddScaledCreature(creature, instanceData);
            } else {
                sMythicPlus->AddScaledCreature(creature, instanceData);
            }
        }


    }

    // When a new creature is added into a mythic+ map add it to the list of creatures to scale later.
    void OnCreatureAddWorld(Creature* creature) override
    {
        Map* map = creature->GetMap();
        if (!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        if (!sMythicPlus->IsCreatureEligible(creature)) {
            return;
        }

        // if we have instance data about zone then just scale the creature otherwise add to be scaled once we do.
        MpInstanceData* instanceData = sMpDataStore->GetInstanceData(map->GetId(), map->GetInstanceId());

        if(instanceData) {
            sMythicPlus->AddScaledCreature(creature, instanceData);
        } else {
            sMythicPlus->AddCreatureForScaling(creature);
        }
    }

    // Cleanup the creature from custom data used for mythic+ mod
    void OnCreatureRemoveWorld(Creature* creature) override
    {
        sMythicPlus->RemoveCreature(creature);
    }

};

void Add_MP_AllCreatureScripts()
{
    MpLogger::debug("Add_MP_AllCreatureScripts");
    new MythicPlus_AllCreatureScript();
}
