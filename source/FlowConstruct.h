#pragma once
#include <list>
#include <vector>
#include <map>
#include <iterator>
#include <string>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridWriter.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>
#include <torch/script.h>
#include <torch/torch.h>
#include "CoordTool.h"

using namespace std;

class FlowConstruct
{
public:
	FlowConstruct(int* dim, double* bound, int oneGroup);
	void constructData(vector<list<map<string, double>*>*>* meshContainsPoints, vtkStructuredGrid* oridata);
	vtkStructuredGrid* getFlowData();
	friend class CoordTool;
	~FlowConstruct();
private:
	int oneGroupNum;
	int* dims;
	double* bounds;
	vtkStructuredGrid* flowData;
	vtkStructuredGridWriter* flowWriter;
	vector<double>* testdata;
	vtkDataArray* targetVel;
	vtkPoints* targetPoints;
	vector<int*>* coord;
	torch::jit::script::Module model;
	void GenerateOnePointFlowData(int i, vector<list<map<string, double>*>*>* meshContainsPoints, double x, double y, double z);
};