/*
 * ArduLiteESP Example - LED Auto Blink
 * LED with automatic non-blocking blink
 *
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int LED_PIN = 2;

LED led{LED_PIN};

void main() {
  led.blink(500);  // Auto blink every 500ms

  forever() {
    led.update();
    wait(10);
  }
}
