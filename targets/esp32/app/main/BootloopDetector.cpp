#include "BootloopDetector.h"
#include "BellLogger.h"
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int BootloopDetector::incompleteBootCounter = 0;

void BootloopDetector::bootCounterIncrementAndCheck() {
    mkdirpData();

    // open the file containing the counter
    std::ifstream counterFile("/spiffs/data/bootloop_counter.txt");
    if (!counterFile.is_open()) {
        BELL_LOG(info, "bootloop_detector",
                 "/spiffs/data/bootloop_counter.txt not found, creating it");
        incompleteBootCounter = 0;
        saveCounter();
        return;
    }

    // read the contents
    std::string counterFileContents(
        (std::istreambuf_iterator<char>(counterFile)),
        std::istreambuf_iterator<char>());
    counterFile.close();

    // convert the contents to an integer
    try {
        incompleteBootCounter = std::stoi(counterFileContents);
    } catch (std::invalid_argument &e) {
        BELL_LOG(error, "bootloop_detector",
                 "Invalid counter file contents: %s",
                 counterFileContents.c_str());
        incompleteBootCounter = 0;
        saveCounter();
    }

    // increment and check the counter
    BELL_LOG(info, "bootloop_detector", "Incomplete boot counter: %d",
             incompleteBootCounter);
    incompleteBootCounter++;
    if (incompleteBootCounter > BootloopDetector::maxIncompleteBootCounter) {
        BELL_LOG(
            info, "bootloop_detector",
            "Incomplete boot counter reached 6, rebooting to recovery mode");
        incompleteBootCounter = 0;
        saveCounter();
        // TODO: reboot to recovery mode
        return;
    }

    saveCounter();
}
void BootloopDetector::bootCounterReset() {
    BELL_LOG(info, "bootloop_detector", "Resetting incomplete boot counter");
    incompleteBootCounter = 0;
    saveCounter();
}
void BootloopDetector::mkdirpData() {
    struct stat buffer;
    if (stat("/spiffs/data", &buffer) != 0) {
        mkdir("/spiffs/data", 0777);
    }
}

void BootloopDetector::saveCounter() {
    std::ofstream counterFile("/spiffs/data/bootloop_counter.txt");
    counterFile << incompleteBootCounter;
    counterFile.close();
}

void BootloopDetector::exportBerryBindings(
    std::shared_ptr<berry::VmState> berry) {
    berry->export_function("bootloop_detector_boot_counter_reset",
                           BootloopDetector::bootCounterReset);
}
