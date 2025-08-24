#ifndef MP_CONSTANTS_H
#define MP_CONSTANTS_H

#include "AdvancementMgr.h"

namespace MpConstants
{
    // Spell IDs for passive stat and resist bonuses
    constexpr int TITANS_STRENGTH_AURA     = 80000001;  // strength
    constexpr int STEEL_FORGED_AURA        = 80000002;  // stamina
    constexpr int CELESTIAL_GRACE_AURA     = 80000003;  // spirit
    constexpr int FORBIDDEN_KNOWLEDGE_AURA = 80000004;  // intellect
    constexpr int SPECTRAL_REFLEXES_AURA   = 80000005;  // agility
    constexpr int ELDRITCH_BARRIER_AURA    = 80000006;  // arcane resistance
    constexpr int HELLFIRE_SHIELDING_AURA  = 80000007;  // fire resistance
    constexpr int PRIMAL_ENDURACE_AURA     = 80000008;  // nature resistance
    constexpr int LICHS_BANE_AURA          = 80000009;  // shadow resistance
    constexpr int GLACIAL_FORTRESS_AURA    = 80000010;  // frost resistance

    /**
     * @brief Adds a static method for looking up the corect advancement aura based on the advancement type.
     *
     * @param advancement
     * @return int
     */
    static int GetAdvancementAura(MpAdvancements advancement)
    {
        switch (advancement)
        {
            case MpAdvancements::MP_ADV_INTELLECT:
                return MpConstants::FORBIDDEN_KNOWLEDGE_AURA;
            case MpAdvancements::MP_ADV_SPIRIT:
                return MpConstants::CELESTIAL_GRACE_AURA;
            case MpAdvancements::MP_ADV_STRENGTH:
                return MpConstants::TITANS_STRENGTH_AURA;
            case MpAdvancements::MP_ADV_AGILITY:
                return MpConstants::SPECTRAL_REFLEXES_AURA;
            case MpAdvancements::MP_ADV_STAMINA:
                return MpConstants::STEEL_FORGED_AURA;
            case MpAdvancements::MP_ADV_RESIST_ARCANE:
                return MpConstants::ELDRITCH_BARRIER_AURA;
            case MpAdvancements::MP_ADV_RESIST_FIRE:
                return MpConstants::HELLFIRE_SHIELDING_AURA;
            case MpAdvancements::MP_ADV_RESIST_NATURE:
                return MpConstants::PRIMAL_ENDURACE_AURA;
            case MpAdvancements::MP_ADV_RESIST_FROST:
                return MpConstants::GLACIAL_FORTRESS_AURA;
            case MpAdvancements::MP_ADV_RESIST_SHADOW:
                return MpConstants::LICHS_BANE_AURA;
            default:
                return 0;
        }
    }

    // Spells used for learning how to make items used for advancement.
    constexpr int SPELL_ORE_FUSION            = 150000;
    constexpr int SPELL_CLOTH_FUSION          = 150001;
    constexpr int SPELL_LEATHER_FUSION        = 150002;
    constexpr int SPELL_ALCHEMY_FUSION        = 150003;
    constexpr int SPELL_GEM_FUSION            = 150004;
    constexpr int SPELL_ESSENCE_FUSION        = 150005;
    constexpr int SPELL_COLD_FUSION           = 150006;
    constexpr int SPELL_FLAME_FUSION          = 150007;
    constexpr int SPELL_ARCANE_FUSION         = 150008;
    constexpr int SPELL_DARK_FUSION           = 150009;
    constexpr int SPELL_EARTH_FUSION          = 150010;
    constexpr int SPELL_ORE_FUSION_RANK_2     = 150011;
    constexpr int SPELL_CLOTH_FUSION_RANK_2   = 150012;
    constexpr int SPELL_LEATHER_FUSION_RANK_2 = 150013;
    constexpr int SPELL_ALCHEMY_FUSION_RANK_2 = 150014;
    constexpr int SPELL_GEM_FUSION_RANK_2     = 150015;
    constexpr int SPELL_ESSENCE_FUSION_RANK_2 = 150016;
    constexpr int SPELL_COLD_FUSION_RANK_2    = 150017;
    constexpr int SPELL_FLAME_FUSION_RANK_2   = 150018;
    constexpr int SPELL_ARCANE_FUSION_RANK_2  = 150019;
    constexpr int SPELL_DARK_FUSION_RANK_2    = 150020;
    constexpr int SPELL_EARTH_FUSION_RANK_2   = 150021;

    // New dropping unique items for mythic plus
    constexpr int ANCIENT_DICE = 911000;
    constexpr int DARK_SPIKE   = 911001;
    constexpr int VEILSTONE    = 911002;

    // Item IDs for fused materials used in advancement crafting
    constexpr int FUSED_RARE_ORE              = 911003;
    constexpr int FUSED_MYTHIC_ORE            = 911004;
    constexpr int FUSED_RARE_CLOTH            = 911005;
    constexpr int FUSED_MYTHIC_CLOTH          = 911006;
    constexpr int FUSED_RARE_LEATHER          = 911007;
    constexpr int FUSED_MYTHIC_LEATHER        = 911008;
    constexpr int FUSED_RARE_ALCHEMY          = 911009;
    constexpr int FUSED_MYTHIC_ALCHEMY        = 911010;
    constexpr int FUSED_RARE_GEM              = 911011;
    constexpr int FUSED_MYTHIC_GEM            = 911012;
    constexpr int FUSED_RARE_ESSENCE          = 911013;
    constexpr int FUSED_MYTHIC_ESSENCE        = 911014;
    constexpr int FUSED_RARE_ICE_STONE        = 911015;
    constexpr int FUSED_MYTHIC_ICE_STONE      = 911016;
    constexpr int FUSED_RARE_INFERNAL_STONE   = 911017;
    constexpr int FUSED_MYTHIC_INFERNAL_STONE = 911018;
    constexpr int FUSED_RARE_ARCANE_CRYSTAL   = 911019;
    constexpr int FUSED_MYTHIC_ARCANE_CRYSTAL = 911020;
    constexpr int FUSED_RARE_DARK_CRYSTAL     = 911021;
    constexpr int FUSED_MYTHIC_DARK_CRYSTAL   = 911022;
    constexpr int FUSED_RARE_EARTH_STONE      = 911023;
    constexpr int FUSED_MYTHIC_EARTH_STONE    = 911024;

    // Shadowy Remains
    constexpr int SHADOWY_REMAINS = 911100;
}

#endif
