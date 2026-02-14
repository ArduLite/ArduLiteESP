/*
 * ArduLiteESP Example - Task Priority
 * Demonstrate FreeRTOS task priorities
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

void lowPriorityTask() {
  forever() {
    uart.sendLine("[LOW] Running...");
    wait(1000);
  }
}

void mediumPriorityTask() {
  forever() {
    uart.sendLine("[MEDIUM] Running...");
    wait(1000);
  }
}

void highPriorityTask() {
  forever() {
    uart.sendLine("[HIGH] Running...");
    wait(1000);
  }
}

void main() {
  uart.begin(115200);
  uart.sendLine("Task Priority Demo");
  uart.sendLine("High priority tasks run first");

  Task low(lowPriorityTask, "low", 2048, 1);        // Priority 1 (low)
  Task med(mediumPriorityTask, "medium", 2048, 3);  // Priority 3 (medium)
  Task high(highPriorityTask, "high", 2048, 5);     // Priority 5 (high)

  forever() {
    wait(1000);
  }
}