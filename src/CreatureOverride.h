// #include "MpDataStore.h"
// #include "MpLogger.h"

// class CreatureOverride {
// public:
//     uint32 entry;
//     MpDifficulty difficulty;
//     Creature* creature;

//     CreatureOverride(uint32 entry): entry(entry), difficulty(difficulty) {
//         sMpDataStore->AddCreatureOverride(this->entry, this);
//     }

//     virtual void Modify(Creature* creature) final {
//         this->creature = creature;
//     }

//     virtual void HandleModify() = 0;


//     void ModifyAttackPower(uint32 ap) {
//         if(this)
//     }

//     void ModifyHealth(uint32 health) {}

//     void ModifyArmor(uint32 armor) {}

//     void ModifySpellPower(uint32 sp) {}

//     ~CreatureOverride() {}
// };
