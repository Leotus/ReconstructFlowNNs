#include "OpenFiles.h"

void FileOpenClass::OpenFile(string fs)
{
	char ch;
	int cnt = 0, bgn;
	string VTK("vtk");
	string PLT("vts");
	string CAS("cas");
	string VTP("vtp");
	ifstream inFile(fs);
	ostringstream buf;
	while (buf && inFile.get(ch))
	{
		buf.put(ch);
	}
	string FileName = buf.str();
	bgn = FileName.find_last_of('.');
	string sub_str = FileName.substr(bgn + 1, FileName.size() - bgn - 1);
	if (sub_str == CAS)
	{
		OpenCAS(FileName);
	}
	if (sub_str == VTK)
	{
		OpenVTK(FileName);
	}
	if (sub_str == PLT)
	{
		OpenPLT(FileName);
	}
	if (sub_str == VTP)
	{
		OpenVTP(FileName);
	}
}

void FileOpenClass::OpenVTK(string fileName)
{
	sgReader->SetFileName(fileName.c_str());
	sgReader->Update();
	gridDatas = sgReader->GetOutput();
}

void FileOpenClass::OpenPLT(string fileName)
{
	XMLsgReader->SetFileName(fileName.c_str());
	XMLsgReader->Update();
	gridDatas = XMLsgReader->GetOutput();
}

void FileOpenClass::OpenCAS(string fileName)
{

}

void FileOpenClass::OpenVTP(string fileName)
{
	polydReader->SetFileName(fileName.c_str());
	polydReader->Update();
	streamline = polydReader->GetOutput();
}

FileOpenClass::FileOpenClass()
{
	sgReader = vtkStructuredGridReader::New();
	XMLsgReader = vtkXMLStructuredGridReader::New();
	polydReader = vtkXMLPolyDataReader::New();
}

FileOpenClass::~FileOpenClass()
{
	sgReader->Delete();
	XMLsgReader->Delete();
	polydReader->Delete();
}

vtkStructuredGrid* FileOpenClass::GetGridData()
{
	return gridDatas;
}

vtkPolyData* FileOpenClass::GetStreamlineData()
{
	return streamline;
}