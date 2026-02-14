/*
 * ArduLiteESP Example - Non-Blocking
 * Perform multiple tasks without blocking delays
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int LED1_PIN = 2;
constexpr int LED2_PIN = 4;
constexpr int BUTTON_PIN = 5;

LED led1{ LED1_PIN };
LED led2{ LED2_PIN };
Button button{ BUTTON_PIN, IN_PULLUP };
Timer timer;

void main() {
  uart.begin(115200);
  uart.sendLine("Non-Blocking Demo");

  led1.blink(500);
  led2.blink(300);
  timer.start();

  forever() {
    // Non-blocking LED updates
    led1.update();
    led2.update();

    // Non-blocking button check
    if (button.pressed()) {
      uart.sendLine("Button pressed!");
    }

    // Non-blocking timer check
    if (timer.timeout(5000)) {
      uart.send("Uptime: ");
      uart.send(millis() / 1000);
      uart.sendLine(" seconds");
    }

    wait(10);
  }
}