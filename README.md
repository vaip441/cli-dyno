# CLI-DYNO

Projekti nimeks osutus CLI-dyno ehk Command Line Interface Dyno.  
Rühma liikmed:

 - Sebastian Lotman
 - Georg Kalme

## Lühikokkuvõte rühmatööst

Rühmatöö jaotus orgaaniliselt järgnevaks:  
Sebastianil tekkis algne idee ja uuris vastavaid võimalusi C++ projekti teostamiseks, uurides samuti jooksvalt vajalikke lahendusi (nt kuidas isegi teha n.ö graafikat terminalis, kuidas saada heli tööle jms). Georg kirjutas selleks vajaliku koodi ja struktureeris selle sobivalt.

## Koodi jooksutamine

Programm on mõeldud jooksma Linux operatsioonisüsteemil.
Programmi kompileerimiseks kasutada repos leiduvat Makefile'i käsuga:

```bash
make all
```

Seejärel on programm juba jooksutatav, kuid heli tööle saamiseks on vajalik veel [PulseAudio](https://www.freedesktop.org/wiki/Software/PulseAudio) heliprogramm, mille saab käsuga

```bash
sudo apt install pulseaudio -y
```

Programmi käivitamiseks tuleb repo algfoldris teha käsk

```bash
bin/cli-dyno
```

## Eeldatav tulemus

Ekraanile ilmub klassikaline dinosauruse mäng Chrome brauserist. Dinosauruse kontrollimiseks on järgmised klahvid:

<kbd>W</kbd> - hüppamiseks  
<kbd>S</kbd> - kükitamiseks  
<kbd>Q</kbd> - sulgeb programmi  
<kbd>R</kbd> - restart  

Mängu eesmärk on vältida lähenevaid takistusi ning saavutada võimalikult kõrge skoor. Skoor salvestatakse ning igal taasmängimisel on näha enda kõrgeimat skoori.
