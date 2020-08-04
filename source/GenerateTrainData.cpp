#include "GenerateTrainData.h"

GenerateTrainData::GenerateTrainData(int* dim,double* bound,int oneGroup):dims(dim),bounds(bound),oneGroupNum(oneGroup) {
	traindata = new vector<vector<map<string, double>*>*>(dims[0]*dims[1]*dims[2]);
	for (int i = 0; i < traindata->size(); i++) {
		(*traindata)[i] = new vector<map<string, double>*>();
	}
	coord = new vector<int*>();
}

void GenerateTrainData::GenerateOnePointTrainData(int i,vector<list<map<string, double>*>*>* meshContainsPoints, double x, double y,double z,double u,double v,double w) {
	vector<int*>::iterator it = coord->begin();
	for (; it != coord->end(); ++it) {
		int index = (*it)[0] + (*it)[1] * (dims[0] - 1) + (*it)[2] * (dims[0] - 1) * (dims[1] - 1);
		if (!(*meshContainsPoints)[index]->empty()) {
			list<map<string, double>*>::iterator listit = (*meshContainsPoints)[index]->begin();
			for (; listit != (*meshContainsPoints)[index]->end(); ++listit) {
				map<string, double>* streamlineP = *listit;
				map<string, double>* newtraindata = new map<string, double>();
				newtraindata->insert(pair<string, double>("dx", x - streamlineP->find("x")->second));
				newtraindata->insert(pair<string, double>("dy", y - streamlineP->find("y")->second));
				newtraindata->insert(pair<string, double>("dz", z - streamlineP->find("z")->second));
				newtraindata->insert(pair<string, double>("u", streamlineP->find("u")->second));
				newtraindata->insert(pair<string, double>("v", streamlineP->find("v")->second));
				newtraindata->insert(pair<string, double>("w", streamlineP->find("w")->second));
				newtraindata->insert(pair<string, double>("targetU", u));
				newtraindata->insert(pair<string, double>("targetV", v));
				newtraindata->insert(pair<string, double>("targetW", w));
				newtraindata->insert(pair<string, double>("nx", x));
				newtraindata->insert(pair<string, double>("ny", y));
				newtraindata->insert(pair<string, double>("nz", z));

				(*traindata)[i]->push_back(newtraindata);
				outfile <<
					// i << "\t" <<
					x - streamlineP->find("x")->second << " " <<
					y - streamlineP->find("y")->second << " " <<
					z - streamlineP->find("z")->second << " " <<
					streamlineP->find("u")->second << " " <<
					streamlineP->find("v")->second << " " <<
					streamlineP->find("w")->second << " " <<
					u << " " <<
					v << " " <<
					w <<  endl;
				if ((*traindata)[i]->size() >= oneGroupNum)
					break;
			}
			if ((*traindata)[i]->size() >= oneGroupNum)
				break;
		}
	}
}


void GenerateTrainData::generateData(vector<list<map<string, double>*>*>* meshContainsPoints, vtkStructuredGrid* oridata) {
	int num = oridata->GetNumberOfPoints();
	vtkPointData* ptdat = oridata->GetPointData();
	vtkDataArray* vel = ptdat->GetArray("velocity");// 速度
	vtkDataArray* xdat = ptdat->GetArray("x");// x
	vtkDataArray* ydat = ptdat->GetArray("y");// y
	vtkDataArray* zdat = ptdat->GetArray("z");// z
	outfile.open("C:\\Users\\ll\\Desktop\\data3.txt", ostream::app);
	int appendsum = 0;
	for (int i = 0; i < num; i++) {
		double x = xdat->GetComponent(i, 0);
		double y = ydat->GetComponent(i, 0);
		double z = zdat->GetComponent(i, 0);
		double u = vel->GetComponent(i, 0);
		double v = vel->GetComponent(i, 1);
		double w = vel->GetComponent(i, 2);
		int nx = int((x - bounds[0]) / ((bounds[1] - bounds[0]) / dims[0]));
		int ny = int((y - bounds[2]) / ((bounds[3] - bounds[2]) / dims[1]));
		int nz = int((z - bounds[4]) / ((bounds[5] - bounds[4]) / dims[2]));
		if (nx < 2 || ny < 2 || nz < 2) { // 边界点不考虑暂时
			continue;
		}
		else if (nx > dims[0] - 3 || ny > dims[1] - 3 || nz > dims[2] - 3) {
			continue;
		}
		else {
			CoordTool::initCoord(nx, ny, nz,coord);
			int appendNum = 0;// 扩散次数
			do {
				GenerateOnePointTrainData(i, meshContainsPoints, x, y, z, u, v, w);
				if ((*traindata)[i]->size() < oneGroupNum) {
					appendNum++;
					CoordTool::appendCoord(nx, ny, nz, appendNum,coord,dims);
				}
			} while ((*traindata)[i]->size() < oneGroupNum);
			appendsum = appendsum + appendNum + 1;
		}
	}
	// outfile << appendsum / 132840 << endl;
	cout << appendsum / (132840 ) << endl;
	outfile.close();
}

vector<vector<map<string, double>*>*>* GenerateTrainData::getTrainData() {
	return traindata;
}