#include "CalDiff.h"

void CalDiff::getInterV(double& u, double& v, double& w, double x, double y, double z)
{
    int* dims = newd->GetDimensions();
    double* bounds = newd->GetBounds();
    vtkPointData* ptdat = newd->GetPointData();
    vtkDataArray* vel = ptdat->GetArray("velocity");//速度
    int x_left = int((x - bounds[0]) / ((bounds[1] - bounds[0]) / (double)(dims[0] - 1)));
    int x_right = x_left + 1;
    int y_left = int((y - bounds[2]) / ((bounds[3] - bounds[2]) / (double)(dims[1] - 1)));
    int y_right = y_left + 1;
    int z_left = int((z - bounds[4]) / ((bounds[5] - bounds[4]) / (double)(dims[2] - 1)));
    int z_right = z_left + 1;
    int index_xl = x_left + y_left * (dims[0] - 1) + z_left * (dims[0] - 1) * (dims[1] - 1);
    int index_xr = x_right + y_left * (dims[0] - 1) + z_left * (dims[0] - 1) * (dims[1] - 1);
    int index_yl = x_left + y_left * (dims[0] - 1) + z_left * (dims[0] - 1) * (dims[1] - 1);
    int index_yr = x_left + y_right * (dims[0] - 1) + z_left * (dims[0] - 1) * (dims[1] - 1);
    int index_zl = x_left + y_left * (dims[0] - 1) + z_left * (dims[0] - 1) * (dims[1] - 1);
    int index_zr = x_left + y_left * (dims[0] - 1) + z_right * (dims[0] - 1) * (dims[1] - 1);
    double scalex = (x - x_left * ((bounds[1] - bounds[0]) / (double)(dims[0] - 1))) / ((bounds[1] - bounds[0]) / (double)(dims[0] - 1));
    double scaley = (y - y_left * ((bounds[3] - bounds[2]) / (double)(dims[1] - 1))) / ((bounds[3] - bounds[2]) / (double)(dims[1] - 1));
    double scalez = (z - z_left * ((bounds[5] - bounds[4]) / (double)(dims[2] - 1))) / ((bounds[5] - bounds[4]) / (double)(dims[2] - 1));
    u = (vel->GetComponent(index_xr, 0) - vel->GetComponent(index_xl, 0)) * scalex + vel->GetComponent(index_xl, 0);
    v = (vel->GetComponent(index_yr, 0) - vel->GetComponent(index_yl, 0)) * scaley + vel->GetComponent(index_yl, 0);
    w = (vel->GetComponent(index_zr, 0) - vel->GetComponent(index_zl, 0)) * scalez + vel->GetComponent(index_zl, 0);
}


// 分布差异1
CalDiff::CalDiff(vtkPolyData* oridata, vtkStructuredGrid* newdata) :oris(oridata), newd(newdata)
{
	cout << "numbers of lines : " << oris->GetNumberOfLines() << endl;

    vtkPointData* ptdat = oris->GetPointData();
    vtkDataArray* xdat = ptdat->GetArray("x");// x
    vtkDataArray* ydat = ptdat->GetArray("y");// y
    vtkDataArray* zdat = ptdat->GetArray("z");// z
    vtkDataArray* vel = ptdat->GetArray("velocity");//速度

    // 找到原来的第一条流线
    vtkCellArray* sline = oris->GetLines();
    
    vtkIdList* list1 = vtkIdList::New(); // 源流线id list
    sline->GetNextCell(list1);

    vtkPolyData* newpoly = vtkPolyData::New(); // 要输出的polydata

    vtkCellArray* lines = vtkCellArray::New(); // 源流线和推导流线

    vtkPoints* newp = vtkPoints::New(); // 新生成的点集
    vtkPolyLine* line1 = vtkPolyLine::New(); // 源流线

    int numberOfPoints = list1->GetNumberOfIds() * 2;
    // numberOfPoints = list1->GetNumberOfIds();
    newp->SetNumberOfPoints(numberOfPoints);

    vtkPolyLine* line2 = vtkPolyLine::New();
    vtkIdList* list2 = line2->GetPointIds();

    int index = 0;
    for (; index < list1->GetNumberOfIds(); index++) {
        newp->SetPoint(
            list1->GetId(index),
            xdat->GetComponent(list1->GetId(index), 0),
            ydat->GetComponent(list1->GetId(index), 0),
            zdat->GetComponent(list1->GetId(index), 0)
        );
    }
    index = 1;
    for (; index < list1->GetNumberOfIds(); index++) {
        double p1x = xdat->GetComponent(list1->GetId(index-1), 0);
        double p1y = ydat->GetComponent(list1->GetId(index - 1), 0);
        double p1z = zdat->GetComponent(list1->GetId(index - 1), 0);
        double u = vel->GetComponent(list1->GetId(index - 1), 0);
        double v = vel->GetComponent(list1->GetId(index - 1), 1);
        double w = vel->GetComponent(list1->GetId(index - 1), 2);
        double p2x = xdat->GetComponent(list1->GetId(index), 0);
        double p2y = ydat->GetComponent(list1->GetId(index), 0);
        double p2z = zdat->GetComponent(list1->GetId(index), 0);
        double t = ((p2x - p1x) * (p2x - p1x) + (p2y - p1y) * (p2y - p1y) + (p2z - p1z) * (p2z - p1z)) / (u * u + v * v + w * w);
        t = sqrt(t);
        double ppu = 0;
        double ppv = 0;
        double ppw = 0;
        getInterV(ppu, ppv, ppw, p1x, p1y, p1z);
        cout << "t:" << t << endl;
        cout << "u:" << ppu << " v:" << ppv << " w:" << ppw << endl;
        double ppx = p1x + ppu * t;
        double ppy = p1y + ppv * t;
        double ppz = p1z + ppw * t;
        cout << "ppx:" << ppx << " ppy:" << ppy << " ppz:" << ppz << endl;
        newp->SetPoint(list1->GetNumberOfIds() + list1->GetId(index), ppx, ppy, ppz);
        list2->InsertNextId(list1->GetNumberOfIds() + list1->GetId(index));
    }

    lines->InsertNextCell(list1);
    lines->InsertNextCell(list2);

    newpoly->SetLines(lines);
    newpoly->SetPoints(newp);

    vtkXMLPolyDataWriter* polyWriter = vtkXMLPolyDataWriter::New();
    polyWriter->SetFileName("C:\\Users\\ll\\Desktop\\models\\test.vtp");

    polyWriter->SetInputData(newpoly);
    polyWriter->Write();

    polyWriter->Delete();
    
}

// 数值差异
CalDiff::CalDiff(vtkStructuredGrid* oridata, vtkStructuredGrid* newdata) :orid(oridata), newd(newdata) 
{
	if (orid->GetNumberOfPoints() != newd->GetNumberOfPoints())
	{
		std::cerr << "前后流场大小不一致" << endl;
	}
	else {
		numOfPoint = orid->GetNumberOfPoints();
		vtkPointData* ptodat = orid->GetPointData();
		vtkDataArray* orivel = ptodat->GetArray("velocity");// 速度
		vtkPointData* ptnewdat = newd->GetPointData();
		vtkDataArray* newvel = ptnewdat->GetArray("velocity");// 速度
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