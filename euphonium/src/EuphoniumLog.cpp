#include "EuphoniumLog.h"

std::shared_ptr<EuphoniumLogger> euphoniumLogger;

void setEuphoniumLogger() {
    euphoniumLogger = std::make_shared<EuphoniumLogger>();
}

void luaLogDebug(std::string filename, int line, std::string log) {
    euphoniumLogger->debug(filename, line, "lua", log.c_str());
}

void luaLogError(std::string filename, int line, std::string log) {
    euphoniumLogger->error(filename, line, "lua", log.c_str());
}

void luaLog(lua_State* L, std::string log) {
    lua_Debug debug;
    lua_getstack(L, 1, &debug);
    lua_getinfo(L, "Sl", &debug);
    euphoniumLogger->info(debug.short_src, debug.currentline, "lua", log.c_str());
}

void luaLogInfo(std::string filename, int line, std::string log) {
    euphoniumLogger->info(filename, line, "lua", log.c_str());
}