// #include "BaseCreatureHandler.h"
// #include "Spell.h"

// /**
//  * Bazzalan need some upgrades so made him more formidable
//  */
// class Ragefire_Bazzalan_Mythic : public BaseCreatureHandler
// {
// public:
//     Ragefire_Bazzalan_Mythic() : BaseCreatureHandler(11519) {}


//     void OnAddToInstance(Creature* creature) override {

//         uint32 health = creature->GetMaxHealth() * 2;
//         creature->SetCreateHealth(health);
//         creature->SetMaxHealth(health);
//         creature->SetHealth(health);
//         creature->ResetPlayerDamageReq();
//         creature->SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, (float)health);

//         creature->AddExtraAttacks(3);
//         creature->SetObjectScale(2.0f);

//     }
// };
