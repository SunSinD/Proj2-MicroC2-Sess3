/**************************************************************/
// Date de création du programme: 11/18/2025
// Date de la dernière modification: 12/18/2025
// Nom du programmeur principal: Sunpreet
//
// Le but principal de ce projet est de développer un jeu au choix qui utilisera l'interface matrice.
// Contrairement aux projets habituels, vous choisirez quel jeu vous voulez développer et comment le faire en respectant les requis minimum.
// Avant de commencer à coder, vous devrez faire valider un document qui référencera au maximum les fonctionnalités de votre jeu.
// Ce document appelé Document de Conception Logicielle (DCL) ou Software Design Document (SDD). Dans le cas d'un jeu, nous parlons de Game Design Document (GDD).
//
// ** CTRL + T dans Arduino IDE pour formater le code**
//
/**************************************************************/

//*********************************************
// Inclusion des librairies nécessaire au projet
//*********************************************

#include <Arduino.h>          // librairie Arduino
#include <MOMO_RGB_Matrix.h>  // librairie de la matrice
#include <EEPROM.h>           // librairie pour EEPROM

#define BOUTON_C 32        // define bouton C
#define BOUTON_B 33        // define bouton B
#define BOUTON_A 31        // define bouton A
#define BOUTON_HAUT 35     // define bouton haut
#define BOUTON_BAS 30      // define bouton bas
#define BOUTON_GAUCHE 36   // define bouton gauche
#define BOUTON_DROITE 34   // define bouton droite

#define ENC_CHA PE5 // define ENC_CHA
#define ENC_CHB PE3 // define ENC_CHB

#define POT_PIN A6 // define POT_PIN

MOMO_RGB_Matrix matrice; // pour la matrice

enum GameState
{
  TITRE,   // État TITRE
  JEU,     // État JEU
  GAMEOVER // État GAMEOVER
};
GameState etat = TITRE; // variable pour l'État

volatile long encoderVal = 0;            // variable encoderVal
volatile unsigned int timerTicks = 0;    // variable timerTicks

int score = 0;                    // variable score
int vies = 3;                     // variable vies
int highScore = 0;                // variable highScore
int crosshairX = 32;              // variable crosshairX
int crosshairY = 16;              // variable crosshairY
unsigned long dernierSpawn = 0;   // variable dernierSpawn
bool godModeEnabled = false;      // variable godModeEnabled
bool needRedraw = true;           // variable needRedraw

class Ball
{
public:
  int x, y;              // variable position x et y
  int rayon;             // variable rayon
  int etape;             // variable etape
  bool active;           // variable active
  unsigned long temps;   // variable temps

  Ball()
  {
    x = 0;          // position x = 0
    y = 0;          // position y = 0
    rayon = 1;      // rayon = 1
    etape = 0;      // etape = 0
    active = false; // active = false, donc pas active au départ
    temps = 0;      // temps = 0;
  }

  void spawn()
  {
    do
    {
      x = random(10, 54);  // x random entre 10 et 54
      y = random(12, 24);  // x random entre 12 et 24
    } while (x < 13 && y < 17);  // Évite l'endroit en haut à gauche pour le score et pour les vies
    rayon = 1;          // rayon = 1 pixel
    etape = 0;          // etape = 0
    active = true;      // active balle
    temps = millis();   // saves le temps
  }

  void update()
  {
    if (!active)      // si balle n'est pas active
      return;         // rien faire

    unsigned long elapsed = millis() - temps;  // temps écoulé

    if (etape < 2 && elapsed >= 1000) // balle grandit
    {
      rayon++;          // rayon +1
      etape++;          // next étape
      temps = millis(); // refresh temps
    }
    else if (etape == 2 && elapsed >= 2000) // pour l'étape 3
    {
      etape = 3;        // étape 3
      temps = millis(); // refresh temps
    }
    else if (etape == 3 && elapsed >= 2000) // pour l'étape 4
    {
      etape = 4;        // étape 4
      temps = millis(); // refresh temps
    }
    else if (etape == 4 && elapsed >= 2000) // pour l'étape 5
    {
      etape = 5;        // étape 5
      temps = millis(); // refresh temps
    }
    else if (etape == 5 && elapsed >= 2000) // balle = boom boom
    {
      active = false;   // gg balle
      if (!godModeEnabled && vies > 0)  // si god est pas activé et qu'il reste des vies
        vies--;         // -1 vie
    }
  }

  void draw()
  {
    if (!active)      // balle = pas active
      return;         // nul

    uint16_t couleur = (etape < 3)    ? matrice.Color888(255, 255, 255) // blanc
                       : (etape == 3) ? matrice.Color888(0, 80, 0)      // devrait etre vert, mais c'est bleu
                       : (etape == 4) ? matrice.Color888(0, 140, 0)     // bleu moyen
                                      : matrice.Color888(0, 200, 0);    // bleu final
    matrice.fillCircle(x, y, rayon, couleur);  // cercle
  }

  bool estTouche(int cx, int cy)
  {
    if (!active)      // si balle est pas active
      return false;   // pas être touchée

    int dx = cx - x;  // distance horiozntal entre crosshair et centre de la balle
    int dy = cy - y;  // distance vertical entre crosshair et centre de la balle
    return (dx * dx + dy * dy <= rayon * rayon);  // true si crosshair est dans le cercle
  }
};

Ball balles[3]; // tableau 3 balles

ISR(TIMER3_COMPA_vect) // interruption du Timer 3
{
  timerTicks++;        // compteur de ticks
  if (timerTicks >= 50)  // si atteint 50 ticks
    timerTicks = 0;    // restart le compteur a 0
}

void ISR_encodeur()
{
  bool valeurCHB = (PINE & (1 << PE3));  // read l'état de enc chb

  if (!valeurCHB)      // si c'est a 0
  {
    if (encoderVal < 99)  // si valeur est moins de 99
      encoderVal++;    // increase la valeur de l'encodeur
  }
  else                 // si le B est à 1
  {
    if (encoderVal > 0)  // si la valeur est + que 0
      encoderVal--;    // diminue la valeur de l'encodeur
  }
}

void setupTimer3()
{
  TCCR3A = 0;          // definit le registre de controle A
  TCCR3B = 0;          // definit le registre de controle B
  TCNT3 = 0;           // definit compteur a 0
  TCCR3B |= (1 << WGM32);  // active mode CTC
  OCR3A = 1249;        // définit la valeur de comparaison
  TCCR3B |= (1 << CS32);   // config le prescaler a 256
  TIMSK3 |= (1 << OCIE3A); // l'interruption = on
}

void setup()
{
  pinMode(BOUTON_HAUT, INPUT_PULLUP);     // configure le bouton haut
  pinMode(BOUTON_BAS, INPUT_PULLUP);      // configure le bouton bas
  pinMode(BOUTON_GAUCHE, INPUT_PULLUP);   // configure le bouton gauche
  pinMode(BOUTON_DROITE, INPUT_PULLUP);   // configure le bouton droite
  pinMode(BOUTON_B, INPUT_PULLUP);        // configure bouton B
  pinMode(BOUTON_A, INPUT_PULLUP);        // configure bouton A
  pinMode(BOUTON_C, INPUT_PULLUP);        // configure bouton C

  DDRE &= ~(1 << ENC_CHA);  // ENC_CHA en mode entrée
  DDRE &= ~(1 << ENC_CHB);  // ENC_CHB en mode entrée

  matrice.MOMO_RGB_MatrixInit(54, 55, 56, 57, 11, 10, 9, false, 64, 32);
  matrice.begin();  // matrice = on

  attachInterrupt(1, ISR_encodeur, RISING);
  setupTimer3();

  EEPROM.get(0, highScore);
  if (highScore < 0 || highScore > 1000)  // si la valeur est invalide
    highScore = 0;  // met le meilleur score a 0

  randomSeed(analogRead(A7)); // initialise randomSeed
  sei();            // active interruptions
}

void afficherTitre()
{
  if (!needRedraw)    // si l'écran a pas besoin d'être redessiné
    return;           // sortir du fonction

  int potVal = analogRead(POT_PIN);  // read la valeur du potentiomètre
  godModeEnabled = (potVal < 512);   // active Godmode si valeur est moindes de 512

  matrice.fillScreen(0);  // wipe screen

  matrice.setCursor(14, 2);                   // cursor
  matrice.setTextColor(matrice.Color888(255, 0, 255));  // jaune
  matrice.setTextSize(1);                     // text = 1
  matrice.print("GOD:");                      // texte "GOD:"

  matrice.setCursor(38, 2);                      // cursor
  matrice.print(godModeEnabled ? "ON" : "OFF");  // montre "ON" si enabled "OFF" si non

  matrice.setCursor(10, 12);                            // cursor
  matrice.setTextColor(matrice.Color888(255, 255, 0));  // couleur mauve
  matrice.print("PROJET");                    // texte "PROJET"

  matrice.setCursor(49, 12);                  // cursor
  matrice.print("2");                         // texte "2"

  matrice.setTextColor(matrice.Color888(0, 255, 0));  // couleur bleu
  matrice.setCursor(14, 22);                          // cursor
  matrice.print("Vies:");                     // le texte "Vies:"

  matrice.setCursor(44, 22);                              // cursor
  matrice.setTextColor(matrice.Color888(255, 255, 255));  // blanc
  matrice.print(encoderVal == 0 ? 3 : encoderVal);  // 3 si encodeur est 0, si non valeur de l'encodeur

  needRedraw = false;  // écran = redrawn
}

void afficherGameOver()
{
  if (!needRedraw)    // if l'écran a pas besoin d'être redrwan
    return;           // Sortir fonction

  matrice.fillScreen(0);  // wipe la matrice

  // Affiche "GAME" en rouge
  matrice.setCursor(8, 2);                    // Positionne le curseur
  matrice.setTextColor(matrice.Color888(255, 0, 0));  // Couleur rouge
  matrice.setTextSize(1);                     // Taille du texte à 1
  matrice.print("GAME");                      // Affiche le texte "GAME"

  // Affiche "OVER" en rouge
  matrice.setCursor(33, 2);                   // Positionne le curseur
  matrice.print("OVER");                      // Affiche le texte "OVER"

  // Affiche "Score:" en blanc
  matrice.setCursor(8, 13);                   // Positionne le curseur
  matrice.setTextColor(matrice.Color888(255, 255, 255));  // Couleur blanche
  matrice.print("Score:");                    // Affiche le texte "Score:"

  // Affiche le score final
  matrice.setCursor(44, 14);                  // Positionne le curseur
  matrice.print(score);                       // Affiche la valeur du score

  // Affiche "Best:" en jaune
  matrice.setCursor(8, 23);                   // Positionne le curseur
  matrice.setTextColor(matrice.Color888(255, 255, 0));  // Couleur jaune
  matrice.print("Best:");                     // Affiche le texte "Best:"

  // Affiche le meilleur score
  matrice.setCursor(38, 23);                  // Positionne le curseur
  matrice.print(highScore);                   // Affiche la valeur du meilleur score

  needRedraw = false;  // Indique que l'écran a été redessiné
}

// Fonction pour gérer l'apparition (spawn) des balles
void gererSpawnBalles()
{
  if (millis() - dernierSpawn < 1000)  // Si moins de 1 seconde s'est écoulée depuis le dernier spawn
    return;                            // Ne rien faire et sortir de la fonction

  int activeCount = 0;           // Compteur de balles actives
  bool auMoinsUneVerte = false;  // Indicateur si au moins une balle est verte (étape 3+)

  // Parcourt toutes les balles pour compter les balles actives et vertes
  for (int i = 0; i < 3; i++)
  {
    if (balles[i].active)        // Si la balle est active
    {
      activeCount++;             // Incrémente le compteur de balles actives
      if (balles[i].etape >= 3)  // Si la balle est à l'étape 3 ou plus (verte)
        auMoinsUneVerte = true;  // Indique qu'au moins une balle est verte
    }
  }

  // Fait apparaître une nouvelle balle si les conditions sont remplies
  if ((auMoinsUneVerte || activeCount == 0) && activeCount < 3)
  {
    for (int i = 0; i < 3; i++)  // Parcourt le tableau de balles
    {
      if (!balles[i].active)     // Si la balle n'est pas active
      {
        balles[i].spawn();       // Fait apparaître cette balle
        dernierSpawn = millis(); // Enregistre le temps du spawn
        break;                   // Sortir de la boucle après avoir spawné une balle
      }
    }
  }
}

// Fonction principale qui s'exécute en boucle
void loop()
{
  // Gestion de l'état TITRE (écran titre)
  if (etat == TITRE)
  {
    int potVal = analogRead(POT_PIN);    // Lit la valeur du potentiomètre
    bool newGodMode = (potVal < 512);    // Détermine le nouvel état du mode God

    // Si le mode God a changé
    if (newGodMode != godModeEnabled)
    {
      godModeEnabled = newGodMode;       // Met à jour le mode God
      needRedraw = true;                 // Indique qu'il faut redessiner l'écran
    }

    static long lastEncoderVal = 0;      // Variable statique pour mémoriser la dernière valeur de l'encodeur
    if (encoderVal != lastEncoderVal)    // Si la valeur de l'encodeur a changé
    {
      lastEncoderVal = encoderVal;       // Met à jour la dernière valeur
      needRedraw = true;                 // Indique qu'il faut redessiner l'écran
    }

    afficherTitre();  // Affiche l'écran titre

    // Si le bouton C est pressé (démarrer le jeu)
    if (digitalRead(BOUTON_C) == LOW)
    {
      etat = JEU;                        // Change l'état à JEU (jeu en cours)
      score = 0;                         // Réinitialise le score à 0
      vies = (encoderVal == 0) ? 3 : encoderVal;  // Définit le nombre de vies (3 si encodeur à 0)
      crosshairX = 32;                   // Recentre le viseur horizontalement
      crosshairY = 16;                   // Recentre le viseur verticalement
      for (int i = 0; i < 3; i++)        // Parcourt toutes les balles
        balles[i].active = false;        // Désactive toutes les balles
      dernierSpawn = millis();           // Enregistre le temps actuel
      needRedraw = true;                 // Indique qu'il faut redessiner l'écran
      delay(200);                        // Délai pour éviter les rebonds du bouton
    }
    return;  // Sortir de la fonction loop pour ce cycle
  }

  // Gestion de l'état GAMEOVER (fin de partie)
  if (etat == GAMEOVER)
  {
    afficherGameOver();  // Affiche l'écran de fin de partie

    // Si le nouveau score est supérieur au meilleur score
    if (score > highScore)
    {
      highScore = score;         // Met à jour le meilleur score
      EEPROM.put(0, highScore);  // Sauvegarde le nouveau meilleur score en EEPROM
    }

    // Si le bouton C est pressé (retourner au menu)
    if (digitalRead(BOUTON_C) == LOW)
    {
      etat = TITRE;        // Retourne à l'écran titre
      needRedraw = true;   // Indique qu'il faut redessiner l'écran
      delay(200);          // Délai pour éviter les rebonds du bouton
    }
    return;  // Sortir de la fonction loop pour ce cycle
  }

  // Vérification de la condition de fin de partie
  if (vies <= 0 || digitalRead(BOUTON_A) == LOW)  // Si plus de vies ou bouton A pressé
  {
    etat = GAMEOVER;       // Change l'état à GAMEOVER
    needRedraw = true;     // Indique qu'il faut redessiner l'écran
    delay(200);            // Délai pour éviter les rebonds du bouton
    return;                // Sortir de la fonction loop pour ce cycle
  }

  // Gestion du déplacement du viseur avec les boutons directionnels
  if (digitalRead(BOUTON_HAUT) == LOW && crosshairY > 2)   // Si bouton haut pressé et pas au bord
    crosshairY--;          // Déplace le viseur vers le haut
  if (digitalRead(BOUTON_BAS) == LOW && crosshairY < 29)   // Si bouton bas pressé et pas au bord
    crosshairY++;          // Déplace le viseur vers le bas
  if (digitalRead(BOUTON_GAUCHE) == LOW && crosshairX > 2) // Si bouton gauche pressé et pas au bord
    crosshairX--;          // Déplace le viseur vers la gauche
  if (digitalRead(BOUTON_DROITE) == LOW && crosshairX < 61) // Si bouton droite pressé et pas au bord
    crosshairX++;          // Déplace le viseur vers la droite

  // Gestion du tir avec le bouton B
  if (digitalRead(BOUTON_B) == LOW)  // Si le bouton B est pressé
  {
    for (int i = 0; i < 3; i++)      // Parcourt toutes les balles
    {
      if (balles[i].estTouche(crosshairX, crosshairY))  // Si le viseur touche la balle
      {
        balles[i].active = false;    // Désactive la balle (elle est détruite)
        score++;                     // Augmente le score de 1 point
      }
    }
    delay(200);  // Délai pour éviter les tirs multiples avec un seul appui
  }

  gererSpawnBalles();  // Gère l'apparition de nouvelles balles

  // Met à jour toutes les balles
  for (int i = 0; i < 3; i++)
    balles[i].update();  // Met à jour chaque balle

  cli();  // Désactive les interruptions temporairement pour éviter les scintillements

  matrice.fillScreen(0);  // Efface l'écran (remplit de noir)

  // Dessine toutes les balles actives
  for (int i = 0; i < 3; i++)
    balles[i].draw();  // Dessine chaque balle

  // Dessine le viseur (croix rouge de 5 pixels)
  matrice.drawPixel(crosshairX, crosshairY, matrice.Color888(255, 0, 0));      // Centre du viseur
  matrice.drawPixel(crosshairX - 1, crosshairY, matrice.Color888(255, 0, 0));  // Gauche
  matrice.drawPixel(crosshairX + 1, crosshairY, matrice.Color888(255, 0, 0));  // Droite
  matrice.drawPixel(crosshairX, crosshairY - 1, matrice.Color888(255, 0, 0));  // Haut
  matrice.drawPixel(crosshairX, crosshairY + 1, matrice.Color888(255, 0, 0));  // Bas

  // Affiche le nombre de vies en haut à gauche en rouge
  matrice.setCursor(1, 1);                    // Positionne le curseur
  matrice.setTextColor(matrice.Color888(255, 0, 0));  // Couleur rouge
  matrice.setTextSize(1);                     // Taille du texte à 1
  matrice.print(godModeEnabled ? 0 : vies);   // Affiche 0 si mode God, sinon le nombre de vies

  // Affiche le score en dessous des vies en blanc
  matrice.setCursor(1, 9);                    // Positionne le curseur
  matrice.setTextColor(matrice.Color888(255, 255, 255));  // Couleur blanche
  matrice.print(score);                       // Affiche le score actuel

  sei();  // Réactive les interruptions

  delay(18);  // Délai de 18 ms pour contrôler la vitesse de rafraîchissement (environ 55 fps)
}