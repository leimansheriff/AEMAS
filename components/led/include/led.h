
#ifndef LED_CONF_H
#define LED_CONF_H
#include "led_strip.h"
#include "led_strip_types.h"
#define LED GPIO_NUM_48
extern led_strip_handle_t led_strip;
void configure_led(void);
#endif // !LED_CONF_H
