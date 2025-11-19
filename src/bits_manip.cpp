//**********************************************************
// Programmer par Guillaume Beaulieu.
// Dernière modification: 15 juillet 2025
// Librairie qui permet de gérer les bits de ports
//**********************************************************

//**********************************************************
// setBit permet de mettre à 1 le bit contenu dans octet à la
// position spécifiée.
// Paramètres: octet = entier pour lequel on veut mettre un 
//                     bit à 1.
//             position = la position du bit à changer.
// Retour: retourne l'entier avec le bit changé.
//**********************************************************
int setBit(int octet, unsigned char position)
{
	int mask = 1 << position; //Création du masque
	return octet | mask; 
}

//**********************************************************
// setBitM permet de mettre à 1 les bits contenu dans mask.
// Paramètres: octet = entier pour lequel on veut mettre un 
//                     bit à 1.
//             mask = le masque à utiliser.
// Retour: retourne l'entier avec les bits changés.
//**********************************************************
int setBitM(int octet, int mask)
{
	return octet | mask;
}

//**********************************************************
// clearBit permet de mettre à 0 le bit contenu dans octet à
// la position spécifiée.
// Paramètres: octet = entier pour lequel on veut mettre un 
//                     bit à 0.
//             position = la position du bit à changer.
// Retour: retourne l'entier avec le bit changé.
//**********************************************************
int clearBit(int octet, unsigned char position)
{
	int mask = 1 << position; //Création du masque
	//On inverse le masque avec ~ donc 0001 devient 1110 par exemple
	return octet & ~mask; 
}

//**********************************************************
// clearBitM permet de mettre à 0 les bits contenus dans mask
// à la position spécifiée.
// Paramètres: octet = entier pour lequel on veut mettre un 
//                     bit à 1.
//             mask = le masque à utiliser.
// Retour: retourne l'entier avec les bits changés.
//**********************************************************
int clearBitM(int octet, int mask)
{
	return octet & ~mask;
}

//**********************************************************
// clearAllBit permet de mettre à 0 tous les bits contenus
// dans octet.
// Paramètres: octet = entier pour lequel on veut mettre tous
//                     les bits à 0.
// Retour: retourne l'entier avec les bits changés.
//**********************************************************
int clearAllBit(int octet)
{
	int mask = 0xff;
	return octet & ~mask;
}

//**********************************************************
// modifyBit permet de mettre à 1 ou 0 le bit contenu dans x 
// à la position spécifiée.
// Paramètres: octet = entier pour lequel on veut mettre un bit
//                 à 1.
//             position = la position du bit à changer.
//			   newState = nouvel état du bit soit 1 ou 0
// Retour: retourne l'entier avec le bit changé.
//**********************************************************
int modifyBit(int octet, unsigned char position, bool newState)
{
	int mask = 1 << position; //Création du masque
	int state = int(newState);
	return (octet&~mask) | (~state&mask);
}

//**********************************************************
// flipBit permet d'inverser le bit contenu dans octet à la
// position spécifiée.
// Paramètres: octet = entier pour lequel on veut mettre un 
//                     bit à 1.
//             position = la position du bit à inverser.
// Retour: retourne l'entier avec le bit changé.
//**********************************************************
int flipBit(int octet, unsigned char position)
{
	int mask = 1 << position; //Création du masque
	return octet ^ mask;
}

//**********************************************************
// isBitSet permet de déterminer si le bit spécifié est à 0
// ou 1. 
// Paramètres: octet = entier pour lequel on veut mettre un 
//                     bit à 1.
//             position = la position du bit à changer.
// Retour: Retourne 1 si le bit est à 1 ou 0 s'il est à zéro.
//**********************************************************
bool isBitSet(int octet, unsigned char position)
{
	octet >>= position; //On isole le bits voulu.
	return (octet&1) != 0;
}