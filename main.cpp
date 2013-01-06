#include <iostream>
#include <fstream>
#include <sstream>
#include "Experiment.h"

using namespace std;

void initPixels(RGBApixel pixels[]);
void turnGfx(int turnCount, int mapSize, RGBApixel pix[], Simulation* sim, GUI* gui); // Use ptrs to save speed

int main()
{
	// Check for memory leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	srand(time((unsigned int)NULL));

	int numExperiments = 8;
	int numTurns = 1000;
	int numBioTypes = 3;

	ofstream metaData("data\\metaData.dat");
	metaData << numExperiments <<"\t";
	metaData << numTurns <<"\t";
	metaData << numBioTypes <<"\t";


	Experiment exp(0, numTurns, 128, true, false);
	SimStats stats = exp.run();

//#pragma omp parallel for
//	for(int i=0; i<numExperiments; i++)
//	{
//		stringstream ss(stringstream::in | stringstream::out);
//		string fName = "data\\";
//		ss << i;
//		fName += ss.str();
//		fName += ".dat";
//		ofstream out(fName);
//		if(out.good())
//		{
//			Experiment exp(i, numTurns, 128, false, false);
//			SimStats stats = exp.run();
//
//			out << stats.toString();
//			out.close();
//		}
//		else
//		{
//			cout<<"\nWriting to file failed: "<<fName<<endl;
//		}
//
//		cout << i << endl;
//	}


	//cout<<"\nExperiments are done and files are written";
	cin.get();
	return 0;
}