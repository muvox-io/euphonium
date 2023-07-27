#include "ManufacuringShell.h"

using namespace euph;
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include "EuphLogger.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_vfs_dev.h"
#include "sdkconfig.h"

ManufacuringShell::ManufacuringShell(std::shared_ptr<euph::EventBus> eventBus)
    : bell::Task("ManufacuringShell", 3 * 1024, 0, 0) {
  this->eventBus = eventBus;
  startTask();
}

void ManufacuringShell::runTask() {

  // Initialize VFS & UART so we can use std::cout/cin
  setvbuf(stdin, NULL, _IONBF, 0);
  /* Install UART driver for interrupt-driven reads and writes */
  ESP_ERROR_CHECK(uart_driver_install((uart_port_t)CONFIG_ESP_CONSOLE_UART_NUM,
                                      256, 0, 0, NULL, 0));
  /* Tell VFS to use UART driver */
  esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);
  esp_vfs_dev_uart_port_set_rx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM,
                                            ESP_LINE_ENDINGS_CR);
  /* Move the caret to the beginning of the next line on '\n' */
  esp_vfs_dev_uart_port_set_tx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM,
                                            ESP_LINE_ENDINGS_CRLF);

  EUPH_LOG(info, TAG, "starting");
  while (true) {
    std::string cmd;
    std::getline(std::cin, cmd);
    this->eventBus->postEvent(
        std::make_unique<VmManufacuringShellEvent>(cmd));
  }
}
