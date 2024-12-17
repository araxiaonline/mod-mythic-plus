#include <array>
#include <string_view>
#include <string>
#include <unordered_map>

#ifndef MP_EVENTS_H
#define MP_EVENTS_H

// This defines
enum class MpEvent
{
    None, // Default catch all for no event
    Invalid, // Used to capture identify error events
    UpgradeAdvancement,
    ResetAdvancement,
    ResetAllAdvancements
};

// Mapping of Event Strings to EventNames for Event Callbacks from client
std::unordered_map<std::string_view, MpEvent> MpEventMap = {{
    {"UpgradeAdvancement", MpEvent::UpgradeAdvancement},
    {"ResetAdvancement", MpEvent::ResetAdvancement},
    {"ResetAllAdvancements", MpEvent::ResetAllAdvancements}
}};


#endif
