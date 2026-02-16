#ifndef ARDULITEESP_UART_H
#define ARDULITEESP_UART_H

#include "ArduLiteESP_Core.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/uart.h"
#include "freertos/queue.h"
#include <string.h>

#ifdef __cplusplus
}
#endif

class UART {
public:
    // Configuration constants (no magic numbers)
    inline static constexpr size_t BUFFER_SIZE = 64;
    inline static constexpr size_t NUMERIC_BUF_SIZE = 34; // enough for 32-bit binary + terminator
    inline static constexpr size_t FLOAT_BUF_SIZE = 16;
    inline static constexpr size_t DTOSTRF_FMT_SIZE = 24;

    inline static constexpr int UART_DRIVER_BUF_SIZE = 1024;
    inline static constexpr int UART_RX_QUEUE_LENGTH = 10;
    inline static constexpr int UART_RX_TASK_STACK = 2048;
    inline static constexpr UBaseType_t UART_RX_TASK_PRIO = 5;
    inline static constexpr int UART_RX_FLOW_CTRL_THRESH = 122;
    inline static constexpr int UART_READ_NO_WAIT_MS = 0;

    static inline int8_t default_tx_pin(uart_port_t port) {
        switch (port) {
            case UART_NUM_0: return 1;
            case UART_NUM_1: return 10;
            case UART_NUM_2: return 17;
            default: return -1;
        }
    }

    static inline int8_t default_rx_pin(uart_port_t port) {
        switch (port) {
            case UART_NUM_0: return 3;
            case UART_NUM_1: return 9;
            case UART_NUM_2: return 16;
            default: return -1;
        }
    }

    explicit UART(uart_port_t port)
        : uart_num(port),
          data_callback(nullptr),
          rx_queue(nullptr),
          rx_task_handle(nullptr),
          index(0) {
        buffer[0] = '\0';
    }

    ~UART() {
        if (rx_task_handle) {
            vTaskDelete(rx_task_handle);
            rx_task_handle = nullptr;
        }
        uart_driver_delete(uart_num);
    }

    void begin(uint32_t baud, void (*callback)(const char *) = nullptr,
               int8_t tx_pin = -1, int8_t rx_pin = -1) {

        data_callback = callback;

        if (tx_pin == -1) tx_pin = default_tx_pin(uart_num);
        if (rx_pin == -1) rx_pin = default_rx_pin(uart_num);

        uart_config_t uart_config = {};
        uart_config.baud_rate = (int)baud;
        uart_config.data_bits = UART_DATA_8_BITS;
        uart_config.parity = UART_PARITY_DISABLE;
        uart_config.stop_bits = UART_STOP_BITS_1;
        uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
        uart_config.rx_flow_ctrl_thresh = UART_RX_FLOW_CTRL_THRESH;
        uart_config.source_clk = UART_SCLK_APB;

        uart_param_config(uart_num, &uart_config);
        uart_set_pin(uart_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        uart_driver_install(uart_num, UART_DRIVER_BUF_SIZE, UART_DRIVER_BUF_SIZE,
                    UART_RX_QUEUE_LENGTH, &rx_queue, 0);

        if (data_callback) {
            xTaskCreate(
                rx_task_entry,
                "uart_rx",
                UART_RX_TASK_STACK,
                this,
                UART_RX_TASK_PRIO,
                &rx_task_handle
            );
        }
    }

    void send(char data) {
        uart_write_bytes(uart_num, &data, 1);
    }

    void send(const char *str) {
        uart_write_bytes(uart_num, str, strlen(str));
    }

    void send(int32_t data) {
        char buf[12];
        itoa(data, buf, 10);
        send(buf);
    }

    void send(uint32_t data) {
        char buf[12];
        utoa(data, buf, 10);
        send(buf);
    }

    void send(uint16_t data) {
        send((uint32_t)data);
    }

    void send(int16_t data) {
        send((int32_t)data);
    }

    void send(uint8_t data) {
        send((uint32_t)data);
    }

    void send(int8_t data) {
        send((int32_t)data);
    }

    // Send with base (HEX, BIN, OCT, DEC)
    void send(uint32_t data, uint8_t base) {
        char buf[NUMERIC_BUF_SIZE];
        utoa(data, buf, base);
        send(buf);
    }

    void send(uint16_t data, uint8_t base) {
        send((uint32_t)data, base);
    }

    void send(uint8_t data, uint8_t base) {
        send((uint32_t)data, base);
    }

    void send(int32_t data, uint8_t base) {
        if (data < 0 && base == 10) {
            send('-');
            data = -data;
        }
        send((uint32_t)data, base);
    }

    void send(int16_t data, uint8_t base) {
        send((int32_t)data, base);
    }

    void send(int8_t data, uint8_t base) {
        send((int32_t)data, base);
    }

    void send(float data, uint8_t decimals = 2) {
        char buf[FLOAT_BUF_SIZE];
        dtostrf(data, 1, decimals, buf);
        send(buf);
    }

    void send(double data, uint8_t decimals = 2) {
        send((float)data, decimals);
    }

    void send(bool data) {
        send(data ? "true" : "false");
    }

    void sendLine(char data) {
        send(data);
        send("\r\n");
    }

    void sendLine(const char *str) {
        send(str);
        send("\r\n");
    }

    void sendLine(int32_t data) {
        send(data);
        send("\r\n");
    }

    void sendLine(uint32_t data) {
        send(data);
        send("\r\n");
    }

    void sendLine(uint16_t data) {
        send(data);
        send("\r\n");
    }

    void sendLine(int16_t data) {
        send(data);
        send("\r\n");
    }

    void sendLine(uint8_t data) {
        send(data);
        send("\r\n");
    }

    void sendLine(int8_t data) {
        send(data);
        send("\r\n");
    }

    // SendLine with base (HEX, BIN, OCT, DEC)
    void sendLine(uint32_t data, uint8_t base) {
        send(data, base);
        send("\r\n");
    }

    void sendLine(uint16_t data, uint8_t base) {
        send(data, base);
        send("\r\n");
    }

    void sendLine(uint8_t data, uint8_t base) {
        send(data, base);
        send("\r\n");
    }

    void sendLine(int32_t data, uint8_t base) {
        send(data, base);
        send("\r\n");
    }

    void sendLine(int16_t data, uint8_t base) {
        send(data, base);
        send("\r\n");
    }

    void sendLine(int8_t data, uint8_t base) {
        send(data, base);
        send("\r\n");
    }

    void sendLine(float data, uint8_t decimals = 2) {
        send(data, decimals);
        send("\r\n");
    }

    void sendLine(double data, uint8_t decimals = 2) {
        send(data, decimals);
        send("\r\n");
    }

    void sendLine(bool data) {
        send(data);
        send("\r\n");
    }

    int available() {
        size_t available_len;
        uart_get_buffered_data_len(uart_num, &available_len);
        return (int)available_len;
    }

    int read() {
        uint8_t data;
        int len = uart_read_bytes(uart_num, &data, 1,
                                  pdMS_TO_TICKS(UART_READ_NO_WAIT_MS));
        return (len > 0) ? data : -1;
    }

    int read(char *buffer, size_t length, uint32_t timeout_ms = 100) {
        return uart_read_bytes(uart_num, (uint8_t*)buffer, length,
                              pdMS_TO_TICKS(timeout_ms));
    }

    void flush() {
        uart_flush(uart_num);
    }

private:
    uart_port_t uart_num;
    void (*data_callback)(const char *);
    QueueHandle_t rx_queue;
    TaskHandle_t rx_task_handle;

    char buffer[BUFFER_SIZE];
    uint8_t index;

    static void rx_task_entry(void *param) {
        UART *self = (UART*)param;
        uart_event_t event;
        uint8_t data;

        while (true) {
            if (xQueueReceive(self->rx_queue, &event, portMAX_DELAY)) {
                if (event.type == UART_DATA) {
                    while (uart_read_bytes(self->uart_num, &data, 1, 0) > 0) {
                        self->receive((char)data);
                    }
                }
            }
        }
    }

    void receive(char received) {
        if (received == '\n' || received == '\r') {
            if (index > 0) {
                buffer[index] = '\0';
                if (data_callback) {
                    data_callback(buffer);
                }
                index = 0;
            }
        } else if (index < sizeof(buffer) - 1) {
            buffer[index++] = received;
        } else {
            index = 0;
        }
    }

    static char* dtostrf(double val, signed char width, unsigned char prec, char *s) {
        char fmt[DTOSTRF_FMT_SIZE];
        snprintf(fmt, sizeof(fmt), "%%%d.%df", width, prec);
        snprintf(s, FLOAT_BUF_SIZE, fmt, val);
        return s;
    }
};

UART uart(UART_NUM_0);
UART uart1(UART_NUM_1);
UART uart2(UART_NUM_2);

// Debug Macros
#ifdef DEBUG
    // Single parameter
    template<typename T>
    inline void debug(T data) {
        uart.send(data);
    }

    // Two parameters (for base)
    template<typename T>
    inline void debug(T data, uint8_t param) {
        uart.send(data, param);
    }

    // Single parameter
    template<typename T>
    inline void debugLine(T data) {
        uart.sendLine(data);
    }

    // Two parameters (for decimals/base)
    template<typename T>
    inline void debugLine(T data, uint8_t param) {
        uart.sendLine(data, param);
    }
#else
    template<typename T>
    inline void debug(T data) {}

    template<typename T>
    inline void debug(T data, uint8_t param) {}

    template<typename T>
    inline void debugLine(T data) {}

    template<typename T>
    inline void debugLine(T data, uint8_t param) {}
#endif

#endif
