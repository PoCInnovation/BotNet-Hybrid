#include <asio.hpp>
#include <asio/error_code.hpp>
#include <iostream>

char fetch_data_server(asio::ip::tcp::socket& socket, asio::error_code& ec)
{
    char buffer[1];
    char bytesRead = asio::read(socket, asio::buffer(buffer, 1), ec);

    if (!ec)
        return bytesRead;
    std::cout << "Error:" << ec.message() << std::endl;
    return 0;
}

void send_data_server(asio::ip::tcp::socket &socket, char *buffer)
{
    asio::write(socket, asio::buffer(buffer, 1));
}

int server(void)
{
    asio::error_code ec;
    asio::io_service io_service;
    asio::ip::tcp::acceptor acceptor_server(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 3612));
    asio::ip::tcp::socket server_socket(io_service);
    acceptor_server.accept(server_socket);
    char sucess_buffer[1] = {9};

    while (true && !ec) {
        char response = fetch_data_server(server_socket, ec);

        if (response == 1) {
            std::cout << "Server received: \"" << response << "\"\n";
            send_data_server(server_socket, sucess_buffer);
        }
    }
    server_socket.close();
    std::cout << "Server closed" << std::endl;
    return 0;
}
