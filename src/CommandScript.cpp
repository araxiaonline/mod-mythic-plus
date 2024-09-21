
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
            {"mythic",HandleMythic, SEC_PLAYER, Console::No},
            {"legendary",HandleLegendary, SEC_PLAYER, Console::No},
            {"ascendant",HandleAscendant, SEC_PLAYER, Console::No},
            {"set", HandleSetDifficulty, SEC_PLAYER, Console::No},
            {"disable", HandleDisable, SEC_ADMINISTRATOR, Console::Yes},
            {"enable", HandleEnable, SEC_ADMINISTRATOR, Console::Yes}
        };

        static ChatCommandTable commandTable =
        {
            {"mp", commandTableMain},
            {"mythicplus", commandTableMain}
        };

        return commandTable;
    }

    static bool HandleHelp(ChatHandler* handler, const std::vector<std::string>& /*args*/)
    {
        std::string helpText = "Mythic+ Commands:\n"
            "  .mp status - show current global settings of Mythic+ mod\n"
            "  .mp [mythic,legendary,ascendant] - Shortcode to set Mythic+ difficulty to level for your group. \n"
            "  .mp set [mythic,legendary,ascendant] - Set Mythic+ difficulty\n"
            "  .mp [enable,disable] - enable or disable this mod\n"
            "  .mp - Show this help message\n";
        handler->PSendSysMessage(helpText);
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
        if(sMythicPlus->IsDifficultyEnabled(difficulty)) {
            handler->PSendSysMessage("|cFFFF0000 The difficulty level you have selected is not enabled.");
            return true;
        }

        if (difficulty == "mythic") {
            sMpDataStore->AddGroupData(group, {
                .group = group,
                .difficulty = MP_DIFFICULTY_MYTHIC
            });
        }
        else if (difficulty == "legendary") {
            sMpDataStore->AddGroupData(group,{
                .group = group,
                .difficulty = MP_DIFFICULTY_LEGENDARY
            });
        }
        else if (difficulty == "ascendant") {
            sMpDataStore->AddGroupData(group, {
                .group = group,
                .difficulty = MP_DIFFICULTY_ASCENDANT
            });
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
