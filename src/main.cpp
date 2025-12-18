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
// ----------------------------------------------------
//
/**************************************************************/

//*********************************************
// Inclusion des librairies nécessaire au projet
//*********************************************

#include <Arduino.h>         // librairie Arduino
#include <MOMO_RGB_Matrix.h> // librairie de la matrice
#include <EEPROM.h>          // librairie pour EEPROM

#define BOUTON_C 32      // define bouton C
#define BOUTON_B 33      // define bouton B
#define BOUTON_A 31      // define bouton A
#define BOUTON_HAUT 35   // define bouton haut
#define BOUTON_BAS 30    // define bouton bas
#define BOUTON_GAUCHE 36 // define bouton gauche
#define BOUTON_DROITE 34 // define bouton droite

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

volatile long encoderVal = 0;         // variable encoderVal
volatile unsigned int timerTicks = 0; // variable timerTicks

int score = 0;                  // variable score
int vies = 3;                   // variable vies
int highScore = 0;              // variable highScore
int crosshairX = 32;            // variable crosshairX
int crosshairY = 16;            // variable crosshairY
unsigned long dernierSpawn = 0; // variable dernierSpawn
bool godModeEnabled = false;    // variable godModeEnabled
bool needRedraw = true;         // variable needRedraw

class Ball
{
public:
  int x, y;            // variable position x et y
  int rayon;           // variable rayon
  int etape;           // variable etape
  bool active;         // variable active
  unsigned long temps; // variable temps

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
      x = random(10, 54); // x random entre 10 et 54
      y = random(12, 24); // x random entre 12 et 24
    } while (x < 13 && y < 17); // Évite l'endroit en haut à gauche pour le score et pour les vies
    rayon = 1;        // rayon = 1 pixel
    etape = 0;        // etape = 0
    active = true;    // active balle
    temps = millis(); // saves le temps
  }

  void update()
  {
    if (!active) // si balle n'est pas active
      return;    // rien faire

    unsigned long elapsed = millis() - temps; // temps écoulé

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
      active = false;                  // gg balle
      if (!godModeEnabled && vies > 0) // si god est pas activé et qu'il reste des vies
        vies--;                        // -1 vie
    }
  }

  void draw()
  {
    if (!active) // balle = pas active
      return;    // nul

    uint16_t couleur = (etape < 3)    ? matrice.Color888(255, 255, 255) // blanc
                       : (etape == 3) ? matrice.Color888(0, 80, 0)      // devrait etre vert, mais c'est bleu
                       : (etape == 4) ? matrice.Color888(0, 140, 0)     // bleu moyen
                                      : matrice.Color888(0, 200, 0);    // bleu final
    matrice.fillCircle(x, y, rayon, couleur);                           // cercle
  }

  bool estTouche(int cx, int cy)
  {
    if (!active)    // si balle est pas active
      return false; // pas être touchée

    int dx = cx - x;                             // distance horiozntal entre crosshair et centre de la balle
    int dy = cy - y;                             // distance vertical entre crosshair et centre de la balle
    return (dx * dx + dy * dy <= rayon * rayon); // true si crosshair est dans le cercle
  }
};

Ball balles[3]; // tableau 3 balles

ISR(TIMER3_COMPA_vect) // interruption du Timer 3
{
  timerTicks++;         // compteur de ticks
  if (timerTicks >= 50) // si atteint 50 ticks
    timerTicks = 0;     // restart le compteur a 0
}

void ISR_encodeur()
{
  bool valeurCHB = (PINE & (1 << PE3)); // read l'état de enc chb

  if (!valeurCHB) // si c'est a 0
  {
    if (encoderVal < 99) // si valeur est moins de 99
      encoderVal++;      // increase la valeur de l'encodeur
  }
  else // si le B est à 1
  {
    if (encoderVal > 0) // si la valeur est + que 0
      encoderVal--;     // diminue la valeur de l'encodeur
  }
}

void setupTimer3()
{
  TCCR3A = 0;              // definit le registre de controle A
  TCCR3B = 0;              // definit le registre de controle B
  TCNT3 = 0;               // definit compteur a 0
  TCCR3B |= (1 << WGM32);  // active mode CTC
  OCR3A = 1249;            // définit la valeur de comparaison
  TCCR3B |= (1 << CS32);   // config le prescaler a 256
  TIMSK3 |= (1 << OCIE3A); // l'interruption = on
}

void setup()
{
  pinMode(BOUTON_HAUT, INPUT_PULLUP);   // configure le bouton haut
  pinMode(BOUTON_BAS, INPUT_PULLUP);    // configure le bouton bas
  pinMode(BOUTON_GAUCHE, INPUT_PULLUP); // configure le bouton gauche
  pinMode(BOUTON_DROITE, INPUT_PULLUP); // configure le bouton droite
  pinMode(BOUTON_B, INPUT_PULLUP);      // configure bouton B
  pinMode(BOUTON_A, INPUT_PULLUP);      // configure bouton A
  pinMode(BOUTON_C, INPUT_PULLUP);      // configure bouton C

  DDRE &= ~(1 << ENC_CHA); // ENC_CHA en mode entrée
  DDRE &= ~(1 << ENC_CHB); // ENC_CHB en mode entrée

  matrice.MOMO_RGB_MatrixInit(54, 55, 56, 57, 11, 10, 9, false, 64, 32);
  matrice.begin(); // matrice = on

  attachInterrupt(1, ISR_encodeur, RISING);
  setupTimer3();

  EEPROM.get(0, highScore);
  if (highScore < 0 || highScore > 1000) // si la valeur est invalide
    highScore = 0;                       // met le meilleur score a 0

  randomSeed(analogRead(A7)); // initialise randomSeed
  sei();                      // active interruptions
}

void afficherTitre()
{
  if (!needRedraw) // si l'écran a pas besoin d'être redessiné
    return;        // sortir du fonction

  int potVal = analogRead(POT_PIN); // read la valeur du potentiomètre
  godModeEnabled = (potVal < 512);  // active Godmode si valeur est moindes de 512

  matrice.fillScreen(0); // wipe screen

  matrice.setCursor(14, 2);                            // cursor
  matrice.setTextColor(matrice.Color888(255, 0, 255)); // jaune
  matrice.setTextSize(1);                              // text = 1
  matrice.print("GOD:");                               // texte "GOD:"

  matrice.setCursor(38, 2);                     // cursor
  matrice.print(godModeEnabled ? "ON" : "OFF"); // montre "ON" si enabled "OFF" si non

  matrice.setCursor(10, 12);                           // cursor
  matrice.setTextColor(matrice.Color888(255, 255, 0)); // couleur mauve
  matrice.print("PROJET");                             // texte "PROJET"

  matrice.setCursor(49, 12); // cursor
  matrice.print("2");        // texte "2"

  matrice.setTextColor(matrice.Color888(0, 255, 0)); // couleur bleu
  matrice.setCursor(14, 22);                         // cursor
  matrice.print("Vies:");                            // le texte "Vies:"

  matrice.setCursor(44, 22);                             // cursor
  matrice.setTextColor(matrice.Color888(255, 255, 255)); // blanc
  matrice.print(encoderVal == 0 ? 3 : encoderVal);       // 3 si encodeur est 0, si non valeur de l'encodeur

  needRedraw = false; // écran = redrawn
}

void afficherGameOver()
{
  if (!needRedraw) // if l'écran a pas besoin d'être redrwan
    return;        // Sortir fonction

  matrice.fillScreen(0); // wipe la matrice

  matrice.setCursor(8, 2);                           // cursor
  matrice.setTextColor(matrice.Color888(255, 0, 0)); // rouge
  matrice.setTextSize(1);                            // taille texte a 1
  matrice.print("GAME");                             // texte "GAME"

  matrice.setCursor(33, 2); // cursor
  matrice.print("OVER");    // texte "OVER"

  matrice.setCursor(8, 13);                              // cursor
  matrice.setTextColor(matrice.Color888(255, 255, 255)); // blanc
  matrice.print("Score:");                               // texte "Score:"

  matrice.setCursor(44, 14); // cursor
  matrice.print(score);      // affiche score

  matrice.setCursor(8, 23);                            // cursor
  matrice.setTextColor(matrice.Color888(255, 255, 0)); // devrait être jaune, mais c'est autre chose sur la matrice, souviens pas
  matrice.print("Best:");                              // texte "Best:"

  matrice.setCursor(38, 23); // cursor
  matrice.print(highScore);  // montre meilleur score

  needRedraw = false; // écran redrawn
}

void gererSpawnBalles()
{
  if (millis() - dernierSpawn < 1000) // si moins de 1 sec depuis le dernierSpawn
    return;                           // rien faire et leave la fonction

  int activeCount = 0;    // activeCount = 0
  bool auMoinsUn = false; // au moins une balle

  for (int i = 0; i < 3; i++)
  {
    if (balles[i].active) // si la balle est active
    {
      activeCount++;            // compteur de balles actives
      if (balles[i].etape >= 3) // si balle est a l'étape 3 ou +
        auMoinsUn = true;       // montre qu'au moins une balle est a 3
    }
  }

  if ((auMoinsUn || activeCount == 0) && activeCount < 3)
  {
    for (int i = 0; i < 3; i++) // tableau de balles
    {
      if (!balles[i].active) // si la balle est pas active
      {
        balles[i].spawn();       // montrer cette balle
        dernierSpawn = millis(); // save le temps du spawn
        break;                   // break après avoir avoir une balle
      }
    }
  }
}

void loop()
{
  if (etat == TITRE) // etat == TITRE
  {
    int potVal = analogRead(POT_PIN); // lire la valeur du potentiomètre
    bool newGodMode = (potVal < 512); // nouvel état du godmode

    if (newGodMode != godModeEnabled)
    {
      godModeEnabled = newGodMode; // refresh godmode
      needRedraw = true;           // redraw matrice
    }

    static long lastEncoderVal = 0;   // variable static pour encodeur
    if (encoderVal != lastEncoderVal) // si la valeur de l'encodeur changer
    {
      lastEncoderVal = encoderVal; // refresh lastEncoderVal
      needRedraw = true;           // montre qu'il faut redraw la matrice
    }

    afficherTitre();

    if (digitalRead(BOUTON_C) == LOW)
    {
      etat = JEU;                                // l'état JEU
      score = 0;                                 // score = 0
      vies = (encoderVal == 0) ? 3 : encoderVal; // nombre de vies (3 si encodeur = 0)
      crosshairX = 32;                           // crosshair x centered
      crosshairY = 16;                           // crosshair y centered
      for (int i = 0; i < 3; i++)                // toutes les balles
        balles[i].active = false;                // turn off toutes les balles
      dernierSpawn = millis();                   // sauver le temps actuel
      needRedraw = true;                         // montre qu'il faut redraw la matrice
      delay(200);                                // delay
    }
    return;
  }

  if (etat == GAMEOVER)
  {
    afficherGameOver(); // afficherGameOver

    if (score > highScore) // si new score est > au meilleur score
    {
      highScore = score;        // refresh le meilleur score
      EEPROM.put(0, highScore); // sauver le nouveau meilleur score en EEPROM
    }

    if (digitalRead(BOUTON_C) == LOW)
    {
      etat = TITRE;      // écran titre
      needRedraw = true; // needRedraw, donc redraw la matrice
      delay(200);        // delay
    }
    return;
  }

  if (vies <= 0 || digitalRead(BOUTON_A) == LOW) // si vies sont 0 ou + ou bouton A est pesser
  {
    etat = GAMEOVER;   // état GAMEOVER
    needRedraw = true; // needRedraw matrice
    delay(200);        // delay
    return;            // return
  }

  if (digitalRead(BOUTON_HAUT) == LOW && crosshairY > 2)    // si bouton haut est pesser et pas au bord
    crosshairY--;                                           // crosshair vers haut
  if (digitalRead(BOUTON_BAS) == LOW && crosshairY < 29)    // si bouton bas est pesser et pas au bord
    crosshairY++;                                           // crosshair vers bas
  if (digitalRead(BOUTON_GAUCHE) == LOW && crosshairX > 2)  // si bouton gauche est pesser et pas au bord
    crosshairX--;                                           // crosshair vers gauche
  if (digitalRead(BOUTON_DROITE) == LOW && crosshairX < 61) // si bouton droite est pesser et pas au bord
    crosshairX++;                                           // crosshair vers droite

  if (digitalRead(BOUTON_B) == LOW) // si le bouton B est pesser
  {
    for (int i = 0; i < 3; i++) // regarder toutes les balles
    {
      if (balles[i].estTouche(crosshairX, crosshairY)) // si le crosshair touche balle
      {
        balles[i].active = false; // détruire la balle
        score++;                  // score +1
      }
    }
    delay(200); // delay
  }

  gererSpawnBalles(); // nouvelles balles

  for (int i = 0; i < 3; i++)
    balles[i].update(); // update chaque balle

  cli(); // turn off les interruptions

  matrice.fillScreen(0); // efface l'écran

  for (int i = 0; i < 3; i++)
    balles[i].draw(); // dessine chaque balle

  matrice.drawPixel(crosshairX, crosshairY, matrice.Color888(255, 0, 0));     // centre du crosshair
  matrice.drawPixel(crosshairX - 1, crosshairY, matrice.Color888(255, 0, 0)); // gauche
  matrice.drawPixel(crosshairX + 1, crosshairY, matrice.Color888(255, 0, 0)); // droite
  matrice.drawPixel(crosshairX, crosshairY - 1, matrice.Color888(255, 0, 0)); // haut
  matrice.drawPixel(crosshairX, crosshairY + 1, matrice.Color888(255, 0, 0)); // bas

  matrice.setCursor(1, 1);                           // cursor
  matrice.setTextColor(matrice.Color888(255, 0, 0)); // rouge
  matrice.setTextSize(1);                            // texte a 1
  matrice.print(godModeEnabled ? 0 : vies);          // 0 si godmode si non les vies

  matrice.setCursor(1, 9);                               // cursor
  matrice.setTextColor(matrice.Color888(255, 255, 255)); // blanc
  matrice.print(score);                                  // score actuel

  sei(); // active interruptions

  delay(18); // delay
}