#include "Experiment.h"

Experiment::Experiment(int simCount, int numTurns, int mapSize, bool useGui, bool useBmp)
{
	this->mapSize = mapSize;
	this->numTurns = numTurns;
	this->useBmp = useBmp;
	this->useGui = useGui;
	gui = NULL;

	sim = new Simulation(mapSize);
	sim->init();

	initPixels();
	
	if(useGui)
	{
		gui = new GUI(mapSize, TEXT("nukeSim"));
		gui->init();
	}
}

Experiment::~Experiment()
{
	delete sim;
	if(useGui)
		delete gui;
}

void Experiment::initPixels()
{
	// Below array should not be hard-coded
	pix[0].Red = 41;
	pix[0].Green = 36;
	pix[0].Blue = 1;

	pix[1].Red = 149;
	pix[1].Green = 64;
	pix[1].Blue = 83;

	pix[2].Red = 89;
	pix[2].Green = 188;
	pix[2].Blue = 191;

	pix[3].Red = 230;
	pix[3].Green = 223;
	pix[3].Blue = 102;
}

void Experiment::turnGfx(int turn)
{
	BMP img;
	img.SetSize(mapSize, mapSize);
	for(int x=0; x<mapSize; x++)
	{
		for(int y=0; y<mapSize; y++)
		{
			int type = sim->getTypeAtPos(x, y);
			if(useBmp)
				img.SetPixel(x, y, pix[type+1]);
			if(useGui)
				gui->setChar(x, y, 0xdb, gui->colors[type+1]);
		}
	}

	if(useBmp)
	{
		string fileName = "img\\s";

		char buffer[5];
		itoa(simCount, buffer, 10);
		fileName += buffer;

		fileName += "t";

		itoa(turn, buffer, 10);
		fileName += buffer;

		fileName += ".bmp";
		img.WriteToFile(fileName.c_str());
	}
	if(useGui)
		gui->draw();
}

void Experiment::saveTurnStats(int turn)
{
	simStats.turnStats[turn] = sim->getTurnStats();
	simStats.numTurns++;
}

SimStats Experiment::run()
{
	for(int turn=0; turn<numTurns; turn++)
	{
		sim->turn();
		turnGfx(turn);
		saveTurnStats(turn);
	}

	return simStats;
}