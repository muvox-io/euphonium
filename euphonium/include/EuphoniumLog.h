#ifndef EUPHONIUM_LOGGER_H
#define EUPHONIUM_LOGGER_H

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <memory>
#include <BellLogger.h>

#define EUPH_LOG(type, ...)                                \
    do                                                      \
    {                                                       \
        bell::bellGlobalLogger->type(__FILE__, __LINE__, __VA_ARGS__); \
    } while (0)

#endif
