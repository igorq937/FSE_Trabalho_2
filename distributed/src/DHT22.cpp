#include "DHT22.hpp"
#include <wiringPi.h>


static int data[5] = {0, 0, 0, 0, 0};
static float ultimaTemperatura = -1;
static float ultimaHumidade = -1;


static const long WAIT_TIME = 1;
static const long MAX_TIMINGS = 85;


DHT22::DHT22(std::string model, std::string tag, int gpio, int wPi){
	this->model = model;
	this->tag = tag;
	this->gpio = gpio;
	this->wPi = wPi;
	this->temperatura = 0;
	this->humidade = 0;
}

DHT22::~DHT22(){}

std::string DHT22::getModel(){
	return model;
}

std::string DHT22::getTag(){
	return tag;
}

int DHT22::getGpio(){
	return gpio;
}

int DHT22::getWPi(){
	return wPi;
}

float DHT22::getTemperatura(){
	return temperatura;
}

float DHT22::getHumidade(){
	return humidade;
}

void DHT22::read(){
	unsigned char laststate = HIGH;
	unsigned char counter	= 0;
	unsigned char j = 0;
	unsigned char i;

	data[0] = data[1] = data[2] = data[3] = data[4] = 0;

	pinMode(wPi, OUTPUT);
	digitalWrite(wPi, LOW);
	delay(18);
	pinMode(wPi, INPUT);

	for(i = 0; i < MAX_TIMINGS; i++){
		counter = 0;
		while(digitalRead(wPi) == laststate){
			counter++;
			delayMicroseconds(1);
			if(counter == 255)
				break;
		}
		laststate = digitalRead(wPi);

		if(counter == 255)
			break;

		if((i >= 4) && (i % 2 == 0)){
			data[j / 8] <<= 1;
			if ( counter > 16 )
				data[j / 8] |= 1;
			j++;
		}
	}

	if((j >= 40) && (data[4] == ( (data[0] + data[1] + data[2] + data[3]) & 0xFF))){

		float h = (float)((data[0] << 8) + data[1]) / 10;
		if(h > 100)
			h = data[0];
		
		float c = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
		if(c > 125)
			c = data[2];

		if(data[2] & 0x80)
			c = -c;

		ultimaTemperatura = temperatura = c;
		ultimaHumidade = humidade = h;
	}else{
		temperatura = ultimaTemperatura;
		humidade = ultimaHumidade;
	}
}
