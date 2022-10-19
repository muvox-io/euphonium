#ifndef BELL_LOGGER_H
#define BELL_LOGGER_H

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <memory>

namespace bell
{

    class AbstractLogger
    {
    public:
        bool enableSubmodule = false;
        virtual void debug(std::string filename, int line, std::string submodule, const char *format, ...) = 0;
        virtual void error(std::string filename, int line, std::string submodule, const char *format, ...) = 0;
        virtual void info(std::string filename, int line, std::string submodule, const char *format, ...) = 0;
    };

    extern std::shared_ptr<bell::AbstractLogger> bellGlobalLogger;
    class BellLogger : public bell::AbstractLogger
    {
    public:
        // static bool enableColors = true;
        void debug(std::string filename, int line, std::string submodule, const char *format, ...)
        {

            printf(colorRed);
            printf("D ");
            if (enableSubmodule) {
                printf(colorReset);
                printf("[%s] ", submodule.c_str());
            }
            printFilename(filename);
            printf(":%d: ", line);
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
            printf("\n");
        };

        void error(std::string filename, int line, std::string submodule, const char *format, ...)
        {

            printf(colorRed);
            printf("E ");
            if (enableSubmodule) {
                printf(colorReset);
                printf("[%s] ", submodule.c_str());
            }
            printFilename(filename);
            printf(":%d: ", line);
            printf(colorRed);
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
            printf("\n");
        };

        void info(std::string filename, int line, std::string submodule, const char *format, ...)
        {

            printf(colorBlue);
            printf("I ");
            if (enableSubmodule) {
                printf(colorReset);
                printf("[%s] ", submodule.c_str());
            }
            printFilename(filename);
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
            std::string basenameStr(filename.substr(filename.rfind("/") + 1));
            unsigned long hash = 5381;
            for (char const &c : basenameStr)
            {
                hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
            }

            printf("\e[0;%dm", allColors[hash % NColors]);

            printf("%s", basenameStr.c_str());
            printf(colorReset);
        }

    private:
        static constexpr const char *colorReset = "\e[0m";
        static constexpr const char *colorRed = "\e[0;31m";
        static constexpr const char *colorBlue = "\e[0;34m";
        static constexpr const int NColors = 15;
        static constexpr int allColors[NColors] = {31, 32, 33, 34, 35, 36, 37, 90, 91, 92, 93, 94, 95, 96, 97};
    };

    void setDefaultLogger();
    void enableSubmoduleLogging();
}

#define BELL_LOG(type, ...)                                      \
    do                                                            \
    {                                                             \
        bell::bellGlobalLogger->type(__FILE__, __LINE__, __VA_ARGS__); \
    } while (0)

#endif