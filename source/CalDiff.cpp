#include "CalDiff.h"

// �ֲ�����
CalDiff::CalDiff(vtkPolyData* oridata, vtkStructuredGrid* newdata) :oris(oridata), newd(newdata)
{
	cout << "numbers of lines : " << oris->GetNumberOfLines() << endl;
}

// ��ֵ����
CalDiff::CalDiff(vtkStructuredGrid* oridata, vtkStructuredGrid* newdata) :orid(oridata), newd(newdata) 
{
	if (orid->GetNumberOfPoints() != newd->GetNumberOfPoints())
	{
		std::cerr << "ǰ��������С��һ��" << endl;
	}
	else {
		numOfPoint = orid->GetNumberOfPoints();
		vtkPointData* ptodat = orid->GetPointData();
		vtkDataArray* orivel = ptodat->GetArray("velocity");// �ٶ�
		vtkPointData* ptnewdat = newd->GetPointData();
		vtkDataArray* newvel = ptnewdat->GetArray("velocity");// �ٶ�
		double ou,ov,ow,nu,nv,nw;
		
	}
}

double CalDiff::getMSE()
{
	return mse;
}

double CalDiff::getAveMSE()
{
	return mse / numOfPoint ;
}

double CalDiff::getL1Loss()
{
	return l1loss;
}

double CalDiff::getAveL1Loss()
{
	return l1loss / numOfPoint;
}