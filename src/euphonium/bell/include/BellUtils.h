#ifndef EUPHONIUM_BELL_UTILS
#define EUPHONIUM_BELL_UTILS

#include <random>
#include <string.h>
#include <vector>

namespace bell {

std::string generateRandomUUID();
void freeAndNull(void *&ptr);

} // namespace bell

#ifdef ESP_PLATFORM
#include <freertos/FreeRTOS.h>

#define BELL_SLEEP_MS(ms) vTaskDelay(ms / portTICK_PERIOD_MS)
#define BELL_YIELD() taskYIELD()

#else
#include <unistd.h>

#define BELL_SLEEP_MS(ms) usleep(ms * 1000)
#define BELL_YIELD() ;

#endif
#endif
