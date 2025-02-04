#include "MpEvent.h"
#include "MpLogger.h"
#include "AdvancementMgr.h"
#include "MpEventProcessor.h"
#include "MpClientDispatcher.h"
#include "Player.h"

#include <string>

/**
 * All handlers for passed between client and server handlers are managed
 * here.
 */

/**
 * Handles Updates to a players advancement system
 * Event format
 * p|playerGuid|UpgradeAdvancement|advancementId|rank|diceSpent|bonus
 */
enum class MP_EVENT_CODE
{
    SUCCESS =                   0,

    // API Errors
    INVALID_EVENT =             100,
    INVALID_ARGUMENT_SIZE =     201,
    INVALID_ARGUMENT =          202,
    INVALID_ARGUMENT_TYPE =     203,

    // Game Response Codes
    FAILED_UPGRADE_ADV =        300,

};

// Send an error event to the client
bool SendEventError(Player* player, const std::string& method, MP_EVENT_CODE code, std::string message)
{
    std::vector<std::string> clientError = { std::to_string(static_cast<int>(code)), message };
    MpLogger::error("(Event Processor) Sending client error: {} {}", code, message);
    sMpClientDispatcher->Dispatch(MpClientEvent::Error, player, clientError);
    return false;
}

class UpdateAdvancements : public MpEventInterface
{
    public:
        const std::string EventName() const override
        {
            return "UpgradeAdvancement";
        }

        bool Execute(Player* player, std::vector<std::string>& args)
        {
            // Store the event data to send back to the client for parsing
            std::vector<std::string> eventData;

            std::string eventName = EventName();
            MpLogger::info("(EventProcessor) Executing {}", eventName.c_str());
            for(auto& arg : args) {
                MpLogger::info("{} Arg: {}", EventName(), arg);
            }

            // Validate the message is int he right format
            if(args.size() != 5) {
                return SendEventError(player, EventName(),MP_EVENT_CODE::INVALID_ARGUMENT_SIZE, "Invalid number of arguments expected 5, found " + std::to_string(args.size()));
            }

            uint32 advancementId = std::stoi(args[0]);
            if(advancementId >= MpAdvancements::MP_ADV_MAX) {
                return SendEventError(player, EventName(),MP_EVENT_CODE::INVALID_ARGUMENT, "Invalid advancement id " + args[0] + " max is " + std::to_string(MpAdvancements::MP_ADV_MAX));
            }

            uint32 diceLevel = std::stoi(args[1]);
            if(diceLevel < 1 || diceLevel > 3) {
                return SendEventError(player, EventName(),MP_EVENT_CODE::INVALID_ARGUMENT, "Invalid dice level " + args[1] + " valid values are 1,2,3");
            }

            uint32 itemEntry1 = std::stoi(args[2]);
            if(itemEntry1 == 0) {
                return SendEventError(player, EventName(),MP_EVENT_CODE::INVALID_ARGUMENT, "Invalid item entry1 can not be empty " + args[2]);
            }

            uint32 itemEntry2 = std::stoi(args[3]);
            uint32 itemEntry3 = std::stoi(args[4]);

            // Upgrade the advancement for the player!
            try {
                if(! sAdvancementMgr->UpgradeAdvancement(player, static_cast<MpAdvancements>(advancementId), diceLevel, itemEntry1, itemEntry2, itemEntry3)) {
                    return SendEventError(player, EventName(),MP_EVENT_CODE::INVALID_ARGUMENT, "Failed to upgrade advancement invalid request see error logs for player " + player->GetName());
                }
            } catch(const std::exception& e) {
                return SendEventError(player, EventName(),MP_EVENT_CODE::FAILED_UPGRADE_ADV, "Failed to upgrade: " + std::string(e.what()) + " for player " + player->GetName());
            }

            eventData = {"0", "success"};

            // Send response back to the client
            sMpClientDispatcher->Dispatch(MpClientEvent::UpgradeAdvancement, player, eventData);

            return true;
        }
};

class GetAdvancementRank : public MpEventInterface
{
    public:
        const std::string EventName() const override
        {
            return "GetAdvancementRank";
        }

        bool Execute(Player* player, std::vector<std::string>& args)
        {
            // Store the event data to send back to the client for parsing
            std::vector<std::string> eventData;

            MpLogger::info("(EventProcessor) Executing {}}", EventName());
            for(auto& arg : args) {
                MpLogger::info("{} Arg: {}", EventName(), arg);
            }

            // Validate the message is int he right format
            if(args.size() != 1) {
                return SendEventError(player, EventName(),MP_EVENT_CODE::INVALID_ARGUMENT_SIZE, "Invalid number of arguments expected 1, found " + std::to_string(args.size()));
            }

            uint32 advancementId = std::stoi(args[0]);
            if(advancementId >= MpAdvancements::MP_ADV_MAX) {
                return SendEventError(player, EventName(),MP_EVENT_CODE::INVALID_ARGUMENT, "Invalid advancement id " + args[0] + " max is " + std::to_string(MpAdvancements::MP_ADV_MAX));
            }

            MpAdvancementRank* rank = sAdvancementMgr->GetAdvancementRank(1, static_cast<MpAdvancements>(advancementId));
            if(!rank) {
                return SendEventError(player, EventName(),MP_EVENT_CODE::INVALID_ARGUMENT, "Failed to get advancement rank for player " + player->GetName());
            }

            eventData = {std::to_string(rank->rank), std::to_string(rank->advancementId)};

            // Send response back to the client
            sMpClientDispatcher->Dispatch(MpClientEvent::GetAdvancementRank, player, eventData);

            return true;
        }
};

void MP_Register_EventHandlers()
{
    auto updateAdvancements = std::make_shared<UpdateAdvancements>();
    sMpEventProcessor->RegisterHandler(MpEvent::UpgradeAdvancement, updateAdvancements);
}
