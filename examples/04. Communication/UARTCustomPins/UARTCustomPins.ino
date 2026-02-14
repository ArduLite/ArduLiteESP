/*
 * ArduLiteESP Example - UART Custom Pins
 * Use UART with custom TX/RX pins
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int TX_PIN = 17;
constexpr int RX_PIN = 16;

void main() {
  // UART2 with custom pins
  uart2.begin(9600, nullptr, TX_PIN, RX_PIN);

  uart2.sendLine("UART2 with custom pins");
  uart2.send("TX: GPIO");
  uart2.sendLine(TX_PIN);
  uart2.send("RX: GPIO");
  uart2.sendLine(RX_PIN);

  int count = 0;

  forever() {
    uart2.send("Count: ");
    uart2.sendLine(count);
    count++;
    wait(1000);
  }
}