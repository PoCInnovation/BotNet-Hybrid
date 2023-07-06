#include "Bot.hpp"

Bot::Bot(asio::ip::tcp::socket &socket) : _main_server(socket),
    _connection_tracker(_io_service)
{
    std::string check_buffer;
    asio::read_until(socket, asio::dynamic_buffer(check_buffer), '\n');
    if (check_buffer != "trackers\n")
        throw std::exception();
    std::string buffer;
    asio::read_until(socket, asio::dynamic_buffer(buffer), "\r\n");
    size_t pos;
    size_t start = 0;
    while ((pos = buffer.find('\n', start)) != std::string::npos) {
        auto substr = buffer.substr(start, pos - start);
        substr.pop_back();
        if (substr != "\r")
            _trackers.push_back(substr);
        start = pos + 1;
        std::cout << "coucou\n";
    }
    std::cout << "trackers online: ";
    for (auto tracker : _trackers) {
        std::cout << tracker << " ";
    }
    std::cout << std::endl;
}

void Bot::connectToTracker()
{
  std::cerr << "Hello\n";
    for (auto tracker : _trackers) {
        try {
          std::cerr << tracker << std::endl;
            asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string(tracker), 3612);
            _connection_tracker.connect(endpoint);
            break;
        } catch (std::exception &e) {
            std::cerr << "Skipped tracker " << tracker << " for: " << e.what()
                << std::endl;
            continue;
        }
    }
}
