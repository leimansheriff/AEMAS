
#ifndef AEMAS_H
#define AEMAS_H

#define BUTTON GPIO_NUM_0
#define RELAY_1 GPIO_NUM_35
#define RELAY_2 GPIO_NUM_36
#define RELAY_3 GPIO_NUM_38
#define RELAY_4 GPIO_NUM_40
#define OFF 0
#define ON 1
typedef enum {
	INIT,
	NORMAL,
	SHEDDING, 
	USER_OVERIDE
} aems_state_t;

#endif // !AEMAS_H
