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
        asio::ip::tcp::socket &_socket;
        void connectToServer();
    public:
        TypeGuesser(asio::ip::tcp::socket &socket)
            : _socket(socket) {}
        void connect(const std::string &ip);
        Type type() { return _type; }
    };
#endif// !__CLIENT_HPP__
