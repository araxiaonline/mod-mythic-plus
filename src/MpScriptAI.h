#include "Creature.h"
#include "CreatureAI.h"
#include "CreatureHooks.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#ifdef _ELUNA_CREATURE_AI_H
    #include "ElunaCreatureAI.h"
    using BaseAI = ElunaCreatureAI;
#else
    using BaseAI = ScriptedAI;
#endif

class MpScriptAI : public BaseAI
{
public:
    MpScriptAI(Creature* creature) : BaseAI(creature) {}

    void JustDied(Unit* killer) override {
        sCreatureHooks->JustDied(me, killer);

        BaseAI::JustDied(killer);
    }

    void Reset() override {
        BaseAI::Reset();
    }

};
