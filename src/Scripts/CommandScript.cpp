
#include "Chat.h"
#include "AdvancementMgr.h"
#include "MpDataStore.h"
#include "MythicPlus.h"
#include "MpDataStore.h"
#include "MpLogger.h"
#include "Player.h"
#include "ScriptMgr.h"

using namespace Acore::ChatCommands;

// make sure this is the new way to do this, i think it's the old busted shit
class MythicPlus_CommandScript : public CommandScript
{
public:
    MythicPlus_CommandScript() : CommandScript("MythicPlus_CommandScript")
    {
    }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable commandTableMain =
        {
            {"", HandleHelp, SEC_PLAYER, Console::No},
            {"status", HandleStatus, SEC_PLAYER, Console::No},
            {"set", HandleSetDifficulty, SEC_PLAYER, Console::No},
            {"disable", HandleDisable, SEC_ADMINISTRATOR, Console::Yes},
            {"enable", HandleEnable, SEC_ADMINISTRATOR, Console::Yes},
            {"rescale", HandleReScale, SEC_GAMEMASTER, Console::No},
            {"rescale all", HandleReScaleAll, SEC_GAMEMASTER, Console::No},
            {"change melee", HandleChangeMelee, SEC_ADMINISTRATOR, Console::Yes},
            {"change spell", HandleChangeSpell, SEC_ADMINISTRATOR, Console::Yes},
            {"change health", HandleChangeHealth, SEC_ADMINISTRATOR, Console::Yes}
        };

        static ChatCommandTable commandTable =
        {
            {"mp", commandTableMain},
            {"mythicplus", commandTableMain},
            {"mp debug", HandleDebug, SEC_PLAYER, Console::No},
            {"mp reload", HandleReload, SEC_GAMEMASTER, Console::No},
            {"advancement", HandleAdvancement, SEC_PLAYER, Console::No}
        };

        return commandTable;
    }

    static bool HandleHelp(ChatHandler* handler, const std::vector<std::string>& /*args*/)
    {
        std::string helpText = "Mythic+ Commands:\n"
            "  .mp status - show current global settings of Mythic+ mod\n"
            "  .mp set [normal, heroic, mythic,legendary,ascendant] - Set Mythic+ difficulty in current beta only supports mythic.\n"
            "  .mp [enable,disable] - enable or disable this mod\n"
            "  .mp - Show this help message\n";
        handler->PSendSysMessage(helpText);
        return true;
    }

    static bool HandleReload(ChatHandler* handler)
    {
        sMpDataStore->LoadScaleFactors();
        handler->PSendSysMessage("Mythic+ scale factors updated.");

        return true;
    }

    static bool HandleDebug(ChatHandler* handler)
    {

        Creature* target = handler->getSelectedCreature();
        if(!target) {
            handler->PSendSysMessage("You must select a creature to debug.");
            return true;
        }

        CreatureTemplate const* creatureTemplate = target->GetCreatureTemplate();
        MpCreatureData* creatureData = sMpDataStore->GetCreatureData(target->GetGUID());


        handler->PSendSysMessage(LANG_NPCINFO_LEVEL, target->GetLevel());
        handler->PSendSysMessage(LANG_NPCINFO_HEALTH, target->GetCreateHealth(), target->GetMaxHealth(), target->GetHealth());
        handler->PSendSysMessage("WeaponDmg Main {} - {}",
            target->GetWeaponDamageRange(BASE_ATTACK, MINDAMAGE),
            target->GetWeaponDamageRange(BASE_ATTACK, MAXDAMAGE)
        );
        handler->PSendSysMessage("WeaponDmg Range {} - {}",
            target->GetWeaponDamageRange(RANGED_ATTACK, MINDAMAGE),
            target->GetWeaponDamageRange(RANGED_ATTACK, MAXDAMAGE)
        );
        handler->PSendSysMessage("WeaponDmg Offhand {} - {}",
            target->GetWeaponDamageRange(OFF_ATTACK, MINDAMAGE),
            target->GetWeaponDamageRange(OFF_ATTACK, MAXDAMAGE)
        );
        handler->PSendSysMessage("Attack Power Main {}", target->GetModifierValue(UNIT_MOD_ATTACK_POWER, BASE_VALUE));
        handler->PSendSysMessage("Attack Power Ranged {}", target->GetModifierValue(UNIT_MOD_ATTACK_POWER_RANGED, BASE_VALUE));
        handler->PSendSysMessage("Armor {}", target->GetArmor());
        handler->PSendSysMessage("Damage Modifier on template {}",creatureTemplate->DamageModifier);

        if(creatureData) {
            handler->PSendSysMessage("CreatureData: {}", creatureData->ToString());
        }

        return true;

    }

    // sets the difficluty for the group
    static bool HandleSetDifficulty(ChatHandler* handler, const std::vector<std::string>& args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        Group* group = player->GetGroup();

        if (!group) {
            MpLogger::debug("HandleSetMythic() No Group for player: {}", player->GetName());
            handler->PSendSysMessage("|cFFFF0000 You must be in a group to be able to set a Mythic+ difficulty.");
            return true;
        }

        if (args.empty()) {
            handler->PSendSysMessage("|cFFFF0000 You must specify a difficulty level. Expected values are 'mythic', 'legendary', or 'ascendant'.");
            return true;
        }

        std::string difficulty = args[0];
        // if(!sMythicPlus->IsDifficultyEnabled(difficulty)) {
        //     handler->PSendSysMessage("|cFFFF0000 The difficulty level you have selected is not enabled.");
        //     return true;
        // }

        if (!group->IsLeader(player->GetGUID())) {
            handler->PSendSysMessage("|cFFFF0000 You must be the group leader to set a Mythic+ difficulty.");
            return true;
        }

        if (player->GetMap()->IsDungeon()) {
            player->ResetInstances(player->GetGUID(), INSTANCE_RESET_CHANGE_DIFFICULTY, false);
            player->SendResetInstanceSuccess(player->GetMap()->GetId());
            return true;
        }

        if (difficulty == "mythic") {
            sMpDataStore->AddGroupData(group, MpGroupData(group, MP_DIFFICULTY_MYTHIC));
        }
        else if (difficulty == "legendary") {
            sMpDataStore->AddGroupData(group,MpGroupData(group, MP_DIFFICULTY_LEGENDARY));
        }
        else if (difficulty == "ascendant") {
            sMpDataStore->AddGroupData(group, MpGroupData(group, MP_DIFFICULTY_ASCENDANT));
        }
        else if (difficulty == "heroic") {
            sMpDataStore->RemoveGroupData(group);
            group->SetDungeonDifficulty(DUNGEON_DIFFICULTY_HEROIC);
        }
        else if (difficulty == "normal") {
            sMpDataStore->RemoveGroupData(group);
            group->SetDungeonDifficulty(DUNGEON_DIFFICULTY_NORMAL);
        }
        else {
            handler->PSendSysMessage("|cFFFF0000 Invalid difficulty level. Expected values are 'mythic', 'legendary', or 'ascendant'.");
            return true;
        }


        handler->PSendSysMessage("Mythic+ difficulty set to: " + difficulty);
        return true;
    }

    static bool HandleMythic(ChatHandler* handler, const std::vector<std::string>& /*args*/)
    {
        return HandleSetDifficulty(handler, {"mythic"});
    }

    static bool HandleLegendary(ChatHandler* handler, const std::vector<std::string>& /*args*/)
    {
        return HandleSetDifficulty(handler, {"legendary"});
    }

    static bool HandleAscendant(ChatHandler* handler, const std::vector<std::string>& /*args*/)
    {
        return HandleSetDifficulty(handler, {"ascendant"});
    }

    static bool HandleStatus(ChatHandler* handler)
    {
        Player* player = handler->GetPlayer();

        Map* map = player->GetMap();
        uint32 mapId = player->GetMapId();

        std::string status = Acore::StringFormat("Mythic+ Status:\n Mythic+ Enabled: {}\n Mythic+ Item Rewards: {}\n Mythic+ DeathLimits: {}\n",
            std::string((sMythicPlus->Enabled) ? "Yes" : "No"),
            std::string((sMythicPlus->EnableItemRewards) ? "Yes" : "No"),
            std::string((sMythicPlus->EnableDeathLimits) ? "Yes" : "No")
        );

        if (player->GetGroup()) {
            auto groupData = sMpDataStore->GetGroupData(player->GetGroup()->GetGUID());
            if (groupData) {
                MpScaleFactor scaleFactors;

                if(map->IsDungeon()) {
                    scaleFactors = sMpDataStore->GetScaleFactor(mapId, groupData->difficulty);
                }

                status += Acore::StringFormat("  Group Difficulty: {}\n Group Deaths: {}\n  Scale FactorStr {}\n",
                    (groupData->difficulty) ? groupData->difficulty : 0,
                    (groupData->GetDeaths(player->GetMapId(), player->GetInstanceId())),
                    scaleFactors.ToString()
                );
            } else {
                status += "  Group Difficulty: Not Set\n";
            }
        }

        handler->PSendSysMessage(status);
        return true;
    }

    static bool HandleReScale(ChatHandler* handler)
    {
        Creature* creature = handler->getSelectedCreature();
        if(!creature) {
            handler->PSendSysMessage("You must select a creature to rescale.");
            return true;
        }

        MpCreatureData* creatureData = sMpDataStore->GetCreatureData(creature->GetGUID());
        if(!creatureData) {
            handler->PSendSysMessage("Creature is not eligible for rescaling.");
            return true;
        }

        auto instanceData = sMpDataStore->GetInstanceData(creature->GetMapId(), creature->GetInstanceId());
        if(!instanceData) {
            handler->PSendSysMessage("No instance data found for this creature.");
            return true;
        }

        if(creature->IsDungeonBoss() || creature->GetEntry() == 23682) {
            sMythicPlus->ScaleCreature(creature->GetLevel(), creature, &instanceData->boss, instanceData->difficulty);
        } else {
            sMythicPlus->ScaleCreature(creature->GetLevel(), creature, &instanceData->creature, instanceData->difficulty);
        }

        handler->PSendSysMessage("Creature rescaled: %s", creature->GetName());

        return true;
    }

    static bool HandleReScaleAll(ChatHandler* handler)
    {
        Player* player = handler->GetPlayer();
        if(!player) {
            handler->PSendSysMessage("You must be a player to rescale all creatures.");
            return true;
        }

        Map* map = player->GetMap();
        if(!map) {
            handler->PSendSysMessage("You must be in a map to rescale all creatures.");
            return true;
        }

        int32 mapId = map->GetId();
        int32 instanceId = map->GetInstanceId();

        auto instanceData = sMpDataStore->GetInstanceData(mapId, instanceId);
        if(!instanceData) {
            handler->PSendSysMessage("No mythic instance data found for this map.");
            return true;
        }

        sMythicPlus->ScaleAll(player, instanceData);
        handler->PSendSysMessage("All creatures rescaled.");

        return true;
    }

    static bool HandleDisable(ChatHandler* handler)
    {
        MpLogger::debug("HandleDisable()");
        sMythicPlus->Enabled = false;
        handler->SendSysMessage("Mythic+ mod has been disabled.");
        return true;
    }

    static bool HandleEnable(ChatHandler* handler)
    {
        MpLogger::debug("HandleEnable()");
        sMythicPlus->Enabled = false;
        handler->SendSysMessage("Mythic+ mod has been enabled.");
        return true;
    }

    static bool HandleChangeMelee(ChatHandler* handler,  const std::vector<std::string>& args)
    {
        if (args.empty()) {
            handler->PSendSysMessage("|cFFFF0000 You must specify a value to set the melee scale factor.");
            return true;
        }

        Player* player = handler->GetSession()->GetPlayer();
        if (!player) {
            handler->PSendSysMessage("|cFFFF0000 Invalid session or player.");
            return true;
        }


        if (player->GetGroup()) {
            auto groupData = sMpDataStore->GetGroupData(player->GetGroup()->GetGUID());

            if(groupData) {
                uint32 value = std::stoi(args[0]);
                sMpDataStore->SetDamageScaleFactor(player->GetMapId(), groupData->difficulty, value);
                handler->PSendSysMessage(Acore::StringFormat("Melee scale factor set to: {}", value));
                return true;
            }
        }

        handler->PSendSysMessage("|cFFFF0000 You must be in a group and mythic+ instance to set a melee scale factor.");
        return true;
    }

    static bool HandleChangeSpell(ChatHandler* handler,  const std::vector<std::string>& args)
    {
        if (args.empty()) {
            handler->PSendSysMessage("|cFFFF0000 You must specify a value to set the spell scale factor.");
            return true;
        }

        Player* player = handler->GetSession()->GetPlayer();
        if (!player) {
            handler->PSendSysMessage("|cFFFF0000 Invalid session or player.");
            return true;
        }

        if (player->GetGroup()) {
            auto groupData = sMpDataStore->GetGroupData(player->GetGroup()->GetGUID());

            if(groupData) {
                uint32 value = std::stoi(args[0]);
                sMpDataStore->SetSpellScaleFactor(player->GetMapId(), groupData->difficulty, value);
                handler->PSendSysMessage(Acore::StringFormat("Spell scale factor set to: {}", value));
                return true;
            }
        }

        handler->PSendSysMessage("|cFFFF0000 You must be in a group and mythic+ instance to set a melee scale factor.");
        return true;
    }

    static bool HandleChangeHealth(ChatHandler* handler,  const std::vector<std::string>& args)
    {
        if (args.empty()) {
            handler->PSendSysMessage("|cFFFF0000 You must specify a value to set the health scale factor.");
            return true;
        }

        Player* player = handler->GetSession()->GetPlayer();
        if (!player) {
            handler->PSendSysMessage("|cFFFF0000 Invalid session or player.");
            return true;
        }

        if (player->GetGroup()) {
            auto groupData = sMpDataStore->GetGroupData(player->GetGroup()->GetGUID());

            if(groupData) {
                uint32 value = std::stoi(args[0]);
                sMpDataStore->SetHealthScaleFactor(player->GetMapId(), groupData->difficulty, value);
                handler->PSendSysMessage(Acore::StringFormat("Health scale factor set to: {}", value));
                return true;
            }
        }

        handler->PSendSysMessage("|cFFFF0000 You must be in a group and mythic+ instance to set a melee scale factor.");
        return true;
    }

    static bool HandleAdvancement(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();
        std::string message = "";

        for(int i =0; i < MpAdvancements::MP_ADV_MAX; i++) {
            MpPlayerRank* playerRank = sAdvancementMgr->GetPlayerAdvancementRank(player, static_cast<MpAdvancements>(i));
            if(!playerRank) {
                continue;
            }

            message += Acore::StringFormat("Your Advancement Bonuses: \n {}: {} bonus: {}", MpAdvancementsToString(static_cast<MpAdvancements>(i)), playerRank->rank, playerRank->bonus);
        }

        if(message.empty()) {
            message = "You have no advancements.";
        }

        handler->PSendSysMessage(message);

        return true;
    }

};

void Add_MP_CommandScripts()
{
    MpLogger::debug("Add_MP_CommandScripts()");
    new MythicPlus_CommandScript();
}
