#pragma once

#include <vtkPointLocator.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vector>
#include <map>
#include <string>


using namespace std;
class ValidateData
{
public:
	static void validateTrainData(vector<vector<map<string, double>*>*>* trdata, vtkPolyData* streamlinedats, int GROUP_NUM);
};

