#include "GenerateTrainData.h"

GenerateTrainData::GenerateTrainData(int* dim,double* bound,int oneGroup):dims(dim),bounds(bound),oneGroupNum(oneGroup) {
	traindata = new vector<vector<map<string, double>*>*>(dims[0]*dims[1]*dims[2]);
	for (int i = 0; i < traindata->size(); i++) {
		(*traindata)[i] = new vector<map<string, double>*>();
	}
	coord = new vector<int*>();
}

bool GenerateTrainData::volidateCoord(int x, int y, int z) {
	if (x < dims[0]-1 && y < dims[1]-1 && z < dims[2]-1) {
		if (x >= 0 && y >= 0 && z >= 0) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

void GenerateTrainData::initCoord(int x, int y, int z) {
	coord->clear();
	for (int i = 0; i <= 1; i++) {
		for (int j = 0; j <= 1; j++) {
			for (int k = 0; k <= 1; k++) {
				coord->push_back(new int[3]{ x - 1 + i,y - 1 + j,z - 1 + k });
			}
		}
	}
}

void GenerateTrainData::appendCoord(int x,int y,int z,int a) {
	coord->clear();
	// 上表面 Y = y-1-a平面
	for (int ix = x - 1 - a; ix <= x + a; ix++) {
		for (int iz = z - 1 - a; iz <= z + a; iz++) {
			if (volidateCoord(ix, y - 1 - a, iz)) {
				coord->push_back(new int[3]{ ix,y - 1 - a, iz });
			}
		}
	}
	// 下表面 Y = y+a平面
	for (int ix = x - 1 - a; ix <= x + a; ix++) {
		for (int iz = z - 1 - a; iz <= z + a; iz++) {
			if (volidateCoord(ix, y + a, iz)) {
				coord->push_back(new int[3]{ ix,y + a, iz });
			}
		}
	}
	// 左表面 X = x-1-a平面
	for (int iy = y - 1 - a + 1; iy <= y + a - 1; iy++) {//同时剔除掉和上下表面的重合棱
		for (int iz = z - 1 - a; iz <= z + 1; iz++) {
			if (volidateCoord(x - 1 - a, iy, iz)) {
				coord->push_back(new int[3]{ x - 1 - a,iy, iz });
			}
		}
	}
	// 右表面 X = x+a平面
	for (int iy = y - 1 - a + 1; iy <= y + a - 1; iy++) {//同时剔除掉和上下表面的重合棱
		for (int iz = z - 1 - a; iz <= z + 1; iz++) {
			if (volidateCoord(x + a, iy, iz)) {
				coord->push_back(new int[3]{ x + a,iy, iz });
			}
		}
	}
	// 后表面Z = z-1-a平面
	for (int ix = x - 1 - a+1; ix <= x + a-1; ix++) {//剔除和左右表面的重合棱
		for (int iy = y - 1 - a + 1; iy <= y + a - 1; iy++) {//剔除和上下表面的重合棱
			if (volidateCoord(ix, iy, z - 1 - a)) {
				coord->push_back(new int[3]{ ix,iy, z - 1 - a });
			}
		}
	}
	// 前表面Z = z+a平面
	for (int ix = x - 1 - a + 1; ix <= x + a - 1; ix++) {//剔除和左右表面的重合棱
		for (int iy = y - 1 - a + 1; iy <= y + a - 1; iy++) {//剔除和上下表面的重合棱
			if (volidateCoord(ix, iy, z + a)) {
				coord->push_back(new int[3]{ ix,iy, z + a });
			}
		}
	}
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
				//outfile <<
				//	// i << "\t" <<
				//	x - streamlineP->find("x")->second << " " <<
				//	y - streamlineP->find("y")->second << " " <<
				//	z - streamlineP->find("z")->second << " " <<
				//	streamlineP->find("u")->second << " " <<
				//	streamlineP->find("v")->second << " " <<
				//	streamlineP->find("w")->second << " " <<
				//	u << " " <<
				//	v << " " <<
				//	w <<  endl;
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
			initCoord(nx, ny, nz);
			int appendNum = 0;// 扩散次数
			do {
				GenerateOnePointTrainData(i, meshContainsPoints, x, y, z, u, v, w);
				if ((*traindata)[i]->size() < oneGroupNum) {
					appendNum++;
					appendCoord(nx, ny, nz, appendNum);
				}
			} while ((*traindata)[i]->size() < oneGroupNum);
			appendsum = appendsum + appendNum + 1;
		}
	}
	// outfile << appendsum / 132840 << endl;
	cout << appendsum / (132840 / 4) << endl;
	outfile.close();
}

vector<vector<map<string, double>*>*>* GenerateTrainData::getTrainData() {
	return traindata;
}