#pragma once

#include <driver/i2c.h>
#include <stdint.h>
#include <cstddef>
#include <vector>

namespace euph {

// Structure of the EEPROM content
struct EEPROMContent {
  uint8_t magic[2];      // Magic bytes
  uint8_t format;        // Format version
  char model[4];         // Model string
  uint8_t serial[16];    // Serial number
  uint8_t secret[16];    // Secret key
  uint8_t reserved[16];  // Reserved bytes
  uint16_t crc;          // CRC16 checksum of the EEPROM content
};

class EEPROMDriver {
 private:
  i2c_port_t port;
  uint8_t address;

 public:
  EEPROMDriver(i2c_port_t port, uint8_t address);

  // Returns true if the EEPROM is present at the given address, else otherwise
  bool isPresent();

  void writeByte(uint8_t data, uint16_t offset);
  void writeBytes(uint8_t* data, uint16_t offset, size_t size);
  uint8_t readByte(uint16_t offset);
  std::vector<uint8_t> readBytes(uint16_t offset, size_t size);
};

}  // namespace euph