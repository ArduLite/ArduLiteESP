/*
 * ArduLiteESP Example - Timer
 * Non-blocking timer for periodic tasks
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int LED_PIN = 2;

Digital led{ LED_PIN, OUT };
Timer timer;

void main() {
  uart.begin(115200);
  timer.start();

  forever() {
    if (timer.timeout(1000)) {
      led.toggle();
      uart.send("Elapsed: ");
      uart.send(timer.elapsed());
      uart.sendLine(" ms");
    }
    wait(10);
  }
}