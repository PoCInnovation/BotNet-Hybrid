#include "TypeGuesser.hpp"
#include <asio/read.hpp>
#include <asio.hpp>
#include <asio/write.hpp>
#include <chrono>
#include <iostream>
#include <thread>

static void server(void)
{
    asio::error_code ec;
    asio::io_service io_service;
    asio::ip::tcp::acceptor acceptor_server(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 3612));
    asio::ip::tcp::socket server_socket(io_service);
    auto until = std::chrono::system_clock::now() + std::chrono::seconds(5);
    auto timer = [&]()
    {
        while (true) {
            if (std::chrono::system_clock::now() > until) {
                acceptor_server.cancel();
                server_socket.cancel();
                std::cerr << "Reached end of timer, probably not upnp compatible" << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    };
    auto thread = std::thread(timer);
    thread.detach();
    acceptor_server.accept(server_socket);
    char success_buffer[1] = {9};
    char buffer[1] = {0};

    if (!ec) {
        server_socket.cancel();
        asio::read(server_socket, asio::buffer(buffer, 1));

        if (buffer[0] == 1) {
            asio::write(server_socket, asio::buffer(success_buffer, 1));
        }
    }
    server_socket.close();
    std::cout << "Server closed\n";
}

void TypeGuesser::connect(const std::string &ip)
{
    asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string(ip), 9570);
    asio::error_code ec;

    _socket.connect(endpoint, ec);
    if (ec) {
        throw ConnectionException(ec.message());
    }
    asio::write(_socket, asio::buffer("bot\0"));
    asio::streambuf buffer;
    asio::read_until(_socket, buffer, "\0");
    std::string data = asio::buffer_cast<const char*>(buffer.data());

    if (ec || data != "active") {
        throw ConnectionException(ec.message());
    }
    asio::write(_socket, asio::buffer("ready\0"), ec);
    if (ec) {
        throw ConnectionException(ec.message());
    }
    try {
        server();
        _type = Type::Tracker;
    } catch (std::exception &e) {
        _type = Type::Bot;
    }
}
