# Requirements
[libnpupnp-5.0.0](https://www.lesbonscomptes.com/upmpdcli/pages/downloads.html) c++ library.

# Run
## Run upnp port mapping
```sh
make upnp
./upnp INTERFACE
```
At the moment, the interface must be given manually. On Linux get it from ```ip addr```.

## Run bot
```sh
make
./client_server
```
