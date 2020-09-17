#pragma once
#include <vtkStructuredGrid.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkPolyData.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPolyLine.h>
#include <vtkCellArray.h>
class CalDiff
{
public:
	double getMSE();
	double getL1Loss();
	double getAveMSE();
	double getAveL1Loss();
	CalDiff(vtkStructuredGrid* oridata, vtkStructuredGrid* newdata);
	CalDiff(vtkPolyData* oridata, vtkStructuredGrid* newdata);
	CalDiff() = default;
private:
	void getInterV(double& u, double& v, double& w, double x, double y, double z);
	vtkPolyData* oris;
	vtkStructuredGrid* orid; 
	vtkStructuredGrid* newd;
	double mse = 0;
	double l1loss = 0;
	int numOfPoint = 0;
};

