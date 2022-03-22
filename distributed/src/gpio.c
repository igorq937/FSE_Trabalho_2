#include "gpio.h"
#include "util.h"
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


static unsigned char bcm_2_wPi[] = {30, 31, 8, 9, 7, 21, 22, 11, 10, 13, 12, 14, 26, 23, 15, 16, 27, 0, 1, 24, 28, 29, 3, 4, 5, 6, 25, 2};


int bcm_to_wPi(unsigned char bcm_gpio){
	return bcm_2_wPi[bcm_gpio];
}

void gpio_set_output(int wPi, bool out){
	digitalWrite(wPi, out);
}

void gpio_read_inputs(IO *inputs, unsigned int inputs_len){
	for(unsigned int i = 0; i < inputs_len; i++){
		inputs[i].value = digitalRead(inputs[i].wPi);
	}
}

void gpio_init_outputs(IO *outputs, unsigned int outputs_len){
	for(unsigned int i = 0; i < outputs_len; i++){
		pinMode(outputs[i].wPi, OUTPUT);
	}
}

void gpio_init_inputs(IO *inputs, unsigned int inputs_len){
	for(unsigned int i = 0; i < inputs_len; i++){
		pinMode(inputs[i].wPi, INPUT);
	}
}

int gpio_init(Json_Config json_config){
	wiringPiSetup();
	gpio_init_outputs(json_config.outputs, json_config.outputs_len);
	gpio_init_inputs(json_config.inputs, json_config.inputs_len);
	return 0;
}
