
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
        static ChatCommandTable commandTableSet =
        {
            {"mythic", HandleSetMythic, SEC_PLAYER, Console::No},
            {"3", HandleSetMythic, SEC_PLAYER, Console::No},
        };

        static ChatCommandTable commandTableMain =
        {
            {"", HandleHelp, SEC_PLAYER, Console::No},
            {"status", HandleStatus, SEC_PLAYER, Console::Yes},
            {"mythic",HandleSetMythic, SEC_PLAYER, Console::No},
            {"set", commandTableSet},
        };

        static ChatCommandTable commandTable =
        {
            {"mp", commandTableMain},
            {"mythicplus", HandleConsoleCommand, SEC_CONSOLE, Console::Yes},
        };


        return commandTable;
    }

    static bool HandleConsoleCommand(ChatHandler* handler, const std::vector<std::string>& args)
    {
        handler->SendSysMessage("Hello Console from MythicPlus! ({})", args.size());
        return true;
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

    static bool HandleSetMythic(ChatHandler* handler, const std::vector<std::string>& /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!player->GetGroup()) {
            MpLogger::debug("HandleSetMythic() No Group for player: {}", player->GetName());
            handler->PSendSysMessage("You must be in a group to be able to set a Mythic+ difficulty.");
            return true;
        }

        MpLogger::debug("HandleSetMythic() Set difficulty player: {}", player->GetName());



        return true;
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

};

void Add_MP_CommandScripts()
{
    MpLogger::debug("Add_MP_CommandScripts()");
    new MythicPlus_CommandScript();
}
