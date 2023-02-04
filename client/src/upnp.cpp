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
 
std::set<std::string> locations;
std::mutex locations_mutex;
std::condition_variable locations_cv;

static std::string service_ctlurl = "";
static std::string service_evturl = "";
static std::string service_type = "";

static int clctxt;
static bool searchdone;

static struct Upnp_Discovery devdetails;

const std::vector<std::pair<std::string, std::string>>
runaction(UpnpClient_Handle hdl, const std::string& action_name,
              const std::vector<std::pair<std::string, std::string>>& args)
{
    std::cerr << "Runaction. Service URL " << service_ctlurl << " action: " << action_name << "\n";

    int errorcode;
    std::string errdesc;
    std::vector<std::pair<std::string, std::string>> responsedata;

    int res = UpnpSendAction(
        hdl, "", service_ctlurl, service_type,
        action_name, args, responsedata, &errorcode, errdesc);
    if (res != UPNP_E_SUCCESS) {
        std::cerr << res << std::endl;
        return responsedata;
    }
    std::cerr << "Response arg count: " << responsedata.size() << std::endl;
    for (const auto &entry : responsedata) {
        std::cerr << "[" << entry.first << "]->[" << entry.second << "]\n";
    }
    return responsedata;
}

static int mycallback(Upnp_EventType event_type, const void *evt, void *ctxt)
{
    UpnpDiscovery *dsp = (UpnpDiscovery*)evt;
    switch (event_type) {
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
    case UPNP_EVENT_RECEIVED:
    {
        // General events from the services we subscribed to
        struct Upnp_Event *evp = (struct Upnp_Event *)evt;
        std::cerr << "Got upnp event for subscription " << evp->Sid <<
            " seq " << evp->EventKey << " Changed:\n";
        for (const auto& entry : evp->ChangedVariables) {
            std::cerr << "[" << entry.first << "]->[" << entry.second << "]\n";
        }
    }
    break;

    default:
        std::cerr << "Got event type " << event_type << "\n";
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
        std::cerr << "Usage: ./upnp INTERFACE\n";
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

    UpnpClient_Handle hdl;
    success = UpnpRegisterClient(mycallback, nullptr, &hdl);
    
    if (success != UPNP_E_SUCCESS) {
        std::cerr << "register client failed: " << success << " " <<
            UpnpGetErrorMessage(success) << "\n";
        return 1;
    }

    int searchctxt;
    success = UpnpSearchAsync(hdl, 3, "upnp:rootdevice", &searchctxt);

    if (success != UPNP_E_SUCCESS) {
        std::cerr << "search async failed: " << success << " " <<
            UpnpGetErrorMessage(success) << "\n";
        return 1;
    }

    std::unique_lock<std::mutex> locklocs(locations_mutex);
    while (true) {
        locations_cv.wait(locklocs);
        if (searchdone)
            break;

        for (auto it = locations.begin(); it != locations.end();
            it = locations.erase(it)) {
            std::string data;
            std::string ct;

            success = UpnpDownloadUrlItem(*it, data, ct);
            if (success != UPNP_E_SUCCESS) {
                std::cerr << "UpnpDownloadUrlItem failed: " << success << " " <<
                    UpnpGetErrorMessage(success) << "\n";
                continue;
            }
 
            UPnPClient::UPnPDeviceDesc desc(*it, data);
            if (!desc.ok) {
                std::cout << "Description parse failed for " << *it << "\n";
                continue;
            }
            std::cout << "Got " << data.size() <<
                "Bytes of description data from " <<
                desc.friendlyName << "\n";

            for (int i = 0; i < desc.services.size(); ++i) {
                std::cout << desc.services[i].serviceId << std::endl;
                if (desc.services[i].serviceId == "urn:upnp-org:serviceId:WANCommonIFC1") {
                    service_type = desc.services[i].serviceType;
                    service_ctlurl = desc.URLBase + desc.services[i].controlURL;
                    service_evturl = desc.URLBase + desc.services[i].eventSubURL + ".xml";
                    break;
                }
            }
            if (service_type == "") {
                for (int j = 0; j < desc.embedded.size(); ++j) {
                    for (int i = 0; i < desc.embedded[j].services.size(); ++i) {
                        std::cout << desc.services[i].serviceId << std::endl;
                        if (desc.embedded[j].services[i].serviceId == "urn:upnp-org:serviceId:WANCommonIFC1") {
                            service_type = desc.embedded[j].services[i].serviceType;
                            service_ctlurl = desc.URLBase + desc.embedded[j].services[i].controlURL;
                            service_evturl = desc.URLBase + desc.embedded[j].services[i].eventSubURL + ".xml";
                            break;
                        }
                    }
                }
            }
            if (service_type == "") continue;

            int timeout = 3600;

            Upnp_SID subsid;
            UpnpSubscribe(hdl, service_evturl.c_str(), &timeout, subsid);            
            std::string externalIpAddress = runaction(hdl, "GetExternalIPAddress",
                std::vector<std::pair<std::string, std::string>>())[0].second;
            std::vector<std::pair<std::string, std::string>> addPortMappingArgs {
                {"NewRemoteHost", ""},
                {"NewExternalPort", std::to_string(UpnpGetServerPort())},
                {"NewProtocol", "TCP"},
                {"NewInternalPort", std::to_string(UpnpGetServerPort())},
                {"NewInternalClient", UpnpGetServerIpAddress()},
                {"NewEnabled", "1"},
                {"NewPortMappingDescription", "Port Mapping Description"},
                {"NewLeaseDuration", std::to_string(timeout)},
            };
            runaction(hdl, "AddPortMapping", addPortMappingArgs);
        }
    }
    sleep(10);
    runaction(hdl, "DeletePortMapping",
        std::vector<std::pair<std::string, std::string>> {
            {"NewRemoteHost", ""},
            {"NewExternalPort", std::to_string(UpnpGetServerPort())},
            {"NewProtocol", "TCP"}});
    UpnpFinish();
    return 0;
}
