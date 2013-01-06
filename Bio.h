// Class used to store data about the animals
#ifndef BIO_H
#define BIO_H

#include "Vec.h"

class Bio
{
private:
	Vec dir;
public:
	int type;
	bool hasMoved;
	bool alive;
	int contaminationLevel;
	int hunger;

	Bio(){hasMoved = false; alive = false; type = -1; contaminationLevel = 0; hunger = 0;}
	void reset(){hasMoved = false; alive = false; type = -1; contaminationLevel = 0; hunger = 0;}
	Vec getDir(){return dir;}
	void setDir(Vec dir){this->dir = dir;}
};

#endif