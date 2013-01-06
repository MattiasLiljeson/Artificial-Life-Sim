#include "Simulation.h"

Simulation::Simulation(int mapSize)
{
	this->mapSize = mapSize;

	bios = new Bio**[NUM_BIO_TYPES];
	for(int type=0; type<NUM_BIO_TYPES; type++)
	{
		bios[type] = new Bio*[mapSize];
		for(int x=0; x<mapSize; x++)
		{
			bios[type][x] = new Bio[mapSize];
		}
	}

	for(int i=0; i<NUM_BIO_TYPES; i++)
		numBios[i] = 0;

	// Set start conditions
	spawnChances[BERRIES]			= 55;
	spawnChances[ELK]				= 30;
	spawnChances[WOLF]				= 15;
	moveRanges[BERRIES]				= 0;
	moveRanges[ELK]					= 2;
	moveRanges[WOLF]				= 2;
	sightRanges[BERRIES]			= 0;
	sightRanges[ELK]				= 10;
	sightRanges[WOLF]				= 5;
	reproductionChances[BERRIES]	= 20;
	reproductionChances[ELK]		= 15;
	reproductionChances[WOLF]		= 25;
	dieOfHunger[BERRIES]			= 0;
	dieOfHunger[ELK]				= 4;
	dieOfHunger[WOLF]				= 2;
}

Simulation::~Simulation()
{
	for(int type=0; type<NUM_BIO_TYPES; type++)
	{
		for(int x=0; x<mapSize; x++)
		{
			delete [] bios[type][x];
		}
		delete [] bios[type];
	}
	delete [] bios;
}

void Simulation::run(int numTurns)
{
	init();
	
	for(int i=0; i<numTurns; i++)
	{
		turn();
	}
}

/*! Returns true if the set number of turns hasn't been run !*/
void Simulation::turn()
{
	initTurn();
	killStarvingBios();
	planMoves();
	move();
	collisionHandling();
	reproduce();
}

TurnStats Simulation::getTurnStats()
{
	TurnStats stats;
	for(int type=0; type<NUM_BIO_TYPES; type++)
	{
		stats.numBios[type] = numBios[type];
	}
	return stats;
}

void Simulation::init()
{
	// Init the map
	for(int i=0; i<NUM_INITIAL_BIOS; i++)
	{
		int ran = rand()%100;
		int type = 0;

		int j = 0;
		bool cont = true;
		while(cont)
		{
			int sum = 0;
			for(int k=0; k<j; k++)
				sum+=spawnChances[k];

			if(sum <= ran && ran < sum+spawnChances[j])
			{
				cont = false;
				type = j;
			}
			j++;
		}
		spawnBio(type);
	}
}

void Simulation::initTurn()
{
	for(int type=0; type<NUM_BIO_TYPES; type++)
	{
		// If the bio can't move it can't move to food or from enemies.
		if(moveRanges[type] > 0)
		{
			for(int x=0; x<mapSize; x++)
			{
				for(int y=0; y<mapSize; y++)
				{
					if(bios[type][x][y].alive == true)
					{
						Bio dbg = bios[type][x][y]; // DEBUG
						bios[type][x][y].hasMoved = false;
					}
				}
			}
		}
	}
}

void Simulation::killStarvingBios()
{
	for(int type=0; type<NUM_BIO_TYPES; type++)
	{
		//#pragma omp parallel for
		for(int x=0; x<mapSize; x++)
		{
			for(int y=0; y<mapSize; y++)
			{
				if(bios[type][x][y].alive)
				{
					if(dieOfHunger[type] != 0)
					{
						if(bios[type][x][y].hunger > dieOfHunger[type])
						{
							killBio(type, x, y);
						}
						bios[type][x][y].hunger++;
					}
				}
			}
		}
	}
}

void Simulation::planMoves()
{
	// Plan movement
	for(int type=0; type<NUM_BIO_TYPES; type++)
	{
		// If the bio can't move it can't move to food or from enemies.
		if(moveRanges[type] > 0)
		{
			//#pragma omp parallel for
			for(int x=0; x<mapSize; x++)
			{
				for(int y=0; y<mapSize; y++)
				{
					if(bios[type][x][y].alive == true)
					{
						Vec tmp = bios[type][x][y].getDir();
						Vec dir = getNewDir(type, x, y);
						dir.setLength(moveRanges[type]);
						bios[type][x][y].setDir(dir);
					}
				}
			}
		}
	}
}

void Simulation::move()
{
	// Can't parallellize in inner loops cause those aren't thread safe
	//#pragma omp parallel for 
	for(int type=0; type<NUM_BIO_TYPES; type++)
	{
		if(moveRanges[type] > 0)
		{
			for(int x=0; x<mapSize; x++)
			{
				for(int y=0; y<mapSize; y++)
				{
					Bio tmp = bios[type][x][y];
					if(tmp.alive == true && tmp.hasMoved == false)
					{
						Vec pos(x,y);

						pos.x += tmp.getDir().x;
						pos.y += tmp.getDir().y;
						/*pos = */fixOutOfBounds(pos);
				
						// Don't move if there's already a a bio at given pos
						Bio dbg = bios[type][pos.x][pos.y]; // DEBUG
						if(bios[type][pos.x][pos.y].alive == false)
						{
							bios[type][x][y].hasMoved = true;
							moveBio(type, x, y, pos.x, pos.y);
						}
					}
				}
			}
		}
	}
}

void Simulation::collisionHandling()
{
	for(int type=0; type<NUM_BIO_TYPES; type++)
	{
		//#pragma omp parallel for
		for(int x=0; x<mapSize; x++)
		{
			for(int y=0; y<mapSize; y++)
			{
				if(bios[type][x][y].alive == true)
				{
					// if not in the bottom of the food chain
					if(type > 0)
					{
						int foodIndex = type - 1; // Who's below me in the food chain?
						// If there's food
						if(bios[foodIndex][x][y].alive == true)
						{
							bios[type][x][y].hunger = 0;
							killBio(foodIndex, x, y);
							// Update contamination levels
						}
					}
				}
			}
		}
	}

	// DEBUG
	//for(int x=0; x<mapSize; x++)
	//{
	//	for(int y=0; y<mapSize; y++)
	//	{
	//		bool found = false;
	//		for(int type=0; type<NUM_BIO_TYPES; type++)
	//		{
	//			if(bios[type][x][y].type != -1)
	//			{
	//				if(found == true)
	//				{
	//					Bio berry = bios[BERRIES][x][y];
	//					Bio elk   = bios[ELK][x][y];
	//					Bio wolf  = bios[WOLF][x][y];
	//					int breakHere = 0;
	//				}
	//				found = true;
	//			}
	//		}
	//	}
	//}
}

void Simulation::reproduce()
{
	// Can't parallellize in inner loops cause those aren't thread safe
	//#pragma omp parallel for
	for(int type=0; type<NUM_BIO_TYPES; type++)
	{
		for(int x=0; x<mapSize; x++)
		{
			for(int y=0; y<mapSize; y++)
			{
				if(bios[type][x][y].alive == true)
				{
					if(rand()%100 < reproductionChances[type])
					{
						int spawnDist = 1;
						Vec pos = getFreeSpotNear(type, Vec(x,y), spawnDist*2);
						if(pos.x != -1)
							spawnBio(type, pos);
					}
				}
			}
		}
	}
}

int Simulation::getTypeAtPos(int x, int y)
{
	int bio = -1;
	for(int type=0; type<NUM_BIO_TYPES; type++)
	{
		if(bios[type][x][y].type != -1)
		{
			bio = type;
		}
	}
	return bio;
}

Vec Simulation::getNewDir(int type, int x, int y)
{
	Vec pos = Vec(x, y);
	Vec dir = bios[type][x][y].getDir();
	Vec enemy(-1, -1);
	Vec food(-1, -1);
	
	// If the bio is in the top of the food chain, it doesn't have any enemies
	if(type<NUM_BIO_TYPES -1)
	{
		enemy = getNearestBio(x, y, type+1, sightRanges[type]);
		if(enemy.x != -1)
		{
			dir.x = enemy.x - pos.x;
			dir.y = enemy.y - pos.y;
			dir.invert(); // Flee, don't go towards
		}
	}

	// Check if in the bottom of food chain, they don't eat
	if(type > 0)
	{
		// If predator nearby, no need to look for food.
		if(enemy.x == -1)
		{
			food = getNearestBio(x, y, type-1, sightRanges[type]);

			// Set direction vector to go towards food
			if(food.x != -1)
			{
				dir.x = food.x - pos.x;
				dir.y = food.y - pos.y;
			}
		}
	}

	return dir;
}

Vec Simulation::getNearestBio(int x, int y, int scanFor, int sightRange)
{
	Vec pos(-1, -1);
	int minDist = INT_MAX;
	int xStart = x-sightRange;
	int yStart = y-sightRange;
	int xStop = x+sightRange + 1; // +1 for the row you're at
	int yStop = y+sightRange + 1; // +1 for the row you're at

	// Iterate through all objects of given type in sightrange, save the nearest
	for(int i=xStart; i<xStop; i++)
	{
		for(int j=yStart; j<yStop; j++)
		{
			if(i != x && j != y)
			{
				// Get in-bounds-corrected bio of position i and j
				Bio bio = scanSpot(scanFor, i, j);
				if(bio.alive == true)
				{
					// Use i and j which can be out of bounds to get the correct distance
					int dist = Vec(x,y).distance(Vec(i,j));
					if(dist < minDist)
					{
						minDist = dist;
						if(dist < sightRange)
						{
							// Create vec which is in bounds
							Vec pos( i, j );
							fixOutOfBounds( pos );
						}
					}
				}
			}
		}
	}

	return pos;
}

bool Simulation::spawnBio(int type)
{
	Vec pos;
	pos = createRandVec(mapSize);
	return spawnBio(type, pos);
}

bool Simulation::spawnBio(int type, Vec pos)
{
	Bio bio;
	bio.alive = true;
	bio.hasMoved = false;
	bio.type = type;
	bio.setDir(Vec(1,1));
	return addBio(bio, pos);
}

bool Simulation::addBio(Bio bio, Vec pos)
{
	return addBio(bio, pos.x, pos.y);
}

bool Simulation::addBio(Bio bio, int x, int y)
{
	bool success = false;
	if(bios[bio.type][x][y].alive != true)
	{
		bios[bio.type][x][y] = bio;
		success = true;
		numBios[bio.type]++;
	}
	return success;
}

bool Simulation::moveBio(int type, int fromx, int fromy, int tox, int toy)
{
	bool success = false;
	if(fromx != tox && fromy != toy)
	{
		Bio* to = &bios[type][tox][toy]; // DEBUG
		Bio* from = &bios[type][fromx][fromy]; // DEBUG

		bios[type][tox][toy] = bios[type][fromx][fromy];
		bios[type][fromx][fromy].reset();

		success = true;
	}
	return success;
}

void Simulation::killBio(int type, int x, int y)
{
	bios[type][x][y].reset();	
	numBios[type]--;
}

Vec Simulation::createVecNear(int maxDist, Vec pos)
{
	return createVecNear(maxDist, pos.x, pos.y);
}
Vec Simulation::createVecNear(int maxDist, int x, int y)
{
	return createRandVec(x - maxDist, x + maxDist, y - maxDist, y + maxDist);
}
Vec Simulation::createRandVec(int max)
{
	return createRandVec(0, max);
}
Vec Simulation::createRandVec(int min, int max)
{
	return createRandVec(0, max, 0, max);
}
Vec Simulation::createRandVec(int minx, int maxx, int miny, int maxy)
{
	Vec vec;
	vec.x = minx + rand()%(maxx-minx);
	vec.y = miny + rand()%(maxy-miny);
	return vec;
}

Vec Simulation::getFreeSpotNear(int type, Vec pos, int range)
{
	int xStart = pos.x - range;
	int yStart = pos.y - range;
	int xStop  = pos.x + range + 1; // +1 for the row you're at
	int yStop  = pos.y + range + 1; // --"--
	Vec tmp(-1, -1);

	Vec* freeSpots = new Vec[(range*2+1)*(range*2+1)]; // +1 for the row you're at
	int numPos = 0;

	for(int x=xStart; x<xStop; x++)
	{
		for(int y=yStart; y<yStop; y++)
		{
			tmp.x = x;
			tmp.y = y;
			fixOutOfBounds( tmp );
			if(bios[type][tmp.x][tmp.y].alive == false)
			{
				freeSpots[numPos] = tmp;
				numPos++;
			}
		}
	}
	int random;
	Vec newSpot;
	if(numPos != 0)
	{
		random = rand()%numPos;
		newSpot = freeSpots[random];
	}
	else
	{
		 newSpot = Vec(-1, -1);
	}

	delete []freeSpots;
	return newSpot;
}

void Simulation::fixOutOfBounds( Vec& pos )
{
	/*return */fixOutOfBounds(pos.x, pos.y);
	//return pos;
}

void Simulation::fixOutOfBounds( int& x, int& y )
{
	x = (x + mapSize) % mapSize;
	y = (y + mapSize) % mapSize;
}

Bio Simulation::scanSpot(int type, int x, int y)
{
	return scanSpot(type, Vec(x,y));
}

Bio Simulation::scanSpot(int type, Vec pos)
{
	Vec inBoundsPos = pos;
	fixOutOfBounds(inBoundsPos);
	Bio bio = bios[type][inBoundsPos.x][inBoundsPos.y];
	return bio;
}