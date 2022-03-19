#ifndef JSON_CONFIG_H_
#define JSON_CONFIG_H_


typedef struct {
    char *type;
    char *tag;
    int gpio;
} IO;

typedef struct {
    char ip[16];
    int port;
    char *name;
    IO *outputs;
    IO *inputs;
    unsigned int outputs_len;
    unsigned int inputs_len;
} Json_Config;


Json_Config json_config_parse(char *json_path);
void json_config_close(Json_Config json_config);


#endif /* JSON_CONFIG_H_ */