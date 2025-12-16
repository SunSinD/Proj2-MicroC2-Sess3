#include <Arduino.h>
#include <bits_manip.h>

#define ENC_CHA_ARD 3 // pin de l'encodeur channel A Arduino
#define ENC_CHA PE5   // pin de l'encodeur channel A
#define ENC_CHB_ARD 5 // pin de l'encodeur channel B Arduino
#define ENC_CHB PE3   // pin de l'encodeur channel B

volatile long encoderVal = 0; // valeur de l'encodeur

void ISR_encodeur()
{
  bool valeurCHB = isBitSet(PINE, ENC_CHB); // lire l'état du channel B

  // Déterminer le sens de rotation
  if (!valeurCHB)
  {
    encoderVal++;
    //Attention à ne pas mettre de Serial dans une ISR en pratique
    //on peut le faire avec parcimonie pour du debug
    //Serial.println("Sens horaire"); 
  }
  else
  {
    encoderVal--;
    //Serial.println("Sens anti-horaire");
  }
}

void setup() {
  Serial.begin(9600);
  DDRE &= ~(1 << ENC_CHA); // configure ENC_CHA comme entrée
  DDRE &= ~(1 << ENC_CHB); // configure ENC_CHB comme entrée

  // Ici on attache une interruption sur le channel A de l'encodeur
  // L'interruption se déclenche sur le front montant (RISING)
  // ici on utilise le numéro d'interruption Arduino correspondant à la pin 3
  // soit l'interruption 1.
  attachInterrupt(1, ISR_encodeur, RISING);
}

void loop() {
  // Le code principal pourrait être vide car l'encodeur est géré par l'interruption
  // mais on peut aussi afficher la valeur de l'encodeur périodiquement
  delay(1000);
  Serial.print("Valeur de l'encodeur: ");
  Serial.println(encoderVal);  
}
