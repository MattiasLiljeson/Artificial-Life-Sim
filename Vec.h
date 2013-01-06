#ifndef VEC_H
#define VEC_H

#include <cmath>

using namespace std;

class Vec
{
public:
	int x;
	int y;
	Vec(){x = 11; y = 11;}
	Vec(int x, int y){this->x = x; this->y = y;}

	int distance(Vec other)
	{
		int distx = x - other.x;
		int disty = y - other.y;

		if(distx < 0)
			distx = -1*distx;
		if(disty < 0)
			disty = -1*disty;
		
		// Manhattan distance, cheap, but not the true distance
		int dist = distx + disty;
		return dist;
	}

	void setLength(int length)
	{
		//// Crude Manhattan distance used
		//int halfLength = length/2; 
		//if(x > halfLength)
		//	x = halfLength;
		//if(y > halfLength)
		//	y = halfLength;*/

		float angle = atan2((float)y,(float)x);
		x = cos(angle)*(float)length;
		y = sin(angle)*(float)length;

	}

	void invert()
	{
		x *= -1;
		y *= -1;
	}

	bool operator==(Vec rhs)
	{
		if(rhs.x == x && rhs.y == y)
			return true;
		else
			return false;
	}
};

#endif //VECTOR_H