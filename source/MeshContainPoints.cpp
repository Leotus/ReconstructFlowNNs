#include "MeshContainPoints.h"
MeshContains::MeshContains(int* dim,double* bound) :dims(dim),bounds(bound) {
	meshContainsPoints = new vector<list<map<string, double>*>*>((dims[0] - 1) * (dims[1] - 1) * (dims[2] - 1));
	for (int i = 0; i < meshContainsPoints->size(); i++) {
		(*meshContainsPoints)[i] = new list<map<string, double>*>();
	}
}

void MeshContains::GenerateMeshContainsPoints(vtkPolyData* streamline) {
	int num = streamline->GetNumberOfPoints();
	vtkPointData* ptdat = streamline->GetPointData();
	vtkDataArray* vel = ptdat->GetArray("velocity");
	vtkDataArray* xdata = ptdat->GetArray("x");
	vtkDataArray* ydata = ptdat->GetArray("y");
	vtkDataArray* zdata = ptdat->GetArray("z");
	for (int i = 0; i < num; i++) {
		double xpo = xdata->GetComponent(i,0);
		double ypo = ydata->GetComponent(i, 0);
		double zpo = zdata->GetComponent(i, 0);
		int x = int((xpo-bounds[0]) / ((bounds[1] - bounds[0]) / (double)(dims[0] - 1)));
		int y = int((ypo-bounds[2]) / ((bounds[3] - bounds[2]) / (double)(dims[1] - 1)));
		int z = int((zpo-bounds[4]) / ((bounds[5] - bounds[4]) / (double)(dims[2] - 1)));
		int index = x + y * (dims[0]-1) + z * (dims[0]-1) * (dims[1]-1);
		double u = vel->GetComponent(i, 0);
		double v = vel->GetComponent(i, 1);
		double w = vel->GetComponent(i, 2);
		map<string, double>*n;
		n = new map<string, double>();
		n->insert(pair<string, double>("x", xpo));
		n->insert(pair<string, double>("y", ypo));
		n->insert(pair<string, double>("z", zpo));
		n->insert(pair<string, double>("u", u));
		n->insert(pair<string, double>("v", v));
		n->insert(pair<string, double>("w", w));
		(*meshContainsPoints)[index]->push_back(n);
	}
	ptdat->Delete();
	vel->Delete();
	xdata->Delete();
	ydata->Delete();
	zdata ->Delete();
}

vector<list<map<string, double>*>*>* MeshContains::GetMeshPoints() const {
	return meshContainsPoints;
}
