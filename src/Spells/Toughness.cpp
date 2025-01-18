// #include "CreatureScript.h"
// #include "PetDefines.h"
// #include "Player.h"
// #include "MpLogger.h"
// #include "SpellAuraEffects.h"
// #include "SpellInfo.h"
// #include "SpellMgr.h"
// #include "SpellScript.h"
// #include "SpellScriptLoader.h"
// #include "UnitAI.h"
// #include "World.h"


// class spell_mp_toughness_aura : public AuraScript
// {
//     PrepareAuraScript(spell_mp_toughness_aura);

//     void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
//     {
//         Player* player = GetTarget()->ToPlayer();
//         if (!player)
//             return;

//         MpLogger::info("Applying Advancement Toughness to Player {}", player->GetName());

//         if (Unit* caster = GetCaster())
//         {
//             if (caster->IsPlayer())
//             {
//                 Player* player = caster->ToPlayer();

//                 // Add 500 Strength
//                 player->HandleStatModifier(UNIT_MOD_STAT_STRENGTH, TOTAL_VALUE, 500, true);

//                 // Apply red glow visual effect
//                 caster->SendPlaySpellVisual(11674); // Visual ID 11674 for red glow
//             }
//         }
//     }

//     void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
//     {
//         LOG_INFO("server.worldserver", "spell_custom_red_glow_strength_aura::HandleEffectRemove");

//         if (Unit* caster = GetCaster())
//         {
//             if (caster->IsPlayer())
//             {
//                 Player* player = caster->ToPlayer();

//                 // Remove 500 Strength
//                 player->HandleStatModifier(UNIT_MOD_STAT_STRENGTH, TOTAL_VALUE, 500, false);
//             }
//         }
//     }

//     void Register() override
//     {
//         LOG_INFO("server.worldserver", "spell_custom_red_glow_strength_aura::Register");

//         OnEffectApply += AuraEffectApplyFn(spell_custom_red_glow_strength_aura::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_STAT, AURA_EFFECT_HANDLE_REAL);
//         OnEffectRemove += AuraEffectRemoveFn(spell_custom_red_glow_strength_aura::HandleEffectRemove, EFFECT_0, SPELL_AURA_MOD_STAT, AURA_EFFECT_HANDLE_REAL);
//     }
// };

// void AddSC_spell_custom_red_glow_strength_aura()
// {
//     // LOG_INFO("server.loading", "Registering spell custom_red_glow_strength_aura");
//     RegisterSpellScript(spell_custom_red_glow_strength_aura);
// }
