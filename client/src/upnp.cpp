#include <set>
#include <mutex>
#include <condition_variable>
#include <thread>
 
#include <libupnpp/control/description.hxx>

#include <npupnp/upnp.h>
#include <npupnp/upnpdebug.h>
#include <npupnp/upnptools.h>

#include <iostream>

static int clctxt;
 
std::set<std::string> locations;
std::mutex locations_mutex;
std::condition_variable locations_cv;
static bool searchdone;

static int mycallback(Upnp_EventType etyp, const void *evt, void *ctxt)
{
    UpnpDiscovery *dsp = (UpnpDiscovery*)evt;
    switch (etyp) {
    case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE:
        goto dldesc;
    case UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE:
        break;
    case UPNP_DISCOVERY_SEARCH_RESULT:
        goto dldesc;
    case UPNP_DISCOVERY_SEARCH_TIMEOUT:
    {
        std::unique_lock<std::mutex> loclock;
        searchdone = true;
        locations_cv.notify_all();
    }
    break;
    default:
        std::cerr << "Received surprising event type " << etyp << "\n";
        break;
    }        
    return 0;
 
dldesc:
    std::unique_lock<std::mutex> loclock;
    locations.insert(dsp->Location);
    locations_cv.notify_all();
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "Usage: npupnp_init <ifname>\n";
        return 1;
    }
    const char *ifname = argv[1];

    UpnpSetLogFileNames("", "");
    UpnpSetLogLevel(UPNP_INFO);
    UpnpInitLog();
 
    int port = 0;
    unsigned int flags = UPNP_FLAG_IPV6;
 
    int success = UpnpInitWithOptions(ifname, port, flags, UPNP_OPTION_NETWORK_WAIT, 5, UPNP_OPTION_END);
 
    if (success != UPNP_E_SUCCESS) {
        std::cerr << "init failed: " << success << " " <<
            UpnpGetErrorMessage(success) << "\n";
        return 1;
    }
 
    std::cout << "IPV4 address: " << UpnpGetServerIpAddress() << "\n";
    std::cout << "IPV6 address: " << UpnpGetServerIp6Address() << "\n";
    std::cout << "Port: " << UpnpGetServerPort() << "\n";    

    UpnpClient_Handle chdl;
    success = UpnpRegisterClient(mycallback, &clctxt, &chdl);
    
    if (success != UPNP_E_SUCCESS) {
        std::cerr << "register client failed: " << success << " " <<
            UpnpGetErrorMessage(success) << "\n";
        return 1;
    }

    int searchctxt;
    success = UpnpSearchAsync(chdl, 3, "upnp:rootdevice", &searchctxt);

    if (success != UPNP_E_SUCCESS) {
        std::cerr << "search async failed: " << success << " " <<
            UpnpGetErrorMessage(success) << "\n";
        return 1;
    }

    std::unique_lock<std::mutex> locklocs(locations_mutex);
    for (;;) {
        locations_cv.wait(locklocs);
        if (searchdone) {
            break;
        }
        for (auto it = locations.begin(); it != locations.end();) {
            std::string data;
            std::string ct;
            int success = UpnpDownloadUrlItem(*it, data, ct);
            if (success != UPNP_E_SUCCESS) {
                std::cerr << "UpnpDownloadUrlItem failed: " << success << " " <<
                    UpnpGetErrorMessage(success) << "\n";
                continue;
            }
 
            UPnPClient::UPnPDeviceDesc desc(*it, data);
            if (!desc.ok) {
                std::cout << "Description parse failed for " << *it << "\n";
            } else {
                std::cout << "Got " << data.size() <<
                    "Bytes of description data. " << "Friendly name: " <<
                    desc.friendlyName << "\n";
            }
            it = locations.erase(it);
        }
    }
    sleep(2);
    UpnpFinish();
    return 0;
}
