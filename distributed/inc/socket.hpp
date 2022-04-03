#pragma once

#include "io.hpp"
#include <exception>
#include <string>


namespace sock {

    const char MODE_CREATE[] = "create";
    const char MODE_UPDATE[] = "update";

    class SocketException : public std::exception {

    private:
        std::string userMessage;

    public:
        SocketException(const std::string message) throw();
        ~SocketException() throw();

        const char *what() const throw();

    };

    void closeSocket(int sock_fd);
    int createSocket(std::string ip, int porta, std::string andar);
    void writeSocket(int sock, IO io, const char* mode_);
    std::string readSocket(int sock);

}
