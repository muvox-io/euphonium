#include "I2CDriver.h"

#define WRITE_BIT I2C_MASTER_WRITE  /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ    /*!< I2C master read */
#define ACK_CHECK_EN 0x1            /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0           /*!< I2C master will not check ack from slave */

void i2cMasterInstall(int sda, int scl, int clkSpeed) {
    BELL_LOG(info, "esp32", "Installing I2C driver");
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda,
        .scl_io_num = scl,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
    };

    i2c_config.master.clk_speed = clkSpeed;
    i2c_param_config(i2c_port, &i2c_config);
    i2c_driver_install(i2c_port, I2C_MODE_MASTER, false, false, false);
}

void i2cDelete() { i2c_driver_delete(i2c_port); }

void i2cMasterWriteToDevice(int portNum, uint8_t deviceAddr, berry::list writeBuf) {
    auto writeBufMapped = std::vector<uint8_t>();
    for(auto const& value: writeBuf) {
        writeBufMapped.push_back(std::any_cast<int>(value));
    }

    auto res = i2c_master_write_to_device(portNum, deviceAddr, writeBufMapped.data(), writeBuf.size(), 1000 / portTICK_PERIOD_MS);

    if (res != ESP_OK) {
        BELL_LOG(info, "i2c", "Write failed!");
    }
}

berry::list i2cMasterReadFromDevice(int portNum, uint8_t deviceAddr, int readSize) {
    auto result = std::vector<uint8_t>(readSize);
    auto res = i2c_master_read_from_device(portNum, deviceAddr, result.data(), readSize, 1000 / portTICK_PERIOD_MS);
    if (res != ESP_OK) {
        BELL_LOG(info, "i2c", "Read failed!");
    }

    auto mappedRes = berry::list();
    for(auto const& value: result) {
        mappedRes.push_back(int(value));
    }

    return mappedRes;
}

berry::list i2cMasterWriteReadDevice(int portNum, uint8_t deviceAddr, berry::list writeBuf, int readSize) {
    auto writeBufMapped = std::vector<uint8_t>();
    for(auto const& value: writeBuf) {
        writeBufMapped.push_back(std::any_cast<int>(value));
    }

    auto result = std::vector<uint8_t>(readSize);
    auto res = i2c_master_write_read_device(portNum, deviceAddr, writeBufMapped.data(), writeBufMapped.size(), result.data(), readSize, 1000 / portTICK_PERIOD_MS);
    if (res != ESP_OK) {
        BELL_LOG(info, "i2c", "Read / Write failed!");
    }

    auto mappedRes = berry::list();
    for(auto const& value: result) {

        mappedRes.push_back(int(value));
    }
    return mappedRes;
}
bool i2cDetect(int address) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 50 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret == ESP_OK;
}

void exportI2CDriver(std::shared_ptr<berry::VmState> berry) {
    berry->export_function("master_install", &i2cMasterInstall, "i2c");
    berry->export_function("delete", &i2cDelete, "i2c");
    berry->export_function("detect", &i2cDetect, "i2c");
    berry->export_function("master_write_to_device", &i2cMasterWriteToDevice, "i2c");
    berry->export_function("master_read_from_device", &i2cMasterReadFromDevice, "i2c");
    berry->export_function("master_write_read_device", &i2cMasterWriteReadDevice, "i2c");
}
