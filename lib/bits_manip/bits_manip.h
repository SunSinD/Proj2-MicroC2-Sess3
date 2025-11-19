#ifndef BITS_MANIP_H
#define BITS_MANIP_H

//**********************************************************
// Programmer par Guillaume Beaulieu.
// Dernière modification: 15 juillet 2025
// Librairie qui permet de gérer les bits de ports
//**********************************************************

int setBit(int octet, unsigned char position);
int setBitM(int octet, int mask);
int clearBit(int octet, unsigned char position);
int clearBitM(int octet, int mask);
int clearAllBit(int octet);
int modifyBit(int octet, unsigned char position, bool newState);
int flipBit(int octet, unsigned char position);
bool isBitSet(int octet, unsigned char position);

#endif