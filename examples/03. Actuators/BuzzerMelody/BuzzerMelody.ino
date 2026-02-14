/*
 * ArduLiteESP Example - Buzzer Melody
 * Play musical notes using buzzer
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int BUZZER_PIN = 25;

Tone buzzer{ BUZZER_PIN };

void main() {
  uart.begin(115200);
  uart.sendLine("Playing melody...");

  forever() {
    // Play "Happy Birthday" melody
    buzzer.playNote('C', 4, 250);
    wait(50);
    buzzer.playNote('C', 4, 250);
    wait(50);
    buzzer.playNote('D', 4, 500);
    wait(50);
    buzzer.playNote('C', 4, 500);
    wait(50);
    buzzer.playNote('F', 4, 500);
    wait(50);
    buzzer.playNote('E', 4, 1000);
    wait(500);

    // Pause before repeat
    wait(2000);
  }
}