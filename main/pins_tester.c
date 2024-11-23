#include <stdio.h>
#include <iot_button.h>
#include "argtable3/argtable3.h"
#include "esp_console.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_private/esp_gpio_reserve.h"

const char *TAG = "pins_tester";

static void button_single_click_cb(void *arg, void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_SINGLE_CLICK");
}

struct
{
    struct arg_int *pin;
    struct arg_int *state;
    struct arg_end *end;
} pins_tester_args;

button_handle_t gpio_btn = NULL;

static void configure_button(int pin, int state)
{
    if (gpio_btn)
    {
        ESP_LOGE(TAG, "Please reset button before creating a new one");
        return;
    }
    if (state > 1 || state < 0)
    {
        ESP_LOGE(TAG, "state %i is not valid", state);
        return;
    }
    if (GPIO_IS_VALID_GPIO(pin))
    {
        if (GPIO_IS_VALID_DIGITAL_IO_PAD(pin))
        {
            if (!esp_gpio_is_pin_reserved(pin))
            {
                button_config_t gpio_btn_cfg = {
                    .type = BUTTON_TYPE_GPIO,
                    .long_press_time = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
                    .short_press_time = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
                    .gpio_button_config = {
                        .gpio_num = (int32_t)pin,
                        .active_level = (uint8_t)state,
                    },
                };
                gpio_btn = iot_button_create(&gpio_btn_cfg);
                if (NULL == gpio_btn)
                {
                    ESP_LOGE(TAG, "Button create failed");
                }
                iot_button_register_cb(gpio_btn, BUTTON_SINGLE_CLICK, button_single_click_cb, NULL);
                return;
            }
        }
    }
    ESP_LOGE(TAG, "pin %i is not valid", pin);
}

static int do_reset_cmd(int argc, char **argv)
{
    if(gpio_btn)
    {
        iot_button_unregister_cb(gpio_btn,BUTTON_SINGLE_CLICK);
        iot_button_delete(gpio_btn);
        gpio_btn = NULL;
        return 0;
    }
    ESP_LOGE(TAG, "Button is already reseted");
    return 1;

}

static int do_config_cmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&pins_tester_args);
    if (nerrors != 0)
    {
        arg_print_errors(stderr, pins_tester_args.end, argv[0]);
        return 0;
    }
    int pin = 0;
    int state = 0;
    /* Check "--pin" option */
    if (pins_tester_args.pin->count)
    {
        pin = pins_tester_args.pin->ival[0];
    }
    /* Check "--state" option */
    if (pins_tester_args.state->count)
    {
        state = pins_tester_args.state->ival[0];
    }

    ESP_LOGW(TAG, "pin chosen is = %i and state chosen is %i", pin, state);
    configure_button(pin, state);
    return 0;
}

void app_main(void)
{

    // initalize console
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();

    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));

    printf("\n ==============================================================\n");
    printf(" |             Steps to Use Pins Tester                       |\n");
    printf(" |                                                            |\n");
    printf(" |  1. Try 'help', check all supported commands               |\n");
    printf(" |                                                            |\n");
    printf(" ==============================================================\n\n");

    pins_tester_args.pin = arg_int0(NULL, "pin", "<int>", "Set the pin number");
    pins_tester_args.state = arg_int1(NULL, "state", "<0|1>", "Set the active state of the pin");
    pins_tester_args.end = arg_end(3);
    const esp_console_cmd_t config_cmd = {
        .command = "config",
        .help = "Config Button",
        .hint = NULL,
        .func = &do_config_cmd,
        .argtable = &pins_tester_args};
    ESP_ERROR_CHECK(esp_console_cmd_register(&config_cmd));

    // TODO: create a reset button cmd
    const esp_console_cmd_t reset_cmd = {
        .command = "reset",
        .help = "Reset Button",
        .hint = NULL,
        .func = &do_reset_cmd,
        .argtable = NULL};
    ESP_ERROR_CHECK(esp_console_cmd_register(&reset_cmd));

    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
