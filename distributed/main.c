#include "inc/cJSON.h"
#include "inc/json_config.h"
#include "inc/gpio.h"
#include <stdio.h>
#include <stdlib.h>


#define DHT22_01   28   // BCM GPIO 20
#define DHT22_02   29   // BCM GPIO 21


int main(int argc, char *argv[]){

    if(argc != 2){
        printf("USO:\n");
        printf("\t%s <arquivo json de configuracao>\n\n", argv[0]);
        printf("EXEMPLO:\n\t%s configuracao_andar_1.json\n", argv[0]);
        exit(1);
    }

    Json_Config json_config = json_config_parse(argv[1]);

    gpio_init(json_config);
    gpio_read_inputs(json_config.inputs, json_config.inputs_len);

    json_config_close(json_config);
    return 0 ;
}