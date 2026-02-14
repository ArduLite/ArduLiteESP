/*
 * ArduLiteESP Example - Button Debounce
 * Button with automatic debouncing and edge detection
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int LED_PIN = 2;
constexpr int BUTTON_PIN = 4;

LED led{LED_PIN};
Button button{BUTTON_PIN, IN_PULLUP};

void main() {
  forever() {
    if (button.pressed()) {
      led.toggle();
    }
    wait(10);
  }
}