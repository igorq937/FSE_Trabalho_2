#include "inc/cJSON.h"
#include "inc/json_config.h"
#include "inc/gpio.h"
#include "inc/dht22.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>


#define DHT22_WAIT_US   1000000


static pthread_t thread_id_dht22;


void exit_thread();
void shutdown();
long long timeInMicroseconds();
void loop_routine();
void* loop();


int main(int argc, char *argv[]){

    signal(SIGINT, shutdown);
    signal(SIGUSR1, exit_thread);

    if(argc != 3){
        printf("USO:\n");
        printf("\t%s <arquivo json de configuracao> <DHT22 GPIO>\n\n", argv[0]);
        printf("EXEMPLO:\n\t%s configuracao_andar_1.json 20\n", argv[0]);
        exit(1);
    }

    Json_Config json_config = json_config_parse(argv[1]);

    gpio_init(json_config);
    dht22_init(bcm_to_wPi((unsigned char)atoi(argv[2])));
    
    pthread_create(&thread_id_dht22, NULL, loop, NULL);
    loop();

    json_config_close(json_config);
    return 0 ;
}


void exit_thread(){
    pthread_exit(NULL);
}


void shutdown(){
    printf("### DESLIGANDO! ###\n");
    pthread_kill(thread_id_dht22, SIGUSR1);
    exit(0);
}


long long timeInMicroseconds(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((((long long)tv.tv_sec)*1000000)+(tv.tv_usec));
}


void dht22_routine(){
    float dht22_temperaure, dht22_humidity;
    dht22_read_data(&dht22_temperaure, &dht22_humidity);
    printf("T: %.1f H: %.1f\n", dht22_temperaure, dht22_humidity);
}


void* loop(){
    long long wait = 0;
    long long currentTime = 0;
    long long lastTime = timeInMicroseconds();
    pthread_t id = pthread_self();

    while(true){
        currentTime = timeInMicroseconds();
    
        if(pthread_equal(id, thread_id_dht22)){
            wait += DHT22_WAIT_US - (currentTime - lastTime);
            dht22_routine();
        }else{
            wait += DHT22_WAIT_US - (currentTime - lastTime);
        }

        if(wait > 0){
            usleep(wait);
		    wait = 0;
        }
        lastTime = timeInMicroseconds();
    }
}