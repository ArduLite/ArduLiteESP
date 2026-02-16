/*
 * ArduLiteESP Example - I2C Master Read Slave
 * Test I2C Slave using Master
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#define DEBUG
#include <ArduLiteESP.h>
#include <ArduLiteESP_I2C.h>

constexpr uint8_t I2C_PORT = 0;
constexpr uint8_t SLAVE_ADDRESS = 0x28;
constexpr uint8_t CMD_READ_COUNTER = 0x01;
constexpr size_t COUNTER_SIZE = 4;
constexpr uint32_t LOOP_DELAY_MS = 1000;

I2C slave_device{ I2C_PORT, SLAVE_ADDRESS };

void main() {
  uart.begin(115200);

  debugLine("I2C Master - Testing Slave");
  debug("Slave Address: 0x");
  debugLine(SLAVE_ADDRESS, HEX);

  if (!slave_device.begin()) {
    debugLine("I2C init failed!");
    return;
  }

  if (!slave_device.ping()) {
    debugLine("Slave not found!");
    return;
  }

  debugLine("Slave found!");
  debugLine("---");

  forever() {
    // Send command
    uint8_t cmd = CMD_READ_COUNTER;
    slave_device.write(&cmd, 1);

    wait(10);

    // Read 4 bytes response
    uint8_t data[COUNTER_SIZE];
    if (slave_device.read(data, COUNTER_SIZE)) {
      uint32_t counter = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];

      debug("Counter: ");
      debugLine(counter);
    } else {
      debugLine("Read failed!");
    }

    wait(LOOP_DELAY_MS);
  }
}