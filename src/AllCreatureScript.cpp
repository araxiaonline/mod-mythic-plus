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

        // // Assign random affix for now.
        if (roll_chance_i(30)) {
            uint32 irand = urand(0, 2);

            if(irand == 0) {
                creature->AddAura(23341, creature);
                creature->SetName("Infernal " + creature->GetName());
            } else if(irand == 1) {
                creature->AddAura(34711, creature);
                creature->SetName("Berserking " + creature->GetName());
            } else {
                creature->AddAura(774, creature);
                creature->SetName("Blessed " + creature->GetName());
            }
        }

        creature->SetAI(new MpScriptAI(creature));
        MpLogger::debug("Creature AI debug info: {}", creature->GetAI()->GetDebugInfo());
    }

    // Cleanup the creature from custom data used for mythic+ mod
    void OnCreatureRemoveWorld(Creature* creature) override
    {
        sMythicPlus->RemoveCreature(creature);
    }

    // CreatureAI* GetCreatureAI(Creature* creature) const override
    // {
    //     if(!creature) {
    //         return nullptr;
    //     }

    //     uint32 instance = creature->GetInstanceId();
    //     if(!instance) {
    //         return nullptr;
    //     }

    //     MpLogger::debug("GetCreatureAI: Instance logger for instanceId {} called on {}", instance, creature->GetGUID().GetCounter());

    //     // Attach to creatures that are in a mythic+ map
    //     MpCreatureData* creatureData = sMpDataStore->GetCreatureData(creature->GetGUID());
    //     if (!creatureData) {
    //         return nullptr;
    //     }

    //     MpLogger::debug("GetCreatureAI: called on {}", creature->GetName());

    //     return new MpScriptAI(creature);
    // }

};

void Add_MP_AllCreatureScripts()
{
    MpLogger::debug("Add_MP_AllCreatureScripts");
    new MythicPlus_AllCreatureScript();
}
