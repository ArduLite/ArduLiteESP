#ifndef ARDULITEESP_TASK_H
#define ARDULITEESP_TASK_H

#include "ArduLiteESP_Core.h"

class Task {
public:
    Task(void (*func)(), const char* name) {
        create(func, name, 2048, 1);
    }

    Task(void (*func)(), const char* name, uint32_t stack_size) {
        create(func, name, stack_size, 1);
    }

    Task(void (*func)(), const char* name,
         uint32_t stack_size, uint8_t priority) {
        create(func, name, stack_size, priority);
    }

    Task(void (*func)(), const char* name,
         uint32_t stack_size, uint8_t priority, uint8_t core) {

        xTaskCreatePinnedToCore(
            task_entry,
            name,
            stack_size,
            (void*)func,
            priority,
            nullptr,
            core
        );
    }

private:
    static void task_entry(void* param) {
        void (*f)() = (void (*)())param;
        f();
        vTaskDelete(nullptr);
    }

    static void create(void (*func)(), const char* name,
                       uint32_t stack_size, uint8_t priority) {

        xTaskCreate(
            task_entry,
            name,
            stack_size,
            (void*)func,
            priority,
            nullptr
        );
    }
};

#endif
