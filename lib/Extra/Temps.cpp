#include <Arduino.h>

// Inclusion de notre fichier d'entête pour notre classe
#include "Temps.h"

// Définition des méthodes de la classe Temps
// Constructeurs par défaut
Temps::Temps()
{
    iH = iM = iS = 0;
}

// Constructeur avec paramètres
Temps::Temps(int iH, int iM, int iS)
{
    this->iH = iH;
    this->iM = iM;
    this->iS = iS;
}

// Accesseurs
void Temps::setH(int iH)
{
    this->iH = iH;
}

// Mutateur
int Temps::getH()
{
    return iH;
}

// Accesseurs
void Temps::setM(int iM)
{
    this->iM = iM;
}

// Mutateur
int Temps::getM()
{
    return iM;
}

// Accesseurs
void Temps::setS(int iS)
{
    this->iS = iS;
}

// Mutateur
int Temps::getS()
{
    return iS;
}

// Méthode qui affiche le temps au format hh:mm:ss
void Temps::affiche()
{
    if (iH < 10)
        Serial.print("0");
    Serial.print(iH);

    Serial.print(":");
    if (iM < 10)
        Serial.print("0");
    Serial.print(iM);
    Serial.print(":");
    if (iS < 10)
        Serial.print("0");
    Serial.print(iS);
}

// Surcharge d'opérateur conditionnelle ==
bool operator==(Temps t1, Temps t2)
{
    // Teste si t1.iH == t2.iH etc.

    // Solution qui ne fonctionne pas car iH, iM et iS de chaque classe est privée et donc inaccessible à l'extérieur de la classe.
    /*if (t1.iH == t2.iH && t1.iM == t2.iM && t1.iS == t2.iS)
        return 1;
    else
        return 0;*/

    // Solution #2 = utiliser les accesseurs
    if (t1.getH() == t2.getH() && t1.getM() == t2.getM() && t1.getS() == t2.getS())
        return 1;
    else
        return 0;
}

// Surcharge d'opérateur conditionnelle !=
bool operator!=(Temps t1, Temps t2)
{
    // Ici on utilise l’opérateur == crée précédament.
    return !(t1 == t2);
}

// Surcharge d'opérateur conditionnelle <
bool operator<(Temps t1, Temps t2)
{
    if (t1.getH() < t2.getH()) // Si les heures sont différentes
        return true;
    else if (t1.getH() == t2.getH() && t1.getH() < t2.getM()) // Si elles sont égales, on compare les minutes
        return true;
    else if (t1.getH() == t2.getH() && t1.getM() == t2.getM() && t1.getS() < t2.getS()) // Et si elles sont aussi égales, on compare les secondes
        return true;
    else // Si tout est égal, alors l'objet n'est pas plus petit que b
        return false;
}

// Surcharge d'opérateur arithmétique +
Temps operator+(Temps t1, Temps t2)
{
    Temps resultat;

    // Attention ici aucune validation de débordement n'est faite. Il serait normalement important de
    // valider le débordement dans le cas du temps 12h.
    resultat.setH(t1.getH() + t2.getH());
    resultat.setM(t1.getM() + t2.getM());
    resultat.setS(t1.getS() + t2.getS());

    return resultat;
}

// Surcharge d'opérateur arithmétique raccourci +=
void Temps::operator+=(Temps t1)
{
    iH += t1.getH();
    iM += t1.getM();
    iS += t1.getS();
}
