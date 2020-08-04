#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <vtkDataSet.h>
#include <vtkStructuredGridReader.h>
#include <vtkStructuredGrid.h>
#include <vtkXMLStructuredGridReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPolyData.h>
#include <assert.h>
#include <string>
using namespace std;

class FileOpenClass
{
public:
	FileOpenClass();
	void OpenFile(string fs);
	void OpenVTK(string Str);
	void OpenPLT(string Str);
	void OpenCAS(string Str);
	void OpenVTP(string Str);
	vtkStructuredGrid* GetGridData();
	vtkPolyData* GetStreamlineData();
	~FileOpenClass();
private:
	vtkStructuredGridReader* sgReader;
	vtkXMLStructuredGridReader* XMLsgReader;
	vtkXMLPolyDataReader* polydReader;
	vtkStructuredGrid* gridDatas;
	vtkPolyData* streamline;
};

