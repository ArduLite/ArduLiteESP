/*
 * ArduLiteESP Example - Distance Alarm
 * Ultrasonic distance sensor with alarm system
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int TRIG_PIN = 5;
constexpr int ECHO_PIN = 18;
constexpr int BUZZER_PIN = 25;
constexpr int LED_PIN = 2;

constexpr float ALARM_DISTANCE = 20.0f;  // cm

Digital trig{ TRIG_PIN, OUT };
Pulse echo{ ECHO_PIN, IN };
Tone buzzer{ BUZZER_PIN };
LED led{ LED_PIN };

void main() {
  uart.begin(115200);
  uart.sendLine("Distance Alarm System");
  uart.send("Alarm threshold: ");
  uart.send(ALARM_DISTANCE, 1);
  uart.sendLine(" cm");

  forever() {
    trig.pulse(2, 10);
    uint32_t duration = echo.read();
    float distance = duration * 0.034f / 2.0f;

    if (duration > 0) {
      uart.send("Distance: ");
      uart.send(distance, 1);
      uart.send(" cm");

      if (distance < ALARM_DISTANCE) {
        // Alarm!
        led.on();
        buzzer.play(1000);
        uart.sendLine(" - ALARM!");
      } else {
        // Safe
        led.off();
        buzzer.stop();
        uart.sendLine(" - OK");
      }
    }

    wait(200);
  }
}