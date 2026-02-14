/*
 * ArduLiteESP Example - Core Pinning
 * Pin tasks to specific CPU cores
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int LED1_PIN = 2;
constexpr int LED2_PIN = 4;

Digital led1{ LED1_PIN, OUT };
Digital led2{ LED2_PIN, OUT };

void core0Task() {
  forever() {
    led1.toggle();
    uart.sendLine("[Core 0] LED1 toggled");
    wait(500);
  }
}

void core1Task() {
  forever() {
    led2.toggle();
    uart.sendLine("[Core 1] LED2 toggled");
    wait(700);
  }
}

void main() {
  uart.begin(115200);
  uart.sendLine("Core Pinning Demo");
  uart.sendLine("Task 1 on Core 0, Task 2 on Core 1");

  Task t0(core0Task, "core0", 2048, 1, 0);  // Pin to Core 0
  Task t1(core1Task, "core1", 2048, 1, 1);  // Pin to Core 1

  forever() {
    wait(1000);
  }
}