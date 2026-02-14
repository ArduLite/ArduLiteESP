/*
 * ArduLiteESP Example - RGB LED
 * Control RGB LED colors using PWM
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int RED_PIN = 25;
constexpr int GREEN_PIN = 26;
constexpr int BLUE_PIN = 27;

PWM red{ RED_PIN, 5000, 8 };
PWM green{ GREEN_PIN, 5000, 8 };
PWM blue{ BLUE_PIN, 5000, 8 };

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  red.write(r);
  green.write(g);
  blue.write(b);
}

void main() {
  uart.begin(115200);
  uart.sendLine("RGB LED Demo");

  forever() {
    uart.sendLine("Red");
    setColor(255, 0, 0);
    wait(1000);

    uart.sendLine("Green");
    setColor(0, 255, 0);
    wait(1000);

    uart.sendLine("Blue");
    setColor(0, 0, 255);
    wait(1000);

    uart.sendLine("Yellow");
    setColor(255, 255, 0);
    wait(1000);

    uart.sendLine("Cyan");
    setColor(0, 255, 255);
    wait(1000);

    uart.sendLine("Magenta");
    setColor(255, 0, 255);
    wait(1000);

    uart.sendLine("White");
    setColor(255, 255, 255);
    wait(1000);

    uart.sendLine("Off");
    setColor(0, 0, 0);
    wait(1000);
  }
}