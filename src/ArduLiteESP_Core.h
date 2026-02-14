#ifndef ARDULITEESP_CORE_H
#define ARDULITEESP_CORE_H

#define forever() for(;;)

#ifdef ARDUINO
  #define main ardulite_user_main
  void ardulite_user_main();
  void setup() { ardulite_user_main(); }
  void loop() {}
#else
  #define main app_main
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/ledc.h"
#include "esp_timer.h"
#include <stdlib.h>

#ifdef __cplusplus
}
#endif

// ============================================================================
// Pin Modes
// ============================================================================
#define IN              0
#define OUT             1
#define IN_PULLUP       2
#define IN_PULLDOWN     3

#define INPUT           IN
#define OUTPUT          OUT
#define INPUT_PULLUP    IN_PULLUP
#define INPUT_PULLDOWN  IN_PULLDOWN

// ============================================================================
// Timing
// ============================================================================
inline void wait(uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

#ifndef ARDUINO
inline uint32_t millis() {
    return (uint32_t)(esp_timer_get_time() / 1000ULL);
}

inline uint64_t micros() {
    return esp_timer_get_time();
}

inline void randomSeed(uint32_t seed) {
    srand(seed);
}

inline int32_t random(int32_t max) {
    return rand() % max;
}

inline int32_t random(int32_t min, int32_t max) {
    return min + (rand() % (max - min));
}
#endif

// ============================================================================
// Digital Pin
// ============================================================================
class Digital {
public:
    explicit Digital(uint8_t p, uint8_t mode)
        : pin(p),
          mask32(1UL << (p % 32)) {

        if (p > 39) return;

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

    inline void on() {
        if (pin < 32) GPIO.out_w1ts = mask32;
        else GPIO.out1_w1ts.val = mask32;
    }

    inline void off() {
        if (pin < 32) GPIO.out_w1tc = mask32;
        else GPIO.out1_w1tc.val = mask32;
    }

    inline void toggle() {
        if (pin < 32) GPIO.out ^= mask32;
        else GPIO.out1.val ^= mask32;
    }

    inline bool read() const {
        if (pin < 32) return (GPIO.in >> pin) & 1U;
        else return (GPIO.in1.val >> (pin - 32)) & 1U;
    }

    inline void write(bool state) {
        state ? on() : off();
    }

    inline void pulse(uint32_t low_us, uint32_t high_us) {
        off();
        ets_delay_us(low_us);
        on();
        ets_delay_us(high_us);
        off();
    }

private:
    uint8_t  pin;
    uint32_t mask32;
};

// ============================================================================
// Timer
// ============================================================================
class Timer {
public:
    Timer() : start_time(0), running(false) {}

    void start() {
        start_time = millis();
        running = true;
    }

    void reset() {
        start();
    }

    void stop() {
        running = false;
    }

    uint32_t elapsed() const {
        if (!running) return 0;
        return millis() - start_time;
    }

    bool timeout(uint32_t ms) {
        if (elapsed() >= ms) {
            reset();
            return true;
        }
        return false;
    }

    bool isRunning() const {
        return running;
    }

private:
    uint32_t start_time;
    bool running;
};

// ============================================================================
// Analog (ADC1 Only - ESP32)
// ============================================================================
class Analog {
public:
    explicit Analog(int pin)
        : gpio((gpio_num_t)pin),
          channel(gpio_to_adc1_channel((gpio_num_t)pin)),
          samples(10),
          smooth_alpha(0.2f),
          smooth_value(-1.0f) {

        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(channel, ADC_ATTEN_DB_11);
    }

    int read() const {
        return adc1_get_raw(channel);
    }

    float readVoltage() const {
        return (read() * 3.3f) / 4095.0f;
    }

    int readAverage(uint16_t num_samples = 0) const {
        if (num_samples == 0) num_samples = samples;

        uint32_t sum = 0;
        for (uint16_t i = 0; i < num_samples; i++) {
            sum += adc1_get_raw(channel);
            vTaskDelay(1);
        }
        return sum / num_samples;
    }

    int readMedian(uint8_t num_samples = 5) {
        int readings[num_samples];

        for (uint8_t i = 0; i < num_samples; i++) {
            readings[i] = adc1_get_raw(channel);
            vTaskDelay(1);
        }

        for (uint8_t i = 0; i < num_samples - 1; i++) {
            for (uint8_t j = 0; j < num_samples - i - 1; j++) {
                if (readings[j] > readings[j + 1]) {
                    int temp = readings[j];
                    readings[j] = readings[j + 1];
                    readings[j + 1] = temp;
                }
            }
        }

        return readings[num_samples / 2];
    }

    int readSmooth(float alpha = 0.0f) {
        if (alpha > 0.0f) smooth_alpha = alpha;

        int raw = adc1_get_raw(channel);

        if (smooth_value < 0.0f) {
            smooth_value = raw;
            return raw;
        }

        smooth_value = smooth_alpha * raw + (1.0f - smooth_alpha) * smooth_value;
        return (int)(smooth_value + 0.5f);
    }

    float readVoltageAverage(uint16_t num_samples = 0) const {
        return (readAverage(num_samples) * 3.3f) / 4095.0f;
    }

    float readVoltageMedian(uint8_t num_samples = 5) {
        return (readMedian(num_samples) * 3.3f) / 4095.0f;
    }

    float readVoltageSmooth(float alpha = 0.0f) {
        return (readSmooth(alpha) * 3.3f) / 4095.0f;
    }

    void setSamples(uint16_t num) {
        samples = num;
    }

    void setSmoothFactor(float alpha) {
        smooth_alpha = alpha;
    }

    void resetSmooth() {
        smooth_value = -1.0f;
    }

private:
    gpio_num_t gpio;
    adc1_channel_t channel;
    uint16_t samples;
    float smooth_alpha;
    mutable float smooth_value;

    static adc1_channel_t gpio_to_adc1_channel(gpio_num_t pin) {
        switch (pin) {
            case GPIO_NUM_32: return ADC1_CHANNEL_4;
            case GPIO_NUM_33: return ADC1_CHANNEL_5;
            case GPIO_NUM_34: return ADC1_CHANNEL_6;
            case GPIO_NUM_35: return ADC1_CHANNEL_7;
            case GPIO_NUM_36: return ADC1_CHANNEL_0;
            case GPIO_NUM_39: return ADC1_CHANNEL_3;
            default: return ADC1_CHANNEL_0;
        }
    }
};

// ============================================================================
// PWM (LEDC - LED Controller)
// ============================================================================
class PWM {
public:
    explicit PWM(uint8_t pin, uint32_t freq = 5000, uint8_t resolution = 8)
        : gpio_pin(pin),
          frequency(freq),
          res_bits(resolution),
          max_duty((1 << resolution) - 1),
          channel(allocate_channel()) {

        if (channel == 255) return;

        if (!fade_installed) {
            ledc_fade_func_install(0);
            fade_installed = true;
        }

        ledc_timer_config_t timer_conf = {};
        timer_conf.speed_mode = LEDC_LOW_SPEED_MODE;
        timer_conf.duty_resolution = (ledc_timer_bit_t)res_bits;
        timer_conf.timer_num = (ledc_timer_t)(channel / 2);
        timer_conf.freq_hz = frequency;
        timer_conf.clk_cfg = LEDC_AUTO_CLK;
        ledc_timer_config(&timer_conf);

        ledc_channel_config_t channel_conf = {};
        channel_conf.gpio_num = pin;
        channel_conf.speed_mode = LEDC_LOW_SPEED_MODE;
        channel_conf.channel = (ledc_channel_t)channel;
        channel_conf.timer_sel = (ledc_timer_t)(channel / 2);
        channel_conf.duty = 0;
        channel_conf.hpoint = 0;
        ledc_channel_config(&channel_conf);
    }

    ~PWM() {
        if (channel != 255) {
            ledc_stop(LEDC_LOW_SPEED_MODE, (ledc_channel_t)channel, 0);
            release_channel(channel);
        }
    }

    void write(uint32_t duty) {
        if (channel == 255) return;
        if (duty > max_duty) duty = max_duty;

        ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)channel, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)channel);
    }

    void writePercent(float percent) {
        if (percent < 0.0f) percent = 0.0f;
        if (percent > 100.0f) percent = 100.0f;

        uint32_t duty = (uint32_t)((percent / 100.0f) * max_duty);
        write(duty);
    }

    void writeFloat(float ratio) {
        if (ratio < 0.0f) ratio = 0.0f;
        if (ratio > 1.0f) ratio = 1.0f;

        uint32_t duty = (uint32_t)(ratio * max_duty);
        write(duty);
    }

    uint32_t read() const {
        if (channel == 255) return 0;
        return ledc_get_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)channel);
    }

    void setFrequency(uint32_t freq) {
        if (channel == 255) return;
        frequency = freq;
        ledc_set_freq(LEDC_LOW_SPEED_MODE, (ledc_timer_t)(channel / 2), freq);
    }

    void fadeTo(uint32_t target_duty, uint32_t fade_time_ms) {
        if (channel == 255) return;
        if (target_duty > max_duty) target_duty = max_duty;

        ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE,
                                (ledc_channel_t)channel,
                                target_duty,
                                fade_time_ms);
        ledc_fade_start(LEDC_LOW_SPEED_MODE, (ledc_channel_t)channel, LEDC_FADE_NO_WAIT);
    }

    void fadeToPercent(float percent, uint32_t fade_time_ms) {
        if (percent < 0.0f) percent = 0.0f;
        if (percent > 100.0f) percent = 100.0f;

        uint32_t duty = (uint32_t)((percent / 100.0f) * max_duty);
        fadeTo(duty, fade_time_ms);
    }

    void stop() {
        if (channel == 255) return;
        ledc_stop(LEDC_LOW_SPEED_MODE, (ledc_channel_t)channel, 0);
    }

    uint32_t getMaxDuty() const {
        return max_duty;
    }

    uint8_t getResolution() const {
        return res_bits;
    }

    uint32_t getFrequency() const {
        return frequency;
    }

private:
    uint8_t  gpio_pin;
    uint32_t frequency;
    uint8_t  res_bits;
    uint32_t max_duty;
    uint8_t  channel;

    static uint8_t channel_mask;
    static bool fade_installed;

    static uint8_t allocate_channel() {
        for (uint8_t i = 0; i < 16; i++) {
            if (!(channel_mask & (1 << i))) {
                channel_mask |= (1 << i);
                return i;
            }
        }
        return 255;
    }

    static void release_channel(uint8_t ch) {
        if (ch < 16) {
            channel_mask &= ~(1 << ch);
        }
    }
};

uint8_t PWM::channel_mask = 0;
bool PWM::fade_installed = false;

#endif
