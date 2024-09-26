#include "MpLogger.h"
#include "Player.h"
#include "MythicPlus.h"
#include "ScriptMgr.h"

class MythicPlus_UnitScript : public UnitScript
{
public:
    MythicPlus_UnitScript() : UnitScript("MythicPlus_UnitScript", true) { }

    void ModifyPeriodicDamageAurasTick(Unit* target, Unit* attacker, uint32& damage, SpellInfo const* /*spellInfo*/) override {
        Map *map = target->GetMap();
        if(!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        // If the target is the enemy then increase the amount of healing by the instance data modifier for spell output.
        if(target->isType(TYPEID_PLAYER) && attacker->isType(TYPEID_UNIT)) {
            Creature* creature = target->ToCreature();
            if(!creature || !sMythicPlus->IsCreatureEligible(creature)) {
                return;
            }

            MpInstanceData* instanceData = sMpDataStore->GetInstanceData(map->GetId(), map->GetInstanceId());
            if(!instanceData) {
                return;
            }

            if(creature->IsDungeonBoss()) {
                damage = damage * (instanceData->boss.spell * 0.8);
            } else {
                damage = damage * (instanceData->creature.spell * 0.8);
            }
        }
    }

    void ModifySpellDamageTaken(Unit* target, Unit* attacker, int32& damage, SpellInfo const* /*spellInfo*/) override {
        Map *map = target->GetMap();
        if(!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        // If the target is the enemy then increase the amount of healing by the instance data modifier for spell output.
        if(target->isType(TYPEID_PLAYER) && attacker->isType(TYPEID_UNIT)) {
            Creature* creature = target->ToCreature();
            if(!creature || !sMythicPlus->IsCreatureEligible(creature)) {
                return;
            }

            MpInstanceData* instanceData = sMpDataStore->GetInstanceData(map->GetId(), map->GetInstanceId());
            if(!instanceData) {
                return;
            }

            if(creature->IsDungeonBoss()) {
                damage = damage * instanceData->boss.spell;
            } else {
                damage = damage * instanceData->creature.spell;
            }
        }
    }

    void ModifyMeleeDamage(Unit* target, Unit* attacker, uint32& damage) override {

        Map *map = target->GetMap();
        if(!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        MpLogger::debug("ModifyMeleeDamage: {} reached for attacker {} attacking {} for {} dmg", map->GetMapName(), attacker->GetName(), target->GetName(), damage);

        // If the target is the enemy then increase the amount of healing by the instance data modifier for spell output.
        if(target->isType(TYPEID_PLAYER) && attacker->isType(TYPEID_UNIT)) {
            Creature* creature = target->ToCreature();

            MpLogger::debug("ModifyMeleeDamage: {} ", creature->GetName());
            if(!creature || !sMythicPlus->IsCreatureEligible(creature)) {
                return;
            }

            MpInstanceData* instanceData = sMpDataStore->GetInstanceData(map->GetId(), map->GetInstanceId());
            if(!instanceData) {
                return;
            }

            auto origDamage = damage;
            if(creature->IsDungeonBoss()) {
                damage = damage * instanceData->boss.melee * 10;
            } else {
                damage = damage * instanceData->creature.melee * 10 ;
            }

            MpLogger::debug("ModifyMeleeDamage: from {} to {} ", damage);
        }

    }

    // When a healing spell hits a mythic+ enemy modify based on the modifiers for the difficulty
    void ModifyHealReceived(Unit* target, Unit* healer, uint32& healing, SpellInfo const* /*spellInfo*/) override {

        Map *map = target->GetMap();
        if(!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        // If the target is the enemy then increase the amount of healing by the instance data modifier for spell output.
        if(target->isType(TYPEID_UNIT)) {
            Creature* creature = target->ToCreature();
            if(!creature || !sMythicPlus->IsCreatureEligible(creature)) {
                return;
            }

            MpInstanceData* instanceData = sMpDataStore->GetInstanceData(map->GetId(), map->GetInstanceId());
            if(!instanceData) {
                return;
            }

            if(creature->IsDungeonBoss()) {
                healing = healing * instanceData->boss.spell;
            } else {
                healing = healing * instanceData->creature.spell;

            }
        }

    }

    // void OnAuraApply(Unit* unit, Aura* aura) override {

    // }

};
void Add_MP_UnitScripts()
{
    new MythicPlus_UnitScript();
}
