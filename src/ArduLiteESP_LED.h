#ifndef ARDULITEESP_LED_H
#define ARDULITEESP_LED_H

#include "ArduLiteESP_Core.h"

class LED {
public:
    explicit LED(uint8_t p)
        : pin(p),
          mask32(1UL << (p % 32)),
          blink_interval(0),
          last_blink_time(0),
          blink_state(false) {

        if (p > 39) return;

        gpio_pullup_dis((gpio_num_t)pin);
        gpio_pulldown_dis((gpio_num_t)pin);

        if (pin < 32) GPIO.enable_w1ts = mask32;
        else GPIO.enable1_w1ts.val = mask32;

        off();
    }

    inline void on() {
        blink_interval = 0;
        if (pin < 32) GPIO.out_w1ts = mask32;
        else GPIO.out1_w1ts.val = mask32;
    }

    inline void off() {
        blink_interval = 0;
        if (pin < 32) GPIO.out_w1tc = mask32;
        else GPIO.out1_w1tc.val = mask32;
    }

    inline void toggle() {
        if (pin < 32) GPIO.out ^= mask32;
        else GPIO.out1.val ^= mask32;
    }

    inline void write(bool state) {
        blink_interval = 0;
        state ? on() : off();
    }

    void blink(uint32_t interval_ms) {
        blink_interval = interval_ms;
        last_blink_time = millis();
        blink_state = false;
    }

    void update() {
        if (blink_interval > 0) {
            uint32_t now = millis();
            if (now - last_blink_time >= blink_interval) {
                toggle();
                blink_state = !blink_state;
                last_blink_time = now;
            }
        }
    }

    void stopBlink() {
        blink_interval = 0;
    }

    bool isBlinking() const {
        return blink_interval > 0;
    }

private:
    uint8_t  pin;
    uint32_t mask32;
    uint32_t blink_interval;
    uint32_t last_blink_time;
    bool blink_state;
};

#endif
