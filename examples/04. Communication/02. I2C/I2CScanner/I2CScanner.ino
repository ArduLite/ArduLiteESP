/*
 * ArduLiteESP Example - I2C Scanner
 * Scan all I2C devices on the bus
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#define DEBUG
#include <ArduLiteESP.h>
#include <ArduLiteESP_I2C.h>

constexpr uint8_t I2C_PORT = 0;
constexpr uint8_t I2C_ADDR_START = 1;
constexpr uint8_t I2C_ADDR_END = 127;

void main() {
  uart.begin(115200);

  debugLine("I2C Scanner");

  int count = 0;
  bool bus_initialized = false;

  for (uint8_t addr = I2C_ADDR_START; addr < I2C_ADDR_END; addr++) {
    I2C device{ I2C_PORT, addr };

    // Initialize bus only once
    if (!bus_initialized) {
      if (!device.begin()) {
        debugLine("I2C init failed!");
        return;
      }
      bus_initialized = true;
    }

    // Check if device exists
    if (device.ping()) {
      debug("Device found at 0x");
      debugLine(addr, 16);  // 16 = HEX
      count++;
    }
  }

  debug("Total devices found: ");
  debugLine(count);

  if (count == 0) {
    debugLine("No devices found!");
  }

  forever() {
    wait(1000);
  }
}