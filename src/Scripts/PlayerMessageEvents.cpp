#include "MpLogger.h"
#include "MythicPlus.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "AdvancementMgr.h"
#include "Chat.h"
#include "Channel.h"
#include "ChannelMgr.h"
#include "MpEventProcessor.h"
#include <boost/algorithm/string/predicate.hpp> // For starts_with

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
public:
    MythicPlus_PlayerMessageEvents() : PlayerScript("MythicPlus_PlayerMessageEvents") {}

    /**
     * Listens to AddOn Chat channel for Mythic+ communication between UI and server mythic+ functionality
     */
    void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Player* receiver) override
    {
        // All communication from the client should be a whisper to themselves over tha addon channel
        if(!player || !receiver) {
            return;
        }

        if(lang == LANG_ADDON) {
            if(msg.empty()) {
                MpLogger::info("Empty AddOn message received from player: {}", player->GetName());
                return;
            }

            // if the message begins with our prefix for our data channel then process the event
            if(boost::starts_with(msg, MP_DATA_CHAT_CHANNEL)) {
                sMpEventProcessor->ProcessMessage(player, msg);
            }
        }
    }

    /**
     * When a player logs in add them to the data channel specifically for Mythic+ communication
     * between UI and server module.
     *
     * Load advancement data for the player at load time used to apply buffs.
     */
    // void OnLogin(Player* player) override
    // {
    //     if(!player) {
    //         return;
    //     }

    //     // Create a channel called MpEx if it does not exist
    //     MpLogger::info("Player {} logged in on team {}", player->GetName(), player->GetTeamId());
    //     ChannelMgr* cmg = ChannelMgr::forTeam(player->GetTeamId());

    //     if(!cmg) {
    //         MpLogger::error("Failed to get channel manager for team {}", player->GetTeamId());
    //         return;
    //     }

    //     Channel* channel = cmg->GetChannel(static_cast<std::string>(MP_DATA_CHAT_CHANNEL), player);
    //     if(!channel) {
    //         MpLogger::error("Failed to get mythic data channel for player {}", player->GetName());
    //         Channel* nchan = new Channel(static_cast<std::string>(MP_DATA_CHAT_CHANNEL), 17, 0, player->GetTeamId());
    //         if(!nchan) {
    //             MpLogger::error("Failed to create mythic data channel for player {}", player->GetName());
    //             return;
    //         }
    //     }

    // }
};

void Add_MP_PlayerMessageEvents()
{
    MpLogger::debug("Add_MP_PlayerEventMessages");
    new MythicPlus_PlayerMessageEvents();
}
