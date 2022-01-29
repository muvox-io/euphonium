#include "Core.h"
#include "FileScriptLoader.h"
#include "PortAudioAudioOutput.h"
#include <EuphoniumLog.h>
#include <optional>

int main(int argc, char *argv[]) {
    initializeEuphoniumLogger();
    bell::createDecoders();
    //auto vm = std::make_shared<berry::VmState>();
//    vm->export_function("ddd", &test);
//    BELL_LOG(info, "asd", "be %d", be_top(vm->raw_ptr()));
//    vm->execute_string("ddd(map({}))");
//    return 0;

    auto core = std::make_shared<Core>();
    auto loader = std::make_shared<FileScriptLoader>();
    auto output = std::make_shared<PortAudioAudioOutput>();
    core->selectAudioOutput(output);
    core->setupBindings();
    core->loadPlugins(loader);
    core->handleScriptingThread();
    return 0;
}
