#ifndef ARDULITEESP_PULSE_H
#define ARDULITEESP_PULSE_H

#include "ArduLiteESP_Core.h"

class Pulse {
public:
    explicit Pulse(uint8_t pin, uint8_t mode = IN, uint32_t timeout_us = 30000)
        : pin_num(pin),
          pin_mode(mode),
          mask32(1UL << (pin % 32)),
          timeout(timeout_us) {

        if (pin > 39) return;

        if (mode == OUT) {
            gpio_pullup_dis((gpio_num_t)pin);
            gpio_pulldown_dis((gpio_num_t)pin);

            if (pin < 32) GPIO.enable_w1ts = mask32;
            else GPIO.enable1_w1ts.val = mask32;
        }
        else {
            if (pin < 32) GPIO.enable_w1tc = mask32;
            else GPIO.enable1_w1tc.val = mask32;

            gpio_pullup_dis((gpio_num_t)pin);
            gpio_pulldown_dis((gpio_num_t)pin);

            if (mode == IN_PULLUP)
                gpio_pullup_en((gpio_num_t)pin);
            else if (mode == IN_PULLDOWN)
                gpio_pulldown_en((gpio_num_t)pin);
        }
    }

    uint32_t read() {
        uint64_t max_time = micros() + timeout;

        while (isHigh()) {
            if (micros() >= max_time) return 0;
        }

        while (isLow()) {
            if (micros() >= max_time) return 0;
        }

        uint64_t start = micros();
        while (isHigh()) {
            if (micros() >= max_time) return 0;
        }

        return (uint32_t)(micros() - start);
    }

    uint32_t readLow() {
        uint64_t max_time = micros() + timeout;

        while (isLow()) {
            if (micros() >= max_time) return 0;
        }

        while (isHigh()) {
            if (micros() >= max_time) return 0;
        }

        uint64_t start = micros();
        while (isLow()) {
            if (micros() >= max_time) return 0;
        }

        return (uint32_t)(micros() - start);
    }

    void setTimeout(uint32_t timeout_us) {
        timeout = timeout_us;
    }

    uint32_t getTimeout() const {
        return timeout;
    }

private:
    uint8_t pin_num;
    uint8_t pin_mode;
    uint32_t mask32;
    uint32_t timeout;

    inline bool isHigh() const {
        if (pin_num < 32) {
            return (GPIO.in >> pin_num) & 1U;
        } else {
            return (GPIO.in1.val >> (pin_num - 32)) & 1U;
        }
    }

    inline bool isLow() const {
        return !isHigh();
    }
};

#endif
