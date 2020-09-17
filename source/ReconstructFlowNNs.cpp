// ReconstructFlowNNs.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include <iostream>

#include "OpenFiles.h"
#include "MeshContainPoints.h"
#include "GenerateTrainData.h"
#include "ValidateData.h"
#include "FlowConstruct.h"
#include "CalDiff.h"




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

    cout << "size of origindata = " << oridats -> GetNumberOfPoints() << endl;
    cout << "size of streamdata = " << streamlinedats -> GetNumberOfPoints() << endl;
    cout << "numbers of lines : " << streamlinedats -> GetNumberOfLines() << endl;

    int* dims = oridats->GetDimensions();
    double* bounds = oridats->GetBounds();
    
    // MeshContains* meshContains = new MeshContains(dims,bounds);
    // meshContains->GenerateMeshContainsPoints(streamlinedats);

    /* 通过流线生成训练数据 */
    //GenerateTrainData* gTraindata = new GenerateTrainData(dims,bounds, GROUP_NUM);
    //gTraindata->generateData(meshContains->GetMeshPoints(),oridats);
    //vector<vector<map<string, double>*>*>* trdata = gTraindata->getTrainData();
    //ValidateData::validateTrainData(trdata, streamlinedats,GROUP_NUM); // 验证训练数据
    
    /*  通过训练好的模型预测流场 */
    // FlowConstruct* flowConstruct = new FlowConstruct(dims, bounds, GROUP_NUM);
    // flowConstruct->constructData(meshContains->GetMeshPoints(), oridats);
    // vtkStructuredGrid* newFlowData = flowConstruct->getFlowData();

    /* todo : 对比流线及新流场数据，检验结果 */

    FileOpenClass* openFile2 = new FileOpenClass();
    string newDataPath = "..\\data\\NewGridConfig.txt";
    openFile2->OpenFile(newDataPath);
    vtkStructuredGrid* newFlowData = openFile->GetGridData(); // 得到保存的新流场数据

    CalDiff* disDiff = new CalDiff(streamlinedats,newFlowData);


    //  清理工作
    delete openFile;
    delete openFile2;
    // delete meshContains;
    // delete gTraindata;
    // delete trdata;
    // delete flowConstruct;
    delete disDiff;
}