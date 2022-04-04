#include "jsonConfig.hpp"
#include "gpioUtil.hpp"
#include "socket.hpp"
#include "dht22.hpp"

#include <iostream>
#include <wiringPi.h>
#include <unistd.h>
#include <vector>
#include <chrono>
#include <thread>


using namespace std;


static JsonConfig *jsonConfig;
int csock = -1;


void loop(chrono::milliseconds waitTime, void (*function)());
void shutdown(int signal);
void readDHT22();


PI_THREAD(dht22Thread){
    loop(chrono::milliseconds(1000), &readDHT22);
    return NULL;
}


int main(int argc, char *argv[]){

    if(argc != 2){
        cout << "Use: " << argv[0] << " <arquivo.json>" << endl << endl;
        cout << "Exemplo: " << argv[0] << " configuracao_andar_1.json" << endl << endl;
        return EXIT_SUCCESS;
    }

    if(wiringPiSetup() == -1){
		cout << "Falha ao iniciar o wiringPi!" << endl;
        return EXIT_FAILURE;
    }

    try{
        jsonConfig = new JsonConfig(argv[1]);      
        csock = sock::createSocket(jsonConfig->getIpServidorCentral(), 
                                    jsonConfig->getPortaServidorCentral(), jsonConfig->getNome());

        while(csock == -1){
            csock = sock::createSocket(jsonConfig->getIpServidorCentral(), 
                                jsonConfig->getPortaServidorCentral(), jsonConfig->getNome());
            cout << "Tentando conectar ao servidor central!" << endl;
            sleep(1);
        }

        cout << "Conectado: " << jsonConfig->getIpServidorCentral() << ":" << jsonConfig->getPortaServidorCentral() << endl;

        gpio::input::set(csock);
        for(auto input : jsonConfig->getInputs())
            gpio::input::init(jsonConfig->getInput(input.getGpio()));

        bool sendContagem = false;
        for(auto input : jsonConfig->getInputs()){
            usleep(10000);
            if(input.getType() == "contagem"){
                if(sendContagem)
                    continue;
                else
                    sendContagem = true;
            }
            sock::writeIoSocket(csock, input, sock::MODE_CREATE);
        }

        for(auto output : jsonConfig->getOutputs()){
            usleep(10000);
            sock::writeIoSocket(csock, output, sock::MODE_CREATE);
            gpio::output::init(output.getWPi());
        }

    }catch(JsonConfigException &e){
        cout << e.what() << endl;
        return EXIT_FAILURE;

    }catch(sock::SocketException &e){
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }

    if(piThreadCreate(dht22Thread)){
        cout << "Não foi possível iniciar todas as threads necessárias para o programa!" << endl;
        return EXIT_FAILURE;
    }

    while(true){
        sleep(1);
        string msg = sock::readSocket(csock);
        gpio::output::set(msg);
    }

    delete jsonConfig;
    return EXIT_SUCCESS;
}

void loop(chrono::milliseconds waitTime, void (*function)()){

    chrono::milliseconds wait;
    chrono::high_resolution_clock::time_point startTime;
    chrono::high_resolution_clock::time_point endTime = chrono::high_resolution_clock::now();

    while(true){
        startTime = chrono::high_resolution_clock::now();
        wait = chrono::duration_cast<chrono::milliseconds>(wait + waitTime - (startTime - endTime));
        
        function();

        if(wait.count() > 0l){
            std::this_thread::sleep_for(wait);
            wait = chrono::milliseconds(0);
        }
        endTime = chrono::high_resolution_clock::now();
    }
}

void readDHT22(){
    jsonConfig->getSensorTemperatura()->read();
    sock::writeDhtSocket(csock, *(jsonConfig->getSensorTemperatura()));
}
