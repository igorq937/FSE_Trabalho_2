#include "json_config.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


static unsigned char bcm_2_wPi[] = {30, 31, 8, 9, 7, 21, 22, 11, 10, 13, 12, 14, 26, 23, 15, 16, 27, 0, 1, 24, 28, 29, 3, 4, 5, 6, 25, 2};


void json_io_config_parse(cJSON *io_array, IO *io_){
    cJSON *tmp = NULL;
    int index = 0;
    cJSON_ArrayForEach(tmp, io_array){
        cJSON *type = cJSON_GetObjectItemCaseSensitive(tmp, "type");
        cJSON *tag = cJSON_GetObjectItemCaseSensitive(tmp, "tag");
        cJSON *gpio = cJSON_GetObjectItemCaseSensitive(tmp, "gpio");
        
        io_[index].type = malloc(sizeof(char)*(strlen(type->valuestring)+1));
        io_[index].tag = malloc(sizeof(char)*(strlen(tag->valuestring)+1));

        strcpy(io_[index].type, type->valuestring);
        strcpy(io_[index].tag, tag->valuestring);
        io_[index].gpio = gpio->valueint;
        io_[index].wPi = bcm_2_wPi[gpio->valueint];
        io_[index].value = false;
        index++;
    }
}

Json_Config json_config_parse(char *json_path){
    
    FILE * fp;
    struct stat info;
    stat(json_path, &info);

    char buffer[info.st_size];

    fp = fopen(json_path, "r");
    fread(&buffer, info.st_size, 1, fp);
    fclose(fp);

    cJSON *config = cJSON_Parse(buffer);
    cJSON *ip = NULL;
    cJSON *port = NULL;
    cJSON *name = NULL;
    cJSON *outputs = NULL;
    cJSON *inputs = NULL;
    Json_Config json_config;

    ip = cJSON_GetObjectItemCaseSensitive(config, "ip");
    port = cJSON_GetObjectItemCaseSensitive(config, "porta");
    name = cJSON_GetObjectItemCaseSensitive(config, "nome");
    outputs = cJSON_GetObjectItemCaseSensitive(config, "outputs");
    inputs = cJSON_GetObjectItemCaseSensitive(config, "inputs");

    json_config.name = malloc(sizeof(char)*(strlen(name->valuestring)+1));
    strcpy(json_config.name, name->valuestring);
    strcpy(json_config.ip, ip->valuestring);
    json_config.port = port->valueint;

    json_config.outputs_len = cJSON_GetArraySize(outputs);
    json_config.inputs_len = cJSON_GetArraySize(inputs);
    json_config.outputs = malloc(sizeof(IO)*json_config.outputs_len);
    json_config.inputs = malloc(sizeof(IO)*json_config.inputs_len);

    json_io_config_parse(outputs, json_config.outputs);
    json_io_config_parse(inputs, json_config.inputs);

    cJSON_Delete(config);
    return json_config;
}

void json_config_close(Json_Config json_config){

    free(json_config.name);

    for(unsigned int index = 0; index < json_config.outputs_len; index++){
        free(json_config.outputs[index].type);
        free(json_config.outputs[index].tag);
    }
    free(json_config.outputs);

    for(unsigned int index = 0; index < json_config.inputs_len; index++){
        free(json_config.inputs[index].type);
        free(json_config.inputs[index].tag);
    }
    free(json_config.inputs);
}