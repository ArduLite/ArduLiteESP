/*
 * ArduLiteESP Example - UART Echo
 * Echo back received serial data
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

void onData(const char* data) {
  uart.send("Echo: ");
  uart.sendLine(data);
}

void main() {
  uart.begin(115200, onData);
  uart.sendLine("UART Echo Ready!");
  uart.sendLine("Type something...");

  forever() {
    wait(100);
  }
}