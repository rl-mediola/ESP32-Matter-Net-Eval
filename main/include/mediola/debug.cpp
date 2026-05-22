#include "debug.hpp"
#include <freertos/FreeRTOS.h>
#include <esp_log.h>

namespace mediola
{

    static constexpr const char TAG[] = "DEBUG";

    static void
    timer_callback(TimerHandle_t xTimer)
    {
        ESP_LOGI(TAG, "---------------------------------------------------------");
        ESP_LOGI(TAG, "Current free heap: Internal: %u/%u External: %u/%u",
                 static_cast<unsigned int>(heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL)),
                 static_cast<unsigned int>(heap_caps_get_total_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL)),
                 static_cast<unsigned int>(heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM)),
                 static_cast<unsigned int>(heap_caps_get_total_size(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM)));
        ESP_LOGI(TAG, "---------------------------------------------------------");
    }

    extern void debug_init(void)
    {
        auto timer = xTimerCreate("Debug Info",
                                  pdMS_TO_TICKS(5000),
                                  pdTRUE,
                                  nullptr,
                                  timer_callback);
        xTimerStart(timer, 0);
    }

}