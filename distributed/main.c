#include "inc/cJSON.h"
#include "inc/json_config.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]){

    if(argc != 2){
        printf("USO:\n");
        printf("\t%s <arquivo json de configuracao>\n\n", argv[0]);
        printf("EXEMPLO:\n\t%s configuracao_andar_1.json\n", argv[0]);
        exit(0);
    }

    Json_Config json_config = json_config_parse(argv[1]);

    /* codigo */

    json_config_close(json_config);
    return 0 ;
}