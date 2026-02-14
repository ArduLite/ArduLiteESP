/*
 * ArduLiteESP Example - Blink
 * Basic LED blinking using Digital class
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int LED_PIN = 2;
constexpr int BLINK_INTERVAL_MS = 500;

Digital led{LED_PIN, OUT};

void main() {
  forever() {
    led.toggle();
    wait(BLINK_INTERVAL_MS);
  }
}