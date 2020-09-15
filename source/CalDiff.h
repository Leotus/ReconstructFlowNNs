#pragma once
#include <vtkStructuredGrid.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkPolyData.h>
class CalDiff
{
public:
	double getMSE();
	double getL1Loss();
	double getAveMSE();
	double getAveL1Loss();
	CalDiff(vtkStructuredGrid* oridata, vtkStructuredGrid* newdata);
	CalDiff(vtkPolyData* oridata, vtkStructuredGrid* newdata);
private:
	vtkPolyData* oris;
	vtkStructuredGrid* orid; 
	vtkStructuredGrid* newd;
	double mse;
	double l1loss;
	int numOfPoint;
};

