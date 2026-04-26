
#ifndef MK3_H
#define MK3_H

#include <stddef.h>
#include <stdint.h>
typedef struct {
	float voltage;
	float current;
	float soc;

	float power_out;
} sys_info_t;

void victron_parser(const uint8_t *buffer, size_t data_len, void *arg);
#endif // !MK3_H
#define MK3_H

