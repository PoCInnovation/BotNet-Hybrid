#include "TypeGuesser.hpp"
#include <iostream>

int main()
{
    asio::io_service io_service;
    asio::ip::tcp::socket mainServer(io_service);
    TypeGuesser guesser(mainServer);

    guesser.connect("192.168.0.12");
    std::cout << "guesser: " << static_cast<int>(guesser.type()) << std::endl;
    switch (guesser.type()) {
        case TypeGuesser::Type::Bot:
            std::cout << "Je suis un super bot" << std::endl;
            break;
        case TypeGuesser::Type::Tracker:
            std::cout << "Je suis un super tracker" << std::endl;
    }
}
