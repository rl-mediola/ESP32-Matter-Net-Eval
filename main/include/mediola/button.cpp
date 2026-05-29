#include "button.hpp"
#include <app/server/Server.h>
#include <driver/gpio.h>
#include <esp_check.h>
#include <esp_log.h>

namespace mediola {

#define BTN_GPIO_NUM GPIO_NUM_28

static const char TAG[]{ "mediola" };

static void callback(void * arg)
{
    chip::Server::GetInstance().ScheduleFactoryReset();
}

extern void button_init(void)
{
    //  zero-initialize the config structure.
    gpio_config_t io_conf = {};
    // interrupt of falling edge
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    // bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = 1ULL << BTN_GPIO_NUM;
    // set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    // enable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;

    gpio_config(&io_conf);

    auto err = gpio_install_isr_service(0);
    err      = gpio_isr_handler_add(BTN_GPIO_NUM, callback, nullptr);
    ESP_RETURN_VOID_ON_ERROR(err, TAG, "gpio_isr_handler_add failed: %s", esp_err_to_name(err));
}

} // namespace mediola
