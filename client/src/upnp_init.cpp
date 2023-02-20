#include "upnp.hpp"

UpnpBotnet::UpnpBotnet(const char *ifname): _ifname(ifname)
{
    InitLogs();
    _success = UpnpInitWithOptions(ifname, UPNP_PORT, UPNP_FLAG_IPV6,
        UPNP_OPTION_NETWORK_WAIT, 5, UPNP_OPTION_END);
 
    CheckErrorAndExit("init failed: ");
    std::cout << "IPV4 address: " << UpnpGetServerIpAddress() << std::endl;
    std::cout << "IPV6 address: " << UpnpGetServerIp6Address() << std::endl;
    std::cout << "Port: " << UpnpGetServerPort() << std::endl;
}

UpnpBotnet::~UpnpBotnet()
{
    // Temporary sleep to have time to check if port is opened
    sleep(20);
    runaction(_hdl, "DeletePortMapping",
        std::vector<std::pair<std::string, std::string>> {
        {"NewRemoteHost", ""},
        {"NewExternalPort", std::to_string(UpnpGetServerPort())},
        {"NewProtocol", "TCP"}});
    UpnpFinish();
}

void UpnpBotnet::InitLogs()
{
    UpnpSetLogFileNames("", "");
    UpnpSetLogLevel(UPNP_INFO);
    UpnpInitLog();        
}

void UpnpBotnet::SearchAsync() {
    int searchctxt;
    _success = UpnpSearchAsync(_hdl, 3, "upnp:rootdevice", &searchctxt);
    CheckErrorAndExit("search async failed: ");

}

void UpnpBotnet::CheckErrorAndExit(const std::string errMessage) {
    if (_success != UPNP_E_SUCCESS) {
        std::cerr << errMessage << _success << " " <<
            UpnpGetErrorMessage(_success) << "\n";
        exit (1);
    }
}

bool UpnpBotnet::CheckErrorAndContinue(const std::string errMessage) {
    if (_success != UPNP_E_SUCCESS) {
        std::cerr << errMessage << _success << " " <<
            UpnpGetErrorMessage(_success) << "\n";
        return true;
    }
    return false;
}
