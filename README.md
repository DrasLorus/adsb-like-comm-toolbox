# adsb-like-comm-toolbox

# Usage

Afin de tester le systeme de communication il est possible d'utiliser un fichier temporaire à la place des modules radio SdR.

# Simulation de la transmission / reception d'une image

## Sans code correcteur d'erreurs

Dans un premier temps, on ne va pas uiliser de modules USRP. Cela va simplifier la mise en oeuvre du processus et le nombre de flags à fournir à l'outil. On va utiliser un fichier temporaire qui va stocker les données que l'on aurait transmis au module d'émission.

Pour générer ce fichier temporaire, on va utiliser la commande suivante:

```bash
./emitter_image --file /tmp/image.raw --bmp_file ../images/leopard.bmp --sleep 10
```

Cette derniere va crée un fichier d'une centaine de MO sur votre disque dur. Ce fichier contient les données I/Q à tranmettre. Les données I/Q sont codées toutes les deux à l'aide d'un octet (int8_t).

Afin de simuler la reception des données via un module USRP et réalsier le décodage de l'inforamtion, il faut utiliser la commande suivante:

```bash
./receiver_image --file /tmp/image.raw --inter
```

Apres l'exeution du processus de décodage, une image *.bmp* doit se trouver dans le repertoire courant. Cette deniere est en théorie identique a celle qui a été transmise car dans le cas présent nous n'avons pas subit de perturbation lors du transfert des données.


## Avec code correcteur d'erreurs

Afin de protéger les données lors d'une transmission réelle, il est possible d'activer un code correcteur d'erreurs. Ce dernier est un code LDPC dont le redement de codage est de 1/2. Il protege les 576 bits a transmettre (valeur par défaut) à l'aide de 576 bits de redondance. Lorsque le recepteur recoit un paquet pour lequel le code CRC n'est pas validé, il déclenche alors l'execution du décodeur afin de corriger les erreurs de transmission. L'utilisation du code LDPC reduit le débit de tranmsission d'un facteur 2 !

Pour transmettre une image la commande devient:

```bash
./emitter_image --file /tmp/image.raw --bmp_file ../images/leopard.bmp--fec --sleep 10
```

Afin de simuler la reception des données via un module USRP et réalsier le décodage de l'inforamtion, il faut utiliser la commande suivante:

```bash
./receiver_image --file /tmp/image.raw --fec --inter
```

Il est a noter qu'en simulation, l'utilisation d'un code correcteur d'erreurs ne sert à rien car le canal de transmission est parfait ;-)


## A récrire lorsque j'aurais le temps...

La génération de 16 trames sockées dans un fichier nommé **/tmp/fichier.raw** peut être réalisé à l'aide de la commande ci-dessous:

```bash
./adsb_emitter --file /tmp/fichier.raw --max_frames 16 -v
```

Le décodage des informations savegardées dans le fichier **/tmp/fichier.raw** s'oppère grace à la commande suivante :

```bash
./adsb_receiver --file /tmp/fichier.raw -v
```

# Installation & compilation du projet

Afin de compiler les codes sources permettant de générer et de décoder les signaux, il est nécessaire de posséder une tool-chain (GCC ou CLANG) ainsi que les bibliotheques logicielles permettant d'accéder aux modules SdR (cf les procédures d'installation ci-dessous).

Pour commencer, ouvrez un terminal et rendez vous dans le repertoire ou vous souhaitez installer le projet.

Ensuite clonez le répertoire du projet

```bash
git clone https://github.com/blegal/adsb-like-comm-toolbox.git
```

Ensuite on va créer le répertoire ou les fichiers temporaires de compilation et l'executable vont se trouver:

```bash
cd adsb-like-comm-toolbox
mkdir build
cd build
```

On configure le projet (génération des scripts de compilation)

```bash
cmake ..
```

On compile l'ensemble des codes source

```bash
make -j 4
```

# Installation des bibliotheques logicielles

## Mac-OS

[ ] - TODO

## Linux (ubuntu)

* apt install libuhd-dev uhd-host uhd-soapysdr

* apt install hackrf soapysdr-module-hackrf libhackrf-dev

* apt install librtlsdr-dev

* apt install libsoapysdr-dev soapysdr-module-all


## Windows (...)



