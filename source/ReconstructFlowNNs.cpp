// ReconstructFlowNNs.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "OpenFiles.h"
#include "MeshContainPoints.h"
#include "GenerateTrainData.h"
#include "ValidateData.h"

int GROUP_NUM = 30; // 每组数目 config

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

    /* 通过流线生成训练数据 */
    //GenerateTrainData* gTraindata = new GenerateTrainData(dims,bounds, GROUP_NUM);
    //gTraindata->generateData(meshContains->GetMeshPoints(),oridats);
    //vector<vector<map<string, double>*>*>* trdata = gTraindata->getTrainData();
    //ValidateData::validateTrainData(trdata, streamlinedats,GROUP_NUM); // 验证训练数据
    
    /* todo :: 通过训练好的模型预测流场 */
    //
    //
    //
}