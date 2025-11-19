#include <Arduino.h>
#include "ClasseCercle.h"

// Fonction pour tester le diamètre et modifier les coordonnées avec un pointeur
void testDiametreEtCoordAvecPointeur(ClasseCercle *unCercle)
{
	Serial.print("Voici le diamètre du cercle: ");
	Serial.println(unCercle->calculDiametre());  
	unCercle->setX(8); // Modifie la coordonnée X du centre du cercle
  unCercle->setY(12); // Modifie la coordonnée Y du centre du cercle
}

void setup() {
  Serial.begin(9600);
}

void loop() {

  // Création d'une instance de ClasseCercle avec un rayon de 5.0
  ClasseCercle cercle(5.0); // Utilisation du constructeur avec paramètre
  ClasseCercle cercle2; // Utilisation du constructeur par défaut
  ClasseCercle test3(7.5);

  // Affichage du centre
  Serial.print("Centre: (");
  Serial.print(cercle.getX());
  Serial.print(", "); 
  Serial.print(cercle.getY());
  Serial.println(")");

  // Affichage du rayon
  Serial.print("Rayon: ");
  Serial.println(cercle.getRayon());

  // Affichage de l'aire
  Serial.print("Aire: ");
  Serial.println(cercle.calculAire());

  // Affichage du périmètre
  Serial.print("Périmètre: ");
  Serial.println(cercle.calculPerimetre());

  // Affichage du diamètre
  Serial.print("Diamètre: ");
  Serial.println(cercle.calculDiametre());

  // test de pointeur
  Serial.println("Test avec pointeur:");
	testDiametreEtCoordAvecPointeur(&test3);
	Serial.print("La coordonnée en X du cercle: ");
	Serial.println(test3.getX());
	Serial.print("La coordonnée en Y du cercle: ");
	Serial.println(test3.getY());
  while(1);  
}
