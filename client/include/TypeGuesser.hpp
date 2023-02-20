#ifndef __BOT_HPP__
    #define __BOT_HPP__
    #include <asio.hpp>
    #include <exception>
    #include <string_view>
    class TypeGuesser {
    public:
        enum class Type {
            Bot,
            Tracker
        };
        class ConnectionException : std::exception {
            const std::string &_what;
        public:
            ConnectionException(const std::string &what)
                : _what(what)
            {

            }
            const char *what() const throw() 
            {
                return _what.c_str();
            }
        };
    private:
        Type _type;
        asio::io_service _io_service;
        asio::ip::tcp::socket _socket = asio::ip::tcp::socket(_io_service);
        void connectToServer();
    public:
        void connect(const std::string &ip);
        Type type() { return _type; }
    };
#endif// !__CLIENT_HPP__
