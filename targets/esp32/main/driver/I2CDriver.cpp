#include "I2CDriver.h"

i2c_ack_type_t ACK_CHECK_ENU = (i2c_ack_type_t)0x1;
i2c_ack_type_t ACK_VALU = (i2c_ack_type_t)0x0;

void i2cInstall(bool isMaster, int sda, int scl, int clkSpeed) {
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

void i2cDelete() {
    i2c_driver_delete(i2c_port);
}

bool i2cWriteRegValueByte(int address, int reg, int value)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (((uint8_t) address) << 1) | I2C_MASTER_WRITE, ACK_CHECK_ENU);
    i2c_master_write_byte(cmd, (uint8_t) reg, ACK_CHECK_ENU);
    i2c_master_write_byte(cmd, (uint8_t) value, ACK_CHECK_ENU);


    i2c_master_stop(cmd);
    esp_err_t res = i2c_master_cmd_begin(i2c_port, cmd, 500 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return res == ESP_OK;
}

bool i2cWriteRegValueInt16(int address, int reg, int value)
{
    auto value16 = (uint16_t) value;

	esp_err_t res = 0;
	uint8_t send_buff[4];
	send_buff[0] = (((uint8_t) address) << 1);
	send_buff[1] = (uint8_t) reg;
	send_buff[2] = (value16 >> 8) & 0xff;
	send_buff[3] = value16 & 0xff;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	res |= i2c_master_start(cmd);
	res |= i2c_master_write(cmd, send_buff, 4, ACK_CHECK_ENU);
	res |= i2c_master_stop(cmd);
	res |= i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);

    return res == ESP_OK;
}

int i2cReadRegValueInt16(int address, int reg)
{
    uint8_t addr = (uint8_t) address;
    uint8_t reg_addr = (uint8_t) reg;

	uint8_t data[2] = {0};

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, ACK_CHECK_ENU);
	i2c_master_write_byte(cmd, reg_addr, ACK_CHECK_ENU);
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_READ, ACK_CHECK_ENU); //check or not
	i2c_master_read(cmd, data, 2, ACK_VALU);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

    uint16_t value = (data[0] << 8) + data[1];
    BELL_LOG(info, "esp32", "READ VAL %d", value);
	return value;
}

bool i2cWriteReg(int address, int reg)
{
    i2c_cmd_handle_t i2c_cmd = i2c_cmd_link_create();

    i2c_master_start(i2c_cmd);
    i2c_master_write_byte(i2c_cmd, (((uint8_t) address) << 1) | I2C_MASTER_WRITE, ACK_CHECK_ENU);
    i2c_master_write_byte(i2c_cmd, (uint8_t) reg, ACK_CHECK_ENU);


    i2c_master_stop(i2c_cmd);
    esp_err_t res = i2c_master_cmd_begin(i2c_port, i2c_cmd, 500 / portTICK_RATE_MS);
    i2c_cmd_link_delete(i2c_cmd);

    return res == ESP_OK;
}

int i2cReadReg(int address, int reg)
{
    i2c_cmd_handle_t i2c_cmd = i2c_cmd_link_create();
    uint8_t data;

    i2c_master_start(i2c_cmd);
    i2c_master_write_byte(i2c_cmd, (((uint8_t) address) << 1) | I2C_MASTER_WRITE, ACK_CHECK_ENU);
    i2c_master_read_byte(i2c_cmd, &data, ACK_CHECK_ENU);


    i2c_master_stop(i2c_cmd);
    esp_err_t res = i2c_master_cmd_begin(i2c_port, i2c_cmd, 500 / portTICK_RATE_MS);
    i2c_cmd_link_delete(i2c_cmd);

    if(res == ESP_OK) {
        return data;
    } else {
        return -1;
    }
}

void exportI2CDriver(std::shared_ptr<berry::VmState> berry) {
    berry->export_function("i2c_install", &i2cInstall);
    berry->export_function("i2c_delete", &i2cDelete);

    berry->export_function("i2c_write8", &i2cWriteRegValueByte);
    berry->export_function("i2c_write8_val", &i2cWriteReg);
    berry->export_function("i2c_read8", &i2cReadReg);

    berry->export_function("i2c_write16", &i2cWriteRegValueInt16);
    berry->export_function("i2c_read16", &i2cReadRegValueInt16);
}
