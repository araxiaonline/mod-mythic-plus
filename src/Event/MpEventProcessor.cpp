
#include "MpEventProcessor.h"
#include "MythicPlus.h"
#include "MpLogger.h"
#include "Player.h"
#include <boost/algorithm/string/replace.hpp>

#include <string_view>
#include <string>
#include <vector>

bool MpEventProcessor::ProcessMessage(Player* player, const std::string& msg) {

    if(!player) {
        MpLogger::error("Null player passed to processMessage");
        return false;
    }

    // check prefix of message channel is formatted correctly
    if(! msg.starts_with(MP_DATA_CHAT_CHANNEL)) {
        MpLogger::error("Invalid message format received from player {} message: {}", player->GetName(), msg);
        return false;
    }

    std::string message = msg;

    // shift the message identifier off the front including first '|' character
    message.erase(0, MP_DATA_CHAT_CHANNEL.size()+1);

    // clean up the message before passing it to the parser
    boost::replace_all(message, "||", "|");

    EventParseRslt result = _parsePlayerMessage(player, message);
    MpEvent event = std::get<0>(result);
    uint32 guid = std::get<1>(result);
    std::vector<std::string> args = std::get<2>(result);

    MpLogger::info("MpEvent Processor - event: {} guid: {} args: {}", event, guid, args.size());

    // If th message was not able to be parsed it is a failure
    if(event == MpEvent::Invalid) {
        MpLogger::warn("Invalid event, could not be parsed for player {} message: {}", player->GetName(), message);
        return false;
    }

    // if the message is not from the same player who called it ignore it as it is attempt to hack the system
    if(player->GetGUID().GetCounter() != guid) {
        MpLogger::warn("Player {} sent a message {} for eventId: {} player guid does not match", player->GetName(), message, event);
        return false;
    }

    // If the event is not registered ignore it
    if(!_eventHandlers.contains(event)) {
        MpLogger::info("No handler registered for event: {}", event);
        return false;
    }

    return Dispatch(event, player, args);
}

void MpEventProcessor::RegisterHandler(MpEvent event, std::shared_ptr<MpEventInterface> handler) {
    _eventHandlers[event] = handler;
}

// This fires the execution to the actual event.
bool MpEventProcessor::Dispatch(MpEvent event, Player* player, std::vector<std::string>& args) {
    if(!_eventHandlers.contains(event)) {
        MpLogger::warn("No handler registered for event: {}", event);
        return false;
    }

    return _eventHandlers[event]->Execute(player, args);
}

// Find our eventId using the string name
MpEvent MpEventProcessor::_getEventByName(std::string_view eventName)
{
    auto it = MpEventMap.find(eventName);
    return (it != MpEventMap.end()) ? it->second : MpEvent::None;
}

/**
 * Parse the incoming message into id, event, and handler arguments
 */
EventParseRslt MpEventProcessor::_parsePlayerMessage(Player* player, const std::string& msg)
{
    if(msg[0] != 'p') {
        MpLogger::warn("Invalid player message format received from player {} message: {}", player->GetName(), msg);
        return EventParseRslt{MpEvent::Invalid, 0, {}};
    }

    std::vector<std::string> handlerArgs;
    char delimiter = '|';

    // split the protocol into valid parts
    std::vector<std::string> parts = _splitString(msg, delimiter);

    if (parts.size() < 3) {
        MpLogger::warn("Malformed player message received from player {}: {}", player->GetName(), msg);
        return EventParseRslt{MpEvent::Invalid, 0, {}};
    }

    MpEvent event = _getEventByName(parts[2]);

    handlerArgs.assign(parts.begin() + 3, parts.end());

    MpLogger::info("Player {} sent a client event message {} for event: {} eventId: {} ", player->GetName(), msg, MP_DATA_CHAT_CHANNEL, parts[2], event);
    return EventParseRslt{event, player->GetGUID().GetCounter(), std::move(handlerArgs)};
}

// Split the string passed in by delimiters
std::vector<std::string> MpEventProcessor::_splitString(const std::string& s, char delimiter) {
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
