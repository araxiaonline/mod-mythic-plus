#ifndef MP_LOGGER_H
#define MP_LOGGER_H

#include "Log.h"

class MpLogger
{
public:
    template<typename... Args>
    static void debug(const char* fmt, Args&&... args) {
        LOG_DEBUG("module.MythicPlus", "[MythicPlus] " + std::string(fmt), std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(const char* fmt, Args&&... args) {
        LOG_ERROR("module.MythicPlus", "[MythicPlus] " + std::string(fmt), std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info(const char* fmt, Args&&... args) {
        LOG_INFO("module.MythicPlus", "[MythicPlus] " + std::string(fmt), std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warn(const char* fmt, Args&&... args) {
        LOG_WARN("module.MythicPlus", "[MythicPlus] " + std::string(fmt), std::forward<Args>(args)...);
    }

    template<typename... Args>
    void trace(const char* fmt, Args&&... args) {
        LOG_TRACE("module.MythicPlus", "[MythicPlus] " + std::string(fmt), std::forward<Args>(args)...);
    }
};




#endif // MP_LOGGER_H
