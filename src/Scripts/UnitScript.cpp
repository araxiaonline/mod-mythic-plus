#include "MpLogger.h"
#include "Player.h"
#include "MythicPlus.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"

class MythicPlus_UnitScript : public UnitScript
{
public:
    MythicPlus_UnitScript() : UnitScript("MythicPlus_UnitScript", true) { }

private:
    // Helper function to determine if a spell scales with Attack Power
    bool IsAttackPowerScalingSpell(SpellInfo const* spellInfo) {
        if (!spellInfo || spellInfo->Effects.empty()) {
            return false;
        }

        auto mainEffect = spellInfo->Effects[0];

        // Check 1: Direct weapon damage effects
        bool isWeaponEffect = (mainEffect.Effect == SPELL_EFFECT_WEAPON_DAMAGE ||
                              mainEffect.Effect == SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL ||
                              mainEffect.Effect == SPELL_EFFECT_NORMALIZED_WEAPON_DMG ||
                              mainEffect.Effect == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);

        // Check 2: Damage class indicates melee/ranged (scales with AP)
        bool isMeleeOrRanged = (spellInfo->DmgClass == SPELL_DAMAGE_CLASS_MELEE ||
                               spellInfo->DmgClass == SPELL_DAMAGE_CLASS_RANGED);

        // Check 3: Requires weapon equipment
        bool requiresWeapon = (spellInfo->EquippedItemClass == ITEM_CLASS_WEAPON);

        // Check 4: Specific spell families known to scale with AP
        bool isKnownAPSpell = false;
        if (spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE) {
            // Rogue poisons and weapon-based abilities
            isKnownAPSpell = (spellInfo->SpellFamilyFlags[0] & 0x10000) || // Deadly Poison flag
                            (spellInfo->SpellFamilyFlags[1] & 0x80000);   // Other poison flags
        }

        // Return true if any indicator suggests AP scaling
        return (isWeaponEffect || isMeleeOrRanged || requiresWeapon || isKnownAPSpell);
    }

    /**
     * @brief This functions processes spell damage for DOTs and Direct Damage Spells it
     * handles special cases for Melee scaling spells and AP scaling spells also so they
     * are not scaled up twice and murder all my friends
     *
     * @tparam DamageType
     * @param target
     * @param attacker
     * @param damage
     * @param spellInfo
     * @param eventType
     * @param logPrefix
     */

    template<typename DamageType>
    void ProcessSpellDamage(Unit* target, Unit* attacker, DamageType& damage, SpellInfo const* spellInfo, MythicPlus::MP_UNIT_EVENT_TYPE eventType, const std::string& logPrefix) {
        if(damage == 0) {
            return;
        }

        // Check if attacker is a GameObject (traps, totems, environmental hazards)
        if (!attacker || !attacker->ToCreature()) {
            MpLogger::debug("SPELL SCALING: Attacker is not a creature (likely GameObject or null), skipping CalcValue reversal");
            // Apply Mythic+ scaling to original damage without CalcValue reversal
            damage = static_cast<DamageType>(modifyIncomingDmgHeal(eventType, target, attacker, static_cast<uint32>(damage), spellInfo));
            return;
        }

        // Debug: Log spell effects to understand what we're dealing with
        // if (spellInfo->Effects.size() > 0) {
        //     auto mainEffect = spellInfo->Effects[0];

        //     MpLogger::debug("{}: Incoming Damage: {} Spell {} (ID: {}) Family: {} has RealPointsPerLevel: {} and BasePoints: {} and DieSides: {} and School: {}", logPrefix,
        //         damage,
        //         spellInfo->SpellName[0],
        //         spellInfo->Id,

        //         mainEffect.RealPointsPerLevel,
        //         mainEffect.BasePoints,
        //         mainEffect.DieSides,
        //         spellInfo->SchoolMask);
        // } else {
        //     MpLogger::debug("{}: Incoming Damage: {} Spell {} (ID: {}) has no effects and School: {}", logPrefix,
        //         damage,
        //         spellInfo->SpellName[0],
        //         spellInfo->Id,
        //         spellInfo->SchoolMask);

        //     return;
        // }

        Creature* creatureCaster = attacker->ToCreature();
        MpCreatureData* creatureData = sMpDataStore->GetCreatureData(creatureCaster->GetGUID());

        if (!creatureCaster) {
            MpLogger::error("Creature caster is null in map {}", attacker ? attacker->GetMap()->GetId() : 0);
            return;
        }
        if (!creatureData) {
            MpLogger::error("Failed to get creature data for {} in map {}", creatureCaster->GetName(), attacker ? attacker->GetMap()->GetId() : 0);
            return;
        }

        // Check if this spell scales with Attack Power
        if (IsAttackPowerScalingSpell(spellInfo)) {

            // need another special case here to determine if a spell was not scaled up by AP meaning the incoming damage is close to the
            // original effect of the spell and therefore should instead use spell effect scaling it should be no more than 15% of the original effect
            bool notScaledByAP = false;
            if (spellInfo && !spellInfo->Effects.empty()) {
                int32 baseEffect = spellInfo->Effects[0].CalcValue(attacker, nullptr, nullptr);
                if (damage <= (baseEffect * 1.15f)) {
                    MpLogger::debug(">>>> MELEE SPELL SCALING: Spell {} (ID: {}) is not scaled by AP damage: {} vs originalEffect: {}", 
                        spellInfo->SpellName[0], spellInfo->Id, damage, baseEffect);
                    notScaledByAP = true;
                }
            } else {
                // If we can't determine the base effect, default to treating it as not AP-scaled
                notScaledByAP = true;
                MpLogger::debug(">>>> MELEE SPELL SCALING: Could not determine base effect for spell, defaulting to spell scaling");
            }

            // if the effect type of the spell is not physical (aka not mitigated by armor/defense) then it needs to instead have the typical
            // spell damage multiplier applied instead of melee damage scaling
            if (! notScaledByAP && (spellInfo->SchoolMask == SPELL_SCHOOL_NORMAL || spellInfo->SchoolMask == SPELL_SCHOOL_MASK_NORMAL)) {
                uint32 meleeDamage = static_cast<uint32>(std::max(0, static_cast<int32>(damage)));

                damage = modifyIncomingDmgHeal(MythicPlus::UNIT_EVENT_MELEE, target, attacker, meleeDamage);

                MpLogger::debug(">>MELEE SPELL SCALING: {} hits with spell: {} ID: {} meleeDamage: {} damage: {}", attacker->GetName(), spellInfo->SpellName[0], spellInfo->Id, meleeDamage, damage);
            } else {

                // get the creatures original attack power
                SpellEffectInfo const& effect = spellInfo->Effects[0];
                uint32 spellDmg = static_cast<uint32>(effect.CalcValue(attacker, nullptr, nullptr) * effect.CalcDamageMultiplier(attacker, nullptr));

                // now take the original attack power * 0.08 and add it to the spell damage
                uint32 apDmg = static_cast<uint32>(creatureData->originalStats->AttackPower * 0.10f);
                uint32 finalDmg = spellDmg + apDmg;

                MpLogger::debug(">> AP BASED DAMAGE Scaledown: origDamage: {} | spellDmg: {} | apDmg: {} | finalDmg: {}", static_cast<int32>(damage), spellDmg, apDmg, finalDmg);

                damage = modifyIncomingDmgHeal(MythicPlus::UNIT_EVENT_SPELL, target, attacker, finalDmg, spellInfo);

                // need to take into consideration if this is a stacking spell and multiply the final damage by the number of stacks
                if(spellInfo->AttributesEx3 & SPELL_ATTR3_DOT_STACKING_RULE) {

                    Aura* aura = target->GetAura(spellInfo->Id, attacker->GetGUID());
                    uint32 stacks = aura ? aura->GetStackAmount() : 1;
                    if(aura) {
                        damage *= stacks;
                    }
                }

            }
            return;
        }

        // Reverse the CalcValue scaling to get original base damage
        int32 originalDamage = static_cast<int32>(damage);

        if (creatureCaster && creatureData && creatureData->originalLevel < 70) {
            CreatureTemplate const* cInfo = creatureCaster->GetCreatureTemplate();

            // Get the scaling factors used in CalcValue
            CreatureBaseStats const* pCBS = sObjectMgr->GetCreatureBaseStats(creatureCaster->GetLevel(), creatureCaster->getClass());
            float CBSPowerCreature = pCBS->BaseDamage[cInfo->expansion];

            uint32 tempLevel = spellInfo->SpellLevel;
            if(tempLevel == 0) {
                tempLevel = creatureData->originalLevel;
            }

            CreatureBaseStats const* spellCBS = sObjectMgr->GetCreatureBaseStats(tempLevel, creatureCaster->getClass());
            float CBSPowerSpell = spellCBS->BaseDamage[cInfo->expansion];

            // MpLogger::debug("SPELL SCALING: Creature Lvl {} -> {} | Spell Lvl {} | tempLevel: {} | CBSPowerCreature: {} CBSPowerSpell: {}",
            //                creatureData->originalLevel, creatureCaster->GetLevel(), tempLevel, CBSPowerCreature, CBSPowerSpell);

            // Reverse the CalcValue scaling: originalDamage = scaledDamage / (CBSPowerCreature / CBSPowerSpell)
            if (CBSPowerCreature > 0.0f) {
                originalDamage = static_cast<int32>(static_cast<int32>(damage) * (CBSPowerSpell / CBSPowerCreature));
            //     MpLogger::debug("SPELL SCALING: Reversed CalcValue scaling - Scaled: {} -> Original: {} (Factor: {:.2f})",
            //                    damage, originalDamage, CBSPowerSpell / CBSPowerCreature);
            }
        }

        // Apply Mythic+ scaling to the original base damage
        damage = static_cast<DamageType>(modifyIncomingDmgHeal(eventType, target, attacker, static_cast<uint32>(originalDamage), spellInfo));
    }

public:

    void ModifyPeriodicDamageAurasTick(Unit* target, Unit* attacker, uint32& damage, SpellInfo const* spellInfo) override {
        if (!target && !attacker) {
            return;
        }

        Map *map = target->GetMap();
        if(!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        auto effects = spellInfo->Effects;
        bool isHot = false;
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i) {
            switch(effects[i].Effect) {
                case SPELL_EFFECT_HEAL:
                case SPELL_EFFECT_HEAL_MAX_HEALTH:
                case SPELL_EFFECT_HEAL_MECHANICAL:
                case SPELL_EFFECT_HEAL_PCT:
                case SPELL_EFFECT_SPIRIT_HEAL:
                    isHot = true;
                    break;
            }
        }

        if(isHot) {
            damage = modifyIncomingDmgHeal(MythicPlus::UNIT_EVENT_HOT, target, attacker, damage, spellInfo);
        } else {
            ProcessSpellDamage(target, attacker, damage, spellInfo, MythicPlus::UNIT_EVENT_DOT, "DOT DAMAGE");
        }
    }

    void ModifySpellDamageTaken(Unit* target, Unit* attacker, int32& damage, SpellInfo const* spellInfo) override {
        if (!target && !attacker) {

            if(spellInfo) {
                MpLogger::info("ModifySpellDamageTaken: Target and attacker are null for spell: {} ID: {}", spellInfo->SpellName[0], spellInfo->Id);
            }

            return;
        }

        Map *map = target->GetMap();
        if(!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        if(!sMythicPlus->EligibleDamageTarget(target)) {
            if(spellInfo) {
                MpLogger::info("ModifySpellDamageTaken: Target is not eligible for spell: {} ID: {}", spellInfo->SpellName[0], spellInfo->Id);
            }
            return;
        }

        MpLogger::debug("ModifySpellDamageTaken: {} hits {} with spell: {} ID: {}", attacker ? attacker->GetName() : "[null]", target ? target->GetName() : "[null]", spellInfo ? spellInfo->SpellName[0] : "[no spell]", spellInfo ? spellInfo->Id : 0);

        // Use the generic ProcessSpellDamage function
        ProcessSpellDamage(target, attacker, damage, spellInfo, MythicPlus::UNIT_EVENT_SPELL, "SPELL DAMAGE");
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
    void ModifyHealReceived(Unit* target, Unit* healer, uint32& healing, SpellInfo const* spellInfo) override {
      if (!target && !healer) {
            return;
        }

        Map *map = target->GetMap();
        if(!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        healing = modifyIncomingDmgHeal(MythicPlus::UNIT_EVENT_HEAL, target, healer, healing, spellInfo);
    }

    void OnAuraApply(Unit* unit, Aura* aura) override {
        if (!unit || !aura) {
            return;
        }

        // Only scale auras for players
        if (!unit->IsPlayer()) {
            return;
        }

        #if defined(MOD_PRESENT_NPCBOTS)
            if (unit->IsNPCBotOrPet()) {
                return;
            }
        #endif

        Map* map = unit->GetMap();
        if (!sMythicPlus->IsMapEligible(map)) {
            return;
        }

        // Get instance data for scaling factors
        MpInstanceData* instanceData = sMpDataStore->GetInstanceData(map->GetId(), map->GetInstanceId());
        if (!instanceData) {
            return;
        }

        SpellInfo const* spellInfo = aura->GetSpellInfo();
        if (!spellInfo) {
            return;
        }

        Creature* creatureCaster = aura->GetCaster()->ToCreature();
        if (!creatureCaster) {
            return;
        }

        // MpLogger::debug("Aura Apply: {} applied to {} by {} Id: {}", spellInfo->SpellName[0], unit->GetName(), aura->GetCaster()->GetName(), aura->GetId());

        // Scale aura effects based on creature type and instance difficulty
        float scaleFactor = 1.0f;

        if (creatureCaster->IsDungeonBoss() || creatureCaster->isWorldBoss()) {
            scaleFactor = instanceData->boss.spell * 0.8f; // Reduce boss aura scaling slightly
        } else {
            scaleFactor = instanceData->creature.spell * 0.7f; // Reduce normal creature aura scaling
        }

        // Apply scaling to stat modifier effects only (damage auras handled elsewhere)
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i) {
            AuraEffect* effect = aura->GetEffect(i);
            if (!effect) {
                continue;
            }

            auto amount = effect->GetAmount();
            // MpLogger::debug("Aura Effect type: {} amount: {}", effect->GetAuraType(), amount);

            // // Only scale stat modifiers and resistances (not damage/healing effects)
            // if (effect->GetAuraType() == SPELL_AURA_MOD_STAT ||
            //     effect->GetAuraType() == SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE ||
            //     effect->GetAuraType() == SPELL_AURA_MOD_RESISTANCE ||
            //     effect->GetAuraType() == SPELL_AURA_MOD_RESISTANCE_PCT ||
            //     effect->GetAuraType() == SPELL_AURA_MOD_BASE_RESISTANCE_PCT ||
            //     effect->GetAuraType() == SPELL_AURA_MOD_DAMAGE_DONE ||
            //     effect->GetAuraType() == SPELL_AURA_MOD_DAMAGE_TAKEN ||
            //     effect->GetAuraType() == SPELL_AURA_MOD_HEALING_DONE ||
            //     effect->GetAuraType() == SPELL_AURA_MOD_HEALING_PCT ||
            //     effect->GetAuraType() == SPELL_AURA_MOD_HEALING_TAKEN_PCT ||
            //     effect->GetAuraType() == SPELL_AURA_MOD_ARMOR_PENETRATION_PCT ||
            //     effect->GetAuraType() == SPELL_AURA_MOD_ATTACK_POWER ||
            //     effect->GetAuraType() == SPELL_AURA_MOD_ATTACK_POWER_PCT ||
            //     effect->GetAuraType() == SPELL_AURA_MOD_SPELL_POWER_PCT) {

            //     int32 originalAmount = effect->GetAmount();
            //     int32 newAmount = int32(originalAmount * scaleFactor);
            //     effect->ChangeAmount(newAmount);

            //     MpLogger::debug("AURA SCALING: Scaled {} stat modifier effect {} from {} to {} (factor: {:.2f})",
            //                    spellInfo->SpellName[0], i, originalAmount, newAmount, scaleFactor);
            // }
        }
    }

    uint32 modifyIncomingDmgHeal(MythicPlus::MP_UNIT_EVENT_TYPE eventType,Unit* target, Unit* attacker, uint32 damageOrHeal, SpellInfo const* spellInfo = nullptr) {
        if (!target && !attacker) {
            MpLogger::info("modifyIncomingDmgHeal: Target and attacker are null for event {}", eventType);
            return damageOrHeal;
        }

        int32 alteredDmgHeal = 0;

        Map *map = target->GetMap();
        if(!sMythicPlus->IsMapEligible(map)) {
            return damageOrHeal;
        }

        if(attacker && attacker->IsPlayer()) {
            return damageOrHeal;
        }

    #if defined(MOD_PRESENT_NPCBOTS)
        if (attacker && attacker->IsNPCBotOrPet()) {
            return damageOrHeal;
        }
    #endif

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
            case MythicPlus::UNIT_EVENT_HOT:
                eventName = "HOT";
                break;
        }

        // If the target is the enemy then increase the amount of healing by the instance data modifier for spell output.
        if(sMythicPlus->EligibleDamageTarget(target)) {
            /**
             * @TODO: Allow more granular control over the scaling of DOT, HOT, and other spell effects
             * in the future if needed
             */
            switch (eventType) {
                case MythicPlus::UNIT_EVENT_MELEE:
                    if(creature->IsDungeonBoss() || creature->isWorldBoss() || creature->GetEntry() == 23682) {
                        alteredDmgHeal = damageOrHeal * instanceData->boss.melee;
                    } else {
                        alteredDmgHeal = damageOrHeal * instanceData->creature.melee;
                    }
                    // MpLogger::debug("Incoming Melee New Damage: {}({}) {} hits {}", alteredDmgHeal, damageOrHeal, attacker->GetName(), target->GetName());

                    break;
                case MythicPlus::UNIT_EVENT_DOT:
                case MythicPlus::UNIT_EVENT_SPELL:
                    if(creature->IsDungeonBoss() || creature->isWorldBoss() || creature->GetEntry() == 23682) {
                        if(spellInfo) {
                            // MpLogger::debug("Scaling spell {} using ScaleDamageSpell() Original Damage: {} New Damage: {}", spellInfo->SpellName[0], damageOrHeal, alteredDmgHeal);
                            alteredDmgHeal = sMythicPlus->ScaleDamageSpell(spellInfo, damageOrHeal, sMpDataStore->GetCreatureData(attacker->GetGUID()), creature, target, instanceData->boss.spell);
                        } else {
                            alteredDmgHeal = damageOrHeal * instanceData->boss.spell;
                            // MpLogger::debug("Scaling spell {} using flat modifier Original Damage: {} New Damage: {}", spellInfo->SpellName[0], damageOrHeal, alteredDmgHeal);
                        }
                    } else {
                        if(spellInfo) {
                            // MpLogger::debug("Scaling spell {} using ScaleDamageSpell() Original Damage: {} New Damage: {}", spellInfo->SpellName[0], damageOrHeal, alteredDmgHeal);
                            alteredDmgHeal = sMythicPlus->ScaleDamageSpell(spellInfo, damageOrHeal, sMpDataStore->GetCreatureData(attacker->GetGUID()), creature, target, instanceData->creature.spell);
                        } else {
                            // MpLogger::debug("Scaling spell {} using flat modifier Original Damage: {} New Damage: {}", spellInfo->SpellName[0], damageOrHeal, alteredDmgHeal);
                            alteredDmgHeal = damageOrHeal * instanceData->creature.spell;
                        }
                    }

                    break;
                case MythicPlus::UNIT_EVENT_HEAL:
                case MythicPlus::UNIT_EVENT_HOT:
                    break;
            }
        }

        /**
         * @TODO: Add more granular control over the scaling of healing spells
         */
        if(sMythicPlus->EligibleHealTarget(target) && (eventType == MythicPlus::UNIT_EVENT_HEAL || eventType == MythicPlus::UNIT_EVENT_HOT)) {
            if(creature->IsDungeonBoss()) {
                if(spellInfo) {
                    alteredDmgHeal = sMythicPlus->ScaleHealSpell(spellInfo, damageOrHeal, sMpDataStore->GetCreatureData(attacker->GetGUID()), creature, attacker->ToCreature(), instanceData->boss.spell * 0.7f);
                } else {
                    alteredDmgHeal = damageOrHeal * instanceData->boss.spell * 0.7f;
                }
            } else {
                if(spellInfo) {
                    alteredDmgHeal = sMythicPlus->ScaleHealSpell(spellInfo, damageOrHeal, sMpDataStore->GetCreatureData(attacker->GetGUID()), creature, attacker->ToCreature(), instanceData->creature.spell * 0.7f);
                } else {
                    alteredDmgHeal = damageOrHeal * instanceData->creature.spell * 0.70f;
                }
            }
            MpLogger::debug("Incoming heal: {}({}) {} hits {}",
                alteredDmgHeal,
                damageOrHeal,
                attacker ? attacker->GetName() : "[null]",
                target ? target->GetName() : "[null]");
        }

        return alteredDmgHeal > 0 ? alteredDmgHeal : damageOrHeal;
    }

};

void Add_MP_UnitScripts()
{
    new MythicPlus_UnitScript();
}
