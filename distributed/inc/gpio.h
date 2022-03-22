#ifndef GPIO_H_
#define GPIO_H_


#include "json_config.h"


int gpio_init();
void gpio_read_inputs(IO *inputs, unsigned int inputs_len);
int bcm_to_wPi(unsigned char bcm_gpio);


#endif /* GPIO_H_ */
