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
    std::string message = std::to_string(player->GetGUID().GetCounter()) + "|" + eventName.data();
    for(auto& arg : args) {
        message += "|" + arg;
    }

    std::string prefix = std::string(MP_DATA_CHAT_CHANNEL);
    std::string fullmsg = prefix + "\t" + message;

    WorldPacket data(SMSG_MESSAGECHAT, 100);
    data << uint8(CHAT_MSG_ADDON);
    data << int32(LANG_ADDON);
    data << player->GetGUID().GetCounter();
    data << uint32(0);
    data << player->GetGUID().GetCounter();
    data << uint32(fullmsg.length() + 1);
    data << fullmsg;
    data << uint8(0);
    player->GetSession()->SendPacket(&data);
    return 0;

    return true;
}
