
#include "Chat.h"
#include "MpDataStore.h"
#include "MythicPlus.h"
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
            "  .mp set [mythic,legendary,ascendant] - Set Mythic+ difficulty to Mythic can also used (3,4,5)\n"
            "  .mp [enable,disable] - enable or disable this mod\n"
            "  .mp - Show this help message\n";
        handler->PSendSysMessage(helpText);
        return true;
    }

    // sets the difficluty for the group
    static bool HandleSetDifficulty(ChatHandler* handler, const std::vector<std::string>& args)
    {

        Player* player = handler->GetSession()->GetPlayer();

        if (!player->GetGroup()) {
            MpLogger::debug("HandleSetMythic() No Group for player: {}", player->GetName());
            handler->PSendSysMessage("You must be in a group to be able to set a Mythic+ difficulty.");
            return true;
        }

        if (args.empty()) {
            handler->PSendSysMessage("You must specify a difficulty level. Expected values are 'mythic', 'legendary', or 'ascendant'.");
            return true;
        }

        std::string difficulty = args[0];
        if (difficulty == "mythic" || difficulty == "3") {
            sMpDataStore->AddGroupData(player->GetGroup(), 3);
        }
        else if (difficulty == "legendary" || difficulty == "4") {
            sMpDataStore->AddGroupData(player->GetGroup(), 4);
        }
        else if (difficulty == "ascendant" || difficulty == "5") {
            sMpDataStore->AddGroupData(player->GetGroup(), 5);
        }
        else {
            handler->PSendSysMessage("Invalid difficulty level. Expected values are 'mythic', 'legendary', or 'ascendant'.");
            return true;
        }

        MpLogger::debug("HandleSetMythic() Set difficulty player: {} {}", player->GetName(), difficulty);

        return true;
    }

    static bool HandleMythic(ChatHandler* handler, const std::vector<std::string>& args)
    {
        return HandleSetDifficulty(handler, std::vector<std::string>{"mythic"});
    }

    static bool HandleLegendary(ChatHandler* handler, const std::vector<std::string>& args)
    {
        return HandleSetDifficulty(handler, std::vector<std::string>{"legendary"});
    }

    static bool HandleAscendant(ChatHandler* handler, const std::vector<std::string>& args)
    {
        return HandleSetDifficulty(handler, std::vector<std::string>{"ascendant"});
    }

    static bool HandleStatus(ChatHandler* handler)
    {
        MpLogger::debug("HandleStatus()");
        std::string status = Acore::StringFormat(
            "Mythic+ Status:\n"
            "  Mythic+ Enabled: %s\n"
            "  Mythic+ Item Rewards: %s\n"
            "  Mythic+ DeathLimits: %s\n",
            sMythicPlus->Enabled ? "Yes" : "No",
            sMythicPlus->EnableItemRewards ? "Yes" : "No",
            sMythicPlus->EnableDeathLimits ? "Yes" : "No");


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
