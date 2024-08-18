#include "Chat.h"
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
        static ChatCommandTable commandTableSet =
        {
            {"mythic", HandleSet, SEC_PLAYER, Console::No},
            {"", HandleSet, SEC_PLAYER, Console::No},
        };

        static ChatCommandTable commandTableMain =
        {
            {"status", HandleStatus, SEC_PLAYER, Console::Yes},
            {"set", commandTableSet}
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

    static bool HandleSet(ChatHandler* handler, const std::vector<std::string>& args)
    {
        MpLogger::debug("HandleSet()");
        MpDataStore* mpds = MpDataStore::getInstance();
        const PlayerData* players = mpds->GetPlayerData();

        Player* player = handler->GetSession()->GetPlayer();
        MpLogger::debug("HandleSet() player: {}", player->GetName());

        mpds->SetPlayerDifficulty(player->GetGUID(), MP_DIFFICULTY_MYTHIC);

        handler->SendSysMessage("Hello World from MythicPlus! ({})", args.size());
        return true;
    }

    static bool HandleStatus(ChatHandler* handler)
    {
        MpLogger::debug("HandleStatus()");
        MpDataStore* mpds = MpDataStore::getInstance();
        const PlayerData* players = mpds->GetPlayerData();


        handler->SendSysMessage("Hello World from MythicPlus! ()");
        return true;
    }

};

void Add_MP_CommandScripts()
{
    MpLogger::debug("Add_MP_CommandScripts()");
    new MythicPlus_CommandScript();
}
