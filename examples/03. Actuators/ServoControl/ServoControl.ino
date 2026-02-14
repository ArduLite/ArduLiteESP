/*
 * ArduLiteESP Example - Servo Control
 * Control servo motor position using potentiometer
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int SERVO_PIN = 18;
constexpr int POT_PIN = 34;

PWM servo{ SERVO_PIN, 50, 16 };  // 50Hz, 16-bit for servo
Analog pot{ POT_PIN };

void main() {
  uart.begin(115200);
  uart.sendLine("Servo Control with Potentiometer");

  forever() {
    int potValue = pot.read();

    // Map 0-4095 to servo range (1000-2000us = 1638-3277 at 16-bit, 50Hz)
    uint32_t servoValue = 1638 + (potValue * 1639 / 4095);

    servo.write(servoValue);

    uart.send("Pot: ");
    uart.send(potValue);
    uart.send(" | Servo: ");
    uart.sendLine(servoValue);

    wait(50);
  }
}