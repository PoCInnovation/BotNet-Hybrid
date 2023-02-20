#include "TypeGuesser.hpp"
#include <iostream>

int main()
{
    TypeGuesser guesser;

    guesser.connect("192.168.0.12");
    std::cout << "guesser: " << static_cast<int>(guesser.type()) << std::endl;
}
