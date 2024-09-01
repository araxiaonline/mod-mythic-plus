#include "MpLogger.h"
#include "Player.h"
#include "ScriptMgr.h"

class MythicPlus_UnitScript : public UnitScript
{
public:
    MythicPlus_UnitScript() : UnitScript("MythicPlus_UnitScript", true) { }

    uint32 DealDamage(Unit* /*AttackerUnit*/, Unit* /*playerVictim*/, uint32 damage, DamageEffectType /*damagetype*/) override {
        return damage;
    }

    void ModifyPeriodicDamageAurasTick(Unit* /*target */, Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*spellInfo*/) override {

    }

    void ModifySpellDamageTaken(Unit* /*target*/, Unit* /*attacker*/, int32& /*damage*/, SpellInfo const* /*spellInfo*/) override {

    }

    void ModifyMeleeDamage(Unit* /*target*/, Unit* /*attacker*/, uint32& /*damage*/) override {

    }

    void ModifyHealReceived(Unit* /*target*/, Unit* /*attacker*/, uint32& /*damage*/, SpellInfo const* /*spellInfo*/) override {

    }

    void OnAuraApply(Unit* unit, Aura* aura) override {

    }

};
