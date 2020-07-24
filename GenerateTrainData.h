#pragma once

#include <list>
#include <vector>
#include <map>
#include <iterator>
#include <set>
#include <string>
#include <fstream>
#include <vtkStructuredGrid.h>
#include <vtkPointData.h>

using namespace std;

class GenerateTrainData
{
public:
	GenerateTrainData(int* dim, double* bound,int oneGroup);
	void generateData(vector<list<map<string, double>*>*>* meshContainsPoints,vtkStructuredGrid* oridata);
	vector<vector<map<string, double>*>*>* getTrainData();
private:
	int oneGroupNum;
	ofstream outfile;
	int* dims;
	double* bounds;
	vector<vector<map<string, double>*>*>* traindata;
	vector<int*>* coord;
	void appendCoord(int x,int y,int z,int a);
	void initCoord(int x, int y,int z);
	bool volidateCoord(int x, int y, int z);
	void GenerateOnePointTrainData(int i, vector<list<map<string, double>*>*>* meshContainsPoints, double x, double y, double z, double u, double v, double w);
};

