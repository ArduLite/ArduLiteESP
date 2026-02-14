/*
 * ArduLiteESP Example - Ultrasonic Sensor
 * Measure distance using HC-SR04 ultrasonic sensor
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int TRIG_PIN = 5;
constexpr int ECHO_PIN = 18;

Digital trig{ TRIG_PIN, OUT };
Pulse echo{ ECHO_PIN, IN };

void main() {
  uart.begin(115200);
  uart.sendLine("Ultrasonic Sensor Ready!");

  forever() {
    trig.pulse(2, 10);  // LOW 2us, HIGH 10us

    uint32_t duration = echo.read();
    float distance = duration * 0.034f / 2.0f;

    if (duration > 0) {
      uart.send("Distance: ");
      uart.send(distance, 1);
      uart.sendLine(" cm");
    } else {
      uart.sendLine("Out of range");
    }

    wait(500);
  }
}