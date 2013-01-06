#ifndef SIMULATION_H
#define SIMULATION_H

#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <string>
#include "Bio.h"
#include "Vec.h"

//DEBUG INCLUDES
#include <fstream>
#include <iostream>

using namespace std;

enum BioTypes{BERRIES, ELK, WOLF};
static const int NUM_BIO_TYPES = 3;
static const int NUM_INITIAL_BIOS = 1024;

struct TurnStats
{
	int numBios[3];
};

class Simulation
{
private:
	int mapSize;

	Bio*** bios;
	int numBios[NUM_BIO_TYPES];
	int spawnChances[NUM_BIO_TYPES];
	int moveRanges[NUM_BIO_TYPES];
	int sightRanges[NUM_BIO_TYPES];
	int reproductionChances[NUM_BIO_TYPES];
	int dieOfHunger[NUM_BIO_TYPES];

	void initTurn();
	void killStarvingBios();
	void planMoves();
	void move();
	void collisionHandling();
	void reproduce();

	Vec getNewDir(int type, int x, int y);
	Vec getNearestBio(int x, int y, int scanFor, int sightRange);

	bool spawnBio(int type);
	bool spawnBio(int type, Vec pos);
	bool addBio(Bio bio, Vec pos);
	bool addBio(Bio bio, int x, int y);
	bool moveBio(int type, int fromx, int fromy, int tox, int toy);
	void killBio(int type, int x, int y);

	Vec createVecNear(int maxDist, Vec pos);
	Vec createVecNear(int maxDist, int x, int y);
	Vec createRandVec(int max);
	Vec createRandVec(int min, int max);
	Vec createRandVec(int minx, int maxx, int miny, int maxy);
	Vec getFreeSpotNear(int type, Vec pos, int radius);

	void fixOutOfBounds( Vec& pos );
	void fixOutOfBounds( int& x, int& y );

	Bio scanSpot(int type, Vec pos);
	Bio scanSpot(int type, int x, int y);

public:
	Simulation(int mapSize);
	~Simulation();

	void init();
	void run(int numTurns);
	void turn();
	TurnStats getTurnStats();
	int getTypeAtPos(int x, int y);

};

#endif //SIMULATION_H