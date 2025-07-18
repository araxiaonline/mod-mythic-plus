#include "CreatureScript.h"
#include "MpConstants.h"
#include "PetDefines.h"
#include "Player.h"
#include "AdvancementMgr.h"
#include "MpLogger.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include "UnitAI.h"
#include "World.h"


class spell_mp_titans_strength_aura : public AuraScript
{
    PrepareAuraScript(spell_mp_titans_strength_aura);

    void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        Player* player = GetCaster()->ToPlayer();
        auto rank = sAdvancementMgr->GetPlayerAdvancementRank(player, MpAdvancements::MP_ADV_STRENGTH);

        if(!rank) {
            amount = 0; // player does not have an advancement
            return;
        }

        amount =  static_cast<int32>(rank->bonus);
        MpLogger::info("In Calc Amount Advancement Titans Strength to Player {} bonus {}", player->GetName(), amount);
    }

    void Register() override
    {
        MpLogger::info("Registering spell_mp_titans_strength_aura");
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mp_titans_strength_aura::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_MOD_STAT);
    }
};

class spell_mp_steel_forged_aura : public AuraScript
{
    PrepareAuraScript(spell_mp_steel_forged_aura);

    void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        Player* player = GetCaster()->ToPlayer();
        auto rank = sAdvancementMgr->GetPlayerAdvancementRank(player, MpAdvancements::MP_ADV_STAMINA);

        if(!rank) {
            amount = 0; // player does not have an advancement
            return;
        }

        amount = static_cast<int32>(rank->bonus);
        MpLogger::info("In Calc Amount Advancement Steel Forged to Player {} bonus {}", player->GetName(), amount);
    }

    void Register() override
    {
        MpLogger::info("Registering spell_mp_steel_forged_aura");
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mp_steel_forged_aura::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_MOD_STAT);
    }
};

class spell_mp_celestial_grace_aura : public AuraScript
{
    PrepareAuraScript(spell_mp_celestial_grace_aura);

    void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        Player* player = GetCaster()->ToPlayer();
        auto rank = sAdvancementMgr->GetPlayerAdvancementRank(player, MpAdvancements::MP_ADV_SPIRIT);

        if(!rank) {
            amount = 0; // player does not have an advancement
            return;
        }

        amount = static_cast<int32>(rank->bonus);
        MpLogger::info("In Calc Amount Advancement Celestial Grace to Player {} bonus {}", player->GetName(), amount);
    }

    void Register() override
    {
        MpLogger::info("Registering spell_mp_celestial_grace_aura");
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mp_celestial_grace_aura::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_MOD_STAT);
    }
};

class spell_mp_forbidden_knowledge_aura : public AuraScript
{
    PrepareAuraScript(spell_mp_forbidden_knowledge_aura);

    void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        Player* player = GetCaster()->ToPlayer();
        auto rank = sAdvancementMgr->GetPlayerAdvancementRank(player, MpAdvancements::MP_ADV_INTELLECT);

        if(!rank) {
            amount = 0; // player does not have an advancement
            return;
        }

        amount = static_cast<int32>(rank->bonus);
        MpLogger::info("In Calc Amount Advancement Forbidden Knowledge to Player {} bonus {}", player->GetName(), amount);
    }

    void Register() override
    {
        MpLogger::info("Registering spell_mp_forbidden_knowledge_aura");
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mp_forbidden_knowledge_aura::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_MOD_STAT);
    }
};

class spell_mp_spectral_reflexes_aura : public AuraScript
{
    PrepareAuraScript(spell_mp_spectral_reflexes_aura);

    void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        Player* player = GetCaster()->ToPlayer();
        auto rank = sAdvancementMgr->GetPlayerAdvancementRank(player, MpAdvancements::MP_ADV_AGILITY);

        if(!rank) {
            amount = 0; // player does not have an advancement
            return;
        }

        amount = static_cast<int32>(rank->bonus);
        MpLogger::info("In Calc Amount Advancement Spectral Reflexes to Player {} bonus {}", player->GetName(), amount);
    }

    void Register() override
    {
        MpLogger::info("Registering spell_mp_spectral_reflexes_aura");
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mp_spectral_reflexes_aura::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_MOD_STAT);
    }
};

class spell_mp_eldritch_barrier_aura : public AuraScript
{
    PrepareAuraScript(spell_mp_eldritch_barrier_aura);

    void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        Player* player = GetCaster()->ToPlayer();
        auto rank = sAdvancementMgr->GetPlayerAdvancementRank(player, MpAdvancements::MP_ADV_RESIST_ARCANE);

        if(!rank) {
            amount = 0; // player does not have an advancement
            return;
        }

        amount = static_cast<int32>(rank->bonus);
        MpLogger::info("In Calc Amount Advancement Eldritch Barrier to Player {} bonus {}", player->GetName(), amount);
    }

    void Register() override
    {
        MpLogger::info("Registering spell_mp_eldritch_barrier_aura");
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mp_eldritch_barrier_aura::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_MOD_RESISTANCE);
    }
};

class spell_mp_hellfire_shielding_aura : public AuraScript
{
    PrepareAuraScript(spell_mp_hellfire_shielding_aura);

    void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        Player* player = GetCaster()->ToPlayer();
        auto rank = sAdvancementMgr->GetPlayerAdvancementRank(player, MpAdvancements::MP_ADV_RESIST_FIRE);

        if(!rank) {
            amount = 0; // player does not have an advancement
            return;
        }

        amount = static_cast<int32>(rank->bonus);
        MpLogger::info("In Calc Amount Advancement Hellfire Shielding to Player {} bonus {}", player->GetName(), amount);
    }

    void Register() override
    {
        MpLogger::info("Registering spell_mp_hellfire_shielding_aura");
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mp_hellfire_shielding_aura::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_MOD_RESISTANCE);
    }
};

class spell_mp_primal_endurance_aura : public AuraScript
{
    PrepareAuraScript(spell_mp_primal_endurance_aura);

    void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        Player* player = GetCaster()->ToPlayer();
        auto rank = sAdvancementMgr->GetPlayerAdvancementRank(player, MpAdvancements::MP_ADV_RESIST_NATURE);
        if(!rank) {
            amount = 0; // player does not have an advancement
            return;
        }

        amount = static_cast<int32>(rank->bonus);
        MpLogger::info("In Calc Amount Advancement Primal Endurance to Player {} bonus {}", player->GetName(), amount);
    }

    void Register() override
    {
        MpLogger::info("Registering spell_mp_primal_endurance_aura");
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mp_primal_endurance_aura::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_MOD_RESISTANCE);
    }
};

class spell_mp_lichs_bane_aura : public AuraScript
{
    PrepareAuraScript(spell_mp_lichs_bane_aura);

    void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        Player* player = GetCaster()->ToPlayer();
        auto rank = sAdvancementMgr->GetPlayerAdvancementRank(player, MpAdvancements::MP_ADV_RESIST_SHADOW);

        if(!rank) {
            amount = 0; // player does not have an advancement
            return;
        }

        amount = static_cast<int32>(rank->bonus);
        MpLogger::info("In Calc Amount Advancement Lich's Bane to Player {} bonus {}", player->GetName(), amount);
    }

    void Register() override
    {
        MpLogger::info("Registering spell_mp_lichs_bane_aura");
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mp_lichs_bane_aura::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_MOD_RESISTANCE);
    }
};

class spell_mp_glacial_fortress_aura : public AuraScript
{
    PrepareAuraScript(spell_mp_glacial_fortress_aura);

    void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        Player* player = GetCaster()->ToPlayer();
        auto rank = sAdvancementMgr->GetPlayerAdvancementRank(player, MpAdvancements::MP_ADV_RESIST_FROST);

        if(!rank) {
            amount = 0; // player does not have an advancement
            return;
        }

        amount = static_cast<int32>(rank->bonus);
        MpLogger::info("In Calc Amount Advancement Glacial Fortress to Player {} bonus {}", player->GetName(), amount);
    }

    void Register() override
    {
        MpLogger::info("Registering spell_mp_glacial_fortress_aura");
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mp_glacial_fortress_aura::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_MOD_RESISTANCE);
    }
};

void AddSC_AdvancementSpells()
{
    RegisterSpellScript(spell_mp_titans_strength_aura);
    RegisterSpellScript(spell_mp_steel_forged_aura);
    RegisterSpellScript(spell_mp_celestial_grace_aura);
    RegisterSpellScript(spell_mp_forbidden_knowledge_aura);
    RegisterSpellScript(spell_mp_spectral_reflexes_aura);
    RegisterSpellScript(spell_mp_eldritch_barrier_aura);
    RegisterSpellScript(spell_mp_hellfire_shielding_aura);
    RegisterSpellScript(spell_mp_primal_endurance_aura);
    RegisterSpellScript(spell_mp_lichs_bane_aura);
    RegisterSpellScript(spell_mp_glacial_fortress_aura);
}
