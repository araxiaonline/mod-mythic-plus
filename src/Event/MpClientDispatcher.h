#ifndef MYTHIC_PLUS_CLIENT_DISPATCHER_H
#define MYTHIC_PLUS_CLIENT_DISPATCHER_H

#include "MpEvent.h"
#include "Player.h"

#include <string>
#include <string_view>
#include <vector>

class MpClientDispatcher
{

public:
    static MpClientDispatcher* instance() {
        static MpClientDispatcher instance;
        return &instance;
    }

    MpClientDispatcher(const MpClientDispatcher&) = delete;
    MpClientDispatcher& operator=(const MpClientDispatcher&) = delete;

    // encode and send a message to the client for an event in the map
    bool Dispatch(MpClientEvent event, Player* player, std::vector<std::string>& args);

private:
    MpClientDispatcher() {};
    ~MpClientDispatcher() {};

};

#define sMpClientDispatcher MpClientDispatcher::instance()

#endif

