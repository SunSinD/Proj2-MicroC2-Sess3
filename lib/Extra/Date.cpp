#include <Arduino.h> 

//Inclusion de notre fichier d'entête pour notre classe
#include <Temps.h> //On inclut d'abord la classe mère
#include <Date.h> //Puis la classe fille (héritée)

//Constructeurs par défaut
Date::Date()
{
  iAnnee = 1970;
  iMois = 1;
  iJour = 1;  
}

//Constructeur avec paramètres
Date::Date(int iAnnee, int iMois, int iJour)
{
  this->iAnnee = iAnnee;
  this->iMois = iMois;
  this->iJour = iJour;  
}

//Accesseurs et Mutateurs
void Date::setAnnee(int iAnnee)
{
  this->iAnnee = iAnnee;  
}

int Date::getAnnee()
{
  return iAnnee;
}

void Date::setMois(int iMois)
{
  this->iMois = iMois;
}

int Date::getMois()
{
  return iMois;
}

void Date::setJour(int iJour)
{
  this->iJour = iJour;
}

int Date::getJour()
{
  return iJour;
}

//Méthode qui affiche la date au format aaaa/mm/jj et l'heure au format hh:mm:ss
void Date::affiche()
{
  Serial.print("Date: ");
  
  Serial.print(iAnnee);
  
  Serial.print("/");  
  if(iMois < 10)
    Serial.print("0");
  Serial.print(iMois);
  Serial.print("/");
  if(iJour < 10)
    Serial.print("0");
  Serial.print(iJour);
  
  Serial.println();
  Serial.print("Heure: ");
  
  if(this->getH() < 10)
    Serial.print("0");
  Serial.print(this->getH());
  
  Serial.print(":");  
  if(this->getM() < 10)
    Serial.print("0");
  Serial.print(this->getM());
  Serial.print(":");
  if(this->getS() < 10)
    Serial.print("0");
  Serial.print(this->getS());
}

