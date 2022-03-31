#include "jsonConfig.hpp"
#include "isrFuncs.hpp"
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
void readDHT22();
void debug();


PI_THREAD(dht22Thread){
    loop(chrono::milliseconds(1000), &readDHT22);
    return NULL;
}

PI_THREAD(debugThread){
    loop(chrono::milliseconds(1000), &debug);
    return NULL;
}


int main(int argc, char *argv[]){

    if(argc != 2){
        cout << "Use: " << argv[0] << " <arquivo.json>" << endl << endl;
        cout << "Exemplo: " << argv[0] << " configuracao_andar_1.json" << endl << endl;
        return EXIT_SUCCESS;
    }

    try{
        jsonConfig = new JsonConfig(argv[1]);      
    }catch(JsonConfigException &e){
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }

    if(wiringPiSetup() == -1){
		cout << "Falha ao iniciar o wiringPi!" << endl;
        return EXIT_FAILURE;
    }
    
    for(auto input : jsonConfig->getInputs())
        isrFuncs::init(jsonConfig->getInput(input.getGpio()));

    if(piThreadCreate(debugThread) || piThreadCreate(dht22Thread)){
        cout << "Nao foi possível iniciar todas as threads necessárias para o progama!" << endl;
        return EXIT_FAILURE;
    }

    while(true){
        sleep(1);
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
