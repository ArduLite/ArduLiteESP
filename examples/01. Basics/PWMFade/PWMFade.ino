/*
 * ArduLiteESP Example - PWM Fade
 * Smooth LED fading using PWM
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int LED_PIN = 2;

PWM led{ LED_PIN, 5000, 8 };  // 5kHz, 8-bit resolution

void main() {
  forever() {
    // Fade up
    for (int i = 0; i <= 255; i += 5) {
      led.write(i);
      wait(20);
    }

    // Fade down
    for (int i = 255; i >= 0; i -= 5) {
      led.write(i);
      wait(20);
    }
  }
}