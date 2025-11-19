// Exemple d'utilisation des interruptions sur une carte Arduino Mega
// Broche 18 (interruption 5) configurée en entrée avec résistance de pull
// up interne et un bouton connecté dessus. Un front descendant sur cette 
// broche déclenche une interruption qui incrémente un compteur. Si le compteur 
// dépasse 4, la LED sur la broche 13 s'allume.

#include <Arduino.h>

#define broche_INTER 18 // Broche 18 correspond à l'interruption 5 sur une carte Arduino Mega
#define no_INTER 5 // Numéro de l'interruption associée à la broche 18

// Compteur de déclenchements de l'interruption ici on le déclare 
// comme "volatile" car il est modifié dans une interruption
volatile int INTER_compteur; 

void setup()
{

  DDRB |= (1 << DDB7); // Met la broche 13 en sortie (équivalent à pinMode(13, OUTPUT);)
  DDRD |= (1 << DDB0); // Met la broche 18 en entrée (équivalent à pinMode(18, INPUT);)
  PORTD |= (1 << PORTD0); // Active la résistance de pull-up interne (équivalent à pinMode(18, INPUT_PULLUP);)
  
  // Ici on attache la fonction d'interruption "interrupt" à l'interruption 5
  // qui est déclenchée par un front descendant (FALLING)
  // On pourrait aussi utiliser RISING ou CHANGE
  attachInterrupt(no_INTER, interrupt, FALLING);
  
  // Ici on pourrait aussi utiliser la macro digitalPinToInterrupt(pin)
  // qui retourne le numéro d'interruption associé à la broche
  // attachInterrupt(digitalPinToInterrupt(broche_INTER), interrupt, CHANGE);
  
  // Pour désactiver une interruption, ici on ne le fait pas
  // car on veut l'utiliser tout le temps.
  // detachInterrupt(no_INTER);

  Serial.begin(9600);
}

void loop()
{
  // On allume la LED sur la broche 13 si le compteur d'interruptions
  // est supérieur à 4
  if (INTER_compteur > 4)
  {
    digitalWrite(13, HIGH);
  }
}
/**********************************************************************************************************/
// Fonction d'interruption
/**********************************************************************************************************/
void interrupt()
{
  // Attention, ne pas utiliser de Serial.print dans une interruption
  // car cela utilise des interruptions internes et peut bloquer le programme
  // De plus, l'utilisation de serial dans une interruption est lente
  // et peut rater des impulsions si elles sont trop rapprochées.
  // Ici nous l'utilisons pour montrer quand l'interruption est déclenchée
  // mais ce n'est pas une bonne pratique dans un programme réel.
  Serial.println("changement"); 

  INTER_compteur++; // On incrémente le compteur d'interruptions

  Serial.println(INTER_compteur); // Attention!
}
