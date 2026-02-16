/*
 * ArduLiteESP Example - I2C Slave Register Map
 * ESP32 as I2C Slave with register-based interface
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#define DEBUG
#include <ArduLiteESP.h>
#include <ArduLiteESP_I2C.h>

constexpr uint8_t I2C_PORT = 0;
constexpr uint8_t SLAVE_ADDRESS = 0x28;
constexpr uint32_t READ_TIMEOUT_MS = 100;
constexpr uint32_t WRITE_TIMEOUT_MS = 100;
constexpr uint32_t LOOP_DELAY_MS = 10;

// Register Map
constexpr uint8_t REG_DEVICE_ID = 0x00;
constexpr uint8_t REG_STATUS = 0x01;
constexpr uint8_t REG_COUNTER_H = 0x02;
constexpr uint8_t REG_COUNTER_L = 0x03;
constexpr uint8_t REG_CONTROL = 0x04;

constexpr uint8_t DEVICE_ID = 0xA5;
constexpr size_t BUFFER_SIZE = 2;

I2CSlave slave{ I2C_PORT, SLAVE_ADDRESS };
uint16_t counter = 0;
uint8_t status = 0x00;
uint8_t control = 0x00;

void main() {
  uart.begin(115200);

  debugLine("I2C Slave Register Map");
  debug("Address: 0x");
  debugLine(SLAVE_ADDRESS, HEX);
  debugLine("Register Map:");
  debugLine("  0x00 - Device ID (R)");
  debugLine("  0x01 - Status (R)");
  debugLine("  0x02 - Counter High (R)");
  debugLine("  0x03 - Counter Low (R)");
  debugLine("  0x04 - Control (R/W)");

  if (!slave.begin()) {
    debugLine("Init failed!");
    return;
  }

  debugLine("Ready!");

  uint8_t buffer[BUFFER_SIZE];

  forever() {
    int len = slave.read(buffer, BUFFER_SIZE, READ_TIMEOUT_MS);

    if (len > 0) {
      uint8_t reg = buffer[0];

      debug("Access register: 0x");
      debugLine(reg, HEX);

      if (len == 1) {
        // Read operation
        uint8_t value = 0;

        switch (reg) {
          case REG_DEVICE_ID:
            value = DEVICE_ID;
            break;
          case REG_STATUS:
            value = status;
            break;
          case REG_COUNTER_H:
            value = (counter >> 8) & 0xFF;
            break;
          case REG_COUNTER_L:
            value = counter & 0xFF;
            break;
          case REG_CONTROL:
            value = control;
            break;
          default:
            value = 0xFF;
            break;
        }

        slave.write(&value, 1, WRITE_TIMEOUT_MS);

        debug("Read 0x");
        debug(reg, HEX);
        debug(" = 0x");
        debugLine(value, HEX);

      } else if (len == 2) {
        // Write operation
        uint8_t value = buffer[1];

        if (reg == REG_CONTROL) {
          control = value;
          debug("Write 0x");
          debug(reg, HEX);
          debug(" = 0x");
          debugLine(value, HEX);
        }
      }
    }

    counter++;
    wait(LOOP_DELAY_MS);
  }
}