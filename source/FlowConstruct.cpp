#include "FlowConstruct.h"


FlowConstruct::FlowConstruct(int* dim, double* bound, int oneGroup) :dims(dim), bounds(bound), oneGroupNum(oneGroup)
{
	flowData = vtkStructuredGrid::New();
	flowData->SetDimensions(dims);
	coord = new vector<int*>();
	flowWriter = vtkStructuredGridWriter::New();
	targetPoints = vtkPoints::New();
	targetPoints->Allocate(dims[0] * dims[1] * dims[2]);
	targetVel = vtkDoubleArray::New();
	targetVel->SetNumberOfComponents(3);
	targetVel->SetName("velocity");
	targetVel->SetNumberOfTuples(dims[0] * dims[1] * dims[2]);
	testdata = new vector<double*>();
	testdata->clear();
}


	
vtkStructuredGrid* FlowConstruct::getFlowData() {
	return flowData;
}


	
void FlowConstruct::GenerateOnePointFlowData(int i, vector<list<map<string, double>*>*>* meshContainsPoints, double x, double y, double z)
{
	vector<int*>::iterator it = coord->begin();
	for (; it != coord->end(); ++it) {
		int index = (*it)[0] + (*it)[1] * (dims[0] - 1) + (*it)[2] * (dims[0] - 1) * (dims[1] - 1);
		if (!(*meshContainsPoints)[index]->empty()) {
			list<map<string, double>*>::iterator listit = (*meshContainsPoints)[index]->begin();
			for (; listit != (*meshContainsPoints)[index]->end(); ++listit) {
				map<string, double>* streamlineP = *listit;
				map<string, double>* newtraindata = new map<string, double>();
				double* newtestdata = new double[6];
				newtestdata[0] = x - streamlineP->find("x")->second; // dx
				newtestdata[1] = y - streamlineP->find("y")->second; // dy
				newtestdata[2] = z - streamlineP->find("z")->second; // dz
				newtestdata[3] = streamlineP->find("u")->second; // su
				newtestdata[4] = streamlineP->find("v")->second; // sv
				newtestdata[5] = streamlineP->find("w")->second; // sw
				testdata->push_back(newtestdata);
				if (testdata->size() >= oneGroupNum)
					break;
			}
			if (testdata->size() >= oneGroupNum)
				break;
		}
	}
}


void FlowConstruct::constructData(vector<list<map<string, double>*>*>* meshContainsPoints, vtkStructuredGrid* oridata)
{
	int num = oridata->GetNumberOfPoints();
	vtkPointData* ptdat = oridata->GetPointData();
	vtkDataArray* xdat = ptdat->GetArray("x");// x
	vtkDataArray* ydat = ptdat->GetArray("y");// y
	vtkDataArray* zdat = ptdat->GetArray("z");// z
	for (int i = 0; i < num; i++) {
		double x = xdat->GetComponent(i, 0);
		double y = ydat->GetComponent(i, 0);
		double z = zdat->GetComponent(i, 0);
		int nx = int((x - bounds[0]) / ((bounds[1] - bounds[0]) / dims[0]));
		int ny = int((y - bounds[2]) / ((bounds[3] - bounds[2]) / dims[1]));
		int nz = int((z - bounds[4]) / ((bounds[5] - bounds[4]) / dims[2]));
		if (nx < 2 || ny < 2 || nz < 2) { // 边界点不考虑暂时
			targetPoints->InsertPoint(i, x, y, z);
			targetVel->InsertTuple(i, new double[3]{0});
			continue;
		}
		else if (nx > dims[0] - 3 || ny > dims[1] - 3 || nz > dims[2] - 3) {
			continue;
			targetPoints->InsertPoint(i, x, y, z);
			targetVel->InsertTuple(i, new double[3]{ 0 });
		}
		else {
			CoordTool::initCoord(nx, ny, nz, coord);
			int appendNum = 0;// 扩散次数
			do {
				GenerateOnePointFlowData(i, meshContainsPoints, x, y, z);
				if (testdata->size() < oneGroupNum) {
					appendNum++;
					CoordTool::appendCoord(nx, ny, nz, appendNum, coord, dims);
				}
			} while (testdata->size() < oneGroupNum);
			// todo :: 找足当前xyz所需要的数据，开始预测
			//
			//
			//

		}
	}
	// todo :: 输出vtk文件
	//
}