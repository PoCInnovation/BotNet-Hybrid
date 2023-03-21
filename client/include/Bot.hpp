#pragma once

#include <asio.hpp>
#include <asio/read_until.hpp>
#include <iostream>

class Bot {
    asio::ip::tcp::socket &_main_server;
    asio::io_service _io_service;
    asio::ip::tcp::socket _connection_tracker;
    std::vector<std::string> _trackers;
public:
    Bot(asio::ip::tcp::socket &socket);
    void connectToTracker();
};
