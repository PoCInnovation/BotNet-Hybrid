# BotNet Hybrid

A BotNet, is a type of virus that repeats in a chain over a network(s) to infect all available devices. Once these infected devices they become "bots" that will be at the service of the botmater and it will be able to give orders to its "army" as to launch attacks (DDOS) or other use.
The goal of this project is therefore to create and imagine our own centralized, decentralised BotNet hybrid structure.

## How does it work?

Once infected, clients connect to the central server and get their role assigned :
- Tracker : The client passed all tests and seem ideal to become a server, its role will be to be a relay between the central server and bots, it will propagate all commands from the central server.
- Bot : The bot only communicates one time with the central server which will send him a list of trackers ips and ports, then he'll act like a usual bot and execute commands he receives from the tracker. If the central server is down he uses cached tracker ips from previous connections.

If the central server is down, the magic trick is that an infiltrated bot can send the new central server ip to a tracker, the tracker can propagate the information to all bots and trackers across the network, the ip should be signed by a pgp key to make sure nobody can destroy the network.

## Getting Started

### Installation

#### Requirements
- [libnpupnp-5.0.0](https://www.lesbonscomptes.com/upmpdcli/pages/downloads.html) C++ library.
- [asio-1.24.0](https://sourceforge.net/projects/asio/files/asio/1.24.0%20%28Stable%29/) (non boost) C++ library.

### Quickstart

#### Server
```
cargo run
```

#### Client
```
make
./client
```

### Usage

Host the server, client are runned on bots.

## Get involved

You're invited to join this project ! Check out the [contributing guide](./CONTRIBUTING.md).

If you're interested in how the project is organized at a higher level, please contact the current project manager.

## Our PoC team :heart:

Developers
| [<img src="https://github.com/Nestyles.png?size=85" width=85><br><sub>Tom Sancho</sub>](https://github.com/Nestyles) | [<img src="https://github.com/TristanMasselot.png?size=85" width=85><br><sub>Tristan Masselot</sub>](https://github.com/TristanMasselot) 
| :---: | :---: 

Manager
| [<img src="https://github.com/lennyvong.png?size=85" width=85><br><sub>Lenny Vongphouthone</sub>](https://github.com/lennyvong)
| :---: |

<h2 align=center>
Organization
</h2>

<p align='center'>
    <a href="https://www.linkedin.com/company/pocinnovation/mycompany/">
        <img src="https://img.shields.io/badge/LinkedIn-0077B5?style=for-the-badge&logo=linkedin&logoColor=white">
    </a>
    <a href="https://www.instagram.com/pocinnovation/">
        <img src="https://img.shields.io/badge/Instagram-E4405F?style=for-the-badge&logo=instagram&logoColor=white">
    </a>
    <a href="https://twitter.com/PoCInnovation">
        <img src="https://img.shields.io/badge/Twitter-1DA1F2?style=for-the-badge&logo=twitter&logoColor=white">
    </a>
    <a href="https://discord.com/invite/Yqq2ADGDS7">
        <img src="https://img.shields.io/badge/Discord-7289DA?style=for-the-badge&logo=discord&logoColor=white">
    </a>
</p>
<p align=center>
    <a href="https://www.poc-innovation.fr/">
        <img src="https://img.shields.io/badge/WebSite-1a2b6d?style=for-the-badge&logo=GitHub Sponsors&logoColor=white">
    </a>
</p>

> :rocket: Don't hesitate to follow us on our different networks, and put a star 🌟 on `PoC's` repositories

> Made with :heart: by PoC
