#ifndef EUPHONIUM_LOGGER_H
#define EUPHONIUM_LOGGER_H

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <memory>
#include <sol.hpp>
#include <BellLogger.h>

void luaLogDebug(std::string filename, int line, std::string log);
void luaLog(lua_State* L, std::string log);
void luaLogError(std::string filename, int line, std::string log);
void luaLogInfo(std::string filename, int line, std::string log);

#define EUPH_LOG(type, ...)                                \
    do                                                      \
    {                                                       \
        bell::bellGlobalLogger->type(__FILE__, __LINE__, __VA_ARGS__); \
    } while (0)

#endif
