#include "MpClientDispatcher.h"
#include "MpEventProcessor.h"
#include "Player.h"
#include "MpLogger.h"
#include "Chat.h"
#include "WorldPacket.h"

#include <string_view>
#include <string>

/**
 * In order to allow communication from client UIs without modifying mod-eluna directly to support
 * This sends a packet directly using the AddOn message channel in the background.
 */
bool MpClientDispatcher::Dispatch(MpClientEvent event, Player* player, std::vector<std::string>& args)
{
    if(!MpClientEventNames.contains(event)) {
        MpLogger::warn("No event registered for event: {}", event);
        return false;
    }

    // Build the message string in same format to send to the client.
    std::string_view eventName = MpClientEventNames.at(event);
    uint32 playerGuid = player->GetGUID().GetCounter();
    std::string message = "s|" + std::to_string(playerGuid) + "|" + std::string(eventName);
    for(auto& arg : args) {
        message += "|" + arg;
    }

    std::string prefix = std::string(MP_DATA_CHAT_CHANNEL);
    std::string fullmsg = prefix + "\t" + message;

    MpLogger::debug("Dispatching client event: {} length {} for event {}", fullmsg, fullmsg.length(), std::string(eventName));

    WorldPacket data(SMSG_MESSAGECHAT, 100);
    data << uint8(ChatMsg::CHAT_MSG_WHISPER);
    data << int32(LANG_ADDON);
    data << player->GetGUID();
    data << uint32(0);
    data << player->GetGUID();
    data << uint32(fullmsg.length() + 1);
    data << fullmsg;
    data << uint8(0);

    player->GetSession()->SendPacket(&data);

    return true;
}
