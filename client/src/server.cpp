#include <asio.hpp>
#include <iostream>

size_t fetch_data_server(asio::ip::tcp::socket& socket)
{
    char buffer[1];
    size_t bytesRead = asio::read(socket, asio::buffer(buffer, 1));

    return bytesRead;
}

void send_data_server(asio::ip::tcp::socket &socket, char *buffer)
{
    asio::write(socket, asio::buffer(buffer, 1));
}

int server(void)
{
    asio::io_service io_service;
    asio::ip::tcp::acceptor acceptor_server(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 3612));
    asio::ip::tcp::socket server_socket(io_service);
    acceptor_server.accept(server_socket);
    char sucess_buffer[1] = {9};

    while (true) {
        char response = fetch_data_server(server_socket);

        if (response == 1) {
            std::cout << "Received: \"" << response << "\"\n";
            send_data_server(server_socket, sucess_buffer);
        }
    }
    return 0;
}
