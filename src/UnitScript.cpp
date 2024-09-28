#include "MpLogger.h"
#include "Player.h"
#include "MythicPlus.h"
#include "ScriptMgr.h"

class MythicPlus_UnitScript : public UnitScript
{
public:
    MythicPlus_UnitScript() : UnitScript("MythicPlus_UnitScript", true) { }

    void ModifyPeriodicDamageAurasTick(Unit* target, Unit* attacker, uint32& damage, SpellInfo const* /*spellInfo*/) override {
        if (!target && !attacker) {
            return;
        }

        Map *map = target->GetMap();
        if(!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        damage = modifyIncomingDmgHeal(MythicPlus::UNIT_EVENT_DOT, target, attacker, damage);
    }

    void ModifySpellDamageTaken(Unit* target, Unit* attacker, int32& damage, SpellInfo const* /*spellInfo*/) override {
        if (!target && !attacker) {
            return;
        }

        Map *map = target->GetMap();
        if(!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        damage = modifyIncomingDmgHeal(MythicPlus::UNIT_EVENT_SPELL, target, attacker, damage);
    }

    /**
     * Directly Modify the melee damage characters and allied creatures will
     * receive from mythic+ scaled enemies.
     */
    void ModifyMeleeDamage(Unit* target, Unit* attacker, uint32& damage) override {
        if (!target && !attacker) {
            return;
        }

        Map *map = target->GetMap();
        if(!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        damage = modifyIncomingDmgHeal(MythicPlus::UNIT_EVENT_MELEE, target, attacker, damage);
    }

    // When a healing spell hits a mythic+ enemy modify based on the modifiers for the difficulty
    void ModifyHealReceived(Unit* target, Unit* healer, uint32& healing, SpellInfo const* /*spellInfo*/) override {
      if (!target && !healer) {
            return;
        }

        Map *map = target->GetMap();
        if(!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        healing = modifyIncomingDmgHeal(MythicPlus::UNIT_EVENT_HEAL, target, healer, healing);
    }

    // void OnAuraApply(Unit* unit, Aura* aura) override {}
};

uint32 modifyIncomingDmgHeal(MythicPlus::MP_UNIT_EVENT_TYPE eventType,Unit* target, Unit* attacker, uint32 damageOrHeal) {
    if (!target && !attacker) {
        return damageOrHeal;
    }

    Map *map = target->GetMap();
    if(!sMythicPlus->IsMapEligible(map)) {
        return damageOrHeal;
    }


    Creature* creature = attacker->ToCreature();
    MpInstanceData* instanceData = sMpDataStore->GetInstanceData(map->GetId(), map->GetInstanceId());
    if(!instanceData) {
        return damageOrHeal;
    }

    // If the target is the enemy then increase the amount of healing by the instance data modifier for spell output.
    if(sMythicPlus->EligibleDamageTarget(target)) {
        /**
         * @TODO: Allow more granular control over the scaling of DOT, HOT, and other spell effects
         * in the future if needed
         */
        switch (eventType) {
            case MythicPlus::UNIT_EVENT_MELEE:
                if(creature->IsDungeonBoss()) {
                    return damageOrHeal * instanceData->boss.melee;
                } else {
                    return damageOrHeal * instanceData->creature.melee;
                }
                break;
            case MythicPlus::UNIT_EVENT_DOT:
            case MythicPlus::UNIT_EVENT_SPELL:
                if(creature->IsDungeonBoss()) {
                    return damageOrHeal * instanceData->boss.spell;
                } else {
                    return damageOrHeal * instanceData->creature.spell;
                }
                break;
        }
    }

    /**
     * @TODO: Add more granular control over the scaling of healing spells
     */
    if(sMythicPlus->EligibleHealTarget(target)) {
        if(creature->IsDungeonBoss()) {
            return damageOrHeal * instanceData->boss.spell;
        } else {
            return damageOrHeal * instanceData->creature.spell;
        }
    }

    return damageOrHeal;
}

void Add_MP_UnitScripts()
{
    new MythicPlus_UnitScript();
}
