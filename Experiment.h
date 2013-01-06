#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "easybmp\EasyBMP.h"
#include "Simulation.h"
#include "GUI.h"

using namespace std;

const int MAX_NUM_TURNS = 10000; // Arbitrary number

class SimStats
{
public:
	TurnStats turnStats[MAX_NUM_TURNS];
	int numTurns;

	SimStats()
	{
		numTurns = 0;
	}

	string toString()
	{
		vector<string> lines;

		for(int turn=0; turn<numTurns; turn++)
		{
			string line;
			string tmp;
			for(int type=0; type<NUM_BIO_TYPES; type++)
			{
				stringstream ss(stringstream::in | stringstream::out);
				ss << turnStats[turn].numBios[type];
				ss >> tmp;
				line += tmp + "\t";
			}
			line += "\n";
			lines.push_back(line);
		}
		
		string list;
		for(int line=0; line<lines.size(); line++)
		{
			list += lines[line];
		}
		return list;
	}
};

class Experiment
{
private:
	int simCount;
	int numTurns;
	int mapSize;

	Simulation* sim;

	bool useBmp;
	RGBApixel pix[4];

	bool useGui;
	GUI* gui;

	SimStats simStats;

	void initPixels();
	void turnGfx(int turn);
	void saveTurnStats(int turn);

public:
	Experiment(int simCount, int numTurns, int mapSize, bool useGui, bool useBmp);
	~Experiment();
	SimStats run();
};

#endif // EXPERIMENT_H