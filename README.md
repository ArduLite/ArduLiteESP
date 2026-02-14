# ArduLiteESP

![Version](https://img.shields.io/badge/version-0.0.1-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Platform](https://img.shields.io/badge/platform-ESP32-orange.svg)

**ArduLiteESP** is a lightweight, modern C++ framework for ESP32 embedded development. Built on top of ESP-IDF with Arduino compatibility, it provides clean and intuitive APIs with direct hardware access for maximum performance.

---

## ✨ Features

- 🚀 **Fast & Lightweight** - Direct register access for GPIO operations
- 🎯 **Modern C++** - Clean API with type safety
- 🔧 **Modular Design** - Include only what you need
- 📦 **Rich Peripherals** - Digital I/O, ADC, PWM, UART, I2C, Timers
- 🎮 **Easy to Use** - Arduino-style simplicity with ESP-IDF power
- 🔄 **FreeRTOS Support** - Built-in multitasking capabilities
- 📚 **Well Documented** - 25+ examples included

---

## 📦 Installation

### Arduino IDE
1. Download the latest release
2. In Arduino IDE: **Sketch** → **Include Library** → **Add .ZIP Library**
3. Select the downloaded file

### PlatformIO
```ini
lib_deps = 
    https://github.com/yourusername/ArduLiteESP
```

---

## 🚀 Quick Start

### Blink Example
```cpp
#include <ArduLiteESP.h>

Digital led{2, OUT};

void main() {
  forever() {
    led.toggle();
    wait(500);
  }
}
```

### Button with Debounce
```cpp
#include <ArduLiteESP.h>

LED led{2};
Button button{4, IN_PULLUP};

void main() {
  forever() {
    if (button.pressed()) {
      led.toggle();
    }
    wait(10);
  }
}
```

### Analog Read with Smoothing
```cpp
#include <ArduLiteESP.h>

Analog sensor{34};

void main() {
  uart.begin(115200);
  sensor.setSmoothFactor(0.2f);
  
  forever() {
    float voltage = sensor.readVoltageSmooth();
    uart.send("Voltage: ");
    uart.send(voltage, 2);
    uart.sendLine(" V");
    wait(100);
  }
}
```

---

## 📚 Core Classes

### Digital I/O
```cpp
Digital led{2, OUT};
led.on();
led.off();
led.toggle();
bool state = led.read();
led.pulse(2, 10);  // LOW 2us, HIGH 10us
```

### Analog (ADC)
```cpp
Analog sensor{34};
int raw = sensor.read();
float voltage = sensor.readVoltage();
int smoothed = sensor.readSmooth();
int averaged = sensor.readAverage(10);
```

### PWM
```cpp
PWM motor{25, 5000, 8};  // Pin, Freq, Resolution
motor.write(128);         // 0-255
motor.writePercent(50.0); // 0-100%
motor.fadeTo(255, 1000);  // Fade to 255 in 1s
```

### Button
```cpp
Button btn{4, IN_PULLUP};
if (btn.pressed()) { /* clicked */ }
if (btn.released()) { /* released */ }
if (btn.held(2000)) { /* held 2 seconds */ }
```

### LED
```cpp
LED led{2};
led.on();
led.blink(500);  // Auto blink 500ms
led.update();    // Call in loop
```

### Timer
```cpp
Timer timer;
timer.start();
if (timer.timeout(1000)) {
  // Every 1 second
}
```

### UART
```cpp
uart.begin(115200);
uart.sendLine("Hello!");
uart.send("Value: ");
uart.sendLine(123);

// With callback
void onData(const char* data) {
  uart.send("Received: ");
  uart.sendLine(data);
}
uart.begin(115200, onData);
```

### I2C
```cpp
#include <ArduLiteESP_I2C.h>

i2c0.begin();
i2c0.scan();
i2c0.writeByte(0x27, 0x00, 0xFF);
uint8_t data;
i2c0.readByte(0x27, 0x00, &data);
```

### Task (Multitasking)
```cpp
void task1() {
  forever() {
    led1.toggle();
    wait(500);
  }
}

void main() {
  Task t1(task1, "led1");
  Task t2(task2, "led2", 2048, 1);     // Custom stack & priority
  Task t3(task3, "led3", 2048, 1, 0);  // Pin to Core 0
}
```

---

## 📖 API Reference

### Digital
| Method | Description |
|--------|-------------|
| `on()` | Set pin HIGH |
| `off()` | Set pin LOW |
| `toggle()` | Toggle pin state |
| `read()` | Read pin state |
| `write(state)` | Write HIGH/LOW |
| `pulse(low_us, high_us)` | Send pulse |

### Analog
| Method | Description |
|--------|-------------|
| `read()` | Read raw ADC value (0-4095) |
| `readVoltage()` | Read voltage (0-3.3V) |
| `readAverage(samples)` | Average of N samples |
| `readMedian(samples)` | Median of N samples |
| `readSmooth()` | Exponential smoothing |
| `setSmoothFactor(alpha)` | Set smoothing (0.0-1.0) |

### PWM
| Method | Description |
|--------|-------------|
| `write(duty)` | Set duty cycle (0-max) |
| `writePercent(percent)` | Set duty (0-100%) |
| `writeFloat(ratio)` | Set duty (0.0-1.0) |
| `fadeTo(duty, time_ms)` | Hardware fade |
| `setFrequency(freq)` | Change frequency |

### Button
| Method | Description |
|--------|-------------|
| `read()` | Read current state |
| `pressed()` | True on press (edge) |
| `released()` | True on release (edge) |
| `held(ms)` | True if held for ms |
| `pressDuration()` | How long pressed (ms) |

### Timer
| Method | Description |
|--------|-------------|
| `start()` | Start timer |
| `stop()` | Stop timer |
| `reset()` | Reset and restart |
| `elapsed()` | Time elapsed (ms) |
| `timeout(ms)` | True every ms (auto-reset) |

---

## 📂 Examples

The library includes **25+ examples** organized by category:

### 01. Basics
- Blink
- DigitalRead
- ButtonDebounce
- LEDBlink
- Timer
- PWMFade
- AnalogRead
- UARTEcho
- DebugMacro

### 02. Sensors
- Ultrasonic
- MultipleAnalog
- AnalogSmoothing

### 03. Actuators
- BuzzerMelody
- ServoControl
- RGBLED

### 04. Communication
- UARTCallback
- UARTCustomPins
- MultipleUART

### 05. Advanced
- Multitasking
- TaskPriority
- CorePinning
- NonBlocking

### 06. Projects
- SmartLight
- DistanceAlarm
- ButtonCounter

---

## 🛠️ Hardware Support

### Supported Pins

| Function | Pins |
|----------|------|
| Digital I/O | 0-39 (except input-only) |
| ADC1 | 32, 33, 34, 35, 36, 39 |
| PWM | Any GPIO pin (16 channels) |
| UART0 | TX:1, RX:3 (default) |
| UART1 | TX:10, RX:9 (default) |
| UART2 | TX:17, RX:16 (default) |
| I2C0 | SDA:21, SCL:22 (default) |
| I2C1 | SDA:33, SCL:32 (default) |

---

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 👨‍💻 Author

**Ajang Rahmat**
- Email: ajangrahmat@gmail.com
- GitHub: [@yourusername](https://github.com/yourusername)

---

## 🙏 Acknowledgments

- Built with assistance from Claude (Anthropic)
- Inspired by Arduino framework
- Powered by ESP-IDF

---

## 📞 Support

If you have any questions or issues, please open an issue on GitHub.

---

**Made with ❤️ for the ESP32 community**
```
