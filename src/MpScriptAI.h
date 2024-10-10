#include "Creature.h"
#include "CreatureAI.h"
#include "CreatureHooks.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#ifdef _ELUNA_CREATURE_AI_H
    #include "ElunaCreatureAI.h"
#endif


class MpScriptAI : public ScriptedAI
{
public:
    MpScriptAI(Creature* creature) : ScriptedAI(creature) {
#ifdef _ELUNA_CREATURE_AI_H
        // Check if Eluna is attached to the creature
        if (ElunaCreatureAI* eluna = dynamic_cast<ElunaCreatureAI*>(creature->AI())) {
            elunaAI = eluna;  // Store a pointer to the Eluna AI
        } else {
            elunaAI = nullptr;  // No Eluna AI attached
        }
#endif
    }

    // Example for JustDied event
    void JustDied(Unit* killer) override {
        sCreatureHooks->JustDied(me, killer);

#ifdef _ELUNA_CREATURE_AI_H
        // If Eluna is attached, call its JustDied event
        if (elunaAI){
            elunaAI->JustDied(killer);
        }
        else
#endif
        {
            // If Eluna is not installed or not attached, call the default AI handler
            ScriptedAI::JustDied(killer);
        }
    }

    void Reset() override {
#ifdef _ELUNA_CREATURE_AI_H
        // If Eluna is attached, call its Reset event
        if (elunaAI){
            elunaAI->Reset();
        }
        else
#endif
        {
            // Call the default AI handler
            ScriptedAI::Reset();
        }
    }

private:
#ifdef ENABLE_ELUNA
    ElunaCreatureAI* elunaAI;  // Store a pointer to Eluna's AI if attached
#endif
};
