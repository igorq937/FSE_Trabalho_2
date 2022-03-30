#include "JsonConfig.hpp"
#include "cJSON.h"

#include <iostream>
#include <fstream>
#include <filesystem>


int bcm_2_wPi[] = {30, 31, 8, 9, 7, 21, 22, 11, 10, 13, 12, 14, 26, 23, 15, 16, 27, 0, 1, 24, 28, 29, 3, 4, 5, 6, 25, 2};


void initIO(cJSON *io_array, std::vector<IO> *io_);

JsonConfig::JsonConfig(std::string json_path){

    std::ifstream jsonFile(json_path);
    std::stringstream buffer;
    buffer << jsonFile.rdbuf();

    cJSON *config = cJSON_Parse(buffer.str().c_str());
    cJSON *nome_ = cJSON_GetObjectItemCaseSensitive(config, "nome");
    cJSON *ipServidorCentral_ = cJSON_GetObjectItemCaseSensitive(config, "ip_servidor_central");
    cJSON *portaServidorCentral_ = cJSON_GetObjectItemCaseSensitive(config, "porta_servidor_central");
    cJSON *ipServidorDistribuido_ = cJSON_GetObjectItemCaseSensitive(config, "ip_servidor_distribuido");
    cJSON *portaServidorDistribuido_ = cJSON_GetObjectItemCaseSensitive(config, "porta_servidor_distribuido");
    
    nome = nome_->valuestring;
    ipServidorCentral = ipServidorCentral_->valuestring;
    portaServidorCentral = portaServidorCentral_->valueint;
    ipServidorDistribuido = ipServidorDistribuido_->valuestring;
    portaServidorDistribuido = portaServidorDistribuido_->valueint;

    cJSON *outputs_ = cJSON_GetObjectItemCaseSensitive(config, "outputs");
    cJSON *inputs_ = cJSON_GetObjectItemCaseSensitive(config, "inputs");
    cJSON *sensorTemperatura_ = cJSON_GetObjectItemCaseSensitive(config, "sensor_temperatura");

    initIO(outputs_, &outputs);
    initIO(inputs_, &inputs);

    cJSON *tmp = NULL;
    cJSON_ArrayForEach(tmp, sensorTemperatura_){
        cJSON *model = cJSON_GetObjectItemCaseSensitive(tmp, "model");
        cJSON *tag = cJSON_GetObjectItemCaseSensitive(tmp, "tag");
        cJSON *gpio = cJSON_GetObjectItemCaseSensitive(tmp, "gpio");

        sensorTemperatura = new DHT22(model->valuestring, tag->valuestring, gpio->valueint, bcm_2_wPi[gpio->valueint]);
    }

    cJSON_Delete(config);
}

JsonConfig::~JsonConfig(){
    delete sensorTemperatura;
}

std::string JsonConfig::getNome(){
    return nome;
}

std::string JsonConfig::getIpServidorCentral(){
    return ipServidorCentral;
}

int JsonConfig::getPortaServidorCentral(){
    return portaServidorCentral;
}

std::string JsonConfig::getIpServidorDistribuido(){
    return ipServidorDistribuido;
}

int JsonConfig::getPortaServidorDistribuido(){
    return portaServidorDistribuido;
}

IO* JsonConfig::getOutput(int gpio){
    for(unsigned int i = 0; i < outputs.size(); i++)
        if(gpio == outputs[i].getGpio())
            return &outputs[i];
    return NULL;
}

IO* JsonConfig::getInput(int gpio){
    for(unsigned int i = 0; i < inputs.size(); i++)
        if(gpio == inputs[i].getGpio())
            return &inputs[i];
    return NULL;
}

std::vector<IO> JsonConfig::getOutputs(){
    return outputs;
}

std::vector<IO> JsonConfig::getInputs(){
    return inputs;
}

DHT22* JsonConfig::getSensorTemperatura(){
    return sensorTemperatura;
}

void initIO(cJSON *io_array, std::vector<IO> *io_){
    cJSON *tmp = NULL;
    int index = 0;
    cJSON_ArrayForEach(tmp, io_array){
        cJSON *type = cJSON_GetObjectItemCaseSensitive(tmp, "type");
        cJSON *tag = cJSON_GetObjectItemCaseSensitive(tmp, "tag");
        cJSON *gpio = cJSON_GetObjectItemCaseSensitive(tmp, "gpio");

        (*io_).push_back(IO(type->valuestring, tag->valuestring, gpio->valueint, bcm_2_wPi[gpio->valueint], 0));
        index++;
    }
}
