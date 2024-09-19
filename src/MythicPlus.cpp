#include "MythicPlus.h"
#include "MapMgr.h"

bool MythicPlus::IsMapEligible(Map* map)
{
    if (!Enabled) {
        return false;
    }

    if (map->IsDungeon()) {
        return true;
    }

    return false;
}

bool MythicPlus::IsDifficultyEnabled(std::string difficulty)
{
    return std::find(enabledDifficulties.begin(), enabledDifficulties.end(), difficulty) != enabledDifficulties.end();
}

bool MythicPlus::IsDungeonDisabled(uint32 dungeon)
{
    return std::find(disabledDungeons.begin(), disabledDungeons.end(), dungeon) != disabledDungeons.end();
}

bool MythicPlus::IsCreatureEligible(Creature* creature)
{
    if (creature->IsDungeonBoss()) {
        return true;
    }

    if ((creature->IsHunterPet() || creature->IsPet() || creature->IsSummon()) && creature->IsControlledByPlayer()) {
        return false;
    }

    if (creature->IsCritter() || creature->IsTotem() || creature->IsTrigger()) {
        return false;
    }

    # if defined(MOD_PRESENT_NPCBOTS)
        if (creature->IsNpcBot()) {
            return false;
        }
    # endif

    // throw out NPCs
    if  ((creature->IsVendor() ||
            creature->HasNpcFlag(UNIT_NPC_FLAG_GOSSIP) ||
            creature->HasNpcFlag(UNIT_NPC_FLAG_QUESTGIVER) ||
            creature->HasNpcFlag(UNIT_NPC_FLAG_TRAINER) ||
            creature->HasNpcFlag(UNIT_NPC_FLAG_TRAINER_PROFESSION) ||
            creature->HasNpcFlag(UNIT_NPC_FLAG_REPAIR) ||
            creature->HasUnitFlag(UNIT_FLAG_IMMUNE_TO_PC) ||
            creature->HasUnitFlag(UNIT_FLAG_NOT_SELECTABLE)) &&
            (!creature->IsDungeonBoss())
        )
    {
        return false;
    }

    return true;

}
