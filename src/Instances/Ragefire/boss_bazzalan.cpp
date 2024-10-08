#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "CreatureAI.h"
#include "MpLogger.h"

class Ragefire_Bazzalan_Mythic : public CreatureScript
{
public:
    Ragefire_Bazzalan_Mythic() : CreatureScript("Ragefire_Bazzalan_Mythic") { }

    struct Ragefire_Bazzalan_MythicAI : public ScriptedAI
    {
        Ragefire_Bazzalan_MythicAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            MpLogger::debug(" >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  Ragefire Bazzalan Mythic reset, restoring custom health");
                uint32 health = 1000000;  // Ensure max health is reset if the boss resets
                me->SetCreateHealth(health);
                me->SetMaxHealth(health);
                me->SetHealth(health);
                me->ResetPlayerDamageReq();
                me->SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, (float)health);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new Ragefire_Bazzalan_MythicAI(creature);
    }
};

void AddSC_Ragefire_Bazzalan_Mythic()
{
    new Ragefire_Bazzalan_Mythic();
}
