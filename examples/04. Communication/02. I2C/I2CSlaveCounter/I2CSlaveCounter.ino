/*
 * ArduLiteESP Example - I2C Slave Counter
 * ESP32 as I2C Slave that responds with counter value
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

// Commands
constexpr uint8_t CMD_READ_COUNTER = 0x01;
constexpr uint8_t CMD_RESET_COUNTER = 0x02;

// Data sizes
constexpr size_t COUNTER_SIZE = 4;

I2CSlave slave{ I2C_PORT, SLAVE_ADDRESS };
uint32_t counter = 0;

void main() {
  uart.begin(115200);

  debugLine("I2C Slave Counter");
  debug("Address: 0x");
  debugLine(SLAVE_ADDRESS, HEX);

  if (!slave.begin()) {
    debugLine("Init failed!");
    return;
  }

  debugLine("Ready!");
  debugLine("Commands:");
  debugLine("  0x01 - Read counter");
  debugLine("  0x02 - Reset counter");

  uint8_t cmd;

  forever() {
    // Wait for command from master
    int len = slave.read(&cmd, 1, READ_TIMEOUT_MS);

    if (len > 0) {
      debug("Command: 0x");
      debugLine(cmd, HEX);

      if (cmd == CMD_READ_COUNTER) {
        // Send counter as 4 bytes (big-endian)
        uint8_t data[COUNTER_SIZE];
        data[0] = (counter >> 24) & 0xFF;
        data[1] = (counter >> 16) & 0xFF;
        data[2] = (counter >> 8) & 0xFF;
        data[3] = counter & 0xFF;

        slave.write(data, COUNTER_SIZE, WRITE_TIMEOUT_MS);

        debug("Sent counter: ");
        debugLine(counter);

      } else if (cmd == CMD_RESET_COUNTER) {
        // Reset counter to zero
        counter = 0;
        debugLine("Counter reset!");

      } else {
        debug("Unknown command: 0x");
        debugLine(cmd, HEX);
      }
    }

    counter++;
    wait(LOOP_DELAY_MS);
  }
}