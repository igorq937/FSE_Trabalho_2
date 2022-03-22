#ifndef DHT22_H_
#define DHT22_H_


void dht22_init(unsigned char dht_wPI_pin);
int dht22_read_data(float *temperature, float *humidity);


#endif /* DHT22_H_ */
