#include "EuphoniumLog.h"

void luaLogDebug(std::string filename, int line, std::string log) {
    bell::bellGlobalLogger->debug(filename, line, "lua", log.c_str());
}

void luaLogError(std::string filename, int line, std::string log) {
    bell::bellGlobalLogger->error(filename, line, "lua", log.c_str());
}

void luaLog(lua_State* L, std::string log) {
    lua_Debug debug;
    lua_getstack(L, 1, &debug);
    lua_getinfo(L, "Sl", &debug);
    bell::bellGlobalLogger->info(debug.short_src, debug.currentline, "lua", log.c_str());
}

void luaLogInfo(std::string filename, int line, std::string log) {
    bell::bellGlobalLogger->info(filename, line, "lua", log.c_str());
}