#ifndef GPIO_H_
#define GPIO_H_


#include "json_config.h"


int gpio_init();
void gpio_read_inputs(IO *inputs, unsigned int inputs_len);


#endif /* GPIO_H_ */
