#include "Bot.hpp"
#include "TypeGuesser.hpp"
#include <asio/write.hpp>
#include <iostream>

void bot(asio::ip::tcp::socket &socket)
{
    Bot b(socket);

    b.connectToTracker();
}

[[noreturn]] void tracker(asio::ip::tcp::socket &socket)
{
    asio::io_service io_service;
    asio::ip::tcp::acceptor acceptor_server(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 3612));
    asio::ip::tcp::socket server_socket(io_service);
    std::vector<asio::ip::tcp::socket> clients;
    std::string received_buffer;

    acceptor_server.async_accept([&](const asio::error_code &ec, asio::ip::tcp::socket peer) {
        if (!ec) {
            asio::write(peer, asio::buffer("real tracker ;)\r\n", 1));
        } else {
            std::cerr << "error accept: " << ec.message() << std::endl;
        }
    });
    while (true) {
        std::size_t n = asio::read_until(socket, asio::dynamic_buffer(received_buffer), "\r\n");
        std::cout << "Sending message to all bots: " << received_buffer << std::endl;
        std::string line = received_buffer.substr(0, n);
        received_buffer.erase(0, n);
        for (auto &client : clients) {
            asio::write(client, asio::buffer(line));
        }
    }
}

int main()
{
    asio::io_service io_service;
    asio::ip::tcp::socket mainServer(io_service);
    TypeGuesser guesser(mainServer);

    guesser.connect("192.168.1.29");
    std::cout << "guesser: " << static_cast<int>(guesser.type()) << std::endl;
    switch (guesser.type()) {
        case TypeGuesser::Type::Bot:
            std::cout << "Je suis un super bot" << std::endl;
            bot(mainServer);
            break;
        case TypeGuesser::Type::Tracker:
            std::cout << "Je suis un super tracker" << std::endl;
            tracker(mainServer);
    }
}
