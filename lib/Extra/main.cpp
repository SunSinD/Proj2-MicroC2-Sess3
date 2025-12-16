#include <Arduino.h>
#include <Temps.h>
#include <Date.h>

void setup() {
  Serial.begin(9600);
}

void loop() {
  //Test de la classe Temps
  Temps t1(12, 30, 45);
  Temps t2(1, 15, 30);
  Temps t3;
  t3 = t1 + t2; // Utilisation de l'opérateur +
  t3.affiche();
  Serial.println();
  t1 += t2; // Utilisation de l'opérateur +=
  t1.affiche();
  Serial.println();
  if (t1 == t3) // Utilisation de l'opérateur ==
    Serial.println("t1 est égal à t3");
  if (t1 != t2) // Utilisation de l'opérateur !=
    Serial.println("t1 est différent de t2");
  if (t2 < t1) // Utilisation de l'opérateur <
    Serial.println("t2 est inférieur à t1");


  //Test de la classe Date
  Date d1(2024, 6, 15);
  d1.affiche();
  Serial.println();
  Date d2;
  d2.affiche();
  Serial.println();
  while(1);
}
