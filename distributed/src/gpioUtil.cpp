#include "gpioUtil.hpp"
#include "socket.hpp"
#include "cJSON.h"
#include <wiringPi.h>


static int csock = -1;


static int initJanela = 0;
static int initContagem = 0;


static IO *io_Janela01 = NULL;
static IO *io_Janela02 = NULL;
static IO *io_Presenca = NULL;
static IO *io_Fumaca = NULL;
static IO *io_Porta = NULL;
static IO *io_Contagem01 = NULL;
static IO *io_Contagem02 = NULL;


static void isrJanela01(void);
static void isrJanela02(void);
static void isrPresenca(void);
static void isrFumaca(void);
static void isrPorta(void);
static void isrContagem01(void);
static void isrContagem02(void);


void gpio::input::set(int sock_){
    csock = sock_;
}

void setPinInput(IO *input, int mode, void (*isrFunc)()){
    wiringPiISR(input->getWPi(), mode, isrFunc);
    input->setValue(digitalRead(input->getWPi()));
}

void gpio::input::init(IO *input){

    if(input->getType() == "janela"){
        if(initJanela == 0){
            setPinInput(input, INT_EDGE_BOTH, &isrJanela01);
            io_Janela01 = input;
        }else{
            setPinInput(input, INT_EDGE_BOTH, &isrJanela02);
            io_Janela02 = input;
        }
        initJanela++;
    }
    
    else if(input->getType() == "presenca"){
        setPinInput(input, INT_EDGE_BOTH, &isrPresenca);
        io_Presenca = input;
    }

    else if(input->getType() == "fumaca"){
        setPinInput(input, INT_EDGE_BOTH, &isrFumaca);
        io_Fumaca = input;
    }

    else if(input->getType() == "porta"){
        setPinInput(input, INT_EDGE_BOTH, &isrPorta);
        io_Porta = input;
    }

    else if(input->getType() == "contagem"){
        if(initContagem == 0){
            setPinInput(input, INT_EDGE_RISING, &isrContagem01);
            io_Contagem01 = input;
        }else{
            setPinInput(input, INT_EDGE_RISING, &isrContagem02);
            io_Contagem02 = input;
        }
        initContagem++;
    }

    else{
        /* TODO Criar exception */
    }

}

void gpio::output::init(int wpi){
    pinMode(wpi, OUTPUT);
    digitalWrite(wpi, LOW);
}

/**
 *Atualiza valor de uma gpio a partir de uma string json.
 *
 *@param gpio_value json do formato {"gpio" : <gpio>, "value" : <1 ou 0>}
 */
void gpio::output::set(std::string gpio_value){
    cJSON *json = cJSON_Parse(gpio_value.c_str());
    cJSON *gpio = cJSON_GetObjectItemCaseSensitive(json,"gpio");
    cJSON *value = cJSON_GetObjectItemCaseSensitive(json, "value");

    if(json == NULL || gpio == NULL || value == NULL)
        return;

    digitalWrite(gpio->valueint, value->valueint);
}

void sendIOSocket(int csock, IO io){
    try{
        sock::writeIoSocket(csock, io, sock::MODE_UPDATE);
    }catch(sock::SocketException &e){
        /* TODO Tentar reconexão */
        return;
    }
}

void isrJanela01(void){
    io_Janela01->setValue((int)!io_Janela01->getValue());
    sendIOSocket(csock, *io_Janela01);
}

void isrJanela02(void){
    io_Janela02->setValue((int)!io_Janela02->getValue());
    sendIOSocket(csock, *io_Janela02);
}

void isrPresenca(void){
    io_Presenca->setValue((int)!io_Presenca->getValue());
    sendIOSocket(csock, *io_Presenca);
}

void isrFumaca(void){
    io_Fumaca->setValue((int)!io_Fumaca->getValue());
    sendIOSocket(csock, *io_Fumaca);
}

void isrPorta(void){
    io_Porta->setValue((int)!io_Porta->getValue());
    sendIOSocket(csock, *io_Porta);
}

void isrContagem01(void){
    io_Contagem01->setValue(abs(io_Contagem01->getValue()+1));
    io_Contagem02->setValue(abs(io_Contagem02->getValue()+1));
    sendIOSocket(csock, *io_Contagem01);
}

void isrContagem02(void){
    io_Contagem01->setValue(abs(io_Contagem01->getValue()-1));
    io_Contagem02->setValue(abs(io_Contagem02->getValue()-1));
    sendIOSocket(csock, *io_Contagem02);
}
