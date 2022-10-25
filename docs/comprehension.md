## 1: Qu'est-ce qu'un botnet et comment cela fonctionne

Un botnet est un réseau d'ordinateurs infectés par un cheval
de troie afin généralement d'effectuer des attaques de
masse (spam, ddos, drop de nouveaux virus etc).

Généralement le réseau est développé avec des campagnes de spam
par mail, des pièces jointes infectées avec parfois l'utilisation
de 0 day afin de se lancer à l'ouverture d'un pdf ou d'une macro word.

Une fois le malware sur la machine il pourra essayer de se propager sur le réseau,
se copier dans le startup des clés usb etc..
Il essayera de persister sur le système en se lançant à chaque démarrage,
se cachant du gestionnaire des tâches par example avec un rootkit.

## 2: Quelles sont ces différentes formes et comment fonctionnent-elles ?

Les botnets sont généralement des réseaux centralisés avec un modèle C&C (commande et contrôle),
la victime a des contacts fréquents avec un serveur afin de montrer qu'il est toujours
en vie et prêt à recevoir des ordres, ces réseaux utilisaient souvent l'IRC afin de
communiquer entre l'hôte et la victime en raison de la facilité d'utilisation
et la non-suspicion des anti-virus.

Depuis on voit de plus en plus de botnets HTTP qui communiquent avec un serveur web
pour paraitre encore moins suspect vu la banalité d'une communication HTTP.

Le modèle C&C est parfait dans le principe tant que le serveur n'est pas compromis,

Pour contrer ce problème on voit de plus en plus de botnets P2P où la sanitée du réseau
se fait entre les victimes elles-mêmes qui deviennent des noeuds, pour donner des ordres
à ce réseau ça peut en revanche se révéler plus compliqué mais on pourrait par exemple
lancer une attaque DDOS seulement quand l'on reçoit un message signé par une clé PGP.

## 3: Qu'est-ce que Shodan et quel est le lien avec un botnet ?

Shodan est un moteur de recherche des appareils connectés à internet, il permet de rechercher
des types spécifiques d'appareils qui pourraient être sensibles à une certaine vulnérabilité.

Shodan possède aussi une API qui pourrait automatiser ce processus et peut se révéler
un outil très utile notamment pour quelqu'un voulant se créer un réseau de zombies,
elle pourrait alors automatiquement récupérer certains appareils puis exploiter les failles
sur l'appareil afin de s'y faire une place.
