#include "isrFuncs.hpp"
#include <wiringPi.h>


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


void isrFuncs::init(IO *input){

    if(input->getType() == "janela"){
        if(initJanela == 0){
            wiringPiISR(input->getWPi(), INT_EDGE_BOTH, &isrJanela01);
            io_Janela01 = input;
        }else{
            wiringPiISR(input->getWPi(), INT_EDGE_BOTH, &isrJanela02);
            io_Janela02 = input;
        }
        initJanela++;

    }else if(input->getType() == "presenca"){
        wiringPiISR(input->getWPi(), INT_EDGE_BOTH, &isrPresenca);
        io_Presenca = input;
    }

    else if(input->getType() == "fumaca"){
        wiringPiISR(input->getWPi(), INT_EDGE_BOTH, &isrFumaca);
        io_Fumaca = input;
    }

    else if(input->getType() == "porta"){
        wiringPiISR(input->getWPi(), INT_EDGE_BOTH, &isrPorta);
        io_Porta = input;
    }

    else if(input->getType() == "contagem"){
        if(initContagem == 0){
            wiringPiISR(input->getWPi(), INT_EDGE_RISING, &isrContagem01);
            io_Contagem01 = input;
        }else{
            wiringPiISR(input->getWPi(), INT_EDGE_RISING, &isrContagem02);
            io_Contagem02 = input;
        }
        initContagem++;
    }
}


void isrJanela01(void){
    io_Janela01->setValue((int)!io_Janela01->getValue());
}

void isrJanela02(void){
    io_Janela02->setValue((int)!io_Janela02->getValue());
}

void isrPresenca(void){
    io_Presenca->setValue((int)!io_Presenca->getValue());
}

void isrFumaca(void){
    io_Fumaca->setValue((int)!io_Fumaca->getValue());
}

void isrPorta(void){
    io_Porta->setValue((int)!io_Porta->getValue());
}

void isrContagem01(void){
    io_Contagem01->setValue(abs(io_Contagem01->getValue()+1));
    io_Contagem02->setValue(abs(io_Contagem02->getValue()+1));
}

void isrContagem02(void){
    io_Contagem01->setValue(abs(io_Contagem01->getValue()-1));
    io_Contagem02->setValue(abs(io_Contagem02->getValue()-1));
}
