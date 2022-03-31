#ifndef JSON_CONFIG_HPP_
#define JSON_CONFIG_HPP_

#include "io.hpp"
#include "dht22.hpp"

#include <string>
#include <vector>
#include <exception>


class JsonConfigException : public std::exception {

private:
    std::string userMessage;

public:
    JsonConfigException(const std::string message) throw();
    ~JsonConfigException() throw();

    const char *what() const throw();

};


class JsonConfig {

private:
    std::string nome;
    std::string ipServidorCentral;
    int portaServidorCentral;
    std::string ipServidorDistribuido;
    int portaServidorDistribuido;
    std::vector<IO> outputs;
    std::vector<IO> inputs;
    DHT22 *sensorTemperatura;

public:
    JsonConfig(std::string json_path);
    ~JsonConfig();

    std::string getNome();
    std::string getIpServidorCentral();
    int getPortaServidorCentral();
    std::string getIpServidorDistribuido();
    int getPortaServidorDistribuido();
    IO* getOutput(int gpio);
    IO* getInput(int gpio);
    std::vector<IO> getOutputs();
    std::vector<IO> getInputs();
    DHT22* getSensorTemperatura();

};

#endif /* JSON_CONFIG_HPP_ */