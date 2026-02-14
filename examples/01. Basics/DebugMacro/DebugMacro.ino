/*
 * ArduLiteESP Example - Debug Macro
 * Using debug macros for conditional logging
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#define DEBUG  // Enable debug mode
#include <ArduLiteESP.h>

constexpr int LED_PIN = 2;
constexpr int SENSOR_PIN = 34;

Digital led{ LED_PIN, OUT };
Analog sensor{ SENSOR_PIN };

void main() {
  uart.begin(115200);

  debugLine("Program started!");

  int count = 0;

  forever() {
    led.toggle();

    int value = sensor.read();

    debug("Count: ");
    debug(count);
    debug(" | ADC: ");
    debugLine(value);

    count++;
    wait(1000);
  }
}