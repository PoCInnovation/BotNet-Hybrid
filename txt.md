## What is a botnet ? How does it work ?

A botnet is a network of devices that have been infected with malicious code connected via internet.\
These devices can be computers, laptops, mobiles, etc...\
The word "botnet" comes from the combination of two words, ro**bot** and **net**work.\
Each compromised device is either called a bot or a zombie and can be used to commit certain tasks controlled by the botmaster, who's the handler of the botnet.\
These tasks are most of the time attacks such as

-   DDoS attack
-   Email spamming
-   Cryptocurrency mining
-   Ad fraud
-   Steal information
-   Banner and pop-up ads
-   ...

## What is its different forms and how they work ? Explain in detail each model

Botnets have 2 different architecture, the **Client-Server Model** and the **Peer-to-Peer (P2P) Architecture**.

### Client-Server Model

In this architecture, one of the bots is used as a central server. Its role is to control the transfer of informations from all the other bots, acting as a client.\
With this method, the hacker uses communications protocol such as IRC to establish a connection and relay information between the server and clients.\
"The bots are then often programmed to remain dormant and await commands from the C&C server before initiating any malicious activities or cyber attacks." `(cf. techtarget.com)`

:heavy_plus_sign: Best for taking and maintaining control over the bots.\
:heavy_plus_sign: Centralized -> there is no confusion during the communication.

:heavy_minus_sign: Centralized -> Easy to locate and destroy the central bot, if the control point is destroyed, the botnet is dead.

### Peer-to-Peer (P2P) Architecture

The p2p architecture is more advanced and secure than the previous method.\
Since it doesn't rely on a centralized server to add new bots, it instead uses p2p. Here, each bot act as a client and a server.\
Every single bot has a list of other infected devices so that they can establish a connection with them when required.

:heavy_plus_sign: Decentralized -> Hard to locate and destroy, destroying one bot wont stop the botnet.

:heavy_minus_sign: Harder to make.

## What is Shodan and what is its link with a botnet ? How it works and what can we do with it ?

Shodan is search engine that searches for internet connected devices.\
These devices can be anything connected to internet, like botnet's devices.\
Shodan can find non-protected devices connected to internet.\
Shodan works by spamming every existing port of every existing IP, all day, everyday. Some of them don't return anything, but when they do, Shodan can access a lot of Metadata such as:

-   Device name
-   Ip address
-   Open port
-   Location (country, city, latitude/longitude...)
-   Organization (for example, the internet provider, the business you work for...)
-   ...

Shodan can tell hackers everything they need to know to break into your network, but hackers search exclusively for software vulnerabilities that will allow them to invade your networks, while Shodan’s vulnerability scan is hidden behind an expensive paywall.
