/*
 * ArduLiteESP Example - Multiple Analog Sensors
 * Read multiple analog sensors simultaneously
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int SENSOR1_PIN = 34;
constexpr int SENSOR2_PIN = 35;
constexpr int SENSOR3_PIN = 32;

Analog sensor1{ SENSOR1_PIN };
Analog sensor2{ SENSOR2_PIN };
Analog sensor3{ SENSOR3_PIN };

void main() {
  uart.begin(115200);
  uart.sendLine("Multiple Analog Sensors");

  forever() {
    float v1 = sensor1.readVoltage();
    float v2 = sensor2.readVoltage();
    float v3 = sensor3.readVoltage();

    uart.send("S1: ");
    uart.send(v1, 2);
    uart.send("V | S2: ");
    uart.send(v2, 2);
    uart.send("V | S3: ");
    uart.send(v3, 2);
    uart.sendLine("V");

    wait(500);
  }
}