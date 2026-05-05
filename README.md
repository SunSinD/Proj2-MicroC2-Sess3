<a id="readme-top"></a>

[![Licence Unlicense][license-shield]][license-url]

<br />
<div align="center">
  <img src="https://i.imgur.com/2rBe5qx.png" alt="Projet 2" width="500" height="800">

  <h1 align="center">Projet 2 - Jeu sur matrice DEL</h1>

  <p align="center">
    Projet d'école pour le cours <strong>243-33A-MO : Microcontrôleur 2</strong><br />
    Réalisé avec PlatformIO, Arduino et C++.
  </p>
</div>

## Description

![Collège Montmorency][product-screenshot]

Ce dépôt contient mon deuxième projet pour le cours de Microcontrôleur 2. Le projet est un petit jeu de cible fait avec un Arduino Mega 2560 et une matrice RGB 64x32.

Le joueur déplace un viseur avec les boutons et doit toucher les cibles avant qu'elles disparaissent. Le programme utilise aussi l'encodeur, le potentiomètre, un timer, les interruptions et la mémoire EEPROM.

## Gameplay

- L'écran de départ montre le nombre de vies et l'état du God Mode.
- Les boutons directionnels déplacent le viseur.
- Le bouton `B` sert à tirer sur une cible.
- Les cibles grossissent avec le temps.
- Si une cible n'est pas touchée assez vite, le joueur perd une vie.
- Le bouton `A` termine la partie.
- Le bouton `C` démarre une partie ou retourne au menu après le game over.
- Le meilleur score est sauvegardé dans l'EEPROM.

## Fonctionnalités

- Écran titre, jeu et écran game over.
- Viseur contrôlé avec les boutons.
- Cibles créées avec une classe `Ball`.
- Lecture de l'encodeur avec une interruption.
- Timer 3 configuré en interruption.
- God Mode contrôlé avec le potentiomètre.
- Sauvegarde du meilleur score avec l'EEPROM.
- Document de conception inclus : `Game_Design_Document.docx`.

## Fait avec

- [![C++][cpp-shield]][cpp-url]
- Arduino Mega 2560
- PlatformIO
- Arduino Framework
- MOMO RGB Matrix

## Structure du projet

```text
.
|-- Game_Design_Document.docx
|-- platformio.ini          # Configuration PlatformIO
|-- src/main.cpp            # Code principal du jeu
|-- include/bits_manip.h    # Déclarations des fonctions
|-- lib/MOMO_RGB_Matrix/    # Librairie de la matrice RGB
|-- lib/bits_manip/         # Fonctions pour manipuler les bits
`-- README.md
```

## Compilation

Pour compiler le projet avec PlatformIO :

```bash
pio run
```

Pour téléverser le programme sur l'Arduino Mega 2560 :

```bash
pio run -t upload
```

## Note

Ce projet a été fait pour un travail scolaire. Le README a été écrit pour décrire mon dépôt et mon programme, au lieu de simplement copier le texte du document de l'enseignant.

## Licence

Distribué sous licence Unlicense. Voir [LICENSE.txt](LICENSE.txt) pour plus d'informations.

<p align="right">(<a href="#readme-top">Retour en haut</a>)</p>

[product-screenshot]: https://www.collegesinstitutes.ca/wp-content/uploads/2022/10/montmorency.png
[cpp-shield]: https://img.shields.io/badge/C++-00599C?style=flat-square&logo=C%2B%2B&logoColor=white
[cpp-url]: https://isocpp.org/
[license-shield]: https://img.shields.io/github/license/SunSinD/Proj2-MicroC2-Sess3?style=for-the-badge
[license-url]: LICENSE.txt
