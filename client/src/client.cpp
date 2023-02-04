#include <asio.hpp>
#include <iostream>

#include "client.hpp"

void send_data_client(asio::ip::tcp::socket &socket, std::string message, asio::error_code ec)
{
    asio::write(socket, asio::buffer(message + '\0'), ec);
    if (!ec) {
        std::cout << "Sent: \"" << message << "\"\n";
    } else {
        std::cout << "Send failed: " << ec.message();
        return;
    }
}

int fetch_data_client(asio::ip::tcp::socket &socket, asio::error_code ec)
{
    asio::streambuf buffer;
    asio::read_until(socket, buffer, "\0");
    std::string data = asio::buffer_cast<const char*>(buffer.data());

    if (!ec) {
        std::cout << "Received: \"" << data << "\"\n";
        if (data == "active") {
            send_data_client(socket, "ready", ec);
            socket.close();
            server();
        }
    } else {
        std::cout << "Receive failed: " << ec.message();
        return 1;
    }
    return 0;
}

bool connect_client_socket(asio::ip::tcp::socket &socket, asio::error_code ec)
{
    asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string("192.168.1.57"), 9570);

    socket.connect(endpoint, ec);
    if (!ec) {
        std::cout << "Socket connected\n";
        return true;
    } else {
        std::cout << "Error: " << ec.message();
        return false;
    }
}

int main(void)
{
    asio::error_code ec;
    asio::io_service io_service;
    asio::ip::tcp::socket socket(io_service);

    if (connect_client_socket(socket, ec) == false)
        return 1;
    send_data_client(socket, "bot", ec);
    fetch_data_client(socket, ec);

    return 0;
}
