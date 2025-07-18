#include "MpEvent.h"
#include "MpLogger.h"
#include "../AdvancementMgr.h"
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
 */
enum class MP_EVENT_CODE
{
    SUCCESS =                   0,

    // Message Errors
    INVALID_EVENT =             100,
    INVALID_ARGUMENT_SIZE =     201,
    INVALID_ARGUMENT =          202,
    INVALID_ARGUMENT_TYPE =     203,

    // Game Response Codes
    FAILED_UPGRADE_ADV =        300,

    // Unknown Error
    UNKNOWN_ERROR =             400,
    NO_HANDLER =                401

};

std::string EventCodeToString(MP_EVENT_CODE code)
{
    switch(code) {
        case MP_EVENT_CODE::SUCCESS: return "Success";
        case MP_EVENT_CODE::INVALID_EVENT: return "Invalid event";
        case MP_EVENT_CODE::INVALID_ARGUMENT_SIZE: return "Invalid argument size";
        case MP_EVENT_CODE::INVALID_ARGUMENT: return "Invalid argument";
        case MP_EVENT_CODE::INVALID_ARGUMENT_TYPE: return "Invalid argument type";
        case MP_EVENT_CODE::FAILED_UPGRADE_ADV: return "Failed to upgrade advancement";
        case MP_EVENT_CODE::NO_HANDLER: return "No handler for event";
        default: return "Unknown error";
    }
}

// Send an error event to the client
bool SendEventError(Player* player, const std::string& method, MP_EVENT_CODE code, std::string message)
{
    std::vector<std::string> clientError = { std::to_string(static_cast<int>(code)), message };
    MpLogger::error("(Event Processor) Sending client error: {} {}", code, message);
    sMpClientDispatcher->Dispatch(MpClientEvent::Error, player, clientError);
    return false;
}

/**
 * Upgrade a Player Advancement
 * Message Format:
 * p|playerGuid|UpgradeAdvancement|advancementId|diceLevel|itemEntry1|itemEntry2|itemEntry3
 */
class UpgradeAdvancements : public MpEventInterface
{
    public:
        const std::string EventName() const override
        {
            return "UpgradeAdvancement";
        }

        bool Execute(Player* player, std::vector<std::string>& args) override
        {
            // Store the event data to send back to the client for parsing
            std::vector<std::string> eventData;

            std::string eventName = EventName();
            MpLogger::info("(EventProcessor) Executing {}", eventName.c_str());
            for(auto& arg : args) {
                MpLogger::info("{} Arg: {}", EventName(), arg);
            }

            // Validate the message is in the right format
            if(args.size() != 2) {
                return SendEventError(player, EventName(), MP_EVENT_CODE::INVALID_ARGUMENT_SIZE, "Invalid number of arguments expected 2, found " + std::to_string(args.size()));
            }

            uint32 advancementId = std::stoi(args[0]);
            if(advancementId >= MpAdvancements::MP_ADV_MAX) {
                return SendEventError(player, EventName(), MP_EVENT_CODE::INVALID_ARGUMENT, "Invalid advancement id " + args[0] + " max is " + std::to_string(MpAdvancements::MP_ADV_MAX));
            }

            uint32 diceLevel = std::stoi(args[1]);
            if(diceLevel < 1 || diceLevel > 3) {
                return SendEventError(player, EventName(), MP_EVENT_CODE::INVALID_ARGUMENT, "Invalid dice level " + args[1] + " valid values are 1,2,3");
            }

            uint32 increase;
            try {
                increase = sAdvancementMgr->UpgradeAdvancement(player, static_cast<MpAdvancements>(advancementId), diceLevel);
                if(increase == 0) {
                    return SendEventError(player, EventName(), MP_EVENT_CODE::INVALID_ARGUMENT, "Failed to upgrade advancement invalid request see error logs for player " + player->GetName());
                }
            } catch(const std::exception& e) {
                return SendEventError(player, EventName(), MP_EVENT_CODE::FAILED_UPGRADE_ADV, "Failed to upgrade: " + std::string(e.what()) + " for player " + player->GetName());
            }

            // Only proceed to here if no errors occurred
            MpPlayerRank* playerRank = sAdvancementMgr->GetPlayerAdvancementRank(player, static_cast<MpAdvancements>(advancementId));
            if (!playerRank) {
                return SendEventError(player, EventName(), MP_EVENT_CODE::INVALID_ARGUMENT, "Failed to get advancement rank for player " + player->GetName());
            }

            // Format the success event data for client increase|newrank|bonus
            eventData = {
                EventCodeToString(MP_EVENT_CODE::SUCCESS),
                std::to_string(increase),
                std::to_string(playerRank->rank),
                std::to_string(playerRank->bonus)
            };

            // Send response back to the client
            sMpClientDispatcher->Dispatch(MpClientEvent::UpgradeAdvancement, player, eventData);

            return true;
        }
};

/**
 * @brief Event is used to get the rank requirements for an advancement to the next levels
 *
 * Message Format:
 * p|playerGuid|GetPlayerRank|advancementId
 *
 * Returns:
 * p|playerGuid|GetPlayerRank|rank|bonus|spentDice
 */
class GetPlayerRank : public MpEventInterface
{
    public:
        const std::string EventName() const override
        {
            return "GetPlayerRank";
        }

        bool Execute(Player* player, std::vector<std::string>& args) override
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

            MpPlayerRank* playerRank = sAdvancementMgr->GetPlayerAdvancementRank(player, static_cast<MpAdvancements>(advancementId));
            if(!playerRank) {
                return SendEventError(player, EventName(),MP_EVENT_CODE::INVALID_ARGUMENT, "Failed to get advancement rank for player " + player->GetName());
            }

            eventData = {
                EventCodeToString(MP_EVENT_CODE::SUCCESS),
                std::to_string(playerRank->rank),
                std::to_string(playerRank->bonus),
                std::to_string(playerRank->diceSpent)
            };

            // Send response back to the client
            sMpClientDispatcher->Dispatch(MpClientEvent::GetPlayerRank, player, eventData);

            return true;
        }
};

/**
 * @brief Event is used to get the rank requirements for an advancement to the next levels
 *
 * Message Format:
 * p|playerGuid|GetAdvancmentRank|advancementId|rank
 *
 * Returns:
 * p|playerGuid|GetAdvancementRank|advancementId|rank|min1|max1|min2|max2|min3|max3|itemEntry1|itemEntryCost1|itemEntry2|itemEntryCost2|itemEntry3|itemEntryCost3
 */
class GetAdvancementRank : public MpEventInterface {
    public:
        const std::string EventName() const override
        {
            return "GetAdvancementRank";
        }

        bool Execute(Player* player, std::vector<std::string>& args) override
        {
            if(args.size() != 2) {
                return SendEventError(player, EventName(),MP_EVENT_CODE::INVALID_ARGUMENT_SIZE, "Invalid number of arguments expected 2, found " + std::to_string(args.size()));
            }

            uint32 advancementId = std::stoi(args[0]);
            if(advancementId >= MpAdvancements::MP_ADV_MAX) {
                return SendEventError(player, EventName(),MP_EVENT_CODE::INVALID_ARGUMENT, "Invalid advancement id " + args[0] + " max is " + std::to_string(MpAdvancements::MP_ADV_MAX));
            }

            uint32 rank = std::stoi(args[1]);
            if(rank == 0) {
                return SendEventError(player, EventName(),MP_EVENT_CODE::INVALID_ARGUMENT, "Invalid rank " + args[1] + " can not be empty");
            }

            MpAdvancementRank* advRank = sAdvancementMgr->GetAdvancementRank(rank, static_cast<MpAdvancements>(advancementId));
            if(!advRank) {
                return SendEventError(player, EventName(),MP_EVENT_CODE::INVALID_ARGUMENT, "Failed to get advancement rank for player " + player->GetName());
            }

            std::vector<std::string> eventData = {
                EventCodeToString(MP_EVENT_CODE::SUCCESS),
                std::to_string(advRank->advancementId),
                std::to_string(advRank->rank),
                std::to_string(advRank->lowRange.first),
                std::to_string(advRank->lowRange.second),
                std::to_string(advRank->midRange.first),
                std::to_string(advRank->midRange.second),
                std::to_string(advRank->highRange.first),
                std::to_string(advRank->highRange.second),
                std::to_string(advRank->material1.first),
                std::to_string(advRank->material1.second),
                std::to_string(advRank->material2.first),
                std::to_string(advRank->material2.second),
                std::to_string(advRank->material3.first),
                std::to_string(advRank->material3.second)
            };

            // Send response back to the client
            sMpClientDispatcher->Dispatch(MpClientEvent::GetAdvancementRank, player, eventData);
            return true;
        }
};

void MP_Register_EventHandlers()
{
    sMpEventProcessor->RegisterHandler(MpEvent::UpgradeAdvancement, std::make_shared<UpgradeAdvancements>());
    sMpEventProcessor->RegisterHandler(MpEvent::GetPlayerRank, std::make_shared<GetPlayerRank>());
    sMpEventProcessor->RegisterHandler(MpEvent::GetAdvancementRank, std::make_shared<GetAdvancementRank>());
}
