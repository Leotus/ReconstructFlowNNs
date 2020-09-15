#include "FlowConstruct.h"


FlowConstruct::FlowConstruct(int* dim, double* bound, int oneGroup) :dims(dim), bounds(bound), oneGroupNum(oneGroup)
{
	flowData = vtkStructuredGrid::New();
	flowData->SetDimensions(dims);
	coord = new vector<int*>();
	flowWriter = vtkStructuredGridWriter::New();
	flowWriter->SetFileName("C:\\Users\\ll\\Desktop\\models\\result_30_fnn.vtp");
	targetPoints = vtkPoints::New();
	targetPoints->Allocate(dims[0] * dims[1] * dims[2]);
	targetVel = vtkDoubleArray::New();
	targetVel->SetNumberOfComponents(3);
	targetVel->SetName("velocity");
	targetVel->SetNumberOfTuples(dims[0] * dims[1] * dims[2]);
	testdata = new vector<double>();
	testdata->clear();
	try {
		model = torch::jit::load("C:\\Users\\ll\\Desktop\\models\\model_30_fnn.pt");  //加载模型
		model.to(at::kCUDA);
	}
	catch (const c10::Error& e) {
		std::cerr << "无法加载模型\n" << e.msg();
	}
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
				testdata->push_back(x - streamlineP->find("x")->second); // dx
				testdata->push_back(y - streamlineP->find("y")->second); // dy
				testdata->push_back(z - streamlineP->find("z")->second); // dz
				testdata->push_back(streamlineP->find("u")->second); // su
				testdata->push_back(streamlineP->find("v")->second); // sv
				testdata->push_back(streamlineP->find("w")->second); // sw
				if (testdata->size()/6 >= oneGroupNum)
					break;
			}
			if (testdata->size()/6 >= oneGroupNum)
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
	vtkDataArray* vel = ptdat->GetArray("velocity");// 速度
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
			targetPoints->InsertPoint(i, x, y, z);
			targetVel->InsertTuple(i, new double[3]{0,0,0});
			cout << i << " - l :" << x << "," << y << "," << z << "\t";
			cout << i << " - v :" << 0 << "," << 0 << "," << 0 << "\t";
			cout << i << " - ov :" << u << "," << v << "," << w << endl;
			continue;
		}
		else if (nx > dims[0] - 3 || ny > dims[1] - 3 || nz > dims[2] - 3) {
			targetPoints->InsertPoint(i, x, y, z);
			targetVel->InsertTuple(i, new double[3]{ 0,0,0 });
			cout << i << " - l :" << x << "," << y << "," << z << "\t";
			cout << i << " - v :" << 0 << "," << 0 << "," << 0 << "\t";
			cout << i << " - ov :" << u << "," << v << "," << w << endl;
			continue;
		}
		else {
			// targetPoints->InsertPoint(i, x, y, z);
			// targetVel->InsertTuple(i, new double[3]{ u,v,w });
			CoordTool::initCoord(nx, ny, nz, coord);
			int appendNum = 0;// 扩散次数
			do {
				GenerateOnePointFlowData(i, meshContainsPoints, x, y, z);
				if (testdata->size()/6 < oneGroupNum) {
					appendNum++;
					CoordTool::appendCoord(nx, ny, nz, appendNum, coord, dims);
				}
			} while (testdata->size()/6 < oneGroupNum);
			// 找足当前xyz所需要的数据，开始预测
			torch::Tensor testTensor = torch::tensor(*testdata).cuda();

			// Create a vector of inputs.
			std::vector<torch::jit::IValue> inputs;
			inputs.push_back(testTensor);

			torch::Tensor result = model.forward(inputs).toTensor();    
			targetVel->InsertTuple(i, new double[3]{ result[0].item().toDouble(),result[1].item().toDouble(),result[2].item().toDouble() });
			targetPoints->InsertPoint(i, x, y, z);

			cout << i << " - l :" << x << ","<< y << "," << z << "\t";
			cout << i << " - v :" << result[0].item().toDouble() << "," << result[1].item().toDouble() <<  "," << result[2].item().toDouble() << "\t";
			cout << i << " - ov :" << u << "," << v << "," << w << endl;
			testdata->clear();
		}
	}
	// 输出vtk文件
	flowData->SetPoints(targetPoints);
	flowData->GetPointData()->SetVectors(targetVel);
	flowWriter->SetInputData(flowData);
	flowWriter->Write();
}