#ifndef MYTHIC_PLUS_EVENT_PROCESSOR_H
#define MYTHIC_PLUS_EVENT_PROCESSOR_H

#include "MpEvent.h"

#include <string>
#include <string_view>
#include <vector>

/**
 * Interface for all Mythic+ Event communication between client and server
 */
class MpEventInterface
{
public:
    MpEventInterface() {};
    virtual ~MpEventInterface() = default;
    [[nodiscard]] virtual bool Execute(std::vector<std::string>&) = 0;
};

using EventParseRslt = std::tuple<MpEvent, uint32_t, std::vector<std::string>>;

/**
 * This class is responsible for processing events that are send from the client. Events
 * are processed FIFO and executed immidiately. This is specifically to allow AddOn or AIO
 * communication to this module without requiring additional changes to mod-eluna or the core.
 *
 * Message Body Format
 * Client Player Events
 * - p|playerGuid|action|input1|input2|input3...
 *   i.e) p|5793|UpgradeAdvancement|0|10|2
 */
class MpEventProcessor
{

public:
    static MpEventProcessor* instance() {
        static MpEventProcessor instance;
        return &instance;
    }

    MpEventProcessor() = default;
    ~MpEventProcessor() = default;

    // /* TODO */ Add generic handler later for other events if needed.
    // bool ProcessMessage(std::string_view message);

    // Process a message from a specific player
    bool ProcessMessage(Player* player, const std::string& message);

    // Registers a handler for a valid MpEvent specified in the MpEvent enum
    // In this design Event:Handler is 1:1
    void RegisterHandler(MpEvent event, std::shared_ptr<MpEventInterface> handler);

    // Dispatch will execute the event handler
    bool Dispatch(MpEvent event, Player* player, std::vector<std::string>& args);

private:
    MpEventProcessor() {}
    ~MpEventProcessor() {}

    // List of registered handlers for event management
    std::unordered_map<MpEvent, std::shared_ptr<MpEventInterface>> _eventHandlers;

    // Get the correct Event associated to handlers by the message strong
    MpEvent _getEventByName(std::string_view eventName);

    // Parse a message from the player
    EventParseRslt _parsePlayerMessage(Player* player, const std::string& msg);

    std::vector<std::string> _splitString(const std::string& s, char delimiter = '|');
};

#define sMpEventProcessor MpEventProcessor::instance()

#endif // MYTHIC_PLUS_EVENT_PROCESSOR_H
