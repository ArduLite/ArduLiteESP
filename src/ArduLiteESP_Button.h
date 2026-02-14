#ifndef ARDULITEESP_BUTTON_H
#define ARDULITEESP_BUTTON_H

#include "ArduLiteESP_Core.h"

class Button {
public:
    explicit Button(uint8_t p, uint8_t mode = IN_PULLUP, uint16_t debounce_ms = 50)
        : pin(p),
          mask32(1UL << (p % 32)),
          debounce_time(debounce_ms),
          last_state(false),
          current_state(false),
          last_debounce_time(0),
          press_time(0),
          inverted(mode == IN_PULLUP) {

        if (p > 39) return;

        if (pin < 32) GPIO.enable_w1tc = mask32;
        else GPIO.enable1_w1tc.val = mask32;

        gpio_pullup_dis((gpio_num_t)pin);
        gpio_pulldown_dis((gpio_num_t)pin);

        if (mode == IN_PULLUP)
            gpio_pullup_en((gpio_num_t)pin);
        else if (mode == IN_PULLDOWN)
            gpio_pulldown_en((gpio_num_t)pin);

        current_state = readRaw();
        last_state = current_state;
    }

    void update() {
        bool reading = readRaw();
        uint32_t now = millis();

        if (reading != last_state) {
            last_debounce_time = now;
        }

        if ((now - last_debounce_time) > debounce_time) {
            if (reading != current_state) {
                current_state = reading;

                if (current_state) {
                    press_time = now;
                }
            }
        }

        last_state = reading;
    }

    bool read() {
        update();
        return current_state;
    }

    bool pressed() {
        update();
        bool result = (current_state && current_state != last_state);
        return result;
    }

    bool released() {
        update();
        bool result = (!current_state && current_state != last_state);
        return result;
    }

    bool held(uint32_t hold_time_ms = 1000) {
        update();
        if (current_state && press_time > 0) {
            return (millis() - press_time) >= hold_time_ms;
        }
        return false;
    }

    uint32_t pressDuration() const {
        if (current_state && press_time > 0) {
            return millis() - press_time;
        }
        return 0;
    }

private:
    uint8_t  pin;
    uint32_t mask32;
    uint16_t debounce_time;
    bool last_state;
    bool current_state;
    uint32_t last_debounce_time;
    uint32_t press_time;
    bool inverted;

    inline bool readRaw() const {
        bool state;
        if (pin < 32) state = (GPIO.in >> pin) & 1U;
        else state = (GPIO.in1.val >> (pin - 32)) & 1U;

        return inverted ? !state : state;
    }
};

#endif
