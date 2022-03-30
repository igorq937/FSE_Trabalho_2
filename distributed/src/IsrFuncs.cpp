#include "IsrFuncs.hpp"
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


static void isr_Janela01(void);
static void isr_Janela02(void);
static void isr_Presenca(void);
static void isr_Fumaca(void);
static void isr_Porta(void);
static void isr_Contagem01(void);
static void isr_Contagem02(void);


void isr_init(IO *input){

    if(input->getType() == "janela"){
        if(initJanela == 0){
            wiringPiISR(input->getWPi(), INT_EDGE_BOTH, &isr_Janela01);
            io_Janela01 = input;
        }else{
            wiringPiISR(input->getWPi(), INT_EDGE_BOTH, &isr_Janela02);
            io_Janela02 = input;
        }
        initJanela++;

    }else if(input->getType() == "presenca"){
        wiringPiISR(input->getWPi(), INT_EDGE_BOTH, &isr_Presenca);
        io_Presenca = input;
    }

    else if(input->getType() == "fumaca"){
        wiringPiISR(input->getWPi(), INT_EDGE_BOTH, &isr_Fumaca);
        io_Fumaca = input;
    }

    else if(input->getType() == "porta"){
        wiringPiISR(input->getWPi(), INT_EDGE_BOTH, &isr_Porta);
        io_Porta = input;
    }

    else if(input->getType() == "contagem"){
        if(initContagem == 0){
            wiringPiISR(input->getWPi(), INT_EDGE_RISING, &isr_Contagem01);
            io_Contagem01 = input;
        }else{
            wiringPiISR(input->getWPi(), INT_EDGE_RISING, &isr_Contagem02);
            io_Contagem02 = input;
        }
        initContagem++;
    }
}


void isr_Janela01(void){
    io_Janela01->setValue((int)!io_Janela01->getValue());
}

void isr_Janela02(void){
    io_Janela02->setValue((int)!io_Janela02->getValue());
}

void isr_Presenca(void){
    io_Presenca->setValue((int)!io_Presenca->getValue());
}

void isr_Fumaca(void){
    io_Fumaca->setValue((int)!io_Fumaca->getValue());
}

void isr_Porta(void){
    io_Porta->setValue((int)!io_Porta->getValue());
}

void isr_Contagem01(void){
    io_Contagem01->setValue(abs(io_Contagem01->getValue()+1));
    io_Contagem02->setValue(abs(io_Contagem02->getValue()+1));
}

void isr_Contagem02(void){
    io_Contagem01->setValue(abs(io_Contagem01->getValue()-1));
    io_Contagem02->setValue(abs(io_Contagem02->getValue()-1));
}
