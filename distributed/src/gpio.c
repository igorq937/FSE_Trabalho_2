#include "gpio.h"
#include "util.h"
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


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
