#ifndef BOOTLOOPDETECTOR_H
#define BOOTLOOPDETECTOR_H
#include "BerryBind.h"


/**
 * The bootloop detector is responsible for preventing bricking the device with
 * a misbehaving script or plugin, which would cause the device to crash on
 * boot. It works by storing a counter in the SPIFFS filesystem, which is
 * incremented as early as possible in the boot process. After the device has
 * been operating stably for bootCounterResetDelay seconds, the counter is reset
 * to zero. If the counter reaches a value of maxIncompleteBootCounter the
 * device will be rebooted to recovery mode to prevent a bootloop.
 *
 * NOTE: If the value in "/spiffs/data/bootloop_counter.txt" equals -1 the
 * bootloop detector will not be enabled.
 **/
class BootloopDetector {
  public:
    /**
     * The maximal number of incomplete boots before the device is rebooted to
     * recovery mode.
     **/
    static const int maxIncompleteBootCounter = 6;

    /**
     * The delay in seconds after which the boot counter is reset to zero.
     **/
    static const int bootCounterResetDelay = 10; // in second

    static int incompleteBootCounter;

    /**
     * Increments the counter and checks if the counter has reached the
     * maxIncompleteBootCounter value. If so, the device will be rebooted to
     * recovery mode.
     * Run as early as possible in the boot process, but after the SPIFFS has
     * been mounted.
     **/
    static void bootCounterIncrementAndCheck();

    /**
     * Resets the incomplete boot counter to zero.
     **/
    static void bootCounterReset();

    /**
     * Saves the boot counter to the SPIFFS filesystem.
     **/
    static void saveCounter();

    /**
     * Exports bindings to berry.
     **/
    static void exportBerryBindings(std::shared_ptr<berry::VmState> berry);


  private:
    /**
     * Creates the /spiffs/data directory if it does not exist.
     **/
    static void mkdirpData();
};

#endif // BOOTLOOPDETECTOR_H
