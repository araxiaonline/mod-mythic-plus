// #include "BaseCreatureHandler.h"

// class Ragefire_Bazzalan_Mythic : public BaseCreatureHandler
// {
// public:
//     Ragefire_Bazzalan_Mythic(uint32 creature) : BaseCreatureHandler(creature.GetEntry());

//     // Implement the required methods from BaseCreatureHandler
//     void OnJustDied(Creature* creature, Unit* killer) override {
//         // Bazzalan-specific behavior on death
//         creature->Yell("The flame... it burns out...", LANG_UNIVERSAL, nullptr);
//     }

//     void OnJustSpawned(Creature* creature) override {
//         // Bazzalan-specific behavior on spawn
//         creature->Yell("The fire rises again!", LANG_UNIVERSAL, nullptr);
//         creature->SetHealth(creature->GetMaxHealth());  // Restore health on spawn
//     }
// };
