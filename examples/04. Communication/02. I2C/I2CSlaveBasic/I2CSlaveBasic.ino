/*
 * ArduLiteESP Example - I2C Slave Basic
 * ESP32 as I2C Slave device (echo mode)
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#define DEBUG
#include <ArduLiteESP.h>
#include <ArduLiteESP_I2C.h>

constexpr uint8_t I2C_PORT = 0;
constexpr uint8_t SLAVE_ADDRESS = 0x28;
constexpr size_t BUFFER_SIZE = 128;
constexpr uint32_t READ_TIMEOUT_MS = 100;
constexpr uint32_t WRITE_TIMEOUT_MS = 100;
constexpr uint32_t LOOP_DELAY_MS = 10;

I2CSlave slave{ I2C_PORT, SLAVE_ADDRESS };

void main() {
  uart.begin(115200);

  debugLine("I2C Slave Mode");
  debug("Address: 0x");
  debugLine(SLAVE_ADDRESS, HEX);
  debugLine("---");

  if (!slave.begin()) {
    debugLine("I2C Slave init failed!");
    return;
  }

  debugLine("I2C Slave ready!");
  debugLine("Waiting for master...");

  uint8_t buffer[BUFFER_SIZE];

  forever() {
    // Read from master
    int len = slave.read(buffer, BUFFER_SIZE, READ_TIMEOUT_MS);

    if (len > 0) {
      debug("Received ");
      debug(len);
      debug(" bytes: ");

      for (int i = 0; i < len; i++) {
        debug(buffer[i], HEX);
        debug(" ");
      }
      debugLine("");

      // Echo back to master
      slave.write(buffer, len, WRITE_TIMEOUT_MS);
      debugLine("Echo sent!");
    }

    wait(LOOP_DELAY_MS);
  }
}