/*
 * ArduLiteESP Example - Analog Read
 * Read analog sensor values and convert to voltage
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int SENSOR_PIN = 34;

Analog sensor{ SENSOR_PIN };

void main() {
  uart.begin(115200);

  forever() {
    int value = sensor.read();
    float voltage = sensor.readVoltage();

    uart.send("ADC: ");
    uart.send(value);
    uart.send(" | Voltage: ");
    uart.send(voltage, 2);
    uart.sendLine(" V");

    wait(1000);
  }
}