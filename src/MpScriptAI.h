#include "Creature.h"
#include "CreatureAI.h"
#include "CreatureHooks.h"
#include "MpLogger.h"
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
    Difficulty _difficulty;
public:
    MpScriptAI(Creature* creature, Difficulty difficulty) : BaseAI(creature) {
        _difficulty = difficulty;
    }

    void JustDied(Unit* killer) override {
        MpLogger::debug("***** MythicPlus Script AI JustDied() for creature: ", me->GetEntry());

        sCreatureHooks->JustDied(me->ToCreature(), killer);

        BaseAI::JustDied(killer);
    }

    void Reset() override {
        MpLogger::debug("***** MythicPlus Script AI Reset() for creature: ", me->GetEntry());
        BaseAI::Reset();
    }

};
