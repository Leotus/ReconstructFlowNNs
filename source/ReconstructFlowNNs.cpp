// ReconstructFlowNNs.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include <iostream>


#include <vtkXMLPolyDataWriter.h>
#include <vtkPolyLine.h>
#include <vtkCellArray.h>


#include "OpenFiles.h"
#include "MeshContainPoints.h"
#include "GenerateTrainData.h"
#include "ValidateData.h"
#include "FlowConstruct.h"




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




/*----------------------------------------vtk poly data 测试---------------------------------------------------------*/
    vtkPointData* ptdat = streamlinedats->GetPointData();
    vtkDataArray* xdat = ptdat->GetArray("x");// x
    vtkDataArray* ydat = ptdat->GetArray("y");// y
    vtkDataArray* zdat = ptdat->GetArray("z");// z
    vtkPolyData* newpoly = vtkPolyData::New();
    vtkCellArray* lines = vtkCellArray::New();

    vtkPoints* newp = vtkPoints::New();
    vtkPolyLine* line1 = vtkPolyLine::New();
    vtkIdList* list1 = line1->GetPointIds();
    newp->SetNumberOfPoints(60);
    for (int i = 0; i < 30; i++) {
        newp->SetPoint(i, xdat->GetComponent(i, 0), ydat->GetComponent(i, 0), zdat->GetComponent(i, 0));
        list1->InsertNextId(i);
    }

    vtkPolyLine* line2 = vtkPolyLine::New();
    vtkIdList* list2 = line2->GetPointIds();
    for (int i = 100; i < 130; i++) {
        int j = i - 100 + 30;
        newp->SetPoint(j, xdat->GetComponent(i, 0), ydat->GetComponent(i, 0), zdat->GetComponent(i, 0));
        list2->InsertNextId(j);
    }

    lines->InsertNextCell(list1);
    lines->InsertNextCell(list2);

    newpoly->SetLines(lines);
    newpoly->SetPoints(newp);

    vtkXMLPolyDataWriter* polyWriter = vtkXMLPolyDataWriter::New();
    polyWriter -> SetFileName("C:\\Users\\ll\\Desktop\\models\\test.vtp");
   
    polyWriter->SetInputData(newpoly);
    polyWriter->Write();
    
/*----------------------------------------vtk poly data 测试---------------------------------------------------------*/




    int* dims = oridats->GetDimensions();
    double* bounds = oridats->GetBounds();
    
    MeshContains* meshContains = new MeshContains(dims,bounds);
    meshContains->GenerateMeshContainsPoints(streamlinedats);

    /* 通过流线生成训练数据 */
    //GenerateTrainData* gTraindata = new GenerateTrainData(dims,bounds, GROUP_NUM);
    //gTraindata->generateData(meshContains->GetMeshPoints(),oridats);
    //vector<vector<map<string, double>*>*>* trdata = gTraindata->getTrainData();
    //ValidateData::validateTrainData(trdata, streamlinedats,GROUP_NUM); // 验证训练数据
    
    /*  通过训练好的模型预测流场 */
    FlowConstruct* flowConstruct = new FlowConstruct(dims, bounds, GROUP_NUM);
    flowConstruct->constructData(meshContains->GetMeshPoints(), oridats);
    vtkStructuredGrid* newFlowData = flowConstruct->getFlowData();

    /* todo : 对比流线及新流场数据，检验结果 */
    
}