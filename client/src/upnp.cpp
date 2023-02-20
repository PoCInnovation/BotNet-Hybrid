#include <libupnpp/control/description.hxx>
#include <npupnp/upnp.h>
#include <npupnp/upnpdebug.h>
#include <npupnp/upnptools.h>

#include "upnp.hpp"

void UpnpOpenPort(UpnpBotnet *Upnp)
{
    Upnp->RegisterClient();
    Upnp->SearchAsync();
    Upnp->Loop();
}
