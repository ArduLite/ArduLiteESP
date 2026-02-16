/*
 * ArduLiteESP Example - UART Callback
 * Process serial commands using callback function
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int LED_PIN = 2;

LED led{ LED_PIN };
int commandCount = 0;

void onCommand(const char* cmd) {
  commandCount++;

  uart.send("[");
  uart.send(commandCount);
  uart.send("] Command: ");
  uart.sendLine(cmd);

  if (strcmp(cmd, "ON") == 0) {
    led.on();
    uart.sendLine("LED ON");
  } else if (strcmp(cmd, "OFF") == 0) {
    led.off();
    uart.sendLine("LED OFF");
  } else if (strcmp(cmd, "TOGGLE") == 0) {
    led.toggle();
    uart.sendLine("LED TOGGLED");
  } else if (strcmp(cmd, "BLINK") == 0) {
    led.blink(500);
    uart.sendLine("LED BLINKING");
  } else {
    uart.sendLine("Unknown command");
  }
}

void main() {
  uart.begin(115200, onCommand);
  uart.sendLine("UART Command Interface");
  uart.sendLine("Commands: ON, OFF, TOGGLE, BLINK");

  forever() {
    led.update();
    wait(10);
  }
}