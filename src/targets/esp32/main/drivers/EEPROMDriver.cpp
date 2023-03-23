#include "EEPROMDriver.h"
#include "assert.h"
#include "hal/i2c_types.h"

using namespace euph;

#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0 /*!< I2C master will not check ack from slave */

#define PAGE_SIZE 8
#define PAGES 32

EEPROMDriver::EEPROMDriver(i2c_port_t port, uint8_t address)
    : port(port), address(address) {}

bool EEPROMDriver::isPresent() {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (address << 1) | WRITE_BIT, ACK_CHECK_EN);
  i2c_master_stop(cmd);
  esp_err_t ret = i2c_master_cmd_begin(port, cmd, 50 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);
  return ret == ESP_OK;
}

void EEPROMDriver::writeByte(uint8_t data, uint16_t offset) {
  esp_err_t ret;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (address << 1) | WRITE_BIT, 1);
  i2c_master_write_byte(cmd, (offset >> 8) & 0xff, 1);
  i2c_master_write_byte(cmd, offset & 0xFF, 1);
  i2c_master_write_byte(cmd, data, 1);
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(port, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);
  assert(ret == ESP_OK);
}

void EEPROMDriver::writeBytes(uint8_t* data, uint16_t offset, size_t size) {
  esp_err_t ret = ESP_OK;

  int bytes_remaining = size;
  int current_address = offset;

  while (bytes_remaining > 0) {

    int to_write = PAGE_SIZE - (current_address % PAGE_SIZE);
    if (to_write > bytes_remaining)
      to_write = bytes_remaining;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | WRITE_BIT, 1);
    i2c_master_write_byte(cmd, (current_address >> 8) & 0xff, 1);
    i2c_master_write_byte(cmd, current_address & 0xFF, 1);

    i2c_master_write(cmd, data + current_address - offset, 1, 1);
    i2c_master_write(cmd, data + current_address - offset + 1, to_write - 1, 1);
    bytes_remaining -= to_write;
    current_address += to_write;
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(port, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    assert(ret == ESP_OK);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

std::vector<uint8_t> EEPROMDriver::readBytes(uint16_t offset, size_t size) {
  std::vector<uint8_t> data;
  data.resize(size);

  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (address << 1) | WRITE_BIT, 1);
  i2c_master_write_byte(cmd, (offset >> 8) & 0xff, 1);
  i2c_master_write_byte(cmd, offset & 0xFF, 1);
  esp_err_t ret = i2c_master_cmd_begin(port, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);
  assert(ret == ESP_OK);
  cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (address << 1) | READ_BIT, 1);
  i2c_master_read(cmd, data.data(), size, I2C_MASTER_LAST_NACK);
  i2c_master_stop(cmd);

  ret = i2c_master_cmd_begin(port, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);
  assert(ret == ESP_OK);

  return data;
}

uint8_t EEPROMDriver::readByte(uint16_t offset) {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (address << 1) | WRITE_BIT, 1);
  i2c_master_write_byte(cmd, (offset >> 8) & 0xff, 1);
  i2c_master_write_byte(cmd, offset & 0xFF, 1);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (address << 1) | READ_BIT, 1);

  uint8_t data;
  i2c_master_read_byte(cmd, &data, I2C_MASTER_NACK);
  i2c_master_stop(cmd);
  i2c_master_cmd_begin(port, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);
  return data;
}