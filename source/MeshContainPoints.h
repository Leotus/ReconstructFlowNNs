#pragma once
#include <vector>
#include <list>
#include <map>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <string>
using namespace std;
class MeshContains
{
public:
	MeshContains(int* dim,double* bound);
	void GenerateMeshContainsPoints(vtkPolyData* streamline);
	vector<list<map<string,double>*>*>* GetMeshPoints() const;
	~MeshContains()
	{
		delete meshContainsPoints;
		delete dims;
		delete bounds;
	}
private:
	vector<list<map<string, double>*>*>* meshContainsPoints;
	int* dims;
	double* bounds;// xmin,xmax,ymin,ymax,zmin,zmax
};

