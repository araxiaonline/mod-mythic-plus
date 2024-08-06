#include "MapMgr.h"
#include "MpDataStore.h"
#include "MpLogger.h"
#include "MythicPlus.h"
#include "ScriptMgr.h"

class MythicPlus_AllCreatureScript : public AllCreatureScript
{
public:
    MythicPlus_AllCreatureScript() : AllCreatureScript("MythicPlus_AllCreatureScript")
    {

    }

    void OnCreatureAddWorld(Creature* creature) override
    {
        MythicPlus* mp = MythicPlus::getInstance();
        if(!mp->IsMapEligible(creature->GetMap())) {
            return;
        }

        MpLogger::debug("OnCreatureAddWorld({}, {}) for instance {}",
            creature->GetName(),
            creature->GetLevel(),
            creature->GetMap()->GetMapName()
        );

        MpDataStore* mpds = MpDataStore::getInstance();
        mpds->AddInstanceCreatureData(
            creature->GetGUID(),
            {
                creature,
                const_cast<MapEntry*>(creature->GetMap()->GetEntry())
            }
        );

        MpLogger::debug("Added creature {} to instance data for instance {}",
            creature->GetName(),
            creature->GetMap()->GetMapName()
        );
    }

    void OnCreatureRemoveWorld(Creature* creature) override
    {
        MythicPlus* mp = MythicPlus::getInstance();
        if(!mp->IsMapEligible(creature->GetMap())) {
            return;
        }

        MpLogger::debug("AllCreatureScript::OnCreatureRemoveWorld({}, {})", creature->GetName(), creature->GetLevel());

        MpDataStore* mpds = MpDataStore::getInstance();
        mpds->RemoveInstanceCreatureData(creature->GetGUID());

        MpLogger::debug("Removed creature {} from instance data for instance {}",
            creature->GetName(),
            creature->GetMap()->GetMapName()
        );
    }

    void OnAllCreatureUpdate(Creature* creature, uint32 /*diff*/) override
    {
        MythicPlus* mp = MythicPlus::getInstance();

        // If the config is out of date and the creature was reset, run modify against it
        // if (ResetCreatureIfNeeded(creature))
        // {
        //     LOG_DEBUG("module.MythicPlus",
        //         "MythicPlus_AllCreatureScript::OnAllCreatureUpdate(): Creature {} ({}) is reset to its original stats.",
        //         creature->GetName(),
        //         creature->GetLevel()
        //     );

        //     // Update the map's level if it is out of date
        //     sMythicPlus->UpdateMapLevelIfNeeded(creature->GetMap());

        //     ModifyCreatureAttributes(creature);
        // }
    }

bool UpdateCreature(Creature* creature)
{
    MythicPlus* mp = MythicPlus::getInstance();

    // make sure we have a creature and that it's assigned to a map
    if (!creature || !creature->GetMap())
        return false;

    // if this isn't a dungeon or a battleground, make no changes
    if (!mp->IsMapEligible(creature->GetMap()))
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
    MpLogger::debug("Add_MP_AllCreatureScripts()");
    new MythicPlus_AllCreatureScript();
}
