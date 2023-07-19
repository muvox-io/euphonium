#include "EEPROMDriver.h"
#include <string.h>
#include <stdexcept>
#include "assert.h"
#include "hal/i2c_types.h"
#include "esp_rom_crc.h"

using namespace euph;

#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0 /*!< I2C master will not check ack from slave */

#define PAGE_SIZE 8

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

EEPROMContent EEPROMDriver::readContent() {
  EEPROMContent content = {};

  // Read the content's of eeprom
  auto eepromContent = readBytes(sizeof(EEPROMContent), 0);

  assert(sizeof(EEPROMContent) == eepromContent.size());

  // Copy into struct
  memcpy(&content, eepromContent.data(), sizeof(EEPROMContent));

  // Verify CRC16
  uint16_t calculatedCRC = esp_rom_crc16_be(0, eepromContent.data(), sizeof(EEPROMContent) - sizeof(uint16_t));

  // Validate crc
  if (calculatedCRC != content.crc) {
    throw std::runtime_error("CRC16 of EEPROM content does not match.");
  }

  return content;
}

void EEPROMDriver::writeContent(const EEPROMContent& content) {
  std::vector<uint8_t> eepromContent(sizeof(EEPROMContent));

  // Copy content into vector
  memcpy(eepromContent.data(), &content, sizeof(EEPROMContent));

  // Assign CRC16
  uint16_t calculatedCRC = esp_rom_crc16_be(0, eepromContent.data(), sizeof(EEPROMContent) - sizeof(uint16_t));
  eepromContent[eepromContent.size() - 2] = calculatedCRC >> 8;
  eepromContent[eepromContent.size() - 1] = calculatedCRC & 0xff;

  writeBytes(eepromContent.data(), eepromContent.size(), 0);
}

void EEPROMDriver::waitForAck() {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  // Wait for ack
  int ackCount = 0;

  while (ackCount < maxAckTries) {
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | WRITE_BIT, 1);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_OK)
      break;
    ackCount++;
  }
}

void EEPROMDriver::writeByte(uint8_t data, uint8_t offset) {
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
  waitForAck();
}

void EEPROMDriver::writeBytes(uint8_t* data, size_t data_len, uint8_t offset) {
  esp_err_t ret = ESP_OK;

  int bytesRemaining = data_len;
  int currentAddress = offset;

  // Perform write in multiple pages
  while (bytesRemaining > 0) {
    int toWrite = PAGE_SIZE - (currentAddress % PAGE_SIZE);

    // Last write
    if (toWrite > bytesRemaining)
      toWrite = bytesRemaining;

    // Prepare page write transmission
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | WRITE_BIT, 1);
    i2c_master_write_byte(cmd, currentAddress, 1);
    i2c_master_write(cmd, data + currentAddress - offset, toWrite, 1);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(port, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    assert(ret == ESP_OK);

    // Wait for ack from eeprom
    waitForAck();

    // Progress with data    
    bytesRemaining -= toWrite;
    currentAddress += toWrite;
  }
}

std::vector<uint8_t> EEPROMDriver::readBytes(size_t size, uint8_t offset) {
  std::vector<uint8_t> data;
  data.resize(size);

  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (address << 1) | WRITE_BIT, 1);
  i2c_master_write_byte(cmd, offset, 1);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (address << 1) | READ_BIT, 1);
  i2c_master_read(cmd, data.data(), size, I2C_MASTER_LAST_NACK);
  i2c_master_stop(cmd);

  esp_err_t ret = i2c_master_cmd_begin(port, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);
  assert(ret == ESP_OK);

  return data;
}

uint8_t EEPROMDriver::readByte(uint8_t offset) {
  uint8_t data = 0;

  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (address << 1) | WRITE_BIT, 1);
  i2c_master_write_byte(cmd, offset, 1);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (address << 1) | READ_BIT, 1);
  i2c_master_read_byte(cmd, &data, I2C_MASTER_NACK);
  i2c_master_stop(cmd);
  esp_err_t ret = i2c_master_cmd_begin(port, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);
  assert(ret == ESP_OK);
  return data;
}
