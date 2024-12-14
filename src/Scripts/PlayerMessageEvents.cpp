#include "MpLogger.h"
#include "MpDataStore.h"
#include "MpScheduler.h"
#include "MythicPlus.h"
#include "Player.h"
#include "Group.h"
#include "ScriptMgr.h"
#include "TaskScheduler.h"
#include "AdvancementMgr.cpp"
#include "Chat.h"
#include "Channel.h"
#include "ChannelMgr.h"

#include <vector>
#include <string>
#include <ranges>

/**
 * This script file is a special event handler attached to the chat channel for MythicPlus to intercept
 * message events from the client hidden chat channel and process them, as well as return events back to
 * the client.  It's a very simplified version of how Eluna / AIO manage messages from UI to C++ mods.
 *
 * All Messages come into a chat channel from a specific user on a hidden channel with details in the MythicPlus.h
 * class for the protocol data definition.
 */

class MythicPlus_PlayerMessageEvents : public PlayerScript
{

    /**
     * Listen to all messages from a playerand process them if
     */
     void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Channel* channel) override
    {
        if(!player || !channel) {
            return;
        }

        // Handle chat message coming in to the data channel from the client. This allows backedend calls into the module
        // from Eluna / AIO without the need to modify mod-eluna directly.
        if(type == CHAT_MSG_CHANNEL && MP_DATA_CHAT_CHANNEL == channel->GetName()) {
            MpLogger::info("Player {} sent a message {} to channel {}", player->GetName(), msg, channel->GetName());
        }
    }

    /**
     * When a player logs in add them to the data channel specifically for Mythic+ communication
     * between UI and server module.
     *
     * Load advancement data for the player at load time used to apply buffs.
     */
    void OnLogin(Player* player) override
    {
        if(!player) {
            return;
        }

        // Create a channel called MpEx if it does not exist
        ChannelMgr* cmg = ChannelMgr::forTeam(TEAM_NEUTRAL);
        Channel* channel = cmg->GetChannel(static_cast<std::string>(MP_DATA_CHAT_CHANNEL), player);

        // If the channel does not yet, exist the first player to login to the server will ensure it is created.
        if(!channel) {
            channel = cmg->GetJoinChannel(static_cast<std::string>(MP_DATA_CHAT_CHANNEL),0);
        }

    }
};

// Split the string passed in by delimiters
std::vector<std::string> splitString(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = s.find(delimiter);

    while (end != std::string::npos) {
        if (end != start) {
            tokens.emplace_back(s.substr(start, end - start));
        }
        start = end + 1;
        end = s.find(delimiter, start);
    }

    // Add the last token if it's not empty
    if (start < s.length()) {
        tokens.emplace_back(s.substr(start));
    }

    return tokens;
}

/**
 * Parse the incoming message into its parts:
 * - p:playerGuid:action:input1:input2:input3...
 * i.e) p:5793:UpgradeAdvancement:0:10:2
 */
void parsePlayerMessage(Player* player, std::string& msg)
{
    if(!player) {
        MpLogger::error("Null player passed to parsePlayerMessage");
        return;
    }

    if(msg[0] != 'p') {
        MpLogger::warn("Invalid player message format received from player {} message: {}", player->GetName(), msg);
        return;
    }

    std::vector<std::string> actionArgs;
    char delimiter = '|';

    // split the protocol into valid parts
    std::vector<std::string> parts = splitString(msg, delimiter);

}
