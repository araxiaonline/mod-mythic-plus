
#include "Chat.h"
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
            {"showstats", HandleDebug, SEC_PLAYER, Console::No},
            // {"mythic",HandleMythic, SEC_PLAYER, Console::No},
            // {"legendary",HandleLegendary, SEC_PLAYER, Console::No},
            // {"ascendant",HandleAscendant, SEC_PLAYER, Console::No},
            {"set", HandleSetDifficulty, SEC_PLAYER, Console::No},
            {"update", HandleUpdate, SEC_GAMEMASTER,Console::No },
            {"disable", HandleDisable, SEC_ADMINISTRATOR, Console::Yes},
            {"enable", HandleEnable, SEC_ADMINISTRATOR, Console::Yes}
        };

        static ChatCommandTable commandTable =
        {
            {"mp", commandTableMain},
            {"mythicplus", commandTableMain},
            {"mp debug", HandleDebug, SEC_PLAYER, Console::No}
        };

        return commandTable;
    }

    static bool HandleHelp(ChatHandler* handler, const std::vector<std::string>& /*args*/)
    {
        std::string helpText = "Mythic+ Commands:\n"
            "  .mp status - show current global settings of Mythic+ mod\n"
            "  .mp set [mythic,legendary,ascendant] - Set Mythic+ difficulty in current beta only supports mythic.\n"
            "  .mp [enable,disable] - enable or disable this mod\n"
            "  .mp - Show this help message\n";
        handler->PSendSysMessage(helpText);
        return true;
    }

    static bool HandleUpdate(ChatHandler* handler, const std::vector<std::string>& /*args*/)
    {
        sMpDataStore->LoadScaleFactors();
        handler->PSendSysMessage("Mythic+ scale factors updated.");

        return true;
    }

    static bool HandleDebug(ChatHandler* handler, const std::vector<std::string>& args)
    {

        Creature* target = handler->getSelectedCreature();
        if(!target) {
            handler->PSendSysMessage("You must select a creature to debug.");
            return true;
        }

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
        handler->PSendSysMessage(LANG_NPCINFO_ARMOR, target->GetArmor());
        handler->PSendSysMessage("Damage Modifier {}", target->GetModifierValue(UNIT_MOD_DAMAGE_MAINHAND, BASE_VALUE));



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
            sMpDataStore->AddGroupData(group, MpGroupData(group, MP_DIFFICULTY_MYTHIC, 0));
        }
        else if (difficulty == "legendary") {
            sMpDataStore->AddGroupData(group,MpGroupData(group, MP_DIFFICULTY_LEGENDARY, 0));
        }
        else if (difficulty == "ascendant") {
            sMpDataStore->AddGroupData(group, MpGroupData(group, MP_DIFFICULTY_ASCENDANT, 0));
        }
        else {
            handler->PSendSysMessage("|cFFFF0000 Invalid difficulty level. Expected values are 'mythic', 'legendary', or 'ascendant'.");
            return true;
        }


        handler->PSendSysMessage("Mythic+ difficulty set to: " + difficulty);
        MpLogger::debug("HandleSetMythic() Set difficulty player: {} {}", player->GetName(), difficulty);

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

        std::string status = Acore::StringFormat(
            "Mythic+ Status:\n"
            "  Mythic+ Enabled: %s\n"
            "  Mythic+ Item Rewards: %s\n"
            "  Mythic+ DeathLimits: %s\n",
            sMythicPlus->Enabled ? "Yes" : "No",
            sMythicPlus->EnableItemRewards ? "Yes" : "No",
            sMythicPlus->EnableDeathLimits ? "Yes" : "No");

        if (player->GetGroup()) {
            auto groupData = sMpDataStore->GetGroupData(player->GetGroup()->GetGUID());
            if (groupData) {
                status += Acore::StringFormat(
                    "  Group Difficulty: %u\n"
                    "  Group Deaths: %u\n",
                    groupData->difficulty,
                    groupData->deaths);
            } else {
                status += "  Group Difficulty: Not Set\n";
            }
        }


        handler->PSendSysMessage(status);
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

};

void Add_MP_CommandScripts()
{
    MpLogger::debug("Add_MP_CommandScripts()");
    new MythicPlus_CommandScript();
}
