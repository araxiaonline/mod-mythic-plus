#include "MpLogger.h"
#include "Player.h"
#include "MythicPlus.h"
#include "ScriptMgr.h"

class MythicPlus_UnitScript : public UnitScript
{
public:
    MythicPlus_UnitScript() : UnitScript("MythicPlus_UnitScript", true) { }

    void ModifyPeriodicDamageAurasTick(Unit* target, Unit* attacker, uint32& damage, SpellInfo const* spellInfo) override {
        if (!target && !attacker) {
            return;
        }

        Map *map = target->GetMap();
        if(!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        bool haspositiveeffect = false;
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i) {
            if (spellInfo->_IsPositiveEffect(i, true)) {
                haspositiveeffect = true;
                break;
            }
        }

        if(haspositiveeffect) {
            damage = modifyIncomingDmgHeal(MythicPlus::UNIT_EVENT_HOT, target, attacker, damage);
        } else {
            damage = modifyIncomingDmgHeal(MythicPlus::UNIT_EVENT_DOT, target, attacker, damage);
        }
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

    uint32 modifyIncomingDmgHeal(MythicPlus::MP_UNIT_EVENT_TYPE eventType,Unit* target, Unit* attacker, uint32 damageOrHeal) {
        if (!target && !attacker) {
            return damageOrHeal;
        }

        int32 alteredDmgHeal = 0;

        Map *map = target->GetMap();
        if(!sMythicPlus->IsMapEligible(map)) {
            return damageOrHeal;
        }

        Creature* creature = attacker ? attacker->ToCreature() : nullptr;
        if (!creature) {
            MpLogger::debug("Attacker was considered not a creature");
            return damageOrHeal;
        }

        MpInstanceData* instanceData = sMpDataStore->GetInstanceData(map->GetId(), map->GetInstanceId());
        if(!instanceData) {
            return damageOrHeal;
        }

        std::string eventName = "";
        switch (eventType) {
            case MythicPlus::UNIT_EVENT_MELEE:
                eventName = "Melee";
                break;
            case MythicPlus::UNIT_EVENT_HEAL:
                eventName = "Heal";
                break;
            case MythicPlus::UNIT_EVENT_DOT:
                eventName = "DOT";
                break;
            case MythicPlus::UNIT_EVENT_SPELL:
                eventName = "Spell";
                break;

        }

        MpLogger::debug("Incoming Event Type ({}): {} hits {} before mod: {}", eventName, attacker->GetName(), target->GetName(), damageOrHeal);

        bool isHeal = false;
        // If the target is the enemy then increase the amount of healing by the instance data modifier for spell output.
        if(sMythicPlus->EligibleDamageTarget(target)) {
            /**
             * @TODO: Allow more granular control over the scaling of DOT, HOT, and other spell effects
             * in the future if needed
             */
            switch (eventType) {
                case MythicPlus::UNIT_EVENT_MELEE:
                    if(creature->IsDungeonBoss()) {
                        alteredDmgHeal = damageOrHeal * instanceData->boss.melee;
                    } else {
                        alteredDmgHeal = damageOrHeal * instanceData->creature.melee;
                    }
                    MpLogger::debug("Incoming Melee New Damage: {}({}) {} hits {}", alteredDmgHeal, damageOrHeal, attacker->GetName(), target->GetName());
                    break;
                case MythicPlus::UNIT_EVENT_DOT:
                case MythicPlus::UNIT_EVENT_SPELL:
                    if(creature->IsDungeonBoss()) {
                        alteredDmgHeal = damageOrHeal * instanceData->boss.spell;
                    } else {
                        alteredDmgHeal = damageOrHeal * instanceData->creature.spell;
                    }
                    MpLogger::debug("Incoming spell or dot New Damage: {}({}) {} hits {}", alteredDmgHeal, damageOrHeal, attacker->GetName(), target->GetName());
                    break;
            }
        }

        /**
         * @TODO: Add more granular control over the scaling of healing spells
         */
        if(sMythicPlus->EligibleHealTarget(target) && (eventType == MythicPlus::UNIT_EVENT_HEAL || eventType == MythicPlus::UNIT_EVENT_HOT)) {
            isHeal = true;
            if(creature->IsDungeonBoss()) {
                alteredDmgHeal = damageOrHeal * instanceData->boss.spell;
            } else {
                alteredDmgHeal = damageOrHeal * instanceData->creature.spell;
            }
            MpLogger::debug("Incoming heal: {}({}) {} hits {}", alteredDmgHeal, damageOrHeal, attacker->GetName(), target->GetName());
        }

        return alteredDmgHeal > 0 ? alteredDmgHeal : damageOrHeal;
    }
    // void OnAuraApply(Unit* unit, Aura* aura) override {}
};

void Add_MP_UnitScripts()
{
    new MythicPlus_UnitScript();
}
