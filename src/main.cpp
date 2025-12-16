/**************************************************************/
// Date de création du programme: 11/18/2025
// Date de la dernière modification: 12/18/2025
// Nom du programmeur principal: Sunpreet Singh
//
// Le but principal de ce projet est de développer un jeu au choix qui utilisera l’interface matrice.
// Contrairement aux projets habituels, vous choisirez quel jeu vous voulez développer et comment le faire en respectant les requis minimum.
// Avant de commencer à coder, vous devrez faire valider un document qui référencera au maximum les fonctionnalités de votre jeu.
// Ce document appelé Document de Conception Logicielle (DCL) ou Software Design Document (SDD). Dans le cas d’un jeu, nous parlons de Game Design Document (GDD).
//
// ** CTRL + T dans Arduino IDE pour formater le code**
//
/**************************************************************/

//*********************************************
// Inclusion des librairies nécessaire au projet
//*********************************************

#include <Arduino.h>
#include <MOMO_RGB_Matrix.h>
#include <EEPROM.h>

#define BOUTON_C 32
#define BOUTON_B 33
#define BOUTON_A 31
#define BOUTON_HAUT 35
#define BOUTON_BAS 30
#define BOUTON_GAUCHE 36
#define BOUTON_DROITE 34
#define ENC_CHA PE5
#define ENC_CHB PE3
#define POT_PIN A6

MOMO_RGB_Matrix matrice;

enum GameState
{
  TITLE,
  PLAYING,
  GAMEOVER
};
GameState etat = TITLE;

volatile long encoderVal = 0;
volatile unsigned int timerTicks = 0;

int score = 0;
int vies = 3;
int highScore = 0;
int crosshairX = 32;
int crosshairY = 16;
unsigned long dernierSpawn = 0;
bool godModeEnabled = false;
bool needRedraw = true;

class Ball
{
public:
  int x, y, rayon, etape;
  bool active;
  unsigned long temps;

  Ball()
  {
    x = 0;
    y = 0;
    rayon = 1;
    etape = 0;
    active = false;
    temps = 0;
  }

  void spawn()
  {
    do
    {
      x = random(10, 54);
      y = random(12, 24);
    } while (x < 13 && y < 17);
    rayon = 1;
    etape = 0;
    active = true;
    temps = millis();
  }

  void update()
  {
    if (!active)
      return;
    unsigned long elapsed = millis() - temps;
    if (etape < 2 && elapsed >= 1000)
    {
      rayon++;
      etape++;
      temps = millis();
    }
    else if (etape == 2 && elapsed >= 2000)
    {
      etape = 3;
      temps = millis();
    }
    else if (etape == 3 && elapsed >= 2000)
    {
      etape = 4;
      temps = millis();
    }
    else if (etape == 4 && elapsed >= 2000)
    {
      etape = 5;
      temps = millis();
    }
    else if (etape == 5 && elapsed >= 2000)
    {
      active = false;
      if (!godModeEnabled && vies > 0)
        vies--;
    }
  }

  void draw()
  {
    if (!active)
      return;
    uint16_t couleur = (etape < 3)    ? matrice.Color888(255, 255, 255)
                       : (etape == 3) ? matrice.Color888(0, 80, 0)
                       : (etape == 4) ? matrice.Color888(0, 140, 0)
                                      : matrice.Color888(0, 200, 0);
    matrice.fillCircle(x, y, rayon, couleur);
  }

  bool estTouche(int cx, int cy)
  {
    if (!active)
      return false;
    int dx = cx - x;
    int dy = cy - y;
    return (dx * dx + dy * dy <= rayon * rayon);
  }
};

Ball balles[3];

ISR(TIMER3_COMPA_vect)
{
  timerTicks++;
  if (timerTicks >= 50)
    timerTicks = 0;
}

void ISR_encodeur()
{
  bool valeurCHB = (PINE & (1 << PE3));
  if (!valeurCHB)
  {
    if (encoderVal < 99)
      encoderVal++;
  }
  else
  {
    if (encoderVal > 0)
      encoderVal--;
  }
}

void setupTimer3()
{
  TCCR3A = 0;
  TCCR3B = 0;
  TCNT3 = 0;
  TCCR3B |= (1 << WGM32);
  OCR3A = 1249;
  TCCR3B |= (1 << CS32);
  TIMSK3 |= (1 << OCIE3A);
}

void setup()
{
  pinMode(BOUTON_HAUT, INPUT_PULLUP);
  pinMode(BOUTON_BAS, INPUT_PULLUP);
  pinMode(BOUTON_GAUCHE, INPUT_PULLUP);
  pinMode(BOUTON_DROITE, INPUT_PULLUP);
  pinMode(BOUTON_B, INPUT_PULLUP);
  pinMode(BOUTON_A, INPUT_PULLUP);
  pinMode(BOUTON_C, INPUT_PULLUP);

  DDRE &= ~(1 << ENC_CHA);
  DDRE &= ~(1 << ENC_CHB);

  matrice.MOMO_RGB_MatrixInit(54, 55, 56, 57, 11, 10, 9, false, 64, 32);
  matrice.begin();

  attachInterrupt(1, ISR_encodeur, RISING);
  setupTimer3();

  EEPROM.get(0, highScore);
  if (highScore < 0 || highScore > 1000)
    highScore = 0;

  randomSeed(analogRead(A7));
  sei();
}

void afficherTitre()
{
  if (!needRedraw)
    return;

  int potVal = analogRead(POT_PIN);
  godModeEnabled = (potVal < 512);

  matrice.fillScreen(0);
  matrice.setCursor(14, 2);
  matrice.setTextColor(matrice.Color888(255, 0, 255));
  matrice.setTextSize(1);
  matrice.print("GOD:");
  matrice.setCursor(38, 2);
  matrice.print(godModeEnabled ? "ON" : "OFF");

  matrice.setCursor(10, 12);
  matrice.setTextColor(matrice.Color888(255, 255, 0));
  matrice.print("PROJET");
  matrice.setCursor(49, 12);
  matrice.print("2");

  matrice.setTextColor(matrice.Color888(0, 255, 0));
  matrice.setCursor(14, 22);
  matrice.print("Vies:");
  matrice.setCursor(44, 22);
  matrice.setTextColor(matrice.Color888(255, 255, 255));
  matrice.print(encoderVal == 0 ? 3 : encoderVal);

  needRedraw = false;
}

void afficherGameOver()
{
  if (!needRedraw)
    return;

  matrice.fillScreen(0);
  matrice.setCursor(8, 2);
  matrice.setTextColor(matrice.Color888(255, 0, 0));
  matrice.setTextSize(1);
  matrice.print("GAME");
  matrice.setCursor(33, 2);
  matrice.print("OVER");

  matrice.setCursor(8, 13);
  matrice.setTextColor(matrice.Color888(255, 255, 255));
  matrice.print("Score:");
  matrice.setCursor(44, 14);
  matrice.print(score);

  matrice.setCursor(8, 23);
  matrice.setTextColor(matrice.Color888(255, 255, 0));
  matrice.print("Best:");
  matrice.setCursor(38, 23);
  matrice.print(highScore);

  needRedraw = false;
}

void gererSpawnBalles()
{
  if (millis() - dernierSpawn < 1000)
    return;

  int activeCount = 0;
  bool auMoinsUneVerte = false;

  for (int i = 0; i < 3; i++)
  {
    if (balles[i].active)
    {
      activeCount++;
      if (balles[i].etape >= 3)
        auMoinsUneVerte = true;
    }
  }

  if ((auMoinsUneVerte || activeCount == 0) && activeCount < 3)
  {
    for (int i = 0; i < 3; i++)
    {
      if (!balles[i].active)
      {
        balles[i].spawn();
        dernierSpawn = millis();
        break;
      }
    }
  }
}

void loop()
{
  if (etat == TITLE)
  {
    int potVal = analogRead(POT_PIN);
    bool newGodMode = (potVal < 512);
    if (newGodMode != godModeEnabled)
    {
      godModeEnabled = newGodMode;
      needRedraw = true;
    }

    static long lastEncoderVal = 0;
    if (encoderVal != lastEncoderVal)
    {
      lastEncoderVal = encoderVal;
      needRedraw = true;
    }

    afficherTitre();

    if (digitalRead(BOUTON_C) == LOW)
    {
      etat = PLAYING;
      score = 0;
      vies = (encoderVal == 0) ? 3 : encoderVal;
      crosshairX = 32;
      crosshairY = 16;
      for (int i = 0; i < 3; i++)
        balles[i].active = false;
      dernierSpawn = millis();
      needRedraw = true;
      delay(200);
    }
    return;
  }

  if (etat == GAMEOVER)
  {
    afficherGameOver();
    if (score > highScore)
    {
      highScore = score;
      EEPROM.put(0, highScore);
    }
    if (digitalRead(BOUTON_C) == LOW)
    {
      etat = TITLE;
      needRedraw = true;
      delay(200);
    }
    return;
  }

  if (vies <= 0 || digitalRead(BOUTON_A) == LOW)
  {
    etat = GAMEOVER;
    needRedraw = true;
    delay(200);
    return;
  }

  if (digitalRead(BOUTON_HAUT) == LOW && crosshairY > 2)
    crosshairY--;
  if (digitalRead(BOUTON_BAS) == LOW && crosshairY < 29)
    crosshairY++;
  if (digitalRead(BOUTON_GAUCHE) == LOW && crosshairX > 2)
    crosshairX--;
  if (digitalRead(BOUTON_DROITE) == LOW && crosshairX < 61)
    crosshairX++;

  if (digitalRead(BOUTON_B) == LOW)
  {
    for (int i = 0; i < 3; i++)
    {
      if (balles[i].estTouche(crosshairX, crosshairY))
      {
        balles[i].active = false;
        score++;
      }
    }
    delay(200);
  }

  gererSpawnBalles();

  for (int i = 0; i < 3; i++)
    balles[i].update();

  cli();
  matrice.fillScreen(0);

  for (int i = 0; i < 3; i++)
    balles[i].draw();

  matrice.drawPixel(crosshairX, crosshairY, matrice.Color888(255, 0, 0));
  matrice.drawPixel(crosshairX - 1, crosshairY, matrice.Color888(255, 0, 0));
  matrice.drawPixel(crosshairX + 1, crosshairY, matrice.Color888(255, 0, 0));
  matrice.drawPixel(crosshairX, crosshairY - 1, matrice.Color888(255, 0, 0));
  matrice.drawPixel(crosshairX, crosshairY + 1, matrice.Color888(255, 0, 0));

  matrice.setCursor(1, 1);
  matrice.setTextColor(matrice.Color888(255, 0, 0));
  matrice.setTextSize(1);
  matrice.print(godModeEnabled ? 0 : vies);

  matrice.setCursor(1, 9);
  matrice.setTextColor(matrice.Color888(255, 255, 255));
  matrice.print(score);

  sei();
  delay(18);
}