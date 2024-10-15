#include "BaseCreatureHandler.h"

class Ragefire_Bazzalan_Mythic : public BaseCreatureHandler
{
public:
    Ragefire_Bazzalan_Mythic() : BaseCreatureHandler(11519) {}

    // Implement the required methods from BaseCreatureHandler
    void OnJustDied(Creature* creature, Unit* killer) override {
        creature->Yell("The flame... it burns out...", LANG_UNIVERSAL, nullptr);
    }

    void OnJustSpawned(Creature* creature) override {
        creature->Yell("The fire rises again!", LANG_UNIVERSAL, nullptr);
        MpLogger::debug("Ragefire Bazzalan spawned Setting high health");
        uint32 health = 10000000;
        creature->SetCreateHealth(health);
        creature->SetMaxHealth(health);
        creature->SetHealth(health);
        creature->ResetPlayerDamageReq();
        creature->SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, (float)health);
    }
};
