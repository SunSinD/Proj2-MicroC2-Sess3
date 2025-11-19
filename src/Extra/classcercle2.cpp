#include "ClasseCercle.h"

// Définition de la constante PI
#define PI 3.14159265358979323846

ClasseCercle::ClasseCercle() {
    fRayon = 1.0;
    iX = 0; // Initialisation du centre X à 0
    iY = 0; // Initialisation du centre Y à 0
}

// Constructeur
ClasseCercle::ClasseCercle(float fRayon) {
    this->fRayon = fRayon;
    iX = 0; // Initialisation du centre X à 0
    iY = 0; // Initialisation du centre Y à 0
}

// Destructeur
ClasseCercle::~ClasseCercle() {
    // Destructeur (vide dans ce cas)
}

// Accesseur du rayon
float ClasseCercle::getRayon() {
    return fRayon;
}

// Accesseur du centre X
int ClasseCercle::getX() {
    return iX;
}

// Accesseur du centre Y
int ClasseCercle::getY() {
    return iY;
}

// Mutateur du rayon
void ClasseCercle::setRayon(float fRayon) {
    this->fRayon = fRayon;
}

// Mutateur du centre X
void ClasseCercle::setX(int iX) {
    this->iX = iX;
}

// Mutateur du centre Y
void ClasseCercle::setY(int iY) {
    this->iY = iY;
}

// Calcul de l'aire
float ClasseCercle::calculAire() {
    return PI * fRayon * fRayon;
}

// Calcul du périmètre
float ClasseCercle::calculPerimetre() {
    return 2.0 * PI * fRayon;
}

// Calcul du diamètre
float ClasseCercle::calculDiametre() {
    return 2.0 * fRayon;
}