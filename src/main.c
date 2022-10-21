/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyr.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>

#define MODULE main
LOG_MODULE_REGISTER(MODULE, LOG_LEVEL_DBG);

struct gpio {
    const char *name;
    struct gpio_dt_spec gpio;
    enum gpio_mode {
        MODE_UNDEFINED = 0,
        MODE_INPUT,
        MODE_OUTPUT,
        MODE_BIDIR
    } mode;
} gpios[] = {
    {"led0", GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios), MODE_OUTPUT},
    {"led1", GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios), MODE_OUTPUT},
    {"led2", GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios), MODE_OUTPUT},
    {"led3", GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios), MODE_OUTPUT},
    {"sw0", GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios), MODE_INPUT},
    {"sw1", GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios), MODE_INPUT},
    {"sw2", GPIO_DT_SPEC_GET(DT_ALIAS(sw2), gpios), MODE_INPUT},
    {"sw3", GPIO_DT_SPEC_GET(DT_ALIAS(sw3), gpios), MODE_INPUT},
    {"A0", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), analog_gpios, 0), MODE_BIDIR},
    {"A1", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), analog_gpios, 1), MODE_BIDIR},
    {"A2", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), analog_gpios, 2), MODE_BIDIR},
    {"A3", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), analog_gpios, 3), MODE_BIDIR},
    {"A4", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), analog_gpios, 4), MODE_BIDIR},
    {"A5", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), analog_gpios, 5), MODE_BIDIR},
    {"D0", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), digital_gpios, 0), MODE_BIDIR},
    {"D1", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), digital_gpios, 1), MODE_BIDIR},
    {"D2", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), digital_gpios, 2), MODE_BIDIR},
    {"D3", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), digital_gpios, 3), MODE_BIDIR},
    {"D4", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), digital_gpios, 4), MODE_BIDIR},
    {"D5", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), digital_gpios, 5), MODE_BIDIR},
    {"D6", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), digital_gpios, 6), MODE_BIDIR},
    {"D7", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), digital_gpios, 7), MODE_BIDIR},
    {"D8", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), digital_gpios, 8), MODE_BIDIR},
    {"D9", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), digital_gpios, 9), MODE_BIDIR},
    {"D10", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), digital_gpios, 10), MODE_BIDIR},
    {"D11", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), digital_gpios, 11), MODE_BIDIR},
    {"D12", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), digital_gpios, 12), MODE_BIDIR},
    {"D13", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), digital_gpios, 13), MODE_BIDIR},
    {"D14", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), digital_gpios, 14), MODE_BIDIR},
    {"D15", GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(arduino_gpios), digital_gpios, 15), MODE_BIDIR},
    {"TX", GPIO_DT_SPEC_GET(DT_NODELABEL(arduino_gpios), tx_gpios), MODE_BIDIR},
    {"RX", GPIO_DT_SPEC_GET(DT_NODELABEL(arduino_gpios), rx_gpios), MODE_BIDIR},
    {"DE", GPIO_DT_SPEC_GET(DT_NODELABEL(arduino_gpios), de_gpios), MODE_BIDIR},
};

static int mode_to_gpio(enum gpio_mode mode)
{
    switch(mode) {
        case MODE_INPUT:
            return GPIO_INPUT;
        case MODE_OUTPUT:
            return GPIO_OUTPUT;
        case MODE_BIDIR:
            return GPIO_INPUT;
        default:
            LOG_ERR("Invalid mode %d", mode);
            return MODE_INPUT;
    }
}

void main(void)
{
    for (int i = 0; i < ARRAY_SIZE(gpios); i++) {
        gpio_pin_configure_dt(&gpios[i].gpio, mode_to_gpio(gpios[i].mode));
    }
}

static struct gpio * find_gpio(const char *name)
{
    for (int i = 0; i < ARRAY_SIZE(gpios); i++) {
        if (strcmp(gpios[i].name, name) == 0)
            return &gpios[i];
    }
    LOG_ERR("GPIO %s not found", name);
    return NULL;
}

static int do_set(struct gpio *gpio, bool value)
{
    if (!gpio)
        return -1;
    if (gpio->mode == MODE_BIDIR)
        gpio_pin_configure_dt(&gpio->gpio, GPIO_OUTPUT);
    return gpio_pin_set_dt(&gpio->gpio, value);
}


static int do_get(struct gpio *gpio)
{
    if (!gpio)
        return -1;
    if (gpio->mode == MODE_BIDIR)
        gpio_pin_configure_dt(&gpio->gpio, GPIO_INPUT);
    return gpio_pin_get_dt(&gpio->gpio);
}

static int sh_set(const struct shell *shell, size_t argc, char **argv)
{
    struct gpio *gpio = find_gpio(argv[1]);
    return do_set(gpio, true);
}

static int sh_clear(const struct shell *shell, size_t argc, char **argv)
{
    struct gpio *gpio = find_gpio(argv[1]);
    return do_set(gpio, false);
}

static int sh_get(const struct shell *shell, size_t argc, char **argv)
{
    struct gpio *gpio = find_gpio(argv[1]);
    int value = do_get(gpio);
    shell_fprintf(shell, SHELL_NORMAL, "%s = %d\n", argv[1], value);
    return 0;
}

SHELL_CMD_ARG_REGISTER(set, NULL, "Set GPIO", sh_set, 2, 0);
SHELL_CMD_ARG_REGISTER(clear, NULL, "Set GPIO", sh_clear, 2, 0);
SHELL_CMD_ARG_REGISTER(get, NULL, "Get GPIO", sh_get, 2, 0);
