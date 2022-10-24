## What is a botnet ? How does it work ?

A botnet is a network of devices that have been infected with malicious code connected via internet . These devices can be computers, laptops, mobiles, etc...\
The word "botnet" comes from the combination of two words, ro**bot** and **net**work.\
Each compromised device is either called a bot or a zombie and can be used to commit certain tasks controlled by the botmaster, who's the handler of the botnet.\
These tasks are most of the time attacks such as

-   DDoS attack
-   Email spamming
-   Cryptocurrency mining
-   Ad fraud
-   Steal information
-   Banner and pop-up ads

(non-exhaustive list)

## What is its different forms and how they work ? Explain in detail each model

Botnets have 2 different architecture, the **Client-Server Model** and the **Peer-to-Peer Architecture**.\

### Client-Server Model

In this architecture, one of the bots is used as a central server. Its role is to control the transfer of informations from all the other bots, acting as a client.\
With this method, the hacker uses communications protocol such as IRC to establish a connection and relay information between the server and clients.\
"The bots are then often programmed to remain dormant and await commands from the C&C server before initiating any malicious activities or cyber attacks." `(cf. techtarget.com)`\

:heavy_plus_sign: Best for taking and maintaining control over the bots.\
:heavy_plus_sign: Centralized -> there is no confusion during the communication.

:heavy_minus_sign: Centralized -> Easy to locate and destroy the central bot, if the control point is destroyed, the botnet is dead.

### Peer-to-Peer Architecture

The peer-to-peer architecture is more advanced and secure than the previous method.\
Since it doesn't rely on a centralized server to add new bots, it instead uses peer-to-peer. Here, each bot act as a client and a server.\
Every single bot has a list of other infected devices so that they can establish a connection with them when required.

:heavy_plus_sign: Centralized -> Hard to locate and destroy, destroying one bot wont stop the botnet.

## What is Shodan and what is its link with a botnet ? How it works and what can we do with it ?
