#ifndef EUPHONIUM_LOGGER_H
#define EUPHONIUM_LOGGER_H

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <memory>
#include <sol.hpp>


class EuphoniumLogger
{
public:
    // static bool enableColors = true;
    void debug(std::string filename, int line, std::string tag, const char *format, ...)
    {
        std::string basenameStr(filename.substr(filename.rfind("/") + 1));

        printf(colorRed);
        printf("D ");
        printFilename("[" + tag + "] ");
        printf("%s", basenameStr.c_str());
        printf(":%d: ", line);
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        printf("\n");
    };

    void error(std::string filename, int line, std::string tag, const char *format, ...)
    {
        std::string basenameStr(filename.substr(filename.rfind("/") + 1));

        printf(colorRed);
        printf("E ");
        printFilename("[" + tag + "] ");
        printf("%s", basenameStr.c_str());
        printf(":%d: ", line);
        printf(colorRed);
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        printf("\n");
    };

    void info(std::string filename, int line, std::string tag, const char *format, ...)
    {
        std::string basenameStr(filename.substr(filename.rfind("/") + 1));

        printf(colorBlue);
        printf("I ");
        printFilename("[" + tag + "] ");
        printf("%s", basenameStr.c_str());
        printf(":%d: ", line);
        printf(colorReset);
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        printf("\n");
    };

    void printFilename(std::string filename)
    {
        unsigned long hash = 5381;
        int c;

        for (char const &c : filename)
        {
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        }

        printf("\e[0;%dm", allColors[hash % NColors]);

        printf("%s", filename.c_str());
        printf(colorReset);
    }

private:
    static constexpr const char *colorReset = "\e[0m";
    static constexpr const char *colorRed = "\e[0;31m";
    static constexpr const char *colorBlue = "\e[0;34m";
    static constexpr const int NColors = 16;
    static constexpr int allColors[NColors] = {30, 31, 32, 33, 34, 35, 36, 37, 90, 91, 92, 93, 94, 95, 96, 97};
};

extern std::shared_ptr<EuphoniumLogger> euphoniumLogger;

void setEuphoniumLogger();

void luaLogDebug(std::string filename, int line, std::string log);
void luaLog(lua_State* L, std::string log);
void luaLogError(std::string filename, int line, std::string log);
void luaLogInfo(std::string filename, int line, std::string log);


#define EUPH_LOG(type, ...)                                \
    do                                                      \
    {                                                       \
        euphoniumLogger->type(__FILE__, __LINE__, __VA_ARGS__); \
    } while (0)

#endif
