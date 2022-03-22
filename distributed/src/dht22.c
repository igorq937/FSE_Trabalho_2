#include <wiringPi.h>


static unsigned char dht_pin = 0;
static int data[5] = { 0, 0, 0, 0, 0 };
static float last_temperature = -1;
static float last_humidity = -1;


#define WAIT_TIME 	1
#define MAX_TIMINGS	85


void dht22_init(unsigned char dht_wPI_pin){
	dht_pin = dht_wPI_pin;
}

int dht22_read_data_(float *temperature, float *humidity){
	*temperature = -1.0f;
	*humidity = -1.0f;
	unsigned char laststate = HIGH;
	unsigned char counter	= 0;
	unsigned char j = 0;
	unsigned char i;

	data[0] = data[1] = data[2] = data[3] = data[4] = 0;

	pinMode(dht_pin, OUTPUT);
	digitalWrite(dht_pin, LOW);
	delay(18);
	pinMode(dht_pin, INPUT);

	for( i = 0; i < MAX_TIMINGS; i++ ){
		counter = 0;
		while(digitalRead(dht_pin) == laststate){
			counter++;
			delayMicroseconds(1);
			if(counter == 255)
				break;
		}
		laststate = digitalRead(dht_pin);

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

		*temperature = c;
		*humidity = h;
		return 0;
	}else{
		*temperature = *humidity = -1;
		return 1;
	}
}

void dht22_read_data(float *temperature, float *humidity){
	if(!dht22_read_data_(temperature, humidity)){
		last_temperature = *temperature;
		last_humidity = *humidity;
	}else{
		*temperature = last_temperature;
		*humidity = last_humidity;
	}
}