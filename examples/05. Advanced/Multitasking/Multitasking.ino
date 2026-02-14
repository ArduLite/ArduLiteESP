/*
 * ArduLiteESP Example - Multitasking
 * Run multiple tasks independently using FreeRTOS
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int LED1_PIN = 2;
constexpr int LED2_PIN = 4;
constexpr int LED3_PIN = 5;

Digital led1{ LED1_PIN, OUT };
Digital led2{ LED2_PIN, OUT };
Digital led3{ LED3_PIN, OUT };

void task1() {
  forever() {
    led1.toggle();
    wait(500);
  }
}

void task2() {
  forever() {
    led2.toggle();
    wait(300);
  }
}

void task3() {
  forever() {
    led3.toggle();
    wait(700);
  }
}

void main() {
  uart.begin(115200);
  uart.sendLine("Multitasking Demo");
  uart.sendLine("LED1: 500ms | LED2: 300ms | LED3: 700ms");

  Task t1(task1, "led1");
  Task t2(task2, "led2");
  Task t3(task3, "led3");

  forever() {
    wait(1000);
  }
}