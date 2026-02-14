/*
 * ArduLiteESP Example - Smart Light System
 * LED with button control and auto-off timer
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int LED_PIN = 2;
constexpr int BUTTON_PIN = 4;

LED led{ LED_PIN };
Button button{ BUTTON_PIN, IN_PULLUP };
Timer autoOffTimer;

enum State {
  OFF,
  ON,
  AUTO_BLINK
};

State currentState = OFF;

void main() {
  uart.begin(115200);
  uart.sendLine("Smart Light System");
  uart.sendLine("Click: ON/OFF | Hold 2s: Auto Blink");

  forever() {
    // Handle button
    if (button.pressed()) {
      if (currentState == OFF) {
        currentState = ON;
        led.on();
        autoOffTimer.start();
        uart.sendLine("State: ON");
      } else if (currentState == ON) {
        currentState = OFF;
        led.off();
        autoOffTimer.stop();
        uart.sendLine("State: OFF");
      } else {
        currentState = OFF;
        led.off();
        uart.sendLine("State: OFF");
      }
    }

    if (button.held(2000)) {
      currentState = AUTO_BLINK;
      led.blink(300);
      uart.sendLine("State: AUTO BLINK");
    }

    // Auto off after 30 seconds
    if (currentState == ON && autoOffTimer.elapsed() > 30000) {
      currentState = OFF;
      led.off();
      uart.sendLine("Auto OFF (timeout)");
    }

    led.update();
    wait(10);
  }
}