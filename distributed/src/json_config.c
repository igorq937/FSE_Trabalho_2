#include "json_config.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


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
    cJSON *io = NULL;
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

    int index = 0;
    cJSON_ArrayForEach(io, outputs){
        cJSON *type = cJSON_GetObjectItemCaseSensitive(io, "type");
        cJSON *tag = cJSON_GetObjectItemCaseSensitive(io, "tag");
        cJSON *gpio = cJSON_GetObjectItemCaseSensitive(io, "gpio");
        
        json_config.outputs[index].type = malloc(sizeof(char)*(strlen(type->valuestring)+1));
        json_config.outputs[index].tag = malloc(sizeof(char)*(strlen(tag->valuestring)+1));

        strcpy(json_config.outputs[index].type, type->valuestring);
        strcpy(json_config.outputs[index].tag, tag->valuestring);
        json_config.outputs[index].gpio = gpio->valueint;
        index++;
    }

    index = 0;
    cJSON_ArrayForEach(io, inputs){
        cJSON *type = cJSON_GetObjectItemCaseSensitive(io, "type");
        cJSON *tag = cJSON_GetObjectItemCaseSensitive(io, "tag");
        cJSON *gpio = cJSON_GetObjectItemCaseSensitive(io, "gpio");
        
        json_config.inputs[index].type = malloc(sizeof(char)*(strlen(type->valuestring)+1));
        json_config.inputs[index].tag = malloc(sizeof(char)*(strlen(tag->valuestring)+1));

        strcpy(json_config.inputs[index].type, type->valuestring);
        strcpy(json_config.inputs[index].tag, tag->valuestring);
        json_config.inputs[index].gpio = gpio->valueint;
        index++;
    }

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