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


void loop(chrono::milliseconds waitTime, void (*function)());
void shutdown(int signal);
void readDHT22();
void debug();


PI_THREAD(dht22Thread){
    loop(chrono::milliseconds(1000), &readDHT22);
    return NULL;
}

PI_THREAD(debugThread){
    // loop(chrono::milliseconds(5000), &debug);
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

    int csock;
    try{
        jsonConfig = new JsonConfig(argv[1]);      
        csock = sock::createSocket(jsonConfig->getIpServidorCentral(), 
                                    jsonConfig->getPortaServidorCentral(), jsonConfig->getNome());

        cout << "sock: " << csock << " IP:" << jsonConfig->getIpServidorCentral() << " port: " << jsonConfig->getPortaServidorCentral() << endl;

        gpio::input::set(csock);
        for(auto input : jsonConfig->getInputs())
            gpio::input::init(jsonConfig->getInput(input.getGpio()));

        for(auto input : jsonConfig->getInputs()){
            usleep(10000);
            sock::writeSocket(csock, input, sock::MODE_CREATE);
        }

        for(auto output : jsonConfig->getOutputs()){
            usleep(10000);
            sock::writeSocket(csock, output, sock::MODE_CREATE);
            gpio::output::init(output.getWPi());
        }

    }catch(JsonConfigException &e){
        cout << e.what() << endl;
        return EXIT_FAILURE;

    }catch(sock::SocketException &e){
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }

    if(piThreadCreate(debugThread) || piThreadCreate(dht22Thread)){
        cout << "Não foi possível iniciar todas as threads necessárias para o programa!" << endl;
        return EXIT_FAILURE;
    }

    while(true){
        sleep(1);
        string msg = sock::readSocket(csock);
        cout << msg << endl;
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
}

void debug(){
    for(auto input : jsonConfig->getInputs()){
        cout << endl;
        cout << "Type: " << jsonConfig->getInput(input.getGpio())->getType() << endl;
        cout << "Tag: " << jsonConfig->getInput(input.getGpio())->getTag() << endl;
        cout << "Value: " << jsonConfig->getInput(input.getGpio())->getValue() << endl;
    }
    cout << endl;
    cout << "Humidity: " << jsonConfig->getSensorTemperatura()->getHumidade() << endl;
    cout << "Temperatura: " << jsonConfig->getSensorTemperatura()->getTemperatura() << endl;
    cout << "gpio: " << jsonConfig->getSensorTemperatura()->getGpio() << endl;
}
