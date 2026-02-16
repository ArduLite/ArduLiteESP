#ifndef ARDULITEESP_I2C_H
#define ARDULITEESP_I2C_H

#include "ArduLiteESP_Core.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/i2c.h"

#ifdef __cplusplus
}
#endif

// ============================================================================
// I2C Master
// ============================================================================
class I2C {
public:
    explicit I2C(uint8_t port, uint8_t addr)
        : i2c_port((i2c_port_t)port),
          address(addr) {
    }

    bool begin(int sda_pin = -1, int scl_pin = -1, uint32_t freq = 100000) {
        // Check if this port is already initialized
        if (port_initialized[i2c_port]) {
            return true;
        }

        // Default pins
        if (sda_pin == -1) sda_pin = (i2c_port == I2C_NUM_0) ? 21 : 33;
        if (scl_pin == -1) scl_pin = (i2c_port == I2C_NUM_0) ? 22 : 32;

        i2c_config_t conf = {};
        conf.mode = I2C_MODE_MASTER;
        conf.sda_io_num = sda_pin;
        conf.scl_io_num = scl_pin;
        conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
        conf.master.clk_speed = freq;
        conf.clk_flags = 0;

        esp_err_t err = i2c_param_config(i2c_port, &conf);
        if (err != ESP_OK) return false;

        err = i2c_driver_install(i2c_port, I2C_MODE_MASTER, 0, 0, 0);
        if (err != ESP_OK) return false;

        port_initialized[i2c_port] = true;
        return true;
    }

    bool writeByte(uint8_t reg, uint8_t data) {
        if (!port_initialized[i2c_port]) return false;

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, reg, true);
        i2c_master_write_byte(cmd, data, true);
        i2c_master_stop(cmd);

        esp_err_t err = i2c_master_cmd_begin(i2c_port, cmd, pdMS_TO_TICKS(1000));
        i2c_cmd_link_delete(cmd);

        return (err == ESP_OK);
    }

    bool writeBytes(uint8_t reg, const uint8_t* data, size_t len) {
        if (!port_initialized[i2c_port]) return false;

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, reg, true);
        i2c_master_write(cmd, (uint8_t*)data, len, true);
        i2c_master_stop(cmd);

        esp_err_t err = i2c_master_cmd_begin(i2c_port, cmd, pdMS_TO_TICKS(1000));
        i2c_cmd_link_delete(cmd);

        return (err == ESP_OK);
    }

    bool write(const uint8_t* data, size_t len) {
        if (!port_initialized[i2c_port]) return false;

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write(cmd, (uint8_t*)data, len, true);
        i2c_master_stop(cmd);

        esp_err_t err = i2c_master_cmd_begin(i2c_port, cmd, pdMS_TO_TICKS(1000));
        i2c_cmd_link_delete(cmd);

        return (err == ESP_OK);
    }

    bool readByte(uint8_t reg, uint8_t* data) {
        if (!port_initialized[i2c_port]) return false;

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, reg, true);
        i2c_master_stop(cmd);

        esp_err_t err = i2c_master_cmd_begin(i2c_port, cmd, pdMS_TO_TICKS(1000));
        i2c_cmd_link_delete(cmd);

        if (err != ESP_OK) return false;

        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_READ, true);
        i2c_master_read_byte(cmd, data, I2C_MASTER_NACK);
        i2c_master_stop(cmd);

        err = i2c_master_cmd_begin(i2c_port, cmd, pdMS_TO_TICKS(1000));
        i2c_cmd_link_delete(cmd);

        return (err == ESP_OK);
    }

    bool readBytes(uint8_t reg, uint8_t* data, size_t len) {
        if (!port_initialized[i2c_port]) return false;

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, reg, true);
        i2c_master_stop(cmd);

        esp_err_t err = i2c_master_cmd_begin(i2c_port, cmd, pdMS_TO_TICKS(1000));
        i2c_cmd_link_delete(cmd);

        if (err != ESP_OK) return false;

        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_READ, true);

        if (len > 1) {
            i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
        }
        i2c_master_read_byte(cmd, data + len - 1, I2C_MASTER_NACK);
        i2c_master_stop(cmd);

        err = i2c_master_cmd_begin(i2c_port, cmd, pdMS_TO_TICKS(1000));
        i2c_cmd_link_delete(cmd);

        return (err == ESP_OK);
    }

    bool read(uint8_t* data, size_t len) {
        if (!port_initialized[i2c_port]) return false;

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_READ, true);

        if (len > 1) {
            i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
        }
        i2c_master_read_byte(cmd, data + len - 1, I2C_MASTER_NACK);
        i2c_master_stop(cmd);

        esp_err_t err = i2c_master_cmd_begin(i2c_port, cmd, pdMS_TO_TICKS(1000));
        i2c_cmd_link_delete(cmd);

        return (err == ESP_OK);
    }

    bool ping() {
        if (!port_initialized[i2c_port]) return false;

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);

        esp_err_t err = i2c_master_cmd_begin(i2c_port, cmd, pdMS_TO_TICKS(50));
        i2c_cmd_link_delete(cmd);

        return (err == ESP_OK);
    }

private:
    i2c_port_t i2c_port;
    uint8_t address;

    static bool port_initialized[2];
};

bool I2C::port_initialized[2] = {false, false};

// ============================================================================
// I2C Slave
// ============================================================================
class I2CSlave {
public:
    explicit I2CSlave(uint8_t port, uint8_t addr)
        : i2c_port((i2c_port_t)port),
          slave_address(addr),
          rx_buffer_size(512),
          tx_buffer_size(512) {
    }

    bool begin(int sda_pin = -1, int scl_pin = -1,
               size_t rx_buf = 512, size_t tx_buf = 512) {

        // Default pins
        if (sda_pin == -1) sda_pin = (i2c_port == I2C_NUM_0) ? 21 : 33;
        if (scl_pin == -1) scl_pin = (i2c_port == I2C_NUM_0) ? 22 : 32;

        rx_buffer_size = rx_buf;
        tx_buffer_size = tx_buf;

        i2c_config_t conf = {};
        conf.mode = I2C_MODE_SLAVE;
        conf.sda_io_num = sda_pin;
        conf.scl_io_num = scl_pin;
        conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
        conf.slave.addr_10bit_en = 0;
        conf.slave.slave_addr = slave_address;
        conf.clk_flags = 0;

        esp_err_t err = i2c_param_config(i2c_port, &conf);
        if (err != ESP_OK) return false;

        err = i2c_driver_install(i2c_port, I2C_MODE_SLAVE,
                                rx_buffer_size, tx_buffer_size, 0);
        if (err != ESP_OK) return false;

        return true;
    }

    // Read data from master (blocking with timeout)
    int read(uint8_t* data, size_t max_len, uint32_t timeout_ms = 100) {
        return i2c_slave_read_buffer(i2c_port, data, max_len,
                                     pdMS_TO_TICKS(timeout_ms));
    }

    // Write data to master (blocking with timeout)
    int write(const uint8_t* data, size_t len, uint32_t timeout_ms = 100) {
        return i2c_slave_write_buffer(i2c_port, (uint8_t*)data, len,
                                      pdMS_TO_TICKS(timeout_ms));
    }

    // Flush receive buffer
    void flush() {
        uint8_t temp[128];
        while (i2c_slave_read_buffer(i2c_port, temp, 128, 0) > 0) {
            // Discard data
        }
    }

    uint8_t getAddress() const {
        return slave_address;
    }

private:
    i2c_port_t i2c_port;
    uint8_t slave_address;
    size_t rx_buffer_size;
    size_t tx_buffer_size;
};

#endif
