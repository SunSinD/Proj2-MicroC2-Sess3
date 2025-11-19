/**************************************************************/
// Date de création du programme: 10/28/2025
// Date de la dernière modification: 11/13/2025
// Nom du programmeur principal: Sunpreet Singh
//
// Le but principal de ce projet est de développer un programme qui servira d'outil de test unifié et portable pour tester l'ensemble de fonctionnalité de l'interface matrice.
// Cet outil servira aux techniciens du département pour deux fonctions critiques :
// 1. Diagnostic de pannes pour la réparation : Le système doit permettre aux techniciens d'isoler rapidement les composants défectueux sur les cartes électroniques en réparation.
// 2. Validation post-assemblage et contrôle qualité : Après l'assemblage d'un appareil, cet outil servira de banc d'essai rapide pour garantir que tous les composants fonctionnent selon les spécifications.
//
// ** CTRL + T dans Arduino IDE pour formater le code**
//
/**************************************************************/

//*********************************************
// Inclusion des librairies nécessaire au projet
//*********************************************

#include <Arduino.h>         // library principale
#include <MOMO_RGB_Matrix.h> // library pour la matrice

#define BOUTON_A 31      // define le bouton A
#define BOUTON_B 33      // define le bouton B
#define BOUTON_C 32      // define le bouton C
#define BOUTON_HAUT 35   // define le bouton haut
#define BOUTON_BAS 30    // define le bouton bas
#define BOUTON_GAUCHE 36 // define le bouton gauche
#define BOUTON_DROITE 34 // define le bouton droit

#define ENCODEUR_A 3      // define le l'encodeur A
#define ENCODEUR_B 4      // define le l'encodeur B
#define BOUTON_ENCODEUR 5 // define le le boutdon de l'encodeur

#define POTENTIOMETRE A6   // define le potentiomètre
#define PHOTORESISTANCE A7 // define la photorésistance

#define LED1_ROUGE 39 // define le LED1 rouge
#define LED1_VERT 40  // define le LED1 vert
#define LED1_BLEU 41  // define le LED1 bleu
#define LED2_ROUGE 42 // define le LED2 rouge
#define LED2_VERT 43  // define le LED2 vert
#define LED2_BLEU 44  // define le LED2 bleu
#define LED3_ROUGE 45 // define le LED3 rouge
#define LED3_VERT 46  // define le LED3 vert
#define LED3_BLEU 47  // define le LED3 bleu

MOMO_RGB_Matrix matrice;

volatile long positionEncodeur = 0;      // position l'encodeur
bool boutons[7] = {false};               // État boutons
bool boutonsPrecedents[7] = {false};     // État précédent boutons
long valeurEncodeur = 0;                 // valeur encodeur
long valeurEncodeurPrecedente = 0;       // valeur précédente encodeur
int valeurPotentiometre = 0;             // valeur potentiomètre
int valeurPotentiometrePrecedente = 0;   // valeur précédente potentiomètre
int valeurPhotoresistance = 0;           // valeur photorésistance
int valeurPhotoresistancePrecedente = 0; // valeur précédente photorésistance
bool matriceBesoinUpdate = true;         // update de la matrice

unsigned long dernierTempsDebounce = 0; // debounce
const unsigned long delaiDebounce = 20; // délai debounce

enum Mode
{
  MODE_TEST_ENTREES,      // test du entrée
  MODE_ANIMATION,         // animation
  MODE_ANIMATION_PERSONAL // animation custom selon moi
};

Mode modeActuel = MODE_TEST_ENTREES;

void ISR_encodeur() // source : gbeaulieuMontmo / exempleEncodeur / src / main.cpp
{
  bool valeurCHB = digitalRead(ENCODEUR_B); // lire l'état du channel B

  // Déterminer le sens de rotation
  if (!valeurCHB)
  {
    positionEncodeur++;
    // Attention à ne pas mettre de Serial dans une ISR en pratique
    // on peut le faire avec parcimonie pour du debug
    // Serial.println("Sens horaire");
  }
  else
  {
    positionEncodeur--;
    // Serial.println("Sens anti-horaire");
  }
}

void setupPins()
{
  pinMode(BOUTON_A, INPUT_PULLUP);        // bouton A entrée
  pinMode(BOUTON_B, INPUT_PULLUP);        // bouton B entrée
  pinMode(BOUTON_C, INPUT_PULLUP);        // bouton C entrée
  pinMode(BOUTON_HAUT, INPUT_PULLUP);     // bouton haut entrée
  pinMode(BOUTON_BAS, INPUT_PULLUP);      // bouton bas entrée
  pinMode(BOUTON_GAUCHE, INPUT_PULLUP);   // bouton gauche entrée
  pinMode(BOUTON_DROITE, INPUT_PULLUP);   // bouton droite entrée
  pinMode(BOUTON_ENCODEUR, INPUT_PULLUP); // bouton encodeur entrée
  pinMode(ENCODEUR_A, INPUT_PULLUP);      // encodeur A entrée
  pinMode(ENCODEUR_B, INPUT_PULLUP);      // encodeur B entrée

  pinMode(LED1_ROUGE, OUTPUT); // LED1 rouge sortie
  pinMode(LED1_VERT, OUTPUT);  // LED1 vert sortie
  pinMode(LED1_BLEU, OUTPUT);  // LED1 bleu sortie
  pinMode(LED2_ROUGE, OUTPUT); // LED2 rouge sortie
  pinMode(LED2_VERT, OUTPUT);  // LED2 vert sortie
  pinMode(LED2_BLEU, OUTPUT);  // LED2 bleu sortie
  pinMode(LED3_ROUGE, OUTPUT); // LED3 rouge sortie
  pinMode(LED3_VERT, OUTPUT);  // LED3 vert sortie
  pinMode(LED3_BLEU, OUTPUT);  // LED3 bleu sortie

  attachInterrupt(1, ISR_encodeur, RISING); // l'interruption pour l'encodeur
}

void lireEntrees() // lire les entrées
{
  unsigned long tempsActuel = millis(); // temps en ms

  for (int i = 0; i < 7; i++)
  {
    boutonsPrecedents[i] = boutons[i]; // sauver l'état précédent
  }

  boutons[0] = !digitalRead(BOUTON_HAUT);   // Lit bouton haut
  boutons[1] = !digitalRead(BOUTON_BAS);    // Lit bouton bas
  boutons[2] = !digitalRead(BOUTON_GAUCHE); // Lit bouton gauche
  boutons[3] = !digitalRead(BOUTON_DROITE); // Lit bouton droite
  boutons[4] = !digitalRead(BOUTON_A);      // Lit bouton A
  boutons[5] = !digitalRead(BOUTON_B);      // Lit bouton B
  boutons[6] = !digitalRead(BOUTON_C);      // Lit bouton C

  valeurEncodeurPrecedente = valeurEncodeur;               // sauver valeur encodeur
  valeurPotentiometrePrecedente = valeurPotentiometre;     // sauver valeur potentiomètre
  valeurPhotoresistancePrecedente = valeurPhotoresistance; // sauver valeur photorésistance

  valeurEncodeur = positionEncodeur;                   // Lit new valeur encodeur
  valeurPotentiometre = analogRead(POTENTIOMETRE);     // Lit valeur potentiomètre
  valeurPhotoresistance = analogRead(PHOTORESISTANCE); // Lit valeur photorésistance

  for (int i = 0; i < 7; i++)
  {
    if (boutons[i] && !boutonsPrecedents[i])
    {
      const char *nomsBoutons[] = {"HAUT", "BAS", "GAUCHE", "DROITE", "A", "B", "C"}; // les boutons
      Serial.print("Bouton : ");                                                      // montre texte
      Serial.println(nomsBoutons[i]);                                                 // montre nom du bouton
    }
  }

  if (modeActuel == MODE_TEST_ENTREES)
  {
    bool changementDetecte = false; // détecter changement

    for (int i = 0; i < 7; i++)
    {
      if (boutons[i] != boutonsPrecedents[i])
      {
        changementDetecte = true; // changement détecté = vrai
        break;
      }
    }

    if (abs(valeurEncodeur - valeurEncodeurPrecedente) > 0 || abs(valeurPotentiometre - valeurPotentiometrePrecedente) > 5 || abs(valeurPhotoresistance - valeurPhotoresistancePrecedente) > 5)
    {
      changementDetecte = true; // changement détecté = vrai
    }

    if (changementDetecte)
    {
      matriceBesoinUpdate = true;         // demande un update la matrice
      dernierTempsDebounce = tempsActuel; // update temps debounce
    }
  }
  else
  { // seulement si il y a une animation
    for (int i = 0; i < 7; i++)
    {
      if (boutons[i] != boutonsPrecedents[i])
      {
        matriceBesoinUpdate = true;         // demande un update la matrice
        dernierTempsDebounce = tempsActuel; // update temps debounce
        break;
      }
    }
  }
}

void updateLEDS()
{
  digitalWrite(LED1_ROUGE, LOW); // ferme LED1 rouge
  digitalWrite(LED1_VERT, LOW);  // ferme LED1 vert
  digitalWrite(LED1_BLEU, LOW);  // ferme LED1 bleu
  digitalWrite(LED2_ROUGE, LOW); // ferme LED2 rouge
  digitalWrite(LED2_VERT, LOW);  // ferme LED2 vert
  digitalWrite(LED2_BLEU, LOW);  // ferme LED2 bleu
  digitalWrite(LED3_ROUGE, LOW); // ferme LED3 rouge
  digitalWrite(LED3_VERT, LOW);  // ferme LED3 vert
  digitalWrite(LED3_BLEU, LOW);  // ferme LED3 bleu

  if (boutons[4])
  {
    digitalWrite(LED1_VERT, HIGH); // allume LED1 vert
    digitalWrite(LED2_VERT, HIGH); // allume LED2 vert
    digitalWrite(LED3_VERT, HIGH); // allume LED3 vert
  }
  if (boutons[5])
  {
    digitalWrite(LED1_ROUGE, HIGH); // allume LED1 rouge
    digitalWrite(LED2_ROUGE, HIGH); // allume LED2 rouge
    digitalWrite(LED3_ROUGE, HIGH); // allume LED3 rouge
  }
  if (boutons[6])
  {
    digitalWrite(LED1_BLEU, HIGH); // allume LED1 bleu
    digitalWrite(LED2_BLEU, HIGH); // allume LED2 bleu
    digitalWrite(LED3_BLEU, HIGH); // allume LED3 bleu
  }
}

void drawArrow(int x, int y, int dir, bool presse) // dessiner des flèches
{
  int t = 3;  // taille de la flèche
  int coul = matrice.Color333(7, 7, 7); // Couleur remplissage
  int cont = matrice.Color333(2, 2, 2); // Couleur contour

  int px, py, b1x, b1y, b2x, b2y; // les points

  if (dir == 0) // haut
  {
    px = x;
    py = y - t; // point haut
    b1x = x - t;
    b1y = y; // coin gauche
    b2x = x + t;
    b2y = y; // coin droite
  }
  else if (dir == 1) // bas
  {
    px = x;
    py = y + t; // point bas
    b1x = x - t;
    b1y = y; // coin gauche
    b2x = x + t;
    b2y = y; // coin droite
  }
  else if (dir == 2) // gauche
  {
    px = x - t;
    py = y; // point gauche
    b1x = x;
    b1y = y - t; // coin haut
    b2x = x;
    b2y = y + t; // coin bas
  }
  else // droite
  {
    px = x + t;
    py = y; // point droite
    b1x = x;
    b1y = y - t; // coin haut
    b2x = x;
    b2y = y + t; // coin bas
  }

  py++;
  b1y++;
  b2y++; // décale vers le bas

  if (presse) // si pesser
  {
    matrice.fillTriangle(px, py, b1x, b1y, b2x, b2y, coul); // dessine la flèche
  }
  else
  {
    matrice.drawLine(px, py, b1x, b1y, cont);   // ligne 1
    matrice.drawLine(px, py, b2x, b2y, cont);   // ligne 2
    matrice.drawLine(b1x, b1y, b2x, b2y, cont); // ligne 3
  }
}

void afficherModeEntree()
{
  if (!matriceBesoinUpdate)
    return;

  matrice.fillScreen(0);                           // wipe
  matrice.setTextSize(1);                          // taille texte = 1
  matrice.setTextColor(matrice.Color333(7, 7, 7)); // couleur texte

  matrice.setCursor(2, 1);
  matrice.print(valeurPotentiometre); // montre valeur potentiomètre
  matrice.setCursor(2, 12);
  matrice.print(valeurPhotoresistance); // montre valeur photorésistance
  matrice.setCursor(2, 23);
  matrice.print(valeurEncodeur); // montre valeur encodeur

  drawArrow(30, 14, 0, boutons[0]); // flèche haut
  drawArrow(30, 24, 1, boutons[1]); // flèche bas
  drawArrow(25, 19, 2, boutons[2]); // flèche gauche
  drawArrow(35, 19, 3, boutons[3]); // flèche droite

  int bx = 59;
  int gris = matrice.Color333(3, 3, 3); // gris

  matrice.setTextColor(matrice.Color333(4, 4, 4)); // texte gris
  matrice.setCursor(50, 4);
  matrice.print("A"); // montre lettre A
  matrice.setCursor(50, 13);
  matrice.print("B"); // montre lettre B
  matrice.setCursor(50, 22);
  matrice.print("C"); // montre lettre C

  if (boutons[4])
    matrice.fillCircle(bx, 7, 2, matrice.Color333(0, 0, 7)); // A pessé, cercle bleu
  else
    matrice.drawCircle(bx, 7, 2, gris); // else cercle gris

  if (boutons[5])
    matrice.fillCircle(bx, 16, 2, matrice.Color333(7, 0, 0)); // B pessé, cercle rouge
  else
    matrice.drawCircle(bx, 16, 2, gris); // else cercle gris

  if (boutons[6])
    matrice.fillCircle(bx, 25, 2, matrice.Color333(0, 7, 0)); // C pessé, cercle vert
  else
    matrice.drawCircle(bx, 25, 2, gris); // else cercle gris

  matriceBesoinUpdate = false;
}

void afficherModeAnimation() // animation
{
  static int frame = 0;                       // num de frame 
  static unsigned long dernierTempsFrame = 0; // temps de la dernière frame
  static int couleurActuelle = 0;             // couleur
  static bool initialise = false;             // initialisation
  unsigned long tempsActuel = millis();       // temps

  if (!initialise)
  {
    frame = 0;                       // frame
    couleurActuelle = 0;             // couleur
    matrice.fillScreen(0);           // efface l'écran
    initialise = true;               // initialisé
    dernierTempsFrame = tempsActuel; // sauver le temps
  }

  if (tempsActuel - dernierTempsFrame >= 3)
  {
    if (frame == 0)
    {
      matrice.fillScreen(0); // efface l'écran
    }

    int couleur; // couleur
    if (couleurActuelle == 0)
    {                                      // if couleur 0
      couleur = matrice.Color333(0, 7, 0); // vert
    }
    else if (couleurActuelle == 1)
    {                                      // if couleur 1
      couleur = matrice.Color333(7, 0, 0); // rouge
    }
    else
    {                                      // if couleur 2
      couleur = matrice.Color333(0, 0, 7); // bleu
    }

    matrice.fillRect(0, 0, 64, frame, couleur); // rectangle

    frame++; // frame

    if (frame > 32)
    {
      frame = 0;                                   // frame
      couleurActuelle = (couleurActuelle + 1) % 3; // change couleur
    }

    dernierTempsFrame = tempsActuel;
  }
}

void afficherAnimationPersonal() // animation personnelle
{
  static int positionY = 0;                  // position Y
  static int direction = 1;                  // direction
  static unsigned long dernierMouvement = 0; // temps du dernier mouvement
  static bool texteAffiche = false;          // affichage du texte
  static int declencheurTexteY = 16;         // position Y déclencher texte
  static bool initialise = false;            // initialisation
  unsigned long tempsActuel = millis();      // temps actuel

  if (!initialise)
  {
    positionY = 0;         // réinitialise position
    direction = 1;         // réinitialise direction
    texteAffiche = false;  // réinitialise état texte
    matrice.fillScreen(0); // efface l'écran
    initialise = true;     // initialisé = vrai
  }

  if (tempsActuel - dernierMouvement > 25) // if 25ms ont passées
  {
    bool devraitAfficherTexte = (positionY > declencheurTexteY && direction == 1) || (positionY > declencheurTexteY && direction == -1 && texteAffiche); // vérifie afficher texte

    if (devraitAfficherTexte != texteAffiche)
    {                                      // si état texte = changé
      matrice.fillScreen(0);               // efface l'écran
      texteAffiche = devraitAfficherTexte; // update état texte
    }
    else
    { // si état texte != changé
      for (int x = 0; x < 64; x++)
      {
        matrice.drawPixel(x, positionY - direction, 0); // efface ligne précédente
      }
    }

    if (texteAffiche)
    {
      matrice.setTextSize(1);                          // taille texte
      matrice.setTextColor(matrice.Color333(7, 7, 0)); // jaune
      matrice.setCursor(10, 12);                       // position texte
      matrice.print("Projet 1");                       // montre le texte
    }

    for (int x = 0; x < 64; x++)
    {
      matrice.drawPixel(x, positionY, matrice.Color333(7, 0, 7)); // magenta
    }

    positionY += direction;
    if (positionY >= 31 || positionY <= 0)
    {
      direction = -direction; // flip la direction
    }

    dernierMouvement = tempsActuel; // update temps
  }
}

void verifierChangementMode()
{                                           // checker le changement de mode
  static bool changementModeTraite = false; // traitement

  if (boutons[2] && boutons[1] && boutons[5]) // Si gauche + bas + B
  {
    if (!changementModeTraite)
    {
      if (modeActuel == MODE_ANIMATION) // Si animation
      {
        modeActuel = MODE_TEST_ENTREES; // change vers mode test
      }
      else
      {
        modeActuel = MODE_ANIMATION; // Change vers l'animation
        matrice.fillScreen(0);       // efface l'écran
      }
      changementModeTraite = true;
    }
  }
  else if (boutons[0] && boutons[1] && boutons[6]) // si haut + bas + C
  {
    if (!changementModeTraite)
    {
      if (modeActuel == MODE_ANIMATION_PERSONAL) // if en animation custom
      {
        modeActuel = MODE_TEST_ENTREES; // test
      }
      else
      {
        modeActuel = MODE_ANIMATION_PERSONAL; // change vers mode animation personal
        matrice.fillScreen(0); // efface l'écran
      }
      changementModeTraite = true;
    }
  }
  else // si rien
  {
    changementModeTraite = false; // réinitialise état
  }
}

void setup()
{
  Serial.begin(9600); // démarre communication série
  setupPins();        // conf les pins

  matrice.begin();            // Initialise la matrice
  matrice.setTextWrap(false); // désactive retour à la ligne

  positionEncodeur = 0;                // position encodeur
  valeurEncodeur = 0;                  // valeur encodeur
  valeurEncodeurPrecedente = 0;        // valeur précédente
  valeurPotentiometrePrecedente = 0;   // valeur potentiomètre
  valeurPhotoresistancePrecedente = 0; // valeur photorésistance
  matriceBesoinUpdate = true;          // update la matrice

  for (int i = 0; i < 7; i++)
  {
    boutons[i] = false;           // réinitialise état bouton
    boutonsPrecedents[i] = false; // reinitialise état précédent
  }

  Serial.println("Systeme demarre"); // montre message
  afficherModeEntree();              // affiche mode entrée
}

void loop()
{
  lireEntrees();            // Lit les entrées
  verifierChangementMode(); // Vérifie changement de mode
  updateLEDS();             // update les LEDs

  if (modeActuel == MODE_TEST_ENTREES)
  {
    afficherModeEntree(); // affiche mode entrée
  }
  else if (modeActuel == MODE_ANIMATION)
  {
    afficherModeAnimation(); // affiche animation
  }
  else if (modeActuel == MODE_ANIMATION_PERSONAL)
  {
    afficherAnimationPersonal(); // affiche animation personal
  }

  delay(5); // attends 5ms
}