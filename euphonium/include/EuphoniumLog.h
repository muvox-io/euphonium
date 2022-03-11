#ifndef EUPHONIUM_LOGGER_H
#define EUPHONIUM_LOGGER_H

#include <BellLogger.h>
#include <deque>
#include <functional>
#include <memory>
#include <sstream>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <mutex>

typedef std::function<void(std::string &)> onLogReceived;

class EuphoniumLogger : public bell::AbstractLogger {
  private:
      std::vector<char> buffer = std::vector<char>(4096);
      std::mutex logMutex;

  public:
    std::deque<std::string> logCache;
    onLogReceived logReceived = nullptr;

    // static bool enableColors = true;
    void debug(std::string filename, int line, std::string submodule,
               const char *format, ...) {
        std::lock_guard<std::mutex> guard(logMutex);
        std::stringstream log;
        log << "D [" << submodule << "] ";
        printf(colorRed);
        printf("D ");
        printf(colorReset);
        printf("[%s] ", submodule.c_str());
        printFilename(filename, log);
        log << ":" << line << ": ";
        printf(":%d: ", line);
        printf(colorReset);
        va_list args;
        va_start(args, format);
        vsprintf(buffer.data(), format, args);
        printf("%s", buffer.data());
        va_end(args);
        log << std::string(buffer.data());
        log << "\n";
        printf("\n");
        handleLog(log);
    };

    void error(std::string filename, int line, std::string submodule,
               const char *format, ...) {
        std::lock_guard<std::mutex> guard(logMutex);
        std::stringstream log;
        printf(colorRed);
        log << "E [" << submodule << "] ";
        printf("E ");
        printf(colorReset);
        printf("[%s] ", submodule.c_str());
        printFilename(filename, log);
        log << ":" << line << ": ";
        printf(":%d: ", line);
        printf(colorReset);
        va_list args;
        va_start(args, format);
        vsprintf(buffer.data(), format, args);
        printf("%s", buffer.data());
        va_end(args);
        log << std::string(buffer.data());
        log << "\n";
        printf("\n");
        handleLog(log);
    };

    void info(std::string filename, int line, std::string submodule,
              const char *format, ...) {
        std::lock_guard<std::mutex> guard(logMutex);
        std::stringstream log;
        printf(colorBlue);
        log << "I [" << submodule << "] ";
        printf("I ");
        printf(colorReset);
        printf("[%s] ", submodule.c_str());
        printFilename(filename, log);
        log << ":" << line << ": ";
        printf(":%d: ", line);
        printf(colorReset);
        va_list args;
        va_start(args, format);
        vsprintf(buffer.data(), format, args);
        printf("%s", buffer.data());
        va_end(args);
        log << std::string(buffer.data());
        log << "\n";
        printf("\n");
        handleLog(log);
    };

    void printFilename(std::string filename, std::stringstream &logStr) {
        std::string basenameStr(filename.substr(filename.rfind("/") + 1));
        logStr << basenameStr;
        unsigned long hash = 5381;
        for (char const &c : basenameStr) {
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        }

        printf("\e[0;%dm", allColors[hash % NColors]);

        printf("%s", basenameStr.c_str());
        printf(colorReset);
    }

    void handleLog(std::stringstream &log) {
        if (logCache.size() > 100) {
            logCache.pop_front();
        }
        logCache.push_back(log.str());
        if (logReceived != nullptr) {
            //logReceived(logStr);
        }
    }

  private:
    static constexpr const char *colorReset = "\e[0m";
    static constexpr const char *colorRed = "\e[0;31m";
    static constexpr const char *colorBlue = "\e[0;34m";
    static constexpr const int NColors = 15;
    static constexpr int allColors[NColors] = {31, 32, 33, 34, 35, 36, 37, 90,
                                               91, 92, 93, 94, 95, 96, 97};
};

void initializeEuphoniumLogger();

#define EUPH_LOG(type, ...)                                                    \
    do {                                                                       \
        bell::bellGlobalLogger->type(__FILE__, __LINE__, __VA_ARGS__);         \
    } while (0)

#endif
