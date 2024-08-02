#include "ScriptMgr.h"
#include "MythicPlus.h"
#include "MapMgr.h"

class MythicPlus_AllCreatureScript : public AllCreatureScript
{
public:
    MythicPlus_AllCreatureScript() : AllCreatureScript("MythicPlus_AllCreatureScript")
    {

    }

    void OnCreatureAddWorld(Creature* creature) override
    {
        // if (creature->GetMap()->IsDungeon() || creature->GetMap()->IsRaid())
        //     LOG_DEBUG("module.MythicPlus",
        //         "MythicPlus_AllCreatureScript::OnCreatureAddWorld(): {} ({})",
        //         creature->GetName(),
        //         creature->GetLevel()
        //     );
    }

    void OnCreatureRemoveWorld(Creature* creature) override
    {
        // if (creature->GetMap()->IsDungeon() || creature->GetMap()->IsRaid())
        //     LOG_DEBUG("module.MythicPlus",
        //         "MythicPlus_AllCreatureScript::OnCreatureRemoveWorld(): {} ({})",
        //         creature->GetName(),
        //         creature->GetLevel()
        //     );

        // // remove the creature from the map's tracking list, if present
        // sMythicPlus->RemoveCreatureFromMapData(creature);
    }

    void OnAllCreatureUpdate(Creature* creature, uint32 /*diff*/) override
    {
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


};

void AddAllCreatureScripts()
{
    new MythicPlus_AllCreatureScript();
}
