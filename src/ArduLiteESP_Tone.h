#ifndef ARDULITEESP_TONE_H
#define ARDULITEESP_TONE_H

#include "ArduLiteESP_Core.h"

class Tone {
public:
    explicit Tone(uint8_t pin)
        : gpio_pin(pin),
          channel(allocate_channel()),
          is_playing(false) {

        if (channel == 255) return;

        ledc_timer_config_t timer_conf = {};
        timer_conf.speed_mode = LEDC_LOW_SPEED_MODE;
        timer_conf.duty_resolution = LEDC_TIMER_10_BIT;
        timer_conf.timer_num = (ledc_timer_t)(channel / 2);
        timer_conf.freq_hz = 1000;
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

    ~Tone() {
        stop();
        if (channel != 255) {
            release_channel(channel);
        }
    }

    void play(uint32_t frequency) {
        if (channel == 255) return;

        ledc_set_freq(LEDC_LOW_SPEED_MODE, (ledc_timer_t)(channel / 2), frequency);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)channel, 512);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)channel);

        is_playing = true;
    }

    void play(uint32_t frequency, uint32_t duration_ms) {
        play(frequency);
        wait(duration_ms);
        stop();
    }

    void stop() {
        if (channel == 255) return;

        ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)channel, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)channel);

        is_playing = false;
    }

    bool isPlaying() const {
        return is_playing;
    }

    void playNote(char note, uint8_t octave = 4, uint32_t duration_ms = 0) {
        uint32_t freq = getNoteFrequency(note, octave);
        if (duration_ms > 0) {
            play(freq, duration_ms);
        } else {
            play(freq);
        }
    }

private:
    uint8_t gpio_pin;
    uint8_t channel;
    bool is_playing;

    static uint8_t channel_mask;

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

    static uint32_t getNoteFrequency(char note, uint8_t octave) {
        const uint32_t base_freq[] = {
            262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494
        };

        uint8_t note_index = 0;
        switch (note) {
            case 'C': case 'c': note_index = 0; break;
            case 'D': case 'd': note_index = 2; break;
            case 'E': case 'e': note_index = 4; break;
            case 'F': case 'f': note_index = 5; break;
            case 'G': case 'g': note_index = 7; break;
            case 'A': case 'a': note_index = 9; break;
            case 'B': case 'b': note_index = 11; break;
            default: return 440;
        }

        uint32_t freq = base_freq[note_index];

        if (octave > 4) {
            freq <<= (octave - 4);
        } else if (octave < 4) {
            freq >>= (4 - octave);
        }

        return freq;
    }
};

uint8_t Tone::channel_mask = 0;

#endif
