/*
 * ArduLiteESP Example - I2C Slave Sensor
 * ESP32 as I2C Slave that sends sensor data
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#define DEBUG
#include <ArduLiteESP.h>
#include <ArduLiteESP_I2C.h>

constexpr uint8_t I2C_PORT = 0;
constexpr uint8_t SLAVE_ADDRESS = 0x28;
constexpr int SENSOR_PIN = 34;
constexpr uint32_t READ_TIMEOUT_MS = 100;
constexpr uint32_t WRITE_TIMEOUT_MS = 100;
constexpr uint32_t LOOP_DELAY_MS = 10;

// Commands
constexpr uint8_t CMD_READ_ADC = 0x01;
constexpr uint8_t CMD_READ_VOLTAGE = 0x02;

// Data sizes
constexpr size_t ADC_SIZE = 2;
constexpr size_t VOLTAGE_SIZE = 4;

I2CSlave slave{ I2C_PORT, SLAVE_ADDRESS };
Analog sensor{ SENSOR_PIN };

void main() {
  uart.begin(115200);

  debugLine("I2C Slave Sensor");
  debug("Address: 0x");
  debugLine(SLAVE_ADDRESS, HEX);
  debug("Sensor Pin: GPIO");
  debugLine(SENSOR_PIN);

  if (!slave.begin()) {
    debugLine("Init failed!");
    return;
  }

  debugLine("Ready!");
  debugLine("Commands:");
  debugLine("  0x01 - Read ADC (16-bit)");
  debugLine("  0x02 - Read Voltage (float)");

  uint8_t cmd;

  forever() {
    int len = slave.read(&cmd, 1, READ_TIMEOUT_MS);

    if (len > 0) {
      debug("Command: 0x");
      debugLine(cmd, HEX);

      if (cmd == CMD_READ_ADC) {
        // Send ADC value as 2 bytes (big-endian)
        int value = sensor.read();
        uint8_t data[ADC_SIZE];
        data[0] = (value >> 8) & 0xFF;
        data[1] = value & 0xFF;

        slave.write(data, ADC_SIZE, WRITE_TIMEOUT_MS);

        debug("Sent ADC: ");
        debugLine(value);

      } else if (cmd == CMD_READ_VOLTAGE) {
        // Send voltage as float (4 bytes)
        float voltage = sensor.readVoltage();
        uint8_t* data = (uint8_t*)&voltage;

        slave.write(data, VOLTAGE_SIZE, WRITE_TIMEOUT_MS);

        debug("Sent voltage: ");
        debugLine(voltage, 2);

      } else {
        debug("Unknown command: 0x");
        debugLine(cmd, HEX);
      }
    }

    wait(LOOP_DELAY_MS);
  }
}