
#ifndef AEMAS_H
#define AEMAS_H

#include "driver/gpio.h"
#define BUTTON GPIO_NUM_0
#define RELAY_1 GPIO_NUM_5
#define RELAY_2 GPIO_NUM_6
#define RELAY_3 GPIO_NUM_7
#define RELAY_4 GPIO_NUM_8
typedef enum {
	INIT,
	NORMAL,
	SHEDDING, 
	USER_OVERIDE
} aems_state_t;


#endif // !AEMAS_H
#define AEMAS_H
