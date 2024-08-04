#ifndef MYTHICPLUS_H
#define MYTHICPLUS_H

#include "MapMgr.h"
#include "Log.h"

class MythicPlus {
private:
    MythicPlus();
        ~MythicPlus();
    public:

        bool Enabled = true;

        bool IsMapEligible(Map* map);

        // create a variadic function called `debug` that takes a string and a variadic number of arguments
        template<typename... Args>
        void debug(const char* fmt, Args&&... args) {
            LOG_DEBUG("module.MythicPlus", "[MythicPlus] " + std::string(fmt), std::forward<Args>(args)...);
        }

        static MythicPlus * getInstance() {
            static MythicPlus instance;
            return &instance;
        }

};

#endif
