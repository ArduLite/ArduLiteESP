/*
 * ArduLiteESP Example - Digital Read
 * Read button state and control LED
 *
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int LED_PIN = 2;
constexpr int BUTTON_PIN = 4;

Digital led{LED_PIN, OUT};
Digital button{BUTTON_PIN, IN_PULLUP};

void main() {
  forever() {
    if (!button.read()) {  // Active LOW (pullup)
      led.on();
    } else {
      led.off();
    }
    wait(10);
  }
}
