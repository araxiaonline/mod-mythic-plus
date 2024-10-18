#include "CreatureAI.h"
#include "MpDataStore.h"
#include "MpLogger.h"
#include "MythicPlus.h"
#include "MpScriptAI.h"
#include "ScriptMgr.h"

class MythicPlus_AllCreatureScript : public AllCreatureScript
{
public:
    MythicPlus_AllCreatureScript() : AllCreatureScript("MythicPlus_AllCreatureScript") {}

    // void OnBeforeCreatureSelectLevel(const CreatureTemplate* /*creatureTemplate*/, Creature* creature, uint8& level) override
    // {
    // }

    // void OnAllCreatureUpdate(Creature* creature, uint32 diff) override
    // {
    // }

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
