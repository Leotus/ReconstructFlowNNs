#pragma once
#include <vector>
using namespace std;

class CoordTool
{
public:
	static void appendCoord(int x, int y, int z, int a, vector<int*>* coord,int dims[3]);
	static void initCoord(int x, int y, int z, vector<int*>* coord);
	static bool volidateCoord(int x, int y, int z,int dims[3]);
};

