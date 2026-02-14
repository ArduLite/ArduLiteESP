/*
 * ArduLiteESP Example - Analog Smoothing
 * Demonstrate different analog filtering techniques
 * 
 * Written by Ajang Rahmat with assistance from Claude
 */

#include <ArduLiteESP.h>

constexpr int SENSOR_PIN = 34;

Analog sensor{ SENSOR_PIN };

void main() {
  uart.begin(115200);
  uart.sendLine("Analog Smoothing Demo");

  // Set smoothing factor (0.0 - 1.0)
  sensor.setSmoothFactor(0.2f);

  forever() {
    int raw = sensor.read();
    int smoothed = sensor.readSmooth();
    int averaged = sensor.readAverage(10);
    int median = sensor.readMedian(5);

    uart.send("Raw: ");
    uart.send(raw);
    uart.send(" | Smooth: ");
    uart.send(smoothed);
    uart.send(" | Avg: ");
    uart.send(averaged);
    uart.send(" | Median: ");
    uart.sendLine(median);

    wait(100);
  }
}