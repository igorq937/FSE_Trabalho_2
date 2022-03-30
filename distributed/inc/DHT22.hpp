#ifndef DHT22_HPP_
#define DHT22_HPP_

#include <string>

class DHT22 {

private:
    std::string model;
    std::string tag;
    int gpio;
    int wPi;
    float temperatura;
    float humidade;

public:
    DHT22(std::string model, std::string tag, int gpio, int wPi);
    ~DHT22();

    std::string getModel();
    std::string getTag();
    int getGpio();
    int getWPi();
    float getTemperatura();
    float getHumidade();

    void read();

};

#endif /* DHT22_HPP_ */
