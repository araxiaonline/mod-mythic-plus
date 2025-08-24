#ifndef MP_LOGGER_H
#define MP_LOGGER_H

#include "Log.h"
#include <exception>
#include <iostream>
#include <boost/stacktrace.hpp>

class MpLogger
{

public:
    template<typename... Args>
    static void debug(const char* fmt, Args&&... args) {
        try {
            LOG_DEBUG("module.MythicPlus", "[MythicPlus] DEBUG " + std::string(fmt), std::forward<Args>(args)...);
        } catch (const std::exception& e) {
            LOG_ERROR("module.MythicPlus", "[MythicPlus] DEBUG EXCEPTION: " + std::string(e.what()) +
                      "\nStack trace: " + boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
        } catch (...) {
            LOG_ERROR("module.MythicPlus", "[MythicPlus] DEBUG NON-STANDARD EXCEPTION");
        }
    }

    template<typename... Args>
    static void error(const char* fmt, Args&&... args) {
        try {
            LOG_ERROR("module.MythicPlus", "[MythicPlus] ERROR " + std::string(fmt), std::forward<Args>(args)...);
        } catch (const std::exception& e) {

            LOG_ERROR("module.MythicPlus", "[MythicPlus] ERROR EXCEPTION: " + std::string(e.what()) +
                      "\nStack trace: " + boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
        } catch (...) {
            LOG_ERROR("module.MythicPlus", "[MythicPlus] ERROR NON-STANDARD EXCEPTION");
        }
    }

    template<typename... Args>
    static void info(const char* fmt, Args&&... args) {
        try {
            LOG_INFO("module.MythicPlus", "[MythicPlus] INFO " + std::string(fmt), std::forward<Args>(args)...);
        } catch (const std::exception& e) {
            LOG_ERROR("module.MythicPlus", "[MythicPlus] INFO EXCEPTION: " + std::string(e.what()) +
                      "\nStack trace: " + boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
        } catch (...) {
            LOG_ERROR("module.MythicPlus", "[MythicPlus] INFO NON-STANDARD EXCEPTION");
        }
    }

    template<typename... Args>
    static void warn(const char* fmt, Args&&... args) {
        try {
            LOG_WARN("module.MythicPlus", "[MythicPlus] WARN " + std::string(fmt), std::forward<Args>(args)...);
        } catch (const std::exception& e) {
            LOG_ERROR("module.MythicPlus", "[MythicPlus] WARN EXCEPTION: " + std::string(e.what()) +
                      "\nStack trace: " + boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
        } catch (...) {
            LOG_ERROR("module.MythicPlus", "[MythicPlus] WARN NON-STANDARD EXCEPTION");
        }
    }

    template<typename... Args>
    static void trace(const char* fmt, Args&&... args) {
        try {
            LOG_TRACE("module.MythicPlus", "[MythicPlus] TRACE " + std::string(fmt), std::forward<Args>(args)...);
        } catch (const std::exception& e) {
            LOG_ERROR("module.MythicPlus", "[MythicPlus] TRACE EXCEPTION: " + std::string(e.what()) +
                      "\nStack trace: " + boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
        } catch (...) {
            LOG_ERROR("module.MythicPlus", "[MythicPlus] TRACE NON-STANDARD EXCEPTION");
        }
    }
};

#endif // MP_LOGGER_H
