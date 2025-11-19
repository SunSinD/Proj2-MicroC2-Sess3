//Petit code pour lister les broches d'un Arduino Mega2560 et indiquer si elles supportent les interruptions PCINT
//et si oui, à quel groupe PCIE elles appartiennent et quel bit dans le registre PCMSKn correspond à la broche.
#include <Arduino.h>

#define NOMBRE_DE_BROCHES_UNO 20    // D0..D13 + A0..A5
#define NOMBRE_DE_BROCHES_MEGA2560 70 // D0..D53 + A0..A15

void setup() {
  Serial.begin(9600);

  Serial.println(F("Liste des broches et support PCINT sur Arduino Mega2560"));
  Serial.println(F("Format: pinName (num) : PCINT? -> PCIE_group / PCMSK_bit"));

  // Arduino Mega: D0..D53 (0..53), A0..A15 (54..69)
  for (int pin = 0; pin < NOMBRE_DE_BROCHES_MEGA2560; pin++) {
    String name;
    if (pin <= 53) {
      name = "D" + String(pin);
    } else {
      name = "A" + String(pin - 54);
    }

    //Si la broche supporte les interruptions PCINT retourne vrai sinon faux
    if (digitalPinToPCICR(pin)) {
      //Trouve quel groupe PCIE et quel bit dans le registre PCMSKn
      int pcieb = digitalPinToPCICRbit(pin);    // bit PCIE (0..3)
      int pcmskbit = digitalPinToPCMSKbit(pin); // bit dans le masque PCMSKn
      Serial.print(name);
      Serial.print(" (");
      Serial.print(pin);
      Serial.print(") : OUI -> PCIE");
      Serial.print(pcieb);
      Serial.print(" / bit ");
      Serial.println(pcmskbit);
    } else {
      Serial.print(name);
      Serial.print(" (");
      Serial.print(pin);
      Serial.println(") : NON");
    }
    
  }

  Serial.println("Fin de la liste.");

}

void loop() {

}
