// ReconstructFlowNNs.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include "OpenFiles.h"
#include "MeshContainPoints.h"
#include "GenerateTrainData.h"

int main()
{
    FileOpenClass* openFile = new FileOpenClass();
    string orinDataPath = "..\\data\\GridConfig.txt";
    openFile->OpenFile(orinDataPath);
    vtkStructuredGrid* oridats = openFile->GetGridData(); // 源数据
    string streamDataPath = "..\\data\\StreamConfig.txt";
    openFile->OpenFile(streamDataPath);
    vtkPolyData* streamlinedats = openFile->GetStreamlineData(); // paraview流线数据

    cout << "size of origindata = " << oridats->GetNumberOfPoints() << endl;
    cout << "size of streamdata = " << streamlinedats->GetNumberOfPoints() << endl;
    
    int* dims = oridats->GetDimensions();
    double* bounds = oridats->GetBounds();
    
    MeshContains* meshContains = new MeshContains(dims,bounds);
    meshContains->GenerateMeshContainsPoints(streamlinedats);

    GenerateTrainData* gTraindata = new GenerateTrainData(dims,bounds,10);
    gTraindata->generateData(meshContains->GetMeshPoints(),oridats);
    vector<vector<map<string, double>*>*>* trdata = gTraindata->getTrainData();
}
