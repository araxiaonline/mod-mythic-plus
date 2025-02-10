#include <array>
#include <string_view>
#include <string>
#include <unordered_map>

#ifndef MP_EVENTS_H
#define MP_EVENTS_H

// This defines list of incoming events typically from the client
enum class MpEvent
{
    None,                   // Default catch all for no event
    Invalid,                // Used to capture identify error events
    UpgradeAdvancement,     // Upgrades a player advancement 1 level
    ResetAdvancement,       // Resets a player Advancement back to 0
    ResetAllAdvancements,   // Resets all player advancements
    GetPlayerRank,          // Get the rank requirements for an advancement to the next levels
    GetAdvancementRank      // Get the details about the rank of a specific advancement (cost, bonus, etc)
};

// Client events that are communicated to the over an addon message to the player client
enum class MpClientEvent
{
    Error,
    UpgradeAdvancement,
    ResetAdvancement,
    ResetAllAdvancements,
    GetPlayerRank,
    GetAdvancementRank
};

// Mapping of Event Strings to EventNames for Event Callbacks from client
inline std::unordered_map<std::string_view, MpEvent> MpEventMap = {{
    {"UpgradeAdvancement", MpEvent::UpgradeAdvancement},
    {"ResetAdvancement", MpEvent::ResetAdvancement},
    {"ResetAllAdvancements", MpEvent::ResetAllAdvancements},
    {"GetPlayerRank", MpEvent::GetPlayerRank},
    {"GetAdvancementRank", MpEvent::ResetAllAdvancements}
}};

inline std::unordered_map<MpClientEvent, std::string_view> MpClientEventNames = {{
    {MpClientEvent::Error, "Error"},
    {MpClientEvent::UpgradeAdvancement, "UpgradeAdvancement"},
    {MpClientEvent::ResetAdvancement, "ResetAdvancement"},
    {MpClientEvent::ResetAllAdvancements, "ResetAllAdvancements"},
    {MpClientEvent::GetPlayerRank, "GetPlayerRank"},
    {MpClientEvent::GetAdvancementRank, "GetAdvancementRank"}
}};


#endif
